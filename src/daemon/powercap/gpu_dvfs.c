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

static uint gpu_dvfs_pc_enabled=0;
static ulong c_req_f;

/* This  subscription will take care automatically of the power monitoring */
static uint current_gpu_pc=0,set_gpu_pc=0;
static uint gpu_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;

static gpu_t *values_gpu_init,*values_gpu_end,*values_gpu_diff;

static ctx_t gpu_pc_ctx,gpu_metric_ctx;
static uint gpu_pc_num_gpus;
static uint *gpu_pc_min_power;
static uint *gpu_pc_max_power;
/* These two variables controls the power ditribution between GPUS */
static uint *gpu_pc_curr_power;
static uint *gpu_pc_util;
static float *pdist;
static uint *c_freq,*t_freq,*n_freq;


void mgt_gpu_power_limit_get_min(ctx_t *c,uint *minv)
{
	int i;
	for (i=0;i<gpu_pc_num_gpus;i++) minv[i]=125;
}

/************************ This function is called by the monitor before the iterative part ************************/
state_t gpu_dvfs_pc_thread_init(void *p)
{
  state_t s;

	if (gpu_init(&gpu_metric_ctx) != EAR_SUCCESS){
		debug("Error at gpu initializatin");
		gpu_dvfs_pc_enabled = 0;
		return EAR_ERROR;
	}
	gpu_data_alloc(&values_gpu_init);
	gpu_data_alloc(&values_gpu_end);
	gpu_data_alloc(&values_gpu_diff);
	c_freq=calloc(gpu_pc_num_gpus,sizeof(uint));
	t_freq=calloc(gpu_pc_num_gpus,sizeof(uint));
	n_freq=calloc(gpu_pc_num_gpus,sizeof(uint));
  gpu_dvfs_pc_enabled=1;
  verbose(1,"Power measurement initialized in dvfs_pc thread initialization");
	gpu_read(&gpu_metric_ctx,values_gpu_init);
	return EAR_SUCCESS;
}
/************************ This function is called by the monitor in iterative part ************************/
state_t gpu_dvfs_pc_thread_main(void *p)
{
  	float power_gpu;
		char gpu_str[512];
		unsigned long long acum_energy;
  	uint c_pstate,t_pstate;
		uint extra;

		gpu_read(&gpu_metric_ctx,values_gpu_end);
    /* Calcular power */
		gpu_data_diff(values_gpu_end,values_gpu_init,values_gpu_diff);
		gpu_data_tostr(values_gpu_diff,gpu_str,sizeof(gpu_str));
    /* debug(gpu_str); */
		mgt_gpu_clock_limit_get_current(&gpu_pc_ctx,c_freq);
		for (i=0;i<gpu_pc_num_gpus;i++){
    if (c_status==PC_STATUS_RUN){
      /* Aplicar limites */
      if ((current_gpu_pc>0)  && (c_status==PC_STATUS_RUN)){
      if (values_gpu_diff[i].power_w > gpu_pc_curr_power[i]){ /* We are above the PC */
				/* Use the list of freuencies */
				n_freq[i] = select_lower_gpu_freq(c_freq[i]);
      }else{ /* We are below the PC */
        if (c_freq[i] < t_freq[i]){
          extra=compute_extra_gpu_power(c_freq[i],t_freq[i]);
          if (((values_gpu_diff[i].power_w+extra)< gpu_pc_curr_power[i]) && (c_mode==PC_MODE_TARGET)){
						n_freq[i] = select_higher_gpu_freq(c_freq[i]);
          }
        }
      }
    } /* (current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN) */
    } /* c_status==PC_STATUS_RUN */
		}
		mgt_gpu_clock_limit_set(&gpu_metric_ctx,n_freq[i]);
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
	sus->suscribe(sus);
	debug("GPU-DVFS Subscription done");
	/* Init data */
	debug("GPU_DVFS: power cap  enable");
	mgt_gpu_load(NULL);
	if ((ret = mgt_gpu_init(&gpu_pc_ctx)) != EAR_SUCCESS){
		debug("Error in mgt_gpu_init");
	}else{
		gpu_pc_enabled = 1;
		mgt_gpu_count(&gpu_pc_ctx,&gpu_pc_num_gpus);
		debug("%d GPUS detectd in gpu_powercap",gpu_pc_num_gpus);
		gpu_pc_min_power = calloc(gpu_pc_num_gpus,sizeof(uint));
		if (gpu_pc_min_power == NULL){
			gpu_pc_enabled = 0;
			ret = EAR_ERROR;
			debug("Error allocating memory in GPU enable");
		}
    gpu_pc_max_power = calloc(gpu_pc_num_gpus,sizeof(uint));
    if (gpu_pc_max_power == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
    }
    gpu_pc_curr_power = calloc(gpu_pc_num_gpus,sizeof(uint));
    if (gpu_pc_curr_power == NULL){
      gpu_pc_enabled = 0;
      ret = EAR_ERROR;
      debug("Error allocating memory in GPU enable");
    }
    gpu_pc_util = calloc(gpu_pc_num_gpus,sizeof(uint));
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
		mgt_gpu_power_limit_get_max(&gpu_pc_ctx,gpu_pc_max_power);
		mgt_gpu_power_limit_get_min(&gpu_pc_ctx,gpu_pc_min_power);
	}
	return ret;

	return EAR_SUCCESS;
}
static state_t int_set_powercap_value(uint limit,uint *gpu_util);
state_t set_powercap_value(uint pid,uint domain,uint limit,uint *gpu_util)
{
	return int_set_powercap_value(limit,gpu_util);
}

static state_t int_set_powercap_value(uint limit,uint *gpu_util)
{
	char cmd[256];
	state_t ret;
	int i;
	float alloc,ualloc;
	uint gpu_idle=0,gpu_run=0,total_util=0;
	/* Set data */
	debug("%s",COL_BLU);
	debug("GPU: set_powercap_value %u",limit);
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
			ualloc = (uint)alloc;
			if (ualloc > gpu_pc_max_power[i]) ualloc = gpu_pc_max_power[i];
			if (ualloc < gpu_pc_min_power[i]) ualloc = gpu_pc_min_power[i];
			gpu_pc_curr_power[i] = ualloc;
		}
	}
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%u power_alloc=%u",i,gpu_util[i],gpu_pc_curr_power[i]);
  }
	memcpy(gpu_pc_util,gpu_util,sizeof(uint)*gpu_pc_num_gpus);
	current_gpu_pc=limit;
	debug("%s",COL_CLR);
	return EAR_SUCCESS;
}

state_t get_powercap_value(uint pid,uint *powercap)
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
	dprintf(fd,"gpu_powercap_value %u\n",current_gpu_pc);
}
void powercap_to_str(char *b)
{
	sprintf(b,"%u",current_gpu_pc);
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

void set_new_utilization(uint *util)
{
	int i;
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%u",i,util[i]);
  }
	int_set_powercap_value(current_gpu_pc,util);

}


