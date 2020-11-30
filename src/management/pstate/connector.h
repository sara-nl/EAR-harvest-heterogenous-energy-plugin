/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#ifndef MANAGEMENT_PSTATE_CONNECTOR
#define MANAGEMENT_PSTATE_CONNECTOR

#define _GNU_SOURCE
#include <sched.h>
#include <management/pstate/pstate.h>

typedef struct governor{
	char name[128];
	ulong max_f;
	ulong min_f;
} governor_t;

state_t frequency_init(uint x);
state_t frequency_dispose();

uint frequency_get_num_pstates();
ulong frequency_get_cpu_freq(uint cpu);
ulong frequency_get_nominal_freq();
ulong frequency_get_nominal_pstate();
ulong *frequency_get_freq_rank_list();
ulong frequency_set_all_cpus(ulong freq_khz);
ulong frequency_set_with_mask(cpu_set_t *mask, ulong freq);
ulong frequency_set_with_list(uint cpus, ulong *cpuf);
ulong frequency_pstate_to_freq(uint pstate_index);
uint frequency_freq_to_pstate(ulong freq_khz);
ulong frequency_pstate_to_freq_list(uint pstate_index, ulong *list, uint pstate_count);
uint frequency_freq_to_pstate_list(ulong freq_khz, ulong *list, uint pstate_count);
void frequency_set_performance_governor_all_cpus();
void frequency_set_userspace_governor_all_cpus();
int frequency_is_valid_frequency(ulong freq_khz);
int frequency_is_valid_pstate(uint pstate);
uint frequency_closest_pstate(ulong freq_khz);
ulong frequency_closest_frequency(ulong freq_khz);
ulong frequency_closest_high_freq(ulong freq_khz, int pstate_minimum);

void get_governor(governor_t *_governor);
void set_governor(governor_t *_governor);

#endif //MANAGEMENT_PSTATE_CONNECTOR
