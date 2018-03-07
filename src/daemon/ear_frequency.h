/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _ear_CPUFREQ_H
#define _ear_CPUFREQ_H
// Initializes information concerning list of available frequencies and hardware
// architectures. Current cpu governor is saved.
int ear_cpufreq_init();
// cpu governor is restored
void ear_cpufreq_end();
//Selects the “userspace” governor.
void ear_set_userspace();
// Sets newfreq frequency for all cpus in current node. Returns old frequency on
// success and 0 on error
ulong ear_cpufreq_set_node(ulong newfreq);
// Returns the total number of p_states in current node
uint  ear_get_num_p_states();
// Returns the frequency associated with a given pstate
ulong ear_get_freq(uint i);
//Selects the “performance” governor.
void ear_set_turbo();
// Start computing the avg freq for the whole application
void ear_begin_app_compute_turbo_freq();
// Ends computing the avg freq for the whole application
ulong ear_end_app_compute_turbo_freq();
// Computing the effective frequency when turbo is on needs root privileges.
// This function starts counters to compute frequency.
void ear_begin_compute_turbo_freq();
// Stops counters to compute effective frequency when turbo is on and returns
// the computed frequency.
ulong ear_end_compute_turbo_freq();
#else
#endif
