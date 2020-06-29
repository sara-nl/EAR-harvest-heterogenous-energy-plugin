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

state_t mgt_gpu_load();

state_t mgt_gpu_init(ctx_t *c);

state_t mgt_gpu_dispose(ctx_t *c);

state_t mgt_gpu_count(ctx_t *c, uint *_dev_count);

state_t mgt_gpu_clock_limit_get_current(ctx_t *c, uint *khz);

state_t mgt_gpu_clock_limit_get_default(ctx_t *c, uint *khz);

state_t mgt_gpu_clock_limit_get_max(ctx_t *c, uint *khz);

state_t mgt_gpu_clock_limit_reset(ctx_t *c);

state_t mgt_gpu_clock_limit_set(ctx_t *c, uint *khz);

state_t mgt_gpu_power_limit_get_current(ctx_t *c, uint *watts);

state_t mgt_gpu_power_limit_get_default(ctx_t *c, uint *watts);

state_t mgt_gpu_power_limit_get_max(ctx_t *c, uint *watts);

state_t mgt_gpu_power_limit_reset(ctx_t *c);

state_t mgt_gpu_power_limit_set(ctx_t *c, uint *watts);

#endif
