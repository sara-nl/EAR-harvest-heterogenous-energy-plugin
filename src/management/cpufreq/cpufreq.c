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

//#define SHOW_DEBUGS 1

#include <common/sizes.h>
#include <common/plugins.h>
#include <common/output/debug.h>
#include <management/cpufreq/cpufreq.h>
#include <management/cpufreq/archs/amd17.h>
#include <management/cpufreq/archs/default.h>
#include <management/cpufreq/drivers/linux_cpufreq.h>
#include <metrics/frequency/cpu.h>

static mgt_ps_driver_ops_t ops_driver;
static mgt_ps_ops_t        ops;
uint                       mgt_cpu_count;

static state_t load_driver_cpufreq(topology_t *tp)
{
   state_t s;

    if (xtate_fail(s, cpufreq_linux_status(tp))) {
    	debug("cpufreq_linux_status returned (%d, '%s')", s, state_msg);
		return s;
	}

	ops_driver.init               = cpufreq_linux_init;
	ops_driver.dispose            = cpufreq_linux_dispose;
	ops_driver.get_available_list = cpufreq_linux_get_available_list;
	ops_driver.get_current_list   = cpufreq_linux_get_current_list;
	ops_driver.get_boost          = cpufreq_linux_get_boost;
	ops_driver.get_governor       = cpufreq_linux_get_governor;
	ops_driver.set_current_list   = cpufreq_linux_set_current_list;
	ops_driver.set_current        = cpufreq_linux_set_current;
	ops_driver.set_governor       = cpufreq_linux_set_governor;
	
	return EAR_SUCCESS;
}

static state_t load_amd17(topology_t *tp)
{
	state_t s;

	if (xtate_fail(s, cpufreq_amd17_status(tp))) {
		debug("cpufreq_amd17_status returned (%d, '%s')", s, state_msg);
		return s;
	}
	
	ops.init               = cpufreq_amd17_init;
	ops.init_user          = cpufreq_amd17_init_user;
	ops.dispose            = cpufreq_amd17_dispose;
	ops.count              = cpufreq_amd17_count;
	ops.get_available_list = cpufreq_amd17_get_available_list;
	ops.get_current_list   = cpufreq_amd17_get_current_list;
	ops.get_nominal        = cpufreq_amd17_get_nominal;
	ops.get_governor       = cpufreq_amd17_get_governor;
	ops.get_index          = cpufreq_amd17_get_index;
	ops.set_current_list   = cpufreq_amd17_set_current_list;
	ops.set_current        = cpufreq_amd17_set_current;
	ops.set_governor       = cpufreq_amd17_set_governor;
	return EAR_SUCCESS;
}

static state_t load_dummy(topology_t *tp)
{
	state_t s;

	if (xtate_fail(s, cpufreq_default_status(tp))) {
		debug("cpufreq_default_status returned (%d, '%s')", s, state_msg);
		return s;
	}

	ops.init               = cpufreq_default_init;
	ops.init_user          = cpufreq_default_init_user;
	ops.dispose            = cpufreq_default_dispose;
	ops.count              = cpufreq_default_count;
	ops.get_available_list = cpufreq_default_get_available_list;
	ops.get_current_list   = cpufreq_default_get_current_list;
	ops.get_nominal        = cpufreq_default_get_nominal;
	ops.get_governor       = cpufreq_default_get_governor;
	ops.get_index          = cpufreq_default_get_index;
	ops.set_current_list   = cpufreq_default_set_current_list;
	ops.set_current        = cpufreq_default_set_current;
	ops.set_governor       = cpufreq_default_set_governor;
	return EAR_SUCCESS;
}

state_t mgt_cpufreq_load(topology_t *tp)
{
	// Thread locking required
	// Saving the number of CPUs because the EARD call seems to require it
	mgt_cpu_count = tp->cpu_count;
	//
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

state_t mgt_cpufreq_init(ctx_t *c)
{
	preturn (ops.init, c, &ops_driver);
}

state_t mgt_cpufreq_init_user(ctx_t *c, const ullong *freq_list, uint freq_count)
{
	preturn (ops.init_user, c, &ops_driver, freq_list, freq_count);
}

state_t mgt_cpufreq_dispose(ctx_t *c)
{
	preturn (ops.dispose, c);
}

/** Getters */
state_t mgt_cpufreq_count(ctx_t *c, uint *pstate_count)
{
	preturn (ops.count, c, pstate_count);
}

state_t mgt_cpufreq_get_available_list(ctx_t *c, pstate_t *pstate_list, uint *pstate_count)
{
	preturn (ops.get_available_list, c, pstate_list, pstate_count);
}

state_t mgt_cpufreq_get_current_list(ctx_t *c, pstate_t *pstate_list)
{
	preturn (ops.get_current_list, c, pstate_list);
}

state_t mgt_cpufreq_get_nominal(ctx_t *c, uint *pstate_index)
{
	preturn (ops.get_nominal, c, pstate_index);
}

state_t mgt_cpufreq_get_governor(ctx_t *c, uint *governor)
{
	preturn (ops.get_governor, c, governor);
}

state_t mgt_cpufreq_get_index(ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest)
{
	preturn (ops.get_index, c, freq_khz, pstate_index, closest);
}

/** Setters */
state_t mgt_cpufreq_set_current_list(ctx_t *c, uint *pstate_index)
{
	preturn (ops.set_current_list, c, pstate_index);
}

state_t mgt_cpufreq_set_current(ctx_t *c, uint pstate_index, int cpu)
{
	preturn (ops.set_current, c, pstate_index, cpu);
}

state_t mgt_cpufreq_set_governor(ctx_t *c, uint governor)
{
	preturn (ops.set_governor, c, governor);
}