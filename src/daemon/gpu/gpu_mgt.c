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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/states.h>

#include <management/gpu/gpu.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/output/debug.h>

static ctx_t gpu_node_mgr;
static uint num_dev;
static uint *def_khz,*max_khz,*current_khz;
static uint *def_w,*max_w,*current_w;
state_t gpu_mgr_init()
{
	uint i;
	state_t ret;
	ret=mgt_gpu_load(NULL);
	if (ret!=EAR_SUCCESS){
		debug("Error in mgt_gpu_load");
		return ret;
	}
	ret=mgt_gpu_init(&gpu_node_mgr);
	if (ret!=EAR_SUCCESS){
		debug("Error in mgt_gpu_init");
		return ret;
	}
	ret=mgt_gpu_count(&gpu_node_mgr,&num_dev);
	if (ret!=EAR_SUCCESS){
		debug("Error in mgt_gpu_count");
		return ret;
	}
	debug("%u gpus in node",num_dev);
	def_khz=calloc(num_dev,sizeof(uint));
	max_khz=calloc(num_dev,sizeof(uint));
	current_khz=calloc(num_dev,sizeof(uint));
	def_w=calloc(num_dev,sizeof(uint));
	max_w=calloc(num_dev,sizeof(uint));
	current_w=calloc(num_dev,sizeof(uint));
	if ((def_khz == NULL) || (max_khz == NULL) || (current_khz == NULL)){
		debug("Memory allocation for GPU freq limits returns NULL");
		return EAR_ALLOC_ERROR;
	}
	if ((def_w == NULL) || (max_w == NULL) || (current_w == NULL)){
		debug("Memory allocation for GPU power limits returns NULL");
		return EAR_ALLOC_ERROR;
	}
	/* GPU frequency limits */
	ret=mgt_gpu_clock_limit_get_default(&gpu_node_mgr,def_khz);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_clock_limit_get_default");
		return ret;
	}
	ret=mgt_gpu_clock_limit_get_max(&gpu_node_mgr,max_khz);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_clock_limit_get_max");
		return ret;
	}
	ret=mgt_gpu_clock_limit_get_current(&gpu_node_mgr,current_khz);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_clock_limit_get_current");
		return ret;
	}
	/* GPU power limits */
	ret=mgt_gpu_power_limit_get_default(&gpu_node_mgr,def_w);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_power_limit_get_default");
		return ret;
	}
	ret=mgt_gpu_power_limit_get_max(&gpu_node_mgr,max_w);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_power_limit_get_max");
		return ret;
	}
	ret=mgt_gpu_power_limit_get_current(&gpu_node_mgr,current_w);
	if (ret!=EAR_SUCCESS){
		debug("mgt_gpu_power_limit_get_current");
		return ret;
	}
	for (i=0;i<num_dev;i++){
		debug("GPU %u freq limits: def %u max %u current %u",i,def_khz[i],max_khz[i],current_khz[i]);
		debug("GPU %u power limits:def %u max %u current %u",i,def_w[i],max_w[i],current_w[i]);
	}
	return EAR_SUCCESS;
}



