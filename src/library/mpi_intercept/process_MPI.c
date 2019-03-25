/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


#include <mpi.h>

#include <common/config.h>
#if IN_MPI_TIME
#include <papi.h>
#include <common/math_operations.h>
#endif
#include <library/mpi_intercept/ear_api.h>

#if IN_MPI_TIME
static long long ear_in_mpi=0;
static long long begin_in_mpi,end_mpi_time;
long long ear_total_in_mpi=0;
long long ear_iteration_in_mpi=0;
#endif
void before_init(){
}

void after_init(){
	ear_init();
}
void before_mpi(mpi_call call_type, p2i buf, p2i dest) {
	ear_mpi_call(call_type,buf,dest);
	#if IN_MPI_TIME
	begin_in_mpi=PAPI_get_real_usec();
	#endif
}

void after_mpi(mpi_call call_type){
	#if IN_MPI_TIME
	end_mpi_time=PAPI_get_real_usec();
	if (end_mpi_time>begin_in_mpi) ear_in_mpi=end_mpi_time-begin_in_mpi;
	else ear_in_mpi= llong_diff_overflow(begin_in_mpi,end_mpi_time);
	ear_total_in_mpi+=ear_in_mpi;
	ear_iteration_in_mpi+=ear_in_mpi;
	#endif
}

void before_finalize() {
	ear_finalize();
}

void after_finalize() {
}


