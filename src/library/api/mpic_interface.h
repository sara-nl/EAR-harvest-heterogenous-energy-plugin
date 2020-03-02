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

#ifndef LIBRARY_API_EARC_INTERFACE_H
#define LIBRARY_API_EARC_INTERFACE_H

#include <library/api/mpi.h>

#define mpic_ear_n 93*2

void mpic_ear_Allgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Allgather_leave(void);

void mpic_ear_Allgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Allgatherv_leave(void);

void mpic_ear_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Allreduce_leave(void);

void mpic_ear_Alltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Alltoall_leave(void);

void mpic_ear_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Alltoallv_leave(void);

void mpic_ear_Barrier_enter(MPI_Comm comm);
void mpic_ear_Barrier_leave(void);

void mpic_ear_Bcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
void mpic_ear_Bcast_leave(void);

void mpic_ear_Bsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Bsend_leave(void);

void mpic_ear_Bsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Bsend_init_leave(void);

void mpic_ear_Cancel_enter(MPI_Request *request);
void mpic_ear_Cancel_leave(void);

void mpic_ear_Cart_create_enter(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
void mpic_ear_Cart_create_leave(void);

void mpic_ear_Cart_sub_enter(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
void mpic_ear_Cart_sub_leave(void);

void mpic_ear_Comm_create_enter(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
void mpic_ear_Comm_create_leave(void);

void mpic_ear_Comm_dup_enter(MPI_Comm comm, MPI_Comm *newcomm);
void mpic_ear_Comm_dup_leave(void);

void mpic_ear_Comm_free_enter(MPI_Comm *comm);
void mpic_ear_Comm_free_leave(void);

void mpic_ear_Comm_rank_enter(MPI_Comm comm, int *rank);
void mpic_ear_Comm_rank_leave(void);

void mpic_ear_Comm_size_enter(MPI_Comm comm, int *size);
void mpic_ear_Comm_size_leave(void);

void mpic_ear_Comm_spawn_enter(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void mpic_ear_Comm_spawn_leave(void);

void mpic_ear_Comm_spawn_multiple_enter(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void mpic_ear_Comm_spawn_multiple_leave(void);

void mpic_ear_Comm_split_enter(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
void mpic_ear_Comm_split_leave(void);

void mpic_ear_File_close_enter(MPI_File *fh);
void mpic_ear_File_close_leave(void);

void mpic_ear_File_read_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_leave(void);

void mpic_ear_File_read_all_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_all_leave(void);

void mpic_ear_File_read_at_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_at_leave(void);

void mpic_ear_File_read_at_all_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_at_all_leave(void);

void mpic_ear_File_write_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_leave(void);

void mpic_ear_File_write_all_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_all_leave(void);

void mpic_ear_File_write_at_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_at_leave(void);

void mpic_ear_File_write_at_all_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_at_all_leave(void);

void mpic_ear_Finalize_enter(void);
void mpic_ear_Finalize_leave(void);

void mpic_ear_Gather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Gather_leave(void);

void mpic_ear_Gatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Gatherv_leave(void);

void mpic_ear_Get_enter(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void mpic_ear_Get_leave(void);

void mpic_ear_Ibsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibsend_leave(void);

void mpic_ear_Init_enter(int *argc, char ***argv);
void mpic_ear_Init_leave(void);

void mpic_ear_Init_thread_enter(int *argc, char ***argv, int required, int *provided);
void mpic_ear_Init_thread_leave(void);

void mpic_ear_Intercomm_create_enter(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
void mpic_ear_Intercomm_create_leave(void);

void mpic_ear_Intercomm_merge_enter(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
void mpic_ear_Intercomm_merge_leave(void);

void mpic_ear_Iprobe_enter(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
void mpic_ear_Iprobe_leave(void);

void mpic_ear_Irecv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Irecv_leave(void);

void mpic_ear_Irsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Irsend_leave(void);

void mpic_ear_Isend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Isend_leave(void);

void mpic_ear_Issend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Issend_leave(void);

void mpic_ear_Probe_enter(int source, int tag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Probe_leave(void);

void mpic_ear_Put_enter(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void mpic_ear_Put_leave(void);

void mpic_ear_Recv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Recv_leave(void);

void mpic_ear_Recv_init_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Recv_init_leave(void);

void mpic_ear_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
void mpic_ear_Reduce_leave(void);

void mpic_ear_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Reduce_scatter_leave(void);

void mpic_ear_Request_free_enter(MPI_Request *request);
void mpic_ear_Request_free_leave(void);

void mpic_ear_Request_get_status_enter(MPI_Request request, int *flag, MPI_Status *status);
void mpic_ear_Request_get_status_leave(void);

void mpic_ear_Rsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Rsend_leave(void);

void mpic_ear_Rsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Rsend_init_leave(void);

void mpic_ear_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Scan_leave(void);

void mpic_ear_Scatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Scatter_leave(void);

void mpic_ear_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Scatterv_leave(void);

void mpic_ear_Send_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Send_leave(void);

void mpic_ear_Send_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Send_init_leave(void);

void mpic_ear_Sendrecv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Sendrecv_leave(void);

void mpic_ear_Sendrecv_replace_enter(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Sendrecv_replace_leave(void);

void mpic_ear_Ssend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Ssend_leave(void);

void mpic_ear_Ssend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ssend_init_leave(void);

void mpic_ear_Start_enter(MPI_Request *request);
void mpic_ear_Start_leave(void);

void mpic_ear_Startall_enter(int count, MPI_Request array_of_requests[]);
void mpic_ear_Startall_leave(void);

void mpic_ear_Test_enter(MPI_Request *request, int *flag, MPI_Status *status);
void mpic_ear_Test_leave(void);

void mpic_ear_Testall_enter(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
void mpic_ear_Testall_leave(void);

void mpic_ear_Testany_enter(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
void mpic_ear_Testany_leave(void);

void mpic_ear_Testsome_enter(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
void mpic_ear_Testsome_leave(void);

void mpic_ear_Wait_enter(MPI_Request *request, MPI_Status *status);
void mpic_ear_Wait_leave(void);

void mpic_ear_Waitall_enter(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
void mpic_ear_Waitall_leave(void);

void mpic_ear_Waitany_enter(int count, MPI_Request *requests, int *index, MPI_Status *status);
void mpic_ear_Waitany_leave(void);

void mpic_ear_Waitsome_enter(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
void mpic_ear_Waitsome_leave(void);

void mpic_ear_Win_complete_enter(MPI_Win win);
void mpic_ear_Win_complete_leave(void);

void mpic_ear_Win_create_enter(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
void mpic_ear_Win_create_leave(void);

void mpic_ear_Win_fence_enter(int assert, MPI_Win win);
void mpic_ear_Win_fence_leave(void);

void mpic_ear_Win_free_enter(MPI_Win *win);
void mpic_ear_Win_free_leave(void);

void mpic_ear_Win_post_enter(MPI_Group group, int assert, MPI_Win win);
void mpic_ear_Win_post_leave(void);

void mpic_ear_Win_start_enter(MPI_Group group, int assert, MPI_Win win);
void mpic_ear_Win_start_leave(void);

void mpic_ear_Win_wait_enter(MPI_Win win);
void mpic_ear_Win_wait_leave(void); 

#if MPI_VERSION >= 3
void mpic_ear_Iallgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallgather_leave(void);

void mpic_ear_Iallgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallgatherv_leave(void);

void mpic_ear_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallreduce_leave(void);

void mpic_ear_Ialltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ialltoall_leave(void);

void mpic_ear_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ialltoallv_leave(void);

void mpic_ear_Ibarrier_enter(MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibarrier_leave(void);

void mpic_ear_Ibcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibcast_leave(void);

void mpic_ear_Igather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Igather_leave(void);

void mpic_ear_Igatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Igatherv_leave(void);

void mpic_ear_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ireduce_leave(void);

void mpic_ear_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ireduce_scatter_leave(void);

void mpic_ear_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscan_leave(void);

void mpic_ear_Iscatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscatter_leave(void);

void mpic_ear_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscatterv_leave(void);
#endif

typedef struct mpic_ear_s
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
} mpic_ear_t;

const char *mpic_ear_names[] =
{
	"mpic_ear_Allgather_enter",
	"mpic_ear_Allgather_leave",
	"mpic_ear_Allgatherv_enter",
	"mpic_ear_Allgatherv_leave",
	"mpic_ear_Allreduce_enter",
	"mpic_ear_Allreduce_leave",
	"mpic_ear_Alltoall_enter",
	"mpic_ear_Alltoall_leave",
	"mpic_ear_Alltoallv_enter",
	"mpic_ear_Alltoallv_leave",
	"mpic_ear_Barrier_enter",
	"mpic_ear_Barrier_leave",
	"mpic_ear_Bcast_enter",
	"mpic_ear_Bcast_leave",
	"mpic_ear_Bsend_enter",
	"mpic_ear_Bsend_leave",
	"mpic_ear_Bsend_init_enter",
	"mpic_ear_Bsend_init_leave",
	"mpic_ear_Cancel_enter",
	"mpic_ear_Cancel_leave",
	"mpic_ear_Cart_create_enter",
	"mpic_ear_Cart_create_leave",
	"mpic_ear_Cart_sub_enter",
	"mpic_ear_Cart_sub_leave",
	"mpic_ear_Comm_create_enter",
	"mpic_ear_Comm_create_leave",
	"mpic_ear_Comm_dup_enter",
	"mpic_ear_Comm_dup_leave",
	"mpic_ear_Comm_free_enter",
	"mpic_ear_Comm_free_leave",
	"mpic_ear_Comm_rank_enter",
	"mpic_ear_Comm_rank_leave",
	"mpic_ear_Comm_size_enter",
	"mpic_ear_Comm_size_leave",
	"mpic_ear_Comm_spawn_enter",
	"mpic_ear_Comm_spawn_leave",
	"mpic_ear_Comm_spawn_multiple_enter",
	"mpic_ear_Comm_spawn_multiple_leave",
	"mpic_ear_Comm_split_enter",
	"mpic_ear_Comm_split_leave",
	"mpic_ear_File_close_enter",
	"mpic_ear_File_close_leave",
	"mpic_ear_File_read_enter",
	"mpic_ear_File_read_leave",
	"mpic_ear_File_read_all_enter",
	"mpic_ear_File_read_all_leave",
	"mpic_ear_File_read_at_enter",
	"mpic_ear_File_read_at_leave",
	"mpic_ear_File_read_at_all_enter",
	"mpic_ear_File_read_at_all_leave",
	"mpic_ear_File_write_enter",
	"mpic_ear_File_write_leave",
	"mpic_ear_File_write_all_enter",
	"mpic_ear_File_write_all_leave",
	"mpic_ear_File_write_at_enter",
	"mpic_ear_File_write_at_leave",
	"mpic_ear_File_write_at_all_enter",
	"mpic_ear_File_write_at_all_leave",
	"mpic_ear_Finalize_enter",
	"mpic_ear_Finalize_leave",
	"mpic_ear_Gather_enter",
	"mpic_ear_Gather_leave",
	"mpic_ear_Gatherv_enter",
	"mpic_ear_Gatherv_leave",
	"mpic_ear_Get_enter",
	"mpic_ear_Get_leave",
	"mpic_ear_Ibsend_enter",
	"mpic_ear_Ibsend_leave",
	"mpic_ear_Init_enter",
	"mpic_ear_Init_leave",
	"mpic_ear_Init_thread_enter",
	"mpic_ear_Init_thread_leave",
	"mpic_ear_Intercomm_create_enter",
	"mpic_ear_Intercomm_create_leave",
	"mpic_ear_Intercomm_merge_enter",
	"mpic_ear_Intercomm_merge_leave",
	"mpic_ear_Iprobe_enter",
	"mpic_ear_Iprobe_leave",
	"mpic_ear_Irecv_enter",
	"mpic_ear_Irecv_leave",
	"mpic_ear_Irsend_enter",
	"mpic_ear_Irsend_leave",
	"mpic_ear_Isend_enter",
	"mpic_ear_Isend_leave",
	"mpic_ear_Issend_enter",
	"mpic_ear_Issend_leave",
	"mpic_ear_Probe_enter",
	"mpic_ear_Probe_leave",
	"mpic_ear_Put_enter",
	"mpic_ear_Put_leave",
	"mpic_ear_Recv_enter",
	"mpic_ear_Recv_leave",
	"mpic_ear_Recv_init_enter",
	"mpic_ear_Recv_init_leave",
	"mpic_ear_Reduce_enter",
	"mpic_ear_Reduce_leave",
	"mpic_ear_Reduce_scatter_enter",
	"mpic_ear_Reduce_scatter_leave",
	"mpic_ear_Request_free_enter",
	"mpic_ear_Request_free_leave",
	"mpic_ear_Request_get_status_enter",
	"mpic_ear_Request_get_status_leave",
	"mpic_ear_Rsend_enter",
	"mpic_ear_Rsend_leave",
	"mpic_ear_Rsend_init_enter",
	"mpic_ear_Rsend_init_leave",
	"mpic_ear_Scan_enter",
	"mpic_ear_Scan_leave",
	"mpic_ear_Scatter_enter",
	"mpic_ear_Scatter_leave",
	"mpic_ear_Scatterv_enter",
	"mpic_ear_Scatterv_leave",
	"mpic_ear_Send_enter",
	"mpic_ear_Send_leave",
	"mpic_ear_Send_init_enter",
	"mpic_ear_Send_init_leave",
	"mpic_ear_Sendrecv_enter",
	"mpic_ear_Sendrecv_leave",
	"mpic_ear_Sendrecv_replace_enter",
	"mpic_ear_Sendrecv_replace_leave",
	"mpic_ear_Ssend_enter",
	"mpic_ear_Ssend_leave",
	"mpic_ear_Ssend_init_enter",
	"mpic_ear_Ssend_init_leave",
	"mpic_ear_Start_enter",
	"mpic_ear_Start_leave",
	"mpic_ear_Startall_enter",
	"mpic_ear_Startall_leave",
	"mpic_ear_Test_enter",
	"mpic_ear_Test_leave",
	"mpic_ear_Testall_enter",
	"mpic_ear_Testall_leave",
	"mpic_ear_Testany_enter",
	"mpic_ear_Testany_leave",
	"mpic_ear_Testsome_enter",
	"mpic_ear_Testsome_leave",
	"mpic_ear_Wait_enter",
	"mpic_ear_Wait_leave",
	"mpic_ear_Waitall_enter",
	"mpic_ear_Waitall_leave",
	"mpic_ear_Waitany_enter",
	"mpic_ear_Waitany_leave",
	"mpic_ear_Waitsome_enter",
	"mpic_ear_Waitsome_leave",
	"mpic_ear_Win_complete_enter",
	"mpic_ear_Win_complete_leave",
	"mpic_ear_Win_create_enter",
	"mpic_ear_Win_create_leave",
	"mpic_ear_Win_fence_enter",
	"mpic_ear_Win_fence_leave",
	"mpic_ear_Win_free_enter",
	"mpic_ear_Win_free_leave",
	"mpic_ear_Win_post_enter",
	"mpic_ear_Win_post_leave",
	"mpic_ear_Win_start_enter",
	"mpic_ear_Win_start_leave",
	"mpic_ear_Win_wait_enter",
	"mpic_ear_Win_wait_leave",
#if MPI_VERSION >= 3
	"mpic_ear_Iallgather_enter",
	"mpic_ear_Iallgather_leave",
	"mpic_ear_Iallgatherv_enter",
	"mpic_ear_Iallgatherv_leave",
	"mpic_ear_Iallreduce_enter",
	"mpic_ear_Iallreduce_leave",
	"mpic_ear_Ialltoall_enter",
	"mpic_ear_Ialltoall_leave",
	"mpic_ear_Ialltoallv_enter",
	"mpic_ear_Ialltoallv_leave",
	"mpic_ear_Ibarrier_enter",
	"mpic_ear_Ibarrier_leave",
	"mpic_ear_Ibcast_enter",
	"mpic_ear_Ibcast_leave",
	"mpic_ear_Igather_enter",
	"mpic_ear_Igather_leave",
	"mpic_ear_Igatherv_enter",
	"mpic_ear_Igatherv_leave",
	"mpic_ear_Ireduce_enter",
	"mpic_ear_Ireduce_leave",
	"mpic_ear_Ireduce_scatter_enter",
	"mpic_ear_Ireduce_scatter_leave",
	"mpic_ear_Iscan_enter",
	"mpic_ear_Iscan_leave",
	"mpic_ear_Iscatter_enter",
	"mpic_ear_Iscatter_leave",
	"mpic_ear_Iscatterv_enter",
	"mpic_ear_Iscatterv_leave",
	#endif
};

#endif //LIBRARY_API_EARC_INTERFACE_H
