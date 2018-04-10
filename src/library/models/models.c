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
#include <library/models/monitoring.h>
#include <library/models/min_energy.h>
#include <library/models/min_time.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/states.h>
#include <common/config.h>

typedef struct policy
{
	void (*init)(uint num_pstates);
	void (*new_loop)();
	void (*end_loop)();
	ulong (*policy)(application_t *sig);
	ulong (*policy_ok)(projection_t *proj, application_t *curr_sig, application_t *last_sig);
}policy_t;

policy_t app_policy;




static const char *__NAME__ = "MODELS";

// Policy
int power_model_policy = MIN_ENERGY_TO_SOLUTION;
double performance_penalty ;
double performance_gain ;

// Process
uint EAR_default_pstate;

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
            strcpy(application.policy,"min_energy");
        break;
        case MIN_TIME_TO_SOLUTION:
            strcpy(application.policy,"min_time");
        break;
        case MONITORING_ONLY:
            strcpy(application.policy,"monitoring_only");
        break;
    }

    ear_verbose(1,"EAR: power policy conf.: policy %s performance penalty %lf performance gain %lf\n",
                application.policy,performance_penalty,performance_gain);
    ear_verbose(1,"EAR: Default p_state %u Default frequency %lu\n", EAR_default_pstate,EAR_default_frequency);

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
        break;
        case MIN_TIME_TO_SOLUTION:
			app_policy.init=min_time_init;
			app_policy.new_loop=min_time_new_loop;
			app_policy.end_loop=min_time_end_loop;
			app_policy.policy=min_time_policy;
			app_policy.policy_ok=min_time_policy_ok;
        break;
        case MONITORING_ONLY:
			app_policy.init=monitoring_init;
			app_policy.new_loop=monitoring_new_loop;
			app_policy.end_loop=monitoring_end_loop;
			app_policy.policy=monitoring_policy;
			app_policy.policy_ok=monitoring_policy_ok;
        break;
    }

}
// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
void policy_global_reconfiguration()
{
#if SHARED_MEMORY
    // We filter initial configuration
    if (system_conf->max_freq<frequency_pstate_to_freq(EAR_default_pstate)){
        ear_verbose(0,"EAR max freq set to %lu because of power capping policies \n",system_conf->max_freq);
        EAR_default_pstate=frequency_freq_to_pstate(system_conf->max_freq);
    }
    if (performance_gain<system_conf->th){
        ear_verbose(0,"EAR min perf. efficiency th set to %lf because of power capping policies \n",system_conf->th);
        performance_gain=system_conf->th;
    }
	EAR_default_frequency=frequency_pstate_to_freq(EAR_default_pstate);
#else
	EAR_default_frequency=frequency_pstate_to_freq(EAR_default_pstate);
#endif
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_min_pstate()
{
#if SHARED_MEMORY
    frequency_freq_to_pstate(system_conf->max_freq);
#else
	return 1;
#endif
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

	user_selected_freq = frequency_pstate_to_freq(EAR_default_pstate);
	ear_verbose(0,"User selected freq is %lu\n",user_selected_freq);
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

	// EAR_USER_DB_PATHNAME
	sprintf(coeff_file, "%s%s", coeff_file, nodename);

	ear_verbose(2, "EAR: Using coefficients %s\n", coeff_file);

	// Default coefficient file
	sprintf(coeff_default_file, "%s.default", coeff_file);

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




uint policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig)
{
	return app_policy.policy_ok(proj, curr_sig,last_sig);
}



// When 'evaluating signature', this function is called.
unsigned long policy_power(unsigned int whole_app, application_t* MY_SIGNATURE)
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

