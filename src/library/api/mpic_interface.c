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
#include <library/api/mpic_interface.h>

void mpic_ear_Allgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    debug(">> C Allgather...............");
    before_mpi(Allgather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allgather_leave(void) {
    debug("<< C Allgather...............");
    after_mpi(Allgather);
}

void mpic_ear_Allgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    debug(">> C Allgatherv...............");
    before_mpi(Allgatherv,(p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allgatherv_leave(void) {
    debug("<< C Allgatherv...............");
    after_mpi(Allgatherv);
}

void mpic_ear_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    debug(">> C Allreduce...............");
    before_mpi(Allreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allreduce_leave(void) {
    debug("<< C Allreduce...............");
    after_mpi(Allreduce);
}

void mpic_ear_Alltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    debug(">> C Alltoall...............");
    before_mpi(Alltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Alltoall_leave(void) {
    debug("<< C Alltoall...............");
    after_mpi(Alltoall);
}

void mpic_ear_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
    debug(">> C Alltoallv...............");
    before_mpi(Alltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Alltoallv_leave(void) {
    debug("<< C Alltoallv...............");
    after_mpi(Alltoallv);
}

void mpic_ear_Barrier_enter(MPI_Comm comm) {
    debug(">> C Barrier...............");
    before_mpi(Barrier, (p2i)comm,0);
}

void mpic_ear_Barrier_leave(void) {
    debug("<< C Barrier...............");
    after_mpi(Barrier);
}

void mpic_ear_Bcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    debug(">> C Bcast...............");
    before_mpi(Bcast, (p2i)comm,0);
}

void mpic_ear_Bcast_leave(void) {
    debug("<< C Bcast...............");
    after_mpi(Bcast);
}

void mpic_ear_Bsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    debug(">> C Bsend...............");
    before_mpi(Bsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Bsend_leave(void) {
    debug("<< C Bsend...............");
    after_mpi(Bsend);
}

void mpic_ear_Bsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Bsend_init...............");
    before_mpi(Bsend_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Bsend_init_leave(void) {
    debug("<< C Bsend_init...............");
    after_mpi(Bsend_init);
}

void mpic_ear_Cancel_enter(MPI_Request *request) {
    debug(">> C Cancel...............");
    before_mpi(Cancel, (p2i)request,(p2i)0);
}

void mpic_ear_Cancel_leave(void) {
    debug("<< C Cancel...............");
    after_mpi(Cancel);
}

void mpic_ear_Cart_create_enter(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
    debug(">> C Cart_create...............");
    before_mpi(Cart_create, (p2i)ndims,(p2i)comm_cart);

}

void mpic_ear_Cart_create_leave(void) {
    debug("<< C Cart_create...............");
    after_mpi(Cart_create);
}

void mpic_ear_Cart_sub_enter(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
    debug(">> C Cart_sub...............");
    before_mpi(Cart_sub, (p2i)remain_dims,(p2i)newcomm);
}

void mpic_ear_Cart_sub_leave(void) {
    debug("<< C Cart_sub...............");
    after_mpi(Cart_sub);
}

void mpic_ear_Comm_create_enter(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
    debug(">> C Comm_create...............");
    before_mpi(Comm_create, (p2i)group,(p2i)newcomm);
}

void mpic_ear_Comm_create_leave(void) {
    debug("<< C Comm_create...............");
    after_mpi(Comm_create);
}

void mpic_ear_Comm_dup_enter(MPI_Comm comm, MPI_Comm *newcomm) {
    debug(">> C Comm_dup...............");
    before_mpi(Comm_dup, (p2i)newcomm,(p2i)0);
}

void mpic_ear_Comm_dup_leave(void) {
    debug("<< C Comm_dup...............");
    after_mpi(Comm_dup);
}

void mpic_ear_Comm_free_enter(MPI_Comm *comm) {
    debug(">> C Comm_free...............");
    before_mpi(Comm_free, (p2i)comm,(p2i)0);
}

void mpic_ear_Comm_free_leave(void) {
    debug("<< C Comm_free...............");
    after_mpi(Comm_free);
}

void mpic_ear_Comm_rank_enter(MPI_Comm comm, int *rank) {
    debug(">> C Comm_rank...............");
    before_mpi(Comm_rank, (p2i)comm,(p2i)rank);
}

void mpic_ear_Comm_rank_leave(void) {
    debug("<< C Comm_rank...............");
    after_mpi(Comm_rank);
}

void mpic_ear_Comm_size_enter(MPI_Comm comm, int *size) {
    debug(">> C Comm_size...............");
    before_mpi(Comm_size, (p2i)size,(p2i)0);
}

void mpic_ear_Comm_size_leave(void) {
    debug("<< C Comm_size...............");
    after_mpi(Comm_size);
}

void mpic_ear_Comm_spawn_enter(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    debug(">> C Comm_spawn...............");
    before_mpi(Comm_spawn, (p2i)command,(p2i)info);
}

void mpic_ear_Comm_spawn_leave(void) {
    debug("<< C Comm_spawn...............");
    after_mpi(Comm_spawn);
}

void mpic_ear_Comm_spawn_multiple_enter(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
    debug(">> C Comm_spawn_multiple...............");
    before_mpi(Comm_spawn_multiple, (p2i)array_of_commands,(p2i)array_of_info);
}

void mpic_ear_Comm_spawn_multiple_leave(void) {
    debug("<< C Comm_spawn_multiple...............");
    after_mpi(Comm_spawn_multiple);
}

void mpic_ear_Comm_split_enter(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
    debug(">> C Comm_split...............");
    before_mpi(Comm_split, (p2i)key,(p2i)newcomm);
}

void mpic_ear_Comm_split_leave(void) {
    debug("<< C Comm_split...............");
    after_mpi(Comm_split);
}

void mpic_ear_File_close_enter(MPI_File *fh) {
    debug(">> C File_close...............");
    before_mpi(File_close, (p2i)fh,(p2i)0);
}

void mpic_ear_File_close_leave(void) {
    debug("<< C File_close...............");
    after_mpi(File_close);
}

void mpic_ear_File_read_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_read...............");
    before_mpi(File_read, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_leave(void) {
    debug("<< C File_read...............");
    after_mpi(File_read);
}

void mpic_ear_File_read_all_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_read_all...............");
    before_mpi(File_read_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_all_leave(void) {
    debug("<< C File_read_all...............");
    after_mpi(File_read_all);
}

void mpic_ear_File_read_at_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_read_at...............");
    before_mpi(File_read_at, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_at_leave(void) {
    debug("<< C File_read_at...............");
    after_mpi(File_read_at);
}

void mpic_ear_File_read_at_all_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_read_at_all...............");
    before_mpi(File_read_at_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_at_all_leave(void) {
    debug("<< C File_read_at_all...............");
    after_mpi(File_read_at_all);
}

void mpic_ear_File_write_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_write...............");
    before_mpi(File_write, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_leave(void) {
    debug("<< C File_write...............");
    after_mpi(File_write);
}

void mpic_ear_File_write_all_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_write_all...............");
    before_mpi(File_write_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_all_leave(void) {
    debug("<< C File_write_all...............");
    after_mpi(File_write_all);
}

void mpic_ear_File_write_at_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_write_at...............");
    before_mpi(File_write_at, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_at_leave(void) {
    debug("<< C File_write_at...............");
    after_mpi(File_write_at);
}

void mpic_ear_File_write_at_all_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
    debug(">> C File_write_at_all...............");
    before_mpi(File_write_at_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_at_all_leave(void) {
    debug("<< C File_write_at_all...............");
    after_mpi(File_write_at_all);
}

void mpic_ear_Finalize_enter(void) {
    debug(">> C Finalize...............");
    before_finalize();
}

void mpic_ear_Finalize_leave(void) {
    debug("<< C Finalize...............");
    after_finalize();
}

void mpic_ear_Gather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    debug(">> C Gather...............");
    before_mpi(Gather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Gather_leave(void) {
    debug("<< C Gather...............");
    after_mpi(Gather);
}

void mpic_ear_Gatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    debug(">> C Gatherv...............");
    before_mpi(Gatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Gatherv_leave(void) {
    debug("<< C Gatherv...............");
    after_mpi(Gatherv);
}

void mpic_ear_Get_enter(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    debug(">> C Get...............");
    before_mpi(Get, (p2i)origin_addr,(p2i)origin_datatype);
}

void mpic_ear_Get_leave(void) {
    debug("<< C Get...............");
    after_mpi(Get);
}

void mpic_ear_Ibsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ibsend...............");
    before_mpi(Ibsend, (p2i)buf,(p2i)datatype);
}

void mpic_ear_Ibsend_leave(void) {
    debug("<< C Ibsend...............");
    after_mpi(Ibsend);
}

void mpic_ear_Init_enter(int *argc, char ***argv) {
    debug(">> C Init...............");
    before_init();
}

void mpic_ear_Init_leave(void) {
    debug("<< C Init...............");
    after_init();
}

void mpic_ear_Init_thread_enter(int *argc, char ***argv, int required, int *provided) {
    debug(">> C Init_thread...............");
    before_init();
}

void mpic_ear_Init_thread_leave(void) {
    debug("<< C Init_thread...............");
    after_init();
}

void mpic_ear_Intercomm_create_enter(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
    debug(">> C Intercomm_create...............");
    before_mpi(Intercomm_create, (p2i)local_leader,(p2i)remote_leader);
}

void mpic_ear_Intercomm_create_leave(void) {
    debug("<< C Intercomm_create...............");
    after_mpi(Intercomm_create);
}

void mpic_ear_Intercomm_merge_enter(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
    debug(">> C Intercomm_merge...............");
    before_mpi(Intercomm_merge, (p2i)newintracomm,(p2i)0);
}

void mpic_ear_Intercomm_merge_leave(void) {
    debug("<< C Intercomm_merge...............");
    after_mpi(Intercomm_merge);
}

void mpic_ear_Iprobe_enter(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
    debug(">> C Iprobe...............");
    before_mpi(Iprobe, (p2i)flag,(p2i)status);
}

void mpic_ear_Iprobe_leave(void) {
    debug("<< C Iprobe...............");
    after_mpi(Iprobe);
}

void mpic_ear_Irecv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Irecv...............");
    before_mpi(Irecv, (p2i)buf,(p2i)request);
}

void mpic_ear_Irecv_leave(void) {
    debug("<< C Irecv...............");
    after_mpi(Irecv);
}

void mpic_ear_Irsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Irsend...............");
    before_mpi(Irsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Irsend_leave(void) {
    debug("<< C Irsend...............");
    after_mpi(Irsend);
}

void mpic_ear_Isend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Isend...............");
    before_mpi(Isend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Isend_leave(void) {
    debug("<< C Isend...............");
    after_mpi(Isend);
}

void mpic_ear_Issend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Issend...............");
    before_mpi(Issend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Issend_leave(void) {
    debug("<< C Issend...............");
    after_mpi(Issend);
}

void mpic_ear_Probe_enter(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    debug(">> C Probe...............");
    before_mpi(Probe, (p2i)source,(p2i)0);
}

void mpic_ear_Probe_leave(void) {
    debug("<< C Probe...............");
    after_mpi(Probe);
}

void mpic_ear_Put_enter(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
    debug(">> C Put...............");
    before_mpi(Put, (p2i)origin_addr,(p2i)0);
}

void mpic_ear_Put_leave(void) {
    debug("<< C Put...............");
    after_mpi(Put);
}

void mpic_ear_Recv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    debug(">> C Recv...............");
    before_mpi(Recv, (p2i)buf,(p2i)source);
}

void mpic_ear_Recv_leave(void) {
    debug("<< C Recv...............");
    after_mpi(Recv);
}

void mpic_ear_Recv_init_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Recv_init...............");
    before_mpi(Recv_init, (p2i)buf,(p2i)source);
}

void mpic_ear_Recv_init_leave(void) {
    debug("<< C Recv_init...............");
    after_mpi(Recv_init);
}

void mpic_ear_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    debug(">> C Reduce...............");
    before_mpi(Reduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Reduce_leave(void) {
    debug("<< C Reduce...............");
    after_mpi(Reduce);
}

void mpic_ear_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    debug(">> C Reduce_scatter...............");
    before_mpi(Reduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Reduce_scatter_leave(void) {
    debug("<< C Reduce_scatter...............");
    after_mpi(Reduce_scatter);
}

void mpic_ear_Request_free_enter(MPI_Request *request) {
    debug(">> C Request_free...............");
    before_mpi(Request_free, (p2i)request,(p2i)0);
}

void mpic_ear_Request_free_leave(void) {
    debug("<< C Request_free...............");
    after_mpi(Request_free);
}

void mpic_ear_Request_get_status_enter(MPI_Request request, int *flag, MPI_Status *status) {
    debug(">> C Request_get_status...............");
    before_mpi(Request_get_status, (p2i)request,(p2i)0);
}

void mpic_ear_Request_get_status_leave(void) {
    debug("<< C Request_get_status...............");
    after_mpi(Request_get_status);
}

void mpic_ear_Rsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    debug(">> C Rsend...............");
    before_mpi(Rsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Rsend_leave(void) {
    debug("<< C Rsend...............");
    after_mpi(Rsend);
}

void mpic_ear_Rsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Rsend_init...............");
    before_mpi(Rsend_init, (p2i)buf,(p2i)0);
}

void mpic_ear_Rsend_init_leave(void) {
    debug("<< C Rsend_init...............");
    after_mpi(Rsend_init);
}

void mpic_ear_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    debug(">> C Scan...............");
    before_mpi(Scan, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scan_leave(void) {
    debug("<< C Scan...............");
    after_mpi(Scan);
}

void mpic_ear_Scatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    debug(">> C Scatter...............");
    before_mpi(Scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scatter_leave(void) {
    debug("<< C Scatter...............");
    after_mpi(Scatter);
}

void mpic_ear_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    debug(">> C Scatterv...............");
    before_mpi(Scatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scatterv_leave(void) {
    debug("<< C Scatterv...............");
    after_mpi(Scatterv);
}

void mpic_ear_Send_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    debug(">> C Send...............");
    before_mpi(Send, (p2i)buf,(p2i)dest);
}

void mpic_ear_Send_leave(void) {
    debug("<< C Send...............");
    after_mpi(Send);
}

void mpic_ear_Send_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Send_init...............");
    before_mpi(Send_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Send_init_leave(void) {
    debug("<< C Send_init...............");
    after_mpi(Send_init);
}

void mpic_ear_Sendrecv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    debug(">> C Sendrecv...............");
    before_mpi(Sendrecv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Sendrecv_leave(void) {
    debug("<< C Sendrecv...............");
    after_mpi(Sendrecv);
}

void mpic_ear_Sendrecv_replace_enter(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
    debug(">> C Sendrecv_replace...............");
    before_mpi(Sendrecv_replace, (p2i)buf,(p2i)dest);
}

void mpic_ear_Sendrecv_replace_leave(void) {
    debug("<< C Sendrecv_replace...............");
    after_mpi(Sendrecv_replace);
}

void mpic_ear_Ssend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
    debug(">> C Ssend...............");
    before_mpi(Ssend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Ssend_leave(void) {
    debug("<< C Ssend...............");
    after_mpi(Ssend);
}

void mpic_ear_Ssend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ssend_init...............");
    before_mpi(Ssend_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Ssend_init_leave(void) {
    debug("<< C Ssend_init...............");
    after_mpi(Ssend_init);
}

void mpic_ear_Start_enter(MPI_Request *request) {
    debug(">> C Start...............");
    before_mpi(Start, (p2i)request,(p2i)0);
}

void mpic_ear_Start_leave(void) {
    debug("<< C Start...............");
    after_mpi(Start);
}

void mpic_ear_Startall_enter(int count, MPI_Request array_of_requests[]) {
    debug(">> C Startall...............");
    before_mpi(Startall, (p2i)array_of_requests,(p2i)0);
}

void mpic_ear_Startall_leave(void) {
    debug("<< C Startall...............");
    after_mpi(Startall);
}

void mpic_ear_Test_enter(MPI_Request *request, int *flag, MPI_Status *status) {
    debug(">> C Test...............");
    before_mpi(Test, (p2i)request,(p2i)0);
}

void mpic_ear_Test_leave(void) {
    debug("<< C Test...............");
    after_mpi(Test);
}

void mpic_ear_Testall_enter(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
    debug(">> C Testall...............");
    before_mpi(Testall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void mpic_ear_Testall_leave(void) {
    debug("<< C Testall...............");
    after_mpi(Testall);
}

void mpic_ear_Testany_enter(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
    debug(">> C Testany...............");
    before_mpi(Testany, (p2i)array_of_requests,(p2i)flag);
}

void mpic_ear_Testany_leave(void) {
    debug("<< C Testany...............");
    after_mpi(Testany);
}

void mpic_ear_Testsome_enter(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
    debug(">> C Testsome...............");
    before_mpi(Testsome, (p2i)array_of_requests,(p2i)outcount);
}

void mpic_ear_Testsome_leave(void) {
    debug("<< C Testsome...............");
    after_mpi(Testsome);
}

void mpic_ear_Wait_enter(MPI_Request *request, MPI_Status *status) {
    debug(">> C Wait...............");
    before_mpi(Wait, (p2i)request,(p2i)0);
}

void mpic_ear_Wait_leave(void) {
    debug("<< C Wait...............");
    after_mpi(Wait);
}

void mpic_ear_Waitall_enter(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
    debug(">> C Waitall...............");
    before_mpi(Waitall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void mpic_ear_Waitall_leave(void) {
    debug("<< C Waitall...............");
    after_mpi(Waitall);
}

void mpic_ear_Waitany_enter(int count, MPI_Request *requests, int *index, MPI_Status *status) {
    debug(">> C Waitany...............");
    before_mpi(Waitany, (p2i)requests,(p2i)index);
}

void mpic_ear_Waitany_leave(void) {
    debug("<< C Waitany...............");
    after_mpi(Waitany);
}

void mpic_ear_Waitsome_enter(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
    debug(">> C Waitsome...............");
    before_mpi(Waitsome, (p2i)array_of_requests,(p2i)outcount);
}

void mpic_ear_Waitsome_leave(void) {
    debug("<< C Waitsome...............");
    after_mpi(Waitsome);
}

void mpic_ear_Win_complete_enter(MPI_Win win) {
    debug(">> C Win_complete...............");
    before_mpi(Win_complete, (p2i)win,(p2i)0);
}

void mpic_ear_Win_complete_leave(void) {
    debug("<< C Win_complete...............");
    after_mpi(Win_complete);
}

void mpic_ear_Win_create_enter(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
    debug(">> C Win_create...............");
    before_mpi(Win_create, (p2i)base,(p2i)info);
}

void mpic_ear_Win_create_leave(void) {
    debug("<< C Win_create...............");
    after_mpi(Win_create);
}

void mpic_ear_Win_fence_enter(int assert, MPI_Win win) {
    debug(">> C Win_fence...............");
    before_mpi(Win_fence, (p2i)win,(p2i)0);
}

void mpic_ear_Win_fence_leave(void) {
    debug("<< C Win_fence...............");
    after_mpi(Win_fence);
}

void mpic_ear_Win_free_enter(MPI_Win *win) {
    debug(">> C Win_free...............");
    before_mpi(Win_free, (p2i)win,(p2i)0);
}

void mpic_ear_Win_free_leave(void) {
    debug("<< C Win_free...............");
    after_mpi(Win_free);
}

void mpic_ear_Win_post_enter(MPI_Group group, int assert, MPI_Win win) {
    debug(">> C Win_post...............");
    before_mpi(Win_post, (p2i)win,(p2i)0);
}

void mpic_ear_Win_post_leave(void) {
    debug("<< C Win_post...............");
    after_mpi(Win_post);
}

void mpic_ear_Win_start_enter(MPI_Group group, int assert, MPI_Win win) {
    debug(">> C Win_start...............");
    before_mpi(Win_start, (p2i)win,(p2i)0);
}

void mpic_ear_Win_start_leave(void) {
    debug("<< C Win_start...............");
    after_mpi(Win_start);
}

void mpic_ear_Win_wait_enter(MPI_Win win) {
    debug(">> C Win_wait...............");
    before_mpi(Win_wait, (p2i)win,(p2i)0);
}

void mpic_ear_Win_wait_leave(void) {
    debug("<< C Win_wait...............");
    after_mpi(Win_wait);
}

#if MPI_VERSION >= 3
void mpic_ear_Iallgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iallgather...............");
    before_mpi(Iallgather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallgather_leave(void) {
    debug("<< C Iallgather...............");
    after_mpi(Iallgather);
}

void mpic_ear_Iallgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iallgatherv...............");
    before_mpi(Iallgatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallgatherv_leave(void) {
    debug("<< C Iallgatherv...............");
    after_mpi(Iallgatherv);
}

void mpic_ear_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iallreduce...............");
    before_mpi(Iallreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallreduce_leave(void) {
    debug("<< C Iallreduce...............");
    after_mpi(Iallreduce);
}

void mpic_ear_Ialltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ialltoall...............");
    before_mpi(Ialltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ialltoall_leave(void) {
    debug("<< C Ialltoall...............");
    after_mpi(Ialltoall);
}

void mpic_ear_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ialltoallv...............");
    before_mpi(Ialltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ialltoallv_leave(void) {
    debug("<< C Ialltoallv...............");
    after_mpi(Ialltoallv);
}

void mpic_ear_Ibarrier_enter(MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ibarrier...............");
    before_mpi(Ibarrier, (p2i)request,(p2i)0);
}

void mpic_ear_Ibarrier_leave(void) {
    debug("<< C Ibarrier...............");
    after_mpi(Ibarrier);
}

void mpic_ear_Ibcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ibcast...............");
    before_mpi(Ibcast, (p2i)buffer,(p2i)request);
}

void mpic_ear_Ibcast_leave(void) {
    debug("<< C Ibcast...............");
    after_mpi(Ibcast);
}

void mpic_ear_Igather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Igather...............");
    before_mpi(Igather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Igather_leave(void) {
    debug("<< C Igather...............");
    after_mpi(Igather);
}

void mpic_ear_Igatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Igatherv...............");
    before_mpi(Igatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Igatherv_leave(void) {
    debug("<< C Igatherv...............");
    after_mpi(Igatherv);
}

void mpic_ear_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ireduce...............");
    before_mpi(Ireduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ireduce_leave(void) {
    debug("<< C Ireduce...............");
    after_mpi(Ireduce);
}

void mpic_ear_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Ireduce_scatter...............");
    before_mpi(Ireduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ireduce_scatter_leave(void) {
    debug("<< C Ireduce_scatter...............");
    after_mpi(Ireduce_scatter);
}

void mpic_ear_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iscan...............");
    before_mpi(Iscan, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscan_leave(void) {
    debug("<< C Iscan...............");
    after_mpi(Iscan);
}

void mpic_ear_Iscatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iscatter...............");
    before_mpi(Iscatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscatter_leave(void) {
    debug("<< C Iscatter...............");
    after_mpi(Iscatter);
}

void mpic_ear_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    debug(">> C Iscatterv...............");
    before_mpi(Iscatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscatterv_leave(void) {
    debug("<< C Iscatterv...............");
    after_mpi(Iscatterv);
}
#endif