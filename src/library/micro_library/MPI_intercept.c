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
    before_mpi(Allgather, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Allgatherv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Allreduce, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Alltoall, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
    before_mpi(Alltoallv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    return res;
}

int MPI_Barrier(MPI_Comm comm) {
    before_mpi(Barrier, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Barrier(comm);
    return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    before_mpi(Bcast, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Bcast(buffer, count, datatype, root, comm);
    return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Bsend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Bsend_init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Cancel(MPI_Request *request) {
    before_mpi(Cancel, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Cancel(request);
    return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
    before_mpi(Cart_create, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
    return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
    before_mpi(Cart_sub, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Cart_sub(comm, remain_dims, newcomm);
    return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
    before_mpi(Comm_create, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_create(comm, group, newcomm);
    return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm) {
    before_mpi(Comm_dup, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_dup(comm, newcomm);
    return res;
}

int MPI_Comm_free(MPI_Comm *comm) {
    before_mpi(Comm_free, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_free(comm);
    return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) {
    before_mpi(Comm_rank, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_rank(comm, rank);
    return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size) {
    before_mpi(Comm_size, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_size(comm, size);
    return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    before_mpi(Comm_spawn, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    before_mpi(Comm_spawn_multiple, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
    before_mpi(Comm_split, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Comm_split(comm, color, key, newcomm);
    return res;
}

int MPI_File_close(MPI_File *fh) {
    before_mpi(File_close, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_close(fh);
    return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_read(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_all, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_read_all(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_at, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_read_at(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_read_at_all, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_read_at_all(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_write(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_all, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_write_all(fh, buf, count, datatype, status);
    return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_at, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_write_at(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    before_mpi(File_write_at_all, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_File_write_at_all(fh, offset, buf, count, datatype, status);
    return res;
}

int MPI_Finalize(void) {
    before_mpi(Finalize, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Finalize();
    return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Gather, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Gatherv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    before_mpi(Get, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibsend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Init(int *argc, char ***argv) {
    before_mpi(Init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Init(argc, argv);
    return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided) {
    before_mpi(Init_thread, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Init_thread(argc, argv, required, provided);
    return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
    before_mpi(Intercomm_create, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
    before_mpi(Intercomm_merge, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Intercomm_merge(intercomm, high, newintracomm);
    return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
    before_mpi(Iprobe, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iprobe(source, tag, comm, flag, status);
    return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Irecv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Irsend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Isend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Issend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Probe, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Probe(source, tag, comm, status);
    return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    before_mpi(Put, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Recv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Recv_init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    before_mpi(Reduce, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
    return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Reduce_scatter, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    return res;
}

int MPI_Request_free(MPI_Request *request) {
    before_mpi(Request_free, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Request_free(request);
    return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status) {
    before_mpi(Request_get_status, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Request_get_status(request, flag, status);
    return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Rsend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Rsend_init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    before_mpi(Scan, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
    return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Scatter, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    before_mpi(Scatterv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Send, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Send(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Send_init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Sendrecv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    before_mpi(Sendrecv_replace, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    before_mpi(Ssend, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
    return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ssend_init, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
    return res;
}

int MPI_Start(MPI_Request *request) {
    before_mpi(Start, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Start(request);
    return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[]) {
    before_mpi(Startall, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Startall(count, array_of_requests);
    return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) {
    before_mpi(Test, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Test(request, flag, status);
    return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
    before_mpi(Testall, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
    return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
    before_mpi(Testany, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Testany(count, array_of_requests, indx, flag, status);
    return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
    before_mpi(Testsome, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status) {
    before_mpi(Wait, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Wait(request, status);
    return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
    before_mpi(Waitall, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status) {
    before_mpi(Waitany, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Waitany(count, requests, index, status);
    return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
    before_mpi(Waitsome, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return res;
}

int MPI_Win_complete(MPI_Win win) {
    before_mpi(Win_complete, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_complete(win);
    return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
    before_mpi(Win_create, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_create(base, size, disp_unit, info, comm, win);
    return res;
}

int MPI_Win_fence(int assert, MPI_Win win) {
    before_mpi(Win_fence, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_fence(assert, win);
    return res;
}

int MPI_Win_free(MPI_Win *win) {
    before_mpi(Win_free, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_free(win);
    return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win) {
    before_mpi(Win_post, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_post(group, assert, win);
    return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win) {
    before_mpi(Win_start, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_start(group, assert, win);
    return res;
}

int MPI_Win_wait(MPI_Win win) {
    before_mpi(Win_wait, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Win_wait(win);
    return res;
}


#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgather, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgatherv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallreduce, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoall, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoallv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibarrier, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ibarrier(comm, request);
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibcast, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ibcast(buffer, count, datatype, root, comm, request);
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igather, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igatherv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce_scatter, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscan, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatter, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatterv, (p2i) sendbuf, (p2i) recvbuf);
    int res = PMPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return res;
}

#endif

#pragma GCC visibility pop

