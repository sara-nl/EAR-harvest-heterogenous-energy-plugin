/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <common/output/debug.h>
#include <management/gpu/archs/nvml.h>

#define NVML_N 7

static struct error_s {
	char *init;
	char *init_not;
	char *null_context;
	char *null_data;
	char *gpus_not;
	char *dlopen;
} Error = {
	.init         = "context already initialized or not empty",
	.init_not     = "context not initialized",
	.null_context = "context pointer is NULL",
	.null_data    = "data pointer is NULL",
	.gpus_not     = "no GPUs detected",
	.dlopen       = "error during dlopen",
};

static uint			 initialized;
static uint			 allocated;
static uint			 dev_count;
static nvmlDevice_t *devices;
static uint 		*clock_max_default;
static uint 		*clock_max_current;
static uint			*clock_max;
static uint			*power_max_default;
static uint			*power_max;

state_t management_nvml_status()
{
	return nvml_status();
}

static state_t static_alloc()
{
	debug("TEMPORAL");

	if (devices == NULL) {
		devices = calloc(dev_count, sizeof(nvmlDevice_t));
	}
	if (devices == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	if (clock_max_current == NULL) {
		clock_max_current = calloc(dev_count, sizeof(uint));
	}
	if (clock_max_current == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	if (clock_max_default == NULL) {
		clock_max_default = calloc(dev_count, sizeof(uint));
	}
	if (clock_max_default == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	if (clock_max == NULL) {
		clock_max = calloc(dev_count, sizeof(uint));
	}
	if (clock_max == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	if (power_max_default == NULL) {
		power_max_default = calloc(dev_count, sizeof(uint));
	}
	if (power_max_default == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	if (power_max == NULL) {
		power_max = calloc(dev_count, sizeof(uint));
	}
	if (power_max == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}

	//
	nvmlReturn_t r;
	uint clock_list1[1000];
	uint clock_list2[1000];
	uint c1, c2, aux;
	int i0, i1, i2;

	// Getting devices
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvmlDeviceGetHandleByIndex(i0, &device[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetHandleByIndex returned %d (%s)",
				  r, nvmlErrorString(r));
			return_msg(EAR_ERROR, nvmlErrorString(r));
		}
	}

	// Full clocks reset (application + locked)
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvmlDeviceResetApplicationsClocks(devices[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceResetApplicationsClocks(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}
		if ((r = nvmlDeviceResetGpuLockedClocks(devices[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceResetGpuLockedClocks(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}
	}

	// Power limit reset
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvmlDeviceGetPowerManagementDefaultLimit(device[i0], &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementDefaultLimit(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}
		if ((r = nvmlDeviceSetPowerManagementLimit(devices[i0], aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceSetPowerManagementLimit(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}
	}

	// Clocks fill
	for (i0 = 0; i0 < dev_count; ++i0)
	{
		clock_max_default[i0] = 0;
		clock_max[i0]         = 0;

		//Retrieving default application clocks
		if ((r = nvmlDeviceGetDefaultApplicationsClock(devices[i0], &clock_max_default[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetDefaultApplicationsClock(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}

		/*
		 * Retrieving a list of clock P_STATEs and saving its maximum.
		 */
		c1 = 1000;
		if ((r = nvmlDeviceGetSupportedMemoryClocks(device[i0], &c1, clock_list1)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetSupportedMemoryClocks(dev: %d) returned %d (%s)",
				  i0, r, nvmlErrorString(r));
		}
		for (i1 = 0; i1 < c1; ++i1)
		{
			debug("\tM%d: %d", i1, clock_list1[i1]);

			c2 = 1000;
			if ((r = nvmlDeviceGetSupportedGraphicsClocks(device, clock_list1[i1], &c2, clock_list2)) != NVML_SUCCESS) {
				debug("\t\tnvmlDeviceGetSupportedGraphicsClocks(dev: %d) returned %d (%s)",
					  i0, r, nvmlErrorString(r));
			}

			for (i2 = 0; i2 < c2; ++i2) {
				if (khz[i0] > clock_list2[i2]) {
					debug("\t\tP%d: %d (selected)", i2, clock_list2[i2]);
					clock_max[i0] = clock_list2[i2];
				} else {
					debug("\t\tP%d: %d", i2, clock_list2[i2]);
				}
			}
		}
	}

	// Power fill
	for (i0 = 0; i0 < dev_count; ++i0)
	{
		if ((r = nvmlDeviceGetPowerManagementDefaultLimit(device, &power_max_default[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementDefaultLimit returned %d (%s)",
				  r, nvmlErrorString(r));
			return_msg(EAR_ERROR, nvmlErrorString(r));
		}
		if ((r = nvmlDeviceGetPowerManagementLimitConstraints(device, &aux, &power_max[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementLimitConstraints returned %d (%s)",
				  r, nvmlErrorString(r));
			return_msg(EAR_ERROR, nvmlErrorString(r));
		}
	}

	return EAR_SUCCESS;
}

state_t management_nvml_init(ctx_t *c)
{
	debug("TEMPORAL");

	state_t s;
	if (initialized) {
		return EAR_SUCCESS;
	}
	if (xtate_fail(s, nvml_count(c, &dev_count))) {
		return s;
	}
	if (xtate_fail(s, static_alloc())) {
		return s;
	}
	initialized = 1;

	return EAR_SUCCESS;
}

state_t nvml_clock_get_limit_current(ctx_t *c, uint *khz)
{
	debug("TEMPORAL");

	int i;
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max_current[i] * 1000u;
	}
	return EAR_SUCCESS;
}

state_t nvml_clock_get_limit_default(ctx_t *c, uint *khz)
{
	debug("TEMPORAL");

	int i;
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max_default[i] * 1000u;
	}
	return EAR_SUCCESS;
}

state_t nvml_clock_get_limit_max(ctx_t *c, uint *khz)
{
	debug("TEMPORAL");

	int i;
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max[i] * 1000u;
	}
	return EAR_SUCCESS;
}

static state_t clocks_reset(int i)
{
	debug("TEMPORAL");

	nvmlReturn_t r;
	if ((r = nvmlDeviceResetGpuLockedClocks(devices[i])) != NVML_SUCCESS)
	{
		debug("nvmlDeviceResetGpuLockedClocks(dev: %d) returned %d (%s)",
			  i, r, nvmlErrorString(r));
		return_msg(EAR_ERROR, nvmlErrorString(r));
	}
	clock_max_current[i] = clock_max_default[i];
	return EAR_SUCCESS;
}

state_t nvml_clock_reset(ctx_t *c)
{
	debug("TEMPORAL");

	state_t s, e;

	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, clocks_reset(i))) {
			e = s;
		}
	}
	return e;
}

static state_t clocks_set(int i, uint mhz)
{
	debug("TEMPORAL");

	nvmlReturn_t r;
	char e;
	if ((r = nvmlDeviceSetGpuLockedClocks(devices[i], 0, mhz)) != NVML_SUCCESS)
	{
		debug("nvmlDeviceSetApplicationsClocks(dev: %d) returned %d (%s)",
			  i, r, nvmlErrorString(r));
		// Unlike POWER functions, which have a function ask about the current
		// POWER LIMIT value, CLOCK functions does not have that possibilty. So
		// If the CLOCK SET function fails, it will try to set the default clock
		// frequency, which we already know its value.
		clocks_reset(i);
		//
		return_msg(EAR_ERROR, nvmlErrorString(r));
	}
	clock_max_current[i] = mhz;
	return EAR_SUCCESS;
}

state_t nvml_clock_set_limit(ctx_t *c, uint *khz)
{
	debug("TEMPORAL");

	state_t s, e;
	int i;

	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, clocks_set(i, khz[i] / 1000u))) {
			e = s;
		}
	}
	return e;
}

state_t nvml_power_get_limit_current(ctx_t *c, uint *watts)
{
	debug("TEMPORAL");

	nvmlReturn_t r;
	state_t s, e;
	uint aux;
	int i;

	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if ((r = nvmlDeviceGetPowerManagementLimit(devices[i], &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementLimit(dev: %d) returned %d (%s)",
				  i, r, nvmlErrorString(r));
			// Setting error and unknown value parameter
			watts[i]  = 0;
			state_msg = nvmlErrorString(r);
			e         = EAR_ERROR;
		}
		watts[i] = aux / 1000u;
	}
	return e;
}

state_t nvml_power_get_limit_default(ctx_t *c, uint *watts)
{
	debug("TEMPORAL");

	int i;
	for (i = 0; i < dev_count; ++i) {
		watts[i] = power_max_default[i] / 1000;
	}
	return EAR_SUCCESS;
}

state_t nvml_power_get_limit_max(ctx_t *c, uint *watts)
{
	debug("TEMPORAL");

	int i;
	for (i = 0; i < dev_count; ++i) {
		watts[i] = power_max[i] / 1000;
	}
	return EAR_SUCCESS;
}

static state_t powers_set(int i, uint mw)
{
	debug("TEMPORAL");

	nvmlReturn_t r;

	if ((r = nvmlDeviceSetPowerManagementLimit(devices[i], mw)) != NVML_SUCCESS)
	{
		debug("nvmlDeviceSetPowerManagementLimit(dev: %d) returned %d (%s)",
			  i, r, nvmlErrorString(r));
		return_msg(EAR_ERROR, nvmlErrorString(r));
	}
	return EAR_SUCCESS;
}

state_t nvml_power_limit_reset(ctx_t *c)
{
	debug("TEMPORAL");

	return nvml_power_set_max(c, power_max_default);
}

state_t nvml_power_limit_set(ctx_t *c, uint *watts)
{
	debug("TEMPORAL");

	nvmlReturn_t r;
	state_t s, e;
	int i;

	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, powers_set(i, watts[i] * 1000))) {
			e = s;
		}
	}
	return e;
}