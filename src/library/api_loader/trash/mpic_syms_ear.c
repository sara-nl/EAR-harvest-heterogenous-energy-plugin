#include <library/mpi_intercept/mpic_syms_ear.h>

void mpic_ear_Allgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
	before_mpi(Allgather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allgather_out(void) {
	after_mpi(Allgather);
}

void mpic_ear_Allgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
	before_mpi(Allgatherv,(p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allgatherv_out(void) {
	after_mpi(Allgatherv);
}

void mpic_ear_Allreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
	before_mpi(Allreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Allreduce_out(void) {
	after_mpi(Allreduce);
}

void mpic_ear_Alltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
	before_mpi(Alltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Alltoall_out(void) {
	after_mpi(Alltoall);
}

void mpic_ear_Alltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
	before_mpi(Alltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Alltoallv_out(void) {
	after_mpi(Alltoallv);
}

void mpic_ear_Barrier_in(MPI_Comm comm) {
	before_mpi(Barrier, (p2i)comm,0);
}

void mpic_ear_Barrier_out(void) {
	after_mpi(Barrier);
}

void mpic_ear_Bcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	before_mpi(Bcast, (p2i)comm,0);
}

void mpic_ear_Bcast_out(void) {
	after_mpi(Bcast);
}

void mpic_ear_Bsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
	before_mpi(Bsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Bsend_out(void) {
	after_mpi(Bsend);
}

void mpic_ear_Bsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Bsend_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Bsend_init_out(void) {
	after_mpi(Bsend_init);
}

void mpic_ear_Cancel_in(MPI_Request *request) {
	before_mpi(Cancel, (p2i)request,(p2i)0);
}

void mpic_ear_Cancel_out(void) {
	after_mpi(Cancel);
}

void mpic_ear_Cart_create_in(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart) {
	before_mpi(Cart_create, (p2i)ndims,(p2i)comm_cart);

}

void mpic_ear_Cart_create_out(void) {
	after_mpi(Cart_create);
}

void mpic_ear_Cart_sub_in(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm) {
	before_mpi(Cart_sub, (p2i)remain_dims,(p2i)newcomm);
}

void mpic_ear_Cart_sub_out(void) {
	after_mpi(Cart_sub);
}

void mpic_ear_Comm_create_in(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) {
	before_mpi(Comm_create, (p2i)group,(p2i)newcomm);
}

void mpic_ear_Comm_create_out(void) {
	after_mpi(Comm_create);
}

void mpic_ear_Comm_dup_in(MPI_Comm comm, MPI_Comm *newcomm) {
	before_mpi(Comm_dup, (p2i)newcomm,(p2i)0);
}

void mpic_ear_Comm_dup_out(void) {
	after_mpi(Comm_dup);
}

void mpic_ear_Comm_free_in(MPI_Comm *comm) {
	before_mpi(Comm_free, (p2i)comm,(p2i)0);
}

void mpic_ear_Comm_free_out(void) {
	after_mpi(Comm_free);
}

void mpic_ear_Comm_rank_in(MPI_Comm comm, int *rank) {
	before_mpi(Comm_rank, (p2i)comm,(p2i)rank);
}

void mpic_ear_Comm_rank_out(void) {
	after_mpi(Comm_rank);
}

void mpic_ear_Comm_size_in(MPI_Comm comm, int *size) {
	before_mpi(Comm_size, (p2i)size,(p2i)0);
}

void mpic_ear_Comm_size_out(void) {
	after_mpi(Comm_size);
}

void mpic_ear_Comm_spawn_in(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
	before_mpi(Comm_spawn, (p2i)command,(p2i)info);
}

void mpic_ear_Comm_spawn_out(void) {
	after_mpi(Comm_spawn);
}

void mpic_ear_Comm_spawn_multiple_in(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) {
	before_mpi(Comm_spawn_multiple, (p2i)array_of_commands,(p2i)array_of_info);
}

void mpic_ear_Comm_spawn_multiple_out(void) {
	after_mpi(Comm_spawn_multiple);
}

void mpic_ear_Comm_split_in(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) {
	before_mpi(Comm_split, (p2i)key,(p2i)newcomm);
}

void mpic_ear_Comm_split_out(void) {
	after_mpi(Comm_split);
}

void mpic_ear_File_close_in(MPI_File *fh) {
	before_mpi(File_close, (p2i)fh,(p2i)0);
}

void mpic_ear_File_close_out(void) {
	after_mpi(File_close);
}

void mpic_ear_File_read_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_read, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_out(void) {
	after_mpi(File_read);
}

void mpic_ear_File_read_all_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_read_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_all_out(void) {
	after_mpi(File_read_all);
}

void mpic_ear_File_read_at_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_read_at, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_at_out(void) {
	after_mpi(File_read_at);
}

void mpic_ear_File_read_at_all_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_read_at_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_read_at_all_out(void) {
	after_mpi(File_read_at_all);
}

void mpic_ear_File_write_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_write, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_out(void) {
	after_mpi(File_write);
}

void mpic_ear_File_write_all_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_write_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_all_out(void) {
	after_mpi(File_write_all);
}

void mpic_ear_File_write_at_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_write_at, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_at_out(void) {
	after_mpi(File_write_at);
}

void mpic_ear_File_write_at_all_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status) {
	before_mpi(File_write_at_all, (p2i)buf,(p2i)datatype);
}

void mpic_ear_File_write_at_all_out(void) {
	after_mpi(File_write_at_all);
}

void mpic_ear_Finalize_in(void) {
	before_finalize();
}

void mpic_ear_Finalize_out(void) {
	after_finalize();
}

void mpic_ear_Gather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	before_mpi(Gather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Gather_out(void) {
	after_mpi(Gather);
}

void mpic_ear_Gatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	before_mpi(Gatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Gatherv_out(void) {
	after_mpi(Gatherv);
}

void mpic_ear_Get_in(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
	before_mpi(Get, (p2i)origin_addr,(p2i)origin_datatype);
}

void mpic_ear_Get_out(void) {
	after_mpi(Get);
}

void mpic_ear_Ibsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Ibsend, (p2i)buf,(p2i)datatype);
}

void mpic_ear_Ibsend_out(void) {
	after_mpi(Ibsend);
}

void mpic_ear_Init_in(int *argc, char ***argv) {
	before_init();
}

void mpic_ear_Init_out(void) {
	after_init();
}

void mpic_ear_Init_thread_in(int *argc, char ***argv, int required, int *provided) {
	before_init();
}

void mpic_ear_Init_thread_out(void) {
	after_init();
}

void mpic_ear_Intercomm_create_in(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) {
	before_mpi(Intercomm_create, (p2i)local_leader,(p2i)remote_leader);
}

void mpic_ear_Intercomm_create_out(void) {
	after_mpi(Intercomm_create);
}

void mpic_ear_Intercomm_merge_in(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) {
	before_mpi(Intercomm_merge, (p2i)newintracomm,(p2i)0);
}

void mpic_ear_Intercomm_merge_out(void) {
	after_mpi(Intercomm_merge);
}

void mpic_ear_Iprobe_in(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) {
	before_mpi(Iprobe, (p2i)flag,(p2i)status);
}

void mpic_ear_Iprobe_out(void) {
	after_mpi(Iprobe);
}

void mpic_ear_Irecv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Irecv, (p2i)buf,(p2i)request);
}

void mpic_ear_Irecv_out(void) {
	after_mpi(Irecv);
}

void mpic_ear_Irsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Irsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Irsend_out(void) {
	after_mpi(Irsend);
}

void mpic_ear_Isend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Isend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Isend_out(void) {
	after_mpi(Isend);
}

void mpic_ear_Issend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Issend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Issend_out(void) {
	after_mpi(Issend);
}

void mpic_ear_Probe_in(int source, int tag, MPI_Comm comm, MPI_Status *status) {
	before_mpi(Probe, (p2i)source,(p2i)0);
}

void mpic_ear_Probe_out(void) {
	after_mpi(Probe);
}

void mpic_ear_Put_in(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) {
	before_mpi(Put, (p2i)origin_addr,(p2i)0);
}

void mpic_ear_Put_out(void) {
	after_mpi(Put);
}

void mpic_ear_Recv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
	before_mpi(Recv, (p2i)buf,(p2i)source);
}

void mpic_ear_Recv_out(void) {
	after_mpi(Recv);
}

void mpic_ear_Recv_init_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Recv_init, (p2i)buf,(p2i)source);
}

void mpic_ear_Recv_init_out(void) {
	after_mpi(Recv_init);
}

void mpic_ear_Reduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
	before_mpi(Reduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Reduce_out(void) {
	after_mpi(Reduce);
}

void mpic_ear_Reduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
	before_mpi(Reduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Reduce_scatter_out(void) {
	after_mpi(Reduce_scatter);
}

void mpic_ear_Request_free_in(MPI_Request *request) {
	before_mpi(Request_free, (p2i)request,(p2i)0);
}

void mpic_ear_Request_free_out(void) {
	after_mpi(Request_free);
}

void mpic_ear_Request_get_status_in(MPI_Request request, int *flag, MPI_Status *status) {
	before_mpi(Request_get_status, (p2i)request,(p2i)0);
}

void mpic_ear_Request_get_status_out(void) {
	after_mpi(Request_get_status);
}

void mpic_ear_Rsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
	before_mpi(Rsend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Rsend_out(void) {
	after_mpi(Rsend);
}

void mpic_ear_Rsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Rsend_init, (p2i)buf,(p2i)0);
}

void mpic_ear_Rsend_init_out(void) {
	after_mpi(Rsend_init);
}

void mpic_ear_Scan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
	before_mpi(Scan, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scan_out(void) {
	after_mpi(Scan);
}

void mpic_ear_Scatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	before_mpi(Scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scatter_out(void) {
	after_mpi(Scatter);
}

void mpic_ear_Scatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	before_mpi(Scatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Scatterv_out(void) {
	after_mpi(Scatterv);
}

void mpic_ear_Send_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
	before_mpi(Send, (p2i)buf,(p2i)dest);
}

void mpic_ear_Send_out(void) {
	after_mpi(Send);
}

void mpic_ear_Send_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Send_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Send_init_out(void) {
	after_mpi(Send_init);
}

void mpic_ear_Sendrecv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
	before_mpi(Sendrecv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Sendrecv_out(void) {
	after_mpi(Sendrecv);
}

void mpic_ear_Sendrecv_replace_in(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) {
	before_mpi(Sendrecv_replace, (p2i)buf,(p2i)dest);
}

void mpic_ear_Sendrecv_replace_out(void) {
	after_mpi(Sendrecv_replace);
}

void mpic_ear_Ssend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
	before_mpi(Ssend, (p2i)buf,(p2i)dest);
}

void mpic_ear_Ssend_out(void) {
	after_mpi(Ssend);
}

void mpic_ear_Ssend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
	before_mpi(Ssend_init, (p2i)buf,(p2i)dest);
}

void mpic_ear_Ssend_init_out(void) {
	after_mpi(Ssend_init);
}

void mpic_ear_Start_in(MPI_Request *request) {
	before_mpi(Start, (p2i)request,(p2i)0);
}

void mpic_ear_Start_out(void) {
	after_mpi(Start);
}

void mpic_ear_Startall_in(int count, MPI_Request array_of_requests[]) {
	before_mpi(Startall, (p2i)array_of_requests,(p2i)0);
}

void mpic_ear_Startall_out(void) {
	after_mpi(Startall);
}

void mpic_ear_Test_in(MPI_Request *request, int *flag, MPI_Status *status) {
	before_mpi(Test, (p2i)request,(p2i)0);
}

void mpic_ear_Test_out(void) {
	after_mpi(Test);
}

void mpic_ear_Testall_in(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) {
	before_mpi(Testall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void mpic_ear_Testall_out(void) {
	after_mpi(Testall);
}

void mpic_ear_Testany_in(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) {
	before_mpi(Testany, (p2i)array_of_requests,(p2i)flag);
}

void mpic_ear_Testany_out(void) {
	after_mpi(Testany);
}

void mpic_ear_Testsome_in(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) {
	before_mpi(Testsome, (p2i)array_of_requests,(p2i)outcount);
}

void mpic_ear_Testsome_out(void) {
	after_mpi(Testsome);
}

void mpic_ear_Wait_in(MPI_Request *request, MPI_Status *status) {
	before_mpi(Wait, (p2i)request,(p2i)0);
}

void mpic_ear_Wait_out(void) {
	after_mpi(Wait);
}

void mpic_ear_Waitall_in(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses) {
	before_mpi(Waitall, (p2i)array_of_requests,(p2i)array_of_statuses);
}

void mpic_ear_Waitall_out(void) {
	after_mpi(Waitall);
}

void mpic_ear_Waitany_in(int count, MPI_Request *requests, int *index, MPI_Status *status) {
	before_mpi(Waitany, (p2i)requests,(p2i)index);
}

void mpic_ear_Waitany_out(void) {
	after_mpi(Waitany);
}

void mpic_ear_Waitsome_in(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses) {
	before_mpi(Waitsome, (p2i)array_of_requests,(p2i)outcount);
}

void mpic_ear_Waitsome_out(void) {
	after_mpi(Waitsome);
}

void mpic_ear_Win_complete_in(MPI_Win win) {
	before_mpi(Win_complete, (p2i)win,(p2i)0);
}

void mpic_ear_Win_complete_out(void) {
	after_mpi(Win_complete);
}

void mpic_ear_Win_create_in(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) {
	before_mpi(Win_create, (p2i)base,(p2i)info);
}

void mpic_ear_Win_create_out(void) {
	after_mpi(Win_create);
}

void mpic_ear_Win_fence_in(int assert, MPI_Win win) {
	before_mpi(Win_fence, (p2i)win,(p2i)0);
}

void mpic_ear_Win_fence_out(void) {
	after_mpi(Win_fence);
}

void mpic_ear_Win_free_in(MPI_Win *win) {
	before_mpi(Win_free, (p2i)win,(p2i)0);
}

void mpic_ear_Win_free_out(void) {
	after_mpi(Win_free);
}

void mpic_ear_Win_post_in(MPI_Group group, int assert, MPI_Win win) {
	before_mpi(Win_post, (p2i)win,(p2i)0);
}

void mpic_ear_Win_post_out(void) {
	after_mpi(Win_post);
}

void mpic_ear_Win_start_in(MPI_Group group, int assert, MPI_Win win) {
	before_mpi(Win_start, (p2i)win,(p2i)0);
}

void mpic_ear_Win_start_out(void) {
	after_mpi(Win_start);
}

void mpic_ear_Win_wait_in(MPI_Win win) {
	before_mpi(Win_wait, (p2i)win,(p2i)0);
}

void mpic_ear_Win_wait_out(void) {
	after_mpi(Win_wait);
}

#if MPI_VERSION >= 3
void mpic_ear_Iallgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallgather_out(void) {
    after_mpi(Iallgather);
}

void mpic_ear_Iallgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallgatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallgatherv_out(void) {
    after_mpi(Iallgatherv);
}

void mpic_ear_Iallreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iallreduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iallreduce_out(void) {
    after_mpi(Iallreduce);
}

void mpic_ear_Ialltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoall, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ialltoall_out(void) {
    after_mpi(Ialltoall);
}

void mpic_ear_Ialltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ialltoallv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ialltoallv_out(void) {
    after_mpi(Ialltoallv);
}

void mpic_ear_Ibarrier_in(MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibarrier, (p2i)request,(p2i)0);
}

void mpic_ear_Ibarrier_out(void) {
    after_mpi(Ibarrier);
}

void mpic_ear_Ibcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ibcast, (p2i)buffer,(p2i)request);
}

void mpic_ear_Ibcast_out(void) {
    after_mpi(Ibcast);
}

void mpic_ear_Igather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igather, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Igather_out(void) {
    after_mpi(Igather);
}

void mpic_ear_Igatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Igatherv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Igatherv_out(void) {
    after_mpi(Igatherv);
}

void mpic_ear_Ireduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ireduce_out(void) {
    after_mpi(Ireduce);
}

void mpic_ear_Ireduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Ireduce_scatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Ireduce_scatter_out(void) {
    after_mpi(Ireduce_scatter);
}

void mpic_ear_Iscan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscan, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscan_out(void) {
    after_mpi(Iscan);
}

void mpic_ear_Iscatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatter, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscatter_out(void) {
    after_mpi(Iscatter);
}

void mpic_ear_Iscatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) {
    before_mpi(Iscatterv, (p2i)sendbuf,(p2i)recvbuf);
}

void mpic_ear_Iscatterv_out(void) {
    after_mpi(Iscatterv);
}
#endif