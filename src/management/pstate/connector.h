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

// Initialized in: ear.c/eard.c/dvfs.c/
state_t frequency_init(uint x);
state_t frequency_dispose();

// Virtualizable. Init required. | Not used without initialization.
uint frequency_get_num_pstates();
// Daemon required. Init required. | Not used without initialization.
ulong frequency_get_cpu_freq(uint cpu);
// Virtualizable. Init required. | Not used without initialization
ulong frequency_get_nominal_freq();
// Virtualizable. Init required. | Not used without initialization.
ulong frequency_get_nominal_pstate();
// Virtualizable. Init required. | Not used without initialization.
ulong *frequency_get_freq_rank_list();
// NEW
ulong frequency_set_cpu(ulong freq_khz, uint cpu);
// Daemon required. Init required. | Not used without initialization.
ulong frequency_set_all_cpus(ulong freq_khz);
// Daemon required. Init required | Not used without initialization.
ulong frequency_set_with_mask(cpu_set_t *mask, ulong freq);
// Daemon required. Init required. | Not used without initialization.
ulong frequency_set_with_list(uint cpus, ulong *cpuf);
// Virtualizable. Init required. | Not used without initialization.
ulong frequency_pstate_to_freq(uint pstate_index);
// Virtualizable. Init required. | Not used without initialization.
uint frequency_freq_to_pstate(ulong freq_khz);
// Init not required.
ulong frequency_pstate_to_freq_list(uint pstate_index, ulong *list, uint pstate_count);
// Init not required.
uint frequency_freq_to_pstate_list(ulong freq_khz, ulong *list, uint pstate_count);
// Daemon required. Init required. | Not used without initialization.
void frequency_set_performance_governor_all_cpus();
// Daemon required. Init required. | Not used without initialization.
void frequency_set_userspace_governor_all_cpus();
// Virtualizable. Init required. | Not used without initialization.
int frequency_is_valid_frequency(ulong freq_khz);
// Virtualizable. Init required. | Not used without initialization.
int frequency_is_valid_pstate(uint pstate);
// Virtualizable. Init required. | Not used without initialization.
uint frequency_closest_pstate(ulong freq_khz);
// Virtualizable. Init required. | Not used without initialization.
ulong frequency_closest_frequency(ulong freq_khz);
// Virtualizable. Init required. | Not used without initialization.
ulong frequency_closest_high_freq(ulong freq_khz, int pstate_minimum);

// Virtualizable. Init required. | Not used without initialization.
void get_governor(governor_t *_governor);
// Daemon required. Init required. | Not used without initialization.

void set_governor(governor_t *_governor);

#endif //MANAGEMENT_PSTATE_CONNECTOR
