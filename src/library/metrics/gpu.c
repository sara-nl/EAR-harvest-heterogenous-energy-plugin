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

#include <library/metrics/gpu.h>

static gpu_ops_t *ops;
static uint dev_count;

state_t gpu_lib_load(settings_conf_t *settings)
{
	// HERE: Get the GPU model identificator, look
	// 'metrics/gpu/gpu.c' for the model list.

	// Replace 0 by GPU model
	return gpu_load(&ops, 0);
}

state_t gpu_lib_init(ctx_t *c)
{
	// HERE: Ask Daemon for dev_count

	preturn (ops->init_data, dev_count);
}

state_t gpu_lib_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t gpu_lib_read(ctx_t *c, gpu_t *data)
{
	// HERE: Ask daemon for GPU metrics
	
	return EAR_SUCCESS;
}

state_t gpu_lib_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	state_t s;
	if (xtate_fail(s, gpu_lib_read(c, data2))) {
		return s;
	}
	if (xtate_fail(s, ops->gpu_data_diff(data2, data1, data_diff))) {
		return s;
	}
	return ops->data_copy(data1, data2);
}

state_t gpu_lib_count(ctx_t *c, uint *dev_count)
{
	*dev_count = dev_count;
	return EAR_SUCCESS;
}

state_t gpu_lib_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	preturn (ops->data_diff, data2, data1, data_diff);
}

state_t gpu_lib_data_alloc(gpu_t **data)
{
	preturn (ops->data_alloc, data);
}

state_t gpu_lib_data_free(gpu_t **data)
{
	preturn (ops->data_free, data);
}

state_t gpu_lib_data_null(gpu_t *data)
{
	preturn (ops->data_null, data);
}

state_t gpu_lib_data_copy(gpu_t *data_dst, gpu_t *data_src)
{
	preturn (ops->data_copy, data_dst, data_src);
}

state_t gpu_lib_data_print(gpu_t *data, int fd)
{
	preturn (ops->data_print, data, fd);
}

state_t gpu_lib_data_tostr(gpu_t *data, char *buffer, int length)
{
	preturn (ops->data_tostr, data, buffer, length);
}
