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
#include <library/models/monitoring.h>
#include <library/models/min_energy.h>
#include <library/models/min_time.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/states.h>

typedef struct policy
{
	void (*init)(uint num_pstates);
	void (*new_loop)();
	void (*end_loop)();
	ulong (*policy)(signature_t *sig);
	ulong (*policy_ok)(projection_t *proj, signature_t *curr_sig, signature_t *last_sig);
	ulong (*default_conf)(ulong user_freq);
}policy_t;

policy_t app_policy;

static const char *__NAME__ = "MODELS";

// Policy
static int power_model_policy = MIN_ENERGY_TO_SOLUTION;
double performance_penalty ;
double performance_gain ;

// Normals
coefficient_t **coefficients;
static uint reset_freq_opt = RESET_FREQ;
static uint ear_models_pstates = 0;
static ulong user_selected_freq;


void print_energy_policy_configuration()
{
    switch(power_model_policy)
    {
        case MIN_ENERGY_TO_SOLUTION:
            strcpy(application.job.policy,"min_energy");
        break;
        case MIN_TIME_TO_SOLUTION:
            strcpy(application.job.policy,"min_time");
        break;
        case MONITORING_ONLY:
            strcpy(application.job.policy,"monitoring_only");
        break;
    }
}

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
int policy_global_configuration(int p_state)
{
#if SHARED_MEMORY
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
	}else return p_state;
#else 
	return p_state;
#endif
}

// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
void policy_global_reconfiguration()
{
#if SHARED_MEMORY
	if (system_conf!=NULL){
	VERBOSE_N(0,"policy_global_reconfiguration max %lu def %lu th %.2lf\n",
	system_conf->max_freq,system_conf->def_freq,system_conf->th);
	switch (power_model_policy){
	case MIN_ENERGY_TO_SOLUTION:
	    // We filter initial configuration
	    if (system_conf->max_freq<EAR_default_frequency){
	        ear_verbose(0,"EAR max freq set to %lu because of power capping policies \n",system_conf->max_freq);
			EAR_default_frequency=system_conf->max_freq;
	       	EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);
	    }
		break;
	case MIN_TIME_TO_SOLUTION:
    case MONITORING_ONLY:
		if (system_conf->def_freq!=EAR_default_frequency){
	        ear_verbose(0,"EAR def freq set to %lu because of power capping policies \n",system_conf->def_freq);
			EAR_default_frequency=system_conf->def_freq;
	       	EAR_default_pstate=frequency_freq_to_pstate(EAR_default_frequency);
		}
	}
    if (performance_gain<system_conf->th){
        ear_verbose(0,"EAR min perf. efficiency th set to %lf because of power capping policies \n",system_conf->th);
        performance_gain=system_conf->th;
    }
	}
#else
	EAR_default_frequency=frequency_pstate_to_freq(EAR_default_pstate);
#endif
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_min_pstate()
{
#if SHARED_MEMORY
    if (system_conf!=NULL) return frequency_freq_to_pstate(system_conf->max_freq);
	else return 1;
#else
	return 1;
#endif
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_def_pstate()
{
#if SHARED_MEMORY
    if (system_conf!=NULL) return frequency_freq_to_pstate(system_conf->def_freq);
	else return EAR_default_pstate;
#else
    return EAR_default_pstate;
#endif
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
ulong get_global_def_freq()
{
#if SHARED_MEMORY
    return system_conf->def_freq;
#else
    return EAR_default_pstate;
#endif
}

double get_global_th()
{
	switch (power_model_policy){
		case MIN_TIME_TO_SOLUTION:
			#if SHARED_MEMORY
				if (system_conf!=NULL) return system_conf->th;
				else return performance_gain;
			#else
				return performance_gain;
			#endif
			break;
		case MIN_ENERGY_TO_SOLUTION:
			return performance_penalty;
			break;
		case MONITORING_ONLY:
			return 0.0;
	}
}





void init_power_policy()
{
	int min_p_state=0;
	unsigned long def_freq;

	ear_verbose(2,"EAR(%s): EAR_init_power_policy\n",__FILE__);
	
	power_model_policy = get_ear_power_policy();

	if (power_model_policy==MIN_ENERGY_TO_SOLUTION) performance_penalty=get_ear_power_policy_th();
	else if (power_model_policy==MIN_TIME_TO_SOLUTION) performance_gain=get_ear_power_policy_th();	

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
		ear_verbose(0,"ear: warning max freq is limited by the system, using %u as default\n",
					def_freq);

		EAR_default_frequency = def_freq;
	}
	print_energy_policy_configuration();
	init_policy_functions();
}

void init_power_models(unsigned int p_states, unsigned long *p_states_list)
{
	char coeff_file[128];
	char coeff_default_file[128];
	char coeff_file_fn[128];
	char nodename[128];
	int begin_pstate, end_pstate;
	int i, ref;

	ear_debug(3, "EAR(%s): EAR_Init_power_models p_states=%u\n", __FILE__, p_states);

	// Initializations
	begin_pstate = 1;
	ear_models_pstates = p_states;

	// Coefficient file
	strcpy(coeff_file, get_ear_coeff_db_pathname());
	gethostname(nodename, sizeof(nodename));

	// Default coefficient file
	sprintf(coeff_default_file, "%sdefault", coeff_file);

	sprintf(coeff_file, "%s%s", coeff_file, nodename);

	ear_verbose(2, "EAR: Using coefficients %s\n", coeff_file);


	// Coefficient pointers allocation
	coefficients = (coefficient_t **) malloc(sizeof(coefficient_t *) * p_states);

	if (coefficients == NULL) {
		ear_verbose(0, "EAR: Error allocating memory for p_states coefficients\n");
		exit(1);
	}

	for (i = 0; i < p_states; i++)
	{
		coefficients[i] = (coefficient_t *) malloc(sizeof(coefficient_t) * p_states);
		if (coefficients[i] == NULL) {
			ear_verbose(0,"EAR: Error allocating memory for p_states coefficients fn %d\n",i);
			exit(1);
		}

		for (ref = 0; ref < p_states; ref++)
		{
			coefficients[i][ref].pstate = p_states_list[ref];
			coefficients[i][ref].available = 0;
		}
	}

	create_projections(p_states);
	//
	reset_performance_projection(p_states);

	// Coefficient pointers allocation and reading
	int size, state;

	if (ear_use_turbo) begin_pstate = 0;
	end_pstate = p_states;

	for (ref = begin_pstate; ref < end_pstate; ref++)
	{
		sprintf(coeff_file_fn, "%s.%d", coeff_file, p_states_list[ref]);

		ear_verbose(2, "EAR: Opening (per node) coefficient file %s\n", coeff_file_fn);

		size = sizeof(coefficient_t) * p_states;
		state = read_coefficients_file(coeff_file_fn, &coefficients[ref], size);

		if (state == EAR_FILE_NOT_FOUND)
		{
			sprintf(coeff_file_fn, "%s.%d", coeff_default_file, p_states_list[ref]);
			ear_verbose(2, "EAR: Opening (default) coefficient file %s\n", coeff_file_fn);

			state = read_coefficients_file(coeff_file_fn, &coefficients[ref], size);
		}

		// If default coefficient file not found
		if (state == EAR_FILE_NOT_FOUND) {
			ear_verbose(2, "EAR: Coefficients are not available per node %s and freq %u\n",
						nodename, p_states_list[ref]);
		}

		// If other king of error during the reading
		if (state == EAR_ALLOC_ERROR || state == EAR_READ_ERROR) {
			ear_verbose(0, "EAR: Error while reading coefficients for %s (%s) (%d)\n",
						coeff_file_fn, strerror(errno), state);
			exit(1);
		}
	}
	app_policy.init(p_states);
}




uint policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{
	return app_policy.policy_ok(proj, curr_sig,last_sig);
}



// When 'evaluating signature', this function is called.
unsigned long policy_power(unsigned int whole_app, signature_t* MY_SIGNATURE)
{
	unsigned long optimal_freq, max_freq;

	if (whole_app) return ear_frequency;
	optimal_freq = app_policy.policy(MY_SIGNATURE);

	if (optimal_freq != ear_frequency)
	{
		ear_debug(3,"EAR(%s):: Changing Frequency to %u at the beggining of iteration\n",
				  __FILE__,optimal_freq);

		ear_frequency = max_freq = eards_change_freq(optimal_freq);

		if (max_freq != optimal_freq) {
			optimal_freq = max_freq;
		}
	} else {
		ear_debug(4,"EAR(%s):: %u selected, no changes are required\n",__FILE__,optimal_freq);
	}

	return optimal_freq;

}

void policy_new_loop()
{
	app_policy.new_loop();
}

void policy_end_loop()
{
    app_policy.end_loop();
}

void policy_default_configuration()
{
	ear_frequency=app_policy.default_conf(user_selected_freq);
	ear_verbose(0,"Going to default frequency %lu\n",ear_frequency);	
	eards_change_freq(ear_frequency);
}

