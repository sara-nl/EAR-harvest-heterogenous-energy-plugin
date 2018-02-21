/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_API_H_
#define _EAR_API_H_

#include <library/ear_dyn_inst/MPI_types.h>
#include <library/ear_dyn_inst/MPI_calls_coded.h>

void ear_init();
void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest);
void ear_finalize();

#else
#endif



