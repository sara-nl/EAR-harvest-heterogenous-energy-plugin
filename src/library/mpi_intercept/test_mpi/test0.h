#ifndef MAIN_H
#define MAIN_H

#include <mpi.h>

#ifndef MPI3_CONST
#  if MPI_VERSION >= 3
#    define MPI3_CONST const
#  else
#    define MPI3_CONST
#  endif
#endif

#endif
