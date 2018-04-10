/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#include <control/frequency.h>
#include <library/common/macros.h>
#include <library/common/externs.h>
#include <library/models/models.h>
#include <library/models/sig_projections.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/states.h>
#include <common/config.h>

static uint me_policy_pstates;
static uint me_reset_freq=RESET_FREQ;
static char *__NAME__ = "min_energy_policy";

// Policy
extern double performance_penalty;
extern coefficient_t **coefficients;

// Process
extern uint EAR_default_pstate;

static double time_max;
extern ulong user_selected_freq;


void min_energy_init(uint num_pstates)
{
	me_policy_pstates=num_pstates;
}
void min_energy_new_loop()
{
    reset_performance_projection(me_policy_pstates);
}

void min_energy_end_loop()
{
    if (me_reset_freq)
    {
		// Use configuration when available
        ear_frequency = eards_change_freq(EAR_default_frequency);
    }
}



// This is the main function in this file, it implements power policy
ulong min_energy_policy(application_t *sig)
{
	application_t *my_app;
	int i,min_pstate;
	unsigned int ref,try_next;
	double freq_gain,perf_gain;
	double power_proj,time_proj,cpi_proj,energy_proj,best_solution,energy_ref;
	double power_ref,cpi_ref,time_ref,time_current;
	ulong best_pstate;
	my_app=sig;

	ear_verbose(1,"min_energy_policy starts \n");
	if (ear_use_turbo) min_pstate=0;
	else min_pstate=get_global_min_pstate();

	// This is the frequency at which we were running
	ref = frequency_freq_to_pstate(ear_frequency);

	// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
	// We must check this is ok changing these values at this point
	policy_global_reconfiguration();
	
	// We compute here our reference

	// If is not the default P_STATE selected in the environment, a projection
	// is made for the reference P_STATE in case the coefficents were available.
	if (ear_frequency != EAR_default_frequency) // Use configuration when available
	{
		ear_verbose(1,"We are not running at default freq\n");
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
			ear_verbose(1,"We are running at default freq\n");
			time_ref=my_app->time;
			power_ref=my_app->DC_power;
			cpi_ref=my_app->CPI;
			energy_ref=power_ref*time_ref;
			best_solution=energy_ref;
			best_pstate=ear_frequency;
	}

	// We compute the maximum performance loss
	time_max = time_ref + (time_ref * performance_penalty);

	ear_verbose(1,"min_pstate %u max_pstate %u\n",min_pstate,me_policy_pstates);

	// MIN_ENERGY_TO_SOLUTION BEGIN
	for (i = min_pstate; i < me_policy_pstates;i++)
	{
		// If coeffs are available
		if (coefficients[ref][i].available)
		{
				power_proj=sig_power_projection(my_app,ear_frequency,i);
				cpi_proj=sig_cpi_projection(my_app,ear_frequency,i);
				time_proj=sig_time_projection(my_app,ear_frequency,i,cpi_proj);
				set_performance_projection(i,time_proj,power_proj,cpi_proj);
				energy_proj=power_proj*time_proj;

				RANK(0) {
					VERBOSE_N(1, "Projection (%u): [power: %lf, time: %lf, energy: %lf]",
						coefficients[ref][i].pstate, power_proj, time_proj, energy_proj);
				}
			ear_verbose(1,"pstate=%u energy_ref %lf best_solution %lf energy_proj %lf\n",i,energy_ref,best_solution,energy_proj);
			if ((energy_proj < best_solution) && (time_proj < time_max))
			{
					best_pstate = coefficients[ref][i].pstate;
					best_solution = energy_proj;
			}
		}
	}

	// Coefficients were not available for this nominal frequency
	#if SHARED_MEMORY
	// Just in case the bestPstate was the frequency at which the application was running
	if (best_pstate>system_conf->max_freq){ 
		log_report_global_policy_freq(my_job_id,system_conf->max_freq);
		ear_verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
		best_pstate,system_conf->max_freq);
		best_pstate=system_conf->max_freq;
	}
	#endif
	ear_verbose(1,"min_energy_policy ends ---> %lu\n",best_pstate);
	return best_pstate;
}


ulong min_energy_policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig)
{
	double energy_last, energy_curr;

	energy_last = last_sig->time*last_sig->DC_power;
	energy_curr = curr_sig->time * curr_sig->DC_power;

	if ((energy_curr<energy_last)&&(curr_sig->time<time_max)) return 1;
	else return 0;
}



