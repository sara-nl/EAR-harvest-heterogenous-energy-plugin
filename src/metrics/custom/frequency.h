/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_METRICS_FREQUENCY_H
#define EAR_METRICS_FREQUENCY_H

#define MSR_IA32_APERF 0x000000E8
#define MSR_IA32_MPERF 0x000000E7

/** Starts tracking frequency for the whole app in the selected CPU. Returns 0
*   on success and -1 on error. */
int aperf_get_global_avg_frequency_init(unsigned int cpu);
/** Ends the tracking previously started and puts the average frequency for the 
*   whole app during the period between this call and the preivous 
*   aperf_get_global_avg_frequency_init() call in *frequency. Returns 0 on
*   success and -1 on error. */
int aperf_get_global_avg_frequency_end(unsigned int cpu, unsigned long *frequency);

/** Starts tracking the frequency for all CPUs. */
void aperf_get_avg_frequency_init_all_cpus();
/** Starts tracking the frequency for all CPUs for the whole app. */
void aperf_get_global_avg_frequency_init_all_cpus();

// These functions are used during the execution
/** Starts tracking the frequency in the selected CPU. Returns 0 on success and -1
*   on error. */
int aperf_get_avg_frequency_init(unsigned int cpu);
/** Ends the tracking previously started and puts the average frequency during the
*   period between this call and the previous aperf_get_avg_frequency_init() and
*   puts it in *frequency. Returns 0 on success and -1 on error. */
int aperf_get_avg_frequency_end(unsigned int cpu, unsigned long *frequency);

/** Ends the tracking previously started with aperf_get_avg_frequency_init_all_cpus()
*   and returns the average frequency for all CPUs between this call and the previous
*   one. */ 
unsigned long aperf_get_avg_frequency_end_all_cpus();
/** Ends the tracking previously started with aperf_get_global_avg_frequency_init_all_cpus()
*   and returns the average frequency for all CPUs for the whole app between
*   this call and the previous one. */
unsigned long aperf_get_global_avg_frequency_end_all_cpus();

// Init & dispose functions
/** Allocates memory to hold the information of num_cpus CPUs regarding their frequency.
*   Returns 0 on success, -1 on error. */
int aperf_init(unsigned int num_cpus);
/** Initializes the frequency value of the CPU cpu at the value given by parameter. */
int aperf_init_cpu(unsigned int cpu, unsigned long max_freq);
/** Initializes the frequency value of all CPUs at the value given by max_freq. */
int aperf_init_all_cpus(unsigned int num_cpus, unsigned long max_freq);
/** Frees the memory allocated to hold the CPU information regarding their frequency. */
void aperf_dispose();

#endif
