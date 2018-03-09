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

// This functions are used to report the avg. freq for the whole app.
int aperf_get_global_avg_frequency_init(unsigned int cpu);
int aperf_get_global_avg_frequency_end(unsigned int cpu, unsigned long *frequency);

void aperf_get_avg_frequency_init_all_cpus();
void aperf_get_global_avg_frequency_init_all_cpus();

// These functions are used during the execution
int aperf_get_avg_frequency_init(unsigned int cpu);
int aperf_get_avg_frequency_end(unsigned int cpu, unsigned long *frequency);

unsigned long aperf_get_avg_frequency_end_all_cpus();
unsigned long aperf_get_global_avg_frequency_end_all_cpus();

// Init & dispose functions
int aperf_init(unsigned int num_cpus);
int aperf_init_cpu(unsigned int cpu, unsigned long max_freq);
int aperf_init_all_cpus(unsigned int num_cpus, unsigned long max_freq);
void aperf_dispose();

#endif
