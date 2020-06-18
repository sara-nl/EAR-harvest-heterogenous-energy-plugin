/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef LIBRARY_EAR_MPI_H
#define LIBRARY_EAR_MPI_H

#include <library/api/mpi.h>

void before_init();

void after_init();

void before_mpi(mpi_call call_type, p2i buf, p2i dest);

void after_mpi(mpi_call call_type);

void before_finalize();

void after_finalize();

#endif //LIBRARY_EAR_MPI_H
