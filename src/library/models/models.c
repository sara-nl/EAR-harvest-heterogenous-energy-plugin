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
#include <library/common/externs.h>
#include <library/models/models.h>
#include <library/models/min_time.h>
#include <library/models/min_energy.h>
#include <library/models/monitoring.h>
#include <daemon/eard_api.h>
#include <control/frequency.h>
#include <metrics/custom/hardware_info.h>

static int use_default=1;

typedef struct policy
{
	void (*init)(uint num_pstates);
	void (*new_loop)();
	void (*end_loop)();
	ulong (*policy)(signature_t *sig,int *ready);
	ulong (*policy_ok)(projection_t *proj, signature_t *curr_sig, signature_t *last_sig);
	ulong (*default_conf)(ulong user_freq);
}policy_t;

policy_t app_policy;

// Policy
static int power_model_policy = MIN_ENERGY_TO_SOLUTION;
double performance_penalty ;
double performance_gain ;

// Normals
//
static uint reset_freq_opt = RESET_FREQ;
static uint ear_models_pstates = 0;
static ulong user_selected_freq;
static int model_nominal=1;

void init_policy_functions()
{
    switch(power_model_policy)
    {
        case MIN_ENERGY_TO_SOLUTION:
			app_policy.init=min_energy_init;
			app_policy.new_loop=min_energy_new_loop;
			app_policy.end_loop=min_energy_end_loop;
			app_policy.policy=min_energy_policy;
			app_policy.policy_ok=min_energy_policy_ok;
			app_policy.default_conf=min_energy_default_conf;
        break;
        case MIN_TIME_TO_SOLUTION:
			app_policy.init=min_time_init;
			app_policy.new_loop=min_time_new_loop;
			app_policy.end_loop=min_time_end_loop;
			app_policy.policy=min_time_policy;
			app_policy.policy_ok=min_time_policy_ok;
			app_policy.default_conf=min_time_default_conf;
        break;
        case MONITORING_ONLY:
			app_policy.init=monitoring_init;
			app_policy.new_loop=monitoring_new_loop;
			app_policy.end_loop=monitoring_end_loop;
			app_policy.policy=monitoring_policy;
			app_policy.policy_ok=monitoring_policy_ok;
			app_policy.default_conf=monitoring_default_conf;
        break;
    }

}

// This function sets the default freq based on the policy
/* For min energy we reconfigure only max_freq, for min time max_freq and th, and for monitoring only max_freq */
int policy_global_configuration(int p_state)
{
	if (system_conf!=NULL){
	switch (power_model_policy){
	case MIN_ENERGY_TO_SOLUTION:
		return frequency_freq_to_pstate(system_conf->max_freq);
		break;
	case MIN_TIME_TO_SOLUTION:
		performance_penalty=system_conf->th;
	case MONITORING_ONLY:
		return frequency_freq_to_pstate(system_conf->def_freq);
		break;
	}
	}
	return p_state;
}

// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
void policy_global_reconfiguration()
{
	if (system_conf!=NULL){
	debug("earl: policy_global_reconfiguration policy %d max %lu def %lu th %.2lf",
	power_model_policy,system_conf->max_freq,system_conf->def_freq,system_conf->th);
	switch (power_model_policy){
	case MIN_ENERGY_TO_SOLUTION:
	    // We filter initial configuration
	    if (system_conf->max_freq<EAR_default_frequency){
	        debug("earl: max freq set to %lu because of power capping policies",system_conf->max_freq);
			EAR_default_frequency=system_conf->max_freq;
	       	EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);
	    }
		break;
	case MIN_TIME_TO_SOLUTION:
		if (system_conf->def_freq!=EAR_default_frequency){
	        debug("EAR def freq set to %lu because of power capping policies",system_conf->def_freq);
			EAR_default_frequency=system_conf->def_freq;
	       	EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);
		}
		break;
	case MONITORING_ONLY:
		if (system_conf->max_freq<ear_frequency){
			debug("earl: max freq set to %lu because of power capping policies",system_conf->max_freq);
			EAR_default_frequency=system_conf->max_freq;
			EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);	
		}
		if (ear_frequency<user_selected_freq){
			if (system_conf->max_freq>=user_selected_freq){
				EAR_default_frequency=user_selected_freq;
				EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);
			}
		}
		break;
	}
    if (performance_gain<system_conf->th){
        	debug("earl: min perf. efficiency th set to %lf because of power capping policies",system_conf->th);
        	performance_gain=system_conf->th;
    }
	}
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_min_pstate()
{
    if (system_conf!=NULL) return frequency_freq_to_pstate(system_conf->max_freq);
	else return model_nominal;
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_def_pstate()
{
    if (system_conf!=NULL) return frequency_freq_to_pstate(system_conf->def_freq);
	else return EAR_default_pstate;
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
ulong get_global_def_freq()
{
    if (system_conf!=NULL) return system_conf->def_freq;
	else return EAR_default_pstate;
}

double get_global_th()
{
	switch (power_model_policy)
	{
		case MIN_TIME_TO_SOLUTION:
			if (system_conf!=NULL){ 
				return system_conf->th;
			}else{ 
				return performance_gain;
			}
			break;
		case MIN_ENERGY_TO_SOLUTION:
			return performance_penalty;
			break;
		case MONITORING_ONLY:
			return 0.0;
	}
	return 0.0;
}

void init_power_policy()
{
	unsigned long def_freq;

	debug("init_power_policy");
	
	power_model_policy = get_ear_power_policy();

	if (power_model_policy==MIN_ENERGY_TO_SOLUTION) performance_penalty=get_ear_power_policy_th();
	else if (power_model_policy==MIN_TIME_TO_SOLUTION) performance_gain=get_ear_power_policy_th();

	if (is_cpu_boost_enabled()) model_nominal=1;
	else model_nominal=0;

	reset_freq_opt=get_ear_reset_freq();

	// This variable defines the first state in which a policy will start on.
	EAR_default_pstate = get_ear_p_state();

	if (EAR_default_pstate >= frequency_get_num_pstates()) {
		EAR_default_pstate = DEFAULT_P_STATE;
	}

	EAR_default_pstate=policy_global_configuration(EAR_default_pstate);

	user_selected_freq = EAR_default_frequency = frequency_pstate_to_freq(EAR_default_pstate);

	
	// IMPORTANT: here is where the environment first P_STATE is set.
	ear_frequency = def_freq = eards_change_freq(EAR_default_frequency);

	if (def_freq != EAR_default_frequency)
	{
		warning("ear: warning max freq is limited by the system, using %lu as default", def_freq);

		EAR_default_frequency = def_freq;
	}
	policy_id_to_name(power_model_policy,application.job.policy);
	init_policy_functions();
	app_policy.init(frequency_get_num_pstates());

}


void init_power_models()
{
	uint p_states;
	p_state=frequency_get_num_pstates();
	ear_models_pstates = p_states;
	projection_create(p_states);
	projection_reset(p_states);
}




uint policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{
	return app_policy.policy_ok(proj, curr_sig,last_sig);
}



// When 'evaluating signature', this function is called.
unsigned long policy_power(unsigned int whole_app, signature_t* MY_SIGNATURE,int *ready)
{
	unsigned long optimal_freq, max_freq;

	if (whole_app) return ear_frequency;
	optimal_freq = app_policy.policy(MY_SIGNATURE,ready);

	if (optimal_freq != ear_frequency)
	{
		debug("earl: Changing Frequency to %u at the beggining of iteration",optimal_freq);

		ear_frequency = max_freq = eards_change_freq(optimal_freq);

		if (max_freq != optimal_freq) {
			optimal_freq = max_freq;
		}
	} else {
		debug("earl: %u selected, no changes are required",optimal_freq);
	}

	return optimal_freq;

}

void force_global_frequency(ulong new_f)
{							
	ear_frequency=new_f;
	eards_change_freq(ear_frequency);
}

void policy_new_loop(loop_id_t *lid)
{
	app_policy.new_loop();
}

void policy_end_loop(loop_id_t *lid)
{
    app_policy.end_loop();
}

ulong policy_default_configuration()
{
	ear_frequency=app_policy.default_conf(user_selected_freq);
	debug("Going to default frequency %lu",ear_frequency);
	eards_change_freq(ear_frequency);
	return ear_frequency;
}

ulong policy_get_default_freq()
{
	return app_policy.default_conf(user_selected_freq);
}

int policy_max_tries()
{
        switch (power_model_policy)
        {
            case MIN_TIME_TO_SOLUTION:
				return 2;
                break;
            case MIN_ENERGY_TO_SOLUTION:
				return 1;
                break;
            case MONITORING_ONLY:
                        return 0;
        }
	return 0;
}
