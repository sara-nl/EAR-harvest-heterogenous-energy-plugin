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

#ifndef LIBRARY_API_MPIC_H
#define LIBRARY_API_MPIC_H

#include <library/api/mpi.h>

#define API_MPIC_N 93

int api_mpic_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
int api_mpic_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
int api_mpic_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int api_mpic_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
int api_mpic_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
int api_mpic_Barrier(MPI_Comm comm);
int api_mpic_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
int api_mpic_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int api_mpic_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Cancel(MPI_Request *request);
int api_mpic_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
int api_mpic_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
int api_mpic_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
int api_mpic_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm);
int api_mpic_Comm_free(MPI_Comm *comm);
int api_mpic_Comm_rank(MPI_Comm comm, int *rank);
int api_mpic_Comm_size(MPI_Comm comm, int *size);
int api_mpic_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
int api_mpic_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
int api_mpic_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
int api_mpic_File_close(MPI_File *fh);
int api_mpic_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int api_mpic_Finalize(void);
int api_mpic_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
int api_mpic_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
int api_mpic_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int api_mpic_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Init(int *argc, char ***argv);
int api_mpic_Init_thread(int *argc, char ***argv, int required, int *provided);
int api_mpic_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
int api_mpic_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
int api_mpic_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
int api_mpic_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
int api_mpic_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int api_mpic_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
int api_mpic_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
int api_mpic_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int api_mpic_Request_free(MPI_Request *request);
int api_mpic_Request_get_status(MPI_Request request, int *flag, MPI_Status *status);
int api_mpic_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int api_mpic_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int api_mpic_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
int api_mpic_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
int api_mpic_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int api_mpic_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
int api_mpic_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
int api_mpic_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int api_mpic_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int api_mpic_Start(MPI_Request *request);
int api_mpic_Startall(int count, MPI_Request array_of_requests[]);
int api_mpic_Test(MPI_Request *request, int *flag, MPI_Status *status);
int api_mpic_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
int api_mpic_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
int api_mpic_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
int api_mpic_Wait(MPI_Request *request, MPI_Status *status);
int api_mpic_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
int api_mpic_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status);
int api_mpic_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
int api_mpic_Win_complete(MPI_Win win);
int api_mpic_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
int api_mpic_Win_fence(int assert, MPI_Win win);
int api_mpic_Win_free(MPI_Win *win);
int api_mpic_Win_post(MPI_Group group, int assert, MPI_Win win);
int api_mpic_Win_start(MPI_Group group, int assert, MPI_Win win);
int api_mpic_Win_wait(MPI_Win win);
#if MPI_VERSION >= 3
int api_mpic_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
int api_mpic_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
int api_mpic_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
int api_mpic_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
int api_mpic_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
int api_mpic_Ibarrier(MPI_Comm comm, MPI_Request *request);
int api_mpic_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
int api_mpic_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
int api_mpic_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
int api_mpic_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
int api_mpic_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
int api_mpic_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
int api_mpic_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
int api_mpic_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
#endif

const char *api_mpic_names[] __attribute__((weak)) =
{
	"api_mpic_Allgather",
	"api_mpic_Allgatherv",
	"api_mpic_Allreduce",
	"api_mpic_Alltoall",
	"api_mpic_Alltoallv",
	"api_mpic_Barrier",
	"api_mpic_Bcast",
	"api_mpic_Bsend",
	"api_mpic_Bsend_init",
	"api_mpic_Cancel",
	"api_mpic_Cart_create",
	"api_mpic_Cart_sub",
	"api_mpic_Comm_create",
	"api_mpic_Comm_dup",
	"api_mpic_Comm_free",
	"api_mpic_Comm_rank",
	"api_mpic_Comm_size",
	"api_mpic_Comm_spawn",
	"api_mpic_Comm_spawn_multiple",
	"api_mpic_Comm_split",
	"api_mpic_File_close",
	"api_mpic_File_read",
	"api_mpic_File_read_all",
	"api_mpic_File_read_at",
	"api_mpic_File_read_at_all",
	"api_mpic_File_write",
	"api_mpic_File_write_all",
	"api_mpic_File_write_at",
	"api_mpic_File_write_at_all",
	"api_mpic_Finalize",
	"api_mpic_Gather",
	"api_mpic_Gatherv",
	"api_mpic_Get",
	"api_mpic_Ibsend",
	"api_mpic_Init",
	"api_mpic_Init_thread",
	"api_mpic_Intercomm_create",
	"api_mpic_Intercomm_merge",
	"api_mpic_Iprobe",
	"api_mpic_Irecv",
	"api_mpic_Irsend",
	"api_mpic_Isend",
	"api_mpic_Issend",
	"api_mpic_Probe",
	"api_mpic_Put",
	"api_mpic_Recv",
	"api_mpic_Recv_init",
	"api_mpic_Reduce",
	"api_mpic_Reduce_scatter",
	"api_mpic_Request_free",
	"api_mpic_Request_get_status",
	"api_mpic_Rsend",
	"api_mpic_Rsend_init",
	"api_mpic_Scan",
	"api_mpic_Scatter",
	"api_mpic_Scatterv",
	"api_mpic_Send",
	"api_mpic_Send_init",
	"api_mpic_Sendrecv",
	"api_mpic_Sendrecv_replace",
	"api_mpic_Ssend",
	"api_mpic_Ssend_init",
	"api_mpic_Start",
	"api_mpic_Startall",
	"api_mpic_Test",
	"api_mpic_Testall",
	"api_mpic_Testany",
	"api_mpic_Testsome",
	"api_mpic_Wait",
	"api_mpic_Waitall",
	"api_mpic_Waitany",
	"api_mpic_Waitsome",
	"api_mpic_Win_complete",
	"api_mpic_Win_create",
	"api_mpic_Win_fence",
	"api_mpic_Win_free",
	"api_mpic_Win_post",
	"api_mpic_Win_start",
	"api_mpic_Win_wait"
#if MPI_VERSION >= 3
	,
	"api_mpic_Iallgather",
	"api_mpic_Iallgatherv",
	"api_mpic_Iallreduce",
	"api_mpic_Ialltoall",
	"api_mpic_Ialltoallv",
	"api_mpic_Ibarrier",
	"api_mpic_Ibcast",
	"api_mpic_Igather",
	"api_mpic_Igatherv",
	"api_mpic_Ireduce",
	"api_mpic_Ireduce_scatter",
	"api_mpic_Iscan",
	"api_mpic_Iscatter",
	"api_mpic_Iscatterv",
#endif
};

#endif //LIBRARY_API_MPIC_H