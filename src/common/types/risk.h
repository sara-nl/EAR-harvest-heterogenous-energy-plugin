/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef RISK_LEVEL_H
#define RISK_LEVEL_H

#include <common/states.h>
#define WARNING1	0x01
#define WARNING2  0x02
#define PANIC			0x04
typedef unsigned int risk_t;
state_t set_risk(risk_t *r,risk_t new_r);
int is_risk_set(risk_t r,risk_t value);
state_t add_risk(risk_t *r,risk_t value);
state_t del_risk(risk_t *r,risk_t value);

#endif

