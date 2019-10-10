#include <mpi.h>

struct mpisyms_t {
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
} mpi_syms;

const char *mpi_names[] = {
	"MPI_Allgather",
	"MPI_Allgatherv",
	"MPI_Allreduce",
	"MPI_Alltoall",
	"MPI_Alltoallv",
	"MPI_Barrier",
	"MPI_Bcast",
	"MPI_Bsend",
	"MPI_Bsend_init",
	"MPI_Cancel",
	"MPI_Cart_create",
	"MPI_Cart_sub",
	"MPI_Comm_create",
	"MPI_Comm_dup",
	"MPI_Comm_free",
	"MPI_Comm_rank",
	"MPI_Comm_size",
	"MPI_Comm_spawn",
	"MPI_Comm_spawn_multiple",
	"MPI_Comm_split",
	"MPI_File_close",
	"MPI_File_read",
	"MPI_File_read_all",
	"MPI_File_read_at",
	"MPI_File_read_at_all",
	"MPI_File_write",
	"MPI_File_write_all",
	"MPI_File_write_at",
	"MPI_File_write_at_all",
	"MPI_Finalize",
	"MPI_Gather",
	"MPI_Gatherv",
	"MPI_Get",
	"MPI_Ibsend",
	"MPI_Init",
	"MPI_Init_thread",
	"MPI_Intercomm_create",
	"MPI_Intercomm_merge",
	"MPI_Iprobe",
	"MPI_Irecv",
	"MPI_Irsend",
	"MPI_Isend",
	"MPI_Issend",
	"MPI_Probe",
	"MPI_Put",
	"MPI_Recv",
	"MPI_Recv_init",
	"MPI_Reduce",
	"MPI_Reduce_scatter",
	"MPI_Request_free",
	"MPI_Request_get_status",
	"MPI_Rsend",
	"MPI_Rsend_init",
	"MPI_Scan",
	"MPI_Scatter",
	"MPI_Scatterv",
	"MPI_Send",
	"MPI_Send_init",
	"MPI_Sendrecv",
	"MPI_Sendrecv_replace",
	"MPI_Ssend",
	"MPI_Ssend_init",
	"MPI_Start",
	"MPI_Startall",
	"MPI_Test",
	"MPI_Testall",
	"MPI_Testany",
	"MPI_Testsome",
	"MPI_Wait",
	"MPI_Waitall",
	"MPI_Waitany",
	"MPI_Waitsome",
	"MPI_Win_complete",
	"MPI_Win_create",
	"MPI_Win_fence",
	"MPI_Win_free",
	"MPI_Win_post",
	"MPI_Win_start",
	"MPI_Win_wait"
#if MPI_VERSION >= 3
	,
 	"MPI_Iallgather",
	"MPI_Iallgatherv",
	"MPI_Iallreduce",
	"MPI_Ialltoall",
	"MPI_Ialltoallv",
	"MPI_Ibarrier",
	"MPI_Ibcast",
	"MPI_Igather",
	"MPI_Igatherv",
	"MPI_Ireduce",
	"MPI_Ireduce_scatter",
	"MPI_Iscan",
	"MPI_Iscatter",
	"MPI_Iscatterv"
#endif
	,
};

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Allgather...............");
	ear_Allgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	mpi_syms.MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	ear_Allgather_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Allgather...............");
	return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Allgatherv...............");
	ear_Allgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	mpi_syms.MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	ear_Allgatherv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Allgatherv...............");
	return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Allreduce...............");
	ear_Allreduce_enter(sendbuf, recvbuf, count, datatype, op, comm);
	mpi_syms.MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	ear_Allreduce_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Allreduce...............");
	return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Alltoall...............");
	ear_Alltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	mpi_syms.MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	ear_Alltoall_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Alltoall...............");
	return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Alltoallv...............");
	ear_Alltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	mpi_syms.MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	ear_Alltoallv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Alltoallv...............");
	return res;
}

int MPI_Barrier(MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Barrier...............");
	ear_Barrier_enter(comm);
	mpi_syms.MPI_Barrier(comm);
	ear_Barrier_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Barrier...............");
	return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Bcast...............");
	ear_Bcast_enter(buffer, count, datatype, root, comm);
	mpi_syms.MPI_Bcast(buffer, count, datatype, root, comm);
	ear_Bcast_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Bcast...............");
	return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Bsend...............");
	ear_Bsend_enter(buf, count, datatype, dest, tag, comm);
	mpi_syms.MPI_Bsend(buf, count, datatype, dest, tag, comm);
	ear_Bsend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Bsend...............");
	return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Bsend_init...............");
	ear_Bsend_init_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	ear_Bsend_init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Bsend_init...............");
	return res;
}

int MPI_Cancel(MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Cancel...............");
	ear_Cancel_enter(request);
	mpi_syms.MPI_Cancel(request);
	ear_Cancel_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Cancel...............");
	return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Cart_create...............");
	ear_Cart_create_enter(comm_old, ndims, dims, periods, reorder, comm_cart);
	mpi_syms.MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	ear_Cart_create_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Cart_create...............");
	return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Cart_sub...............");
	ear_Cart_sub_enter(comm, remain_dims, newcomm);
	mpi_syms.MPI_Cart_sub(comm, remain_dims, newcomm);
	ear_Cart_sub_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Cart_sub...............");
	return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_create...............");
	ear_Comm_create_enter(comm, group, newcomm);
	mpi_syms.MPI_Comm_create(comm, group, newcomm);
	ear_Comm_create_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_create...............");
	return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_dup...............");
	ear_Comm_dup_enter(comm, newcomm);
	mpi_syms.MPI_Comm_dup(comm, newcomm);
	ear_Comm_dup_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_dup...............");
	return res;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_free...............");
	ear_Comm_free_enter(comm);
	mpi_syms.MPI_Comm_free(comm);
	ear_Comm_free_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_free...............");
	return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_rank...............");
	ear_Comm_rank_enter(comm, rank);
	mpi_syms.MPI_Comm_rank(comm, rank);
	ear_Comm_rank_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_rank...............");
	return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_size...............");
	ear_Comm_size_enter(comm, size);
	mpi_syms.MPI_Comm_size(comm, size);
	ear_Comm_size_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_size...............");
	return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_spawn...............");
	ear_Comm_spawn_enter(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	mpi_syms.MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	ear_Comm_spawn_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_spawn...............");
	return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_spawn_multiple...............");
	ear_Comm_spawn_multiple_enter(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	mpi_syms.MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	ear_Comm_spawn_multiple_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_spawn_multiple...............");
	return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Comm_split...............");
	ear_Comm_split_enter(comm, color, key, newcomm);
	mpi_syms.MPI_Comm_split(comm, color, key, newcomm);
	ear_Comm_split_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Comm_split...............");
	return res;
}

int MPI_File_close(MPI_File *fh)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_close...............");
	ear_File_close_enter(fh);
	mpi_syms.MPI_File_close(fh);
	ear_File_close_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_close...............");
	return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_read...............");
	ear_File_read_enter(fh, buf, count, datatype, status);
	mpi_syms.MPI_File_read(fh, buf, count, datatype, status);
	ear_File_read_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_read...............");
	return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_read_all...............");
	ear_File_read_all_enter(fh, buf, count, datatype, status);
	mpi_syms.MPI_File_read_all(fh, buf, count, datatype, status);
	ear_File_read_all_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_read_all...............");
	return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_read_at...............");
	ear_File_read_at_enter(fh, offset, buf, count, datatype, status);
	mpi_syms.MPI_File_read_at(fh, offset, buf, count, datatype, status);
	ear_File_read_at_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_read_at...............");
	return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_read_at_all...............");
	ear_File_read_at_all_enter(fh, offset, buf, count, datatype, status);
	mpi_syms.MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	ear_File_read_at_all_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_read_at_all...............");
	return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_write...............");
	ear_File_write_enter(fh, buf, count, datatype, status);
	mpi_syms.MPI_File_write(fh, buf, count, datatype, status);
	ear_File_write_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_write...............");
	return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_write_all...............");
	ear_File_write_all_enter(fh, buf, count, datatype, status);
	mpi_syms.MPI_File_write_all(fh, buf, count, datatype, status);
	ear_File_write_all_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_write_all...............");
	return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_write_at...............");
	ear_File_write_at_enter(fh, offset, buf, count, datatype, status);
	mpi_syms.MPI_File_write_at(fh, offset, buf, count, datatype, status);
	ear_File_write_at_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_write_at...............");
	return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_File_write_at_all...............");
	ear_File_write_at_all_enter(fh, offset, buf, count, datatype, status);
	mpi_syms.MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	ear_File_write_at_all_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_File_write_at_all...............");
	return res;
}

int MPI_Finalize(void)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Finalize...............");
	ear_Finalize_enter();
	mpi_syms.MPI_Finalize();
	ear_Finalize_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Finalize...............");
	return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Gather...............");
	ear_Gather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpi_syms.MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	ear_Gather_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Gather...............");
	return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Gatherv...............");
	ear_Gatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	mpi_syms.MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	ear_Gatherv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Gatherv...............");
	return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Get...............");
	ear_Get_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	mpi_syms.MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	ear_Get_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Get...............");
	return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Ibsend...............");
	ear_Ibsend_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	ear_Ibsend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Ibsend...............");
	return res;
}

int MPI_Init(int *argc, char ***argv)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Init...............");
	ear_Init_enter(argc, argv);
	mpi_syms.MPI_Init(argc, argv);
	ear_Init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Init...............");
	return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Init_thread...............");
	ear_Init_thread_enter(argc, argv, required, provided);
	mpi_syms.MPI_Init_thread(argc, argv, required, provided);
	ear_Init_thread_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Init_thread...............");
	return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Intercomm_create...............");
	ear_Intercomm_create_enter(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	mpi_syms.MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	ear_Intercomm_create_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Intercomm_create...............");
	return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Intercomm_merge...............");
	ear_Intercomm_merge_enter(intercomm, high, newintracomm);
	mpi_syms.MPI_Intercomm_merge(intercomm, high, newintracomm);
	ear_Intercomm_merge_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Intercomm_merge...............");
	return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Iprobe...............");
	ear_Iprobe_enter(source, tag, comm, flag, status);
	mpi_syms.MPI_Iprobe(source, tag, comm, flag, status);
	ear_Iprobe_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Iprobe...............");
	return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Irecv...............");
	ear_Irecv_enter(buf, count, datatype, source, tag, comm, request);
	mpi_syms.MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	ear_Irecv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Irecv...............");
	return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Irsend...............");
	ear_Irsend_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	ear_Irsend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Irsend...............");
	return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Isend...............");
	ear_Isend_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	ear_Isend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Isend...............");
	return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Issend...............");
	ear_Issend_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	ear_Issend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Issend...............");
	return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Probe...............");
	ear_Probe_enter(source, tag, comm, status);
	mpi_syms.MPI_Probe(source, tag, comm, status);
	ear_Probe_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Probe...............");
	return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Put...............");
	ear_Put_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	mpi_syms.MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	ear_Put_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Put...............");
	return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Recv...............");
	ear_Recv_enter(buf, count, datatype, source, tag, comm, status);
	mpi_syms.MPI_Recv(buf, count, datatype, source, tag, comm, status);
	ear_Recv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Recv...............");
	return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Recv_init...............");
	ear_Recv_init_enter(buf, count, datatype, source, tag, comm, request);
	mpi_syms.MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	ear_Recv_init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Recv_init...............");
	return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Reduce...............");
	ear_Reduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm);
	mpi_syms.MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	ear_Reduce_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Reduce...............");
	return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Reduce_scatter...............");
	ear_Reduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	mpi_syms.MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	ear_Reduce_scatter_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Reduce_scatter...............");
	return res;
}

int MPI_Request_free(MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Request_free...............");
	ear_Request_free_enter(request);
	mpi_syms.MPI_Request_free(request);
	ear_Request_free_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Request_free...............");
	return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Request_get_status...............");
	ear_Request_get_status_enter(request, flag, status);
	mpi_syms.MPI_Request_get_status(request, flag, status);
	ear_Request_get_status_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Request_get_status...............");
	return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Rsend...............");
	ear_Rsend_enter(buf, count, datatype, dest, tag, comm);
	mpi_syms.MPI_Rsend(buf, count, datatype, dest, tag, comm);
	ear_Rsend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Rsend...............");
	return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Rsend_init...............");
	ear_Rsend_init_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	ear_Rsend_init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Rsend_init...............");
	return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Scan...............");
	ear_Scan_enter(sendbuf, recvbuf, count, datatype, op, comm);
	mpi_syms.MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	ear_Scan_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Scan...............");
	return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Scatter...............");
	ear_Scatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpi_syms.MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	ear_Scatter_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Scatter...............");
	return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Scatterv...............");
	ear_Scatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpi_syms.MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	ear_Scatterv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Scatterv...............");
	return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Send...............");
	ear_Send_enter(buf, count, datatype, dest, tag, comm);
	mpi_syms.MPI_Send(buf, count, datatype, dest, tag, comm);
	ear_Send_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Send...............");
	return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Send_init...............");
	ear_Send_init_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	ear_Send_init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Send_init...............");
	return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Sendrecv...............");
	ear_Sendrecv_enter(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	mpi_syms.MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	ear_Sendrecv_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Sendrecv...............");
	return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Sendrecv_replace...............");
	ear_Sendrecv_replace_enter(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	mpi_syms.MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	ear_Sendrecv_replace_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Sendrecv_replace...............");
	return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Ssend...............");
	ear_Ssend_enter(buf, count, datatype, dest, tag, comm);
	mpi_syms.MPI_Ssend(buf, count, datatype, dest, tag, comm);
	ear_Ssend_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Ssend...............");
	return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Ssend_init...............");
	ear_Ssend_init_enter(buf, count, datatype, dest, tag, comm, request);
	mpi_syms.MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	ear_Ssend_init_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Ssend_init...............");
	return res;
}

int MPI_Start(MPI_Request *request)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Start...............");
	ear_Start_enter(request);
	mpi_syms.MPI_Start(request);
	ear_Start_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Start...............");
	return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	mpi_verb(VB_MPI_INT, ">> MPI_Startall...............");
	ear_Startall_enter(count, array_of_requests);
	mpi_syms.MPI_Startall(count, array_of_requests);
	ear_Startall_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Startall...............");
	return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Test...............");
	ear_Test_enter(request, flag, status);
	mpi_syms.MPI_Test(request, flag, status);
	ear_Test_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Test...............");
	return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	mpi_verb(VB_MPI_INT, ">> MPI_Testall...............");
	ear_Testall_enter(count, array_of_requests, flag, array_of_statuses);
	mpi_syms.MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	ear_Testall_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Testall...............");
	return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Testany...............");
	ear_Testany_enter(count, array_of_requests, indx, flag, status);
	mpi_syms.MPI_Testany(count, array_of_requests, indx, flag, status);
	ear_Testany_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Testany...............");
	return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	mpi_verb(VB_MPI_INT, ">> MPI_Testsome...............");
	ear_Testsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	mpi_syms.MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	ear_Testsome_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Testsome...............");
	return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Wait...............");
	ear_Wait_enter(request, status);
	mpi_syms.MPI_Wait(request, status);
	ear_Wait_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Wait...............");
	return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Waitall...............");
	ear_Waitall_enter(count, array_of_requests, array_of_statuses);
	mpi_syms.MPI_Waitall(count, array_of_requests, array_of_statuses);
	ear_Waitall_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Waitall...............");
	return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Waitany...............");
	ear_Waitany_enter(count, requests, index, status);
	mpi_syms.MPI_Waitany(count, requests, index, status);
	ear_Waitany_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Waitany...............");
	return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Waitsome...............");
	ear_Waitsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	mpi_syms.MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	ear_Waitsome_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Waitsome...............");
	return res;
}

int MPI_Win_complete(MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_complete...............");
	ear_Win_complete_enter(win);
	mpi_syms.MPI_Win_complete(win);
	ear_Win_complete_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_complete...............");
	return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_create...............");
	ear_Win_create_enter(base, size, disp_unit, info, comm, win);
	mpi_syms.MPI_Win_create(base, size, disp_unit, info, comm, win);
	ear_Win_create_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_create...............");
	return res;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_fence...............");
	ear_Win_fence_enter(assert, win);
	mpi_syms.MPI_Win_fence(assert, win);
	ear_Win_fence_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_fence...............");
	return res;
}

int MPI_Win_free(MPI_Win *win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_free...............");
	ear_Win_free_enter(win);
	mpi_syms.MPI_Win_free(win);
	ear_Win_free_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_free...............");
	return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_post...............");
	ear_Win_post_enter(group, assert, win);
	mpi_syms.MPI_Win_post(group, assert, win);
	ear_Win_post_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_post...............");
	return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_start...............");
	ear_Win_start_enter(group, assert, win);
	mpi_syms.MPI_Win_start(group, assert, win);
	ear_Win_start_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_start...............");
	return res;
}

int MPI_Win_wait(MPI_Win win)
{
	mpi_verb(VB_MPI_INT, ">> MPI_Win_wait...............");
	ear_Win_wait_enter(win);
	mpi_syms.MPI_Win_wait(win);
	ear_Win_wait_leave();
	mpi_verb(VB_MPI_INT, "<< MPI_Win_wait...............");
	return res;
}


#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iallgather...............");
    ear_Iallgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    mpi_syms.MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    ear_Iallgather_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iallgather...............");
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iallgatherv...............");
    ear_Iallgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    mpi_syms.MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    ear_Iallgatherv_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iallgatherv...............");
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iallreduce...............");
    ear_Iallreduce_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    mpi_syms.MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    ear_Iallreduce_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iallreduce...............");
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ialltoall...............");
    ear_Ialltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    mpi_syms.MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    ear_Ialltoall_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ialltoall...............");
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ialltoallv...............");
    ear_Ialltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    mpi_syms.MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    ear_Ialltoallv_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ialltoallv...............");
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ibarrier...............");
    ear_Ibarrier_enter(comm, request);
    mpi_syms.MPI_Ibarrier(comm, request);
    ear_Ibarrier_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ibarrier...............");
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ibcast...............");
    ear_Ibcast_enter(buffer, count, datatype, root, comm, request);
    mpi_syms.MPI_Ibcast(buffer, count, datatype, root, comm, request);
    ear_Ibcast_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ibcast...............");
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Igather...............");
    ear_Igather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpi_syms.MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    ear_Igather_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Igather...............");
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Igatherv...............");
    ear_Igatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    mpi_syms.MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    ear_Igatherv_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Igatherv...............");
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ireduce...............");
    ear_Ireduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    mpi_syms.MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    ear_Ireduce_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ireduce...............");
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Ireduce_scatter...............");
    ear_Ireduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    mpi_syms.MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    ear_Ireduce_scatter_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Ireduce_scatter...............");
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iscan...............");
    ear_Iscan_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    mpi_syms.MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    ear_Iscan_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iscan...............");
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iscatter...............");
    ear_Iscatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpi_syms.MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    ear_Iscatter_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iscatter...............");
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    mpi_verb(VB_MPI_INT, ">> MPI_Iscatterv...............");
    ear_Iscatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpi_syms.MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    ear_Iscatterv_leave();
    mpi_verb(VB_MPI_INT, "<< MPI_Iscatterv...............");
    return res;
}
#endif