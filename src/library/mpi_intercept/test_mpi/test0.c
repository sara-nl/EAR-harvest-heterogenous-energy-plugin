#include <stdio.h>
#include <dlfcn.h>
#include "test0.h"

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgather 1\n");
	MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	return 0;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgatherv 1\n");
	MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	return 0;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Allreduce 1\n");
	MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	return 0;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoall 1\n");
	MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	return 0;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoallv 1\n");
	MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	return 0;
}

int MPI_Barrier(MPI_Comm comm)
{
	printf("MPI_Barrier 1\n");
	MPI_Barrier(comm);
	return 0;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	printf("MPI_Bcast 1\n");
	MPI_Bcast(buffer, count, datatype, root, comm);
	return 0;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Bsend 1\n");
	MPI_Bsend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Bsend_init 1\n");
	MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Cancel(MPI_Request *request)
{
	printf("MPI_Cancel 1\n");
	MPI_Cancel(request);
	return 0;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	printf("MPI_Cart_create 1\n");
	MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	return 0;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	printf("MPI_Cart_sub 1\n");
	MPI_Cart_sub(comm, remain_dims, newcomm);
	return 0;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	printf("MPI_Comm_create 1\n");
	MPI_Comm_create(comm, group, newcomm);
	return 0;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	printf("MPI_Comm_dup 1\n");
	MPI_Comm_dup(comm, newcomm);
	return 0;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	printf("MPI_Comm_free 1\n");
	MPI_Comm_free(comm);
	return 0;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	printf("MPI_Comm_rank 1\n");
	MPI_Comm_rank(comm, rank);
	return 0;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	printf("MPI_Comm_size 1\n");
	MPI_Comm_size(comm, size);
	return 0;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn 1\n");
	MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	return 0;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn_multiple 1\n");
	MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	return 0;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	printf("MPI_Comm_split 1\n");
	MPI_Comm_split(comm, color, key, newcomm);
	return 0;
}

int MPI_File_close(MPI_File *fh)
{
	printf("MPI_File_close 1\n");
	MPI_File_close(fh);
	return 0;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read 1\n");
	MPI_File_read(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_all 1\n");
	MPI_File_read_all(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at 1\n");
	MPI_File_read_at(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at_all 1\n");
	MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write 1\n");
	MPI_File_write(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_all 1\n");
	MPI_File_write_all(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at 1\n");
	MPI_File_write_at(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at_all 1\n");
	MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_Finalize(void)
{
	printf("MPI_Finalize 1\n");
	MPI_Finalize();
	return 0;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gather 1\n");
	MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gatherv 1\n");
	MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	return 0;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Get 1\n");
	MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	return 0;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ibsend 1\n");
	MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Init(int *argc, char ***argv)
{
	symplug_join(RTLD_NEXT, (void *) &mpi_syms, mpi_names, (int) (sizeof(mpi_names) / sizeof(mpi_names[0])));
	printf("MPI_Init 1 (%d)\n", (int) (sizeof(mpi_names) / sizeof(mpi_names[0])));
	MPI_Init(argc, argv);
	return 0;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	printf("MPI_Init_thread 1\n");
	MPI_Init_thread(argc, argv, required, provided);
	return 0;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	printf("MPI_Intercomm_create 1\n");
	MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	return 0;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	printf("MPI_Intercomm_merge 1\n");
	MPI_Intercomm_merge(intercomm, high, newintracomm);
	return 0;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	printf("MPI_Iprobe 1\n");
	MPI_Iprobe(source, tag, comm, flag, status);
	return 0;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irecv 1\n");
	MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	return 0;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irsend 1\n");
	MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Isend 1\n");
	MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Issend 1\n");
	MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Probe 1\n");
	MPI_Probe(source, tag, comm, status);
	return 0;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Put 1\n");
	MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	return 0;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Recv 1\n");
	MPI_Recv(buf, count, datatype, source, tag, comm, status);
	return 0;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Recv_init 1\n");
	MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	return 0;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	printf("MPI_Reduce 1\n");
	MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	return 0;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Reduce_scatter 1\n");
	MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	return 0;
}

int MPI_Request_free(MPI_Request *request)
{
	printf("MPI_Request_free 1\n");
	MPI_Request_free(request);
	return 0;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	printf("MPI_Request_get_status 1\n");
	MPI_Request_get_status(request, flag, status);
	return 0;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Rsend 1\n");
	MPI_Rsend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Rsend_init 1\n");
	MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Scan 1\n");
	MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	return 0;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatter 1\n");
	MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatterv 1\n");
	MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Send 1\n");
	MPI_Send(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Send_init 1\n");
	MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv 1\n");
	MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	return 0;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv_replace 1\n");
	MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	return 0;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Ssend 1\n");
	MPI_Ssend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ssend_init 1\n");
	MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Start(MPI_Request *request)
{
	printf("MPI_Start 1\n");
	MPI_Start(request);
	return 0;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	printf("MPI_Startall 1\n");
	MPI_Startall(count, array_of_requests);
	return 0;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	printf("MPI_Test 1\n");
	MPI_Test(request, flag, status);
	return 0;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	printf("MPI_Testall 1\n");
	MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	return 0;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	printf("MPI_Testany 1\n");
	MPI_Testany(count, array_of_requests, indx, flag, status);
	return 0;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	printf("MPI_Testsome 1\n");
	MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	return 0;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	printf("MPI_Wait 1\n");
	MPI_Wait(request, status);
	return 0;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitall 1\n");
	MPI_Waitall(count, array_of_requests, array_of_statuses);
	return 0;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	printf("MPI_Waitany 1\n");
	MPI_Waitany(count, requests, index, status);
	return 0;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitsome 1\n");
	MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	return 0;
}

int MPI_Win_complete(MPI_Win win)
{
	printf("MPI_Win_complete 1\n");
	MPI_Win_complete(win);
	return 0;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	printf("MPI_Win_create 1\n");
	MPI_Win_create(base, size, disp_unit, info, comm, win);
	return 0;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	printf("MPI_Win_fence 1\n");
	MPI_Win_fence(assert, win);
	return 0;
}

int MPI_Win_free(MPI_Win *win)
{
	printf("MPI_Win_free 1\n");
	MPI_Win_free(win);
	return 0;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_post 1\n");
	MPI_Win_post(group, assert, win);
	return 0;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_start 1\n");
	MPI_Win_start(group, assert, win);
	return 0;
}

int MPI_Win_wait(MPI_Win win)
{
	printf("MPI_Win_wait 1\n");
	MPI_Win_wait(win);
	return 0;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgather 1\n");
    MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return 0;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgatherv 1\n");
    MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return 0;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallreduce 1\n");
    MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    return 0;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoall 1\n");
    MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return 0;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoallv 1\n");
    MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return 0;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibarrier 1\n");
    MPI_Ibarrier(comm, request);
    return 0;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibcast 1\n");
    MPI_Ibcast(buffer, count, datatype, root, comm, request);
    return 0;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igather 1\n");
    MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igatherv 1\n");
    MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    return 0;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce 1\n");
    MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    return 0;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce_scatter 1\n");
    MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    return 0;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscan 1\n");
    MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return 0;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatter 1\n");
    MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatterv 1\n");
    MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}
#endif