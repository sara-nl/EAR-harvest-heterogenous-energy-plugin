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

#ifndef LIBRARY_LOADER_MPIC_SYMS_H
#define LIBRARY_LOADER_MPIC_SYMS_H

#include <library/api/mpi.h>

#define mpic_n 93

typedef struct mpic_s
{
	int (*Allgather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	int (*Allgatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
	int (*Allreduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*Alltoall) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	int (*Alltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
	int (*Barrier) (MPI_Comm comm);
	int (*Bcast) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
	int (*Bsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*Bsend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Cancel) (MPI_Request *request);
	int (*Cart_create) (MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
	int (*Cart_sub) (MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
	int (*Comm_create) (MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
	int (*Comm_dup) (MPI_Comm comm, MPI_Comm *newcomm);
	int (*Comm_free) (MPI_Comm *comm);
	int (*Comm_rank) (MPI_Comm comm, int *rank);
	int (*Comm_size) (MPI_Comm comm, int *size);
	int (*Comm_spawn) (MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	int (*Comm_spawn_multiple) (int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	int (*Comm_split) (MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
	int (*File_close) (MPI_File *fh);
	int (*File_read) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_read_all) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_read_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_read_at_all) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_write) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_write_all) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_write_at) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*File_write_at_all) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*Finalize) (void);
	int (*Gather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*Gatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*Get) (void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	int (*Ibsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Init) (int *argc, char ***argv);
	int (*Init_thread) (int *argc, char ***argv, int required, int *provided);
	int (*Intercomm_create) (MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
	int (*Intercomm_merge) (MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
	int (*Iprobe) (int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
	int (*Irecv) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Irsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Isend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Issend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Probe) (int source, int tag, MPI_Comm comm, MPI_Status *status);
	int (*Put) (MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	int (*Recv) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
	int (*Recv_init) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Reduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
	int (*Reduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*Request_free) (MPI_Request *request);
	int (*Request_get_status) (MPI_Request request, int *flag, MPI_Status *status);
	int (*Rsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*Rsend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Scan) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*Scatter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*Scatterv) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*Send) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*Send_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Sendrecv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	int (*Sendrecv_replace) (void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	int (*Ssend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*Ssend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*Start) (MPI_Request *request);
	int (*Startall) (int count, MPI_Request array_of_requests[]);
	int (*Test) (MPI_Request *request, int *flag, MPI_Status *status);
	int (*Testall) (int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
	int (*Testany) (int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
	int (*Testsome) (int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
	int (*Wait) (MPI_Request *request, MPI_Status *status);
	int (*Waitall) (int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
	int (*Waitany) (int count, MPI_Request *requests, int *index, MPI_Status *status);
	int (*Waitsome) (int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
	int (*Win_complete) (MPI_Win win);
	int (*Win_create) (void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
	int (*Win_fence) (int assert, MPI_Win win);
	int (*Win_free) (MPI_Win *win);
	int (*Win_post) (MPI_Group group, int assert, MPI_Win win);
	int (*Win_start) (MPI_Group group, int assert, MPI_Win win);
	int (*Win_wait) (MPI_Win win);
#if MPI_VERSION >= 3
	int (*Iallgather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*Iallgatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*Iallreduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*Ialltoall) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*Ialltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*Ibarrier) (MPI_Comm comm, MPI_Request *request);
	int (*Ibcast) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
	int (*Igather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*Igatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*Ireduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
	int (*Ireduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*Iscan) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*Iscatter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*Iscatterv) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	#endif
} mpic_t;

const char *mpic_names[] =
{
	"MPI_Allgather",
	"MPI_Allgatherv",
	"MPI_Allreduce",
	"MPI_Alltoall",
	"MPI_Alltoallv",
	"MPI_Barrier",
	"MPI_Bcast",
	"MPI_Bsend",
	"MPI_Bsend_init",
	"MPI_Cancel",
	"MPI_Cart_create",
	"MPI_Cart_sub",
	"MPI_Comm_create",
	"MPI_Comm_dup",
	"MPI_Comm_free",
	"MPI_Comm_rank",
	"MPI_Comm_size",
	"MPI_Comm_spawn",
	"MPI_Comm_spawn_multiple",
	"MPI_Comm_split",
	"MPI_File_close",
	"MPI_File_read",
	"MPI_File_read_all",
	"MPI_File_read_at",
	"MPI_File_read_at_all",
	"MPI_File_write",
	"MPI_File_write_all",
	"MPI_File_write_at",
	"MPI_File_write_at_all",
	"MPI_Finalize",
	"MPI_Gather",
	"MPI_Gatherv",
	"MPI_Get",
	"MPI_Ibsend",
	"MPI_Init",
	"MPI_Init_thread",
	"MPI_Intercomm_create",
	"MPI_Intercomm_merge",
	"MPI_Iprobe",
	"MPI_Irecv",
	"MPI_Irsend",
	"MPI_Isend",
	"MPI_Issend",
	"MPI_Probe",
	"MPI_Put",
	"MPI_Recv",
	"MPI_Recv_init",
	"MPI_Reduce",
	"MPI_Reduce_scatter",
	"MPI_Request_free",
	"MPI_Request_get_status",
	"MPI_Rsend",
	"MPI_Rsend_init",
	"MPI_Scan",
	"MPI_Scatter",
	"MPI_Scatterv",
	"MPI_Send",
	"MPI_Send_init",
	"MPI_Sendrecv",
	"MPI_Sendrecv_replace",
	"MPI_Ssend",
	"MPI_Ssend_init",
	"MPI_Start",
	"MPI_Startall",
	"MPI_Test",
	"MPI_Testall",
	"MPI_Testany",
	"MPI_Testsome",
	"MPI_Wait",
	"MPI_Waitall",
	"MPI_Waitany",
	"MPI_Waitsome",
	"MPI_Win_complete",
	"MPI_Win_create",
	"MPI_Win_fence",
	"MPI_Win_free",
	"MPI_Win_post",
	"MPI_Win_start",
	"MPI_Win_wait"
	#if MPI_VERSION >= 3
	,
	"MPI_Iallgather",
	"MPI_Iallgatherv",
	"MPI_Iallreduce",
	"MPI_Ialltoall",
	"MPI_Ialltoallv",
	"MPI_Ibarrier",
	"MPI_Ibcast",
	"MPI_Igather",
	"MPI_Igatherv",
	"MPI_Ireduce",
	"MPI_Ireduce_scatter",
	"MPI_Iscan",
	"MPI_Iscatter",
	"MPI_Iscatterv",
	#endif
};

#endif //LIBRARY_LOADER_MPIC_SYMS_H
