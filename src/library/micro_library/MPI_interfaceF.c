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
#include "process_MPI.h"
#include "MPI_calls_coded.h"
#include "MPI_interfaceF.h"

#ifdef EAR_INTERFACE_F_DEBUG
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

void EAR_MPI_Allgather_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Allgather, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Allgather_F_leave(void) {
    after_mpi(Allgather);
}

void EAR_MPI_Allgatherv_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Allgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Allgatherv_F_leave(void) {
    after_mpi(Allgatherv);
}

void EAR_MPI_Allreduce_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Allreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Allreduce_F_leave(void) {
    after_mpi(Allreduce);
}

void EAR_MPI_Alltoall_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Alltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Alltoall_F_leave(void) {
    after_mpi(Alltoall);
}

void EAR_MPI_Alltoallv_F_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Alltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Alltoallv_F_leave(void) {
    after_mpi(Alltoallv);
}

void EAR_MPI_Barrier_F_enter(MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Barrier, (p2i)comm, (p2i)ierror);
}

void EAR_MPI_Barrier_F_leave(void) {
    after_mpi(Barrier);
}

void EAR_MPI_Bcast_F_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Bcast, (p2i)buffer, 0);
}

void EAR_MPI_Bcast_F_leave(void) {
    after_mpi(Bcast);
}

void EAR_MPI_Bsend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Bsend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Bsend_F_leave(void) {
    after_mpi(Bsend);
}

void EAR_MPI_Bsend_init_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Bsend_init, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Bsend_init_F_leave(void) {
    after_mpi(Bsend_init);
}

void EAR_MPI_Cancel_F_enter(MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Cancel, (p2i)request, 0);
}

void EAR_MPI_Cancel_F_leave(void) {
    after_mpi(Cancel);
}

void EAR_MPI_Cart_create_F_enter(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) {
    before_mpi(Cart_create, (p2i)ndims, 0);
}

void EAR_MPI_Cart_create_F_leave(void) {
    after_mpi(Cart_create);
}

void EAR_MPI_Cart_sub_F_enter(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) {
    before_mpi(Cart_sub, (p2i)remain_dims, 0);
}

void EAR_MPI_Cart_sub_F_leave(void) {
    after_mpi(Cart_sub);
}

void EAR_MPI_Comm_create_F_enter(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) {
    before_mpi(Comm_create, (p2i)group, 0);
}

void EAR_MPI_Comm_create_F_leave(void) {
    after_mpi(Comm_create);
}

void EAR_MPI_Comm_dup_F_enter(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) {
    before_mpi(Comm_dup, (p2i)newcomm, 0);
}

void EAR_MPI_Comm_dup_F_leave(void) {
    after_mpi(Comm_dup);
}

void EAR_MPI_Comm_free_F_enter(MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Comm_free, (p2i)comm, 0);
}

void EAR_MPI_Comm_free_F_leave(void) {
    after_mpi(Comm_free);
}

void EAR_MPI_Comm_rank_F_enter(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) {
    before_mpi(Comm_rank, (p2i)rank, 0);
}

void EAR_MPI_Comm_rank_F_leave(void) {
    after_mpi(Comm_rank);
}

void EAR_MPI_Comm_size_F_enter(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) {
    before_mpi(Comm_size, (p2i)size, 0);
}

void EAR_MPI_Comm_size_F_leave(void) {
    after_mpi(Comm_size);
}

void EAR_MPI_Comm_spawn_F_enter(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
    before_mpi(Comm_spawn, (p2i)command, 0);
}

void EAR_MPI_Comm_spawn_F_leave(void) {
    after_mpi(Comm_spawn);
}

void EAR_MPI_Comm_spawn_multiple_F_enter(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
    before_mpi(Comm_spawn_multiple, (p2i)array_of_commands, 0);
}

void EAR_MPI_Comm_spawn_multiple_F_leave(void) {
    after_mpi(Comm_spawn_multiple);
}

void EAR_MPI_Comm_split_F_enter(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) {
    before_mpi(Comm_split, (p2i)comm, 0);
}

void EAR_MPI_Comm_split_F_leave(void) {
    after_mpi(Comm_split);
}

void EAR_MPI_File_close_F_enter(MPI_File *fh, MPI_Fint *ierror) {
    before_mpi(File_close, (p2i)fh, 0);
}

void EAR_MPI_File_close_F_leave(void) {
    after_mpi(File_close);
}

void EAR_MPI_File_read_F_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_read, (p2i)buf, 0);
}

void EAR_MPI_File_read_F_leave(void) {
    after_mpi(File_read);
}

void EAR_MPI_File_read_all_F_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_read_all, (p2i)buf, 0);
}

void EAR_MPI_File_read_all_F_leave(void) {
    after_mpi(File_read_all);
}

void EAR_MPI_File_read_at_F_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_read_at, (p2i)buf, 0);
}

void EAR_MPI_File_read_at_F_leave(void) {
    after_mpi(File_read_at);
}

void EAR_MPI_File_read_at_all_F_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_read_at_all, (p2i)buf, 0);
}

void EAR_MPI_File_read_at_all_F_leave(void) {
    after_mpi(File_read_at_all);
}

void EAR_MPI_File_write_F_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_write, (p2i)buf, 0);
}

void EAR_MPI_File_write_F_leave(void) {
    after_mpi(File_write);
}

void EAR_MPI_File_write_all_F_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_write_all, (p2i)buf, 0);
}

void EAR_MPI_File_write_all_F_leave(void) {
    after_mpi(File_write_all);
}

void EAR_MPI_File_write_at_F_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_write_at, (p2i)buf, 0);
}

void EAR_MPI_File_write_at_F_leave(void) {
    after_mpi(File_write_at);
}

void EAR_MPI_File_write_at_all_F_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    before_mpi(File_write_at_all, (p2i)buf, 0);
}

void EAR_MPI_File_write_at_all_F_leave(void) {
    after_mpi(File_write_at_all);
}

void EAR_MPI_Finalize_F_enter(MPI_Fint *ierror) {
    before_finalize();
}

void EAR_MPI_Finalize_F_leave(void) {
    after_finalize();
}

void EAR_MPI_Gather_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Gather, (p2i)sendbuf, 0);
}

void EAR_MPI_Gather_F_leave(void) {
    after_mpi(Gather);
}

void EAR_MPI_Gatherv_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Gatherv, (p2i)sendbuf, 0);
}

void EAR_MPI_Gatherv_F_leave(void) {
    after_mpi(Gatherv);
}

void EAR_MPI_Get_F_enter(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Get, (p2i)origin_addr, 0);
}

void EAR_MPI_Get_F_leave(void) {
    after_mpi(Get);
}

void EAR_MPI_Ibsend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ibsend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Ibsend_F_leave(void) {
    after_mpi(Ibsend);
}

void EAR_MPI_Init_F_enter(MPI_Fint *ierror) {
    before_init();
}

void EAR_MPI_Init_F_leave(void) {
    after_init();
}

void EAR_MPI_Init_thread_F_enter(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) {
    before_init();
}

void EAR_MPI_Init_thread_F_leave(void) {
    after_init();
}

void EAR_MPI_Intercomm_create_F_enter(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) {
    before_mpi(Intercomm_create,0 , 0);
}

void EAR_MPI_Intercomm_create_F_leave(void) {
    after_mpi(Intercomm_create);
}

void EAR_MPI_Intercomm_merge_F_enter(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) {
    before_mpi(Intercomm_merge, 0, 0);
}

void EAR_MPI_Intercomm_merge_F_leave(void) {
    after_mpi(Intercomm_merge);
}

void EAR_MPI_Iprobe_F_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Iprobe, (p2i)source, 0);
}

void EAR_MPI_Iprobe_F_leave(void) {
    after_mpi(Iprobe);
}

void EAR_MPI_Irecv_F_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Irecv, (p2i)buf, (p2i)source);
}

void EAR_MPI_Irecv_F_leave(void) {
    after_mpi(Irecv);
}

void EAR_MPI_Irsend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Irsend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Irsend_F_leave(void) {
    after_mpi(Irsend);
}

void EAR_MPI_Isend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Isend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Isend_F_leave(void) {
    after_mpi(Isend);
}

void EAR_MPI_Issend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Issend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Issend_F_leave(void) {
    after_mpi(Issend);
}

void EAR_MPI_Probe_F_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Probe, (p2i)source, 0);
}

void EAR_MPI_Probe_F_leave(void) {
    after_mpi(Probe);
}

void EAR_MPI_Put_F_enter(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Put, (p2i)origin_addr, 0);
}

void EAR_MPI_Put_F_leave(void) {
    after_mpi(Put);
}

void EAR_MPI_Recv_F_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Recv, (p2i)buf, (p2i)source);
}

void EAR_MPI_Recv_F_leave(void) {
    after_mpi(Recv);
}

void EAR_MPI_Recv_init_F_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Recv_init, (p2i)buf, (p2i)source);
}

void EAR_MPI_Recv_init_F_leave(void) {
    after_mpi(Recv_init);
}

void EAR_MPI_Reduce_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Reduce, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Reduce_F_leave(void) {
    after_mpi(Reduce);
}

void EAR_MPI_Reduce_scatter_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Reduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Reduce_scatter_F_leave(void) {
    after_mpi(Reduce_scatter);
}

void EAR_MPI_Request_free_F_enter(MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Request_free, (p2i)request, 0);
}

void EAR_MPI_Request_free_F_leave(void) {
    after_mpi(Request_free);
}

void EAR_MPI_Request_get_status_F_enter(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Request_get_status, (p2i)request, 0);
}

void EAR_MPI_Request_get_status_F_leave(void) {
    after_mpi(Request_get_status);
}

void EAR_MPI_Rsend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Rsend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Rsend_F_leave(void) {
    after_mpi(Rsend);
}

void EAR_MPI_Rsend_init_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Rsend_init, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Rsend_init_F_leave(void) {
    after_mpi(Rsend_init);
}

void EAR_MPI_Scan_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Scan, (p2i)sendbuf, 0);
}

void EAR_MPI_Scan_F_leave(void) {
    after_mpi(Scan);
}

void EAR_MPI_Scatter_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Scatter_F_leave(void) {
    after_mpi(Scatter);
}

void EAR_MPI_Scatterv_F_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Scatterv, (p2i)sendbuf, 0);
}

void EAR_MPI_Scatterv_F_leave(void) {
    after_mpi(Scatterv);
}

void EAR_MPI_Send_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Send, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Send_F_leave(void) {
    after_mpi(Send);
}

void EAR_MPI_Send_init_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Send_init, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Send_init_F_leave(void) {
    after_mpi(Send_init);
}

void EAR_MPI_Sendrecv_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Sendrecv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Sendrecv_F_leave(void) {
    after_mpi(Sendrecv);
}

void EAR_MPI_Sendrecv_replace_F_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Sendrecv_replace, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Sendrecv_replace_F_leave(void) {
    after_mpi(Sendrecv_replace);
}

void EAR_MPI_Ssend_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Ssend, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Ssend_F_leave(void) {
    after_mpi(Ssend);
}

void EAR_MPI_Ssend_init_F_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ssend_init, (p2i)buf, (p2i)dest);
}

void EAR_MPI_Ssend_init_F_leave(void) {
    after_mpi(Ssend_init);
}

void EAR_MPI_Start_F_enter(MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Start, (p2i)request, 0);
}

void EAR_MPI_Start_F_leave(void) {
    after_mpi(Start);
}

void EAR_MPI_Startall_F_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) {
    before_mpi(Startall, (p2i)count, 0);
}

void EAR_MPI_Startall_F_leave(void) {
    after_mpi(Startall);
}

void EAR_MPI_Test_F_enter(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Test, (p2i)request, 0);
}

void EAR_MPI_Test_F_leave(void) {
    after_mpi(Test);
}

void EAR_MPI_Testall_F_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    before_mpi(Testall, 0, 0);
}

void EAR_MPI_Testall_F_leave(void) {
    after_mpi(Testall);
}

void EAR_MPI_Testany_F_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Testany, 0, 0);
}

void EAR_MPI_Testany_F_leave(void) {
    after_mpi(Testany);
}

void EAR_MPI_Testsome_F_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    before_mpi(Testsome, 0, 0);
}

void EAR_MPI_Testsome_F_leave(void) {
    after_mpi(Testsome);
}

void EAR_MPI_Wait_F_enter(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Wait, (p2i)request, 0);
}

void EAR_MPI_Wait_F_leave(void) {
    after_mpi(Wait);
}

void EAR_MPI_Waitall_F_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    before_mpi(Waitall, (p2i)count, 0);
}

void EAR_MPI_Waitall_F_leave(void) {
    after_mpi(Waitall);
}

void EAR_MPI_Waitany_F_enter(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) {
    before_mpi(Waitany, 0, 0);
}

void EAR_MPI_Waitany_F_leave(void) {
    after_mpi(Waitany);
}

void EAR_MPI_Waitsome_F_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    before_mpi(Waitsome, 0, 0);
}

void EAR_MPI_Waitsome_F_leave(void) {
    after_mpi(Waitsome);
}

void EAR_MPI_Win_complete_F_enter(MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_complete, 0, 0);
}

void EAR_MPI_Win_complete_F_leave(void) {
    after_mpi(Win_complete);
}

void EAR_MPI_Win_create_F_enter(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_create, 0, 0);
}

void EAR_MPI_Win_create_F_leave(void) {
    after_mpi(Win_create);
}

void EAR_MPI_Win_fence_F_enter(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_fence, 0, 0);
}

void EAR_MPI_Win_fence_F_leave(void) {
    after_mpi(Win_fence);
}

void EAR_MPI_Win_free_F_enter(MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_free, 0, 0);
}

void EAR_MPI_Win_free_F_leave(void) {
    after_mpi(Win_free);
}

void EAR_MPI_Win_post_F_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_post, 0, 0);
}

void EAR_MPI_Win_post_F_leave(void) {
    after_mpi(Win_post);
}

void EAR_MPI_Win_start_F_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_start, 0, 0);
}

void EAR_MPI_Win_start_F_leave(void) {
    after_mpi(Win_start);
}

void EAR_MPI_Win_wait_F_enter(MPI_Fint *win, MPI_Fint *ierror) {
    before_mpi(Win_wait, 0, 0);
}

void EAR_MPI_Win_wait_F_leave(void) {
    after_mpi(Win_wait);
}


#if MPI_VERSION >= 3
void EAR_MPI_Iallgather_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallgather, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iallgather_F_leave(void) {
    after_mpi(Iallgather);
}

void EAR_MPI_Iallgatherv_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iallgatherv_F_leave(void) {
    after_mpi(Iallgatherv);
}

void EAR_MPI_Iallreduce_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iallreduce_F_leave(void) {
    after_mpi(Iallreduce);
}

void EAR_MPI_Ialltoall_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ialltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Ialltoall_F_leave(void) {
    after_mpi(Ialltoall);
}

void EAR_MPI_Ialltoallv_F_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Ialltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Ialltoallv_F_leave(void) {
    after_mpi(Ialltoallv);
}

void EAR_MPI_Ibarrier_F_enter(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ibarrier, (p2i)request, 0);
}

void EAR_MPI_Ibarrier_F_leave(void) {
    after_mpi(Ibarrier);
}

void EAR_MPI_Ibcast_F_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ibcast, (p2i)buffer, 0);
}

void EAR_MPI_Ibcast_F_leave(void) {
    after_mpi(Ibcast);
}

void EAR_MPI_Igather_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Igather, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Igather_F_leave(void) {
    after_mpi(Igather);
}

void EAR_MPI_Igatherv_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Igatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Igatherv_F_leave(void) {
    after_mpi(Igatherv);
}

void EAR_MPI_Ireduce_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ireduce, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Ireduce_F_leave(void) {
    after_mpi(Ireduce);
}

void EAR_MPI_Ireduce_scatter_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ireduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Ireduce_scatter_F_leave(void) {
    after_mpi(Ireduce_scatter);
}

void EAR_MPI_Iscan_F_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscan, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iscan_F_leave(void) {
    after_mpi(Iscan);
}

void EAR_MPI_Iscatter_F_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscatter, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iscatter_F_leave(void) {
    after_mpi(Iscatter);
}

void EAR_MPI_Iscatterv_F_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscatterv, (p2i)sendbuf, (p2i)recvbuf);
}

void EAR_MPI_Iscatterv_F_leave(void) {
    after_mpi(Iscatterv);
}

#endif

#pragma GCC visibility pop

