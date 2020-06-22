/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

const char *nvml_names[] =
{
	"nvmlDeviceGetHandleByIndex_v2",
	//"nvmlDeviceSetGpuLockedClocks",
	"nvmlDeviceSetApplicationsClocks",
	"nvmlDeviceGetApplicationsClock",
	"nvmlDeviceResetGpuLockedClocks",
	"nvmlDeviceSetPowerManagementLimit",
	"nvmlDeviceGetPowerManagementLimit",
	"nvmlDeviceGetPowerManagementDefaultLimit",
	"nvmlDeviceGetPowerManagementLimitConstraints",
	"nvmlErrorString",
};

static struct nvml_s
{
	nvmlReturn_t (*DevHandle)		(uint index, nvmlDevice_t *dev);
	nvmlReturn_t (*DevSetClock)		(nvmlDevice_t dev, uint mhz_min, uint mhz_max);
	nvmlReturn_t (*DevGetClock)		(nvmlDevice_t dev, nvmlClockType_t type, uint mhz_max);
	nvmlReturn_t (*DevResetClock)	(nvmlDevice_t dev);
	nvmlReturn_t (*DevSetPower)		(nvmlDevice_t dev, uint limit);
	nvmlReturn_t (*DevGetPower)		(nvmlDevice_t dev, uint *limit);
	nvmlReturn_t (*DevGetPowerDef)	(nvmlDevice_t dev, uint *limit);
	nvmlReturn_t (*DevGetPowerCons)	(nvmlDevice_t dev, uint *mw_min, uint *mw_max);
	char* (*ErrorString) (nvmlReturn_t result);
} nvml;

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
static uint			 dev_count;
static nvmlDevice_t *devices;
static uint 		*clock_max_default;
static uint			*clock_max;

state_t management_nvml_status()
{
	return nvml_status();
}

static int load_test(void *libvml)
{
	void **p = (void **) &nvml;
	int error;
	int i;

	//
	symplug_join(libnvml, (void **) &nvml, nvml_names, NVML_N);

	for(i = 0, error = 0; i < NVML_N; ++i) {
		debug("symbol %s: %d", nvml_names[i], (p[i] != NULL));
		error += (p[i] == NULL);
	}
	if (error > 0) {
		memset((void *) &nvml, 0, sizeof(nvml_t));
		dlclose(libnvml);
		return 0;
	}

	return 1;
}

static int reset_clocks()
{
	nvmlDeviceGetDefaultApplicationsClock



	nvmlDevice_t device;
	uint clock_list1[1000];
	uint clock_list2[1000];
	int i0, i1, i2;
	uint c1, c2;

	for (i0 = 0; i0 < dev_count; ++i0)
	{
		clock_max_default[i0] = 0;
		clock_max[i0]         = 0;

		/*
		 * Resetting application and locked clocks.
		 */
		nvmlDeviceResetApplicationsClocks

		/*
		 * Retrieving a list of clock P_STATEs and saving its maximum.
		 */
		c1 = 1000;
		if ((r = nvmlDeviceGetSupportedMemoryClocks(device[i0], &c1, clock_list1)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetSupportedMemoryClocks returned %d (%s)",
				  r, nvmlErrorString(r));
			return 0;
		}
		for (i1 = 0; i1 < c1; ++i1)
		{
			debug("\tM%d: %d", i1, clock_list1[i1]);

			c2 = 1000;
			if ((r = nvmlDeviceGetSupportedGraphicsClocks(device, clock_list1[i1], &c2, clock_list2)) != NVML_SUCCESS) {
				debug("\t\tnvmlDeviceGetSupportedGraphicsClocks returned %d (%s)",
					  r, nvmlErrorString(r));
				return 0;
			}

			for (i2 = 0; i2 < c2; ++i2) {
				if (khz[i0] > clock_list2[i2]) {
					debug("\t\tP%d: %d (selected)", i2, clock_list2[i2]);
					khz[i0] = clock_list2[i2];
				} else {
					debug("\t\tP%d: %d", i2, clock_list2[i2]);
				}
			}
		}
	}
}

state_t management_nvml_init(ctx_t *c)
{
	state_t s = nvml_count(c, &dev_count);

	if (initialized) {
		return EAR_SUCCESS;
	}
	if (state_ok(s)) {
		if (!load_test(c->context)) {
			return_msg(EAR_ERROR, Error.dlopen);
		}
	}
	*clock_max = calloc(dev_count, sizeof(uint));
	if (*clock_max == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	devices = calloc(dev_count, sizeof(nvmlDevice_t));
	if (devices == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	for (i = 0; i < dev_count; ++i) {
		if ((r = nvmlDeviceGetHandleByIndex(i, &device[i])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetHandleByIndex returned %d (%s)",
				  r, nvmlErrorString(r));
			return_msg(EAR_ERROR, nvmlErrorString(r));
		}
	}
	initialized = 1;

	return EAR_SUCCESS;
}

state_t nvml_clock_set_limit(ctx_t *c, uint *khz)
{
	int i;

	for (i = 0; i < dev_count; ++i)
	{
		if ((r = nvmlDeviceSetGpuLockedClocks(devices[i], 0, khz[i])) != NVML_SUCCESS) {
			debug("nvmlDeviceSetApplicationsClocks returned %d (%s)",
				  r, nvml.ErrorString(r));
			continue;
		}
		clock_max[i] = khz[i];
	}

	return EAR_SUCCESS;
}

state_t nvml_clock_get_limit_current(ctx_t *c, uint *khz)
{

}

state_t nvml_clock_get_limit_max(ctx_t *c, uint *khz)
{


	return EAR_SUCCESS;
}

state_t nvml_clock_reset(ctx_t *c)
{
	nvml.DevResetClock();
}






state_t nvml_power_set_max(ctx_t *c, uint *watts)
{
	nvml.DevSetPower();
}

state_t nvml_power_get_max(ctx_t *c, uint *watts)
{
	nvml.DevGetPower();
}

state_t nvml_power_get_max_default(ctx_t *c, uint *watts)
{
	nvmlReturn_t r;
	int i;

	for (i = 0; i < dev_count; ++i)
	{
		nvmlDevice_t device;

		// Testing if all is right
		if ((r = nvml.DevHandle(i, &device)) != NVML_SUCCESS) {
			continue;
		}
		if ((r = nvml.DevGetPowerCons(i, watts)) != NVML_SUCCESS)
		{

		}
	}
}

state_t nvml_power_reset(ctx_t *c)
{
	// No reset function
	nvml.DevSetPower();
}