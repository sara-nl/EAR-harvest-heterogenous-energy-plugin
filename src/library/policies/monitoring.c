/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/hardware/frequency.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <daemon/powercap_status.h>


#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif



state_t policy_init(polctx_t *c)
{
	return EAR_SUCCESS;
}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	*ready=1;
	*new_freq=DEF_FREQ(c->app->def_freq);
	printf("policy_apply %d\n",is_powercap_set(&c->app->pc_opt));
#if POWERCAP
	if (is_powercap_set(&c->app->pc_opt)){ 
		verbose(1,"Powercap is set to %uWatts",get_powercapopt_value(&c->app->pc_opt));
	}else{
		verbose(1,"Powercap is NOT set ");
	}
#else
	verbose(1,"Powercap not defined");
#endif
	
	return EAR_SUCCESS;
}
state_t policy_ok(polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok)
{
	ulong eff_f;
	uint power_status;
	*ok=1;
#if POWERCAP
    if (is_powercap_set(&c->app->pc_opt)){
      power_status=compute_power_status(&c->app->pc_opt,(uint)(curr_sig->DC_power));
      eff_f=frequency_closest_high_freq(curr_sig->avg_f,1);
      verbose(1,"Powercap is set to %u Watts, status %u effective freq %lu",get_powercapopt_value(&c->app->pc_opt),power_status,eff_f);
      if (eff_f<curr_sig->def_f){
        verbose(1,"Running with powercap, status %u and effective freq %lu vs selected %lu",power_status,eff_f,curr_sig->def_f);
      }
    }else{
      verbose(1,"Powercap is not set");
      power_status=PC_STATUS_ERROR;
    }
#endif

	return EAR_SUCCESS;
}


state_t policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
	*freq_set=DEF_FREQ(c->app->def_freq);
	return EAR_SUCCESS;
}

state_t policy_max_tries(polctx_t *c,int *intents)
{
	*intents=0;
	return EAR_SUCCESS;
}


