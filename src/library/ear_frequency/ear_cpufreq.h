/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _ear_CPUFREQ_H
#define _ear_CPUFREQ_H
//Initializes information concerning list of available frequencies and hardware architectures. Current cpu governor is saved.
void ear_cpufreq_init();
//cpu governor is restored
void ear_cpufreq_end();
//Returns cpu frequency for cpuid
unsigned long ear_cpufreq_get(unsigned int cpuid);
//Sets newfreq frequency for cpuid
unsigned long ear_cpufreq_set(unsigned int cpuid,unsigned long newfreq);
//Sets newfreq frequency for all cpus in current node
unsigned long ear_cpufreq_set_node(unsigned long newfreq);
//Returns nominal frequency for current node
unsigned long ear_get_nominal_frequency();
//Returns the total number of p_states in current node
unsigned int  ear_get_num_p_states();
//Returns a pointer to the first elements of the vector frequencies. (from 0..(pstates-1))
unsigned long *ear_get_pstate_list();
//Returns the frequency associated with a given pstate
unsigned long ear_get_freq(unsigned int i);
// Returns the CPU frequency for cpuid 0
unsigned long ear_my_frequency();
//Returns the maximum frequency in the current node
unsigned long ear_max_f();
//Returns the minimum frequency in the current node
unsigned long ear_min_f();
#define NOMINAL_F0 ear_get_nominal_frequency()
// Returns the pstate associated with a given frequency
unsigned int ear_get_pstate(unsigned long f);
#else
#endif
