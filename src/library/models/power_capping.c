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
#include <common/types/signature.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/states.h>

static uint pc_policy_pstates;
static char *__NAME__ = "power_capping";

// Policy
extern double power_cap_limit;
extern coefficient_t **coefficients;

// Process
extern uint EAR_default_pstate;

extern ulong user_selected_freq;

void power_cap_energy_init(uint num_pstates)
{
	pc_policy_pstates=num_pstates;
}
void power_cap_energy_new_loop()
{
    proj_perf_reset_old(me_policy_pstates);
}

void power_cap_energy_end_loop()
{
    if (me_reset_freq)
    {
		// Use configuration when available
        ear_frequency = eards_change_freq(get_global_def_freq());
    }
}



// This is the main function in this file, it implements power policy
ulong power_cap_energy_policy(signature_t *sig)
{
	signature_t *my_app;
	int i,min_pstate;
	unsigned int ref,try_next;
	double freq_gain,perf_gain;
	double power_proj,time_proj,cpi_proj,energy_proj,best_solution,energy_ref;
	double power_ref,cpi_ref,time_ref,time_current;
	ulong best_freq;
	my_app=sig;
	if (ear_use_turbo) min_pstate=0;
	else min_pstate=get_global_min_pstate();
	best_freq=EAR_default_frequency;

	// This is the frequency at which we were running
	ref = frequency_freq_to_pstate(ear_frequency);

	// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
	// We must check this is ok changing these values at this point
	policy_global_reconfiguration();
	if (!eards_connected()) return EAR_default_frequency;
	
	// POWER_CAPPING BEGIN
	for (i = min_pstate; i < pc_policy_pstates;i++)
	{
		// If coeffs are available
		if (coefficients[ref][i].available)
		{
				power_proj=sig_power_projection(my_app,ear_frequency,i);
				cpi_proj=sig_cpi_projection(my_app,ear_frequency,i);
				time_proj=sig_time_projection(my_app,ear_frequency,i,cpi_proj);
				proj_perf_set_old(i,time_proj,power_proj,cpi_proj);
				if (power_proj<xxx) && (time_proj<YYY) best_freq=frequency_pstate_to_freq(i);
		}
	}

	// Coefficients were not available for this nominal frequency
	// Just in case the bestPstate was the frequency at which the application was running
	if (best_freq>system_conf->max_freq){ 
		log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
		ear_verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
		best_freq,system_conf->max_freq);
		best_freq=system_conf->max_freq;
	}
	return best_freq;
}


ulong power_cap_energy_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{

	return 1;
}

ulong  min_energy_default_conf(ulong f)
{
    // Just in case the bestPstate was the frequency at which the application was running
    if (f>system_conf->max_freq){
        log_report_global_policy_freq(application.job.id,application.job.step_id,system_conf->max_freq);
        ear_verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
        f,system_conf->max_freq);
		return system_conf->max_freq;
    } else return f;
}
