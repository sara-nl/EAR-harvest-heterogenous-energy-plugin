/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <common/config.h>
#include <common/colors.h>
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <daemon/powercap/powercap_status_conf.h>
#include <daemon/powercap/powercap_status.h>
#include <metrics/gpu/gpu.h>
#include <management/gpu/gpu.h>
#include <common/system/monitor.h>


#define DEBUG_PERIOD 15
#define MIN_GPU_IDLE_POWER 30

static uint gpu_dvfs_pc_enabled=0;
static ulong c_req_f;

/* This  subscription will take care automatically of the power monitoring */
static ulong current_gpu_pc=0,set_gpu_pc=0;
static uint gpu_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;

static gpu_t *values_gpu_init,*values_gpu_end,*values_gpu_diff;

static ctx_t gpu_pc_ctx,gpu_metric_ctx;
static uint gpu_pc_num_gpus;
static ulong *gpu_pc_min_power;
static ulong *gpu_pc_max_power;
/* These two variables controls the power ditribution between GPUS */
static ulong *gpu_pc_curr_power;
static ulong *gpu_pc_util;
static float *pdist;
static ulong *c_freq,*t_freq,*n_freq;
static ulong **gpu_freq_list=NULL;
static uint *gpu_num_freqs=NULL;
static gpu_ops_t     *ops_met;

ulong select_lower_gpu_freq(uint i,ulong f)
{
	int j=0,found=0;
	/* If we are the minimum*/
	// debug("Looking for lower freq for %lu",f);
	if (f == gpu_freq_list[i][gpu_num_freqs[i]-1]) return f;
	/* Otherwise, look for f*/
	do{
		if (gpu_freq_list[i][j] == f) found = 1;
		else j++;
	}while((found==0) && (j!=gpu_num_freqs[i]));
	if (!found){
		// debug("Frequency %lu not found in GPU %u",f,i);
		return gpu_freq_list[i][gpu_num_freqs[i]-1];
	}else{
		// debug("Frequency %lu found in position %d",f,j);
	}
	if (j < (gpu_num_freqs[i]-1)){
		return gpu_freq_list[i][j+1];
	}
	return f;
}

ulong select_higher_gpu_freq(uint i,ulong f)
{
  int j=0,found=0;
	// debug("Looking for higher freq for %lu",f);
	/* If we are the maximum*/
	if (f == gpu_freq_list[i][0]) return f;
	/* Otherwise, look for f*/
  do{
    if (gpu_freq_list[i][j] == f) found = 1;
    else j++;
  }while((found==0) && (j!=gpu_num_freqs[i]));
  if (!found){
    // debug("Frequency %lu not found in GPU %u",f,i);
    return gpu_freq_list[i][0];
  }else{
    // debug("Frequency %lu found in position %d",f,j);
  }
  return gpu_freq_list[i][j-1];
}

static void printf_gpu_freq_list(ulong **f,uint *num_f)
{
	int i,j;
#if SHOW_DEBUGS
	for (i=0;i<gpu_pc_num_gpus;i++){
		debug("GPU[%d]------------- limit %u",i,num_f[i]);
		for (j=0;j<num_f[i];j++){
			debug("Freq[%d]=%lu",j,f[i][j]);
		}
	}
#endif
}

/************************ This function is called by the monitor before the iterative part ************************/
state_t gpu_dvfs_pc_thread_init(void *p)
{
  state_t s;
	if (gpu_load(&ops_met,0,NULL)!=EAR_SUCCESS){
		debug("Error at gpu load");
		gpu_dvfs_pc_enabled = 0;
    return EAR_ERROR;
	}
	if (gpu_init(&gpu_metric_ctx) != EAR_SUCCESS){
		debug("Error at gpu initialization");
		gpu_dvfs_pc_enabled = 0;
		return EAR_ERROR;
	}
	gpu_data_alloc(&values_gpu_init);
	gpu_data_alloc(&values_gpu_end);
	gpu_data_alloc(&values_gpu_diff);
	mgt_gpu_freq_list(&gpu_pc_ctx, &gpu_freq_list, &gpu_num_freqs);
	printf_gpu_freq_list(gpu_freq_list,gpu_num_freqs);
  gpu_dvfs_pc_enabled=1;
  debug("Power measurement initialized in gpu_dvfs_pc thread initialization");
	if (gpu_read(&gpu_metric_ctx,values_gpu_init)!= EAR_SUCCESS){
		debug("Error in gpu_read in gpu_dvfs_pc");
		gpu_dvfs_pc_enabled=0;
	}
	return EAR_SUCCESS;
}
/************************ This function is called by the monitor in iterative part ************************/
state_t gpu_dvfs_pc_thread_main(void *p)
{
  	float power_gpu;
		char gpu_str[512];
		ulong extra;
		uint i;

		if (!gpu_dvfs_pc_enabled) return EAR_SUCCESS;
		if (gpu_read(&gpu_metric_ctx,values_gpu_end)!=EAR_SUCCESS){
    	debug("Error in gpu_read gpu_dvfs_pc");
			return EAR_ERROR;
		}
    /* Calcular power */
		gpu_data_diff(values_gpu_end,values_gpu_init,values_gpu_diff);
		mgt_gpu_freq_limit_get_current(&gpu_pc_ctx,c_freq);
	#if 0
		for (i=0;i<gpu_pc_num_gpus;i++){
				debug("GPU_DVFS[%u] current_f=%lu powercap %lu consumption %.1lf",i,c_freq[i],gpu_pc_curr_power[i], values_gpu_diff[i].power_w);
		}
	#endif
		for (i=0;i<gpu_pc_num_gpus;i++){
		n_freq[i] = c_freq[i];
    if (c_status==PC_STATUS_RUN){
      /* Aplicar limites */
      if ((current_gpu_pc>0)  && (c_status==PC_STATUS_RUN)){
      if (values_gpu_diff[i].power_w > gpu_pc_curr_power[i]){ /* We are above the PC */
				/* Use the list of freuencies */
				n_freq[i] = select_lower_gpu_freq(i,c_freq[i]);
				verbose(0,"%sReducing the GPU-freq[%d] from %lu to %lu (cpower %lf limit %lu) %s",COL_RED,i,c_freq[i],n_freq[i],values_gpu_diff[i].power_w,gpu_pc_curr_power[i],COL_CLR);
      }else{ /* We are below the PC */
        if (c_freq[i] < t_freq[i]){
          extra=compute_extra_gpu_power(values_gpu_diff[i].power_w,c_freq[i],t_freq[i]);
          if (((values_gpu_diff[i].power_w+extra)< gpu_pc_curr_power[i]) && (c_mode==PC_MODE_TARGET)){
						n_freq[i] = select_higher_gpu_freq(i,c_freq[i]);
						verbose(0,"%sIncreasing the GPU-freq[%d] from %lu to %lu, estimated %lf=(%lf+%lu), limit %lu %s",COL_GRE,i,c_freq[i],n_freq[i],values_gpu_diff[i].power_w+extra,values_gpu_diff[i].power_w,extra,gpu_pc_curr_power[i],COL_CLR);
          }
        }
      }
    } /* (current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN) */
    } /* c_status==PC_STATUS_RUN */
		}
		if (c_status==PC_STATUS_RUN){
			#if 0
			for (i=0;i<gpu_pc_num_gpus;i++){
				debug("GPU_DVFS[%u] selecting f=%lu",i,n_freq[i]);
			}
			#endif
			mgt_gpu_freq_limit_set(&gpu_metric_ctx,n_freq);
		}else{
			// debug("GPU-dvfs freq not modifies because idle");
		}
    /* Copiar init=end */
		gpu_data_copy(values_gpu_init,values_gpu_end);
		return EAR_SUCCESS;
}


state_t disable()
{
	gpu_pc_enabled = 0;

	return EAR_SUCCESS;
	/* Stop thread */
}

state_t enable(suscription_t *sus)
{
	state_t ret=EAR_SUCCESS;
	debug("GPU-DVFS");
	/* Suscription is for gpu power monitoring */
	if (sus == NULL){
		debug("NULL subscription in GPU-DVFS powercap");
		return EAR_ERROR;
	}
	sus->call_main = gpu_dvfs_pc_thread_main;
	sus->call_init = gpu_dvfs_pc_thread_init;
	sus->time_relax = 1000;
	sus->time_burst = 1000;
	/* Init data */
	debug("GPU_DVFS: power cap  enable");
	mgt_gpu_load(NULL);
	if ((ret = mgt_gpu_init(&gpu_pc_ctx)) != EAR_SUCCESS){
		debug("Error in mgt_gpu_init");
	}else{
		gpu_pc_enabled = 1;
		mgt_gpu_count(&gpu_pc_ctx,&gpu_pc_num_gpus);
		debug("%d GPUS detectd in gpu_powercap",gpu_pc_num_gpus);
		gpu_pc_min_power = calloc(gpu_pc_num_gpus,sizeof(ulong));
		if (gpu_pc_min_power == NULL){
			gpu_pc_enabled = 0;
			ret = EAR_ERROR;
			debug("Error allocating memory in GPU enable");
		}
    gpu_pc_max_power = calloc(gpu_pc_num_gpus,sizeof(ulong));
    if (gpu_pc_max_power == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
    }
    gpu_pc_curr_power = calloc(gpu_pc_num_gpus,sizeof(ulong));
    if (gpu_pc_curr_power == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
    }
    gpu_pc_util = calloc(gpu_pc_num_gpus,sizeof(ulong));
    if (gpu_pc_util == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
    }
		pdist = calloc(gpu_pc_num_gpus,sizeof(float));
		if (pdist == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
		}
		c_freq=calloc(gpu_pc_num_gpus,sizeof(ulong));
		t_freq=calloc(gpu_pc_num_gpus,sizeof(ulong));
		n_freq=calloc(gpu_pc_num_gpus,sizeof(ulong));
		mgt_gpu_power_cap_get_rank(&gpu_pc_ctx,gpu_pc_min_power,gpu_pc_max_power);
	}
	sus->suscribe(sus);
	debug("GPU-DVFS Subscription done");
	return ret;

	return EAR_SUCCESS;
}
static state_t int_set_powercap_value(ulong limit,ulong *gpu_util);
state_t set_powercap_value(uint pid,uint domain,ulong limit,ulong *gpu_util)
{
	return int_set_powercap_value(limit,gpu_util);
}

static state_t int_set_powercap_value(ulong limit,ulong *gpu_util)
{
	char cmd[256];
	state_t ret;
	int i;
	float alloc;
	ulong ualloc;
	uint gpu_idle=0,gpu_run=0,total_util=0;
	/* Set data */
	current_gpu_pc=limit;
	debug("%s",COL_BLU);
	debug("GPU: set_powercap_value %lu",limit);
	debug("GPU: Phase 1, allocating power to idle GPUS");
	for (i=0;i<gpu_pc_num_gpus;i++){	
		pdist[i]=0.0;
		total_util+=gpu_util[i];
		if (gpu_util[i]==0){ 
			gpu_idle++;
			gpu_pc_curr_power[i]=MIN_GPU_IDLE_POWER;
			limit=limit - MIN_GPU_IDLE_POWER;
		}else{
			gpu_run++;
		}
	}
	debug("GPU: Phase 2: Allocating power to %u running GPUS",gpu_run);
	for (i=0;i<gpu_pc_num_gpus;i++){
		if (gpu_util[i]>0){
			pdist[i]=(float)gpu_util[i]/(float)total_util;
			alloc=(float)limit*pdist[i];
			ualloc = (ulong)alloc;
			if (ualloc > gpu_pc_max_power[i]) ualloc = gpu_pc_max_power[i];
			if (ualloc < gpu_pc_min_power[i]) ualloc = gpu_pc_min_power[i];
			gpu_pc_curr_power[i] = ualloc;
		}
	}
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%lu power_alloc=%lu",i,gpu_util[i],gpu_pc_curr_power[i]);
  }
	memcpy(gpu_pc_util,gpu_util,sizeof(ulong)*gpu_pc_num_gpus);
	debug("%s",COL_CLR);
	return EAR_SUCCESS;
}

state_t get_powercap_value(uint pid,ulong *powercap)
{
	/* copy data */
	debug("GPU::get_powercap_value");
	*powercap=current_gpu_pc;
	return EAR_SUCCESS;
}

uint is_powercap_policy_enabled(uint pid)
{
	return gpu_pc_enabled;
}

void print_powercap_value(int fd)
{
	dprintf(fd,"gpu_powercap_value %lu\n",current_gpu_pc);
}
void powercap_to_str(char *b)
{
	char cmd[128];
	int i=0;
	cmd[0]='\0';
	for (i=0;i<gpu_pc_num_gpus;i++){
		sprintf(cmd,"GPU_powercap[%d]=%lu ",i,current_gpu_pc);
		strcat(b,cmd);
	}
}

void set_status(uint status)
{
	debug("GPU. set_status %u",status);
	c_status=status;
}
uint get_powercap_strategy()
{
	debug("GPU. get_powercap_strategy");
	return PC_POWER;
}

void set_pc_mode(uint mode)
{
	debug("GPU. set_pc_mode");
	c_mode=mode;
}


void set_verb_channel(int fd)
{
  WARN_SET_FD(fd);
  VERB_SET_FD(fd);
  DEBUG_SET_FD(fd);
}

void set_new_utilization(ulong *util)
{
	int i;
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%lu",i,util[i]);
  }
	int_set_powercap_value(current_gpu_pc,util);

}

void set_app_req_freq(ulong *f)
{
	int i;
	for (i=0;i<gpu_pc_num_gpus;i++) {
  	debug("GPU_DVFS:GPU %d Requested application freq set to %lu",i,f[i]);
		t_freq[i]=f[i];
	}
}

uint get_powercap_status(uint *in_target,uint *tbr)
{
	int i;
	uint used=0;
	uint g_tbr;
	/* If we are not using th GPU we can release all the power */
	for (i=0;i<gpu_pc_num_gpus;i++){
		used += (gpu_pc_util[i]>0);
	}
	if (!used){
		*in_target=1;
		if (gpu_pc_num_gpus == 0) *tbr=current_gpu_pc;
		else *tbr=(current_gpu_pc- (MIN_GPU_IDLE_POWER*gpu_pc_num_gpus));
		return 1;
	}
	/* Otherwise, if we don't know app requirements , we are not in the target */
	if (t_freq[i] == 0 ){
		return 0;
	}
	/* If we know, we must check */
	*in_target = 1;*tbr=0;
	for (i=0;i<gpu_pc_num_gpus;i++){
		if (t_freq[i] != c_freq[i]){ 
			*in_target=0;
			debug("We cannot release power from GPU %d",i);
		}else{
			g_tbr = (uint)((gpu_pc_curr_power[i] - values_gpu_diff[i].power_w) *0.75);
			*tbr = *tbr +  g_tbr;
			debug("We can release %u W from GPU %d since target = %lu current %lu",g_tbr,i,t_freq[i] ,c_freq[i]);
		}
	}
	if (*in_target){
		return 1;	
	}
	return 0;
}
