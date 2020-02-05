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

#include <nvml.h>
#include <stdlib.h>
#include <string.h>
#include <metrics/energy/gpu/nvml.h>

#define SHOW_DEBUGS 1
#include <common/output/verbose.h>

static struct error_s {
	char *null_context;
	char *null_data;
} Error = {
	.null_context = "context pointer is NULL",
	.null_data = "data pointer is NULL",
};

typedef struct nvsml_s {
	uint num_gpus;
} nvsml_t;

state_t nvml_status()
{
	int n_gpus;

	if (nvmlInit() != NVML_SUCCESS) {
		return EAR_ERROR;
	}
	if (nvmlDeviceGetCount(&n_gpus) != NVML_SUCCESS) {
		return EAR_ERROR;
	}
	if (n_gpus <= 0) {
		return EAR_ERROR;
	}

	nvmlShutdown();
	return EAR_SUCCESS;
}

state_t nvml_init(pcontext_t *c)
{
	nvsml_t *n;

	if (c->initialized == 1) {
		return EAR_ERROR;
	}
	if (nvmlInit() != NVML_SUCCESS) {
		return EAR_ERROR;
	}

	c->initialized = 1;
	c->context = calloc(1, sizeof(nvsml_t));
	n = (nvsml_t *) c->context;

	if (c->context == NULL) {
		return nvml_dispose(c);
	}
	if (state_fail(nvml_count(c, &n->num_gpus))) {
		return nvml_dispose(c);
	}

	return EAR_SUCCESS;
}

state_t nvml_dispose(pcontext_t *c)
{
	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (c->context != NULL) {
		free(c->context);
	}

	c->initialized = 1;
	c->context = NULL;
	nvmlShutdown();

	return EAR_SUCCESS;
}

state_t nvml_count(pcontext_t *c, uint *count)
{
	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (nvmlDeviceGetCount(count) != NVML_SUCCESS) {
		*count = 0;
		return EAR_ERROR;
	}
	if (((int) *count) <= 0) {
		*count = 0;
		return EAR_ERROR;
	}
	return EAR_SUCCESS;
}

state_t nvml_read(pcontext_t *c, gpu_energy_t *data_read)
{
	#define OK NVML_SUCCESS
	nvmlReturn_t s0, s1, s2, s3, s4, s5;
	nvsml_t *n = c->context;
	nvmlUtilization_t util;
	nvmlEnableState_t mode;
	nvmlDevice_t device;
	uint freq_gpu_mhz;
	uint freq_mem_mhz;
	uint temp_gpu;
	uint power_mw;
	int i;

	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (c->context == NULL) {
		return_msg(EAR_ERROR, Error.null_context);
	}

	for (i = 0; i < n->num_gpus; ++i)
	{
		if ((s0 = nvmlDeviceGetHandleByIndex(i, &device)) != OK) {
			memset(&data_read[i], 0, sizeof(gpu_energy_t));
			continue;
		}
		if ((s0 = nvmlDeviceGetPowerManagementMode(device, &mode)) != OK) {
			memset(&data_read[i], 0, sizeof(gpu_energy_t));
			continue;
		}
		if (mode != NVML_FEATURE_ENABLED) {
			memset(&data_read[i], 0, sizeof(gpu_energy_t));
			continue;
		}

		s0 = nvmlDeviceGetPowerUsage(device, &power_mw);
		s1 = nvmlDeviceGetClockInfo(device, NVML_CLOCK_MEM, &freq_mem_mhz);
		s2 = nvmlDeviceGetClockInfo(device, NVML_CLOCK_SM , &freq_gpu_mhz);
		s3 = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp_gpu);
		s4 = nvmlDeviceGetUtilizationRates(device, &util);

		data_read[i].power_w      = ((double) power_mw) / 1000.0;
		data_read[i].freq_mem_mhz = (ulong) freq_mem_mhz;
		data_read[i].freq_gpu_mhz = (ulong) freq_gpu_mhz;
		data_read[i].util_mem     = (ulong) util.memory;
		data_read[i].util_gpu     = (ulong) util.gpu;
		data_read[i].temp_gpu     = (ulong) temp_gpu;
		data_read[i].temp_mem     = 0LU;

		data_read[i].correct = (s0 == OK) &&
							   (s1 == OK) &&
							   (s2 == OK) &&
							   (s3 == OK) &&
							   (s4 == OK);
	}

	return EAR_SUCCESS;
}

state_t nvml_data_alloc(pcontext_t *c, gpu_energy_t **data_read)
{
	nvsml_t *n = c->context;

	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (c->context == NULL) {
		return EAR_ERROR;
	}
	if (*data_read != NULL) {
		return EAR_ERROR;
	}
	*data_read = calloc(n->num_gpus, sizeof(gpu_energy_t));
	if (*data_read == NULL) {
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

state_t nvml_data_free(pcontext_t *c, gpu_energy_t **data_read)
{
	if (data_read != NULL) {
		free(*data_read);
	}
	return EAR_SUCCESS;
}

state_t nvml_data_null(pcontext_t *c, gpu_energy_t *data_read)
{
	nvsml_t *n = c->context;
	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (c->context == NULL) {
		return EAR_ERROR;
	}
	if (data_read == NULL) {
		return EAR_ERROR;
	}
	memset(data_read, 0, n->num_gpus * sizeof(gpu_energy_t));

	return EAR_SUCCESS;
}

static void nvml_read_diff(gpu_energy_t *data_read1, gpu_energy_t *data_read2, gpu_energy_t *data_avrg, int i)
{
	// Averages
	data_avrg[i].freq_gpu_mhz = (data_read2[i].freq_gpu_mhz - data_read1[i].freq_gpu_mhz);
	data_avrg[i].freq_mem_mhz = (data_read2[i].freq_mem_mhz - data_read1[i].freq_mem_mhz);
	data_avrg[i].util_gpu = (data_read2[i].util_gpu - data_read1[i].util_gpu);
	data_avrg[i].util_mem = (data_read2[i].util_mem - data_read1[i].util_mem);
	data_avrg[i].temp_gpu = (data_read2[i].temp_gpu - data_read1[i].temp_gpu);
	data_avrg[i].temp_mem = (data_read2[i].temp_mem - data_read1[i].temp_mem);
	data_avrg[i].power_w  = (data_read2[i].power_w  - data_read1[i].power_w);
	data_avrg[i].energy_j = (data_read2[i].energy_j - data_read1[i].energy_j);
}

state_t nvml_data_diff(pcontext_t *c, gpu_energy_t *data_read1, gpu_energy_t *data_read2, gpu_energy_t *data_avrg)
{
	nvsml_t *n = c->context;
	int i;

	if (c->initialized != 1) {
		return EAR_ERROR;
	}
	if (c->context == NULL) {
		return EAR_ERROR;
	}
	if (data_read1 == NULL || data_read2 == NULL || data_avrg == NULL) {
		return EAR_ERROR;
	}

	for (i = 0; i < n->num_gpus; i++) {
		nvml_read_diff(data_read1, data_read2, data_avrg, i);
	}
}

#if MAIN
// gcc -O3 -DMAIN -I /hpc/base/cuda/cuda-10.1/targets/x86_64-linux/include/ -I ../../../ -g -o nvml nvml.c ../../../common/libcommon.a -L/hpc/base/cuda/cuda-10.1/lib64/stubs/ -lnvidia-ml
// ln -s /hpc/base/cuda/cuda-10.1/lib64/stubs/libnvidia-ml.so ./libnvidia-ml.so.1
// LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH ./nvml
int main(int argc, char *argv[])
{
    gpu_energy_t *data_aux = NULL;
	pcontext_t c;
    state_t s;
    uint k;
    uint i;

    s = nvml_init(&c);
    debug("nvml_init %d", s);

    s = nvml_data_alloc(&c, &data_aux);
    debug("nvml_data_alloc %d", s);

    s = nvml_count(&c, &k);
    debug("nvml_count %d", s);

    while (1)
    {
        sleep(1);

        s = nvml_read(&c, data_aux);
        debug("nvml_read %d", s);

        for (i = 0; i < k; ++i)
        {
            debug("######## %u %0.2lf %lu %lu %lu %lu %lu %lu",
                i                       , data_aux[i].power_w,
                data_aux[i].freq_gpu_mhz, data_aux[i].freq_mem_mhz,
                data_aux[i].util_gpu    , data_aux[i].util_mem,
                data_aux[i].temp_gpu    , data_aux[i].temp_mem);
        }
    }

    return 0;
}
#endif
