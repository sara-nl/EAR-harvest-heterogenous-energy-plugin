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
	uint num_gpus;
} nvsmi_context_t;

state_t nvsmi_gpu_status()
{
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_init(pcontext_t *c, gpu_power_t **dread, gpu_power_t **ddiff)
{
	nvsmi_context_t *nvsmi_c;
	uint num_gpus;

	if (state_fail(nvsmi_gpu_count(c, &num_gpus))) {
		return EAR_ERROR;
	}	

    if (dread != NULL) {
		*dread = malloc(num_gpus * sizeof(gpu_power_t));
		if (*dread == NULL) {
        	nvsmi_gpu_dispose(c, dread, ddiff);
        	return EAR_ERROR;
		}
	} else {
		return EAR_ERROR;
	}

    if (ddiff != NULL) {
        *ddiff = malloc(num_gpus * sizeof(gpu_power_t));
    	if (ddiff == NULL) {
        	nvsmi_gpu_dispose(c, dread, ddiff);
        	return EAR_ERROR;
    	}
    }

    if (c != NULL) {
        c->context = malloc(sizeof(nvsmi_c));
    	if (c->context == NULL) {
        	nvsmi_gpu_dispose(c, dread, ddiff);
        	return EAR_ERROR;
		}
		nvsmi_c = (nvsmi_context_t *) c->context;
		nvsmi_c->num_gpus = num_gpus;
	}
    
    return EAR_SUCCESS;
}

state_t nvsmi_gpu_dispose(pcontext_t *c, gpu_power_t **dread, gpu_power_t **davrg)
{
    if (c->context != NULL) {
        free(c->context);
    }
	if (dread != NULL) {
		if (*dread != NULL) {
        	free(*dread);
		}
	}
	if (davrg != NULL) {
		if (*davrg != NULL) {
        	free(*davrg);
		}
	}
	c->context = NULL;
	dread = NULL;
	davrg = NULL;
    return EAR_SUCCESS;
}

static void power_gpu_read_diff(gpu_power_t *dread, gpu_power_t *ddiff, int i)
{
    // Averages
    ddiff[i].freq_gpu_mhz = (ddiff[i].freq_gpu_mhz + dread[i].freq_gpu_mhz) / 2;
    ddiff[i].freq_mem_mhz = (ddiff[i].freq_mem_mhz + dread[i].freq_mem_mhz) / 2;
    ddiff[i].util_gpu     = (ddiff[i].util_gpu     + dread[i].util_gpu) / 2;
    ddiff[i].util_mem     = (ddiff[i].util_mem     + dread[i].util_mem) / 2;
    ddiff[i].temp_gpu     = (ddiff[i].temp_gpu     + dread[i].temp_gpu) / 2;
    ddiff[i].temp_mem     = (ddiff[i].temp_mem     + dread[i].temp_mem) / 2;

    // Time difference to float
    ullong time_us        = timestamp_diff(&dread[i].time, &ddiff[i].time, TIME_USECS);
    float  time_s         = (float) time_us;
           time_s         = (time_s / 1000000.0);

    // Getting energy
    ddiff[i].power_w      = (ddiff[i].power_w + dread[i].power_w) / 2.0;
    ddiff[i].energy_j     = (ddiff[i].power_w / time_s);
}

state_t nvsmi_gpu_read(pcontext_t *c, gpu_power_t *dread, gpu_power_t *ddiff)
{
	static const char *command = "nvidia-smi"                                   \
					" --query-gpu='power.draw,clocks.current.sm,"               \
					"clocks.current.memory,utilization.gpu,utilization.memory," \
					"temperature.gpu,temperature.memory'"                       \
 					" --format='csv,noheader,nounits'";
	
    FILE* file = popen(command, "r");
	int num_gpus;
    int i;
    int s;
    int r;

	num_gpus = ((nvsmi_context_t *) c->context)->num_gpus;
	
    if (num_gpus == 0) {
        return EAR_ERROR;
    }

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
}

state_t nvsmi_gpu_count(pcontext_t *c, uint *count)
{
	static const char *command = "nvidia-smi -L";
    FILE* file = popen(command, "r");
	int s;
	
	//
	*count = 0;

	while (fgets(buffer, SZ_BUFF_BIG, file)) {
		*count += 1;
	}

	if (*count == 0) { 
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

