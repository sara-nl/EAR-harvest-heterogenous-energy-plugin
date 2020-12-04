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

/*
 * Usage:
 * Just call dynais() passing the sample and the size of
 * this sample. It will be returned one of these states:
 *      END_LOOP
 *      NO_LOOP
 *      IN_LOOP
 *      NEW_ITERATION
 *      NEW_LOOP
 *      END_NEW_LOOP
 *
 * Errors:
 * A returned dynais_init() value different than 0,
 * means that something went wrong while allocating
 * memory.
 *
 * You HAVE to call dynais_init() method before with
 * the window length and the number of levels and
 * dynais_dispose() at the end of the execution.
 *
 * You can also set the METRICS define to 1 in case you
 * want some metrics at the end of the execution. Moreover,
 * you can increase MAX_LEVELS in case you need more or
 * METRICS_WINDOW, used to store the information of the
 * different loops found (sorted by size) because could
 * be necessary if you want to test big single level
 * windows.
 */

#ifndef DYNAIS_H
#define DYNAIS_H

#include <common/types/generic.h>
#include <common/hardware/topology.h>

#define MAX_LEVELS      10
#define METRICS_WINDOW  40000

// DynAIS output states
#define END_LOOP       -1
#define NO_LOOP         0
#define IN_LOOP         1
#define NEW_ITERATION   2
#define NEW_LOOP        3
#define END_NEW_LOOP    4

typedef int (*dynais_call_t) (uint sample, uint *size, uint *level);

dynais_call_t dynais_init(topology_t *tp, uint window, uint levels);

void dynais_dispose();

int dynais_build_type();

uint dynais_sample_convert(ulong sample);

#endif //DYNAIS_H
