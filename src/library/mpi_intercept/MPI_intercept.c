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


#include "MPI_interface.h"
#include <mpi.h>
#include <stdio.h>
#ifdef EAR_INTERCEPT_C_DEBUG
#define VERBOSE_INTERCEPT_C(tags,text,...) fprintf(stderr,"EAR_INTERCEPT_C[%d]: %s\n",getpid(),text);
#else
#define VERBOSE_INTERCEPT_C(tags,text,...) 
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
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Allgather...............");
    EAR_MPI_Allgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    int res = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    EAR_MPI_Allgather_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Allgather...............");
    return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Allgatherv...............");
    EAR_MPI_Allgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    int res = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    EAR_MPI_Allgatherv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Allgatherv...............");
    return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Allreduce...............");
    EAR_MPI_Allreduce_enter(sendbuf, recvbuf, count, datatype, op, comm);
    int res = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    EAR_MPI_Allreduce_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Allreduce...............");
    return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Alltoall...............");
    EAR_MPI_Alltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    int res = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    EAR_MPI_Alltoall_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Alltoall...............");
    return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Alltoallv...............");
    EAR_MPI_Alltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    int res = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    EAR_MPI_Alltoallv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Alltoallv...............");
    return res;
}

int MPI_Barrier(MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Barrier...............");
    EAR_MPI_Barrier_enter(comm);
    int res = PMPI_Barrier(comm);
    EAR_MPI_Barrier_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Barrier...............");
    return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Bcast...............");
    EAR_MPI_Bcast_enter(buffer, count, datatype, root, comm);
    int res = PMPI_Bcast(buffer, count, datatype, root, comm);
    EAR_MPI_Bcast_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Bcast...............");
    return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Bsend...............");
    EAR_MPI_Bsend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
    EAR_MPI_Bsend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Bsend...............");
    return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Bsend_init...............");
    EAR_MPI_Bsend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Bsend_init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Bsend_init...............");
    return res;
}

int MPI_Cancel(MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Cancel...............");
    EAR_MPI_Cancel_enter(request);
    int res = PMPI_Cancel(request);
    EAR_MPI_Cancel_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Cancel...............");
    return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Cart_create...............");
    EAR_MPI_Cart_create_enter(comm_old, ndims, dims, periods, reorder, comm_cart);
    int res = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
    EAR_MPI_Cart_create_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Cart_create...............");
    return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Cart_sub...............");
    EAR_MPI_Cart_sub_enter(comm, remain_dims, newcomm);
    int res = PMPI_Cart_sub(comm, remain_dims, newcomm);
    EAR_MPI_Cart_sub_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Cart_sub...............");
    return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_create...............");
    EAR_MPI_Comm_create_enter(comm, group, newcomm);
    int res = PMPI_Comm_create(comm, group, newcomm);
    EAR_MPI_Comm_create_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_create...............");
    return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_dup...............");
    EAR_MPI_Comm_dup_enter(comm, newcomm);
    int res = PMPI_Comm_dup(comm, newcomm);
    EAR_MPI_Comm_dup_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_dup...............");
    return res;
}

int MPI_Comm_free(MPI_Comm *comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_free...............");
    EAR_MPI_Comm_free_enter(comm);
    int res = PMPI_Comm_free(comm);
    EAR_MPI_Comm_free_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_free...............");
    return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_rank...............");
    EAR_MPI_Comm_rank_enter(comm, rank);
    int res = PMPI_Comm_rank(comm, rank);
    EAR_MPI_Comm_rank_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_rank...............");
    return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_size...............");
    EAR_MPI_Comm_size_enter(comm, size);
    int res = PMPI_Comm_size(comm, size);
    EAR_MPI_Comm_size_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_size...............");
    return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_spawn...............");
    EAR_MPI_Comm_spawn_enter(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    int res = PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    EAR_MPI_Comm_spawn_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_spawn...............");
    return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_spawn_multiple...............");
    EAR_MPI_Comm_spawn_multiple_enter(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    int res = PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    EAR_MPI_Comm_spawn_multiple_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_spawn_multiple...............");
    return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Comm_split...............");
    EAR_MPI_Comm_split_enter(comm, color, key, newcomm);
    int res = PMPI_Comm_split(comm, color, key, newcomm);
    EAR_MPI_Comm_split_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Comm_split...............");
    return res;
}

int MPI_File_close(MPI_File *fh) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_close...............");
    EAR_MPI_File_close_enter(fh);
    int res = PMPI_File_close(fh);
    EAR_MPI_File_close_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_close...............");
    return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_read...............");
    EAR_MPI_File_read_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_read(fh, buf, count, datatype, status);
    EAR_MPI_File_read_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_read...............");
    return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_read_all...............");
    EAR_MPI_File_read_all_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_read_all(fh, buf, count, datatype, status);
    EAR_MPI_File_read_all_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_read_all...............");
    return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_read_at...............");
    EAR_MPI_File_read_at_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_read_at(fh, offset, buf, count, datatype, status);
    EAR_MPI_File_read_at_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_read_at...............");
    return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_read_at_all...............");
    EAR_MPI_File_read_at_all_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_read_at_all(fh, offset, buf, count, datatype, status);
    EAR_MPI_File_read_at_all_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_read_at_all...............");
    return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_write...............");
    EAR_MPI_File_write_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_write(fh, buf, count, datatype, status);
    EAR_MPI_File_write_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_write...............");
    return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_write_all...............");
    EAR_MPI_File_write_all_enter(fh, buf, count, datatype, status);
    int res = PMPI_File_write_all(fh, buf, count, datatype, status);
    EAR_MPI_File_write_all_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_write_all...............");
    return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_write_at...............");
    EAR_MPI_File_write_at_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_write_at(fh, offset, buf, count, datatype, status);
    EAR_MPI_File_write_at_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_write_at...............");
    return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_File_write_at_all...............");
    EAR_MPI_File_write_at_all_enter(fh, offset, buf, count, datatype, status);
    int res = PMPI_File_write_at_all(fh, offset, buf, count, datatype, status);
    EAR_MPI_File_write_at_all_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_File_write_at_all...............");
    return res;
}

int MPI_Finalize(void) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Finalize...............");
    EAR_MPI_Finalize_enter();
    int res = PMPI_Finalize();
    EAR_MPI_Finalize_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Finalize...............");
    return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Gather...............");
    EAR_MPI_Gather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    EAR_MPI_Gather_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Gather...............");
    return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Gatherv...............");
    EAR_MPI_Gatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    int res = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    EAR_MPI_Gatherv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Gatherv...............");
    return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Get...............");
    EAR_MPI_Get_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    int res = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    EAR_MPI_Get_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Get...............");
    return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ibsend...............");
    EAR_MPI_Ibsend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Ibsend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ibsend...............");
    return res;
}

int MPI_Init(int *argc, char ***argv) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Init...............");
	printf("INITTTTT\n");
    EAR_MPI_Init_enter(argc, argv);
    int res = PMPI_Init(argc, argv);
    EAR_MPI_Init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Init...............");
    return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Init_thread...............");
    EAR_MPI_Init_thread_enter(argc, argv, required, provided);
    int res = PMPI_Init_thread(argc, argv, required, provided);
    EAR_MPI_Init_thread_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Init_thread...............");
    return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Intercomm_create...............");
    EAR_MPI_Intercomm_create_enter(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    int res = PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    EAR_MPI_Intercomm_create_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Intercomm_create...............");
    return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Intercomm_merge...............");
    EAR_MPI_Intercomm_merge_enter(intercomm, high, newintracomm);
    int res = PMPI_Intercomm_merge(intercomm, high, newintracomm);
    EAR_MPI_Intercomm_merge_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Intercomm_merge...............");
    return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iprobe...............");
    EAR_MPI_Iprobe_enter(source, tag, comm, flag, status);
    int res = PMPI_Iprobe(source, tag, comm, flag, status);
    EAR_MPI_Iprobe_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iprobe...............");
    return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Irecv...............");
    EAR_MPI_Irecv_enter(buf, count, datatype, source, tag, comm, request);
    int res = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    EAR_MPI_Irecv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Irecv...............");
    return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Irsend...............");
    EAR_MPI_Irsend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Irsend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Irsend...............");
    return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Isend...............");
    EAR_MPI_Isend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Isend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Isend...............");
    return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Issend...............");
    EAR_MPI_Issend_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Issend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Issend...............");
    return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Probe...............");
    EAR_MPI_Probe_enter(source, tag, comm, status);
    int res = PMPI_Probe(source, tag, comm, status);
    EAR_MPI_Probe_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Probe...............");
    return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Put...............");
    EAR_MPI_Put_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    int res = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    EAR_MPI_Put_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Put...............");
    return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Recv...............");
    EAR_MPI_Recv_enter(buf, count, datatype, source, tag, comm, status);
    int res = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    EAR_MPI_Recv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Recv...............");
    return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Recv_init...............");
    EAR_MPI_Recv_init_enter(buf, count, datatype, source, tag, comm, request);
    int res = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    EAR_MPI_Recv_init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Recv_init...............");
    return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Reduce...............");
    EAR_MPI_Reduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm);
    int res = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
    EAR_MPI_Reduce_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Reduce...............");
    return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Reduce_scatter...............");
    EAR_MPI_Reduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    int res = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    EAR_MPI_Reduce_scatter_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Reduce_scatter...............");
    return res;
}

int MPI_Request_free(MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Request_free...............");
    EAR_MPI_Request_free_enter(request);
    int res = PMPI_Request_free(request);
    EAR_MPI_Request_free_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Request_free...............");
    return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Request_get_status...............");
    EAR_MPI_Request_get_status_enter(request, flag, status);
    int res = PMPI_Request_get_status(request, flag, status);
    EAR_MPI_Request_get_status_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Request_get_status...............");
    return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Rsend...............");
    EAR_MPI_Rsend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
    EAR_MPI_Rsend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Rsend...............");
    return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Rsend_init...............");
    EAR_MPI_Rsend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Rsend_init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Rsend_init...............");
    return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Scan...............");
    EAR_MPI_Scan_enter(sendbuf, recvbuf, count, datatype, op, comm);
    int res = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
    EAR_MPI_Scan_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Scan...............");
    return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Scatter...............");
    EAR_MPI_Scatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    EAR_MPI_Scatter_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Scatter...............");
    return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Scatterv...............");
    EAR_MPI_Scatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    int res = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    EAR_MPI_Scatterv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Scatterv...............");
    return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Send...............");
    EAR_MPI_Send_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Send(buf, count, datatype, dest, tag, comm);
    EAR_MPI_Send_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Send...............");
    return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Send_init...............");
    EAR_MPI_Send_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Send_init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Send_init...............");
    return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Sendrecv...............");
    EAR_MPI_Sendrecv_enter(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    int res = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    EAR_MPI_Sendrecv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Sendrecv...............");
    return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Sendrecv_replace...............");
    EAR_MPI_Sendrecv_replace_enter(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    int res = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    EAR_MPI_Sendrecv_replace_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Sendrecv_replace...............");
    return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ssend...............");
    EAR_MPI_Ssend_enter(buf, count, datatype, dest, tag, comm);
    int res = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
    EAR_MPI_Ssend_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ssend...............");
    return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ssend_init...............");
    EAR_MPI_Ssend_init_enter(buf, count, datatype, dest, tag, comm, request);
    int res = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
    EAR_MPI_Ssend_init_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ssend_init...............");
    return res;
}

int MPI_Start(MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Start...............");
    EAR_MPI_Start_enter(request);
    int res = PMPI_Start(request);
    EAR_MPI_Start_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Start...............");
    return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[]) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Startall...............");
    EAR_MPI_Startall_enter(count, array_of_requests);
    int res = PMPI_Startall(count, array_of_requests);
    EAR_MPI_Startall_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Startall...............");
    return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Test...............");
    EAR_MPI_Test_enter(request, flag, status);
    int res = PMPI_Test(request, flag, status);
    EAR_MPI_Test_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Test...............");
    return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Testall...............");
    EAR_MPI_Testall_enter(count, array_of_requests, flag, array_of_statuses);
    int res = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
    EAR_MPI_Testall_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Testall...............");
    return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Testany...............");
    EAR_MPI_Testany_enter(count, array_of_requests, indx, flag, status);
    int res = PMPI_Testany(count, array_of_requests, indx, flag, status);
    EAR_MPI_Testany_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Testany...............");
    return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Testsome...............");
    EAR_MPI_Testsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    int res = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    EAR_MPI_Testsome_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Testsome...............");
    return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Wait...............");
    EAR_MPI_Wait_enter(request, status);
    int res = PMPI_Wait(request, status);
    EAR_MPI_Wait_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Wait...............");
    return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Waitall...............");
    EAR_MPI_Waitall_enter(count, array_of_requests, array_of_statuses);
    int res = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    EAR_MPI_Waitall_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Waitall...............");
    return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Waitany...............");
    EAR_MPI_Waitany_enter(count, requests, index, status);
    int res = PMPI_Waitany(count, requests, index, status);
    EAR_MPI_Waitany_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Waitany...............");
    return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Waitsome...............");
    EAR_MPI_Waitsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    int res = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    EAR_MPI_Waitsome_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Waitsome...............");
    return res;
}

int MPI_Win_complete(MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_complete...............");
    EAR_MPI_Win_complete_enter(win);
    int res = PMPI_Win_complete(win);
    EAR_MPI_Win_complete_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_complete...............");
    return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_create...............");
    EAR_MPI_Win_create_enter(base, size, disp_unit, info, comm, win);
    int res = PMPI_Win_create(base, size, disp_unit, info, comm, win);
    EAR_MPI_Win_create_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_create...............");
    return res;
}

int MPI_Win_fence(int assert, MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_fence...............");
    EAR_MPI_Win_fence_enter(assert, win);
    int res = PMPI_Win_fence(assert, win);
    EAR_MPI_Win_fence_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_fence...............");
    return res;
}

int MPI_Win_free(MPI_Win *win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_free...............");
    EAR_MPI_Win_free_enter(win);
    int res = PMPI_Win_free(win);
    EAR_MPI_Win_free_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_free...............");
    return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_post...............");
    EAR_MPI_Win_post_enter(group, assert, win);
    int res = PMPI_Win_post(group, assert, win);
    EAR_MPI_Win_post_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_post...............");
    return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_start...............");
    EAR_MPI_Win_start_enter(group, assert, win);
    int res = PMPI_Win_start(group, assert, win);
    EAR_MPI_Win_start_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_start...............");
    return res;
}

int MPI_Win_wait(MPI_Win win) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Win_wait...............");
    EAR_MPI_Win_wait_enter(win);
    int res = PMPI_Win_wait(win);
    EAR_MPI_Win_wait_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Win_wait...............");
    return res;
}


#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iallgather...............");
    EAR_MPI_Iallgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    EAR_MPI_Iallgather_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iallgather...............");
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iallgatherv...............");
    EAR_MPI_Iallgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    int res = PMPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    EAR_MPI_Iallgatherv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iallgatherv...............");
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iallreduce...............");
    EAR_MPI_Iallreduce_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = PMPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    EAR_MPI_Iallreduce_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iallreduce...............");
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ialltoall...............");
    EAR_MPI_Ialltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = PMPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    EAR_MPI_Ialltoall_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ialltoall...............");
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ialltoallv...............");
    EAR_MPI_Ialltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    int res = PMPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    EAR_MPI_Ialltoallv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ialltoallv...............");
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ibarrier...............");
    EAR_MPI_Ibarrier_enter(comm, request);
    int res = PMPI_Ibarrier(comm, request);
    EAR_MPI_Ibarrier_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ibarrier...............");
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ibcast...............");
    EAR_MPI_Ibcast_enter(buffer, count, datatype, root, comm, request);
    int res = PMPI_Ibcast(buffer, count, datatype, root, comm, request);
    EAR_MPI_Ibcast_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ibcast...............");
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Igather...............");
    EAR_MPI_Igather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    EAR_MPI_Igather_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Igather...............");
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Igatherv...............");
    EAR_MPI_Igatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    int res = PMPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    EAR_MPI_Igatherv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Igatherv...............");
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ireduce...............");
    EAR_MPI_Ireduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    int res = PMPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    EAR_MPI_Ireduce_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ireduce...............");
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Ireduce_scatter...............");
    EAR_MPI_Ireduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    int res = PMPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    EAR_MPI_Ireduce_scatter_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Ireduce_scatter...............");
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iscan...............");
    EAR_MPI_Iscan_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = PMPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    EAR_MPI_Iscan_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iscan...............");
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iscatter...............");
    EAR_MPI_Iscatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    EAR_MPI_Iscatter_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iscatter...............");
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    VERBOSE_INTERCEPT_C(VB_MPI_INT, ">> MPI_Iscatterv...............");
    EAR_MPI_Iscatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = PMPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    EAR_MPI_Iscatterv_leave();
    VERBOSE_INTERCEPT_C(VB_MPI_INT, "<< MPI_Iscatterv...............");
    return res;
}

#endif

#pragma GCC visibility pop

