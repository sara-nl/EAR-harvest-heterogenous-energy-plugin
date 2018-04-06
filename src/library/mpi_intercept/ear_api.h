/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_API_H_
#define _EAR_API_H_

#include <library/mpi_intercept/MPI_types.h>
#include <library/mpi_intercept/MPI_calls_coded.h>

/** Initializes all the elements of the library as well as connecting to the daemon. */
void ear_init();
/** Given the information corresponding a MPI call, creates a DynAIS event
*   and processes it as well as creating the trace. If the library is in
*   a learning phase it does nothing. */
void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest);
/** Finalizes the processes, closing and registering metrics and traces, as well as
*   closing the connection to the daemon and releasing the memory from DynAIS. */
void ear_finalize();

#else
#endif



