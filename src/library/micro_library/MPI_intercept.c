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
#include "MPI_interface.h"

#ifdef EAR_INTERCEPT_C_DEBUG
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

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    EAR_MPI_Allgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    int res = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    EAR_MPI_Allgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    int res = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    EAR_MPI_Allreduce_enter(sendbuf, recvbuf, count, datatype, op, comm);
    int res = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    EAR_MPI_Alltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    int res = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
    EAR_MPI_Alltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    int res = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    return res;
}

int MPI_Barrier(MPI_Comm comm) {
    EAR_MPI_Barrier_enter(comm);
    int res = PMPI_Barrier(comm);
    return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    EAR_MPI_Bcast_enter(buffer, count, datatype, root, comm);
    int res = PMPI_Bcast(buffer, count, datatype, root, comm);
    return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    EAR_MPI_Bsend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Bsend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Cancel(MPI_Request *request) {
    EAR_MPI_Cancel_enter(request);
    int res = PMPI_Cancel(request);
    return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
    EAR_MPI_Cart_create_enter(comm_old, ndims, dims, periods, reorder, comm_cart);
    int res = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
    return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
    EAR_MPI_Cart_sub_enter(comm, remain_dims, newcomm);
    int res = PMPI_Cart_sub(comm, remain_dims, newcomm);
    return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
    EAR_MPI_Comm_create_enter(comm, group, newcomm);
    int res = PMPI_Comm_create(comm, group, newcomm);
    return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm) {
    EAR_MPI_Comm_dup_enter(comm, newcomm);
    int res = PMPI_Comm_dup(comm, newcomm);
    return res;
}

int MPI_Comm_free(MPI_Comm *comm) {
    EAR_MPI_Comm_free_enter(comm);
    int res = PMPI_Comm_free(comm);
    return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) {
    EAR_MPI_Comm_rank_enter(comm, rank);
    int res = PMPI_Comm_rank(comm, rank);
    return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size) {
    EAR_MPI_Comm_size_enter(comm, size);
    int res = PMPI_Comm_size(comm, size);
    return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    EAR_MPI_Comm_spawn_enter(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    int res = PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    EAR_MPI_Comm_spawn_multiple_enter(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    int res = PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
    EAR_MPI_Comm_split_enter(comm, color, key, newcomm);
    int res = PMPI_Comm_split(comm, color, key, newcomm);
    return res;
}

int MPI_File_close(MPI_File *fh) {
    EAR_MPI_File_close_enter(fh);
    int res = PMPI_File_close(fh);
    return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_read_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_read(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_read_all_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_read_all(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_read_at_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_read_at(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_read_at_all_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_read_at_all(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_write_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_write(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_write_all_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_write_all(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_write_at_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_write_at(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    EAR_MPI_File_write_at_all_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_write_at_all(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_Finalize(void) {
    EAR_MPI_Finalize_enter();
    int res = PMPI_Finalize();
    return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    EAR_MPI_Gather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    EAR_MPI_Gatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    int res = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    EAR_MPI_Get_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    int res = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ibsend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Init(int *argc, char ***argv) {
    EAR_MPI_Init_enter(argc, argv);
    int res = PMPI_Init(argc, argv);
    return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided) {
    EAR_MPI_Init_thread_enter(argc, argv, required, provided);
    int res = PMPI_Init_thread(argc, argv, required, provided);
    return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
    EAR_MPI_Intercomm_create_enter(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    int res = PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
    EAR_MPI_Intercomm_merge_enter(intercomm, high, newintracomm);
    int res = PMPI_Intercomm_merge(intercomm, high, newintracomm);
    return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
    EAR_MPI_Iprobe_enter(source, tag, comm, flag, status);
    int res = PMPI_Iprobe(source, tag, comm, flag, status);
    return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Irecv_enter(buf, count, datatype, source, tag, comm, request);
    int res = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Irsend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Isend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Issend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    EAR_MPI_Probe_enter(source, tag, comm, status);
    int res = PMPI_Probe(source, tag, comm, status);
    return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    EAR_MPI_Put_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    int res = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    EAR_MPI_Recv_enter(buf, count, datatype, source, tag, comm, status);
    int res = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Recv_init_enter(buf, count, datatype, source, tag, comm, request);
    int res = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    EAR_MPI_Reduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm);
    int res = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
    return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    EAR_MPI_Reduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    int res = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    return res;
}

int MPI_Request_free(MPI_Request *request) {
    EAR_MPI_Request_free_enter(request);
    int res = PMPI_Request_free(request);
    return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status) {
    EAR_MPI_Request_get_status_enter(request, flag, status);
    int res = PMPI_Request_get_status(request, flag, status);
    return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    EAR_MPI_Rsend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Rsend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    EAR_MPI_Scan_enter(sendbuf, recvbuf, count, datatype, op, comm);
    int res = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
    return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    EAR_MPI_Scatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    EAR_MPI_Scatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    EAR_MPI_Send_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Send(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Send_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    EAR_MPI_Sendrecv_enter(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    int res = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    EAR_MPI_Sendrecv_replace_enter(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    int res = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    EAR_MPI_Ssend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ssend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Start(MPI_Request *request) {
    EAR_MPI_Start_enter(request);
    int res = PMPI_Start(request);
    return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[]) {
    EAR_MPI_Startall_enter(count, array_of_requests);
    int res = PMPI_Startall(count, array_of_requests);
    return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) {
    EAR_MPI_Test_enter(request, flag, status);
    int res = PMPI_Test(request, flag, status);
    return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
    EAR_MPI_Testall_enter(count, array_of_requests, flag, array_of_statuses);
    int res = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
    return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
    EAR_MPI_Testany_enter(count, array_of_requests, indx, flag, status);
    int res = PMPI_Testany(count, array_of_requests, indx, flag, status);
    return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
    EAR_MPI_Testsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    int res = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status) {
    EAR_MPI_Wait_enter(request, status);
    int res = PMPI_Wait(request, status);
    return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
    EAR_MPI_Waitall_enter(count, array_of_requests, array_of_statuses);
    int res = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status) {
    EAR_MPI_Waitany_enter(count, requests, index, status);
    int res = PMPI_Waitany(count, requests, index, status);
    return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
    EAR_MPI_Waitsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    int res = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return res;
}

int MPI_Win_complete(MPI_Win win) {
    EAR_MPI_Win_complete_enter(win);
    int res = PMPI_Win_complete(win);
    return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
    EAR_MPI_Win_create_enter(base, size, disp_unit, info, comm, win);
    int res = PMPI_Win_create(base, size, disp_unit, info, comm, win);
    return res;
}

int MPI_Win_fence(int assert, MPI_Win win) {
    EAR_MPI_Win_fence_enter(assert, win);
    int res = PMPI_Win_fence(assert, win);
    return res;
}

int MPI_Win_free(MPI_Win *win) {
    EAR_MPI_Win_free_enter(win);
    int res = PMPI_Win_free(win);
    return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win) {
    EAR_MPI_Win_post_enter(group, assert, win);
    int res = PMPI_Win_post(group, assert, win);
    return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win) {
    EAR_MPI_Win_start_enter(group, assert, win);
    int res = PMPI_Win_start(group, assert, win);
    return res;
}

int MPI_Win_wait(MPI_Win win) {
    EAR_MPI_Win_wait_enter(win);
    int res = PMPI_Win_wait(win);
    return res;
}


#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iallgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iallgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    int res = PMPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iallreduce_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = PMPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ialltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = PMPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ialltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    int res = PMPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ibarrier_enter(comm, request);
    int res = PMPI_Ibarrier(comm, request);
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ibcast_enter(buffer, count, datatype, root, comm, request);
    int res = PMPI_Ibcast(buffer, count, datatype, root, comm, request);
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Igather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Igatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    int res = PMPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ireduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    int res = PMPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Ireduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    int res = PMPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iscan_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = PMPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iscatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    EAR_MPI_Iscatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

#endif

#pragma GCC visibility pop

