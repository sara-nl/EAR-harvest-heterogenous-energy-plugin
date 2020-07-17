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

#define NVML_N 17

const char *nvml_names[] =
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
	"nvmlDeviceGetClock",
	"nvmlDeviceGetMaxClockInfo", 
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
	nvmlReturn_t (*GetClock)			(nvmlDevice_t device, nvmlClockType_t clockType, nvmlClockId_t clockId, uint* mhz);
	nvmlReturn_t (*GetMaxClock)			(nvmlDevice_t device, nvmlClockType_t clockType, uint* mhz);
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
static uint			  initialized;
static uint			  dev_count;
static nvmlDevice_t  *devices;
static ulong 		 *clock_max_default; // KHz
static ulong 		 *clock_max_current; // KHz
static ulong		 *clock_max; // KHz
static ulong		 *clock_max_mem; // MHz
static ulong		**clock_list; // MHz
static uint			 *clock_lens;
static ulong		 *power_max_default; // W
static ulong		 *power_max; // W
static ulong		 *power_min; // W

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
	#define static_alloc(v, l, s) \
		if ((v = calloc(l, s)) == NULL) { return_msg(EAR_SYSCALL_ERROR, strerror(errno)); }

	static_alloc(devices,           dev_count, sizeof(nvmlDevice_t));
	static_alloc(clock_max_current, dev_count, sizeof(ulong));
	static_alloc(clock_max_default, dev_count, sizeof(ulong));
	static_alloc(clock_max,         dev_count, sizeof(ulong));
	static_alloc(clock_max_mem,     dev_count, sizeof(ulong));
	static_alloc(power_max_default, dev_count, sizeof(ulong));
	static_alloc(power_max,         dev_count, sizeof(ulong));
	static_alloc(power_min,         dev_count, sizeof(ulong));
	static_alloc(clock_list,        dev_count, sizeof(ulong *));
	static_alloc(clock_lens,        dev_count, sizeof(uint));

	/*
	 * Fill
	 */
	uint aux_mem[1000];
	uint aux_gpu[1000];
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
			return_msg(EAR_ERROR, myErrorString(r));
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
			return_msg(EAR_ERROR, myErrorString(r));
		}
		if ((r = nvml.SetPowerLimit(devices[i0], aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceSetPowerManagementLimit(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			return_msg(EAR_ERROR, myErrorString(r));
		}
	}

	/*
	 *
	 * Clocks fill
	 *
	 */
	debug("clock list:");

	for (i0 = 0; i0 < dev_count; ++i0)
	{
		clock_max_default[i0] = 0LU;
		clock_max[i0]         = 0LU;

		//Retrieving default application clocks
		if ((r = nvml.GetDefaultAppsClock(devices[i0], NVML_CLOCK_GRAPHICS, &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetDefaultApplicationsClock(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			return_msg(EAR_ERROR, myErrorString(r));
		}

		clock_max_default[i0] = ((ulong) aux) * 1000LU;
		clock_max_current[i0] = clock_max_default[i0];
		debug("D%d current: %lu KHz", i0, clock_max_current[i0]);
		debug("D%d default: %lu KHz", i0, clock_max_default[i0]);

		/*
		 * Retrieving a list of clock P_STATEs and saving its maximum.
		 */
		c1 = 1000;
		if ((r = nvml.GetMemoryClocks(devices[i0], &c1, aux_mem)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetSupportedMemoryClocks(dev: %d) returned %d (%s)",
				  i0, r, nvml.ErrorString(r));
			return_msg(EAR_ERROR, myErrorString(r));
		}
		for (i1 = 0; i1 < c1; ++i1)
		{
			if (aux_mem[i1] > clock_max_mem[i0]) {
				clock_max_mem[i0] = (ulong) aux_mem[i1];
			}
			debug("D%d MEM %d: %d (%u)", i0, i1, aux_mem[i1], c1);

			c2 = 1000;
			if ((r = nvml.GetGraphicsClocks(devices[i0], aux_mem[i1], &c2, aux_gpu)) != NVML_SUCCESS) {
				debug("\t\tnvmlDeviceGetSupportedGraphicsClocks(dev: %d) returned %d (%s)",
					  i0, r, nvml.ErrorString(r));
				return_msg(EAR_ERROR, myErrorString(r));
			}

			for (i2 = 0; i2 < c2; ++i2) {
				if (aux_gpu[i2] > clock_max[i0]) {
					debug("D%d new max: %d MHz", i0, aux_gpu[i2]);
					clock_max[i0] = (ulong) aux_gpu[i2];
				} else {
					//debug("D%d: %d", i0, aux_gpu[i2]);
				}
			}
		}
		clock_max[i0] *= 1000LU;
	}

	/*
	 *
	 * Power fill
	 *
	 */
	debug("power list:");
	
	for (i0 = 0; i0 < dev_count; ++i0)
	{
		if ((r = nvml.GetPowerDefaultLimit(devices[i0], &aux)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementDefaultLimit returned %d (%s)",
				  r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		if ((r = nvml.GetPowerLimitConstraints(devices[i0], &c1, &c2)) != NVML_SUCCESS) {
			debug("nvmlDeviceGetPowerManagementLimitConstraints returned %d (%s)",
				  r, nvml.ErrorString(r));
			//return_msg(EAR_ERROR, myErrorString(r));
		}
		power_max_default[i0] = ((ulong) aux) / 1000LU;
		power_max[i0] = ((ulong) c2) / 1000LU;
		power_min[i0] = ((ulong) c1) / 1000LU;
		debug("D%d default: %lu W", i0, power_max_default[i0]);
		debug("D%d new max: %lu W", i0, power_max[i0]);
		debug("D%d new min: %lu W", i0, power_min[i0]);
	}

	/*
 	 *
 	 * Clock list allocation
 	 *
	 */
	for (i0 = 0; i0 < dev_count; ++i0)
	{
		clock_lens[i0] = 1000;

		if ((r = nvml.GetGraphicsClocks(devices[i0], clock_max_mem[i0], &clock_lens[i0], aux_gpu)) != NVML_SUCCESS) {
			clock_lens[i0] = 0;
			return_msg(EAR_ERROR, myErrorString(r));
		}

		clock_list[i0] = calloc(clock_lens[i0], sizeof(ulong *));

		for (i1 = 0; i1 < clock_lens[i0]; ++i1) {
			clock_list[i0][i1] = (ulong) aux_gpu[i1];
			//debug("D%u,i%u: %lu", d, i, clock_list[i0][i]);
		}
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

state_t nvml_clock_cap_get_current(ctx_t *c, ulong *khz)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	uint mhz;
	for (i = 0; i < dev_count; ++i)
	{
		//khz[i] = clock_max_current[i];
		//nvml.GetClock(devices[i], NVML_CLOCK_GRAPHICS, NVML_CLOCK_ID_CURRENT, &mhz);
		//fprintf(stderr, "NVML_CLOCK_ID_CURRENT %u\n", mhz*1000);
		nvml.GetClock(devices[i], NVML_CLOCK_GRAPHICS, NVML_CLOCK_ID_APP_CLOCK_TARGET, &mhz);
		fprintf(stderr, "NVML_CLOCK_ID_APP_CLOCK_TARGET %u\n", mhz*1000U);
		khz[i] = ((ulong) mhz) * 1000LU;
		//nvml.GetClock(devices[i], NVML_CLOCK_GRAPHICS, NVML_CLOCK_ID_APP_CLOCK_DEFAULT, &mhz);
		//fprintf(stderr, "NVML_CLOCK_ID_APP_CLOCK_DEFAULT %u\n", mhz*1000);
		//nvml.GetMaxClock(devices[i], NVML_CLOCK_GRAPHICS, &mhz);
		//fprintf(stderr, "nvmlDeviceGetMaxClockInfo %u\n", mhz*1000);
	}
	return EAR_SUCCESS;
}

state_t nvml_clock_cap_get_default(ctx_t *c, ulong *khz)
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

state_t nvml_clock_cap_get_max(ctx_t *c, ulong *khz)
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

	#if 0
	if ((r = nvml.ResetLockedClocks(devices[i])) != NVML_SUCCESS)
	{
		debug("nvmlDeviceResetGpuLockedClocks(dev: %d) returned %d (%s)",
			  i, r, nvml.ErrorString(r));
	#endif
		if ((r = nvml.ResetAppsClocks(devices[i])) != NVML_SUCCESS) {
			return_msg(EAR_ERROR, myErrorString(r));
		}
	#if 0
	}
	#endif
	clock_max_current[i] = clock_max_default[i];
	return EAR_SUCCESS;
}

state_t nvml_clock_cap_reset(ctx_t *c)
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

static uint clocks_find(uint d, uint mhz)
{
	ulong *_clock_list = clock_list[d];
	uint   _clock_lens = clock_lens[d];
	uint mhz0;
	uint mhz1;
	uint i;

	if (mhz > (uint) _clock_list[0]) {
		return (uint) _clock_list[0];
	}

	for (i = 0; i < _clock_lens-1; ++i)
	{
		mhz0 = (uint) _clock_list[i+0];
		mhz1 = (uint) _clock_list[i+1];
		//debug("mhz0 %u, mhz1 %u, mhz %u", mhz0, mhz1, mhz);

		if (mhz <= mhz0 && mhz >= mhz1) {
			if ((mhz0 - mhz) <= (mhz - mhz1)) {
				return mhz0;
			} else {
				return mhz1;
			}
		}
	}

	return (uint) _clock_list[i];
}

static state_t clocks_set(int i, uint mhz)
{
	nvmlReturn_t r;
	uint parsed_mhz;

	parsed_mhz = clocks_find(i, mhz);

	debug("D%d setting clock %u KHz (parsed => %u)", i, mhz * 1000, parsed_mhz * 1000);
#if 0
	if ((r = nvml.SetLockedClocks(devices[i], 0, parsed_mhz)) != NVML_SUCCESS)
	{
		debug("nvmlDeviceSetGpuLockedClocks(dev: %d) returned %d (%s)",
			i, r, nvml.ErrorString(r));
#endif
		if ((r = nvml.SetAppsClocks(devices[i], clock_max_mem[i], parsed_mhz)) != NVML_SUCCESS)
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
#if 0
	}
#endif
	clock_max_current[i] = ((ulong) parsed_mhz) * 1000LU;
	return EAR_SUCCESS;
}

state_t nvml_clock_cap_set(ctx_t *c, ulong *khz)
{
	state_t s, e;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, clocks_set(i, (uint) (khz[i] / 1000LU)))) {
			e = s;
		}
	}
	return e;
}

state_t nvml_clock_list(ctx_t *c, ulong ***list_khz, uint **list_len)
{
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	if (list_khz != NULL) {
		*list_khz = clock_list;
	}
	if (list_len != NULL) {
		*list_len = clock_lens;
	}
	return EAR_SUCCESS;
}

state_t nvml_power_cap_get_current(ctx_t *c, ulong *watts)
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

state_t nvml_power_cap_get_default(ctx_t *c, ulong *watts)
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

state_t nvml_power_cap_get_rank(ctx_t *c, ulong *watts_min, ulong *watts_max)
{
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0; i < dev_count && watts_max != NULL; ++i) {
		watts_max[i] = power_max[i];
	}
	for (i = 0; i < dev_count && watts_min != NULL; ++i) {
		watts_min[i] = power_min[i];
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

state_t nvml_power_cap_reset(ctx_t *c)
{
	return nvml_power_cap_set(c, power_max_default);
}

state_t nvml_power_cap_set(ctx_t *c, ulong *watts)
{
	state_t s, e;
	int i;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	for (i = 0, e = EAR_SUCCESS; i < dev_count; ++i) {
		if (xtate_fail(s, powers_set(i, (uint) (watts[i] * 1000LU)))) {
			e = s;
		}
	}
	return e;
}
