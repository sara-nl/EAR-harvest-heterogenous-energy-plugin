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

#ifndef LIBRARY_API_EARF_SYMS_H
#define LIBRARY_API_EARF_SYMS_H

#include <library/api/mpi.h>

#define earf_n 93*2

void earf_Allgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Allgather_leave(void);
void earf_Allgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Allgatherv_leave(void);
void earf_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Allreduce_leave(void);
void earf_Alltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Alltoall_leave(void);
void earf_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Alltoallv_leave(void);
void earf_Barrier_enter(MPI_Fint *comm, MPI_Fint *ierror);
void earf_Barrier_leave(void);
void earf_Bcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Bcast_leave(void);
void earf_Bsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Bsend_leave(void);
void earf_Bsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Bsend_init_leave(void);
void earf_Cancel_enter(MPI_Fint *request, MPI_Fint *ierror);
void earf_Cancel_leave(void);
void earf_Cart_create_enter(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror);
void earf_Cart_create_leave(void);
void earf_Cart_sub_enter(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror);
void earf_Cart_sub_leave(void);
void earf_Comm_create_enter(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror);
void earf_Comm_create_leave(void);
void earf_Comm_dup_enter(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror);
void earf_Comm_dup_leave(void);
void earf_Comm_free_enter(MPI_Fint *comm, MPI_Fint *ierror);
void earf_Comm_free_leave(void);
void earf_Comm_rank_enter(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror);
void earf_Comm_rank_leave(void);
void earf_Comm_size_enter(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror);
void earf_Comm_size_leave(void);
void earf_Comm_spawn_enter(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void earf_Comm_spawn_leave(void);
void earf_Comm_spawn_multiple_enter(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void earf_Comm_spawn_multiple_leave(void);
void earf_Comm_split_enter(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror);
void earf_Comm_split_leave(void);
void earf_File_close_enter(MPI_File *fh, MPI_Fint *ierror);
void earf_File_close_leave(void);
void earf_File_read_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_read_leave(void);
void earf_File_read_all_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_read_all_leave(void);
void earf_File_read_at_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_read_at_leave(void);
void earf_File_read_at_all_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_read_at_all_leave(void);
void earf_File_write_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_write_leave(void);
void earf_File_write_all_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_write_all_leave(void);
void earf_File_write_at_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_write_at_leave(void);
void earf_File_write_at_all_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void earf_File_write_at_all_leave(void);
void earf_Finalize_enter(MPI_Fint *ierror);
void earf_Finalize_leave(void);
void earf_Gather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Gather_leave(void);
void earf_Gatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Gatherv_leave(void);
void earf_Get_enter(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void earf_Get_leave(void);
void earf_Ibsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ibsend_leave(void);
void earf_Init_enter(MPI_Fint *ierror);
void earf_Init_leave(void);
void earf_Init_thread_enter(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror);
void earf_Init_thread_leave(void);
void earf_Intercomm_create_enter(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror);
void earf_Intercomm_create_leave(void);
void earf_Intercomm_merge_enter(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror);
void earf_Intercomm_merge_leave(void);
void earf_Iprobe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void earf_Iprobe_leave(void);
void earf_Irecv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Irecv_leave(void);
void earf_Irsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Irsend_leave(void);
void earf_Isend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Isend_leave(void);
void earf_Issend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Issend_leave(void);
void earf_Probe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void earf_Probe_leave(void);
void earf_Put_enter(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void earf_Put_leave(void);
void earf_Recv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void earf_Recv_leave(void);
void earf_Recv_init_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Recv_init_leave(void);
void earf_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Reduce_leave(void);
void earf_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Reduce_scatter_leave(void);
void earf_Request_free_enter(MPI_Fint *request, MPI_Fint *ierror);
void earf_Request_free_leave(void);
void earf_Request_get_status_enter(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror);
void earf_Request_get_status_leave(void);
void earf_Rsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Rsend_leave(void);
void earf_Rsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Rsend_init_leave(void);
void earf_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Scan_leave(void);
void earf_Scatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Scatter_leave(void);
void earf_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Scatterv_leave(void);
void earf_Send_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Send_leave(void);
void earf_Send_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Send_init_leave(void);
void earf_Sendrecv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void earf_Sendrecv_leave(void);
void earf_Sendrecv_replace_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void earf_Sendrecv_replace_leave(void);
void earf_Ssend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Ssend_leave(void);
void earf_Ssend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ssend_init_leave(void);
void earf_Start_enter(MPI_Fint *request, MPI_Fint *ierror);
void earf_Start_leave(void);
void earf_Startall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror);
void earf_Startall_leave(void);
void earf_Test_enter(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void earf_Test_leave(void);
void earf_Testall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void earf_Testall_leave(void);
void earf_Testany_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void earf_Testany_leave(void);
void earf_Testsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void earf_Testsome_leave(void);
void earf_Wait_enter(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror);
void earf_Wait_leave(void);
void earf_Waitall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void earf_Waitall_leave(void);
void earf_Waitany_enter(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror);
void earf_Waitany_leave(void);
void earf_Waitsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void earf_Waitsome_leave(void);
void earf_Win_complete_enter(MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_complete_leave(void);
void earf_Win_create_enter(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_create_leave(void);
void earf_Win_fence_enter(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_fence_leave(void);
void earf_Win_free_enter(MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_free_leave(void);
void earf_Win_post_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_post_leave(void);
void earf_Win_start_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_start_leave(void);
void earf_Win_wait_enter(MPI_Fint *win, MPI_Fint *ierror);
void earf_Win_wait_leave(void);
#if MPI_VERSION >= 3
void earf_Iallgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iallgather_leave(void);
void earf_Iallgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iallgatherv_leave(void);
void earf_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iallreduce_leave(void);
void earf_Ialltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ialltoall_leave(void);
void earf_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror);
void earf_Ialltoallv_leave(void);
void earf_Ibarrier_enter(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ibarrier_leave(void);
void earf_Ibcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ibcast_leave(void);
void earf_Igather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Igather_leave(void);
void earf_Igatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Igatherv_leave(void);
void earf_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ireduce_leave(void);
void earf_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Ireduce_scatter_leave(void);
void earf_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iscan_leave(void);
void earf_Iscatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iscatter_leave(void);
void earf_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void earf_Iscatterv_leave(void);
#endif

typedef struct earf_s
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
} earf_t;

const char *earf_names[] __attribute__((weak)) =
{
	"earf_Allgather_enter",
	"earf_Allgather_leave",
	"earf_Allgatherv_enter",
	"earf_Allgatherv_leave",
	"earf_Allreduce_enter",
	"earf_Allreduce_leave",
	"earf_Alltoall_enter",
	"earf_Alltoall_leave",
	"earf_Alltoallv_enter",
	"earf_Alltoallv_leave",
	"earf_Barrier_enter",
	"earf_Barrier_leave",
	"earf_Bcast_enter",
	"earf_Bcast_leave",
	"earf_Bsend_enter",
	"earf_Bsend_leave",
	"earf_Bsend_init_enter",
	"earf_Bsend_init_leave",
	"earf_Cancel_enter",
	"earf_Cancel_leave",
	"earf_Cart_create_enter",
	"earf_Cart_create_leave",
	"earf_Cart_sub_enter",
	"earf_Cart_sub_leave",
	"earf_Comm_create_enter",
	"earf_Comm_create_leave",
	"earf_Comm_dup_enter",
	"earf_Comm_dup_leave",
	"earf_Comm_free_enter",
	"earf_Comm_free_leave",
	"earf_Comm_rank_enter",
	"earf_Comm_rank_leave",
	"earf_Comm_size_enter",
	"earf_Comm_size_leave",
	"earf_Comm_spawn_enter",
	"earf_Comm_spawn_leave",
	"earf_Comm_spawn_multiple_enter",
	"earf_Comm_spawn_multiple_leave",
	"earf_Comm_split_enter",
	"earf_Comm_split_leave",
	"earf_File_close_enter",
	"earf_File_close_leave",
	"earf_File_read_enter",
	"earf_File_read_leave",
	"earf_File_read_all_enter",
	"earf_File_read_all_leave",
	"earf_File_read_at_enter",
	"earf_File_read_at_leave",
	"earf_File_read_at_all_enter",
	"earf_File_read_at_all_leave",
	"earf_File_write_enter",
	"earf_File_write_leave",
	"earf_File_write_all_enter",
	"earf_File_write_all_leave",
	"earf_File_write_at_enter",
	"earf_File_write_at_leave",
	"earf_File_write_at_all_enter",
	"earf_File_write_at_all_leave",
	"earf_Finalize_enter",
	"earf_Finalize_leave",
	"earf_Gather_enter",
	"earf_Gather_leave",
	"earf_Gatherv_enter",
	"earf_Gatherv_leave",
	"earf_Get_enter",
	"earf_Get_leave",
	"earf_Ibsend_enter",
	"earf_Ibsend_leave",
	"earf_Init_enter",
	"earf_Init_leave",
	"earf_Init_thread_enter",
	"earf_Init_thread_leave",
	"earf_Intercomm_create_enter",
	"earf_Intercomm_create_leave",
	"earf_Intercomm_merge_enter",
	"earf_Intercomm_merge_leave",
	"earf_Iprobe_enter",
	"earf_Iprobe_leave",
	"earf_Irecv_enter",
	"earf_Irecv_leave",
	"earf_Irsend_enter",
	"earf_Irsend_leave",
	"earf_Isend_enter",
	"earf_Isend_leave",
	"earf_Issend_enter",
	"earf_Issend_leave",
	"earf_Probe_enter",
	"earf_Probe_leave",
	"earf_Put_enter",
	"earf_Put_leave",
	"earf_Recv_enter",
	"earf_Recv_leave",
	"earf_Recv_init_enter",
	"earf_Recv_init_leave",
	"earf_Reduce_enter",
	"earf_Reduce_leave",
	"earf_Reduce_scatter_enter",
	"earf_Reduce_scatter_leave",
	"earf_Request_free_enter",
	"earf_Request_free_leave",
	"earf_Request_get_status_enter",
	"earf_Request_get_status_leave",
	"earf_Rsend_enter",
	"earf_Rsend_leave",
	"earf_Rsend_init_enter",
	"earf_Rsend_init_leave",
	"earf_Scan_enter",
	"earf_Scan_leave",
	"earf_Scatter_enter",
	"earf_Scatter_leave",
	"earf_Scatterv_enter",
	"earf_Scatterv_leave",
	"earf_Send_enter",
	"earf_Send_leave",
	"earf_Send_init_enter",
	"earf_Send_init_leave",
	"earf_Sendrecv_enter",
	"earf_Sendrecv_leave",
	"earf_Sendrecv_replace_enter",
	"earf_Sendrecv_replace_leave",
	"earf_Ssend_enter",
	"earf_Ssend_leave",
	"earf_Ssend_init_enter",
	"earf_Ssend_init_leave",
	"earf_Start_enter",
	"earf_Start_leave",
	"earf_Startall_enter",
	"earf_Startall_leave",
	"earf_Test_enter",
	"earf_Test_leave",
	"earf_Testall_enter",
	"earf_Testall_leave",
	"earf_Testany_enter",
	"earf_Testany_leave",
	"earf_Testsome_enter",
	"earf_Testsome_leave",
	"earf_Wait_enter",
	"earf_Wait_leave",
	"earf_Waitall_enter",
	"earf_Waitall_leave",
	"earf_Waitany_enter",
	"earf_Waitany_leave",
	"earf_Waitsome_enter",
	"earf_Waitsome_leave",
	"earf_Win_complete_enter",
	"earf_Win_complete_leave",
	"earf_Win_create_enter",
	"earf_Win_create_leave",
	"earf_Win_fence_enter",
	"earf_Win_fence_leave",
	"earf_Win_free_enter",
	"earf_Win_free_leave",
	"earf_Win_post_enter",
	"earf_Win_post_leave",
	"earf_Win_start_enter",
	"earf_Win_start_leave",
	"earf_Win_wait_enter",
	"earf_Win_wait_leave"
	#if MPI_VERSION >= 3
	,
	"earf_Iallgather_enter",
	"earf_Iallgather_leave",
	"earf_Iallgatherv_enter",
	"earf_Iallgatherv_leave",
	"earf_Iallreduce_enter",
	"earf_Iallreduce_leave",
	"earf_Ialltoall_enter",
	"earf_Ialltoall_leave",
	"earf_Ialltoallv_enter",
	"earf_Ialltoallv_leave",
	"earf_Ibarrier_enter",
	"earf_Ibarrier_leave",
	"earf_Ibcast_enter",
	"earf_Ibcast_leave",
	"earf_Igather_enter",
	"earf_Igather_leave",
	"earf_Igatherv_enter",
	"earf_Igatherv_leave",
	"earf_Ireduce_enter",
	"earf_Ireduce_leave",
	"earf_Ireduce_scatter_enter",
	"earf_Ireduce_scatter_leave",
	"earf_Iscan_enter",
	"earf_Iscan_leave",
	"earf_Iscatter_enter",
	"earf_Iscatter_leave",
	"earf_Iscatterv_enter",
	"earf_Iscatterv_leave"
	#endif
};

#endif //LIBRARY_API_EARF_SYMS_H
