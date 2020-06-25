/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef MANAGEMENT_GPU_NVML
#define MANAGEMENT_GPU_NVML

state_t management_nvml_status();

state_t management_nvml_init(ctx_t *c);

state_t nvml_clock_limit_get_current(ctx_t *c, uint *khz);

state_t nvml_clock_limit_get_default(ctx_t *c, uint *khz);

state_t nvml_clock_limit_get_max(ctx_t *c, uint *khz);

state_t nvml_clock_limit_reset(ctx_t *c);

state_t nvml_clock_limit_set(ctx_t *c, uint *khz);

state_t nvml_power_limit_get_current(ctx_t *c, uint *watts);

state_t nvml_power_limit_get_default(ctx_t *c, uint *watts);

state_t nvml_power_limit_get_max(ctx_t *c, uint *watts);

state_t nvml_power_limit_reset(ctx_t *c);

state_t nvml_power_limit_set(ctx_t *c, uint *watts);

#endif