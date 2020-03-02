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

#ifndef LIBRARY_API_EARC_SYMS_H
#define LIBRARY_API_EARC_SYMS_H

#include <library/api/mpi.h>

#define earc_n 93*2

void earc_Allgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void earc_Allgather_leave(void);
void earc_Allgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
void earc_Allgatherv_leave(void);
void earc_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void earc_Allreduce_leave(void);
void earc_Alltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void earc_Alltoall_leave(void);
void earc_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
void earc_Alltoallv_leave(void);
void earc_Barrier_enter(MPI_Comm comm);
void earc_Barrier_leave(void);
void earc_Bcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
void earc_Bcast_leave(void);
void earc_Bsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void earc_Bsend_leave(void);
void earc_Bsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Bsend_init_leave(void);
void earc_Cancel_enter(MPI_Request *request);
void earc_Cancel_leave(void);
void earc_Cart_create_enter(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
void earc_Cart_create_leave(void);
void earc_Cart_sub_enter(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
void earc_Cart_sub_leave(void);
void earc_Comm_create_enter(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
void earc_Comm_create_leave(void);
void earc_Comm_dup_enter(MPI_Comm comm, MPI_Comm *newcomm);
void earc_Comm_dup_leave(void);
void earc_Comm_free_enter(MPI_Comm *comm);
void earc_Comm_free_leave(void);
void earc_Comm_rank_enter(MPI_Comm comm, int *rank);
void earc_Comm_rank_leave(void);
void earc_Comm_size_enter(MPI_Comm comm, int *size);
void earc_Comm_size_leave(void);
void earc_Comm_spawn_enter(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void earc_Comm_spawn_leave(void);
void earc_Comm_spawn_multiple_enter(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void earc_Comm_spawn_multiple_leave(void);
void earc_Comm_split_enter(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
void earc_Comm_split_leave(void);
void earc_File_close_enter(MPI_File *fh);
void earc_File_close_leave(void);
void earc_File_read_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_read_leave(void);
void earc_File_read_all_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_read_all_leave(void);
void earc_File_read_at_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_read_at_leave(void);
void earc_File_read_at_all_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_read_at_all_leave(void);
void earc_File_write_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_write_leave(void);
void earc_File_write_all_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_write_all_leave(void);
void earc_File_write_at_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_write_at_leave(void);
void earc_File_write_at_all_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void earc_File_write_at_all_leave(void);
void earc_Finalize_enter(void);
void earc_Finalize_leave(void);
void earc_Gather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void earc_Gather_leave(void);
void earc_Gatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
void earc_Gatherv_leave(void);
void earc_Get_enter(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void earc_Get_leave(void);
void earc_Ibsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Ibsend_leave(void);
void earc_Init_enter(int *argc, char ***argv);
void earc_Init_leave(void);
void earc_Init_thread_enter(int *argc, char ***argv, int required, int *provided);
void earc_Init_thread_leave(void);
void earc_Intercomm_create_enter(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
void earc_Intercomm_create_leave(void);
void earc_Intercomm_merge_enter(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
void earc_Intercomm_merge_leave(void);
void earc_Iprobe_enter(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
void earc_Iprobe_leave(void);
void earc_Irecv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Irecv_leave(void);
void earc_Irsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Irsend_leave(void);
void earc_Isend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Isend_leave(void);
void earc_Issend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Issend_leave(void);
void earc_Probe_enter(int source, int tag, MPI_Comm comm, MPI_Status *status);
void earc_Probe_leave(void);
void earc_Put_enter(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void earc_Put_leave(void);
void earc_Recv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
void earc_Recv_leave(void);
void earc_Recv_init_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Recv_init_leave(void);
void earc_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
void earc_Reduce_leave(void);
void earc_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void earc_Reduce_scatter_leave(void);
void earc_Request_free_enter(MPI_Request *request);
void earc_Request_free_leave(void);
void earc_Request_get_status_enter(MPI_Request request, int *flag, MPI_Status *status);
void earc_Request_get_status_leave(void);
void earc_Rsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void earc_Rsend_leave(void);
void earc_Rsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Rsend_init_leave(void);
void earc_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void earc_Scan_leave(void);
void earc_Scatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void earc_Scatter_leave(void);
void earc_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void earc_Scatterv_leave(void);
void earc_Send_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void earc_Send_leave(void);
void earc_Send_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Send_init_leave(void);
void earc_Sendrecv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void earc_Sendrecv_leave(void);
void earc_Sendrecv_replace_enter(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void earc_Sendrecv_replace_leave(void);
void earc_Ssend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void earc_Ssend_leave(void);
void earc_Ssend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void earc_Ssend_init_leave(void);
void earc_Start_enter(MPI_Request *request);
void earc_Start_leave(void);
void earc_Startall_enter(int count, MPI_Request array_of_requests[]);
void earc_Startall_leave(void);
void earc_Test_enter(MPI_Request *request, int *flag, MPI_Status *status);
void earc_Test_leave(void);
void earc_Testall_enter(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
void earc_Testall_leave(void);
void earc_Testany_enter(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
void earc_Testany_leave(void);
void earc_Testsome_enter(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
void earc_Testsome_leave(void);
void earc_Wait_enter(MPI_Request *request, MPI_Status *status);
void earc_Wait_leave(void);
void earc_Waitall_enter(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
void earc_Waitall_leave(void);
void earc_Waitany_enter(int count, MPI_Request *requests, int *index, MPI_Status *status);
void earc_Waitany_leave(void);
void earc_Waitsome_enter(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
void earc_Waitsome_leave(void);
void earc_Win_complete_enter(MPI_Win win);
void earc_Win_complete_leave(void);
void earc_Win_create_enter(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
void earc_Win_create_leave(void);
void earc_Win_fence_enter(int assert, MPI_Win win);
void earc_Win_fence_leave(void);
void earc_Win_free_enter(MPI_Win *win);
void earc_Win_free_leave(void);
void earc_Win_post_enter(MPI_Group group, int assert, MPI_Win win);
void earc_Win_post_leave(void);
void earc_Win_start_enter(MPI_Group group, int assert, MPI_Win win);
void earc_Win_start_leave(void);
void earc_Win_wait_enter(MPI_Win win);
void earc_Win_wait_leave(void);
#if MPI_VERSION >= 3
void earc_Iallgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void earc_Iallgather_leave(void);
void earc_Iallgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void earc_Iallgatherv_leave(void);
void earc_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void earc_Iallreduce_leave(void);
void earc_Ialltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void earc_Ialltoall_leave(void);
void earc_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void earc_Ialltoallv_leave(void);
void earc_Ibarrier_enter(MPI_Comm comm, MPI_Request *request);
void earc_Ibarrier_leave(void);
void earc_Ibcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
void earc_Ibcast_leave(void);
void earc_Igather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void earc_Igather_leave(void);
void earc_Igatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void earc_Igatherv_leave(void);
void earc_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
void earc_Ireduce_leave(void);
void earc_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void earc_Ireduce_scatter_leave(void);
void earc_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void earc_Iscan_leave(void);
void earc_Iscatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void earc_Iscatter_leave(void);
void earc_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void earc_Iscatterv_leave(void);
#endif

typedef struct earc_s
{
	void (*Allgather_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Allgather_leave) (void);
	void (*Allgatherv_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Allgatherv_leave) (void);
	void (*Allreduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Allreduce_leave) (void);
	void (*Alltoall_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Alltoall_leave) (void);
	void (*Alltoallv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Alltoallv_leave) (void);
	void (*Barrier_enter) (MPI_Comm comm);
	void (*Barrier_leave) (void);
	void (*Bcast_enter) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
	void (*Bcast_leave) (void);
	void (*Bsend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Bsend_leave) (void);
	void (*Bsend_init_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Bsend_init_leave) (void);
	void (*Cancel_enter) (MPI_Request *request);
	void (*Cancel_leave) (void);
	void (*Cart_create_enter) (MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
	void (*Cart_create_leave) (void);
	void (*Cart_sub_enter) (MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
	void (*Cart_sub_leave) (void);
	void (*Comm_create_enter) (MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
	void (*Comm_create_leave) (void);
	void (*Comm_dup_enter) (MPI_Comm comm, MPI_Comm *newcomm);
	void (*Comm_dup_leave) (void);
	void (*Comm_free_enter) (MPI_Comm *comm);
	void (*Comm_free_leave) (void);
	void (*Comm_rank_enter) (MPI_Comm comm, int *rank);
	void (*Comm_rank_leave) (void);
	void (*Comm_size_enter) (MPI_Comm comm, int *size);
	void (*Comm_size_leave) (void);
	void (*Comm_spawn_enter) (MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	void (*Comm_spawn_leave) (void);
	void (*Comm_spawn_multiple_enter) (int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	void (*Comm_spawn_multiple_leave) (void);
	void (*Comm_split_enter) (MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
	void (*Comm_split_leave) (void);
	void (*File_close_enter) (MPI_File *fh);
	void (*File_close_leave) (void);
	void (*File_read_enter) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_leave) (void);
	void (*File_read_all_enter) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_all_leave) (void);
	void (*File_read_at_enter) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_at_leave) (void);
	void (*File_read_at_all_enter) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_at_all_leave) (void);
	void (*File_write_enter) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_leave) (void);
	void (*File_write_all_enter) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_all_leave) (void);
	void (*File_write_at_enter) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_at_leave) (void);
	void (*File_write_at_all_enter) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_at_all_leave) (void);
	void (*Finalize_enter) (void);
	void (*Finalize_leave) (void);
	void (*Gather_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Gather_leave) (void);
	void (*Gatherv_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Gatherv_leave) (void);
	void (*Get_enter) (void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	void (*Get_leave) (void);
	void (*Ibsend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Ibsend_leave) (void);
	void (*Init_enter) (int *argc, char ***argv);
	void (*Init_leave) (void);
	void (*Init_thread_enter) (int *argc, char ***argv, int required, int *provided);
	void (*Init_thread_leave) (void);
	void (*Intercomm_create_enter) (MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
	void (*Intercomm_create_leave) (void);
	void (*Intercomm_merge_enter) (MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
	void (*Intercomm_merge_leave) (void);
	void (*Iprobe_enter) (int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
	void (*Iprobe_leave) (void);
	void (*Irecv_enter) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Irecv_leave) (void);
	void (*Irsend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Irsend_leave) (void);
	void (*Isend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Isend_leave) (void);
	void (*Issend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Issend_leave) (void);
	void (*Probe_enter) (int source, int tag, MPI_Comm comm, MPI_Status *status);
	void (*Probe_leave) (void);
	void (*Put_enter) (MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	void (*Put_leave) (void);
	void (*Recv_enter) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
	void (*Recv_leave) (void);
	void (*Recv_init_enter) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Recv_init_leave) (void);
	void (*Reduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
	void (*Reduce_leave) (void);
	void (*Reduce_scatter_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Reduce_scatter_leave) (void);
	void (*Request_free_enter) (MPI_Request *request);
	void (*Request_free_leave) (void);
	void (*Request_get_status_enter) (MPI_Request request, int *flag, MPI_Status *status);
	void (*Request_get_status_leave) (void);
	void (*Rsend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Rsend_leave) (void);
	void (*Rsend_init_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Rsend_init_leave) (void);
	void (*Scan_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Scan_leave) (void);
	void (*Scatter_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Scatter_leave) (void);
	void (*Scatterv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Scatterv_leave) (void);
	void (*Send_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Send_leave) (void);
	void (*Send_init_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Send_init_leave) (void);
	void (*Sendrecv_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	void (*Sendrecv_leave) (void);
	void (*Sendrecv_replace_enter) (void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	void (*Sendrecv_replace_leave) (void);
	void (*Ssend_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Ssend_leave) (void);
	void (*Ssend_init_enter) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Ssend_init_leave) (void);
	void (*Start_enter) (MPI_Request *request);
	void (*Start_leave) (void);
	void (*Startall_enter) (int count, MPI_Request array_of_requests[]);
	void (*Startall_leave) (void);
	void (*Test_enter) (MPI_Request *request, int *flag, MPI_Status *status);
	void (*Test_leave) (void);
	void (*Testall_enter) (int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
	void (*Testall_leave) (void);
	void (*Testany_enter) (int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
	void (*Testany_leave) (void);
	void (*Testsome_enter) (int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
	void (*Testsome_leave) (void);
	void (*Wait_enter) (MPI_Request *request, MPI_Status *status);
	void (*Wait_leave) (void);
	void (*Waitall_enter) (int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
	void (*Waitall_leave) (void);
	void (*Waitany_enter) (int count, MPI_Request *requests, int *index, MPI_Status *status);
	void (*Waitany_leave) (void);
	void (*Waitsome_enter) (int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
	void (*Waitsome_leave) (void);
	void (*Win_complete_enter) (MPI_Win win);
	void (*Win_complete_leave) (void);
	void (*Win_create_enter) (void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
	void (*Win_create_leave) (void);
	void (*Win_fence_enter) (int assert, MPI_Win win);
	void (*Win_fence_leave) (void);
	void (*Win_free_enter) (MPI_Win *win);
	void (*Win_free_leave) (void);
	void (*Win_post_enter) (MPI_Group group, int assert, MPI_Win win);
	void (*Win_post_leave) (void);
	void (*Win_start_enter) (MPI_Group group, int assert, MPI_Win win);
	void (*Win_start_leave) (void);
	void (*Win_wait_enter) (MPI_Win win);
	void (*Win_wait_leave) (void);
	#if MPI_VERSION >= 3
	void (*Iallgather_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Iallgather_leave) (void);
	void (*Iallgatherv_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Iallgatherv_leave) (void);
	void (*Iallreduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Iallreduce_leave) (void);
	void (*Ialltoall_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Ialltoall_leave) (void);
	void (*Ialltoallv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Ialltoallv_leave) (void);
	void (*Ibarrier_enter) (MPI_Comm comm, MPI_Request *request);
	void (*Ibarrier_leave) (void);
	void (*Ibcast_enter) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Ibcast_leave) (void);
	void (*Igather_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Igather_leave) (void);
	void (*Igatherv_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Igatherv_leave) (void);
	void (*Ireduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
	void (*Ireduce_leave) (void);
	void (*Ireduce_scatter_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Ireduce_scatter_leave) (void);
	void (*Iscan_enter) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Iscan_leave) (void);
	void (*Iscatter_enter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Iscatter_leave) (void);
	void (*Iscatterv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Iscatterv_leave) (void);
	#endif
} earc_t;

const char *earc_names[] __attribute__((weak)) =
{
	"earc_Allgather_enter",
	"earc_Allgather_leave",
	"earc_Allgatherv_enter",
	"earc_Allgatherv_leave",
	"earc_Allreduce_enter",
	"earc_Allreduce_leave",
	"earc_Alltoall_enter",
	"earc_Alltoall_leave",
	"earc_Alltoallv_enter",
	"earc_Alltoallv_leave",
	"earc_Barrier_enter",
	"earc_Barrier_leave",
	"earc_Bcast_enter",
	"earc_Bcast_leave",
	"earc_Bsend_enter",
	"earc_Bsend_leave",
	"earc_Bsend_init_enter",
	"earc_Bsend_init_leave",
	"earc_Cancel_enter",
	"earc_Cancel_leave",
	"earc_Cart_create_enter",
	"earc_Cart_create_leave",
	"earc_Cart_sub_enter",
	"earc_Cart_sub_leave",
	"earc_Comm_create_enter",
	"earc_Comm_create_leave",
	"earc_Comm_dup_enter",
	"earc_Comm_dup_leave",
	"earc_Comm_free_enter",
	"earc_Comm_free_leave",
	"earc_Comm_rank_enter",
	"earc_Comm_rank_leave",
	"earc_Comm_size_enter",
	"earc_Comm_size_leave",
	"earc_Comm_spawn_enter",
	"earc_Comm_spawn_leave",
	"earc_Comm_spawn_multiple_enter",
	"earc_Comm_spawn_multiple_leave",
	"earc_Comm_split_enter",
	"earc_Comm_split_leave",
	"earc_File_close_enter",
	"earc_File_close_leave",
	"earc_File_read_enter",
	"earc_File_read_leave",
	"earc_File_read_all_enter",
	"earc_File_read_all_leave",
	"earc_File_read_at_enter",
	"earc_File_read_at_leave",
	"earc_File_read_at_all_enter",
	"earc_File_read_at_all_leave",
	"earc_File_write_enter",
	"earc_File_write_leave",
	"earc_File_write_all_enter",
	"earc_File_write_all_leave",
	"earc_File_write_at_enter",
	"earc_File_write_at_leave",
	"earc_File_write_at_all_enter",
	"earc_File_write_at_all_leave",
	"earc_Finalize_enter",
	"earc_Finalize_leave",
	"earc_Gather_enter",
	"earc_Gather_leave",
	"earc_Gatherv_enter",
	"earc_Gatherv_leave",
	"earc_Get_enter",
	"earc_Get_leave",
	"earc_Ibsend_enter",
	"earc_Ibsend_leave",
	"earc_Init_enter",
	"earc_Init_leave",
	"earc_Init_thread_enter",
	"earc_Init_thread_leave",
	"earc_Intercomm_create_enter",
	"earc_Intercomm_create_leave",
	"earc_Intercomm_merge_enter",
	"earc_Intercomm_merge_leave",
	"earc_Iprobe_enter",
	"earc_Iprobe_leave",
	"earc_Irecv_enter",
	"earc_Irecv_leave",
	"earc_Irsend_enter",
	"earc_Irsend_leave",
	"earc_Isend_enter",
	"earc_Isend_leave",
	"earc_Issend_enter",
	"earc_Issend_leave",
	"earc_Probe_enter",
	"earc_Probe_leave",
	"earc_Put_enter",
	"earc_Put_leave",
	"earc_Recv_enter",
	"earc_Recv_leave",
	"earc_Recv_init_enter",
	"earc_Recv_init_leave",
	"earc_Reduce_enter",
	"earc_Reduce_leave",
	"earc_Reduce_scatter_enter",
	"earc_Reduce_scatter_leave",
	"earc_Request_free_enter",
	"earc_Request_free_leave",
	"earc_Request_get_status_enter",
	"earc_Request_get_status_leave",
	"earc_Rsend_enter",
	"earc_Rsend_leave",
	"earc_Rsend_init_enter",
	"earc_Rsend_init_leave",
	"earc_Scan_enter",
	"earc_Scan_leave",
	"earc_Scatter_enter",
	"earc_Scatter_leave",
	"earc_Scatterv_enter",
	"earc_Scatterv_leave",
	"earc_Send_enter",
	"earc_Send_leave",
	"earc_Send_init_enter",
	"earc_Send_init_leave",
	"earc_Sendrecv_enter",
	"earc_Sendrecv_leave",
	"earc_Sendrecv_replace_enter",
	"earc_Sendrecv_replace_leave",
	"earc_Ssend_enter",
	"earc_Ssend_leave",
	"earc_Ssend_init_enter",
	"earc_Ssend_init_leave",
	"earc_Start_enter",
	"earc_Start_leave",
	"earc_Startall_enter",
	"earc_Startall_leave",
	"earc_Test_enter",
	"earc_Test_leave",
	"earc_Testall_enter",
	"earc_Testall_leave",
	"earc_Testany_enter",
	"earc_Testany_leave",
	"earc_Testsome_enter",
	"earc_Testsome_leave",
	"earc_Wait_enter",
	"earc_Wait_leave",
	"earc_Waitall_enter",
	"earc_Waitall_leave",
	"earc_Waitany_enter",
	"earc_Waitany_leave",
	"earc_Waitsome_enter",
	"earc_Waitsome_leave",
	"earc_Win_complete_enter",
	"earc_Win_complete_leave",
	"earc_Win_create_enter",
	"earc_Win_create_leave",
	"earc_Win_fence_enter",
	"earc_Win_fence_leave",
	"earc_Win_free_enter",
	"earc_Win_free_leave",
	"earc_Win_post_enter",
	"earc_Win_post_leave",
	"earc_Win_start_enter",
	"earc_Win_start_leave",
	"earc_Win_wait_enter",
	"earc_Win_wait_leave"
	#if MPI_VERSION >= 3
	,
	"earc_Iallgather_enter",
	"earc_Iallgather_leave",
	"earc_Iallgatherv_enter",
	"earc_Iallgatherv_leave",
	"earc_Iallreduce_enter",
	"earc_Iallreduce_leave",
	"earc_Ialltoall_enter",
	"earc_Ialltoall_leave",
	"earc_Ialltoallv_enter",
	"earc_Ialltoallv_leave",
	"earc_Ibarrier_enter",
	"earc_Ibarrier_leave",
	"earc_Ibcast_enter",
	"earc_Ibcast_leave",
	"earc_Igather_enter",
	"earc_Igather_leave",
	"earc_Igatherv_enter",
	"earc_Igatherv_leave",
	"earc_Ireduce_enter",
	"earc_Ireduce_leave",
	"earc_Ireduce_scatter_enter",
	"earc_Ireduce_scatter_leave",
	"earc_Iscan_enter",
	"earc_Iscan_leave",
	"earc_Iscatter_enter",
	"earc_Iscatter_leave",
	"earc_Iscatterv_enter",
	"earc_Iscatterv_leave",
	#endif
};

#endif //LIBRARY_API_EARC_SYMS_H
