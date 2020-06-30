/**
 * Copyright © 2017-present BSC-Lenovo
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
	state_t (*init)                     (ctx_t *c);
	state_t (*dispose)                  (ctx_t *c);
	state_t (*count)                    (ctx_t *c, uint *dev_count);
	state_t (*clock_limit_get_current)  (ctx_t *c, uint *khz);
	state_t (*clock_limit_get_default)  (ctx_t *c, uint *khz);
	state_t (*clock_limit_get_max)      (ctx_t *c, uint *khz);
	state_t (*clock_limit_reset)        (ctx_t *c);
	state_t (*clock_limit_set)          (ctx_t *c, uint *khz);
	state_t (*power_limit_get_current)  (ctx_t *c, uint *watts);
	state_t (*power_limit_get_default)  (ctx_t *c, uint *watts);
	state_t (*power_limit_get_max)      (ctx_t *c, uint *watts);
	state_t (*power_limit_reset)        (ctx_t *c);
	state_t (*power_limit_set)          (ctx_t *c, uint *watts);
} mgt_gpu_ops_t;


/** Discovers the low level API. Returns function pointers, but is not required. */
state_t mgt_gpu_load(mgt_gpu_ops_t **ops);

/** Initializes the context. */
state_t mgt_gpu_init(ctx_t *c);

state_t mgt_gpu_dispose(ctx_t *c);

/** Counts the number of GPUs in the node. */
state_t mgt_gpu_count(ctx_t *c, uint *_dev_count);

/** Gets the current clock limit for each GPU in the node. */
state_t mgt_gpu_clock_limit_get_current(ctx_t *c, uint *khz);

/** Gets the default clock limit for each GPU in the node. */
state_t mgt_gpu_clock_limit_get_default(ctx_t *c, uint *khz);

/** Gets the maximum clock limit for each GPU in the node. */
state_t mgt_gpu_clock_limit_get_max(ctx_t *c, uint *khz);

/** Resets the current clock limit on each GPU to its default value. */
state_t mgt_gpu_clock_limit_reset(ctx_t *c);

/** Sets the current clock limit on each GPU (one value per GPU in the khz array). */
state_t mgt_gpu_clock_limit_set(ctx_t *c, uint *khz);

/** Gets the current power limit for each GPU in the node. */
state_t mgt_gpu_power_limit_get_current(ctx_t *c, uint *watts);

/** Gets the default power limit for each GPU in the node. */
state_t mgt_gpu_power_limit_get_default(ctx_t *c, uint *watts);

/** Gets the maximum possible power limit for each GPU in the node. */
state_t mgt_gpu_power_limit_get_max(ctx_t *c, uint *watts);

/** Resets the current power limit on each GPU to its default value. */
state_t mgt_gpu_power_limit_reset(ctx_t *c);

/** Sets the current power limit on each GPU (one value per GPU in the watts array). */
state_t mgt_gpu_power_limit_set(ctx_t *c, uint *watts);

#endif