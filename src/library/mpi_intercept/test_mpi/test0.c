#include <stdio.h>
#include <dlfcn.h>
#include "test0.h"

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgather\n");
	return 0;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgatherv\n");
	return 0;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Allreduce\n");
	return 0;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoall\n");
	return 0;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoallv\n");
	return 0;
}

int MPI_Barrier(MPI_Comm comm)
{
	printf("MPI_Barrier\n");
	return 0;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	printf("MPI_Bcast\n");
	return 0;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Bsend\n");
	return 0;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Bsend_init\n");
	return 0;
}

int MPI_Cancel(MPI_Request *request)
{
	printf("MPI_Cancel\n");
	return 0;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	printf("MPI_Cart_create\n");
	return 0;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	printf("MPI_Cart_sub\n");
	return 0;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	printf("MPI_Comm_create\n");
	return 0;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	printf("MPI_Comm_dup\n");
	return 0;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	printf("MPI_Comm_free\n");
	return 0;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	printf("MPI_Comm_rank\n");
	return 0;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	printf("MPI_Comm_size\n");
	return 0;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn\n");
	return 0;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn_multiple\n");
	return 0;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	printf("MPI_Comm_split\n");
	return 0;
}

int MPI_File_close(MPI_File *fh)
{
	printf("MPI_File_close\n");
	return 0;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read\n");
	return 0;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_all\n");
	return 0;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at\n");
	return 0;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at_all\n");
	return 0;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write\n");
	return 0;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_all\n");
	return 0;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at\n");
	return 0;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at_all\n");
	return 0;
}

int MPI_Finalize(void)
{
	printf("MPI_Finalize\n");
	return 0;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gather\n");
	return 0;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gatherv\n");
	return 0;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Get\n");
	return 0;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ibsend\n");
	return 0;
}

int MPI_Init(int *argc, char ***argv)
{
	printf("MPI_Init\n");
	return 0;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	printf("MPI_Init_thread\n");
	return 0;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	printf("MPI_Intercomm_create\n");
	return 0;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	printf("MPI_Intercomm_merge\n");
	return 0;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	printf("MPI_Iprobe\n");
	return 0;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irecv\n");
	return 0;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irsend\n");
	return 0;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Isend\n");
	return 0;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Issend\n");
	return 0;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Probe\n");
	return 0;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Put\n");
	return 0;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Recv\n");
	return 0;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Recv_init\n");
	return 0;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	printf("MPI_Reduce\n");
	return 0;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Reduce_scatter\n");
	return 0;
}

int MPI_Request_free(MPI_Request *request)
{
	printf("MPI_Request_free\n");
	return 0;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	printf("MPI_Request_get_status\n");
	return 0;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Rsend\n");
	return 0;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Rsend_init\n");
	return 0;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Scan\n");
	return 0;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatter\n");
	return 0;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatterv\n");
	return 0;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Send\n");
	return 0;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Send_init\n");
	return 0;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv\n");
	return 0;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv_replace\n");
	return 0;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Ssend\n");
	return 0;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ssend_init\n");
	return 0;
}

int MPI_Start(MPI_Request *request)
{
	printf("MPI_Start\n");
	return 0;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	printf("MPI_Startall\n");
	return 0;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	printf("MPI_Test\n");
	return 0;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	printf("MPI_Testall\n");
	return 0;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	printf("MPI_Testany\n");
	return 0;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	printf("MPI_Testsome\n");
	return 0;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	printf("MPI_Wait\n");
	return 0;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitall\n");
	return 0;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	printf("MPI_Waitany\n");
	return 0;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitsome\n");
	return 0;
}

int MPI_Win_complete(MPI_Win win)
{
	printf("MPI_Win_complete\n");
	return 0;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	printf("MPI_Win_create\n");
	return 0;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	printf("MPI_Win_fence\n");
	return 0;
}

int MPI_Win_free(MPI_Win *win)
{
	printf("MPI_Win_free\n");
	return 0;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_post\n");
	return 0;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_start\n");
	return 0;
}

int MPI_Win_wait(MPI_Win win)
{
	printf("MPI_Win_wait\n");
	return 0;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgather\n");
    return 0;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgatherv\n");
    return 0;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallreduce\n");
    return 0;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoall\n");
    return 0;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoallv\n");
    return 0;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibarrier\n");
    return 0;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibcast\n");
    return 0;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igather\n");
    return 0;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igatherv\n");
    return 0;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce\n");
    return 0;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce_scatter\n");
    return 0;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscan\n");
    return 0;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatter\n");
    return 0;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatterv\n");
    return 0;
}
#endif
