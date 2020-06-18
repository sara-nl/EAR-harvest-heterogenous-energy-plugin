/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef MODEL_API_H
#define MODEL_API_H
#include <common/states.h>
#include <common/types/signature.h>
#include <common/hardware/architecture.h>


/* This function loads any information needed by the energy model */
state_t model_init(char *etc,char *tmp,architecture_t * arch_desc);

state_t model_project_time(signature_t *sign,ulong from,ulong to,double *ptime);

state_t model_project_power(signature_t *sign, ulong from,ulong to,double *ppower);

state_t model_projection_available(ulong from,ulong to);
#endif

