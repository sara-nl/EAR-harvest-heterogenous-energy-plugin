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
#define SHOW_DEBUGS 1
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/hardware/frequency.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <daemon/eard_api.h>
#include <daemon/powercap_status.h>

typedef unsigned long ulong;
#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define FREQ_DEF(f) (!ext_def_freq?f:ext_def_freq)
#else
#define FREQ_DEF(f) f
#endif

static ulong req_f;

state_t policy_init(polctx_t *c)
{
	if (c!=NULL) return EAR_SUCCESS;
	else return EAR_ERROR;
}

state_t policy_loop_init(polctx_t *c,loop_id_t *l)
{
	if (c!=NULL){
    projection_reset(c->num_pstates);
		return EAR_SUCCESS;
	}else{
		return EAR_ERROR;
	}
}

state_t policy_loop_end(polctx_t *c,loop_id_t *l)
{
    if ((c!=NULL) && (c->reset_freq_opt))
    {
        *(c->ear_frequency) = eards_change_freq(FREQ_DEF(c->app->def_freq));
    }
	return EAR_SUCCESS;
}


state_t policy_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
	signature_t *my_app = sig;
	int i,min_pstate;
	unsigned int ref;
	double power_proj,time_proj,energy_proj,best_solution,energy_ref;
	double power_ref,time_ref,max_penalty,time_max;

  ulong best_freq,best_pstate,freq_ref,eff_f;
	ulong curr_freq,nominal;
	ulong curr_pstate,def_pstate,def_freq;
	state_t st;
	uint power_status;


	if ((c!=NULL) && (c->app!=NULL)){



    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_closest_pstate(c->app->max_freq);

		nominal=frequency_pstate_to_freq(min_pstate);


	// Default values

		max_penalty=c->app->settings[0];
		def_freq=FREQ_DEF(c->app->def_freq);
		def_pstate=frequency_closest_pstate(def_freq);

		// This is the frequency at which we were running
    #ifdef POWERCAP
    curr_freq=frequency_closest_high_freq(my_app->avg_f,1);
    #else
    curr_freq=*(c->ear_frequency);
    #endif

		curr_pstate = frequency_closest_pstate(curr_freq);

		eff_f=frequency_closest_high_freq(my_app->avg_f,1);

#if POWERCAP
    if (is_powercap_set(&c->app->pc_opt)){ 
			verbose(1,"Powercap is set to %u Watts",get_powercapopt_value(&c->app->pc_opt));
      power_status=compute_power_status(&c->app->pc_opt,(uint)(my_app->DC_power));
			if ((eff_f<curr_freq) && (power_status==PC_STATUS_GREEDY)){
				verbose(1,"We are running at less effective frequency than selected curr=%lu def=%lu eff=%lu",curr_freq,def_freq,eff_f);
			}
		}
#endif

		*ready=1;

		// If is not the default P_STATE selected in the environment, a projection
		// is made for the reference P_STATE in case the coefficents were available.
		// 
		if (curr_freq != def_freq) // Use configuration when available
		{
		if (projection_available(curr_pstate,def_pstate)==EAR_SUCCESS)
		{
				st=project_power(my_app,curr_pstate,def_pstate,&power_ref);
				st=project_time(my_app,curr_pstate,def_pstate,&time_ref);
				best_freq=def_freq;
        debug("projecting from %lu to %lu\t time: %.2lf\t power: %.2lf", curr_pstate, def_pstate, time_ref, power_ref);
		}
		else
		{
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_freq=curr_freq;
		}
		}
		// If it is the default P_STATE selected in the environment, then a projection
		// is not needed, so the signature will be enough as a reference. 
		else
		{ // we are running at default frequency , signature is our reference
			if (curr_freq == nominal){
				/* And we are the nominal freq */
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_freq=curr_freq;
			}else{
				/* Nominal is the reference , if projections are ready, we project time and power */
         debug("current_freq is not the nominal");

				if (projection_available(curr_pstate,min_pstate) == EAR_SUCCESS){
					project_power(my_app,curr_pstate,min_pstate,&power_ref);
					project_time(my_app,curr_pstate,min_pstate,&time_ref);
					best_freq=nominal;
          debug("projecting to nominal\t time: %.2lf\t power: %.2lf", time_ref, power_ref);
				}else{
        	time_ref=my_app->time;
        	power_ref=my_app->DC_power;
        	best_freq=curr_freq;
				}
			}

		}
		energy_ref=power_ref*time_ref;
		best_solution=energy_ref;

	// We compute the maximum performance loss
	time_max = time_ref + (time_ref * max_penalty);

   debug("Max_freq set to %lu min_pstate = %d nominal %lu curr_frequency %lu curr_pstate %lu time_max: %.2lf",c->app->max_freq,min_pstate,nominal,curr_freq,curr_pstate,time_max);

	debug("Policy Signature (CPI=%lf GBS=%lf Power=%lf Time=%lf TPI=%lf)",my_app->CPI,my_app->GBS,my_app->DC_power,my_app->time,my_app->TPI);

	// MIN_ENERGY_TO_SOLUTION ALGORITHM
	// Calcular el min_pstate que este dentro del limite
	for (i = min_pstate; i < c->num_pstates;i++)
	{
		if (projection_available(curr_pstate,i)==EAR_SUCCESS)
		{
				st=project_power(my_app,curr_pstate,i,&power_proj);
				st=project_time(my_app,curr_pstate,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				energy_proj=power_proj*time_proj;
        debug("projected from %lu to %d\t time: %.2lf\t power: %.2lf energy: %.2lf", curr_pstate, i, time_proj, power_proj, energy_proj);
			if ((energy_proj < best_solution) && (time_proj < time_max))
			{
          debug("new best solution found");
					best_freq = frequency_pstate_to_freq(i);
					best_solution = energy_proj;
					best_pstate=i;
			}
		}
	}
	/* Corregir frecuencia por powercap y activar greedy si es necesario */
	}else{ 
		*ready=0;
		return EAR_ERROR;
	}
  #ifdef POWERCAP
  projection_t *p;
  uint plimit;
  req_f=best_freq;
  plimit=c->app->pc_opt.last_t1_allocated;
  p=projection_get(best_pstate);
  if (p->Power>plimit){
  	do{
        best_pstate++;
        p=projection_get(best_pstate);
  	}while((p->Power>plimit) && (best_pstate<c->num_pstates));
  	best_freq=frequency_pstate_to_freq(best_pstate);
  	verbose(1,"Frequency selected is not valid because powercap limits req_f %lu selected %lu, status should be greedy",req_f,best_freq);
  }
  #endif

	*new_freq=best_freq;
	return EAR_SUCCESS;
}


state_t policy_ok(polctx_t *c,signature_t *curr_sig,signature_t *last_sig,int *ok)
{
	double energy_last, energy_curr;
	uint power_status,next_status;
	ulong eff_f;

	if ((c==NULL) || (curr_sig==NULL) || (last_sig==NULL)) return EAR_ERROR;

#if POWERCAP
    if (is_powercap_set(&c->app->pc_opt)){
      power_status=compute_power_status(&c->app->pc_opt,(uint)(curr_sig->DC_power));
      eff_f=frequency_closest_high_freq(curr_sig->avg_f,1);
			verbose(1,"Powercap is set to %u Watts, status %u effective freq %lu",get_powercapopt_value(&c->app->pc_opt),power_status,eff_f);
      if (eff_f<curr_sig->def_f){
        verbose(1,"Running with powercap, status %u and effective freq %lu vs selected %lu",power_status,eff_f,curr_sig->def_f);
      }
      next_status=compute_next_status(&c->app->pc_opt,(uint)(curr_sig->DC_power),eff_f,req_f);
      verbose(1,"New application state should be %u",next_status);
    }else{
      verbose(1,"Powercap is not set");
      power_status=PC_STATUS_ERROR;
    }
#endif

	if (curr_sig->def_f==last_sig->def_f) *ok=1;


	energy_last = last_sig->time*last_sig->DC_power;
	energy_curr = curr_sig->time * curr_sig->DC_power;

	if ((energy_curr<energy_last)&&(curr_sig->time<(last_sig->time*c->app->settings[0]))) *ok=1;
	else *ok=0;
	return EAR_SUCCESS;
}

state_t  policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
		if (c!=NULL){
			*freq_set=c->app->max_freq;
		}else EAR_ERROR;
		return EAR_SUCCESS;
}

state_t policy_max_tries(polctx_t *c,int *intents)
{
  *intents=1;
  return EAR_SUCCESS;
}

