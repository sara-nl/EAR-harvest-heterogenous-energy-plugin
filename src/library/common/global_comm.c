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
#include <common/config.h>
#include <common/output/verbose.h>
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

void check_node_signatures(masters_info_t *mi,lib_shared_data_t *data,shsignature_t *sig)
{
    #if SHARE_INFO_PER_PROCESS
    int max_ppn=mi->max_ppn;
    #endif
    #if SHARE_INFO_PER_NODE
    int max_ppn=1;
    #endif

    /* If the master signature is ready we check the others */
  if ((mi->my_master_rank>=0) && sig[0].ready){
    if (are_signatures_ready(data,sig)){
      print_shared_signatures(data,sig);
      clean_signatures(data,sig);
      if (!mi->node_info_pending){
        #if SHARE_INFO_PER_PROCESS
        copy_my_mpi_info(data,sig,mi->my_mpi_info);
        #endif
        #if SHARE_INFO_PER_NODE
        compute_per_node_mpi_info(data,sig,mi->my_mpi_info);
        #endif
        if (ishare_global_info(mi->masters_comm,(char *)mi->my_mpi_info,
          sizeof(mpi_information_t)*max_ppn,
          (char *)mi->nodes_info,sizeof(mpi_information_t)*max_ppn,&mi->req)!=EAR_SUCCESS){
          error("Sending node mpi_info_t to other masters master_rank %d",mi->my_master_rank);
        }else{
          verbose(1,"mpi_info for master_rank %d sent to other nodes",mi->my_master_rank);
          mi->node_info_pending=1;
        }
      }
    }
  }
}


void check_mpi_info(masters_info_t *mi,int *node_cp,int *rank_cp)
{
  *node_cp=-1;
  *rank_cp=-1;
  if ((mi->my_master_rank>=0) && mi->node_info_pending && (is_info_ready(&mi->req)==EAR_SUCCESS)){
    #if SHARE_INFO_PER_PROCESS
    int max_ppn=mi->max_ppn;
    #endif
    #if SHARE_INFO_PER_NODE
    int max_ppn=1;
    #endif

    verbose(1,"Info received in master %d",mi->my_master_rank);
    select_global_cp(mi->my_master_size,max_ppn,mi->ppn,mi->nodes_info,node_cp,rank_cp);
    verbose(1,"Global CP is node %d rank %d",*node_cp,*rank_cp);
    mi->node_info_pending=0;
  }
}

