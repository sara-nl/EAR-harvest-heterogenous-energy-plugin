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
#define GPU_METRICS     8

static char buffer[SZ_BUFF_BIG];
static pthread_t    samp_t_accumu;
static FILE        *samp_t_stream;
static uint         samp_num_gpus;
static uint         samp_enabled;
static gpu_power_t *samp_data;
static uint         samp_ms;

typedef struct nvsmi_context_s {
	uint correct;
} nvsmi_context_t;

state_t nvsmi_gpu_status()
{
	static const char *command = "nvidia-smi";
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_init(pcontext_t *c)
{
	if (samp_n_gpus == 0) {
		if (state_fail(nvsmi_gpu_count(c, &samp_n_gpus))) {
			return EAR_ERROR;
		}
	}
	if (samp_gpu_data == NULL){
		samp_gpu_data = calloc(samp_n_gpus, sizeof(gpu_power_t));
		if (samp_gpu_data == NULL) {
			return EAR_ERROR;
		}
	}

    return EAR_SUCCESS;
}

state_t nvsmi_gpu_dispose(pcontext_t *c)
{
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

static void nvsmi_gpu_sample_add(uint i, gpu_power_t *data_aux)
{
	samp_data[i].freq_gpu_mhz += data_aux->freq_gpu_mhz;
	samp_data[i].freq_mem_mhz += data_aux->freq_mem_mhz;
	samp_data[i].util_gpu     += data_aux->util_gpu;
	samp_data[i].util_mem     += data_aux->util_mem;
	samp_data[i].temp_gpu     += data_aux->temp_gpu;
	samp_data[i].temp_mem     += data_aux->temp_mem;
	samp_data[i].power_w      += data_aux->power_w;
	samp_data[i].correct       = 1;
}

static void *nvsmi_gpu_sample_main(void *p)
{
	static gpu_power_t data_aux;
	int i;
	int s;
	int r;

	#define gprint(d, i, s) \
		printf("i: %u, ", i); \
		printf("correct: %u (%d), ", d[i].correct, s); \
		printf("c_g: %u, ", d[i].freq_gpu_mhz); \
		printf("c_m: %u, ", d[i].freq_mem_mhz); \
   		printf("u_g: %u, ", d[i].util_gpu); \
    	printf("u_m: %u, ", d[i].util_mem); \
    	printf("temp_g: %u, ", d[i].temp_gpu); \
    	printf("temp_m: %u, ", d[i].temp_mem); \
		printf("power is: %f, ", d[i].power_w); \
		printf("energy is: %f\n", d[i].energy_j);

	while (samp_enabled)
	{
		do {
			s = fscanf(samp_t_stream, "%u, %d, %lu, %lu, %lu, %lu, %lu, %lu", &i,
					   &data_aux.power_w , &data_aux.freq_gpu_mhz, &data_aux.freq_mem_mhz,
					   &data_aux.util_gpu, &data_aux.util_mem,
					   &data_aux.temp_gpu, &data_aux.temp_mem);

			printf("%u %lu\n", i, data_aux.power_w);

			if (s == GPU_METRICS) {
				nvsmi_gpu_sample_add(i, &data_aux);
			}
		}
		while (s == GPU_METRICS);

		for (i = 0; i < nvsmi_c->num_gpus; ++i) {
			gprint(data_read, i, 0);
		}

		sleep(1);
	}

	return EAR_SUCCESS;
}

static state_t nvsmi_gpu_sample_cancel()
{
	if (samp_enabled == 0) {
		return EAR_ERROR;
	}
	if (samp_t_stream != NULL) {
		pclose(samp_t_stream);
		samp_t_stream = NULL;
	}
	if (samp_enabled == 1) {
		samp_enabled = 0;
		pthread_join(samp_t_stream, NULL);
	}
	return EAR_SUCCESS;
}

static int nvsmi_gpu_sample_is_ok()
{
	void *retval;
	if (samp_enabled == 0) {
		return 0;
	}
	if (ferror(samp_t_stream)) {
		nvsmi_gpu_sample_cancel();
		return 0;
	}
	if (!pthread_tryjoin_np(samp_t_accumu, &retval)){
		nvsmi_gpu_sample_cancel();
		return 0;
	}
	return 1;
}

static state_t nvsmi_gpu_sample_create(pcontext_t *c, uint loop_ms)
{
	static const char *command = "nvidia-smi"                                   \
					" --query-gpu='index,power.draw,clocks.current.sm,"         \
					"clocks.current.memory,utilization.gpu,utilization.memory," \
					"temperature.gpu,temperature.memory'"                       \
 					" --format='csv,noheader,nounits'";
	nvsmi_context_t *nvsmi_c;

	if (loop_ms == 0) {
		samp_ms = 1000;
	} else {
		samp_ms = loop_ms;
	}

	if (samp_enabled == 1) {
		return EAR_ERROR;
	}

	//
	sprintf(buffer, "%s --loop-ms='%u'", command, samp_ms);

	if ((samp_t_stream = popen(buffer, "r")) == NULL) {
		return EAR_ERROR;
	}

	//
	samp_enabled = 1;

	for (i = 0; i < samp_num_gpus; i++) {
		power_gpu_read_null(&samp_data[i]);
	}

	if (pthread_create(&samp_t_accumu, NULL, nvsmi_gpu_sample_main, NULL)) {
		nvsmi_gpu_sample_cancel();
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

state_t nvsmi_gpu_sample(pcontext_t *c, uint loop_ms)
{
	return nvsmi_gpu_sample_create(c, loop_ms);
}

state_t nvsmi_gpu_read(pcontext_t *c, gpu_power_t *data_read)
{
	if (samp_num_gpus == 0) {
		return EAR_ERROR;
	}

	if (!nvsmi_gpu_sample_is_ok())
	{
		nvsmi_gpu_data_null(c, data_read);

		nvsmi_gpu_sample_cancel();
		nvsmi_gpu_sample_create(c, samp_ms);

		return EAR_ERROR;
	}

	memcpy(data_read, samp_data, samp_num_gpus * sizeof(gpu_power_t));
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_data_alloc(pcontext_t *c, gpu_power_t **data_read)
{
	if (data_read != NULL) {
		*data_read = calloc(samp_num_gpus, sizeof(gpu_power_t));
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
	if (data_read != NULL) {
		memset(data_read, 0, samp_num_gpus * sizeof(gpu_power_t));
	}
	return EAR_SUCCESS;
}

state_t nvsmi_gpu_data_diff(pcontext_t *c, gpu_power_t *data_read1, gpu_power_t *data_read2, gpu_power_t *data_avrg)
{
	int i;

	if (data_read1 == NULL || data_read2 == NULL || data_avrg == NULL) {
		return EAR_ERROR;
	}
	for (i = 0; i < samp_num_gpus; i++) {
		//power_gpu_read_diff(data_read1, data_read2, data_avrg, i);
	}

	return EAR_SUCCESS;
}
