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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <common/config.h>
#include <signal.h>
#include <common/colors.h>
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/execute.h>
#include <daemon/powercap/powercap_status_conf.h>
#include <management/gpu/gpu.h>
#include <common/system/monitor.h>




static uint current_gpu_pc=0,set_gpu_pc=0;
static uint gpu_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;

static ctx_t gpu_pc_ctx;
static uint gpu_pc_num_gpus;
static ulong *gpu_pc_min_power;
static ulong *gpu_pc_max_power;
static ulong *gpu_pc_curr_power;
static ulong *gpu_pc_util;
static float *pdist;

#define MIN_GPU_IDLE_POWER 30

state_t disable()
{
	gpu_pc_enabled = 0;
	return EAR_SUCCESS;
}


state_t enable(suscription_t *sus)
{
	state_t ret=EAR_SUCCESS;
	char cmd[256];
	debug("GPU: power cap  enable");
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
		mgt_gpu_power_cap_get_rank(&gpu_pc_ctx,gpu_pc_min_power,gpu_pc_max_power);
	}
	return ret;
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
	float alloc,ualloc;
	ulong gpu_idle=0,gpu_run=0,total_util=0;
	/* Set data */
	debug("%s",COL_BLU);
	debug("GPU: set_powercap_value %lu",limit);
	debug("GPU: Phase 1, allocating power to idle GPUS");
	for (i=0;i<gpu_pc_num_gpus;i++){	
		pdist[i]=0.0;
		total_util+=gpu_util[i];
		if (gpu_util[i]==0){ 
			gpu_idle++;
			gpu_pc_curr_power[i]=gpu_pc_min_power[i];
			limit=limit - MIN_GPU_IDLE_POWER;
		}else{
			gpu_run++;
		}
	}
	debug("GPU: Phase 2: Allocating power to %lu running GPUS",gpu_run);
	for (i=0;i<gpu_pc_num_gpus;i++){
		if (gpu_util[i]>0){
			pdist[i]=(float)gpu_util[i]/(float)total_util;
			alloc=(float)limit*pdist[i];
			ualloc = alloc;
			if (ualloc > gpu_pc_max_power[i]) ualloc = gpu_pc_max_power[i];
			if (ualloc < gpu_pc_min_power[i]) ualloc = gpu_pc_min_power[i];
			gpu_pc_curr_power[i] = ualloc;
		}
	}
	ret = mgt_gpu_power_cap_set(&gpu_pc_ctx,gpu_pc_curr_power);
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%lu power_alloc=%lu",i,gpu_util[i],gpu_pc_curr_power[i]);
  }
	memcpy(gpu_pc_util,gpu_util,sizeof(ulong)*gpu_pc_num_gpus);
	current_gpu_pc=limit;
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

void set_new_utilization(ulong *util)
{
	int i;
  for (i=0;i<gpu_pc_num_gpus;i++) {
    debug("GPU: util_gpu[%d]=%lu",i,util[i]);
  }
	int_set_powercap_value(current_gpu_pc,util);

}
