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

#ifndef MANAGEMENT_FREQUENCY_DRIVERS_CPUFREQ
#define MANAGEMENT_FREQUENCY_DRIVERS_CPUFREQ

#include <management/cpufreq/cpufreq.h>

state_t pstate_cpufreq_status(topology_t *tp);

state_t pstate_cpufreq_init(ctx_t *c);

state_t pstate_cpufreq_dispose(ctx_t *c);

/** Getters **/
state_t pstate_cpufreq_get_available_list(ctx_t *c, const ullong **freq_list, uint *freq_count);

state_t pstate_cpufreq_get_current_list(ctx_t *c, const ullong **freq_list);

state_t pstate_cpufreq_get_boost(ctx_t *c, uint *boost_enabled);

state_t pstate_cpufreq_get_governor(ctx_t *c, uint *governor);

/** Setters **/
state_t pstate_cpufreq_set_current_list(ctx_t *c, uint *freq_index);

//Sets a frequency in a specified CPU. Use all_cpus to set that P_STATE in all CPUs.
state_t pstate_cpufreq_set_current(ctx_t *c, uint freq_index, int cpu);

//You can recover the governor (and its frequency) by setting Governor.last and Governor.init.
state_t pstate_cpufreq_set_governor(ctx_t *c, uint governor);

#endif //MANAGEMENT_FREQUENCY_DRIVERS_CPUFREQ
