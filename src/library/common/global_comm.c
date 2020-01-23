/**************************************************************
* *       Energy Aware Runtime (EAR)
* *       This program is part of the Energy Aware Runtime (EAR).
* *
* *       EAR provides a dynamic, transparent and ligth-weigth solution for
* *       Energy management.
* *
* *       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
* *
* *       Copyright (C) 2017  
* *       BSC Contact     mailto:ear-support@bsc.es
* *       Lenovo contact  mailto:hpchelp@lenovo.com
* *
* *       EAR is free software; you can redistribute it and/or
* *       modify it under the terms of the GNU Lesser General Public
* *       License as published by the Free Software Foundation; either
* *       version 2.1 of the License, or (at your option) any later version.
* *       
* *       EAR is distributed in the hope that it will be useful,
* *       but WITHOUT ANY WARRANTY; without even the implied warranty of
* *       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* *       Lesser General Public License for more details.
* *       
* *       You should have received a copy of the GNU Lesser General Public
* *       License along with EAR; if not, write to the Free Software
* *       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
* *       The GNU LEsser General Public License is contained in the file COPYING  
* */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/states.h> 
#include <library/common/global_comm.h>


state_t ishare_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv,MPI_Request *req)
{
	if ((data_send == NULL) || (data_recv==NULL))	return EAR_ERROR;
	if ((size_send==0) || (size_recv==0)) return EAR_ERROR;
	PMPI_Iallgather(data_send, size_send, MPI_SIGNED_CHAR, 	
									data_recv, size_recv, MPI_SIGNED_CHAR, comm,req);
	return EAR_SUCCESS;
}

state_t share_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv)
{
	PMPI_Allgather(data_send, size_send, MPI_SIGNED_CHAR, 	
									data_recv, size_recv, MPI_SIGNED_CHAR, comm);
	return EAR_SUCCESS;
}


state_t is_info_ready(MPI_Request *req)
{
	int flag;
	PMPI_Test(req, &flag, MPI_STATUS_IGNORE);
	if (flag) return EAR_SUCCESS;
	else return EAR_NOT_READY;
}

state_t wait_for_data(MPI_Request *req)
{
	
	PMPI_Wait(req, MPI_STATUS_IGNORE);
	return EAR_SUCCESS;
}
