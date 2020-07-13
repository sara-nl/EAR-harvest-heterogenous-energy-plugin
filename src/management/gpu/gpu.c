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

#include <common/output/debug.h>
#include <management/gpu/gpu.h>
#include <management/gpu/archs/nvml.h>
#include <management/gpu/archs/dummy.h>

static mgt_gpu_ops_t ops;
static uint loaded;

state_t mgt_gpu_load(mgt_gpu_ops_t **_ops)
{
	if (loaded != 0) {
		return EAR_SUCCESS;
	}
	#ifdef CUDA_BASE
	if (state_ok(mgt_nvml_status()))
	{
		debug("selected NVML");
		ops.init                    = mgt_nvml_init;
		ops.dispose                 = mgt_nvml_dispose;
		ops.count                   = mgt_nvml_count;
		ops.clock_limit_get_current = nvml_clock_limit_get_current;
		ops.clock_limit_get_default = nvml_clock_limit_get_default;
		ops.clock_limit_get_max     = nvml_clock_limit_get_max;
		ops.clock_limit_reset       = nvml_clock_limit_reset;
		ops.clock_limit_set         = nvml_clock_limit_set;
		ops.power_limit_get_current = nvml_power_limit_get_current;
		ops.power_limit_get_default = nvml_power_limit_get_default;
		ops.power_limit_get_max     = nvml_power_limit_get_max;
		ops.power_limit_reset       = nvml_power_limit_reset;
		ops.power_limit_set         = nvml_power_limit_set;
	} else
	#endif
	{
		debug("selected DUMMY");
		ops.init                    = mgt_dummy_init;
		ops.dispose                 = mgt_dummy_dispose;
		ops.count                   = mgt_dummy_count;
		ops.clock_limit_get_current = dummy_clock_limit_get_current;
		ops.clock_limit_get_default = dummy_clock_limit_get_default;
		ops.clock_limit_get_max     = dummy_clock_limit_get_max;
		ops.clock_limit_reset       = dummy_clock_limit_reset;
		ops.clock_limit_set         = dummy_clock_limit_set;
		ops.power_limit_get_current = dummy_power_limit_get_current;
		ops.power_limit_get_default = dummy_power_limit_get_default;
		ops.power_limit_get_max     = dummy_power_limit_get_max;
		ops.power_limit_reset       = dummy_power_limit_reset;
		ops.power_limit_set         = dummy_power_limit_set;
	}
	if (_ops != NULL) {
		*_ops = &ops;
	}
	// Dummy loads anyway
	loaded = 1;

	return EAR_SUCCESS;
}

state_t mgt_gpu_init(ctx_t *c)
{
	preturn (ops.init, c);
}

state_t mgt_gpu_dispose(ctx_t *c)
{
	preturn (ops.dispose, c);
}

state_t mgt_gpu_count(ctx_t *c, uint *dev_count)
{
	preturn (ops.count, c, dev_count);
}

state_t mgt_gpu_clock_limit_get_current(ctx_t *c, uint *khz)
{
	preturn (ops.clock_limit_get_current, c, khz);
}

state_t mgt_gpu_clock_limit_get_default(ctx_t *c, uint *khz)
{
	preturn (ops.clock_limit_get_default, c, khz);
}

state_t mgt_gpu_clock_limit_get_max(ctx_t *c, uint *khz)
{
	preturn (ops.clock_limit_get_max, c, khz);
}

state_t mgt_gpu_clock_limit_reset(ctx_t *c)
{
	preturn (ops.clock_limit_reset, c);
}

state_t mgt_gpu_clock_limit_set(ctx_t *c, uint *khz)
{
	preturn (ops.clock_limit_set, c, khz);
}

state_t mgt_gpu_power_limit_get_current(ctx_t *c, uint *watts)
{
	preturn (ops.power_limit_get_current, c, watts);
}

state_t mgt_gpu_power_limit_get_default(ctx_t *c, uint *watts)
{
	preturn (ops.power_limit_get_default, c, watts);
}

state_t mgt_gpu_power_limit_get_max(ctx_t *c, uint *watts)
{
	preturn (ops.power_limit_get_max, c, watts);
}

state_t mgt_gpu_power_limit_reset(ctx_t *c)
{
	preturn (ops.power_limit_reset, c);
}

state_t mgt_gpu_power_limit_set(ctx_t *c, uint *watts)
{
	preturn (ops.power_limit_set, c, watts);
}
