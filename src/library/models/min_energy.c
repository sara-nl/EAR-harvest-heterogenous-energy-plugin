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
#include <common/states.h>
#include <common/config.h>
#include <common/types/log.h>
#include <common/types/signature.h>
#include <common/types/projection.h>
#include <common/types/application.h>
#include <common/output/verbose.h>
#include <daemon/eard_api.h>
#include <control/frequency.h>
#include <library/models/models.h>
#include <library/common/externs.h>


//
static uint me_policy_pstates;
static uint me_reset_freq=RESET_FREQ;
static int use_models=1;

// Policy
extern double performance_penalty;
extern application_t *signatures;
extern uint *sig_ready;

// Process
extern uint EAR_default_pstate;

//
static double time_max;
extern ulong user_selected_freq;

//
void min_energy_init(uint num_pstates)
{
	char *env_use_models;
	me_policy_pstates=num_pstates;
	env_use_models=getenv("EAR_USE_MODELS");
	if ((env_use_models!=NULL) && (atoi(env_use_models)==0)) use_models=0;
}

void min_energy_new_loop()
{
    projection_reset(me_policy_pstates);
}

void min_energy_end_loop()
{
    if (me_reset_freq)
    {
		// Use configuration when available
        ear_frequency = eards_change_freq(get_global_def_freq());
    }
}

static void go_next_me(int curr_pstate,int *ready,ulong *best_pstate)
{
	int next_pstate;
	if (curr_pstate<(me_policy_pstates-1)){
		*ready=0;
		next_pstate=curr_pstate+1;
		*best_pstate=frequency_pstate_to_freq(next_pstate);
	}else{
		*ready=1;
		*best_pstate=frequency_pstate_to_freq(curr_pstate);
	}
	debug("Current %d next %u\n",curr_pstate,*best_pstate);
}

static int is_better_min_energy(signature_t * curr_sig,signature_t *prev_sig)
{
	double curr_energy,pre_energy;
	curr_energy=curr_sig->time*curr_sig->DC_power;
	pre_energy=prev_sig->time*prev_sig->DC_power;
	if (curr_energy>pre_energy){ 
		debug("Curr energy %lf prev energy %lf\n",curr_energy,pre_energy);
		return 0;
	}
	debug("Time %lf max %lf\n",curr_sig->time,time_max);
	if (curr_sig->time<time_max) return 1;
	return 0;
}

// This is the main function in this file, it implements power policy
ulong min_energy_policy(signature_t *sig,int *ready)
{
	signature_t *my_app;
	int i,min_pstate;
	unsigned int ref;
	double power_proj,time_proj,energy_proj,best_solution,energy_ref;
	double power_ref,time_ref;
	ulong best_pstate;
	state_t st;
	my_app=sig;
	debug("min_energy_policy starts \n");
	if (ear_use_turbo) min_pstate=0;
	else min_pstate=get_global_min_pstate();

	*ready=1;

	// This is the frequency at which we were running
	ref = frequency_freq_to_pstate(ear_frequency);

	// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
	// We must check this is ok changing these values at this point
	policy_global_reconfiguration();
	if (!eards_connected()) return EAR_default_frequency;

	if (use_models)
	{
	
	// We compute here our reference

	// If is not the default P_STATE selected in the environment, a projection
	// is made for the reference P_STATE in case the coefficents were available.
	if (ear_frequency != EAR_default_frequency) // Use configuration when available
	{
		if (projection_available(ref,EAR_default_pstate)==EAR_SUCCESS)
		{
				st=project_power(my_app,ref,EAR_default_pstate,&power_ref);
				st=project_time(my_app,ref,EAR_default_pstate,&time_ref);
				best_pstate=EAR_default_frequency;
		}
		else
		{
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_pstate=ear_frequency;
		}
	}
	// If it is the default P_STATE selected in the environment, then a projection
	// is not needed, so the signature will be enough as a reference. 
	else
	{ // we are running at default frequency , signature is our reference
			time_ref=my_app->time;
			power_ref=my_app->DC_power;
			best_pstate=ear_frequency;
	}
	energy_ref=power_ref*time_ref;
	best_solution=energy_ref;

	// We compute the maximum performance loss
	time_max = time_ref + (time_ref * performance_penalty);

	debug("min_energy: From %d to %d\n",min_pstate,me_policy_pstates);

	// MIN_ENERGY_TO_SOLUTION ALGORITHM
	for (i = min_pstate; i < me_policy_pstates;i++)
	{
		if (projection_available(ref,i)==EAR_SUCCESS)
		{
				st=project_power(my_app,ref,i,&power_proj);
				st=project_time(my_app,ref,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				energy_proj=power_proj*time_proj;
				debug("pstate=%u energy_ref %lf best_solution %lf energy_proj %lf\n",i,energy_ref,best_solution,energy_proj);
			if ((energy_proj < best_solution) && (time_proj < time_max))
			{
					best_pstate = frequency_pstate_to_freq(i);
					best_solution = energy_proj;
			}
		}
	}
	}else{ /* Use models is set to 0 */
		ulong prev_pstate,curr_pstate;
		signature_t *prev_sig;
		debug("We are not using models \n");
		/* We must not use models , we will check one by one*/
		/* If we are not running at default freq, we must check if we must follow */
		if (sig_ready[EAR_default_pstate]==0){
			*ready=0;
			best_pstate=EAR_default_frequency;
		} else{
			time_ref= signatures[EAR_default_pstate].signature.time;
			time_max = time_ref + (time_ref * performance_penalty);
			/* This is the normal case */
			curr_pstate=frequency_freq_to_pstate(ear_frequency);
			if (ear_frequency != EAR_default_frequency){
					prev_pstate=curr_pstate-1;
					prev_sig=&signatures[prev_pstate].signature;
					if (is_better_min_energy(my_app,prev_sig)){
						go_next_me(curr_pstate,ready,&best_pstate);
					}else{
						*ready=1;
						best_pstate=frequency_pstate_to_freq(prev_pstate);
					}
			}else{
				go_next_me(curr_pstate,ready,&best_pstate);
			}
		}
		debug("Curr freq %u next freq %u ready=%d\n",ear_frequency,best_pstate,*ready);
		
	}

	// Just in case the bestPstate was the frequency at which the application was running
	if (best_pstate>system_conf->max_freq){ 
		log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
		debug("EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
		best_pstate,system_conf->max_freq);
		best_pstate=system_conf->max_freq;
	}
	debug("min_energy_policy ends ---> %lu\n",best_pstate);
	return best_pstate;
}


ulong min_energy_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{
	double energy_last, energy_curr;

	if (curr_sig->def_f==last_sig->def_f) return 1;

	energy_last = last_sig->time*last_sig->DC_power;
	energy_curr = curr_sig->time * curr_sig->DC_power;

	if ((energy_curr<energy_last)&&(curr_sig->time<time_max)) return 1;
	else return 0;
}

ulong  min_energy_default_conf(ulong f)
{
    // Just in case the bestPstate was the frequency at which the application was running
    if (f>system_conf->max_freq){
		return system_conf->max_freq;
    } else return f;
}
