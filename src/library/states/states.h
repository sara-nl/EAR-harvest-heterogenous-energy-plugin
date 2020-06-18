/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _EAR_BASE_H_
#define _EAR_BASE_H

/** Executed at application start. Reads EAR configuration env. variables */
void states_begin_job(int my_id,  char *app_name);

/** Executed at NEW_LOOP or END_NEW_LOOP DynAIS event */
void states_begin_period(int my_id, ulong event, ulong size,ulong level);

/** Executed at NEW_ITERATION DynAIS event */
void states_new_iteration(int my_id, uint size, uint iterations, uint level, ulong event,ulong mpi_calls_iter);

/** Executed at END_LOOP or END_NEW_LOOP DynAIS event */
void states_end_period(uint iterations);

/** Executed at application end */
void states_end_job(int my_id, char *app_name);

/** Returns the current EAR state */
int states_my_state();

/** */
ulong select_near_freq(ulong avg);

#endif
