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
#include <control/frequency.h>
#include <library/common/macros.h>
#include <library/common/externs.h>
#include <library/models/models.h>
#include <library/models/sig_projections.h>
#include <daemon/eard_api.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/states.h>

static const char *__NAME__ = "min_time_to_solution:";
static uint mt_policy_pstates;
static uint mt_reset_freq=RESET_FREQ;
extern coefficient_t **coefficients;
extern uint EAR_default_pstate;
extern double performance_gain;

void min_time_init(uint pstates)
{
    mt_policy_pstates=pstates;
}

void min_time_new_loop()
{
    reset_performance_projection(mt_policy_pstates);
}

void min_time_end_loop()
{
    if (mt_reset_freq)
    {
        // Use configuration when available
        ear_frequency = eards_change_freq(get_global_def_freq());
    }
}


// This is the main function in this file, it implements power policy
ulong min_time_policy(signature_t *sig)
{
    signature_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next;
    double freq_gain,perf_gain;
    double power_proj,time_proj,cpi_proj,energy_proj,best_solution,energy_ref;
    double power_ref,cpi_ref,time_ref,time_current;
    ulong best_pstate;
    my_app=sig;


    if (ear_use_turbo) min_pstate=0;
    else min_pstate=get_global_min_pstate();

    // This is the frequency at which we were running
    ref = frequency_freq_to_pstate(ear_frequency);

    // This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
    // We must check this is ok changing these values at this point
    policy_global_reconfiguration();
	if (!eards_connected()) return EAR_default_frequency;

    // We compute here our reference

    // If is not the default P_STATE selected in the environment, a projection
    // is made for the reference P_STATE in case the coefficents were available.
    if (ear_frequency != EAR_default_frequency) // Use configuration when available
    {
        if (coefficients[ref][EAR_default_pstate].available)
        {
                power_ref=sig_power_projection(my_app,ear_frequency,EAR_default_pstate);
                cpi_ref=sig_cpi_projection(my_app,ear_frequency,EAR_default_pstate);
                time_ref=sig_time_projection(my_app,ear_frequency,EAR_default_pstate,cpi_ref);
                energy_ref=power_ref*time_ref;
                best_solution=energy_ref;
                best_pstate=EAR_default_frequency;
        }
        else
        {
                time_ref=my_app->time;
                power_ref=my_app->DC_power;
                cpi_ref=my_app->CPI;
                energy_ref=power_ref*time_ref;
                best_solution=energy_ref;
                best_pstate=ear_frequency;
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
            best_pstate=ear_frequency;
    }

	set_performance_projection(EAR_default_pstate,time_ref,power_ref,cpi_ref);

	// ref=1 is nominal 0=turbo, we are not using it
	#if EAR_PERFORMANCE_TESTS
	if (ear_frequency == EAR_default_frequency){
		VERBOSE_N(2,"MIN_TIME: def_pstate %u max_pstate %u th %.2lf best=%u (ear_freq=%lu def_freq %lu )\n",EAR_default_pstate,min_pstate,performance_gain,best_pstate,
		ear_frequency,EAR_default_frequency);
	}
	#endif

		try_next=1;
		i=EAR_default_pstate-1;
		time_current=time_ref;

		while(try_next && (i >= min_pstate))
		{
			if (coefficients[ref][i].available)
			{
				#if EAR_PERFORMANCE_TESTS
				if (ear_frequency == EAR_default_frequency){
					VERBOSE_N(1,"Comparing %u with %u",best_pstate,i);
				}
				#endif
				power_proj=sig_power_projection(my_app,ear_frequency,i);
				cpi_proj=sig_cpi_projection(my_app,ear_frequency,i);
				time_proj=sig_time_projection(my_app,ear_frequency,i,cpi_proj);
				set_performance_projection(i,time_proj,power_proj,cpi_proj);
				freq_gain=performance_gain*(double)(coefficients[ref][i].pstate-best_pstate)/(double)best_pstate;
				perf_gain=(time_current-time_proj)/time_current;
				#if EAR_PERFORMANCE_TESTS
				if (ear_frequency == EAR_default_frequency){
					VERBOSE_N(1,"Freq gain %lf Perf gain %lf\n",freq_gain,perf_gain);
				}
				#endif

				// OK
				if (perf_gain>=freq_gain)
				{
					best_pstate=coefficients[ref][i].pstate;
					time_current = time_proj;
					i--;
				}
				else
				{
					try_next = 0;
				}
			} // Coefficients available
			else{ 
				#if EAR_PERFORMANCE_TESTS
				if (ear_frequency == EAR_default_frequency){
					VERBOSE_N(1,"Coefficients for node %s [%d][%d] not available.... try=0\n",node_name,ref,i);
					
				}
				#endif
				try_next=0;
			}
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
	double energy_proj, energy_real;

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
        ear_verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
        f,system_conf->max_freq);
        return system_conf->max_freq;
    }else return f;
}

