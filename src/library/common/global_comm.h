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

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <library/common/library_shared_data.h>

typedef struct masters_info{
	MPI_Comm masters_comm;
	MPI_Comm new_world_comm;
	int my_master_rank;
	int my_master_size;
	int *ppn;
	int max_ppn;
	mpi_information_t *my_mpi_info;
	mpi_information_t *nodes_info;
	int node_info_pending;
	MPI_Request req;
}masters_info_t;

state_t ishare_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv,MPI_Request *req);

state_t share_global_info(MPI_Comm comm,char * data_send, int size_send,char * data_recv, int size_recv);

state_t is_info_ready(MPI_Request *req);

state_t wait_for_data(MPI_Request *req);


void check_mpi_info(masters_info_t *mi,int *node_cp,int *rank_cp,int show_sig);
void print_mpi_info(masters_info_t *mi);
void check_node_signatures(masters_info_t *mi,lib_shared_data_t *data,shsignature_t *sig,int show_sig);
void detect_load_unbalance(masters_info_t *mi);

#endif
