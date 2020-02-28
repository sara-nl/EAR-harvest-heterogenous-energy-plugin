#ifndef _LOADER_C_SYMS_H_
#define _LOADER_C_SYMS_H_

#include <library/mpi_intercept/mpi.h>

typedef struct mpic_mpi_s
{
	int (*MPI_Allgather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	int (*MPI_Allgatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
	int (*MPI_Allreduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*MPI_Alltoall) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
	int (*MPI_Alltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
	int (*MPI_Barrier) (MPI_Comm comm);
	int (*MPI_Bcast) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
	int (*MPI_Bsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*MPI_Bsend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Cancel) (MPI_Request *request);
	int (*MPI_Cart_create) (MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
	int (*MPI_Cart_sub) (MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
	int (*MPI_Comm_create) (MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
	int (*MPI_Comm_dup) (MPI_Comm comm, MPI_Comm *newcomm);
	int (*MPI_Comm_free) (MPI_Comm *comm);
	int (*MPI_Comm_rank) (MPI_Comm comm, int *rank);
	int (*MPI_Comm_size) (MPI_Comm comm, int *size);
	int (*MPI_Comm_spawn) (MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	int (*MPI_Comm_spawn_multiple) (int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
	int (*MPI_Comm_split) (MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
	int (*MPI_File_close) (MPI_File *fh);
	int (*MPI_File_read) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_read_all) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_read_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_read_at_all) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_write) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_write_all) (MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_write_at) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_File_write_at_all) (MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
	int (*MPI_Finalize) (void);
	int (*MPI_Gather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*MPI_Gatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*MPI_Get) (void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	int (*MPI_Ibsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Init) (int *argc, char ***argv);
	int (*MPI_Init_thread) (int *argc, char ***argv, int required, int *provided);
	int (*MPI_Intercomm_create) (MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
	int (*MPI_Intercomm_merge) (MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
	int (*MPI_Iprobe) (int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
	int (*MPI_Irecv) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Irsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Isend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Issend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Probe) (int source, int tag, MPI_Comm comm, MPI_Status *status);
	int (*MPI_Put) (MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
	int (*MPI_Recv) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
	int (*MPI_Recv_init) (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Reduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
	int (*MPI_Reduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*MPI_Request_free) (MPI_Request *request);
	int (*MPI_Request_get_status) (MPI_Request request, int *flag, MPI_Status *status);
	int (*MPI_Rsend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*MPI_Rsend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Scan) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
	int (*MPI_Scatter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*MPI_Scatterv) (MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
	int (*MPI_Send) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*MPI_Send_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Sendrecv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	int (*MPI_Sendrecv_replace) (void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
	int (*MPI_Ssend) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
	int (*MPI_Ssend_init) (MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Start) (MPI_Request *request);
	int (*MPI_Startall) (int count, MPI_Request array_of_requests[]);
	int (*MPI_Test) (MPI_Request *request, int *flag, MPI_Status *status);
	int (*MPI_Testall) (int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
	int (*MPI_Testany) (int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
	int (*MPI_Testsome) (int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
	int (*MPI_Wait) (MPI_Request *request, MPI_Status *status);
	int (*MPI_Waitall) (int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
	int (*MPI_Waitany) (int count, MPI_Request *requests, int *index, MPI_Status *status);
	int (*MPI_Waitsome) (int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
	int (*MPI_Win_complete) (MPI_Win win);
	int (*MPI_Win_create) (void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
	int (*MPI_Win_fence) (int assert, MPI_Win win);
	int (*MPI_Win_free) (MPI_Win *win);
	int (*MPI_Win_post) (MPI_Group group, int assert, MPI_Win win);
	int (*MPI_Win_start) (MPI_Group group, int assert, MPI_Win win);
	int (*MPI_Win_wait) (MPI_Win win);
#if MPI_VERSION >= 3
	int (*MPI_Iallgather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Iallgatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Iallreduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ialltoall) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ialltoallv) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ibarrier) (MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ibcast) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Igather) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Igatherv) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ireduce) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Ireduce_scatter) (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Iscan) (MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Iscatter) (MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
	int (*MPI_Iscatterv) (MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
#endif
} mpic_mpi_t;

#endif //_LOADER_C_SYMS_H_
