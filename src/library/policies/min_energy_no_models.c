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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/projection.h>
#include <control/frequency.h>
#include <daemon/eard_api.h>
#include <library/policies/policy_api.h>
#include <common/math_operations.h>

typedef unsigned long ulong;
static signature_t *sig_list;
static unsigned int *sig_ready;

static void go_next_me(int curr_pstate,int *ready,ulong *best_freq,unsigned long num_pstates)
{
	int next_pstate;
	if ((curr_pstate<(num_pstates-1))){
		*ready=0;
		next_pstate=curr_pstate+1;
		*best_freq=frequency_pstate_to_freq(next_pstate);
	}else{
		*ready=1;
		*best_freq=frequency_pstate_to_freq(curr_pstate);
	}
}

static int is_better_min_energy(signature_t * curr_sig,signature_t *prev_sig,double time_max)
{
	double curr_energy,pre_energy;
	curr_energy=curr_sig->time*curr_sig->DC_power;
	pre_energy=prev_sig->time*prev_sig->DC_power;
	if (curr_energy>pre_energy){ 
			return 0;
	}
	if (curr_sig->time<time_max) return 1;
	return 0;
}


state_t policy_init(polctx_t *c)
{
	if (c!=NULL){
		sig_list=(signature_t *)malloc(sizeof(signature_t)*c->num_pstates);
		sig_ready=(unsigned int *)malloc(sizeof(unsigned int)*c->num_pstates);
		if ((sig_list==NULL) || (sig_ready==NULL)) return EAR_ERROR;
		memset(sig_list,0,sizeof(signature_t)*c->num_pstates);
		memset(sig_ready,0,sizeof(unsigned int)*c->num_pstates);
		return EAR_SUCCESS;
	}else return EAR_ERROR;
}


state_t policy_loop_init(polctx_t *c,loop_id_t *loop_id)
{
		if (c!=NULL){ 
			projection_reset(c->num_pstates);
			memset(sig_ready,0,sizeof(unsigned int)*c->num_pstates);
			return EAR_SUCCESS;
		}else{
			return EAR_ERROR;
		}
		
}

state_t policy_loop_end(polctx_t *c,loop_id_t *loop_id)
{
    if ((c!=NULL) && (c->reset_freq_opt))
    {
        // Use configuration when available
        *(c->ear_frequency) = eards_change_freq(c->app->def_freq);
    }
	return EAR_SUCCESS;
}


// This is the main function in this file, it implements power policy
state_t policy_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
    signature_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next;
    double power_proj,time_proj;
		double max_penalty,time_max;
    double power_ref,time_ref,time_current;
    ulong best_freq,best_pstate,freq_ref;
		ulong curr_freq;
		ulong curr_pstate,def_pstate,def_freq,prev_pstate;
		state_t st;
    my_app=sig;

		*ready=0;

		if (c==NULL) return EAR_ERROR;
		if (c->app==NULL) return EAR_ERROR;

    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_freq_to_pstate(c->app->max_freq);

    // This is the frequency at which we were running
	curr_freq=*(c->ear_frequency);
    curr_pstate = frequency_freq_to_pstate(curr_freq);

		// New signature ready
		sig_ready[curr_pstate]=1;
		signature_copy(&sig_list[curr_pstate], sig);
	

		// Default values
		
		max_penalty=c->app->settings[0];
		def_freq=c->app->def_freq;
		def_pstate=frequency_freq_to_pstate(def_freq);


	// ref=1 is nominal 0=turbo, we are not using it

	signature_t *prev_sig;
	/* We must not use models , we will check one by one*/
	/* If we are not running at default freq, we must check if we must follow */
	if (sig_ready[def_pstate]==0){
		*ready=0;
		*new_freq=def_freq;
	} else{
		time_ref = sig_list[curr_pstate].time;
		time_max = time_ref + (time_ref * max_penalty);
		/* This is the normal case */
		if (curr_pstate != def_pstate){
			prev_pstate=curr_pstate-1;
			prev_sig=&sig_list[prev_pstate];
			if (is_better_min_energy(my_app,prev_sig,time_max)){
				go_next_me(curr_pstate,ready,new_freq,c->num_pstates);
			}else{
				*ready=1;
				*new_freq=frequency_pstate_to_freq(prev_pstate);
			}
		}else{
			go_next_me(curr_pstate,ready,new_freq,c->num_pstates);
		}
	}

	return EAR_SUCCESS;
}


state_t policy_ok(polctx_t *c,signature_t *curr_sig,signature_t *last_sig,int *ok)
{

	state_t st=EAR_SUCCESS;

	if ((c==NULL) || (curr_sig==NULL) || (last_sig==NULL)) return EAR_ERROR;
	
	if (curr_sig->def_f==last_sig->def_f) *ok=1;

	// Check that efficiency is enough
	if (curr_sig->time < last_sig->time) *ok=1;

	// If time is similar and it was same freq it is ok	
	if (equal_with_th(curr_sig->time , last_sig->time,0.1) && (curr_sig->def_f == last_sig->def_f)) *ok=1;
	
	// If we run at a higher freq but we are slower, we are not ok	
	if ((curr_sig->time > last_sig->time) && (curr_sig->def_f > last_sig->def_f)) *ok=0;

	else *ok=0;

	return st;

}
state_t  policy_get_default_freq(polctx_t *c,ulong *f)
{
		if ((c!=NULL) && (c->app!=NULL)){
    // Just in case the bestPstate was the frequency at which the application was running
        *f=c->app->def_freq;
    }
		return EAR_SUCCESS;
}
	
state_t policy_max_tries(polctx_t *c,int *intents)
{
  *intents=2;
  return EAR_SUCCESS;
}

