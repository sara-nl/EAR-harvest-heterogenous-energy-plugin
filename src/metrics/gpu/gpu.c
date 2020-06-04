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

#include <metrics/gpu/gpu.h>
#include <metrics/gpu/gpu/nvml.h>

struct energy_gpu_ops
{
	state_t (*init)			(ctx_t *c);
	state_t (*dispose)		(ctx_t *c);
	state_t (*read)			(ctx_t *c, gpu_t *data);
	state_t (*read_copy)	(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff);
	state_t (*count)		(ctx_t *c, uint *gpu_count);
	state_t (*data_diff)	(gpu_t *data2, gpu_t *data1, gpu_t *data_diff);
	state_t (*data_alloc)	(gpu_t **data);
	state_t (*data_free)	(gpu_t **data);
	state_t (*data_null)	(gpu_t *data);
	state_t (*data_copy)	(gpu_t *data_dst, gpu_t *data_src);
	state_t (*data_print)	(gpu_t *data, int fd);
	state_t (*data_tostr)	(gpu_t *data, char *buffer, int length);
} ops;

#define MODEL_NVML		 0
#define MODEL_ERROR		-1

static uint model;
static uint loaded;

state_t gpu_load()
{
	if (loaded != 0) {
		return EAR_SUCCESS;
	}

	if (state_ok(nvml_status()))
	{
		ops.init		= nvml_init;
		ops.dispose		= nvml_dispose;
		ops.read		= nvml_read;
		ops.read_copy	= nvml_read_copy;
		ops.count		= nvml_count;
		ops.data_diff	= nvml_data_diff;
		ops.data_alloc	= nvml_data_alloc;
		ops.data_free	= nvml_data_free;
		ops.data_null	= nvml_data_null;
		ops.data_diff	= nvml_data_diff;
		ops.data_copy	= nvml_data_copy;
		ops.data_print	= nvml_data_print;
		ops.data_tostr	= nvml_data_tostr;
		model			= MODEL_NVML;
		loaded			= 1;
	} else {
		return_msg(EAR_INCOMPATIBLE, Generr.api_incompatible);
	}

	return EAR_SUCCESS;
}

state_t gpu_init(ctx_t *c)
{
	preturn (ops.init, c);
}

state_t gpu_dispose(ctx_t *c)
{
	preturn (ops.dispose, c);
}

state_t gpu_read(ctx_t *c, gpu_t *data)
{
	preturn (ops.read, c, data);
}

state_t gpu_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	preturn (ops.read_copy, c, data2, data1, data_diff);
}

state_t gpu_count(ctx_t *c, uint *dev_count)
{
	preturn (ops.count, c, dev_count);
}

state_t gpu_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	preturn (ops.data_diff, data2, data1, data_diff);
}

state_t gpu_data_alloc(gpu_t **data)
{
	preturn (ops.data_alloc, data);
}

state_t gpu_data_free(gpu_t **data)
{
	preturn (ops.data_free, data);
}

state_t gpu_data_null(gpu_t *data)
{
	preturn (ops.data_null, data);
}

state_t gpu_data_copy(gpu_t *data_dst, gpu_t *data_src)
{
	preturn (ops.data_copy, data_dst, data_src);
}

state_t gpu_data_print(gpu_t *data, int fd)
{
	preturn (ops.data_print, data, fd);
}

state_t gpu_data_tostr(gpu_t *data, char *buffer, int length)
{
	preturn (ops.data_tostr, data, buffer, length);
}
