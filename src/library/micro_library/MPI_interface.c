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
#include <stdio.h>
#include "process_MPI.h"
#include "MPI_calls_coded.h"
#include "MPI_interface.h"

#ifdef EAR_INTERFACE_C_DEBUG
#else
#endif

#ifndef MPI3_CONST
#  if MPI_VERSION >= 3
#    define MPI3_CONST const
#  else
#    define MPI3_CONST
#  endif
#endif

#pragma GCC visibility push(default)

void EAR_MPI_Allgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Allgather, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Allgather_leave(void) {
    after_mpi(Allgather);
}

void EAR_MPI_Allgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Allgatherv,(p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Allgatherv_leave(void) {
    after_mpi(Allgatherv);
}

void EAR_MPI_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Allreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Allreduce_leave(void) {
    after_mpi(Allreduce);
}

void EAR_MPI_Alltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Alltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Alltoall_leave(void) {
    after_mpi(Alltoall);
}

void EAR_MPI_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Alltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Alltoallv_leave(void) {
    after_mpi(Alltoallv);
}

void EAR_MPI_Barrier_enter(MPI_Comm comm) {
    before_mpi(Barrier, (p2i)comm,0);
}

void EAR_MPI_Barrier_leave(void) {
    after_mpi(Barrier);
}

void EAR_MPI_Bcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    before_mpi(Bcast, (p2i)comm,0);
}

void EAR_MPI_Bcast_leave(void) {
    after_mpi(Bcast);
}

void EAR_MPI_Bsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Bsend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Bsend_leave(void) {
    after_mpi(Bsend);
}

void EAR_MPI_Bsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Bsend_init, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Bsend_init_leave(void) {
    after_mpi(Bsend_init);
}

void EAR_MPI_Cancel_enter(MPI_Request *request) {
    before_mpi(Cancel, (p2i)request,(p2i)0);
}

void EAR_MPI_Cancel_leave(void) {
    after_mpi(Cancel);
}

void EAR_MPI_Cart_create_enter(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
    before_mpi(Cart_create, (p2i)ndims,(p2i)comm_cart);

}

void EAR_MPI_Cart_create_leave(void) {
    after_mpi(Cart_create);
}

void EAR_MPI_Cart_sub_enter(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
    before_mpi(Cart_sub, (p2i)remain_dims,(p2i)newcomm);
}

void EAR_MPI_Cart_sub_leave(void) {
    after_mpi(Cart_sub);
}

void EAR_MPI_Comm_create_enter(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
    before_mpi(Comm_create, (p2i)group,(p2i)newcomm);
}

void EAR_MPI_Comm_create_leave(void) {
    after_mpi(Comm_create);
}

void EAR_MPI_Comm_dup_enter(MPI_Comm comm, MPI_Comm *newcomm) {
    before_mpi(Comm_dup, (p2i)newcomm,(p2i)0);
}

void EAR_MPI_Comm_dup_leave(void) {
    after_mpi(Comm_dup);
}

void EAR_MPI_Comm_free_enter(MPI_Comm *comm) {
    before_mpi(Comm_free, (p2i)comm,(p2i)0);
}

void EAR_MPI_Comm_free_leave(void) {
    after_mpi(Comm_free);
}

void EAR_MPI_Comm_rank_enter(MPI_Comm comm, int *rank) {
    before_mpi(Comm_rank, (p2i)comm,(p2i)rank);
}

void EAR_MPI_Comm_rank_leave(void) {
    after_mpi(Comm_rank);
}

void EAR_MPI_Comm_size_enter(MPI_Comm comm, int *size) {
    before_mpi(Comm_size, (p2i)size,(p2i)0);
}

void EAR_MPI_Comm_size_leave(void) {
    after_mpi(Comm_size);
}

void EAR_MPI_Comm_spawn_enter(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    before_mpi(Comm_spawn, (p2i)command,(p2i)info);
}

void EAR_MPI_Comm_spawn_leave(void) {
    after_mpi(Comm_spawn);
}

void EAR_MPI_Comm_spawn_multiple_enter(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    before_mpi(Comm_spawn_multiple, (p2i)array_of_commands,(p2i)array_of_info);
}

void EAR_MPI_Comm_spawn_multiple_leave(void) {
    after_mpi(Comm_spawn_multiple);
}

void EAR_MPI_Comm_split_enter(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
    before_mpi(Comm_split, (p2i)key,(p2i)newcomm);
}

void EAR_MPI_Comm_split_leave(void) {
    after_mpi(Comm_split);
}

void EAR_MPI_File_close_enter(MPI_File *fh) {
    before_mpi(File_close, (p2i)fh,(p2i)0);
}

void EAR_MPI_File_close_leave(void) {
    after_mpi(File_close);
}

void EAR_MPI_File_read_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_read_leave(void) {
    after_mpi(File_read);
}

void EAR_MPI_File_read_all_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_all, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_read_all_leave(void) {
    after_mpi(File_read_all);
}

void EAR_MPI_File_read_at_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_at, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_read_at_leave(void) {
    after_mpi(File_read_at);
}

void EAR_MPI_File_read_at_all_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_at_all, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_read_at_all_leave(void) {
    after_mpi(File_read_at_all);
}

void EAR_MPI_File_write_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_write_leave(void) {
    after_mpi(File_write);
}

void EAR_MPI_File_write_all_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_all, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_write_all_leave(void) {
    after_mpi(File_write_all);
}

void EAR_MPI_File_write_at_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_at, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_write_at_leave(void) {
    after_mpi(File_write_at);
}

void EAR_MPI_File_write_at_all_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_at_all, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_File_write_at_all_leave(void) {
    after_mpi(File_write_at_all);
}

void EAR_MPI_Finalize_enter(void) {
    before_finalize();
}

void EAR_MPI_Finalize_leave(void) {
    after_finalize();
}

void EAR_MPI_Gather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Gather, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Gather_leave(void) {
    after_mpi(Gather);
}

void EAR_MPI_Gatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Gatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Gatherv_leave(void) {
    after_mpi(Gatherv);
}

void EAR_MPI_Get_enter(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    before_mpi(Get, (p2i)origin_addr,(p2i)origin_datatype);
}

void EAR_MPI_Get_leave(void) {
    after_mpi(Get);
}

void EAR_MPI_Ibsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibsend, (p2i)buf,(p2i)datatype);
}

void EAR_MPI_Ibsend_leave(void) {
    after_mpi(Ibsend);
}

void EAR_MPI_Init_enter(int *argc, char ***argv) {
    before_init();
}

void EAR_MPI_Init_leave(void) {
    after_init();
}

void EAR_MPI_Init_thread_enter(int *argc, char ***argv, int required, int *provided) {
    before_init();
}

void EAR_MPI_Init_thread_leave(void) {
    after_init();
}

void EAR_MPI_Intercomm_create_enter(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
    before_mpi(Intercomm_create, (p2i)local_leader,(p2i)remote_leader);
}

void EAR_MPI_Intercomm_create_leave(void) {
    after_mpi(Intercomm_create);
}

void EAR_MPI_Intercomm_merge_enter(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
    before_mpi(Intercomm_merge, (p2i)newintracomm,(p2i)0);
}

void EAR_MPI_Intercomm_merge_leave(void) {
    after_mpi(Intercomm_merge);
}

void EAR_MPI_Iprobe_enter(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
    before_mpi(Iprobe, (p2i)flag,(p2i)status);
}

void EAR_MPI_Iprobe_leave(void) {
    after_mpi(Iprobe);
}

void EAR_MPI_Irecv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Irecv, (p2i)buf,(p2i)request);
}

void EAR_MPI_Irecv_leave(void) {
    after_mpi(Irecv);
}

void EAR_MPI_Irsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Irsend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Irsend_leave(void) {
    after_mpi(Irsend);
}

void EAR_MPI_Isend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Isend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Isend_leave(void) {
    after_mpi(Isend);
}

void EAR_MPI_Issend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Issend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Issend_leave(void) {
    after_mpi(Issend);
}

void EAR_MPI_Probe_enter(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Probe, (p2i)source,(p2i)0);
}

void EAR_MPI_Probe_leave(void) {
    after_mpi(Probe);
}

void EAR_MPI_Put_enter(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    before_mpi(Put, (p2i)origin_addr,(p2i)0);
}

void EAR_MPI_Put_leave(void) {
    after_mpi(Put);
}

void EAR_MPI_Recv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Recv, (p2i)buf,(p2i)source);
}

void EAR_MPI_Recv_leave(void) {
    after_mpi(Recv);
}

void EAR_MPI_Recv_init_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Recv_init, (p2i)buf,(p2i)source);
}

void EAR_MPI_Recv_init_leave(void) {
    after_mpi(Recv_init);
}

void EAR_MPI_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    before_mpi(Reduce, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Reduce_leave(void) {
    after_mpi(Reduce);
}

void EAR_MPI_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Reduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Reduce_scatter_leave(void) {
    after_mpi(Reduce_scatter);
}

void EAR_MPI_Request_free_enter(MPI_Request *request) {
    before_mpi(Request_free, (p2i)request,(p2i)0);
}

void EAR_MPI_Request_free_leave(void) {
    after_mpi(Request_free);
}

void EAR_MPI_Request_get_status_enter(MPI_Request request, int *flag, MPI_Status *status) {
    before_mpi(Request_get_status, (p2i)request,(p2i)0);
}

void EAR_MPI_Request_get_status_leave(void) {
    after_mpi(Request_get_status);
}

void EAR_MPI_Rsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Rsend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Rsend_leave(void) {
    after_mpi(Rsend);
}

void EAR_MPI_Rsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Rsend_init, (p2i)buf,(p2i)0);
}

void EAR_MPI_Rsend_init_leave(void) {
    after_mpi(Rsend_init);
}

void EAR_MPI_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Scan, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Scan_leave(void) {
    after_mpi(Scan);
}

void EAR_MPI_Scatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Scatter_leave(void) {
    after_mpi(Scatter);
}

void EAR_MPI_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Scatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Scatterv_leave(void) {
    after_mpi(Scatterv);
}

void EAR_MPI_Send_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Send, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Send_leave(void) {
    after_mpi(Send);
}

void EAR_MPI_Send_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Send_init, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Send_init_leave(void) {
    after_mpi(Send_init);
}

void EAR_MPI_Sendrecv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Sendrecv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Sendrecv_leave(void) {
    after_mpi(Sendrecv);
}

void EAR_MPI_Sendrecv_replace_enter(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Sendrecv_replace, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Sendrecv_replace_leave(void) {
    after_mpi(Sendrecv_replace);
}

void EAR_MPI_Ssend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Ssend, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Ssend_leave(void) {
    after_mpi(Ssend);
}

void EAR_MPI_Ssend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ssend_init, (p2i)buf,(p2i)dest);
}

void EAR_MPI_Ssend_init_leave(void) {
    after_mpi(Ssend_init);
}

void EAR_MPI_Start_enter(MPI_Request *request) {
    before_mpi(Start, (p2i)request,(p2i)0);
}

void EAR_MPI_Start_leave(void) {
    after_mpi(Start);
}

void EAR_MPI_Startall_enter(int count, MPI_Request array_of_requests[]) {
    before_mpi(Startall, (p2i)array_of_requests,(p2i)0);
}

void EAR_MPI_Startall_leave(void) {
    after_mpi(Startall);
}

void EAR_MPI_Test_enter(MPI_Request *request, int *flag, MPI_Status *status) {
    before_mpi(Test, (p2i)request,(p2i)0);
}

void EAR_MPI_Test_leave(void) {
    after_mpi(Test);
}

void EAR_MPI_Testall_enter(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
    before_mpi(Testall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void EAR_MPI_Testall_leave(void) {
    after_mpi(Testall);
}

void EAR_MPI_Testany_enter(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
    before_mpi(Testany, (p2i)array_of_requests,(p2i)flag);
}

void EAR_MPI_Testany_leave(void) {
    after_mpi(Testany);
}

void EAR_MPI_Testsome_enter(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
    before_mpi(Testsome, (p2i)array_of_requests,(p2i)outcount);
}

void EAR_MPI_Testsome_leave(void) {
    after_mpi(Testsome);
}

void EAR_MPI_Wait_enter(MPI_Request *request, MPI_Status *status) {
    before_mpi(Wait, (p2i)request,(p2i)0);
}

void EAR_MPI_Wait_leave(void) {
    after_mpi(Wait);
}

void EAR_MPI_Waitall_enter(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
    before_mpi(Waitall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void EAR_MPI_Waitall_leave(void) {
    after_mpi(Waitall);
}

void EAR_MPI_Waitany_enter(int count, MPI_Request *requests, int *index, MPI_Status *status) {
    before_mpi(Waitany, (p2i)requests,(p2i)index);
}

void EAR_MPI_Waitany_leave(void) {
    after_mpi(Waitany);
}

void EAR_MPI_Waitsome_enter(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
    before_mpi(Waitsome, (p2i)array_of_requests,(p2i)outcount);
}

void EAR_MPI_Waitsome_leave(void) {
    after_mpi(Waitsome);
}

void EAR_MPI_Win_complete_enter(MPI_Win win) {
    before_mpi(Win_complete, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_complete_leave(void) {
    after_mpi(Win_complete);
}

void EAR_MPI_Win_create_enter(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
    before_mpi(Win_create, (p2i)base,(p2i)info);
}

void EAR_MPI_Win_create_leave(void) {
    after_mpi(Win_create);
}

void EAR_MPI_Win_fence_enter(int assert, MPI_Win win) {
    before_mpi(Win_fence, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_fence_leave(void) {
    after_mpi(Win_fence);
}

void EAR_MPI_Win_free_enter(MPI_Win *win) {
    before_mpi(Win_free, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_free_leave(void) {
    after_mpi(Win_free);
}

void EAR_MPI_Win_post_enter(MPI_Group group, int assert, MPI_Win win) {
    before_mpi(Win_post, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_post_leave(void) {
    after_mpi(Win_post);
}

void EAR_MPI_Win_start_enter(MPI_Group group, int assert, MPI_Win win) {
    before_mpi(Win_start, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_start_leave(void) {
    after_mpi(Win_start);
}

void EAR_MPI_Win_wait_enter(MPI_Win win) {
    before_mpi(Win_wait, (p2i)win,(p2i)0);
}

void EAR_MPI_Win_wait_leave(void) {
    after_mpi(Win_wait);
}


#if MPI_VERSION >= 3
void EAR_MPI_Iallgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgather, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iallgather_leave(void) {
    after_mpi(Iallgather);
}

void EAR_MPI_Iallgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iallgatherv_leave(void) {
    after_mpi(Iallgatherv);
}

void EAR_MPI_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iallreduce_leave(void) {
    after_mpi(Iallreduce);
}

void EAR_MPI_Ialltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Ialltoall_leave(void) {
    after_mpi(Ialltoall);
}

void EAR_MPI_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Ialltoallv_leave(void) {
    after_mpi(Ialltoallv);
}

void EAR_MPI_Ibarrier_enter(MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibarrier, (p2i)request,(p2i)0);
}

void EAR_MPI_Ibarrier_leave(void) {
    after_mpi(Ibarrier);
}

void EAR_MPI_Ibcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibcast, (p2i)buffer,(p2i)request);
}

void EAR_MPI_Ibcast_leave(void) {
    after_mpi(Ibcast);
}

void EAR_MPI_Igather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igather, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Igather_leave(void) {
    after_mpi(Igather);
}

void EAR_MPI_Igatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Igatherv_leave(void) {
    after_mpi(Igatherv);
}

void EAR_MPI_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Ireduce_leave(void) {
    after_mpi(Ireduce);
}

void EAR_MPI_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Ireduce_scatter_leave(void) {
    after_mpi(Ireduce_scatter);
}

void EAR_MPI_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscan, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iscan_leave(void) {
    after_mpi(Iscan);
}

void EAR_MPI_Iscatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatter, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iscatter_leave(void) {
    after_mpi(Iscatter);
}

void EAR_MPI_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void EAR_MPI_Iscatterv_leave(void) {
    after_mpi(Iscatterv);
}

#endif

#pragma GCC visibility pop

