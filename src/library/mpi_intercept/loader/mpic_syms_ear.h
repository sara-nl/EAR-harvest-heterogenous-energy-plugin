#ifndef _LOADER_C_EARS_H_
#define _LOADER_C_EARS_H_

#include <library/mpi_intercept/mpi.h>

typedef struct mpic_ear_s
{
	void (*Allgather_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Allgather_out) (void);
	void (*Allgatherv_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Allgatherv_out) (void);
	void (*Allreduce_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Allreduce_out) (void);
	void (*Alltoall_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Alltoall_out) (void);
	void (*Alltoallv_in) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
	void (*Alltoallv_out) (void);
	void (*Barrier_in) (MPI_Comm comm);
	void (*Barrier_out) (void);
	void (*Bcast_in) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
	void (*Bcast_out) (void);
	void (*Bsend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Bsend_out) (void);
	void (*Bsend_init_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Bsend_init_out) (void);
	void (*Cancel_in) (MPI_Request *request);
	void (*Cancel_out) (void);
	void (*Cart_create_in) (MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
	void (*Cart_create_out) (void);
	void (*Cart_sub_in) (MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
	void (*Cart_sub_out) (void);
	void (*Comm_create_in) (MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
	void (*Comm_create_out) (void);
	void (*Comm_dup_in) (MPI_Comm comm, MPI_Comm *newcomm);
	void (*Comm_dup_out) (void);
	void (*Comm_free_in) (MPI_Comm *comm);
	void (*Comm_free_out) (void);
	void (*Comm_rank_in) (MPI_Comm comm, int *rank);
	void (*Comm_rank_out) (void);
	void (*Comm_size_in) (MPI_Comm comm, int *size);
	void (*Comm_size_out) (void);
	void (*Comm_spawn_in) (MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	void (*Comm_spawn_out) (void);
	void (*Comm_spawn_multiple_in) (int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	void (*Comm_spawn_multiple_out) (void);
	void (*Comm_split_in) (MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
	void (*Comm_split_out) (void);
	void (*File_close_in) (MPI_File *fh);
	void (*File_close_out) (void);
	void (*File_read_in) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_out) (void);
	void (*File_read_all_in) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_all_out) (void);
	void (*File_read_at_in) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_at_out) (void);
	void (*File_read_at_all_in) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_read_at_all_out) (void);
	void (*File_write_in) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_out) (void);
	void (*File_write_all_in) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_all_out) (void);
	void (*File_write_at_in) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_at_out) (void);
	void (*File_write_at_all_in) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	void (*File_write_at_all_out) (void);
	void (*Finalize_in) (void);
	void (*Finalize_out) (void);
	void (*Gather_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Gather_out) (void);
	void (*Gatherv_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Gatherv_out) (void);
	void (*Get_in) (void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	void (*Get_out) (void);
	void (*Ibsend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Ibsend_out) (void);
	void (*Init_in) (int *argc, char ***argv);
	void (*Init_out) (void);
	void (*Init_thread_in) (int *argc, char ***argv, int required, int *provided);
	void (*Init_thread_out) (void);
	void (*Intercomm_create_in) (MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
	void (*Intercomm_create_out) (void);
	void (*Intercomm_merge_in) (MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
	void (*Intercomm_merge_out) (void);
	void (*Iprobe_in) (int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
	void (*Iprobe_out) (void);
	void (*Irecv_in) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Irecv_out) (void);
	void (*Irsend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Irsend_out) (void);
	void (*Isend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Isend_out) (void);
	void (*Issend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Issend_out) (void);
	void (*Probe_in) (int source, int tag, MPI_Comm comm, MPI_Status *status);
	void (*Probe_out) (void);
	void (*Put_in) (MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	void (*Put_out) (void);
	void (*Recv_in) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
	void (*Recv_out) (void);
	void (*Recv_init_in) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Recv_init_out) (void);
	void (*Reduce_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
	void (*Reduce_out) (void);
	void (*Reduce_scatter_in) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Reduce_scatter_out) (void);
	void (*Request_free_in) (MPI_Request *request);
	void (*Request_free_out) (void);
	void (*Request_get_status_in) (MPI_Request request, int *flag, MPI_Status *status);
	void (*Request_get_status_out) (void);
	void (*Rsend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Rsend_out) (void);
	void (*Rsend_init_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Rsend_init_out) (void);
	void (*Scan_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	void (*Scan_out) (void);
	void (*Scatter_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Scatter_out) (void);
	void (*Scatterv_in) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	void (*Scatterv_out) (void);
	void (*Send_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Send_out) (void);
	void (*Send_init_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Send_init_out) (void);
	void (*Sendrecv_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	void (*Sendrecv_out) (void);
	void (*Sendrecv_replace_in) (void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	void (*Sendrecv_replace_out) (void);
	void (*Ssend_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	void (*Ssend_out) (void);
	void (*Ssend_init_in) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	void (*Ssend_init_out) (void);
	void (*Start_in) (MPI_Request *request);
	void (*Start_out) (void);
	void (*Startall_in) (int count, MPI_Request array_of_requests[]);
	void (*Startall_out) (void);
	void (*Test_in) (MPI_Request *request, int *flag, MPI_Status *status);
	void (*Test_out) (void);
	void (*Testall_in) (int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
	void (*Testall_out) (void);
	void (*Testany_in) (int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
	void (*Testany_out) (void);
	void (*Testsome_in) (int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
	void (*Testsome_out) (void);
	void (*Wait_in) (MPI_Request *request, MPI_Status *status);
	void (*Wait_out) (void);
	void (*Waitall_in) (int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
	void (*Waitall_out) (void);
	void (*Waitany_in) (int count, MPI_Request *requests, int *index, MPI_Status *status);
	void (*Waitany_out) (void);
	void (*Waitsome_in) (int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
	void (*Waitsome_out) (void);
	void (*Win_complete_in) (MPI_Win win);
	void (*Win_complete_out) (void);
	void (*Win_create_in) (void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
	void (*Win_create_out) (void);
	void (*Win_fence_in) (int assert, MPI_Win win);
	void (*Win_fence_out) (void);
	void (*Win_free_in) (MPI_Win *win);
	void (*Win_free_out) (void);
	void (*Win_post_in) (MPI_Group group, int assert, MPI_Win win);
	void (*Win_post_out) (void);
	void (*Win_start_in) (MPI_Group group, int assert, MPI_Win win);
	void (*Win_start_out) (void);
	void (*Win_wait_in) (MPI_Win win);
	void (*Win_wait_out) (void);
	#if MPI_VERSION >= 3
	void (*Iallgather_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Iallgather_out) (void);
	void (*Iallgatherv_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Iallgatherv_out) (void);
	void (*Iallreduce_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Iallreduce_out) (void);
	void (*Ialltoall_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Ialltoall_out) (void);
	void (*Ialltoallv_in) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	void (*Ialltoallv_out) (void);
	void (*Ibarrier_in) (MPI_Comm comm, MPI_Request *request);
	void (*Ibarrier_out) (void);
	void (*Ibcast_in) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Ibcast_out) (void);
	void (*Igather_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Igather_out) (void);
	void (*Igatherv_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Igatherv_out) (void);
	void (*Ireduce_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
	void (*Ireduce_out) (void);
	void (*Ireduce_scatter_in) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Ireduce_scatter_out) (void);
	void (*Iscan_in) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	void (*Iscan_out) (void);
	void (*Iscatter_in) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Iscatter_out) (void);
	void (*Iscatterv_in) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	void (*Iscatterv_out) (void);
	#endif
} mpic_ear_t;

void mpic_ear_Allgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Allgather_out(void);
void mpic_ear_Allgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Allgatherv_out(void);
void mpic_ear_Allreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Allreduce_out(void);
void mpic_ear_Alltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Alltoall_out(void);
void mpic_ear_Alltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
void mpic_ear_Alltoallv_out(void);
void mpic_ear_Barrier_in(MPI_Comm comm);
void mpic_ear_Barrier_out(void);
void mpic_ear_Bcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
void mpic_ear_Bcast_out(void);
void mpic_ear_Bsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Bsend_out(void);
void mpic_ear_Bsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Bsend_init_out(void);
void mpic_ear_Cancel_in(MPI_Request *request);
void mpic_ear_Cancel_out(void);
void mpic_ear_Cart_create_in(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
void mpic_ear_Cart_create_out(void);
void mpic_ear_Cart_sub_in(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
void mpic_ear_Cart_sub_out(void);
void mpic_ear_Comm_create_in(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
void mpic_ear_Comm_create_out(void);
void mpic_ear_Comm_dup_in(MPI_Comm comm, MPI_Comm *newcomm);
void mpic_ear_Comm_dup_out(void);
void mpic_ear_Comm_free_in(MPI_Comm *comm);
void mpic_ear_Comm_free_out(void);
void mpic_ear_Comm_rank_in(MPI_Comm comm, int *rank);
void mpic_ear_Comm_rank_out(void);
void mpic_ear_Comm_size_in(MPI_Comm comm, int *size);
void mpic_ear_Comm_size_out(void);
void mpic_ear_Comm_spawn_in(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void mpic_ear_Comm_spawn_out(void);
void mpic_ear_Comm_spawn_multiple_in(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void mpic_ear_Comm_spawn_multiple_out(void);
void mpic_ear_Comm_split_in(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
void mpic_ear_Comm_split_out(void);
void mpic_ear_File_close_in(MPI_File *fh);
void mpic_ear_File_close_out(void);
void mpic_ear_File_read_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_out(void);
void mpic_ear_File_read_all_in(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_all_out(void);
void mpic_ear_File_read_at_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_at_out(void);
void mpic_ear_File_read_at_all_in(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_read_at_all_out(void);
void mpic_ear_File_write_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_out(void);
void mpic_ear_File_write_all_in(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_all_out(void);
void mpic_ear_File_write_at_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_at_out(void);
void mpic_ear_File_write_at_all_in(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void mpic_ear_File_write_at_all_out(void);
void mpic_ear_Finalize_in(void);
void mpic_ear_Finalize_out(void);
void mpic_ear_Gather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Gather_out(void);
void mpic_ear_Gatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Gatherv_out(void);
void mpic_ear_Get_in(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void mpic_ear_Get_out(void);
void mpic_ear_Ibsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibsend_out(void);
void mpic_ear_Init_in(int *argc, char ***argv);
void mpic_ear_Init_out(void);
void mpic_ear_Init_thread_in(int *argc, char ***argv, int required, int *provided);
void mpic_ear_Init_thread_out(void);
void mpic_ear_Intercomm_create_in(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
void mpic_ear_Intercomm_create_out(void);
void mpic_ear_Intercomm_merge_in(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
void mpic_ear_Intercomm_merge_out(void);
void mpic_ear_Iprobe_in(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
void mpic_ear_Iprobe_out(void);
void mpic_ear_Irecv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Irecv_out(void);
void mpic_ear_Irsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Irsend_out(void);
void mpic_ear_Isend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Isend_out(void);
void mpic_ear_Issend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Issend_out(void);
void mpic_ear_Probe_in(int source, int tag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Probe_out(void);
void mpic_ear_Put_in(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void mpic_ear_Put_out(void);
void mpic_ear_Recv_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Recv_out(void);
void mpic_ear_Recv_init_in(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Recv_init_out(void);
void mpic_ear_Reduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
void mpic_ear_Reduce_out(void);
void mpic_ear_Reduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Reduce_scatter_out(void);
void mpic_ear_Request_free_in(MPI_Request *request);
void mpic_ear_Request_free_out(void);
void mpic_ear_Request_get_status_in(MPI_Request request, int *flag, MPI_Status *status);
void mpic_ear_Request_get_status_out(void);
void mpic_ear_Rsend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Rsend_out(void);
void mpic_ear_Rsend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Rsend_init_out(void);
void mpic_ear_Scan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void mpic_ear_Scan_out(void);
void mpic_ear_Scatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Scatter_out(void);
void mpic_ear_Scatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void mpic_ear_Scatterv_out(void);
void mpic_ear_Send_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Send_out(void);
void mpic_ear_Send_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Send_init_out(void);
void mpic_ear_Sendrecv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Sendrecv_out(void);
void mpic_ear_Sendrecv_replace_in(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void mpic_ear_Sendrecv_replace_out(void);
void mpic_ear_Ssend_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void mpic_ear_Ssend_out(void);
void mpic_ear_Ssend_init_in(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ssend_init_out(void);
void mpic_ear_Start_in(MPI_Request *request);
void mpic_ear_Start_out(void);
void mpic_ear_Startall_in(int count, MPI_Request array_of_requests[]);
void mpic_ear_Startall_out(void);
void mpic_ear_Test_in(MPI_Request *request, int *flag, MPI_Status *status);
void mpic_ear_Test_out(void);
void mpic_ear_Testall_in(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
void mpic_ear_Testall_out(void);
void mpic_ear_Testany_in(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
void mpic_ear_Testany_out(void);
void mpic_ear_Testsome_in(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
void mpic_ear_Testsome_out(void);
void mpic_ear_Wait_in(MPI_Request *request, MPI_Status *status);
void mpic_ear_Wait_out(void);
void mpic_ear_Waitall_in(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
void mpic_ear_Waitall_out(void);
void mpic_ear_Waitany_in(int count, MPI_Request *requests, int *index, MPI_Status *status);
void mpic_ear_Waitany_out(void);
void mpic_ear_Waitsome_in(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
void mpic_ear_Waitsome_out(void);
void mpic_ear_Win_complete_in(MPI_Win win);
void mpic_ear_Win_complete_out(void);
void mpic_ear_Win_create_in(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
void mpic_ear_Win_create_out(void);
void mpic_ear_Win_fence_in(int assert, MPI_Win win);
void mpic_ear_Win_fence_out(void);
void mpic_ear_Win_free_in(MPI_Win *win);
void mpic_ear_Win_free_out(void);
void mpic_ear_Win_post_in(MPI_Group group, int assert, MPI_Win win);
void mpic_ear_Win_post_out(void);
void mpic_ear_Win_start_in(MPI_Group group, int assert, MPI_Win win);
void mpic_ear_Win_start_out(void);
void mpic_ear_Win_wait_in(MPI_Win win);
void mpic_ear_Win_wait_out(void);
#if MPI_VERSION >= 3
void mpic_ear_Iallgather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallgather_out(void);
void mpic_ear_Iallgatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallgatherv_out(void);
void mpic_ear_Iallreduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iallreduce_out(void);
void mpic_ear_Ialltoall_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ialltoall_out(void);
void mpic_ear_Ialltoallv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ialltoallv_out(void);
void mpic_ear_Ibarrier_in(MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibarrier_out(void);
void mpic_ear_Ibcast_in(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ibcast_out(void);
void mpic_ear_Igather_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Igather_out(void);
void mpic_ear_Igatherv_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Igatherv_out(void);
void mpic_ear_Ireduce_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ireduce_out(void);
void mpic_ear_Ireduce_scatter_in(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Ireduce_scatter_out(void);
void mpic_ear_Iscan_in(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscan_out(void);
void mpic_ear_Iscatter_in(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscatter_out(void);
void mpic_ear_Iscatterv_in(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void mpic_ear_Iscatterv_out(void);
#endif

#endif //_LOADER_C_VOIDS_H_
