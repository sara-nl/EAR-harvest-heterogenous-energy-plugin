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

#ifndef LIBRARY_API_MPIF_H
#define LIBRARY_API_MPIF_H

#include <library/api/mpi.h>

#define API_MPIF_N 93

void api_mpif_Allgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Allgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Alltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Barrier_enter(MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Bcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Bsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Bsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Cancel_enter(MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Cart_create_enter(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror);
void api_mpif_Cart_sub_enter(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror);
void api_mpif_Comm_create_enter(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror);
void api_mpif_Comm_dup_enter(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror);
void api_mpif_Comm_free_enter(MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Comm_rank_enter(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror);
void api_mpif_Comm_size_enter(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror);
void api_mpif_Comm_spawn_enter(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void api_mpif_Comm_spawn_multiple_enter(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
void api_mpif_Comm_split_enter(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror);
void api_mpif_File_close_enter(MPI_File *fh, MPI_Fint *ierror);
void api_mpif_File_read_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_read_all_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_read_at_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_read_at_all_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_write_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_write_all_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_write_at_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_File_write_at_all_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
void api_mpif_Finalize_enter(MPI_Fint *ierror);
void api_mpif_Gather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Gatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Get_enter(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Ibsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Init_enter(MPI_Fint *ierror);
void api_mpif_Init_thread_enter(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror);
void api_mpif_Intercomm_create_enter(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror);
void api_mpif_Intercomm_merge_enter(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror);
void api_mpif_Iprobe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Irecv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Irsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Isend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Issend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Probe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Put_enter(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Recv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Recv_init_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Request_free_enter(MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Request_get_status_enter(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Rsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Rsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Scatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Send_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Send_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Sendrecv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Sendrecv_replace_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Ssend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Ssend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Start_enter(MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Startall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror);
void api_mpif_Test_enter(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Testall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void api_mpif_Testany_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Testsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void api_mpif_Wait_enter(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Waitall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void api_mpif_Waitany_enter(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror);
void api_mpif_Waitsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
void api_mpif_Win_complete_enter(MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_create_enter(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_fence_enter(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_free_enter(MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_post_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_start_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
void api_mpif_Win_wait_enter(MPI_Fint *win, MPI_Fint *ierror);
#if MPI_VERSION >= 3
void api_mpif_Iallgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Iallgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Ialltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror);
void api_mpif_Ibarrier_enter(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Ibcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Igather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Igatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Iscatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
void api_mpif_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
#endif

const char *api_mpif_names[] __attribute__((weak)) =
{
	"api_mpif_Allgather",
	"api_mpif_Allgatherv",
	"api_mpif_Allreduce",
	"api_mpif_Alltoall",
	"api_mpif_Alltoallv",
	"api_mpif_Barrier",
	"api_mpif_Bcast",
	"api_mpif_Bsend",
	"api_mpif_Bsend_init",
	"api_mpif_Cancel",
	"api_mpif_Cart_create",
	"api_mpif_Cart_sub",
	"api_mpif_Comm_create",
	"api_mpif_Comm_dup",
	"api_mpif_Comm_free",
	"api_mpif_Comm_rank",
	"api_mpif_Comm_size",
	"api_mpif_Comm_spawn",
	"api_mpif_Comm_spawn_multiple",
	"api_mpif_Comm_split",
	"api_mpif_File_close",
	"api_mpif_File_read",
	"api_mpif_File_read_all",
	"api_mpif_File_read_at",
	"api_mpif_File_read_at_all",
	"api_mpif_File_write",
	"api_mpif_File_write_all",
	"api_mpif_File_write_at",
	"api_mpif_File_write_at_all",
	"api_mpif_Finalize",
	"api_mpif_Gather",
	"api_mpif_Gatherv",
	"api_mpif_Get",
	"api_mpif_Ibsend",
	"api_mpif_Init",
	"api_mpif_Init_thread",
	"api_mpif_Intercomm_create",
	"api_mpif_Intercomm_merge",
	"api_mpif_Iprobe",
	"api_mpif_Irecv",
	"api_mpif_Irsend",
	"api_mpif_Isend",
	"api_mpif_Issend",
	"api_mpif_Probe",
	"api_mpif_Put",
	"api_mpif_Recv",
	"api_mpif_Recv_init",
	"api_mpif_Reduce",
	"api_mpif_Reduce_scatter",
	"api_mpif_Request_free",
	"api_mpif_Request_get_status",
	"api_mpif_Rsend",
	"api_mpif_Rsend_init",
	"api_mpif_Scan",
	"api_mpif_Scatter",
	"api_mpif_Scatterv",
	"api_mpif_Send",
	"api_mpif_Send_init",
	"api_mpif_Sendrecv",
	"api_mpif_Sendrecv_replace",
	"api_mpif_Ssend",
	"api_mpif_Ssend_init",
	"api_mpif_Start",
	"api_mpif_Startall",
	"api_mpif_Test",
	"api_mpif_Testall",
	"api_mpif_Testany",
	"api_mpif_Testsome",
	"api_mpif_Wait",
	"api_mpif_Waitall",
	"api_mpif_Waitany",
	"api_mpif_Waitsome",
	"api_mpif_Win_complete",
	"api_mpif_Win_create",
	"api_mpif_Win_fence",
	"api_mpif_Win_free",
	"api_mpif_Win_post",
	"api_mpif_Win_start",
	"api_mpif_Win_wait"
#if MPI_VERSION >= 3
	,
	"api_mpif_Iallgather",
	"api_mpif_Iallgatherv",
	"api_mpif_Iallreduce",
	"api_mpif_Ialltoall",
	"api_mpif_Ialltoallv",
	"api_mpif_Ibarrier",
	"api_mpif_Ibcast",
	"api_mpif_Igather",
	"api_mpif_Igatherv",
	"api_mpif_Ireduce",
	"api_mpif_Ireduce_scatter",
	"api_mpif_Iscan",
	"api_mpif_Iscatter",
	"api_mpif_Iscatterv",
#endif
};

#endif //LIBRARY_API_MPIF_H