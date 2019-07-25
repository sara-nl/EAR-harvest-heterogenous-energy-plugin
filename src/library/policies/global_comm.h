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

#ifndef _GLOBAL_COMM_H
#define _GLOBAL_COMM_H
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


state_t ishare_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv,MPI_Request *req);

state_t share_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv);

state_t is_info_ready(MPI_Request *req);

state_t wait_for_data(MPI_Request *req);
#endif
