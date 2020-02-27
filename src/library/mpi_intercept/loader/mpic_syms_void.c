#include <library/mpi_intercept/mpic_syms_void.h>

void mpic_void_Allgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
}

void mpic_void_Allgather_out(void) {
}

void mpic_void_Allgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
}

void mpic_void_Allgatherv_out(void) {
}

void mpic_void_Allreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
}

void mpic_void_Allreduce_out(void) {
}

void mpic_void_Alltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
}

void mpic_void_Alltoall_out(void) {
}

void mpic_void_Alltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
}

void mpic_void_Alltoallv_out(void) {
}

void mpic_void_Barrier_in(MPI_Comm comm) {
}

void mpic_void_Barrier_out(void) {
}

void mpic_void_Bcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
}

void mpic_void_Bcast_out(void) {
}

void mpic_void_Bsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
}

void mpic_void_Bsend_out(void) {
}

void mpic_void_Bsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Bsend_init_out(void) {
}

void mpic_void_Cancel_in(MPI_Request *request) {
}

void mpic_void_Cancel_out(void) {
}

void mpic_void_Cart_create_in(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {

}

void mpic_void_Cart_create_out(void) {
}

void mpic_void_Cart_sub_in(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
}

void mpic_void_Cart_sub_out(void) {
}

void mpic_void_Comm_create_in(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
}

void mpic_void_Comm_create_out(void) {
}

void mpic_void_Comm_dup_in(MPI_Comm comm, MPI_Comm *newcomm) {
}

void mpic_void_Comm_dup_out(void) {
}

void mpic_void_Comm_free_in(MPI_Comm *comm) {
}

void mpic_void_Comm_free_out(void) {
}

void mpic_void_Comm_rank_in(MPI_Comm comm, int *rank) {
}

void mpic_void_Comm_rank_out(void) {
}

void mpic_void_Comm_size_in(MPI_Comm comm, int *size) {
}

void mpic_void_Comm_size_out(void) {
}

void mpic_void_Comm_spawn_in(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
}

void mpic_void_Comm_spawn_out(void) {
}

void mpic_void_Comm_spawn_multiple_in(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
}

void mpic_void_Comm_spawn_multiple_out(void) {
}

void mpic_void_Comm_split_in(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
}

void mpic_void_Comm_split_out(void) {
}

void mpic_void_File_close_in(MPI_File *fh) {
}

void mpic_void_File_close_out(void) {
}

void mpic_void_File_read_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_read_out(void) {
}

void mpic_void_File_read_all_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_read_all_out(void) {
}

void mpic_void_File_read_at_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_read_at_out(void) {
}

void mpic_void_File_read_at_all_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_read_at_all_out(void) {
}

void mpic_void_File_write_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_write_out(void) {
}

void mpic_void_File_write_all_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_write_all_out(void) {
}

void mpic_void_File_write_at_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_write_at_out(void) {
}

void mpic_void_File_write_at_all_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
}

void mpic_void_File_write_at_all_out(void) {
}

void mpic_void_Finalize_in(void) {
}

void mpic_void_Finalize_out(void) {
}

void mpic_void_Gather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
}

void mpic_void_Gather_out(void) {
}

void mpic_void_Gatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
}

void mpic_void_Gatherv_out(void) {
}

void mpic_void_Get_in(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
}

void mpic_void_Get_out(void) {
}

void mpic_void_Ibsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ibsend_out(void) {
}

void mpic_void_Init_in(int *argc, char ***argv) {
}

void mpic_void_Init_out(void) {
}

void mpic_void_Init_thread_in(int *argc, char ***argv, int required, int *provided) {
}

void mpic_void_Init_thread_out(void) {
}

void mpic_void_Intercomm_create_in(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
}

void mpic_void_Intercomm_create_out(void) {
}

void mpic_void_Intercomm_merge_in(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
}

void mpic_void_Intercomm_merge_out(void) {
}

void mpic_void_Iprobe_in(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
}

void mpic_void_Iprobe_out(void) {
}

void mpic_void_Irecv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Irecv_out(void) {
}

void mpic_void_Irsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Irsend_out(void) {
}

void mpic_void_Isend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Isend_out(void) {
}

void mpic_void_Issend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Issend_out(void) {
}

void mpic_void_Probe_in(int source, int tag, MPI_Comm comm, MPI_Status *status) {
}

void mpic_void_Probe_out(void) {
}

void mpic_void_Put_in(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
}

void mpic_void_Put_out(void) {
}

void mpic_void_Recv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
}

void mpic_void_Recv_out(void) {
}

void mpic_void_Recv_init_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Recv_init_out(void) {
}

void mpic_void_Reduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
}

void mpic_void_Reduce_out(void) {
}

void mpic_void_Reduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
}

void mpic_void_Reduce_scatter_out(void) {
}

void mpic_void_Request_free_in(MPI_Request *request) {
}

void mpic_void_Request_free_out(void) {
}

void mpic_void_Request_get_status_in(MPI_Request request, int *flag, MPI_Status *status) {
}

void mpic_void_Request_get_status_out(void) {
}

void mpic_void_Rsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
}

void mpic_void_Rsend_out(void) {
}

void mpic_void_Rsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Rsend_init_out(void) {
}

void mpic_void_Scan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
}

void mpic_void_Scan_out(void) {
}

void mpic_void_Scatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
}

void mpic_void_Scatter_out(void) {
}

void mpic_void_Scatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
}

void mpic_void_Scatterv_out(void) {
}

void mpic_void_Send_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
}

void mpic_void_Send_out(void) {
}

void mpic_void_Send_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Send_init_out(void) {
}

void mpic_void_Sendrecv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
}

void mpic_void_Sendrecv_out(void) {
}

void mpic_void_Sendrecv_replace_in(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
}

void mpic_void_Sendrecv_replace_out(void) {
}

void mpic_void_Ssend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
}

void mpic_void_Ssend_out(void) {
}

void mpic_void_Ssend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ssend_init_out(void) {
}

void mpic_void_Start_in(MPI_Request *request) {
}

void mpic_void_Start_out(void) {
}

void mpic_void_Startall_in(int count, MPI_Request array_of_requests[]) {
}

void mpic_void_Startall_out(void) {
}

void mpic_void_Test_in(MPI_Request *request, int *flag, MPI_Status *status) {
}

void mpic_void_Test_out(void) {
}

void mpic_void_Testall_in(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
}

void mpic_void_Testall_out(void) {
}

void mpic_void_Testany_in(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
}

void mpic_void_Testany_out(void) {
}

void mpic_void_Testsome_in(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
}

void mpic_void_Testsome_out(void) {
}

void mpic_void_Wait_in(MPI_Request *request, MPI_Status *status) {
}

void mpic_void_Wait_out(void) {
}

void mpic_void_Waitall_in(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
}

void mpic_void_Waitall_out(void) {
}

void mpic_void_Waitany_in(int count, MPI_Request *requests, int *index, MPI_Status *status) {
}

void mpic_void_Waitany_out(void) {
}

void mpic_void_Waitsome_in(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
}

void mpic_void_Waitsome_out(void) {
}

void mpic_void_Win_complete_in(MPI_Win win) {
}

void mpic_void_Win_complete_out(void) {
}

void mpic_void_Win_create_in(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
}

void mpic_void_Win_create_out(void) {
}

void mpic_void_Win_fence_in(int assert, MPI_Win win) {
}

void mpic_void_Win_fence_out(void) {
}

void mpic_void_Win_free_in(MPI_Win *win) {
}

void mpic_void_Win_free_out(void) {
}

void mpic_void_Win_post_in(MPI_Group group, int assert, MPI_Win win) {
}

void mpic_void_Win_post_out(void) {
}

void mpic_void_Win_start_in(MPI_Group group, int assert, MPI_Win win) {
}

void mpic_void_Win_start_out(void) {
}

void mpic_void_Win_wait_in(MPI_Win win) {
}

void mpic_void_Win_wait_out(void) {
}

#if MPI_VERSION >= 3
void mpic_void_Iallgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iallgather_out(void) {
}

void mpic_void_Iallgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iallgatherv_out(void) {
}

void mpic_void_Iallreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iallreduce_out(void) {
}

void mpic_void_Ialltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ialltoall_out(void) {
}

void mpic_void_Ialltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ialltoallv_out(void) {
}

void mpic_void_Ibarrier_in(MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ibarrier_out(void) {
}

void mpic_void_Ibcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ibcast_out(void) {
}

void mpic_void_Igather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Igather_out(void) {
}

void mpic_void_Igatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Igatherv_out(void) {
}

void mpic_void_Ireduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ireduce_out(void) {
}

void mpic_void_Ireduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Ireduce_scatter_out(void) {
}

void mpic_void_Iscan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iscan_out(void) {
}

void mpic_void_Iscatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iscatter_out(void) {
}

void mpic_void_Iscatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
}

void mpic_void_Iscatterv_out(void) {
}
#endif