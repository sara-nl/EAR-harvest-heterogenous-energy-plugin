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

#ifndef MANAGEMENT_GPU
#define MANAGEMENT_GPU

#include <common/types.h>
#include <common/states.h>
#include <common/plugins.h>
#include <common/system/time.h>

typedef struct mgt_gpu_ops_s
{
	state_t (*init)                   (ctx_t *c);
	state_t (*dispose)                (ctx_t *c);
	state_t (*count)                  (ctx_t *c, uint *dev_count);
	state_t (*alloc_array)            (ctx_t *c, ulong **array);
	state_t (*clock_cap_get_current)  (ctx_t *c, ulong *khz);
	state_t (*clock_cap_get_default)  (ctx_t *c, ulong *khz);
	state_t (*clock_cap_get_max)      (ctx_t *c, ulong *khz);
	state_t (*clock_cap_reset)        (ctx_t *c);
	state_t (*clock_cap_set)          (ctx_t *c, ulong *khz);
	state_t (*clock_list)             (ctx_t *c, ulong ***list_khz, uint **list_len);
	state_t (*power_cap_get_current)  (ctx_t *c, ulong *watts);
	state_t (*power_cap_get_default)  (ctx_t *c, ulong *watts);
	state_t (*power_cap_get_rank)     (ctx_t *c, ulong *watts_min, ulong *watts_max);
	state_t (*power_cap_reset)        (ctx_t *c);
	state_t (*power_cap_set)          (ctx_t *c, ulong *watts);
} mgt_gpu_ops_t;


/** Discovers the low level API. Returns function pointers, but is not required. */
state_t mgt_gpu_load(mgt_gpu_ops_t **ops);

/** Initializes the context. */
state_t mgt_gpu_init(ctx_t *c);

state_t mgt_gpu_dispose(ctx_t *c);

/** Counts the number of GPUs in the node. */
state_t mgt_gpu_count(ctx_t *c, uint *_dev_count);

/** Allocates an array of watts or clocks per device. */
state_t mgt_gpu_alloc_array(ctx_t *c, ulong *array);

/** Gets the current clock cap for each GPU in the node. */
state_t mgt_gpu_clock_cap_get_current(ctx_t *c, ulong *khz);

/** Gets the default clock cap for each GPU in the node. */
state_t mgt_gpu_clock_cap_get_default(ctx_t *c, ulong *khz);

/** Gets the maximum clock cap for each GPU in the node. */
state_t mgt_gpu_clock_cap_get_max(ctx_t *c, ulong *khz);

/** Resets the current clock cap on each GPU to its default value. */
state_t mgt_gpu_clock_cap_reset(ctx_t *c);

/** Sets the current clock cap on each GPU (one value per GPU in the khz array). */
state_t mgt_gpu_clock_cap_set(ctx_t *c, ulong *khz);

/** Gets a list of clocks and list length per device. */
state_t mgt_gpu_clock_list(ctx_t *c, ulong ***list_khz, uint **list_len);

/** Gets the current power cap for each GPU in the node. */
state_t mgt_gpu_power_cap_get_current(ctx_t *c, ulong *watts);

/** Gets the default power cap for each GPU in the node. */
state_t mgt_gpu_power_cap_get_default(ctx_t *c, ulong *watts);

/** Gets the minimum/maximum possible power cap for each GPU in the node. */
state_t mgt_gpu_power_cap_get_rank(ctx_t *c, ulong *watts_min, ulong *watts_max);

/** Resets the current power cap on each GPU to its default value. */
state_t mgt_gpu_power_cap_reset(ctx_t *c);

/** Sets the current power cap on each GPU (one value per GPU in the watts array). */
state_t mgt_gpu_power_cap_set(ctx_t *c, ulong *watts);

#endif