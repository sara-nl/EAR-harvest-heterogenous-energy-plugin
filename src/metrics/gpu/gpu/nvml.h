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

#ifndef METRICS_GPU_NVML
#define METRICS_GPU_NVML

#include <metrics/gpu/gpu.h>

state_t nvml_status();

state_t nvml_init(ctx_t *c);

state_t nvml_dispose(ctx_t *c);

state_t nvml_count(ctx_t *c, uint *gpu_count);

state_t nvml_pool(void *c);

state_t nvml_read(ctx_t *c, gpu_t *data);

state_t nvml_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

/* Sets the required variables to work with nvml_data_* functions. */
state_t nvml_data_init(uint dev_count);

state_t nvml_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

state_t nvml_data_alloc(gpu_t **data);

state_t nvml_data_free(gpu_t **data);

state_t nvml_data_null(gpu_t *data);

state_t nvml_data_copy(gpu_t *data_dst, gpu_t *data_src);

state_t nvml_data_print(gpu_t *data, int fd);

state_t nvml_data_tostr(gpu_t *data, char *buffer, int length);

#endif //METRICS_GPU_NVML
