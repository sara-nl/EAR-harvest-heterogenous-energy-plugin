/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_CONTROL_FREQUENCY_H
#define EAR_CONTROL_FREQUENCY_H

int frequency_init();
void frequency_dispose();
uint frequency_get_num_freqs();
uint frequency_get_num_pstates();
uint frequency_get_num_online_cpus();
ulong frequency_get_cpu_freq(uint cpu);
ulong frequency_get_nominal_freq();
ulong *frequency_get_freq_rank_list();
ulong frequency_set_all_cpus(ulong freq);
ulong frequency_pstate_to_freq(uint pstate);
uint frequency_freq_to_pstate(ulong freq);
void frequency_set_performance_governor_all_cpus();
void frequency_set_userspace_governor_all_cpus();
void frequency_save_previous_configuration();
void frequency_recover_previous_configuration();

#endif //EAR_CONTROL_FREQUENCY_H
