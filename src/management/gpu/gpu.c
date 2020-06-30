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

#include <management/gpu/gpu.h>
#include <management/gpu/archs/nvml.h>

static struct mgt_gpu_ops_s
{
	state_t (*init)						(ctx_t *c);
	state_t (*dispose)					(ctx_t *c);
	state_t (*count)					(ctx_t *c, uint *dev_count);
	state_t (*clock_limit_get_current)	(ctx_t *c, uint *khz);
	state_t (*clock_limit_get_default)	(ctx_t *c, uint *khz);
	state_t (*clock_limit_get_max)		(ctx_t *c, uint *khz);
	state_t (*clock_limit_reset)		(ctx_t *c);
	state_t (*clock_limit_set)			(ctx_t *c, uint *khz);
	state_t (*power_limit_get_current)	(ctx_t *c, uint *watts);
	state_t (*power_limit_get_default)	(ctx_t *c, uint *watts);
	state_t (*power_limit_get_max)		(ctx_t *c, uint *watts);
	state_t (*power_limit_reset)		(ctx_t *c);
	state_t (*power_limit_set)			(ctx_t *c, uint *watts);
} ops;

state_t mgt_gpu_load()
{
	#ifdef CUDA_BASE
	if (state_ok(mgt_nvml_status()))
	{
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
		return EAR_SUCCESS;
	}
	#endif
	return EAR_ERROR;
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
