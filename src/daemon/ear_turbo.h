/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#define MSR_IA32_APERF 0x000000E8
#define MSR_IA32_MPERF 0x000000E7
#define EAPERF -1
// These functions are used during the execution
int aperf_get_avg_frequency_init(unsigned int cpu);
int aperf_get_avg_frequency_end(unsigned int cpu, unsigned long *frequency);

// This functions are used to report the avg. freq for the whole app.
int aperf_start_computing_app_avg_freq(unsigned int cpu);
int aperf_end_computing_app_avg_freq(unsigned int cpu, unsigned long *frequency);


int aperf_init(unsigned int num_cpus);
int aperf_init_cpu(unsigned int cpu,unsigned long max_freq);
void aperf_dispose();
