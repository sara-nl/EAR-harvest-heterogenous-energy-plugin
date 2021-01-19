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
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <library/policies/policy_api.h>

static uint last_pc=0;

#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif
static ulong *ext_gpu_def_freq;
static ulong g_freq=0;



state_t policy_init(polctx_t *c)
{
	char *gpu_freq=getenv(SCHED_EAR_GPU_DEF_FREQ);
	int i;
	state_t ret;
	gpu_lib_alloc_array(&ext_gpu_def_freq);
	if ((gpu_freq!=NULL) && (c->app->user_type==AUTHORIZED)){ 
		g_freq=atol(gpu_freq);
		for (i=0;i<c->num_gpus;i++) ext_gpu_def_freq[i]=g_freq;
	}else{ 
		gpu_lib_freq_limit_get_current(&c->gpu_mgt_ctx, ext_gpu_def_freq);
	}
	for (i=0;i<c->num_gpus;i++){
		verbose(1,"GPU_monitoring: GPU %d initialized with %lu",i,ext_gpu_def_freq[i]);
	}
	ret = gpu_lib_freq_limit_set(ext_gpu_def_freq);
	return EAR_SUCCESS;
}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	ulong eff_f,f;
	uint i;
	debug("apply: num_gpus %d",my_sig->gpu_sig.num_gpus);
	for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
		debug("[GPU %d Power %.2lf Freq %.2f Mem_freq %.2f Util %lu Mem_util %lu]",i,
		my_sig->gpu_sig.gpu_data[i].GPU_power,(float)my_sig->gpu_sig.gpu_data[i].GPU_freq/1000.0,(float)my_sig->gpu_sig.gpu_data[i].GPU_mem_freq/1000.0,
		my_sig->gpu_sig.gpu_data[i].GPU_util,my_sig->gpu_sig.gpu_data[i].GPU_mem_util);
	}
	
	*ready=1;
	memcpy(new_freq,ext_gpu_def_freq,sizeof(ulong)*c->num_gpus);
	
	return EAR_SUCCESS;
}
state_t policy_ok(polctx_t *c, signature_t *my_sig,signature_t *prev_sig,int *ok)
{
	ulong eff_f;
	uint i;
	debug("num_gpus %d",my_sig->gpu_sig.num_gpus);
	for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
		debug("[GPU %d Power %.2lf Freq %.2f Mem_freq %.2f Util %lu Mem_util %lu]",i,
		my_sig->gpu_sig.gpu_data[i].GPU_power,(float)my_sig->gpu_sig.gpu_data[i].GPU_freq/1000.0,(float)my_sig->gpu_sig.gpu_data[i].GPU_mem_freq/1000.0,
		my_sig->gpu_sig.gpu_data[i].GPU_util,my_sig->gpu_sig.gpu_data[i].GPU_mem_util);
	}
	*ok=1;

	return EAR_SUCCESS;
}


state_t policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
	return EAR_SUCCESS;
}

state_t policy_max_tries(polctx_t *c,int *intents)
{
	*intents=0;
	return EAR_SUCCESS;
}


