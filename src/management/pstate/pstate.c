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

#define SHOW_DEBUGS 1

#include <common/sizes.h>
#include <common/plugins.h>
#include <common/output/debug.h>
#include <management/pstate/pstate.h>
#include <management/pstate/archs/amd17.h>
#include <management/pstate/archs/dummy.h>
#include <management/pstate/drivers/cpufreq.h>
#include <metrics/frequency/cpu.h>

static mgt_ps_driver_ops_t ops_driver;
static mgt_ps_ops_t        ops;

static state_t load_driver_cpufreq(topology_t *tp)
{
   state_t s;

    if (xtate_fail(s, pstate_cpufreq_status(tp))) {
    	debug("pstate_cpufreq_status returned (%d, '%s')", s, state_msg);
		return s;
	}

	ops_driver.init               = pstate_cpufreq_init;
	ops_driver.dispose            = pstate_cpufreq_dispose;
	ops_driver.get_available_list = pstate_cpufreq_get_available_list;
	ops_driver.get_current_list   = pstate_cpufreq_get_current_list;
	ops_driver.get_governor       = pstate_cpufreq_get_governor;
	ops_driver.set_current_list   = pstate_cpufreq_set_current_list;
	ops_driver.set_current        = pstate_cpufreq_set_current;
	ops_driver.set_governor       = pstate_cpufreq_set_governor;
	
	return EAR_SUCCESS;
}

static state_t load_amd17(topology_t *tp)
{
	state_t s;

	if (xtate_fail(s, pstate_amd17_status(tp))) {
		debug("pstate_amd17_status returned (%d, '%s')", s, state_msg);
		return s;
	}
	
	ops.init               = pstate_amd17_init;
	ops.dispose            = pstate_amd17_dispose;
	ops.count              = pstate_amd17_count;
	ops.get_available_list = pstate_amd17_get_available_list;
	ops.get_current_list   = pstate_amd17_get_current_list;
	ops.get_nominal        = pstate_amd17_get_nominal;
	ops.get_governor       = pstate_amd17_get_governor;
	ops.get_index          = pstate_amd17_get_index;
	ops.set_current_list   = pstate_amd17_set_current_list;
	ops.set_current        = pstate_amd17_set_current;
	ops.set_governor       = pstate_amd17_set_governor;
	return EAR_SUCCESS;
}

static state_t load_dummy(topology_t *tp)
{
	state_t s;

	if (xtate_fail(s, pstate_dummy_status(tp))) {
		debug("pstate_dummy_status returned (%d, '%s')", s, state_msg);
		return s;
	}

	ops.init               = pstate_dummy_init;
	ops.dispose            = pstate_dummy_dispose;
	ops.count              = pstate_dummy_count;
	ops.get_available_list = pstate_dummy_get_available_list;
	ops.get_current_list   = pstate_dummy_get_current_list;
	ops.get_nominal        = pstate_dummy_get_nominal;
	ops.get_governor       = pstate_dummy_get_governor;
	ops.get_index          = pstate_dummy_get_index;
	ops.set_current_list   = pstate_dummy_set_current_list;
	ops.set_current        = pstate_dummy_set_current;
	ops.set_governor       = pstate_dummy_set_governor;
	return EAR_SUCCESS;
}

state_t mgt_pstate_load(topology_t *tp)
{
	// Thread locking required

	if (state_fail(load_driver_cpufreq(tp))) {
		// Look for another driver
	}
	if (ops_driver.init == NULL) {
		return_msg(EAR_ERROR, Generr.api_incompatible);
	}
	// Los multi pstates funcionan siempre que el thread y el core compartan frecuencia
	if (state_fail(load_amd17(tp))) {
		if (state_fail(load_dummy(tp))) {
			// Look for another architecture
		}
	}
	if (ops.init == NULL) {
		return_msg(EAR_ERROR, Generr.api_incompatible);
	}

	return EAR_SUCCESS;
}

state_t mgt_pstate_init(ctx_t *c)
{
	preturn (ops.init, c, &ops_driver);
}

state_t mgt_pstate_dispose(ctx_t *c)
{
	preturn (ops.dispose, c);
}

/** Getters */
state_t mgt_pstate_count(ctx_t *c, uint *pstate_count)
{
	preturn (ops.count, c, pstate_count);
}

state_t mgt_pstate_get_available_list(ctx_t *c, pstate_t *pstate_list, uint *pstate_count)
{
	preturn (ops.get_available_list, c, pstate_list, pstate_count);
}

state_t mgt_pstate_get_current_list(ctx_t *c, pstate_t *pstate_list)
{
	preturn (ops.get_current_list, c, pstate_list);
}

state_t mgt_pstate_get_nominal(ctx_t *c, uint *pstate_index)
{
	preturn (ops.get_nominal, c, pstate_index);
}

state_t mgt_pstate_get_governor(ctx_t *c, uint *governor)
{
	preturn (ops.get_governor, c, governor);
}

state_t mgt_pstate_get_index(ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest)
{
	preturn (ops.get_index, c, freq_khz, pstate_index, closest);
}

/** Setters */
state_t mgt_pstate_set_current_list(ctx_t *c, uint *pstate_index)
{
	preturn (ops.set_current_list, c, pstate_index);
}

state_t mgt_pstate_set_current(ctx_t *c, uint pstate_index)
{
	preturn (ops.set_current, c, pstate_index);
}

state_t mgt_pstate_set_governor(ctx_t *c, uint governor)
{
	preturn (ops.set_governor, c, governor);
}

state_t mgt_pstate_governor_tostr(uint governor, char *buffer)
{
	if (governor == Governor.conservative) {
		sprintf(buffer, Goverstr.conservative);
	} else if (governor == Governor.performance) {
		sprintf(buffer, Goverstr.performance);
	} else if (governor == Governor.userspace) {
		sprintf(buffer, Goverstr.userspace);
	} else if (governor == Governor.powersave) {
		sprintf(buffer, Goverstr.powersave);
	} else if (governor == Governor.ondemand) {
		sprintf(buffer, Goverstr.ondemand);
	} else {
		sprintf(buffer, Goverstr.other);
		return_msg(EAR_ERROR, "undefined governor");
	}
	return EAR_SUCCESS;
}

state_t mgt_pstate_governor_toint(char *buffer, uint *governor)
{
	if (strncmp(buffer, Goverstr.conservative, 12) == 0) {
		*governor = Governor.conservative;
	} else if (strncmp(buffer, Goverstr.performance, 11) == 0) {
		*governor = Governor.performance;
	} else if (strncmp(buffer, Goverstr.userspace, 9) == 0) {
		*governor = Governor.userspace;
	} else if (strncmp(buffer, Goverstr.powersave, 9) == 0) {
		*governor = Governor.powersave;
	} else if (strncmp(buffer, Goverstr.ondemand, 8) == 0) {
		*governor = Governor.ondemand;
	} else {
		*governor = Governor.other;
		return_msg(EAR_ERROR, "undefined governor");
	}
	return EAR_SUCCESS;
}

#ifdef MAINN
char opt1[SZ_PATH];
char cmnd[SZ_PATH];
pstate_t pstate_list[128];
uint pstate_count;
uint governor;
uint nominal;
topology_t topo;
state_t s;
ctx_t c;
int i;

freq_cpu_t freq1;
freq_cpu_t freq2;
ulong *freqs;
ulong freqs_count;

int is(char *buf, char *string)
{
    return strcmp(buf, string) == 0;
}

int main(int argc, char *argv[])
{
	state_assert(s, topology_init(&topo),   return 0);
	state_assert(s, mgt_pstate_load(&topo), return 0);

	state_assert(s, freq_cpu_init(&topo),   return 0);
	state_assert(s, freq_cpu_data_alloc(&freq1,   NULL,         NULL), return 0);
	state_assert(s, freq_cpu_data_alloc(&freq2, &freqs, &freqs_count), return 0);

    while (1)
    {
        debug("command: ");
        scanf("%s", cmnd);

        if (is(cmnd, "init")) {
			debug("initializing...");
			state_assert(s, mgt_pstate_init(&c),);
		} else if (is(cmnd, "dispose")) {
			debug("disposing...");
			state_assert(s, mgt_pstate_dispose(&c),);
		} else if (is(cmnd, "get_governor")) {
			state_assert(s, mgt_pstate_get_governor(&c, &governor),);
			debug("getting governor... %u", governor);
		} else if (is(cmnd, "set_governor")) {
 			scanf("%s", opt1);
			state_assert(s, mgt_pstate_set_governor(&c, (uint) atoi(opt1)),);
			debug("getting governor... %u", governor);
		} else if (is(cmnd, "get_available_list")) {
			state_assert(s, mgt_pstate_get_available_list(&c, pstate_list, &pstate_count),);
			for (i = 0; i < pstate_count; ++i) {
				debug("avail P%llu: %llu KHz, %llu MHz", pstate_list[i].idx, pstate_list[i].khz, pstate_list[i].mhz);
			}
		} else if (is(cmnd, "get_current_list")) {
			state_assert(s, mgt_pstate_get_current_list(&c, pstate_list),);
			for (i = 0; i < topo.cpu_count; ++i) {
				debug("current CPU%d: P%llu, %llu KHz, %llu MHz", i, pstate_list[i].idx, pstate_list[i].khz, pstate_list[i].mhz);
			}
		} else if (is(cmnd, "set_current")) {
 			scanf("%s", opt1);
			state_assert(s, mgt_pstate_set_current(&c, (uint) atoi(opt1)),);
		} else if (is(cmnd, "set_current_list")) {
		} else if (is(cmnd, "get_nominal")) {
			state_assert(s, mgt_pstate_get_nominal(&c, &nominal),);
			debug("nominal P_STATE is %u", nominal);
		} else if (is(cmnd, "test")) {
			state_assert(s, freq_cpu_read(&freq1),);
			sleep(2);
			state_assert(s, freq_cpu_read_diff(&freq2, &freq1, freqs, NULL),);
			for (i = 0; i < freqs_count; ++i) {
				debug("aperf freqs CPU%d: %lu", i, freqs[i]);
			}
		} else {
			debug("command not found");
		}
	}

	return 0;
}
#endif
