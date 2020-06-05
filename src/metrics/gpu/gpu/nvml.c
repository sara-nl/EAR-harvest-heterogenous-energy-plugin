/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#define SHOW_DEBUGS 1
#include <nvml.h>
#include <stdlib.h>
#include <common/types.h>
#include <common/monitor.h>
#include <common/states.h>
#include <common/output/debug.h>
#include <metrics/gpu/gpu/nvml.h>

static pthread_mutex_t lock;
static suscription_t *sus;
static uint dev_count;
static uint bursting;
static gpu_t *pool;

static struct error_s {
	char *init;
	char *init_not;
	char *null_context;
	char *null_data;
	char *gpus_not;
} Error = {
	.init         = "context already initialized or not empty",
	.init_not     = "context not initialized",
	.null_context = "context pointer is NULL",
	.null_data    = "data pointer is NULL",
	.gpus_not     = "no GPUs detected"
};

static state_t nvml_init()
{
	nvmlReturn_t r;
	while (pthread_mutex_trylock(&lock));
	if (initialized) {
		pthread_mutex_unlock(lock);
		return EAR_SUCCESS;
	}
	if ((r = nvmlInit()) != NVML_SUCCESS) {
		pthread_mutex_unlock(lock);
		return_msg(EAR_ERROR, (char *) nvmlErrorString(r));
	}
	initialized = 1;
	pthread_mutex_unlock(lock);
	return EAR_SUCCESS;
}

state_t nvml_status()
{
	nvmlReturn_t r;

	// DLOPEN
	if (xtate_fail(s, nvml_count(NULL, NULL))) {
		return s;
	}
	if (dev_count == 0) {
		return_msg(EAR_ERROR, Error.gpus_not);
	}
	return EAR_SUCCESS;
}

state_t nvml_init(ctx_t *c)
{
	nvmlReturn_t ret;
	state_t s;

	if ((r = nvml_init()) != NVML_SUCCESS) {
		return s;
	}
	if (initialized) {
		return EAR_SUCCESS;
	}
	if (xtate_fail(s, nvml_count(NULL, NULL))) {
		return s;
	}
	if (xtate_fail(s, nvml_data_alloc(&pool))) {
		return s;
	}
	// All is ok
	sus = suscription();
	sus.call_main  = nvml_pool;
	sus.memm_main  = c;
	sus.time_relax = 1000;
	sus.time_burst = 300;
	monitor_register(sus);
	
	return EAR_SUCCESS;
}

state_t nvml_dispose(ctx_t *c)
{
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	return EAR_SUCCESS;
}

state_t nvml_count(ctx_t *c, uint *_dev_count)
{
	nvmlReturn_t r;
	*_dev_count = 0;
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}
	while (pthread_mutex_trylock(&lock));
	if (dev_count != 0) {
		if( _dev_count != NULL) {
			*_dev_count = dev_count;
		}
		pthread_mutex_unlock(lock);
		return EAR_SUCCESS;
	}
	if ((r = nvmlDeviceGetCount_v2(&dev_count)) != NVML_SUCCESS) {
		pthread_mutex_unlock(lock);
		return_msg(EAR_ERROR, (char *) nvmlErrorString(r));
	}
	pthread_mutex_unlock(lock);
	if (((int) dev_count) <= 0) {
		return_msg(EAR_ERROR, Error.gpus_not);
	}
	if (_dev_count != NULL) {
		*_dev_count = dev_count;
	}
	return EAR_SUCCESS;
}

state_t nvml_pool(void *p)
{
	if (!initialized) {
		return_msg(EAR_NOT_INITIALIZED, Error.init_not);
	}

	//
	timestamp_t time;
	int working = 0;
	int i;

	// Lock
	while (pthread_mutex_trylock(lock));
	timestamp_getfast(&time);

	//
	for (i = 0; i < dev_count; ++i)
	{
		nvmlEnableState_t mode;
		nvmlDevice_t device;
		int s;

		// Testing if all is right
		if ((s = nvmlDeviceGetHandleByIndex(i, &device)) != NVML_SUCCESS) {
			continue;
		}
		if ((s = nvmlDeviceGetPowerManagementMode(device, &mode)) != NVML_SUCCESS) {
			continue;
		}
		if (mode != NVML_FEATURE_ENABLED) {
			continue;
		}

		nvmlProcessInfo_t procs[8];
		nvmlUtilization_t util;
		int proc_count = 8;
		uint freq_gpu_mhz;
		uint freq_mem_mhz;
		uint temp_gpu;
		uint power_mw;

		// Getting the metrics by calling NVML (no MEM temp)
		s = nvmlDeviceGetPowerUsage(device, &power_mw);
		s = nvmlDeviceGetClockInfo(device, NVML_CLOCK_MEM, &freq_mem_mhz);
		s = nvmlDeviceGetClockInfo(device, NVML_CLOCK_SM , &freq_gpu_mhz);
		s = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp_gpu);
		s = nvmlDeviceGetUtilizationRates(device, &util);
		s = nvmlDeviceGetComputeRunningProcesses(device, &proc_count, procs);

		// Pooling the data
		pool[i].samples      += 1;
		pool[i].time          = time;
		pool[i].freq_mem_mhz += (ulong) freq_mem_mhz;
		pool[i].freq_gpu_mhz += (ulong) freq_gpu_mhz;
		pool[i].util_mem     += (ulong) util.memory;
		pool[i].util_gpu     += (ulong) util.gpu;
		pool[i].temp_gpu     += (ulong) temp_gpu;
		pool[i].temp_mem      = 0;
		pool[i].energy_j      = 0;
		pool[i].power_w      += (double) power_mw;
		pool[i].working       = proc_count > 0;
		pool[i].correct       = 1;

		// Burst or not
		working += pool[i].working;
	}
		
	if (working  > 0 && !bursting) {
		monitor_burst(sus);
		bursting = 1;
	}
	if (working == 0 &&  bursting) {
		monitor_relax(sus);
		bursting = 0;
	}

	// Lock
	pthread_mutex_unlock(lock);

	return EAR_SUCCESS;
}

state_t nvml_read(ctx_t *c, gpu_t *data)
{
	return nvml_data_copy(data, pool);
}

state_t nvml_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	state_t s;
	if (xtate_fail(s, nvml_read(c, data2))) {
		return s;
	}
	if (xtate_fail(s, nvml_data_diff(data2, data1, data_diff))) {
		return s;
	}
	return nvml_data_copy(data1, data2);
}

static void nvml_read_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff, int i)
{
	gpu_t *d3 = &data_diff[i];
	gpu_t *d2 = &data2[i];
	gpu_t *d1 = &data1[i];
	ullong utime;
	double dtime;

	// Cleaning
	nvml_data_null(d3);

	if (d2->correct != 1 || d1->correct != 1) {
		return;
	}
	// Computing time
	time_i = timestamp_diff(&d2->time, &d1->time, TIME_USECS);
	time_f = ((double) time_i) / 1000000.0;
	// Averages
	d3->samples = d2->samples - d1->samples;

	if (d3->samples == 0) {
		memset(d3, 0, sizeof(gpu_t));
		return;
	}
	// No overflow control is required (64-bits are enough)
	d3->freq_gpu_mhz = (d2->freq_gpu_mhz - d1->freq_gpu_mhz) / d3->samples;
	d3->freq_mem_mhz = (d2->freq_mem_mhz - d1->freq_mem_mhz) / d3->samples;
	d3->util_gpu     = (d2->util_gpu     - d1->util_gpu)     / d3->samples;
	d3->util_mem     = (d2->util_mem     - d1->util_mem)     / d3->samples;
	d3->temp_gpu     = (d2->temp_gpu     - d1->temp_gpu)     / d3->samples;
	d3->temp_mem     = 0;
	d3->power_w      = (d2->power_w      - d1->power_w )     / (d3->samples * 1000);
	d3->energy_j     = (d2->power_w)     * time_f;
	d3->working      = (d2->working);
	//
	debug("%d freq gpu (MHz), %lu = %lu - %lu", i, d3->freq_gpu_mhz, d2->freq_gpu_mhz, d1->freq_gpu_mhz);
	debug("%d freq mem (MHz), %lu = %lu - %lu", i, d3->freq_mem_mhz, d2->freq_mem_mhz, d1->freq_mem_mhz);
	debug("%d power    (W)  , %0.2lf = %0.2lf - %0.2lf", i, d3->power_w, d2->power_w, d1->power_w);
	debug("%d energy   (J)  , %0.2lf = %0.2lf - %0.2lf", i, d3->energy_j, d2->energy_j, d1->energy_j);
}

state_t nvml_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	int i;
	if (data2 == NULL || data1 == NULL || data_diff == NULL) {
		return_msg(EAR_ERROR, Error.null_data);
	}
	for (i = 0; i < dev_count; i++) {
		nvml_read_diff(data2, data1, data_diff, i);
	}
	return EAR_SUCCESS;
}

state_t nvml_data_alloc(gpu_t **data)
{
	if (data == NULL) {
		return_msg(EAR_ERROR, Error.null_data);
	}
	*data = calloc(dev_count, sizeof(gpu_t));
	if (*data == NULL) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}

state_t nvml_data_free(gpu_t **data)
{
	if (data != NULL) {
		free(*data);
	}
	return EAR_SUCCESS;
}

state_t nvml_data_null(gpu_t *data)
{
	if (data == NULL) {
		return_msg(EAR_ERROR, Error.null_data);
	}
	memset(data, 0, dev_count * sizeof(gpu_t));
	return EAR_SUCCESS;
}

state_t nvml_data_copy(gpu_t *data_dst, gpu_t *data_src)
{
	if (data_dst == NULL || data_src == NULL) {
		return_msg(EAR_ERROR, Error.null_data);
	}
	while (pthread_mutex_trylock(lock));
	memcpy(data_dst, data_src, dev_count * sizeof(gpu_t));
	pthread_mutex_unlock(lock);
	return EAR_SUCCESS;
}

state_t nvml_data_print(gpu_t *data, int fd)
{
	char buffer[1024];
	nvml_data_tostr(data, buffer, 1024);
	fprintf(fd, "s", buffer);
	return EAR_SUCCESS;
}

state_t nvml_data_tostr(gpu_t *data, char *buffer, int length)
{
	snprintf(buffer, length, "gpu%u: %lu;%0.2lf;%0.2lf;%lu;%lu;%lu;%lu;%lu;%lu;%u;%lu\n",
		i                 , data->time,
		data->energy_j    , data->power_w,
		data->freq_gpu_mhz, data->freq_mem_mhz,
		data->util_gpu    , data->util_mem,
		data->temp_gpu    , data->temp_mem,
		data->working     , data->samples);
	return EAR_SUCCESS;
}
