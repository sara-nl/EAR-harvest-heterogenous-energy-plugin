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
#include <nvml.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <common/output/debug.h>
#include <common/system/symplug.h>
#include <common/config/config_env.h>
#include <management/gpu/archs/nvml.h>

#define NVML_N 15

static const char *nvml_names[] =
{
	"nvmlInit_v2",
	"nvmlDeviceGetCount_v2",
	"nvmlDeviceGetHandleByIndex_v2",
	"nvmlDeviceGetDefaultApplicationsClock",
	"nvmlDeviceGetSupportedMemoryClocks",
	"nvmlDeviceGetSupportedGraphicsClocks",
	"nvmlDeviceSetGpuLockedClocks",
	"nvmlDeviceSetApplicationsClocks",
	"nvmlDeviceResetApplicationsClocks",
	"nvmlDeviceResetGpuLockedClocks",
	"nvmlDeviceGetPowerManagementLimit",
	"nvmlDeviceGetPowerManagementDefaultLimit",
	"nvmlDeviceGetPowerManagementLimitConstraints",
	"nvmlDeviceSetPowerManagementLimit",
	"nvmlErrorString",
};

static struct nvml_s
{
	nvmlReturn_t (*Init)				(void);
	nvmlReturn_t (*Count)				(uint *devCount);
	nvmlReturn_t (*Handle)				(uint index, nvmlDevice_t *device);
	nvmlReturn_t (*GetDefaultAppsClock)	(nvmlDevice_t dev, nvmlClockType_t clockType, uint* mhz);
	nvmlReturn_t (*GetMemoryClocks)		(nvmlDevice_t dev, uint *count, uint *mhz);
	nvmlReturn_t (*GetGraphicsClocks)	(nvmlDevice_t dev, uint mem_mhz, uint *count, uint *mhz);
	nvmlReturn_t (*SetLockedClocks)		(nvmlDevice_t dev, uint min_mhz, uint max_mhz);
	nvmlReturn_t (*SetAppsClocks)		(nvmlDevice_t dev, uint mem_mgz, uint gpu_mhz);
	nvmlReturn_t (*ResetAppsClocks)		(nvmlDevice_t dev);
	nvmlReturn_t (*ResetLockedClocks)	(nvmlDevice_t dev);
	nvmlReturn_t (*GetPowerLimit)		(nvmlDevice_t dev, uint *watts);
	nvmlReturn_t (*GetPowerDefaultLimit)(nvmlDevice_t dev, uint *watts);
	nvmlReturn_t (*GetPowerLimitConstraints)(nvmlDevice_t dev, uint *min_watts, uint *max_watts);
	nvmlReturn_t (*SetPowerLimit)		(nvmlDevice_t dev, uint watts);
	char*        (*ErrorString)			(nvmlReturn_t result);
} nvml;

static struct error_s {
	char *init;
	char *init_not;
	char *null_data;
	char *gpus_not;
	char *dlopen;
} Error = {
	.init         = "context already initialized or not empty",
	.init_not     = "context not initialized",
	.null_data    = "data pointer is NULL",
	.gpus_not     = "no GPUs detected",
	.dlopen       = "error during dlopen",
};

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint			 initialized;
static uint			 dev_count;
static nvmlDevice_t *devices;
static uint			*clock_errors;
static uint 		*clock_max_default; // KHz
static uint 		*clock_max_current; // KHz
static uint			*clock_max; // KHz
static uint			*clock_max_mem; // KHz
static uint			*power_errors;
static uint			*power_max_default; // W
static uint			*power_max; // W

#define myErrorString(r) \
	((char *) nvml.ErrorString(r))

static state_t static_init()
{
	nvmlReturn_t r;

	/*
	 * Initialization
	 */
	if ((r = nvml.Init()) != NVML_SUCCESS) {
		return_msg(EAR_ERROR, (char *) nvml.ErrorString(r));
	}
	if ((r = nvml.Count(&dev_count)) != NVML_SUCCESS) {
		return_msg(EAR_ERROR, (char *) nvml.ErrorString(r));
	}
	if (((int) dev_count) <= 0) {
		return_msg(EAR_ERROR, Error.gpus_not);
	}

	/*
	 * Allocation
	 */
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
	if (clock_max_mem == NULL) {
		clock_max_mem = calloc(dev_count, sizeof(uint));
	}
	if (clock_max_mem == NULL) {
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

	/*
	 * Fill
	 */
	uint clock_list1[1000];
	uint clock_list2[1000];
	uint c1, c2, aux;
	int i0, i1, i2;

	// Getting devices
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvml.Handle(i0, &devices[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetHandleByIndex returned %d (%s)",
				  r, nvml.ErrorString(r));
			return_msg(EAR_ERROR, myErrorString(r));
		}
	}

	// Full clocks reset (application + locked)
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvml.ResetAppsClocks(devices[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceResetApplicationsClocks(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		if ((r = nvml.ResetLockedClocks(devices[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceResetGpuLockedClocks(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
	}

	// Power limit reset
	for (i0 = 0; i0 < dev_count; ++i0) {
		if ((r = nvml.GetPowerDefaultLimit(devices[i0], &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementDefaultLimit(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		if ((r = nvml.SetPowerLimit(devices[i0], aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceSetPowerManagementLimit(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
	}


	// Clocks fill
	debug("clock list:");

	for (i0 = 0; i0 < dev_count; ++i0)
	{
		clock_max_default[i0] = 0;
		clock_max[i0]         = 0;

		//Retrieving default application clocks
		if ((r = nvml.GetDefaultAppsClock(devices[i0], NVML_CLOCK_GRAPHICS, &clock_max_default[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetDefaultApplicationsClock(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}

		clock_max_default[i0] *= 1000;
		clock_max_current[i0]  = clock_max_default[i0];
		debug("D%d current: %d KHz", i0, clock_max_current[i0]);
		debug("D%d default: %d KHz", i0, clock_max_default[i0]);

		/*
		 * Retrieving a list of clock P_STATEs and saving its maximum.
		 */
		c1 = 1000;
		if ((r = nvml.GetMemoryClocks(devices[i0], &c1, clock_list1)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetSupportedMemoryClocks(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		for (i1 = 0; i1 < c1; ++i1)
		{
			if (clock_list1[i1] > clock_max_mem[i0]) {
				clock_max_mem[i0] = clock_list1[i1];
			}
			//debug("D%d MEM %d: %d", i0, i1, clock_list1[i1]);

			c2 = 1000;
			if ((r = nvml.GetGraphicsClocks(devices[i0], clock_list1[i1], &c2, clock_list2)) != NVML_SUCCESS) {
				debug("\t\tnvmlDeviceGetSupportedGraphicsClocks(dev: %d) returned %d (%s)",
					  i0, r, nvml.ErrorString(r));
				//return_msg(EAR_ERROR, myErrorString(r));
			}

			for (i2 = 0; i2 < c2; ++i2) {
				if (clock_list2[i2] > clock_max[i0]) {
					debug("D%d new max: %d MHz", i0, clock_list2[i2]);
					clock_max[i0] = clock_list2[i2];
				} else {
					//debug("D%d: %d", i0, clock_list2[i2]);
				}
			}
		}
		clock_max[i0] *= 1000;
	}

	// Power fill
	debug("power list:");
	
	for (i0 = 0; i0 < dev_count; ++i0)
	{
		if ((r = nvml.GetPowerDefaultLimit(devices[i0], &power_max_default[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementDefaultLimit returned %d (%s)",
				  r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		if ((r = nvml.GetPowerLimitConstraints(devices[i0], &aux, &power_max[i0])) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementLimitConstraints returned %d (%s)",
				  r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		power_max_default[i0] /= 1000;
		power_max[i0]         /= 1000;
		debug("D%d default: %d W", i0, power_max_default[i0]);
		debug("D%d new max: %d W", i0, power_max[i0]);
	}

	return EAR_SUCCESS;
}

static int load_test(char *path)
{
	void **p = (void **) &nvml;
	void *libnvml;
	int error;
	int i;

	//
	debug("trying to access to '%s'", path);
	if (access(path, X_OK) != 0) {
		return 0;
	}
	if ((libnvml = dlopen(path, RTLD_NOW | RTLD_LOCAL)) == NULL) {
		debug("dlopen fail");
		return 0;
	}
	debug("dlopen ok");

	//
	symplug_join(libnvml, (void **) &nvml, nvml_names, NVML_N);

	for(i = 0, error = 0; i < NVML_N; ++i) {
		debug("symbol %s: %d", nvml_names[i], (p[i] != NULL));
		error += (p[i] == NULL);
	}
	if (error > 0) {
		memset((void *) &nvml, 0, sizeof(nvml));
		dlclose(libnvml);
		return 0;
	}

	return 1;
}

static int static_load()
{
	if (load_test(getenv(HACK_FILE_NVML))) return 1;
	if (load_test(CUDA_BASE "/targets/x86_64-linux/lib/libnvidia-ml.so")) return 1;
	if (load_test(CUDA_BASE "/lib64/libnvidia-ml.so")) return 1;
	if (load_test(CUDA_BASE "/lib/libnvidia-ml.so")) return 1;
	if (load_test("/usr/lib64/libnvidia-ml.so")) return 1;
	if (load_test("/usr/lib/libnvidia-ml.so")) return 1;
	return 0;
}

static state_t nvml_init_prime()
{
	state_t s;
	while (pthread_mutex_trylock(&lock));
	if (initialized) {
		pthread_mutex_unlock(&lock);
		return EAR_SUCCESS;
	}
	if (!static_load()) {
		pthread_mutex_unlock(&lock);
		return_msg(EAR_ERROR, Error.dlopen);
	}
	dev_count = 0;
	if (xtate_fail(s, static_init())) {
		pthread_mutex_unlock(&lock);
		return s;
	}
	initialized = 1;
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t mgt_nvml_status()
{
	return mgt_nvml_init(NULL);
}

state_t mgt_nvml_init(ctx_t *c)
{
	return mgt_nvml_count(NULL, NULL);
}

state_t mgt_nvml_dispose(ctx_t *c)
{
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	return EAR_SUCCESS;
}

state_t mgt_nvml_count(ctx_t *c, uint *_dev_count)
{
	state_t s;

	if (_dev_count != NULL) {
		*_dev_count = 0;
	}
	if (!initialized) {
		if (xtate_fail(s, nvml_init_prime())) {
			return s;
		}
	}
	if (_dev_count != NULL) {
		*_dev_count = dev_count;
	}

	return EAR_SUCCESS;
}

state_t nvml_clock_limit_get_current(ctx_t *c, uint *khz)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max_current[i];
	}
	return EAR_SUCCESS;
}

state_t nvml_clock_limit_get_default(ctx_t *c, uint *khz)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max_default[i];
	}
	return EAR_SUCCESS;
}

state_t nvml_clock_limit_get_max(ctx_t *c, uint *khz)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count; ++i) {
		khz[i] = clock_max[i];
	}
	return EAR_SUCCESS;
}

static state_t clocks_reset(int i)
{
	nvmlReturn_t r;
	debug("resetting clocks of device %d", i);
	if ((r = nvml.ResetLockedClocks(devices[i])) != NVML_SUCCESS)
	{
		debug("nvmlDeviceResetGpuLockedClocks(dev: %d) returned %d (%s)",
			  i, r, nvml.ErrorString(r));
		
		if ((r = nvml.ResetAppsClocks(devices[i])) != NVML_SUCCESS) {
			return_msg(EAR_ERROR, myErrorString(r));
		}
	}
	clock_max_current[i] = clock_max_default[i];
	return EAR_SUCCESS;
}

state_t nvml_clock_limit_reset(ctx_t *c)
{
	state_t s, e;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, clocks_reset(i))) {
			e = s;
		}
	}
	return e;
}

static state_t clocks_set(int i, ulong mhz)
{
	nvmlReturn_t r;
	debug("D%d setting clock to %lu KHz", i, mhz * 1000);
	if ((r = nvml.SetLockedClocks(devices[i], 0, mhz)) != NVML_SUCCESS)
	{
		debug("nvmlDeviceSetGpuLockedClocks(dev: %d) returned %d (%s)",
			i, r, nvml.ErrorString(r));

		if ((r = nvml.SetAppsClocks(devices[i], clock_max_mem[i], mhz)) != NVML_SUCCESS)
		{
			debug("nvmlDeviceSetApplicationsClocks(dev: %d) returned %d (%s)",
				i, r, nvml.ErrorString(r));
			// Unlike POWER functions, which have a function ask about the current
			// POWER LIMIT value, CLOCK functions does not have that possibilty. So
			// If the CLOCK SET function fails, it will try to set the default clock
			// frequency, which we already know its value.
			clocks_reset(i);
			//
			return_msg(EAR_ERROR, myErrorString(r));
		}
	}
	clock_max_current[i] = mhz * 1000;
	return EAR_SUCCESS;
}

state_t nvml_clock_limit_set(ctx_t *c, ulong *khz)
{
	state_t s, e;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	debug("nvml_clock_limit_set devices %d",dev_count);
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, clocks_set(i, khz[i] / 1000u))) {
			e = s;
		}
	}
	return e;
}

state_t nvml_power_limit_get_current(ctx_t *c, uint *watts)
{
	nvmlReturn_t r;
	state_t e;
	uint aux;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if ((r = nvml.GetPowerLimit(devices[i], &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementLimit(dev: %d) returned %d (%s)",
				  i, r, nvml.ErrorString(r));
			// Setting error and unknown value parameter
			watts[i]  = 0;
			state_msg = (char *) nvml.ErrorString(r);
			e         = EAR_ERROR;
		}
		watts[i] = aux / 1000u;
	}
	return e;
}

state_t nvml_power_limit_get_default(ctx_t *c, uint *watts)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count; ++i) {
		watts[i] = power_max_default[i];
	}
	return EAR_SUCCESS;
}

state_t nvml_power_limit_get_max(ctx_t *c, uint *watts)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count; ++i) {
		watts[i] = power_max[i];
	}
	return EAR_SUCCESS;
}

static state_t powers_set(int i, uint mw)
{
	nvmlReturn_t r;
	
	debug("D%d setting power to %u mW", i, mw);
	if ((r = nvml.SetPowerLimit(devices[i], mw)) != NVML_SUCCESS)
	{
		debug("nvmlDeviceSetPowerManagementLimit(dev: %d) returned %d (%s)",
			  i, r, nvml.ErrorString(r));
		return_msg(EAR_ERROR, myErrorString(r));
	}
	return EAR_SUCCESS;
}

state_t nvml_power_limit_reset(ctx_t *c)
{
	return nvml_power_limit_set(c, power_max_default);
}

state_t nvml_power_limit_set(ctx_t *c, uint *watts)
{
	state_t s, e;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, powers_set(i, watts[i] * 1000))) {
			e = s;
		}
	}
	return e;
}
