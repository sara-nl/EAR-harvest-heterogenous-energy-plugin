/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_TRACER_MPI_H
#define EAR_TRACER_MPI_H

void traces_mpi_init();

void traces_mpi_call(int global_rank, int local_rank, ulong time, ulong ev, ulong a1, ulong a2, ulong a3);

void traces_mpi_end();

#endif //EAR_TRACER_MPI_H
