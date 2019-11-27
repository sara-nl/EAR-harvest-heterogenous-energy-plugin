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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <metrics/energy/energy_gpu.h>
#include <metrics/energy/gpu/nvsmi.h>

#define SZ_BUFF_BIG     4096
#define GPU_METRICS     7

static char buffer[SZ_BUFF_BIG];
static uint ok = 1;
static uint ko = 0;

typedef struct nvsmi_context_s {
	FILE *stream;
	uint num_gpus;
} nvsmi_context_t;

state_t nvsmi_gpu_status()
{
	static const char *command = "nvidia-smi";

	return EAR_SUCCESS;
}

state_t nvsmi_gpu_init(pcontext_t *c, uint sampling_ms)
{
	nvsmi_context_t *nvsmi_c;
	uint num_gpus;

	if (state_fail(nvsmi_gpu_count(c, &num_gpus))) {
		return EAR_ERROR;
	}

    if (c != NULL) {
        c->context = malloc(sizeof(nvsmi_c));
    	if (c->context == NULL) {
        	nvsmi_gpu_dispose(c);
        	return EAR_ERROR;
		}
		nvsmi_c = (nvsmi_context_t *) c->context;
		nvsmi_c->num_gpus = num_gpus;
	}
    
    return EAR_SUCCESS;
}

state_t nvsmi_gpu_dispose(pcontext_t *c)
{
    if (c->context != NULL) {
        free(c->context);
    }
    if (c->stream != NULL) {
    	pclose(c->stream);
    }
	c->context = NULL;
    c->stream  = NULL;
    return EAR_SUCCESS;
}

state_t nvsmi_gpu_count(pcontext_t *c, uint *count)
{
	static const char *command = "nvidia-smi -L";
	FILE* file = popen(command, "r");

	//
	*count = 0;

	if (file == NULL) {
		return EAR_ERROR;
	}

	while (fgets(buffer, SZ_BUFF_BIG, file)) {
		*count += 1;
	}

	pclose(file);

	if (*count == 0) {
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

static state_t nvsmi_gpu_sampling_start(pcontext_t *c, uint loop_ms)
{
	static const char *command = "nvidia-smi"                                   \
					" --query-gpu='power.draw,clocks.current.sm,"               \
					"clocks.current.memory,utilization.gpu,utilization.memory," \
					"temperature.gpu,temperature.memory'"                       \
 					" --format='csv,noheader,nounits'";
	sprintf(buffer, "%s --loop-ms='%u'", command, loop_ms);

	//
	if (c == NULL) {
		return EAR_ERROR;
	}
	if ((c->stream = popen(command, "r")) == NULL) {
		return EAR_ERROR;
	}
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_sampling_start(pcontext_t *c, uint loop_ms)
{
	return nvsmi_gpu_sampling_start(c, loop_ms);
}

state_t nvsmi_gpu_read(pcontext_t *c, gpu_power_t *data_read)
{
	FILE *stream;
	int num_gpus;
    int i;
    int s;
    int r;

    if (c == NULL) {
    	return EAR_ERROR;
    }

	num_gpus = ((nvsmi_context_t *) c->context)->num_gpus;
	stream   = ((nvsmi_context_t *) c->context)->stream;

    if (num_gpus == 0) {
        return EAR_ERROR;
    }
	if (stream == NULL) {
		nvsmi_gpu_sampling_start(c, 1000);
    }

	#define gprint(d, i) \
		printf("correct: %u, ", d[i].correct); \
		printf("c_g: %u, ", d[i].freq_gpu_mhz); \
		printf("c_m: %u, ", d[i].freq_mem_mhz); \
   		printf("u_g: %u, ", d[i].util_gpu); \
    	printf("u_m: %u, ", d[i].util_mem); \
    	printf("temp_g: %u, ", d[i].temp_gpu); \
    	printf("temp_m: %u, ", d[i].temp_mem); \
		printf("power is: %f, ", d[i].power_w); \
		printf("energy is: %f\n", d[i].energy_j);

	do {
    	i = 0;
		s = fscanf(file, "%f, %u, %u, %u, %u, %u, %u",
				   &data_read[i].power_w, &data_read[i].freq_gpu_mhz, &data_read[i].freq_mem_mhz,
				   &data_read[i].util_gpu, &data_read[i].util_mem,
				   &data_read[i].temp_gpu, &data_read[i].temp_mem);

		data_read[i].correct = (s == GPU_METRICS);
		data_read[i].time = time;

		gprint(data_read, 0);
	}
	while (s == GPU_METRICS);

	return EAR_SUCCESS;
/*
    // Copying and cleaning
    memcpy(ddiff, dread, sizeof(gpu_power_t) * num_gpus);
    memset(dread,     0, sizeof(gpu_power_t) * num_gpus);

    // Getting time
    timestamp time;
    timestamp_getprecise(&time);

    // Getting GPU data
    r = EAR_SUCCESS;
    i = 0;

    do {
        s = fscanf(file, "%f, %u, %u, %u, %u, %u, %u",
            &dread[i].power_w , &dread[i].freq_gpu_mhz, &dread[i].freq_mem_mhz,
            &dread[i].util_gpu, &dread[i].util_mem,
            &dread[i].temp_gpu, &dread[i].temp_mem);

        dread[i].correct = (s == GPU_METRICS);
        dread[i].time = time;

        if (dread[i].correct && ddiff != NULL) {
            if (ddiff[i].correct) {
                power_gpu_read_diff(dread, ddiff, i);
            }
        } else {
            memset(&dread[i], 0, sizeof(gpu_power_t));
            r = EAR_WARNING;
        }

        i += 1;
    }
    while (i < num_gpus);

    pclose(file);

    return r;
*/
}

state_t nvsmi_gpu_data_alloc(pcontext_t *c, gpu_power_t **data_read)
{
	nvsmi_context_t *nvsmi_c;
	uint num_gpus;

	if (c == NULL) {
		return EAR_ERROR;
	}
	if ((nvsmi_c = (nvsmi_context_t *) c->context) == NULL) {
		return EAR_ERROR;
	}
	if ((num_gpus = nvsmi_c->num_gpus) == 0) {
		return EAR_ERROR;
	}
	if (data_read != NULL) {
		*data_read = calloc(num_gpus, sizeof(gpu_power_t));
		if (*data_read == NULL) {
			return EAR_ERROR;
		}
	}
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_data_free(pcontext_t *c, gpu_power_t **data_read)
{
	if (data_read != NULL) {
		free(*data_read);
	}
}

state_t nvsmi_gpu_data_null(pcontext_t *c, gpu_power_t *data_read)
{
	nvsmi_context_t *nvsmi_c;
	uint num_gpus;

	if (c == NULL) {
		return EAR_ERROR;
	}
	if ((nvsmi_c = (nvsmi_context_t *) c->context) == NULL) {
		return EAR_ERROR;
	}
	if ((num_gpus = nvsmi_c->num_gpus) == 0) {
		return EAR_ERROR;
	}
	if (data_read != NULL) {
		memset(data_read, 0, num_gpus * sizeof(gpu_power_t));
	}
	return EAR_SUCCESS;
}

static void power_gpu_read_diff(gpu_power_t *data_read1, gpu_power_t *data_read2, gpu_power_t *data_avrg, int i)
{
	// Averages
	data_avrg[i].freq_gpu_mhz = (data_read1[i].freq_gpu_mhz + data_read2[i].freq_gpu_mhz) / 2;
	data_avrg[i].freq_mem_mhz = (data_read1[i].freq_mem_mhz + data_read2[i].freq_mem_mhz) / 2;
	data_avrg[i].util_gpu     = (data_read1[i].util_gpu     + data_read2[i].util_gpu) / 2;
	data_avrg[i].util_mem     = (data_read1[i].util_mem     + data_read2[i].util_mem) / 2;
	data_avrg[i].temp_gpu     = (data_read1[i].temp_gpu     + data_read2[i].temp_gpu) / 2;
	data_avrg[i].temp_mem     = (data_read1[i].temp_mem     + data_read2[i].temp_mem) / 2;

	// Time difference to float
	ullong time_us            = timestamp_diff(&data_read2[i].time, &data_read1[i].time, TIME_USECS);
	float  time_s             = (float) time_us;
	       time_s             = (time_s / 1000000.0);

	// Getting energy
	data_avrg[i].power_w      = (data_read1[i].power_w + data_read2[i].power_w) / 2.0;
	data_avrg[i].energy_j     = (data_avrg[i].power_w / time_s);
}

state_t nvsmi_gpu_data_diff(pcontext_t *c, gpu_power_t *data_read1, gpu_power_t *data_read2, gpu_power_t *data_avrg)
{
	int i;

	if (c == NULL) {
		return EAR_ERROR;
	}
	if ((nvsmi_c = (nvsmi_context_t *) c->context) == NULL) {
		return EAR_ERROR;
	}
	if ((num_gpus = nvsmi_c->num_gpus) == 0) {
		return EAR_ERROR;
	}
	if (data_read1 == NULL || data_read2 == NULL || data_avrg == NULL) {
		return EAR_ERROR;
	}
	for (i = 0; i < num_gpus; i++) {
		power_gpu_read_diff(data_read1, data_read2, data_avrg, i);
	}

	return EAR_SUCCESS;
}