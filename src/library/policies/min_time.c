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
#include <common/types/generic.h>
#include <common/types/log.h>
#include <common/types/projection.h>
#include <common/types/application.h>
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <daemon/eard_api.h>
#include <control/frequency.h>
#include <library/common/externs.h>
#include <library/policies/policy.h>



/*
 *   "ear_policy_init",
 *     "ear_policy_apply",
 *       "ear_policy_get_default_freq",
 *         "ear_policy_set_default_freq",
 *           "ear_policy_ok",
 *             "ear_policy_max_tries",
 *               "ear_policy_end",
 *                 "ear_policy_loop_init",
 *                   "ear_policy_loop_end",
 *                     "ear_policy_new_iteration",
 *                       "ear_policy_mpi_init",
 *                         "ear_policy_mpi_end"
 *                           "ear_policy_configure",
 *                           */
/*
 * state_t (*init)        (polctx_t *c);
 * state_t (*apply)       (polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready);
 * state_t (*get_default_freq)   (polctx_t *c, ulong *freq_set);
 * state_t (*set_default_freq)   (polctx_t *c);
 * state_t (*ok)          (polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok);
 * state_t (*max_tries)   (int *intents);
 * state_t (*end)         ();
 * state_t (*loop_init)   (loop_id_t *loop_id);
 * state_t (*loop_end)    (loop_id_t *loop_id);
 * state_t (*new_iter)    (loop_id_t *loop_id);
 * state_t (*mpi_init)    ();
 * state_t (*mpi_end)     ();
 * state_t (*reconfigure) (polctx_t *c);
 * state_t (*configure) (polctx_t *c);
 * */


static uint mt_policy_pstates;
static uint mt_reset_freq=RESET_FREQ;
extern uint EAR_default_pstate;
extern double performance_gain;
extern application_t *signatures;
extern uint *sig_ready;
static int use_models=1;

void min_time_init(uint pstates)
{
    mt_policy_pstates=pstates;
    char *env_use_models;
    env_use_models=getenv("EAR_USE_MODELS");
    if ((env_use_models!=NULL) && (atoi(env_use_models)==0)) use_models=0;

}

void min_time_new_loop()
{
    projection_reset(mt_policy_pstates);
}

void min_time_end_loop()
{
    if (mt_reset_freq)
    {
        // Use configuration when available
        ear_frequency = eards_change_freq(get_global_def_freq());
    }
}
static void go_next_mt(int curr_pstate,int *ready,ulong *best_pstate,int min_pstate)
{
	int next_pstate;
	if (curr_pstate==min_pstate){
		*ready=1;
		*best_pstate=frequency_pstate_to_freq(curr_pstate);
	}else{
		next_pstate=curr_pstate-1;
		*ready=0;
		*best_pstate=frequency_pstate_to_freq(next_pstate);
	}
}

static int is_better_min_time(signature_t * curr_sig,signature_t *prev_sig)
{
  double freq_gain,perf_gain;
	int curr_freq,prev_freq;

	curr_freq=curr_sig->def_f;
	prev_freq=prev_sig->def_f;
	debug("curr %u prev %u\n",curr_freq,prev_freq);
	freq_gain=performance_gain*(double)((curr_freq-prev_freq)/(double)prev_freq);
  perf_gain=(prev_sig->time-curr_sig->time)/prev_sig->time;
	debug("Performance gain %lf Frequency gain %lf\n",perf_gain,freq_gain);
	if (perf_gain>=freq_gain) return 1;
    return 0;
}



// This is the main function in this file, it implements power policy
ulong min_time_policy(signature_t *sig,int *ready)
{
    signature_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next;
    double freq_gain,perf_gain;
    double power_proj,time_proj;
    double power_ref,time_ref,time_current;
    ulong best_pstate,freq_ref;
		state_t st;
    my_app=sig;

		*ready=1;

    if (ear_use_turbo) min_pstate=0;
    else min_pstate=get_global_min_pstate();

    // This is the frequency at which we were running
    ref = frequency_freq_to_pstate(ear_frequency);

    // This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
    // We must check this is ok changing these values at this point
    policy_global_reconfiguration();
	if (!eards_connected()) return EAR_default_frequency;

	if (use_models){

    // We compute here our reference

    // If is not the default P_STATE selected in the environment, a projection
    // is made for the reference P_STATE in case the projections were available.
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

	projection_set(EAR_default_pstate,time_ref,power_ref);

	// ref=1 is nominal 0=turbo, we are not using it

		try_next=1;
		i=EAR_default_pstate-1;
		time_current=time_ref;


		while(try_next && (i >= min_pstate))
		{
			if (projection_available(ref,i)==EAR_SUCCESS)
			{
				st=project_power(my_app,ref,i,&power_proj);
				st=project_time(my_app,ref,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				freq_ref=frequency_pstate_to_freq(i);
				freq_gain=performance_gain*(double)(freq_ref-best_pstate)/(double)best_pstate;
				perf_gain=(time_current-time_proj)/time_current;

				// OK
				if (perf_gain>=freq_gain)
				{
					best_pstate=freq_ref;
					time_current = time_proj;
					i--;
				}
				else
				{
					try_next = 0;
				}
			} // Projections available
			else{
				try_next=0;
			}
		}	
	}else{/* Use models is set to 0 */
        ulong prev_pstate,curr_pstate;
        signature_t *prev_sig;
        debug("We are not using models \n");
        /* We must not use models , we will check one by one*/
        /* If we are not running at default freq, we must check if we must follow */
        if (sig_ready[EAR_default_pstate]==0){
            *ready=0;
            best_pstate=EAR_default_frequency;
        } else{
        	/* This is the normal case */
        		curr_pstate=frequency_freq_to_pstate(ear_frequency);
        		if (ear_frequency != EAR_default_frequency){
                	prev_pstate=curr_pstate+1;
                	prev_sig=&signatures[prev_pstate].signature;
                	if (is_better_min_time(my_app,prev_sig)){
										go_next_mt(curr_pstate,ready,&best_pstate,min_pstate);
                	}else{
                    	*ready=1;
                    	best_pstate=frequency_pstate_to_freq(prev_pstate);
                	}
        		}else{
					go_next_mt(curr_pstate,ready,&best_pstate,min_pstate);
				}
        }
		debug("Curr freq %u next freq %u ready=%d\n",ear_frequency,best_pstate,*ready);
     }


	// Coefficients were not available for this nominal frequency
	if (system_conf!=NULL){
	// Just in case the bestPstate was the frequency at which the application was running
	if (best_pstate>system_conf->max_freq){ 
		log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
		best_pstate=system_conf->max_freq;
	}
	}

	return best_pstate;
}


ulong min_time_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{

	if (curr_sig->def_f==last_sig->def_f) return 1;

	// Check that efficiency is enough
	if (curr_sig->time < last_sig->time) return 1;

	// If time is similar and it was same freq it is ok	
	if (equal_with_th(curr_sig->time , last_sig->time,0.1) && (curr_sig->def_f == last_sig->def_f)) return 1;
	
	// If we run at a higher freq but we are slower, we are not ok	
	if ((curr_sig->time > last_sig->time) && (curr_sig->def_f > last_sig->def_f)) return 0;

	else return 0;

}
ulong  min_time_default_conf(ulong f)
{
    // Just in case the bestPstate was the frequency at which the application was running
    if ((system_conf!=NULL) && (f>system_conf->max_freq)){
        log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
        debug("EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
        f,system_conf->max_freq);
        return system_conf->max_freq;
    }else return f;
}

