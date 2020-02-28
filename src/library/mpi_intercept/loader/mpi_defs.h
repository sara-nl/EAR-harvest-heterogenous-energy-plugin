#ifndef _LOADER_DEFS_H_
#define _LOADER_DEFS_H_

#include <library/mpi_intercept/mpi.h>
#include <library/mpi_intercept/loader/mpic_syms.h>
#include <library/mpi_intercept/loader/mpif_syms.h>
#include <library/mpi_intercept/loader/mpic_syms_ear.h>
#include <library/mpi_intercept/loader/mpif_syms_ear.h>
#include <library/mpi_intercept/loader/mpic_syms_void.h>
#include <library/mpi_intercept/loader/mpif_syms_void.h>

extern mpic_mpi_t mpic_mpi;
extern mpif_mpi_t mpif_mpi;
extern mpic_ear_t mpic_ear;
extern mpif_ear_t mpif_ear;
extern mpic_ear_t mpic_void;
extern mpif_ear_t mpif_void;

#endif //_LOADER_DEFS_H_
