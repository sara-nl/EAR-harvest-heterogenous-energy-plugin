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

#ifndef LOADER_F_SYMS_H
#define LOADER_F_SYMS_H

#include <library/api/mpi.h>

#define mpif_mpi_n 93

typedef struct mpif_mpi_s
{
	void (*mpi_allgather) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_allgatherv) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_allreduce) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_alltoall) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_alltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_barrier) (MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_bcast) (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_bsend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_bsend_init) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_cancel) (MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_cart_create) (MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror);
	void (*mpi_cart_sub) (MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror);
	void (*mpi_comm_create) (MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*mpi_comm_dup) (MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*mpi_comm_free) (MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_comm_rank) (MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror);
	void (*mpi_comm_size) (MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror);
	void (*mpi_comm_spawn) (MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
	void (*mpi_comm_spawn_multiple) (MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror);
	void (*mpi_comm_split) (MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror);
	void (*mpi_file_close) (MPI_File *fh, MPI_Fint *ierror);
	void (*mpi_file_read) (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_read_all) (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_read_at) (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_read_at_all) (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_write) (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_write_all) (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_write_at) (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_file_write_at_all) (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror);
	void (*mpi_finalize) (MPI_Fint *ierror);
	void (*mpi_gather) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_gatherv) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_get) (MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_ibsend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_init) (MPI_Fint *ierror);
	void (*mpi_init_thread) (MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror);
	void (*mpi_intercomm_create) (MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror);
	void (*mpi_intercomm_merge) (MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror);
	void (*mpi_iprobe) (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_irecv) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_irsend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_isend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_issend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_probe) (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_put) (MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_recv) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_recv_init) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_reduce) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_reduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_request_free) (MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_request_get_status) (MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_rsend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_rsend_init) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_scan) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_scatter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_scatterv) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_send) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_send_init) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_sendrecv) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_sendrecv_replace) (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_ssend) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_ssend_init) (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_start) (MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_startall) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror);
	void (*mpi_test) (MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_testall) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*mpi_testany) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_testsome) (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*mpi_wait) (MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_waitall) (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*mpi_waitany) (MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror);
	void (*mpi_waitsome) (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror);
	void (*mpi_win_complete) (MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_create) (void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_fence) (MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_free) (MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_post) (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_start) (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror);
	void (*mpi_win_wait) (MPI_Fint *win, MPI_Fint *ierror);
	#if MPI_VERSION >= 3
	void (*mpi_iallgather) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_iallgatherv) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_iallreduce) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_ialltoall) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_ialltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror);
	void (*mpi_ibarrier) (MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_ibcast) (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_igather) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_igatherv) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_ireduce) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_ireduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_iscan) (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_iscatter) (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	void (*mpi_iscatterv) (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror);
	#endif
} mpif_mpi_t;

char *mpic_mpi_names[] =
{
	"mpi_allgather",
	"mpi_allgatherv",
	"mpi_allreduce",
	"mpi_alltoall",
	"mpi_alltoallv",
	"mpi_barrier",
	"mpi_bcast",
	"mpi_bsend",
	"mpi_bsend_init",
	"mpi_cancel",
	"mpi_cart_create",
	"mpi_cart_sub",
	"mpi_comm_create",
	"mpi_comm_dup",
	"mpi_comm_free",
	"mpi_comm_rank",
	"mpi_comm_size",
	"mpi_comm_spawn",
	"mpi_comm_spawn_multiple",
	"mpi_comm_split",
	"mpi_file_close",
	"mpi_file_read",
	"mpi_file_read_all",
	"mpi_file_read_at",
	"mpi_file_read_at_all",
	"mpi_file_write",
	"mpi_file_write_all",
	"mpi_file_write_at",
	"mpi_file_write_at_all",
	"mpi_finalize",
	"mpi_gather",
	"mpi_gatherv",
	"mpi_get",
	"mpi_ibsend",
	"mpi_init",
	"mpi_init_thread",
	"mpi_intercomm_create",
	"mpi_intercomm_merge",
	"mpi_iprobe",
	"mpi_irecv",
	"mpi_irsend",
	"mpi_isend",
	"mpi_issend",
	"mpi_probe",
	"mpi_put",
	"mpi_recv",
	"mpi_recv_init",
	"mpi_reduce",
	"mpi_reduce_scatter",
	"mpi_request_free",
	"mpi_request_get_status",
	"mpi_rsend",
	"mpi_rsend_init",
	"mpi_scan",
	"mpi_scatter",
	"mpi_scatterv",
	"mpi_send",
	"mpi_send_init",
	"mpi_sendrecv",
	"mpi_sendrecv_replace",
	"mpi_ssend",
	"mpi_ssend_init",
	"mpi_start",
	"mpi_startall",
	"mpi_test",
	"mpi_testall",
	"mpi_testany",
	"mpi_testsome",
	"mpi_wait",
	"mpi_waitall",
	"mpi_waitany",
	"mpi_waitsome",
	"mpi_win_complete",
	"mpi_win_create",
	"mpi_win_fence",
	"mpi_win_free",
	"mpi_win_post",
	"mpi_win_start",
	"mpi_win_wait"
	#if MPI_VERSION >= 3
	,
	"mpi_iallgather",
	"mpi_iallgatherv",
	"mpi_iallreduce",
	"mpi_ialltoall",
	"mpi_ialltoallv",
	"mpi_ibarrier",
	"mpi_ibcast",
	"mpi_igather",
	"mpi_igatherv",
	"mpi_ireduce",
	"mpi_ireduce_scatter",
	"mpi_iscan",
	"mpi_iscatter",
	"mpi_iscatterv"
	#endif
};

#endif //LOADER_F_SYMS_H