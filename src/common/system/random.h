/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_COMMON_RANDOM_H
#define EAR_COMMON_RANDOM_H

#include <common/types/generic.h>

/* Gets a 32 bits random number, it uses hardware functions which
 * spends up to 300 cycles, so use it carefully. */
uint random_get();

/* Gets a 32 bits random number between min and min + offset, it uses
 * random_get() internally, so use it carefully. */
uint random_getrank(uint min, uint offset);

#endif //EAR_COMMON_TIME_H
