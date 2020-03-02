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

#ifndef LIBRARY_MPI_F_INTERFACE_H
#define LIBRARY_MPI_F_INTERFACE_H

#include <library/api/mpi.h>

#define mpif_ear_n 93*2

void mpif_ear_Allgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Allgather_leave(void);

void mpif_ear_Allgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Allgatherv_leave(void);

void mpif_ear_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Allreduce_leave(void);

void mpif_ear_Alltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Alltoall_leave(void);

void mpif_ear_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Alltoallv_leave(void);

void mpif_ear_Barrier_enter(MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Barrier_leave(void);

void mpif_ear_Bcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Bcast_leave(void);

void mpif_ear_Bsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Bsend_leave(void);

void mpif_ear_Bsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Bsend_init_leave(void);

void mpif_ear_Cancel_enter(MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Cancel_leave(void);

void mpif_ear_Cart_create_enter(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror);
void mpif_ear_Cart_create_leave(void);

void mpif_ear_Cart_sub_enter(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror);
void mpif_ear_Cart_sub_leave(void);

void mpif_ear_Comm_create_enter(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror);
void mpif_ear_Comm_create_leave(void);

void mpif_ear_Comm_dup_enter(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror);
void mpif_ear_Comm_dup_leave(void);

void mpif_ear_Comm_free_enter(MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Comm_free_leave(void);

void mpif_ear_Comm_rank_enter(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror);
void mpif_ear_Comm_rank_leave(void);

void mpif_ear_Comm_size_enter(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror);
void mpif_ear_Comm_size_leave(void);

void mpif_ear_Comm_spawn_enter(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void mpif_ear_Comm_spawn_leave(void);

void mpif_ear_Comm_spawn_multiple_enter(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void mpif_ear_Comm_spawn_multiple_leave(void);

void mpif_ear_Comm_split_enter(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror);
void mpif_ear_Comm_split_leave(void);

void mpif_ear_File_close_enter(MPI_File *fh, MPI_Fint *ierror);
void mpif_ear_File_close_leave(void);

void mpif_ear_File_read_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_read_leave(void);

void mpif_ear_File_read_all_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_read_all_leave(void);

void mpif_ear_File_read_at_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_read_at_leave(void);

void mpif_ear_File_read_at_all_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_read_at_all_leave(void);

void mpif_ear_File_write_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_write_leave(void);

void mpif_ear_File_write_all_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_write_all_leave(void);

void mpif_ear_File_write_at_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_write_at_leave(void);

void mpif_ear_File_write_at_all_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void mpif_ear_File_write_at_all_leave(void);

void mpif_ear_Finalize_enter(MPI_Fint *ierror);
void mpif_ear_Finalize_leave(void);

void mpif_ear_Gather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Gather_leave(void);

void mpif_ear_Gatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Gatherv_leave(void);

void mpif_ear_Get_enter(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Get_leave(void);

void mpif_ear_Ibsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ibsend_leave(void);

void mpif_ear_Init_enter(MPI_Fint *ierror);
void mpif_ear_Init_leave(void);

void mpif_ear_Init_thread_enter(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror);
void mpif_ear_Init_thread_leave(void);

void mpif_ear_Intercomm_create_enter(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror);
void mpif_ear_Intercomm_create_leave(void);

void mpif_ear_Intercomm_merge_enter(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror);
void mpif_ear_Intercomm_merge_leave(void);

void mpif_ear_Iprobe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Iprobe_leave(void);

void mpif_ear_Irecv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Irecv_leave(void);

void mpif_ear_Irsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Irsend_leave(void);

void mpif_ear_Isend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Isend_leave(void);

void mpif_ear_Issend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Issend_leave(void);

void mpif_ear_Probe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Probe_leave(void);

void mpif_ear_Put_enter(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Put_leave(void);

void mpif_ear_Recv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Recv_leave(void);

void mpif_ear_Recv_init_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Recv_init_leave(void);

void mpif_ear_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Reduce_leave(void);

void mpif_ear_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Reduce_scatter_leave(void);

void mpif_ear_Request_free_enter(MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Request_free_leave(void);

void mpif_ear_Request_get_status_enter(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Request_get_status_leave(void);

void mpif_ear_Rsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Rsend_leave(void);

void mpif_ear_Rsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Rsend_init_leave(void);

void mpif_ear_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Scan_leave(void);

void mpif_ear_Scatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Scatter_leave(void);

void mpif_ear_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Scatterv_leave(void);

void mpif_ear_Send_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Send_leave(void);

void mpif_ear_Send_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Send_init_leave(void);

void mpif_ear_Sendrecv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Sendrecv_leave(void);

void mpif_ear_Sendrecv_replace_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Sendrecv_replace_leave(void);

void mpif_ear_Ssend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Ssend_leave(void);

void mpif_ear_Ssend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ssend_init_leave(void);

void mpif_ear_Start_enter(MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Start_leave(void);

void mpif_ear_Startall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror);
void mpif_ear_Startall_leave(void);

void mpif_ear_Test_enter(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Test_leave(void);

void mpif_ear_Testall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void mpif_ear_Testall_leave(void);

void mpif_ear_Testany_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Testany_leave(void);

void mpif_ear_Testsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void mpif_ear_Testsome_leave(void);

void mpif_ear_Wait_enter(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Wait_leave(void);

void mpif_ear_Waitall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void mpif_ear_Waitall_leave(void);

void mpif_ear_Waitany_enter(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror);
void mpif_ear_Waitany_leave(void);

void mpif_ear_Waitsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void mpif_ear_Waitsome_leave(void);

void mpif_ear_Win_complete_enter(MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_complete_leave(void);

void mpif_ear_Win_create_enter(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_create_leave(void);

void mpif_ear_Win_fence_enter(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_fence_leave(void);

void mpif_ear_Win_free_enter(MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_free_leave(void);

void mpif_ear_Win_post_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_post_leave(void);

void mpif_ear_Win_start_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_start_leave(void);

void mpif_ear_Win_wait_enter(MPI_Fint *win, MPI_Fint *ierror);
void mpif_ear_Win_wait_leave(void);

#if MPI_VERSION >= 3
void mpif_ear_Iallgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iallgather_leave(void);

void mpif_ear_Iallgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iallgatherv_leave(void);

void mpif_ear_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iallreduce_leave(void);

void mpif_ear_Ialltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ialltoall_leave(void);

void mpif_ear_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror);
void mpif_ear_Ialltoallv_leave(void);

void mpif_ear_Ibarrier_enter(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ibarrier_leave(void);

void mpif_ear_Ibcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ibcast_leave(void);

void mpif_ear_Igather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Igather_leave(void);

void mpif_ear_Igatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Igatherv_leave(void);

void mpif_ear_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ireduce_leave(void);

void mpif_ear_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Ireduce_scatter_leave(void);

void mpif_ear_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iscan_leave(void);

void mpif_ear_Iscatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iscatter_leave(void);

void mpif_ear_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void mpif_ear_Iscatterv_leave(void);
#endif

typedef struct mpif_ear_s
{
	void (*Allgather_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Allgather_leave) (void);
	void (*Allgatherv_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Allgatherv_leave) (void);
	void (*Allreduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Allreduce_leave) (void);
	void (*Alltoall_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Alltoall_leave) (void);
	void (*Alltoallv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Alltoallv_leave) (void);
	void (*Barrier_enter) (MPI_Fint *comm, MPI_Fint *ierror);
	void (*Barrier_leave) (void);
	void (*Bcast_enter) (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Bcast_leave) (void);
	void (*Bsend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Bsend_leave) (void);
	void (*Bsend_init_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Bsend_init_leave) (void);
	void (*Cancel_enter) (MPI_Fint *request, MPI_Fint *ierror);
	void (*Cancel_leave) (void);
	void (*Cart_create_enter) (MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror);
	void (*Cart_create_leave) (void);
	void (*Cart_sub_enter) (MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror);
	void (*Cart_sub_leave) (void);
	void (*Comm_create_enter) (MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*Comm_create_leave) (void);
	void (*Comm_dup_enter) (MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*Comm_dup_leave) (void);
	void (*Comm_free_enter) (MPI_Fint *comm, MPI_Fint *ierror);
	void (*Comm_free_leave) (void);
	void (*Comm_rank_enter) (MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror);
	void (*Comm_rank_leave) (void);
	void (*Comm_size_enter) (MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror);
	void (*Comm_size_leave) (void);
	void (*Comm_spawn_enter) (MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
	void (*Comm_spawn_leave) (void);
	void (*Comm_spawn_multiple_enter) (MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
	void (*Comm_spawn_multiple_leave) (void);
	void (*Comm_split_enter) (MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*Comm_split_leave) (void);
	void (*File_close_enter) (MPI_File *fh, MPI_Fint *ierror);
	void (*File_close_leave) (void);
	void (*File_read_enter) (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_read_leave) (void);
	void (*File_read_all_enter) (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_read_all_leave) (void);
	void (*File_read_at_enter) (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_read_at_leave) (void);
	void (*File_read_at_all_enter) (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_read_at_all_leave) (void);
	void (*File_write_enter) (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_write_leave) (void);
	void (*File_write_all_enter) (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_write_all_leave) (void);
	void (*File_write_at_enter) (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_write_at_leave) (void);
	void (*File_write_at_all_enter) (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*File_write_at_all_leave) (void);
	void (*Finalize_enter) (MPI_Fint *ierror);
	void (*Finalize_leave) (void);
	void (*Gather_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Gather_leave) (void);
	void (*Gatherv_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Gatherv_leave) (void);
	void (*Get_enter) (MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
	void (*Get_leave) (void);
	void (*Ibsend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ibsend_leave) (void);
	void (*Init_enter) (MPI_Fint *ierror);
	void (*Init_leave) (void);
	void (*Init_thread_enter) (MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror);
	void (*Init_thread_leave) (void);
	void (*Intercomm_create_enter) (MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror);
	void (*Intercomm_create_leave) (void);
	void (*Intercomm_merge_enter) (MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror);
	void (*Intercomm_merge_leave) (void);
	void (*Iprobe_enter) (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*Iprobe_leave) (void);
	void (*Irecv_enter) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Irecv_leave) (void);
	void (*Irsend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Irsend_leave) (void);
	void (*Isend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Isend_leave) (void);
	void (*Issend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Issend_leave) (void);
	void (*Probe_enter) (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*Probe_leave) (void);
	void (*Put_enter) (MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
	void (*Put_leave) (void);
	void (*Recv_enter) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*Recv_leave) (void);
	void (*Recv_init_enter) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Recv_init_leave) (void);
	void (*Reduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Reduce_leave) (void);
	void (*Reduce_scatter_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Reduce_scatter_leave) (void);
	void (*Request_free_enter) (MPI_Fint *request, MPI_Fint *ierror);
	void (*Request_free_leave) (void);
	void (*Request_get_status_enter) (MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*Request_get_status_leave) (void);
	void (*Rsend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Rsend_leave) (void);
	void (*Rsend_init_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Rsend_init_leave) (void);
	void (*Scan_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Scan_leave) (void);
	void (*Scatter_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Scatter_leave) (void);
	void (*Scatterv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Scatterv_leave) (void);
	void (*Send_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Send_leave) (void);
	void (*Send_init_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Send_init_leave) (void);
	void (*Sendrecv_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*Sendrecv_leave) (void);
	void (*Sendrecv_replace_enter) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*Sendrecv_replace_leave) (void);
	void (*Ssend_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Ssend_leave) (void);
	void (*Ssend_init_enter) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ssend_init_leave) (void);
	void (*Start_enter) (MPI_Fint *request, MPI_Fint *ierror);
	void (*Start_leave) (void);
	void (*Startall_enter) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror);
	void (*Startall_leave) (void);
	void (*Test_enter) (MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*Test_leave) (void);
	void (*Testall_enter) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*Testall_leave) (void);
	void (*Testany_enter) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*Testany_leave) (void);
	void (*Testsome_enter) (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*Testsome_leave) (void);
	void (*Wait_enter) (MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror);
	void (*Wait_leave) (void);
	void (*Waitall_enter) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*Waitall_leave) (void);
	void (*Waitany_enter) (MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror);
	void (*Waitany_leave) (void);
	void (*Waitsome_enter) (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*Waitsome_leave) (void);
	void (*Win_complete_enter) (MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_complete_leave) (void);
	void (*Win_create_enter) (void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_create_leave) (void);
	void (*Win_fence_enter) (MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_fence_leave) (void);
	void (*Win_free_enter) (MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_free_leave) (void);
	void (*Win_post_enter) (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_post_leave) (void);
	void (*Win_start_enter) (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_start_leave) (void);
	void (*Win_wait_enter) (MPI_Fint *win, MPI_Fint *ierror);
	void (*Win_wait_leave) (void);
	#if MPI_VERSION >= 3
	void (*Iallgather_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iallgather_leave) (void);
	void (*Iallgatherv_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iallgatherv_leave) (void);
	void (*Iallreduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iallreduce_leave) (void);
	void (*Ialltoall_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ialltoall_leave) (void);
	void (*Ialltoallv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror);
	void (*Ialltoallv_leave) (void);
	void (*Ibarrier_enter) (MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ibarrier_leave) (void);
	void (*Ibcast_enter) (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ibcast_leave) (void);
	void (*Igather_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Igather_leave) (void);
	void (*Igatherv_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Igatherv_leave) (void);
	void (*Ireduce_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ireduce_leave) (void);
	void (*Ireduce_scatter_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Ireduce_scatter_leave) (void);
	void (*Iscan_enter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iscan_leave) (void);
	void (*Iscatter_enter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iscatter_leave) (void);
	void (*Iscatterv_enter) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*Iscatterv_leave) (void);
	#endif
} mpif_ear_t;

const char *mpif_ear_names[] =
{
	"mpif_ear_Allgather_enter",
	"mpif_ear_Allgather_leave",
	"mpif_ear_Allgatherv_enter",
	"mpif_ear_Allgatherv_leave",
	"mpif_ear_Allreduce_enter",
	"mpif_ear_Allreduce_leave",
	"mpif_ear_Alltoall_enter",
	"mpif_ear_Alltoall_leave",
	"mpif_ear_Alltoallv_enter",
	"mpif_ear_Alltoallv_leave",
	"mpif_ear_Barrier_enter",
	"mpif_ear_Barrier_leave",
	"mpif_ear_Bcast_enter",
	"mpif_ear_Bcast_leave",
	"mpif_ear_Bsend_enter",
	"mpif_ear_Bsend_leave",
	"mpif_ear_Bsend_init_enter",
	"mpif_ear_Bsend_init_leave",
	"mpif_ear_Cancel_enter",
	"mpif_ear_Cancel_leave",
	"mpif_ear_Cart_create_enter",
	"mpif_ear_Cart_create_leave",
	"mpif_ear_Cart_sub_enter",
	"mpif_ear_Cart_sub_leave",
	"mpif_ear_Comm_create_enter",
	"mpif_ear_Comm_create_leave",
	"mpif_ear_Comm_dup_enter",
	"mpif_ear_Comm_dup_leave",
	"mpif_ear_Comm_free_enter",
	"mpif_ear_Comm_free_leave",
	"mpif_ear_Comm_rank_enter",
	"mpif_ear_Comm_rank_leave",
	"mpif_ear_Comm_size_enter",
	"mpif_ear_Comm_size_leave",
	"mpif_ear_Comm_spawn_enter",
	"mpif_ear_Comm_spawn_leave",
	"mpif_ear_Comm_spawn_multiple_enter",
	"mpif_ear_Comm_spawn_multiple_leave",
	"mpif_ear_Comm_split_enter",
	"mpif_ear_Comm_split_leave",
	"mpif_ear_File_close_enter",
	"mpif_ear_File_close_leave",
	"mpif_ear_File_read_enter",
	"mpif_ear_File_read_leave",
	"mpif_ear_File_read_all_enter",
	"mpif_ear_File_read_all_leave",
	"mpif_ear_File_read_at_enter",
	"mpif_ear_File_read_at_leave",
	"mpif_ear_File_read_at_all_enter",
	"mpif_ear_File_read_at_all_leave",
	"mpif_ear_File_write_enter",
	"mpif_ear_File_write_leave",
	"mpif_ear_File_write_all_enter",
	"mpif_ear_File_write_all_leave",
	"mpif_ear_File_write_at_enter",
	"mpif_ear_File_write_at_leave",
	"mpif_ear_File_write_at_all_enter",
	"mpif_ear_File_write_at_all_leave",
	"mpif_ear_Finalize_enter",
	"mpif_ear_Finalize_leave",
	"mpif_ear_Gather_enter",
	"mpif_ear_Gather_leave",
	"mpif_ear_Gatherv_enter",
	"mpif_ear_Gatherv_leave",
	"mpif_ear_Get_enter",
	"mpif_ear_Get_leave",
	"mpif_ear_Ibsend_enter",
	"mpif_ear_Ibsend_leave",
	"mpif_ear_Init_enter",
	"mpif_ear_Init_leave",
	"mpif_ear_Init_thread_enter",
	"mpif_ear_Init_thread_leave",
	"mpif_ear_Intercomm_create_enter",
	"mpif_ear_Intercomm_create_leave",
	"mpif_ear_Intercomm_merge_enter",
	"mpif_ear_Intercomm_merge_leave",
	"mpif_ear_Iprobe_enter",
	"mpif_ear_Iprobe_leave",
	"mpif_ear_Irecv_enter",
	"mpif_ear_Irecv_leave",
	"mpif_ear_Irsend_enter",
	"mpif_ear_Irsend_leave",
	"mpif_ear_Isend_enter",
	"mpif_ear_Isend_leave",
	"mpif_ear_Issend_enter",
	"mpif_ear_Issend_leave",
	"mpif_ear_Probe_enter",
	"mpif_ear_Probe_leave",
	"mpif_ear_Put_enter",
	"mpif_ear_Put_leave",
	"mpif_ear_Recv_enter",
	"mpif_ear_Recv_leave",
	"mpif_ear_Recv_init_enter",
	"mpif_ear_Recv_init_leave",
	"mpif_ear_Reduce_enter",
	"mpif_ear_Reduce_leave",
	"mpif_ear_Reduce_scatter_enter",
	"mpif_ear_Reduce_scatter_leave",
	"mpif_ear_Request_free_enter",
	"mpif_ear_Request_free_leave",
	"mpif_ear_Request_get_status_enter",
	"mpif_ear_Request_get_status_leave",
	"mpif_ear_Rsend_enter",
	"mpif_ear_Rsend_leave",
	"mpif_ear_Rsend_init_enter",
	"mpif_ear_Rsend_init_leave",
	"mpif_ear_Scan_enter",
	"mpif_ear_Scan_leave",
	"mpif_ear_Scatter_enter",
	"mpif_ear_Scatter_leave",
	"mpif_ear_Scatterv_enter",
	"mpif_ear_Scatterv_leave",
	"mpif_ear_Send_enter",
	"mpif_ear_Send_leave",
	"mpif_ear_Send_init_enter",
	"mpif_ear_Send_init_leave",
	"mpif_ear_Sendrecv_enter",
	"mpif_ear_Sendrecv_leave",
	"mpif_ear_Sendrecv_replace_enter",
	"mpif_ear_Sendrecv_replace_leave",
	"mpif_ear_Ssend_enter",
	"mpif_ear_Ssend_leave",
	"mpif_ear_Ssend_init_enter",
	"mpif_ear_Ssend_init_leave",
	"mpif_ear_Start_enter",
	"mpif_ear_Start_leave",
	"mpif_ear_Startall_enter",
	"mpif_ear_Startall_leave",
	"mpif_ear_Test_enter",
	"mpif_ear_Test_leave",
	"mpif_ear_Testall_enter",
	"mpif_ear_Testall_leave",
	"mpif_ear_Testany_enter",
	"mpif_ear_Testany_leave",
	"mpif_ear_Testsome_enter",
	"mpif_ear_Testsome_leave",
	"mpif_ear_Wait_enter",
	"mpif_ear_Wait_leave",
	"mpif_ear_Waitall_enter",
	"mpif_ear_Waitall_leave",
	"mpif_ear_Waitany_enter",
	"mpif_ear_Waitany_leave",
	"mpif_ear_Waitsome_enter",
	"mpif_ear_Waitsome_leave",
	"mpif_ear_Win_complete_enter",
	"mpif_ear_Win_complete_leave",
	"mpif_ear_Win_create_enter",
	"mpif_ear_Win_create_leave",
	"mpif_ear_Win_fence_enter",
	"mpif_ear_Win_fence_leave",
	"mpif_ear_Win_free_enter",
	"mpif_ear_Win_free_leave",
	"mpif_ear_Win_post_enter",
	"mpif_ear_Win_post_leave",
	"mpif_ear_Win_start_enter",
	"mpif_ear_Win_start_leave",
	"mpif_ear_Win_wait_enter",
	"mpif_ear_Win_wait_leave",
#if MPI_VERSION >= 3
	"mpif_ear_Iallgather_enter",
	"mpif_ear_Iallgather_leave",
	"mpif_ear_Iallgatherv_enter",
	"mpif_ear_Iallgatherv_leave",
	"mpif_ear_Iallreduce_enter",
	"mpif_ear_Iallreduce_leave",
	"mpif_ear_Ialltoall_enter",
	"mpif_ear_Ialltoall_leave",
	"mpif_ear_Ialltoallv_enter",
	"mpif_ear_Ialltoallv_leave",
	"mpif_ear_Ibarrier_enter",
	"mpif_ear_Ibarrier_leave",
	"mpif_ear_Ibcast_enter",
	"mpif_ear_Ibcast_leave",
	"mpif_ear_Igather_enter",
	"mpif_ear_Igather_leave",
	"mpif_ear_Igatherv_enter",
	"mpif_ear_Igatherv_leave",
	"mpif_ear_Ireduce_enter",
	"mpif_ear_Ireduce_leave",
	"mpif_ear_Ireduce_scatter_enter",
	"mpif_ear_Ireduce_scatter_leave",
	"mpif_ear_Iscan_enter",
	"mpif_ear_Iscan_leave",
	"mpif_ear_Iscatter_enter",
	"mpif_ear_Iscatter_leave",
	"mpif_ear_Iscatterv_enter",
	"mpif_ear_Iscatterv_leave",
#endif
};

#endif //LIBRARY_MPI_F_INTERFACE_H
