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

#ifndef METRICS_ENERGY_GPU_H
#define METRICS_ENERGY_GPU_H

#include <metrics/gpu/gpu.h>

state_t lib_gpu_load(settings_conf_t *settings);

state_t lib_gpu_init(ctx_t *c);

state_t lib_gpu_dispose(ctx_t *c);

state_t lib_gpu_count(ctx_t *c, uint *dev_count);

state_t lib_gpu_read(ctx_t *c, gpu_t *data);

state_t lib_gpu_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

state_t lib_gpu_data_init(uint dev_count);

state_t lib_gpu_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

state_t lib_gpu_data_alloc(gpu_t **data);

state_t lib_gpu_data_free(gpu_t **data);

state_t lib_gpu_data_null(gpu_t *data);

state_t lib_gpu_data_copy(gpu_t *data_dst, gpu_t *data_src);

state_t lib_gpu_data_print(gpu_t *data, int fd);

state_t lib_gpu_data_tostr(gpu_t *data, char *buffer, int length);

#endif
