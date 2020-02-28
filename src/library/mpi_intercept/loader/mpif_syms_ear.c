#include <library/mpi_intercept/loader/mpif_syms_ear.h>

void mpif_ear_Allgather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Allgather, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Allgather_out(void) {
	after_mpi(Allgather);
}

void mpif_ear_Allgatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Allgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Allgatherv_out(void) {
	after_mpi(Allgatherv);
}

void mpif_ear_Allreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Allreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Allreduce_out(void) {
	after_mpi(Allreduce);
}

void mpif_ear_Alltoall_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Alltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Alltoall_out(void) {
	after_mpi(Alltoall);
}

void mpif_ear_Alltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Alltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Alltoallv_out(void) {
	after_mpi(Alltoallv);
}

void mpif_ear_Barrier_in(MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Barrier, (p2i)comm, (p2i)ierror);
}

void mpif_ear_Barrier_out(void) {
	after_mpi(Barrier);
}

void mpif_ear_Bcast_in(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Bcast, (p2i)buffer, 0);
}

void mpif_ear_Bcast_out(void) {
	after_mpi(Bcast);
}

void mpif_ear_Bsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Bsend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Bsend_out(void) {
	after_mpi(Bsend);
}

void mpif_ear_Bsend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Bsend_init, (p2i)buf, (p2i)dest);
}

void mpif_ear_Bsend_init_out(void) {
	after_mpi(Bsend_init);
}

void mpif_ear_Cancel_in(MPI_Fint *request, MPI_Fint *ierror) {
	before_mpi(Cancel, (p2i)request, 0);
}

void mpif_ear_Cancel_out(void) {
	after_mpi(Cancel);
}

void mpif_ear_Cart_create_in(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) {
before_mpi(Cart_create, (p2i)ndims, 0);
}

void mpif_ear_Cart_create_out(void) {
	after_mpi(Cart_create);
}

void mpif_ear_Cart_sub_in(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) {
before_mpi(Cart_sub, (p2i)remain_dims, 0);
}

void mpif_ear_Cart_sub_out(void) {
	after_mpi(Cart_sub);
}

void mpif_ear_Comm_create_in(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) {
	before_mpi(Comm_create, (p2i)group, 0);
}

void mpif_ear_Comm_create_out(void) {
	after_mpi(Comm_create);
}

void mpif_ear_Comm_dup_in(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) {
	before_mpi(Comm_dup, (p2i)newcomm, 0);
}

void mpif_ear_Comm_dup_out(void) {
	after_mpi(Comm_dup);
}

void mpif_ear_Comm_free_in(MPI_Fint *comm, MPI_Fint *ierror) {
	before_mpi(Comm_free, (p2i)comm, 0);
}

void mpif_ear_Comm_free_out(void) {
	after_mpi(Comm_free);
}

void mpif_ear_Comm_rank_in(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) {
	before_mpi(Comm_rank, (p2i)rank, 0);
}

void mpif_ear_Comm_rank_out(void) {
	after_mpi(Comm_rank);
}

void mpif_ear_Comm_size_in(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) {
	before_mpi(Comm_size, (p2i)size, 0);
}

void mpif_ear_Comm_size_out(void) {
	after_mpi(Comm_size);
}

void mpif_ear_Comm_spawn_in(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
before_mpi(Comm_spawn, (p2i)command, 0);
}

void mpif_ear_Comm_spawn_out(void) {
	after_mpi(Comm_spawn);
}

void mpif_ear_Comm_spawn_multiple_in(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
before_mpi(Comm_spawn_multiple, (p2i)array_of_commands, 0);
}

void mpif_ear_Comm_spawn_multiple_out(void) {
	after_mpi(Comm_spawn_multiple);
}

void mpif_ear_Comm_split_in(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) {
	before_mpi(Comm_split, (p2i)comm, 0);
}

void mpif_ear_Comm_split_out(void) {
	after_mpi(Comm_split);
}

void mpif_ear_File_close_in(MPI_File *fh, MPI_Fint *ierror) {
	before_mpi(File_close, (p2i)fh, 0);
}

void mpif_ear_File_close_out(void) {
	after_mpi(File_close);
}

void mpif_ear_File_read_in(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_read, (p2i)buf, 0);
}

void mpif_ear_File_read_out(void) {
	after_mpi(File_read);
}

void mpif_ear_File_read_all_in(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_read_all, (p2i)buf, 0);
}

void mpif_ear_File_read_all_out(void) {
	after_mpi(File_read_all);
}

void mpif_ear_File_read_at_in(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_read_at, (p2i)buf, 0);
}

void mpif_ear_File_read_at_out(void) {
	after_mpi(File_read_at);
}

void mpif_ear_File_read_at_all_in(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_read_at_all, (p2i)buf, 0);
}

void mpif_ear_File_read_at_all_out(void) {
	after_mpi(File_read_at_all);
}

void mpif_ear_File_write_in(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_write, (p2i)buf, 0);
}

void mpif_ear_File_write_out(void) {
	after_mpi(File_write);
}

void mpif_ear_File_write_all_in(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_write_all, (p2i)buf, 0);
}

void mpif_ear_File_write_all_out(void) {
	after_mpi(File_write_all);
}

void mpif_ear_File_write_at_in(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_write_at, (p2i)buf, 0);
}

void mpif_ear_File_write_at_out(void) {
	after_mpi(File_write_at);
}

void mpif_ear_File_write_at_all_in(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
	before_mpi(File_write_at_all, (p2i)buf, 0);
}

void mpif_ear_File_write_at_all_out(void) {
	after_mpi(File_write_at_all);
}

void mpif_ear_Finalize_in(MPI_Fint *ierror) {
	before_finalize();
}

void mpif_ear_Finalize_out(void) {
	after_finalize();
}

void mpif_ear_Gather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Gather, (p2i)sendbuf, 0);
}

void mpif_ear_Gather_out(void) {
	after_mpi(Gather);
}

void mpif_ear_Gatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Gatherv, (p2i)sendbuf, 0);
}

void mpif_ear_Gatherv_out(void) {
	after_mpi(Gatherv);
}

void mpif_ear_Get_in(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Get, (p2i)origin_addr, 0);
}

void mpif_ear_Get_out(void) {
	after_mpi(Get);
}

void mpif_ear_Ibsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Ibsend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Ibsend_out(void) {
	after_mpi(Ibsend);
}

void mpif_ear_Init_in(MPI_Fint *ierror) {
	before_init();
}

void mpif_ear_Init_out(void) {
	after_init();
}

void mpif_ear_Init_thread_in(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) {
	before_init();
}

void mpif_ear_Init_thread_out(void) {
	after_init();
}

void mpif_ear_Intercomm_create_in(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) {
	before_mpi(Intercomm_create,0 , 0);
}

void mpif_ear_Intercomm_create_out(void) {
	after_mpi(Intercomm_create);
}

void mpif_ear_Intercomm_merge_in(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) {
	before_mpi(Intercomm_merge, 0, 0);
}

void mpif_ear_Intercomm_merge_out(void) {
	after_mpi(Intercomm_merge);
}

void mpif_ear_Iprobe_in(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Iprobe, (p2i)source, 0);
}

void mpif_ear_Iprobe_out(void) {
	after_mpi(Iprobe);
}

void mpif_ear_Irecv_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
	before_mpi(Irecv, (p2i)buf, (p2i)source);
}

void mpif_ear_Irecv_out(void) {
	after_mpi(Irecv);
}

void mpif_ear_Irsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Irsend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Irsend_out(void) {
	after_mpi(Irsend);
}

void mpif_ear_Isend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Isend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Isend_out(void) {
	after_mpi(Isend);
}

void mpif_ear_Issend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Issend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Issend_out(void) {
	after_mpi(Issend);
}

void mpif_ear_Probe_in(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Probe, (p2i)source, 0);
}

void mpif_ear_Probe_out(void) {
	after_mpi(Probe);
}

void mpif_ear_Put_in(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
before_mpi(Put, (p2i)origin_addr, 0);
}

void mpif_ear_Put_out(void) {
	after_mpi(Put);
}

void mpif_ear_Recv_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Recv, (p2i)buf, (p2i)source);
}

void mpif_ear_Recv_out(void) {
	after_mpi(Recv);
}

void mpif_ear_Recv_init_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
	before_mpi(Recv_init, (p2i)buf, (p2i)source);
}

void mpif_ear_Recv_init_out(void) {
	after_mpi(Recv_init);
}

void mpif_ear_Reduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Reduce, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Reduce_out(void) {
	after_mpi(Reduce);
}

void mpif_ear_Reduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Reduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Reduce_scatter_out(void) {
	after_mpi(Reduce_scatter);
}

void mpif_ear_Request_free_in(MPI_Fint *request, MPI_Fint *ierror) {
	before_mpi(Request_free, (p2i)request, 0);
}

void mpif_ear_Request_free_out(void) {
	after_mpi(Request_free);
}

void mpif_ear_Request_get_status_in(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Request_get_status, (p2i)request, 0);
}

void mpif_ear_Request_get_status_out(void) {
	after_mpi(Request_get_status);
}

void mpif_ear_Rsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Rsend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Rsend_out(void) {
	after_mpi(Rsend);
}

void mpif_ear_Rsend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Rsend_init, (p2i)buf, (p2i)dest);
}

void mpif_ear_Rsend_init_out(void) {
	after_mpi(Rsend_init);
}

void mpif_ear_Scan_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Scan, (p2i)sendbuf, 0);
}

void mpif_ear_Scan_out(void) {
	after_mpi(Scan);
}

void mpif_ear_Scatter_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Scatter_out(void) {
	after_mpi(Scatter);
}

void mpif_ear_Scatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Scatterv, (p2i)sendbuf, 0);
}

void mpif_ear_Scatterv_out(void) {
	after_mpi(Scatterv);
}

void mpif_ear_Send_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Send, (p2i)buf, (p2i)dest);
}

void mpif_ear_Send_out(void) {
	after_mpi(Send);
}

void mpif_ear_Send_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Send_init, (p2i)buf, (p2i)dest);
}

void mpif_ear_Send_init_out(void) {
	after_mpi(Send_init);
}

void mpif_ear_Sendrecv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
before_mpi(Sendrecv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Sendrecv_out(void) {
	after_mpi(Sendrecv);
}

void mpif_ear_Sendrecv_replace_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Sendrecv_replace, (p2i)buf, (p2i)dest);
}

void mpif_ear_Sendrecv_replace_out(void) {
	after_mpi(Sendrecv_replace);
}

void mpif_ear_Ssend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
before_mpi(Ssend, (p2i)buf, (p2i)dest);
}

void mpif_ear_Ssend_out(void) {
	after_mpi(Ssend);
}

void mpif_ear_Ssend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
before_mpi(Ssend_init, (p2i)buf, (p2i)dest);
}

void mpif_ear_Ssend_init_out(void) {
	after_mpi(Ssend_init);
}

void mpif_ear_Start_in(MPI_Fint *request, MPI_Fint *ierror) {
	before_mpi(Start, (p2i)request, 0);
}

void mpif_ear_Start_out(void) {
	after_mpi(Start);
}

void mpif_ear_Startall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) {
	before_mpi(Startall, (p2i)count, 0);
}

void mpif_ear_Startall_out(void) {
	after_mpi(Startall);
}

void mpif_ear_Test_in(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Test, (p2i)request, 0);
}

void mpif_ear_Test_out(void) {
	after_mpi(Test);
}

void mpif_ear_Testall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
	before_mpi(Testall, 0, 0);
}

void mpif_ear_Testall_out(void) {
	after_mpi(Testall);
}

void mpif_ear_Testany_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Testany, 0, 0);
}

void mpif_ear_Testany_out(void) {
	after_mpi(Testany);
}

void mpif_ear_Testsome_in(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
	before_mpi(Testsome, 0, 0);
}

void mpif_ear_Testsome_out(void) {
	after_mpi(Testsome);
}

void mpif_ear_Wait_in(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Wait, (p2i)request, 0);
}

void mpif_ear_Wait_out(void) {
	after_mpi(Wait);
}

void mpif_ear_Waitall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
	before_mpi(Waitall, (p2i)count, 0);
}

void mpif_ear_Waitall_out(void) {
	after_mpi(Waitall);
}

void mpif_ear_Waitany_in(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) {
	before_mpi(Waitany, 0, 0);
}

void mpif_ear_Waitany_out(void) {
	after_mpi(Waitany);
}

void mpif_ear_Waitsome_in(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
	before_mpi(Waitsome, 0, 0);
}

void mpif_ear_Waitsome_out(void) {
	after_mpi(Waitsome);
}

void mpif_ear_Win_complete_in(MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_complete, 0, 0);
}

void mpif_ear_Win_complete_out(void) {
	after_mpi(Win_complete);
}

void mpif_ear_Win_create_in(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_create, 0, 0);
}

void mpif_ear_Win_create_out(void) {
	after_mpi(Win_create);
}

void mpif_ear_Win_fence_in(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_fence, 0, 0);
}

void mpif_ear_Win_fence_out(void) {
	after_mpi(Win_fence);
}

void mpif_ear_Win_free_in(MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_free, 0, 0);
}

void mpif_ear_Win_free_out(void) {
	after_mpi(Win_free);
}

void mpif_ear_Win_post_in(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_post, 0, 0);
}

void mpif_ear_Win_post_out(void) {
	after_mpi(Win_post);
}

void mpif_ear_Win_start_in(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_start, 0, 0);
}

void mpif_ear_Win_start_out(void) {
	after_mpi(Win_start);
}

void mpif_ear_Win_wait_in(MPI_Fint *win, MPI_Fint *ierror) {
	before_mpi(Win_wait, 0, 0);
}

void mpif_ear_Win_wait_out(void) {
	after_mpi(Win_wait);
}

#if MPI_VERSION >= 3
void mpif_ear_Iallgather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallgather, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iallgather_out(void) {
    after_mpi(Iallgather);
}

void mpif_ear_Iallgatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallgatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iallgatherv_out(void) {
    after_mpi(Iallgatherv);
}

void mpif_ear_Iallreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iallreduce, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iallreduce_out(void) {
    after_mpi(Iallreduce);
}

void mpif_ear_Ialltoall_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ialltoall, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Ialltoall_out(void) {
    after_mpi(Ialltoall);
}

void mpif_ear_Ialltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) {
    before_mpi(Ialltoallv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Ialltoallv_out(void) {
    after_mpi(Ialltoallv);
}

void mpif_ear_Ibarrier_in(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ibarrier, (p2i)request, 0);
}

void mpif_ear_Ibarrier_out(void) {
    after_mpi(Ibarrier);
}

void mpif_ear_Ibcast_in(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ibcast, (p2i)buffer, 0);
}

void mpif_ear_Ibcast_out(void) {
    after_mpi(Ibcast);
}

void mpif_ear_Igather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Igather, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Igather_out(void) {
    after_mpi(Igather);
}

void mpif_ear_Igatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Igatherv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Igatherv_out(void) {
    after_mpi(Igatherv);
}

void mpif_ear_Ireduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ireduce, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Ireduce_out(void) {
    after_mpi(Ireduce);
}

void mpif_ear_Ireduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Ireduce_scatter, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Ireduce_scatter_out(void) {
    after_mpi(Ireduce_scatter);
}

void mpif_ear_Iscan_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscan, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iscan_out(void) {
    after_mpi(Iscan);
}

void mpif_ear_Iscatter_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscatter, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iscatter_out(void) {
    after_mpi(Iscatter);
}

void mpif_ear_Iscatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
    before_mpi(Iscatterv, (p2i)sendbuf, (p2i)recvbuf);
}

void mpif_ear_Iscatterv_out(void) {
    after_mpi(Iscatterv);
}
#endif