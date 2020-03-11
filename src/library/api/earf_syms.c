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

#include <common/output/debug.h>
#include <library/api/api.h>
#include <library/api/earf_syms.h>

void earf_Allgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Allgather...............");
    before_mpi(Allgather, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Allgather_leave(void) {
    debug("<< F Allgather...............");
    after_mpi(Allgather);
}

void earf_Allgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Allgatherv...............");
    before_mpi(Allgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Allgatherv_leave(void) {
    debug("<< F Allgatherv...............");
    after_mpi(Allgatherv);
}

void earf_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Allreduce...............");
    before_mpi(Allreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Allreduce_leave(void) {
    debug("<< F Allreduce...............");
    after_mpi(Allreduce);
}

void earf_Alltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Alltoall...............");
    before_mpi(Alltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Alltoall_leave(void) {
    debug("<< F Alltoall...............");
    after_mpi(Alltoall);
}

void earf_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Alltoallv...............");
    before_mpi(Alltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Alltoallv_leave(void) {
    debug("<< F Alltoallv...............");
    after_mpi(Alltoallv);
}

void earf_Barrier_enter(MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Barrier...............");
    before_mpi(Barrier, (p2i)comm, (p2i)ierror);
}

void earf_Barrier_leave(void) {
    debug("<< F Barrier...............");
    after_mpi(Barrier);
}

void earf_Bcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Bcast...............");
    before_mpi(Bcast, (p2i)buffer, 0);
}

void earf_Bcast_leave(void) {
    debug("<< F Bcast...............");
    after_mpi(Bcast);
}

void earf_Bsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Bsend...............");
    before_mpi(Bsend, (p2i)buf, (p2i)dest);
}

void earf_Bsend_leave(void) {
    debug("<< F Bsend...............");
    after_mpi(Bsend);
}

void earf_Bsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Bsend_init...............");
    before_mpi(Bsend_init, (p2i)buf, (p2i)dest);
}

void earf_Bsend_init_leave(void) {
    debug("<< F Bsend_init...............");
    after_mpi(Bsend_init);
}

void earf_Cancel_enter(MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Cancel...............");
    before_mpi(Cancel, (p2i)request, 0);
}

void earf_Cancel_leave(void) {
    debug("<< F Cancel...............");
    after_mpi(Cancel);
}

void earf_Cart_create_enter(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) {
    debug(">> F Cart_create...............");
    before_mpi(Cart_create, (p2i)ndims, 0);
}

void earf_Cart_create_leave(void) {
    debug("<< F Cart_create...............");
    after_mpi(Cart_create);
}

void earf_Cart_sub_enter(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) {
    debug(">> F Cart_sub...............");
    before_mpi(Cart_sub, (p2i)remain_dims, 0);
}

void earf_Cart_sub_leave(void) {
    debug("<< F Cart_sub...............");
    after_mpi(Cart_sub);
}

void earf_Comm_create_enter(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) {
    debug(">> F Comm_create...............");
    before_mpi(Comm_create, (p2i)group, 0);
}

void earf_Comm_create_leave(void) {
    debug("<< F Comm_create...............");
    after_mpi(Comm_create);
}

void earf_Comm_dup_enter(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) {
    debug(">> F Comm_dup...............");
    before_mpi(Comm_dup, (p2i)newcomm, 0);
}

void earf_Comm_dup_leave(void) {
    debug("<< F Comm_dup...............");
    after_mpi(Comm_dup);
}

void earf_Comm_free_enter(MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Comm_free...............");
    before_mpi(Comm_free, (p2i)comm, 0);
}

void earf_Comm_free_leave(void) {
    debug("<< F Comm_free...............");
    after_mpi(Comm_free);
}

void earf_Comm_rank_enter(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) {
    debug(">> F Comm_rank...............");
    before_mpi(Comm_rank, (p2i)rank, 0);
}

void earf_Comm_rank_leave(void) {
    debug("<< F Comm_rank...............");
    after_mpi(Comm_rank);
}

void earf_Comm_size_enter(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) {
    debug(">> F Comm_size...............");
    before_mpi(Comm_size, (p2i)size, 0);
}

void earf_Comm_size_leave(void) {
    debug("<< F Comm_size...............");
    after_mpi(Comm_size);
}

void earf_Comm_spawn_enter(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
    debug(">> F Comm_spawn...............");
    before_mpi(Comm_spawn, (p2i)command, 0);
}

void earf_Comm_spawn_leave(void) {
    debug("<< F Comm_spawn...............");
    after_mpi(Comm_spawn);
}

void earf_Comm_spawn_multiple_enter(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
    debug(">> F Comm_spawn_multiple...............");
    before_mpi(Comm_spawn_multiple, (p2i)array_of_commands, 0);
}

void earf_Comm_spawn_multiple_leave(void) {
    debug("<< F Comm_spawn_multiple...............");
    after_mpi(Comm_spawn_multiple);
}

void earf_Comm_split_enter(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) {
    debug(">> F Comm_split...............");
    before_mpi(Comm_split, (p2i)comm, 0);
}

void earf_Comm_split_leave(void) {
    debug("<< F Comm_split...............");
    after_mpi(Comm_split);
}

void earf_File_close_enter(MPI_File *fh, MPI_Fint *ierror) {
    debug(">> F File_close...............");
    before_mpi(File_close, (p2i)fh, 0);
}

void earf_File_close_leave(void) {
    debug("<< F File_close...............");
    after_mpi(File_close);
}

void earf_File_read_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_read...............");
    before_mpi(File_read, (p2i)buf, 0);
}

void earf_File_read_leave(void) {
    debug("<< F File_read...............");
    after_mpi(File_read);
}

void earf_File_read_all_enter(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_read_all...............");
    before_mpi(File_read_all, (p2i)buf, 0);
}

void earf_File_read_all_leave(void) {
    debug("<< F File_read_all...............");
    after_mpi(File_read_all);
}

void earf_File_read_at_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_read_at...............");
    before_mpi(File_read_at, (p2i)buf, 0);
}

void earf_File_read_at_leave(void) {
    debug("<< F File_read_at...............");
    after_mpi(File_read_at);
}

void earf_File_read_at_all_enter(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_read_at_all...............");
    before_mpi(File_read_at_all, (p2i)buf, 0);
}

void earf_File_read_at_all_leave(void) {
    debug("<< F File_read_at_all...............");
    after_mpi(File_read_at_all);
}

void earf_File_write_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_write...............");
    before_mpi(File_write, (p2i)buf, 0);
}

void earf_File_write_leave(void) {
    debug("<< F File_write...............");
    after_mpi(File_write);
}

void earf_File_write_all_enter(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_write_all...............");
    before_mpi(File_write_all, (p2i)buf, 0);
}

void earf_File_write_all_leave(void) {
    debug("<< F File_write_all...............");
    after_mpi(File_write_all);
}

void earf_File_write_at_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_write_at...............");
    before_mpi(File_write_at, (p2i)buf, 0);
}

void earf_File_write_at_leave(void) {
    debug("<< F File_write_at...............");
    after_mpi(File_write_at);
}

void earf_File_write_at_all_enter(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
    debug(">> F File_write_at_all...............");
    before_mpi(File_write_at_all, (p2i)buf, 0);
}

void earf_File_write_at_all_leave(void) {
    debug("<< F File_write_at_all...............");
    after_mpi(File_write_at_all);
}

void earf_Finalize_enter(MPI_Fint *ierror) {
    debug(">> F Finalize...............");
    before_finalize();
}

void earf_Finalize_leave(void) {
    debug("<< F Finalize...............");
    after_finalize();
}

void earf_Gather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Gather...............");
    before_mpi(Gather, (p2i)sendbuf, 0);
}

void earf_Gather_leave(void) {
    debug("<< F Gather...............");
    after_mpi(Gather);
}

void earf_Gatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Gatherv...............");
    before_mpi(Gatherv, (p2i)sendbuf, 0);
}

void earf_Gatherv_leave(void) {
    debug("<< F Gatherv...............");
    after_mpi(Gatherv);
}

void earf_Get_enter(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Get...............");
    before_mpi(Get, (p2i)origin_addr, 0);
}

void earf_Get_leave(void) {
    debug("<< F Get...............");
    after_mpi(Get);
}

void earf_Ibsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ibsend...............");
    before_mpi(Ibsend, (p2i)buf, (p2i)dest);
}

void earf_Ibsend_leave(void) {
    debug("<< F Ibsend...............");
    after_mpi(Ibsend);
}

void earf_Init_enter(MPI_Fint *ierror) {
    debug(">> F Init...............");
    before_init();
}

void earf_Init_leave(void) {
    debug("<< F Init...............");
    after_init();
}

void earf_Init_thread_enter(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) {
    debug(">> F Init_thread...............");
    before_init();
}

void earf_Init_thread_leave(void) {
    debug("<< F Init_thread...............");
    after_init();
}

void earf_Intercomm_create_enter(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) {
    debug(">> F Intercomm_create...............");
    before_mpi(Intercomm_create,0 , 0);
}

void earf_Intercomm_create_leave(void) {
    debug("<< F Intercomm_create...............");
    after_mpi(Intercomm_create);
}

void earf_Intercomm_merge_enter(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) {
    debug(">> F Intercomm_merge...............");
    before_mpi(Intercomm_merge, 0, 0);
}

void earf_Intercomm_merge_leave(void) {
    debug("<< F Intercomm_merge...............");
    after_mpi(Intercomm_merge);
}

void earf_Iprobe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Iprobe...............");
    before_mpi(Iprobe, (p2i)source, 0);
}

void earf_Iprobe_leave(void) {
    debug("<< F Iprobe...............");
    after_mpi(Iprobe);
}

void earf_Irecv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Irecv...............");
    before_mpi(Irecv, (p2i)buf, (p2i)source);
}

void earf_Irecv_leave(void) {
    debug("<< F Irecv...............");
    after_mpi(Irecv);
}

void earf_Irsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Irsend...............");
    before_mpi(Irsend, (p2i)buf, (p2i)dest);
}

void earf_Irsend_leave(void) {
    debug("<< F Irsend...............");
    after_mpi(Irsend);
}

void earf_Isend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Isend...............");
    before_mpi(Isend, (p2i)buf, (p2i)dest);
}

void earf_Isend_leave(void) {
    debug("<< F Isend...............");
    after_mpi(Isend);
}

void earf_Issend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Issend...............");
    before_mpi(Issend, (p2i)buf, (p2i)dest);
}

void earf_Issend_leave(void) {
    debug("<< F Issend...............");
    after_mpi(Issend);
}

void earf_Probe_enter(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Probe...............");
    before_mpi(Probe, (p2i)source, 0);
}

void earf_Probe_leave(void) {
    debug("<< F Probe...............");
    after_mpi(Probe);
}

void earf_Put_enter(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Put...............");
    before_mpi(Put, (p2i)origin_addr, 0);
}

void earf_Put_leave(void) {
    debug("<< F Put...............");
    after_mpi(Put);
}

void earf_Recv_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Recv...............");
    before_mpi(Recv, (p2i)buf, (p2i)source);
}

void earf_Recv_leave(void) {
    debug("<< F Recv...............");
    after_mpi(Recv);
}

void earf_Recv_init_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Recv_init...............");
    before_mpi(Recv_init, (p2i)buf, (p2i)source);
}

void earf_Recv_init_leave(void) {
    debug("<< F Recv_init...............");
    after_mpi(Recv_init);
}

void earf_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Reduce...............");
    before_mpi(Reduce, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Reduce_leave(void) {
    debug("<< F Reduce...............");
    after_mpi(Reduce);
}

void earf_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Reduce_scatter...............");
    before_mpi(Reduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Reduce_scatter_leave(void) {
    debug("<< F Reduce_scatter...............");
    after_mpi(Reduce_scatter);
}

void earf_Request_free_enter(MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Request_free...............");
    before_mpi(Request_free, (p2i)request, 0);
}

void earf_Request_free_leave(void) {
    debug("<< F Request_free...............");
    after_mpi(Request_free);
}

void earf_Request_get_status_enter(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Request_get_status...............");
    before_mpi(Request_get_status, (p2i)request, 0);
}

void earf_Request_get_status_leave(void) {
    debug("<< F Request_get_status...............");
    after_mpi(Request_get_status);
}

void earf_Rsend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Rsend...............");
    before_mpi(Rsend, (p2i)buf, (p2i)dest);
}

void earf_Rsend_leave(void) {
    debug("<< F Rsend...............");
    after_mpi(Rsend);
}

void earf_Rsend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Rsend_init...............");
    before_mpi(Rsend_init, (p2i)buf, (p2i)dest);
}

void earf_Rsend_init_leave(void) {
    debug("<< F Rsend_init...............");
    after_mpi(Rsend_init);
}

void earf_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Scan...............");
    before_mpi(Scan, (p2i)sendbuf, 0);
}

void earf_Scan_leave(void) {
    debug("<< F Scan...............");
    after_mpi(Scan);
}

void earf_Scatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Scatter...............");
    before_mpi(Scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Scatter_leave(void) {
    debug("<< F Scatter...............");
    after_mpi(Scatter);
}

void earf_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Scatterv...............");
    before_mpi(Scatterv, (p2i)sendbuf, 0);
}

void earf_Scatterv_leave(void) {
    debug("<< F Scatterv...............");
    after_mpi(Scatterv);
}

void earf_Send_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Send...............");
    before_mpi(Send, (p2i)buf, (p2i)dest);
}

void earf_Send_leave(void) {
    debug("<< F Send...............");
    after_mpi(Send);
}

void earf_Send_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Send_init...............");
    before_mpi(Send_init, (p2i)buf, (p2i)dest);
}

void earf_Send_init_leave(void) {
    debug("<< F Send_init...............");
    after_mpi(Send_init);
}

void earf_Sendrecv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Sendrecv...............");
    before_mpi(Sendrecv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Sendrecv_leave(void) {
    debug("<< F Sendrecv...............");
    after_mpi(Sendrecv);
}

void earf_Sendrecv_replace_enter(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Sendrecv_replace...............");
    before_mpi(Sendrecv_replace, (p2i)buf, (p2i)dest);
}

void earf_Sendrecv_replace_leave(void) {
    debug("<< F Sendrecv_replace...............");
    after_mpi(Sendrecv_replace);
}

void earf_Ssend_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Ssend...............");
    before_mpi(Ssend, (p2i)buf, (p2i)dest);
}

void earf_Ssend_leave(void) {
    debug("<< F Ssend...............");
    after_mpi(Ssend);
}

void earf_Ssend_init_enter(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ssend_init...............");
    before_mpi(Ssend_init, (p2i)buf, (p2i)dest);
}

void earf_Ssend_init_leave(void) {
    debug("<< F Ssend_init...............");
    after_mpi(Ssend_init);
}

void earf_Start_enter(MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Start...............");
    before_mpi(Start, (p2i)request, 0);
}

void earf_Start_leave(void) {
    debug("<< F Start...............");
    after_mpi(Start);
}

void earf_Startall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) {
    debug(">> F Startall...............");
    before_mpi(Startall, (p2i)count, 0);
}

void earf_Startall_leave(void) {
    debug("<< F Startall...............");
    after_mpi(Startall);
}

void earf_Test_enter(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Test...............");
    before_mpi(Test, (p2i)request, 0);
}

void earf_Test_leave(void) {
    debug("<< F Test...............");
    after_mpi(Test);
}

void earf_Testall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    debug(">> F Testall...............");
    before_mpi(Testall, 0, 0);
}

void earf_Testall_leave(void) {
    debug("<< F Testall...............");
    after_mpi(Testall);
}

void earf_Testany_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Testany...............");
    before_mpi(Testany, 0, 0);
}

void earf_Testany_leave(void) {
    debug("<< F Testany...............");
    after_mpi(Testany);
}

void earf_Testsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    debug(">> F Testsome...............");
    before_mpi(Testsome, 0, 0);
}

void earf_Testsome_leave(void) {
    debug("<< F Testsome...............");
    after_mpi(Testsome);
}

void earf_Wait_enter(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Wait...............");
    before_mpi(Wait, (p2i)request, 0);
}

void earf_Wait_leave(void) {
    debug("<< F Wait...............");
    after_mpi(Wait);
}

void earf_Waitall_enter(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    debug(">> F Waitall...............");
    before_mpi(Waitall, (p2i)count, 0);
}

void earf_Waitall_leave(void) {
    debug("<< F Waitall...............");
    after_mpi(Waitall);
}

void earf_Waitany_enter(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) {
    debug(">> F Waitany...............");
    before_mpi(Waitany, 0, 0);
}

void earf_Waitany_leave(void) {
    debug("<< F Waitany...............");
    after_mpi(Waitany);
}

void earf_Waitsome_enter(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
    debug(">> F Waitsome...............");
    before_mpi(Waitsome, 0, 0);
}

void earf_Waitsome_leave(void) {
    debug("<< F Waitsome...............");
    after_mpi(Waitsome);
}

void earf_Win_complete_enter(MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_complete...............");
    before_mpi(Win_complete, 0, 0);
}

void earf_Win_complete_leave(void) {
    debug("<< F Win_complete...............");
    after_mpi(Win_complete);
}

void earf_Win_create_enter(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_create...............");
    before_mpi(Win_create, 0, 0);
}

void earf_Win_create_leave(void) {
    debug("<< F Win_create...............");
    after_mpi(Win_create);
}

void earf_Win_fence_enter(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_fence...............");
    before_mpi(Win_fence, 0, 0);
}

void earf_Win_fence_leave(void) {
    debug("<< F Win_fence...............");
    after_mpi(Win_fence);
}

void earf_Win_free_enter(MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_free...............");
    before_mpi(Win_free, 0, 0);
}

void earf_Win_free_leave(void) {
    debug("<< F Win_free...............");
    after_mpi(Win_free);
}

void earf_Win_post_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_post...............");
    before_mpi(Win_post, 0, 0);
}

void earf_Win_post_leave(void) {
    debug("<< F Win_post...............");
    after_mpi(Win_post);
}

void earf_Win_start_enter(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_start...............");
    before_mpi(Win_start, 0, 0);
}

void earf_Win_start_leave(void) {
    debug("<< F Win_start...............");
    after_mpi(Win_start);
}

void earf_Win_wait_enter(MPI_Fint *win, MPI_Fint *ierror) {
    debug(">> F Win_wait...............");
    before_mpi(Win_wait, 0, 0);
}

void earf_Win_wait_leave(void) {
    debug("<< F Win_wait...............");
    after_mpi(Win_wait);
}

#if MPI_VERSION >= 3
void earf_Iallgather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iallgather...............");
    before_mpi(Iallgather, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iallgather_leave(void) {
    debug("<< F Iallgather...............");
    after_mpi(Iallgather);
}

void earf_Iallgatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iallgatherv...............");
    before_mpi(Iallgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iallgatherv_leave(void) {
    debug("<< F Iallgatherv...............");
    after_mpi(Iallgatherv);
}

void earf_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iallreduce...............");
    before_mpi(Iallreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iallreduce_leave(void) {
    debug("<< F Iallreduce...............");
    after_mpi(Iallreduce);
}

void earf_Ialltoall_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ialltoall...............");
    before_mpi(Ialltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Ialltoall_leave(void) {
    debug("<< F Ialltoall...............");
    after_mpi(Ialltoall);
}

void earf_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) {
    debug(">> F Ialltoallv...............");
    before_mpi(Ialltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Ialltoallv_leave(void) {
    debug("<< F Ialltoallv...............");
    after_mpi(Ialltoallv);
}

void earf_Ibarrier_enter(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ibarrier...............");
    before_mpi(Ibarrier, (p2i)request, 0);
}

void earf_Ibarrier_leave(void) {
    debug("<< F Ibarrier...............");
    after_mpi(Ibarrier);
}

void earf_Ibcast_enter(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ibcast...............");
    before_mpi(Ibcast, (p2i)buffer, 0);
}

void earf_Ibcast_leave(void) {
    debug("<< F Ibcast...............");
    after_mpi(Ibcast);
}

void earf_Igather_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Igather...............");
    before_mpi(Igather, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Igather_leave(void) {
    debug("<< F Igather...............");
    after_mpi(Igather);
}

void earf_Igatherv_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Igatherv...............");
    before_mpi(Igatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Igatherv_leave(void) {
    debug("<< F Igatherv...............");
    after_mpi(Igatherv);
}

void earf_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ireduce...............");
    before_mpi(Ireduce, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Ireduce_leave(void) {
    debug("<< F Ireduce...............");
    after_mpi(Ireduce);
}

void earf_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Ireduce_scatter...............");
    before_mpi(Ireduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Ireduce_scatter_leave(void) {
    debug("<< F Ireduce_scatter...............");
    after_mpi(Ireduce_scatter);
}

void earf_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iscan...............");
    before_mpi(Iscan, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iscan_leave(void) {
    debug("<< F Iscan...............");
    after_mpi(Iscan);
}

void earf_Iscatter_enter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iscatter...............");
    before_mpi(Iscatter, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iscatter_leave(void) {
    debug("<< F Iscatter...............");
    after_mpi(Iscatter);
}

void earf_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    debug(">> F Iscatterv...............");
    before_mpi(Iscatterv, (p2i)sendbuf, (p2i)recvbuf);
}

void earf_Iscatterv_leave(void) {
    debug("<< F Iscatterv...............");
    after_mpi(Iscatterv);
}
#endif
