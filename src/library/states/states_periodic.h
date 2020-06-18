/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _STATES_PERIODIC_H_
#define _STATES_PERIODIC_H_

/** Executed at job end */
void states_periodic_end_job(int my_id, FILE *ear_fd, char *app_name);

/** Executed at job end */
void states_periodic_begin_job(int my_id, FILE *ear_fd, char *app_name);

/** It must be executed when EAR goes to periodic mode. states_periodic_new_iteration must be executed after that when going to periodic mode */
void states_periodic_begin_period(int my_id, FILE *ear_fd, unsigned long event, unsigned int size);

/** It must be executed when EAR is running in periodic mode. It is called at the end/begin of each period (called just once) */
void states_periodic_end_period(uint iterations);


/** Executed every N mpi calls */
void states_periodic_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event, ulong mpi_calls_iter);
#else
#endif

