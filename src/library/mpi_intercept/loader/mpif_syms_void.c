#include <library/mpi_intercept/mpif_syms_void.h>

void mpif_void_Allgather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Allgather_out(void) {
}

void mpif_void_Allgatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Allgatherv_out(void) {
}

void mpif_void_Allreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Allreduce_out(void) {
}

void mpif_void_Alltoall_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Alltoall_out(void) {
}

void mpif_void_Alltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Alltoallv_out(void) {
}

void mpif_void_Barrier_in(MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Barrier_out(void) {
}

void mpif_void_Bcast_in(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Bcast_out(void) {
}

void mpif_void_Bsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Bsend_out(void) {
}

void mpif_void_Bsend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Bsend_init_out(void) {
}

void mpif_void_Cancel_in(MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Cancel_out(void) {
}

void mpif_void_Cart_create_in(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) {
}

void mpif_void_Cart_create_out(void) {
}

void mpif_void_Cart_sub_in(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) {
}

void mpif_void_Cart_sub_out(void) {
}

void mpif_void_Comm_create_in(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) {
}

void mpif_void_Comm_create_out(void) {
}

void mpif_void_Comm_dup_in(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) {
}

void mpif_void_Comm_dup_out(void) {
}

void mpif_void_Comm_free_in(MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Comm_free_out(void) {
}

void mpif_void_Comm_rank_in(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) {
}

void mpif_void_Comm_rank_out(void) {
}

void mpif_void_Comm_size_in(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) {
}

void mpif_void_Comm_size_out(void) {
}

void mpif_void_Comm_spawn_in(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
}

void mpif_void_Comm_spawn_out(void) {
}

void mpif_void_Comm_spawn_multiple_in(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) {
}

void mpif_void_Comm_spawn_multiple_out(void) {
}

void mpif_void_Comm_split_in(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) {
}

void mpif_void_Comm_split_out(void) {
}

void mpif_void_File_close_in(MPI_File *fh, MPI_Fint *ierror) {
}

void mpif_void_File_close_out(void) {
}

void mpif_void_File_read_in(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_read_out(void) {
}

void mpif_void_File_read_all_in(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_read_all_out(void) {
}

void mpif_void_File_read_at_in(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_read_at_out(void) {
}

void mpif_void_File_read_at_all_in(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_read_at_all_out(void) {
}

void mpif_void_File_write_in(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_write_out(void) {
}

void mpif_void_File_write_all_in(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_write_all_out(void) {
}

void mpif_void_File_write_at_in(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_write_at_out(void) {
}

void mpif_void_File_write_at_all_in(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) {
}

void mpif_void_File_write_at_all_out(void) {
}

void mpif_void_Finalize_in(MPI_Fint *ierror) {
}

void mpif_void_Finalize_out(void) {
}

void mpif_void_Gather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Gather_out(void) {
}

void mpif_void_Gatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Gatherv_out(void) {
}

void mpif_void_Get_in(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Get_out(void) {
}

void mpif_void_Ibsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ibsend_out(void) {
}

void mpif_void_Init_in(MPI_Fint *ierror) {
}

void mpif_void_Init_out(void) {
}

void mpif_void_Init_thread_in(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) {
}

void mpif_void_Init_thread_out(void) {
}

void mpif_void_Intercomm_create_in(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) {
}

void mpif_void_Intercomm_create_out(void) {
}

void mpif_void_Intercomm_merge_in(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) {
}

void mpif_void_Intercomm_merge_out(void) {
}

void mpif_void_Iprobe_in(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Iprobe_out(void) {
}

void mpif_void_Irecv_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Irecv_out(void) {
}

void mpif_void_Irsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Irsend_out(void) {
}

void mpif_void_Isend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Isend_out(void) {
}

void mpif_void_Issend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Issend_out(void) {
}

void mpif_void_Probe_in(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Probe_out(void) {
}

void mpif_void_Put_in(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Put_out(void) {
}

void mpif_void_Recv_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Recv_out(void) {
}

void mpif_void_Recv_init_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Recv_init_out(void) {
}

void mpif_void_Reduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Reduce_out(void) {
}

void mpif_void_Reduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Reduce_scatter_out(void) {
}

void mpif_void_Request_free_in(MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Request_free_out(void) {
}

void mpif_void_Request_get_status_in(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Request_get_status_out(void) {
}

void mpif_void_Rsend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Rsend_out(void) {
}

void mpif_void_Rsend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Rsend_init_out(void) {
}

void mpif_void_Scan_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Scan_out(void) {
}

void mpif_void_Scatter_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Scatter_out(void) {
}

void mpif_void_Scatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Scatterv_out(void) {
}

void mpif_void_Send_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Send_out(void) {
}

void mpif_void_Send_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Send_init_out(void) {
}

void mpif_void_Sendrecv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Sendrecv_out(void) {
}

void mpif_void_Sendrecv_replace_in(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Sendrecv_replace_out(void) {
}

void mpif_void_Ssend_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Ssend_out(void) {
}

void mpif_void_Ssend_init_in(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ssend_init_out(void) {
}

void mpif_void_Start_in(MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Start_out(void) {
}

void mpif_void_Startall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) {
}

void mpif_void_Startall_out(void) {
}

void mpif_void_Test_in(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Test_out(void) {
}

void mpif_void_Testall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
}

void mpif_void_Testall_out(void) {
}

void mpif_void_Testany_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Testany_out(void) {
}

void mpif_void_Testsome_in(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
}

void mpif_void_Testsome_out(void) {
}

void mpif_void_Wait_in(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Wait_out(void) {
}

void mpif_void_Waitall_in(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
}

void mpif_void_Waitall_out(void) {
}

void mpif_void_Waitany_in(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) {
}

void mpif_void_Waitany_out(void) {
}

void mpif_void_Waitsome_in(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) {
}

void mpif_void_Waitsome_out(void) {
}

void mpif_void_Win_complete_in(MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_complete_out(void) {
}

void mpif_void_Win_create_in(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_create_out(void) {
}

void mpif_void_Win_fence_in(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_fence_out(void) {
}

void mpif_void_Win_free_in(MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_free_out(void) {
}

void mpif_void_Win_post_in(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_post_out(void) {
}

void mpif_void_Win_start_in(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_start_out(void) {
}

void mpif_void_Win_wait_in(MPI_Fint *win, MPI_Fint *ierror) {
}

void mpif_void_Win_wait_out(void) {
}

#if MPI_VERSION >= 3
void mpif_void_Iallgather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iallgather_out(void) {
}

void mpif_void_Iallgatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iallgatherv_out(void) {
}

void mpif_void_Iallreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iallreduce_out(void) {
}

void mpif_void_Ialltoall_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ialltoall_out(void) {
}

void mpif_void_Ialltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) {
}

void mpif_void_Ialltoallv_out(void) {
}

void mpif_void_Ibarrier_in(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ibarrier_out(void) {
}

void mpif_void_Ibcast_in(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ibcast_out(void) {
}

void mpif_void_Igather_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Igather_out(void) {
}

void mpif_void_Igatherv_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Igatherv_out(void) {
}

void mpif_void_Ireduce_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ireduce_out(void) {
}

void mpif_void_Ireduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Ireduce_scatter_out(void) {
}

void mpif_void_Iscan_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iscan_out(void) {
}

void mpif_void_Iscatter_in(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iscatter_out(void) {
}

void mpif_void_Iscatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) {
}

void mpif_void_Iscatterv_out(void) {
}
#endif