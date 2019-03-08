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
#include <common/output/verbose.h>
#include <common/types/log.h>
#include <common/types/projection.h>
#include <common/types/application.h>
#include <daemon/eard_api.h>
#include <control/frequency.h>
#include <library/common/externs.h>
#include <library/models/models.h>

#define NO_MODELS_SM_VERBOSE	0


static uint sm_policy_pstates;
static uint sm_reset_freq=RESET_FREQ;
extern coefficient_t **coefficients;
extern uint EAR_default_pstate;
extern double performance_gain;
extern application_t *signatures;
extern uint *sig_ready;
static int use_models=1;
static double *timep,*powerp;

void supermuc_init(uint pstates)
{
    sm_policy_pstates=pstates;
    char *env_use_models;
	verbose(SM_VERB,"supermuc_init\n");
    env_use_models=getenv("EAR_USE_MODELS");
    if ((env_use_models!=NULL) && (atoi(env_use_models)==0)) use_models=0;
	timep=(double*)malloc(pstates*sizeof(double));
	powerp=(double*)malloc(pstates*sizeof(double));
	sm_penalty=0.1;

}

void supermuc_new_loop()
{
    projection_reset(sm_policy_pstates);
}

void supermuc_end_loop()
{
    if (sm_reset_freq)
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
	verbose(NO_MODELS_SM_VERBOSE,"curr %u prev %u\n",curr_freq,prev_freq);
	freq_gain=performance_gain*(double)((curr_freq-prev_freq)/(double)prev_freq);
   	perf_gain=(prev_sig->time-curr_sig->time)/prev_sig->time;
	verbose(NO_MODELS_SM_VERBOSE,"Performance gain %lf Frequency gain %lf\n",perf_gain,freq_gain);
	if (perf_gain>=freq_gain) return 1;
    return 0;
}

static void fill_projections(signature_t *sig,int ref)
{
	int i;
	verbose(SM_VERB,"Creating projections using time_ref=%lf power_ref=%lf , ref=%ul \n",sig->time,sig->DC_power,ref);
	for (i=0;i<pstates;i++)
	{
		if (i==ref){
			timep[i]=sig->time;
			powerp[i]=sig->DC_power;
		}else{	
			if (coefficients[ref][i].available){
				timep[i]=project_time(sig,&coefficients[ref][i]);
				powerp[i]=project_power(sig,&coefficients[ref][i]);
			}else{
				timep[i]=0;
				powerp[i]=0;
			}
		
		}
		projection_set(i,timep[i],powerp[i]);
	}
}



// This is the main function in this file, it implements power policy
ulong supermuc_policy(signature_t *sig,int *ready)
{
    signature_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next,boosted=0;
    double freq_gain,perf_gain;
    double power_proj,time_proj,cpi_proj,energy_proj,best_solution,energy_ref;
    double power_ref,cpi_ref,time_ref,time_current;
    ulong best_freq;
    my_app=sig;

	*ready=1;

	verbose(SM_VERB,"supermuc_policy\n");
	if ((timep==NULL) || (powerp==NULL)){
		*ready=0;
		return EAR_default_frequency
	}

    if (ear_use_turbo) min_pstate=0;
    else min_pstate=get_global_min_pstate();


    // This is the frequency at which we were running
    ref = frequency_freq_to_pstate(ear_frequency);


	fill_projections(sig,ref);

    // This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
    // We must check this is ok changing these values at this point
    policy_global_reconfiguration();
	if (!eards_connected()) return EAR_default_frequency;

	if (use_models){

    // We compute here our reference

    // If is not the default P_STATE selected in the environment, a projection
    // is made for the reference P_STATE in case the coefficents were available.
    if (ear_frequency != EAR_default_frequency) // Use configuration when available
    {
        if (coefficients[ref][EAR_default_pstate].available)
        {
                power_ref=powerp[EAR_default_pstate];
                time_ref=timep[EAR_default_pstate];
                energy_ref=power_ref*time_ref;
                best_solution=energy_ref;
                best_freq=EAR_default_frequency;
        }
        else
        {
                time_ref=my_app->time;
                power_ref=my_app->DC_power;
                cpi_ref=my_app->CPI;
                energy_ref=power_ref*time_ref;
                best_solution=energy_ref;
                best_freq=ear_frequency;
        }
    }
    // If it is the default P_STATE selected in the environment, then a projection
    // is not needed, so the signature will be enough as a reference.
    else
    { // we are running at default frequency , signature is our reference
            time_ref=my_app->time;
            power_ref=my_app->DC_power;
            cpi_ref=my_app->CPI;
            energy_ref=power_ref*time_ref;
            best_solution=energy_ref;
            best_freq=ear_frequency;
    }

	projection_set(EAR_default_pstate,time_ref,power_ref);

	// ref=1 is nominal 0=turbo, we are not using it

		try_next=1;
		i=EAR_default_pstate-1;
		time_current=time_ref;

		/* MIN_TIME PHASE */

		while(try_next && (i >= min_pstate))
		{
			if (coefficients[ref][i].available)
			{
				freq_gain=performance_gain*(double)(coefficients[ref][i].pstate-best_freq)/(double)best_freq;
				perf_gain=(time_current-timep[i])/time_current;

				// OK
				if (perf_gain>=freq_gain)
				{
					best_freq=coefficients[ref][i].pstate;
					time_current = timep[i];
					i--;
					boosted=1;
				}
				else
				{
					try_next = 0;
				}
			} // Coefficients available
			else{ 
				try_next=0;
			}
		}	
		/* after that phase, we will chech if we should reduce the frequency */
		if (!boosted){
		/* MIN_ENERGY PHASE */
			best_solution=powerp[ref]*timep[ref];
			time_max=timep[ref]+timep[ref]*sm_penalty;
			verbose(SM_VERB,"Min_energy phase: reference = time  %lf power %lf energy %lf\n",timep[ref],powerp[ref],best_solution);
			for (i = EAR_default_pstate+1; i < sm_policy_pstates;i++)
			{
			/* If coeffs are available */
						if (coefficients[ref][i].available)
						{
						energy_proj=powerp[i]*timep[i];
						if ((energy_proj < best_solution) && (timep[i] < time_max))
						{
							best_freq = coefficients[ref][i].pstate;
							best_solution = energy_proj;
							verbose(SM_VERB,"Selecting freq %lu\n",best_freq);
						}
						}
			}
			verbose(SM_VERB,"Frequency selected after MIN_ENERGY phase %ul energy estimated %lf\n",best_freq,best_solution);

		}
	}else{/* Use models is set to 0 */
        ulong prev_pstate,curr_pstate,next_pstate;
        signature_t *prev_sig;
        verbose(NO_MODELS_SM_VERBOSE,"We are not using models \n");
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
		verbose(NO_MODELS_SM_VERBOSE,"Curr freq %u next freq %u ready=%d\n",ear_frequency,best_pstate,*ready);
     }

	


	// Coefficients were not available for this nominal frequency
	if (system_conf!=NULL){
	// Just in case the bestPstate was the frequency at which the application was running
	if (best_freq>system_conf->max_freq){ 
		log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
		best_freq=system_conf->max_freq;
	}
	}

	return best_freq;
}


ulong supermuc_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{
	double energy_proj, energy_real;

	verbose(SM_VERB,"supermuc_policy_ok\n");

	if (curr_sig->def_f==last_sig->def_f) return 1;

	// Check that efficiency is enough
	if (curr_sig->time < last_sig->time) return 1;

	// If time is similar and it was same freq it is ok	
	if (equal_with_th(curr_sig->time , last_sig->time,0.1) && (curr_sig->def_f == last_sig->def_f)) return 1;
	
	// If we run at a higher freq but we are slower, we are not ok	
	if ((curr_sig->time > last_sig->time) && (curr_sig->def_f > last_sig->def_f)) return 0;

	else return 0;

}
ulong  supermuc_default_conf(ulong f)
{
    // Just in case the bestPstate was the frequency at which the application was running
    verbose(SM_VERB,"supermuc_default_conf\n");
    if ((system_conf!=NULL) && (f>system_conf->max_freq)){
        log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
        verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
        f,system_conf->max_freq);
        return system_conf->max_freq;
    }else return f;
}

