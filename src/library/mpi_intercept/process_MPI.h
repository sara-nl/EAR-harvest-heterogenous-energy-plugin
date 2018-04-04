/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  Authors: Julita Corbalan (julita.corbalan@bsc.es) 
    and Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef PROCESS_MPI_H
#define PROCESS_MPI_H

#include <library/mpi_intercept/MPI_types.h>

// called before the MPI_init. Connects with ear_daemon
void before_init();
// called after the MPI_init
void after_init();
// called before all the mpi calls except mpi_init and mpi_finalize. It calls dynais and, based on 
// dynais reported status, calls the main EAR functionallity
void before_mpi(int call, p2i buf, p2i dest);
// called after all the mpi calls excel mpi_init and mpi_finalize. 
void after_mpi();
// called before mpi_finalize, it calls EAR functions to summarize metrics
void before_finalize();
// called after mpi_finalize
void after_finalize();

#endif //PROCESS_MPI_H
