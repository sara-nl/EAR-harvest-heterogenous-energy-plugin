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

#ifndef METRICS_GPU_H
#define METRICS_GPU_H

#include <common/types.h>
#include <common/states.h>
#include <common/plugins.h>
#include <common/system/time.h>

#define MODEL_NVML			2
#define MODEL_DUMMY			1
#define MODEL_UNDEFINED		0

typedef struct gpu_s
{
	timestamp_t time;
	ulong samples;
	ulong freq_gpu; // khz
	ulong freq_mem; // khz
	ulong util_gpu; // percent
	ulong util_mem; // percent
	ulong temp_gpu; // celsius
	ulong temp_mem; // celsius
	double energy_j;
	double power_w;
	uint working;
	uint correct;
} gpu_t;

typedef struct gpu_ops_s
{
	state_t (*init)			(ctx_t *c);
	state_t (*init_unprivileged) (ctx_t *c);
	state_t (*dispose)		(ctx_t *c);
	state_t (*read)			(ctx_t *c, gpu_t *data);
	state_t (*read_raw)		(ctx_t *c, gpu_t *data);
	state_t (*read_copy)	(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff);
	state_t (*count)		(uint *gpu_count);
	state_t (*data_diff)	(gpu_t *data2, gpu_t *data1, gpu_t *data_diff);
	state_t (*data_merge)	(gpu_t *data_diff, gpu_t *data_merge);
	state_t (*data_alloc)	(gpu_t **data);
	state_t (*data_free)	(gpu_t **data);
	state_t (*data_null)	(gpu_t *data);
	state_t (*data_copy)	(gpu_t *data_dst, gpu_t *data_src);
	state_t (*data_print)	(gpu_t *data, int fd);
	state_t (*data_tostr)	(gpu_t *data, char *buffer, int length);
} gpu_ops_t;

/** Discovers the low level API. Returns function pointers and models but is not required. */
state_t gpu_load(gpu_ops_t **ops, uint model_force, uint *model_used);

/** Initializes the context. */
state_t gpu_init(ctx_t *c);

/** Initializes the context to use unprivileged functions only. */
state_t gpu_init_unprivileged(ctx_t *c);

state_t gpu_dispose(ctx_t *c);

/** Counts the number of GPUs in the node. */
// The ctx_t is obsolete.
state_t gpu_count(ctx_t *c, uint *dev_count);

/** Reads the GPU device data and stores it in the gpu_t array data (1 per device). */
state_t gpu_read(ctx_t *c, gpu_t *data);

/** Performs a gpu_read() over data2, a gpu_data_diff() and copies data2 in data1. */
state_t gpu_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

/** Reads the GPU device data directly from the hardware (not pooled). */
state_t gpu_read_raw(ctx_t *c, gpu_t *data);

/** Substracts the elements of the gpu_t array (data_diff = data2 - data1). */
state_t gpu_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff);

/** Adds all the elements of the data_diff array. */
state_t gpu_data_merge(gpu_t *data_diff, gpu_t *data_merge);

/** Allocates an array of gpu_t (1 per device). */
state_t gpu_data_alloc(gpu_t **data);

/** Frees an array of gpu_t. */
state_t gpu_data_free(gpu_t **data);

/** Sets to 0 an array of gpu_t. */
state_t gpu_data_null(gpu_t *data);

/** Copies an array of gpu_t. */
state_t gpu_data_copy(gpu_t *data_dst, gpu_t *data_src);

/** Prints an array of gpu_t in the channel fd. */
state_t gpu_data_print(gpu_t *data, int fd);

/** Copies the printing string in a char buffer. */
state_t gpu_data_tostr(gpu_t *data, char *buffer, int length);

/* Special function, returns the using model (if not initialized returns MODEL_UNDEFINED). */
uint gpu_model();

#endif
