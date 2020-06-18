/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _EAR_API_H_
#define _EAR_API_H_

#include <library/api/mpi.h>

/** Initializes all the elements of the library as well as connecting to the daemon. */
void ear_init();

/** Given the information corresponding a MPI call, creates a DynAIS event
*   and processes it as well as creating the trace. If the library is in
*   a learning phase it does nothing. */
void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest);

/** Finalizes the processes, closing and registering metrics and traces, as well as
*   closing the connection to the daemon and releasing the memory from DynAIS. */
void ear_finalize();

/***** API for manual application modification *********/
void ear_new_iteration(unsigned long loop_id);

void ear_end_loop(unsigned long loop_id);

unsigned long ear_new_loop();

#endif
