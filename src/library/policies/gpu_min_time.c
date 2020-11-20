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
#include <library/policies/policy_api.h>

static uint last_pc=0;

#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif

#if 0
#define debug(...) \
{ \
        dprintf(2, __VA_ARGS__); \
        dprintf(2, "\n"); \
}
#endif

#define LOW_UTIL_GPU_TH 25
#define MED_UTIL_GPU_TH 50
#define HIGH_UTIL_GPU_TH 75
#define LOW_GPU_FREQ_RED ((100 - (LOW_UTIL_GPU_TH + 10 ))/100.0)
#define MED_GPU_FREQ_RED ((100 - (MED_UTIL_GPU_TH + 10))/100.0)
#define HIGH_GPU_FREQ_RED ((100 - (HIGH_UTIL_GPU_TH + 10))/100.0)
#define LOW_GPU_FREQ 759
#define MED_GPU_FREQ 949
#define HIGH_GPU_FREQ 1088
#if 0
#define LOW_GPU_FREQ 544
#define MED_GPU_FREQ 544
#define HIGH_GPU_FREQ 632
#endif



state_t policy_init(polctx_t *c)
{
	return EAR_SUCCESS;
}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	ulong eff_f,f;
	uint i;
	ulong util,freq;
	debug("apply: num_gpus %d",my_sig->gpu_sig.num_gpus);
	for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
		debug("[GPU %d Power %.2lf Freq %.2f Mem_freq %.2f Util %lu Mem_util %lu]",i,
		my_sig->gpu_sig.gpu_data[i].GPU_power,(float)my_sig->gpu_sig.gpu_data[i].GPU_freq/1000.0,(float)my_sig->gpu_sig.gpu_data[i].GPU_mem_freq/1000.0,
		my_sig->gpu_sig.gpu_data[i].GPU_util,my_sig->gpu_sig.gpu_data[i].GPU_mem_util);
	}

	for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
		util = my_sig->gpu_sig.gpu_data[i].GPU_util;
		freq = my_sig->gpu_sig.gpu_data[i].GPU_freq;
		if (util > 0){		
			if (util < LOW_UTIL_GPU_TH){
				//new_freq[i] = freq - (freq * LOW_GPU_FREQ_RED);
				new_freq[i] = LOW_GPU_FREQ;
			}else if (util < MED_UTIL_GPU_TH){
				//new_freq[i] = freq - (freq * MED_GPU_FREQ_RED);
				new_freq[i] = MED_GPU_FREQ;
			}else if (util < HIGH_UTIL_GPU_TH ){
				//new_freq[i] = freq - (freq * HIGH_GPU_FREQ_RED);
				new_freq[i] = HIGH_GPU_FREQ;
			}else{		
				new_freq[i] = freq ;
			}
		}else{
			new_freq[i]=freq;
		}
			debug("Setting gpu_freq[%d]=%lu",i,new_freq[i]);
	}		
	
	*ready=1;
	
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


