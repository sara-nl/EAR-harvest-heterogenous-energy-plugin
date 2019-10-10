int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgather 1");
	mpi_syms.MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	return 0;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgatherv 1");
	mpi_syms.MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	return 0;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Allreduce 1");
	mpi_syms.MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	return 0;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoall 1");
	mpi_syms.MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	return 0;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoallv 1");
	mpi_syms.MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	return 0;
}

int MPI_Barrier(MPI_Comm comm)
{
	printf("MPI_Barrier 1");
	mpi_syms.MPI_Barrier(comm);
	return 0;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	printf("MPI_Bcast 1");
	mpi_syms.MPI_Bcast(buffer, count, datatype, root, comm);
	return 0;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Bsend 1");
	mpi_syms.MPI_Bsend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Bsend_init 1");
	mpi_syms.MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Cancel(MPI_Request *request)
{
	printf("MPI_Cancel 1");
	mpi_syms.MPI_Cancel(request);
	return 0;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	printf("MPI_Cart_create 1");
	mpi_syms.MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	return 0;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	printf("MPI_Cart_sub 1");
	mpi_syms.MPI_Cart_sub(comm, remain_dims, newcomm);
	return 0;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	printf("MPI_Comm_create 1");
	mpi_syms.MPI_Comm_create(comm, group, newcomm);
	return 0;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	printf("MPI_Comm_dup 1");
	mpi_syms.MPI_Comm_dup(comm, newcomm);
	return 0;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	printf("MPI_Comm_free 1");
	mpi_syms.MPI_Comm_free(comm);
	return 0;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	printf("MPI_Comm_rank 1");
	mpi_syms.MPI_Comm_rank(comm, rank);
	return 0;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	printf("MPI_Comm_size 1");
	mpi_syms.MPI_Comm_size(comm, size);
	return 0;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn 1");
	mpi_syms.MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	return 0;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn_multiple 1");
	mpi_syms.MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	return 0;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	printf("MPI_Comm_split 1");
	mpi_syms.MPI_Comm_split(comm, color, key, newcomm);
	return 0;
}

int MPI_File_close(MPI_File *fh)
{
	printf("MPI_File_close 1");
	mpi_syms.MPI_File_close(fh);
	return 0;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read 1");
	mpi_syms.MPI_File_read(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_all 1");
	mpi_syms.MPI_File_read_all(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at 1");
	mpi_syms.MPI_File_read_at(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at_all 1");
	mpi_syms.MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write 1");
	mpi_syms.MPI_File_write(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_all 1");
	mpi_syms.MPI_File_write_all(fh, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at 1");
	mpi_syms.MPI_File_write_at(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at_all 1");
	mpi_syms.MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	return 0;
}

int MPI_Finalize(void)
{
	printf("MPI_Finalize 1");
	mpi_syms.MPI_Finalize();
	return 0;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gather 1");
	mpi_syms.MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gatherv 1");
	mpi_syms.MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	return 0;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Get 1");
	mpi_syms.MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	return 0;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ibsend 1");
	mpi_syms.MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Init(int *argc, char ***argv)
{
	printf("MPI_Init 1");
	mpi_syms.MPI_Init(argc, argv);
	return 0;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	printf("MPI_Init_thread 1");
	mpi_syms.MPI_Init_thread(argc, argv, required, provided);
	return 0;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	printf("MPI_Intercomm_create 1");
	mpi_syms.MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	return 0;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	printf("MPI_Intercomm_merge 1");
	mpi_syms.MPI_Intercomm_merge(intercomm, high, newintracomm);
	return 0;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	printf("MPI_Iprobe 1");
	mpi_syms.MPI_Iprobe(source, tag, comm, flag, status);
	return 0;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irecv 1");
	mpi_syms.MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	return 0;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irsend 1");
	mpi_syms.MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Isend 1");
	mpi_syms.MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Issend 1");
	mpi_syms.MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Probe 1");
	mpi_syms.MPI_Probe(source, tag, comm, status);
	return 0;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Put 1");
	mpi_syms.MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	return 0;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Recv 1");
	mpi_syms.MPI_Recv(buf, count, datatype, source, tag, comm, status);
	return 0;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Recv_init 1");
	mpi_syms.MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	return 0;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	printf("MPI_Reduce 1");
	mpi_syms.MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	return 0;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Reduce_scatter 1");
	mpi_syms.MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	return 0;
}

int MPI_Request_free(MPI_Request *request)
{
	printf("MPI_Request_free 1");
	mpi_syms.MPI_Request_free(request);
	return 0;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	printf("MPI_Request_get_status 1");
	mpi_syms.MPI_Request_get_status(request, flag, status);
	return 0;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Rsend 1");
	mpi_syms.MPI_Rsend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Rsend_init 1");
	mpi_syms.MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Scan 1");
	mpi_syms.MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	return 0;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatter 1");
	mpi_syms.MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatterv 1");
	mpi_syms.MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	return 0;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Send 1");
	mpi_syms.MPI_Send(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Send_init 1");
	mpi_syms.MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv 1");
	mpi_syms.MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	return 0;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv_replace 1");
	mpi_syms.MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	return 0;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Ssend 1");
	mpi_syms.MPI_Ssend(buf, count, datatype, dest, tag, comm);
	return 0;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ssend_init 1");
	mpi_syms.MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	return 0;
}

int MPI_Start(MPI_Request *request)
{
	printf("MPI_Start 1");
	mpi_syms.MPI_Start(request);
	return 0;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	printf("MPI_Startall 1");
	mpi_syms.MPI_Startall(count, array_of_requests);
	return 0;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	printf("MPI_Test 1");
	mpi_syms.MPI_Test(request, flag, status);
	return 0;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	printf("MPI_Testall 1");
	mpi_syms.MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	return 0;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	printf("MPI_Testany 1");
	mpi_syms.MPI_Testany(count, array_of_requests, indx, flag, status);
	return 0;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	printf("MPI_Testsome 1");
	mpi_syms.MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	return 0;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	printf("MPI_Wait 1");
	mpi_syms.MPI_Wait(request, status);
	return 0;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitall 1");
	mpi_syms.MPI_Waitall(count, array_of_requests, array_of_statuses);
	return 0;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	printf("MPI_Waitany 1");
	mpi_syms.MPI_Waitany(count, requests, index, status);
	return 0;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitsome 1");
	mpi_syms.MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	return 0;
}

int MPI_Win_complete(MPI_Win win)
{
	printf("MPI_Win_complete 1");
	mpi_syms.MPI_Win_complete(win);
	return 0;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	printf("MPI_Win_create 1");
	mpi_syms.MPI_Win_create(base, size, disp_unit, info, comm, win);
	return 0;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	printf("MPI_Win_fence 1");
	mpi_syms.MPI_Win_fence(assert, win);
	return 0;
}

int MPI_Win_free(MPI_Win *win)
{
	printf("MPI_Win_free 1");
	mpi_syms.MPI_Win_free(win);
	return 0;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_post 1");
	mpi_syms.MPI_Win_post(group, assert, win);
	return 0;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_start 1");
	mpi_syms.MPI_Win_start(group, assert, win);
	return 0;
}

int MPI_Win_wait(MPI_Win win)
{
	printf("MPI_Win_wait 1");
	mpi_syms.MPI_Win_wait(win);
	return 0;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgather 1");
    mpi_syms.MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return 0;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgatherv 1");
    mpi_syms.MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return 0;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallreduce 1");
    mpi_syms.MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    return 0;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoall 1");
    mpi_syms.MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return 0;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoallv 1");
    mpi_syms.MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return 0;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibarrier 1");
    mpi_syms.MPI_Ibarrier(comm, request);
    return 0;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibcast 1");
    mpi_syms.MPI_Ibcast(buffer, count, datatype, root, comm, request);
    return 0;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igather 1");
    mpi_syms.MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igatherv 1");
    mpi_syms.MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    return 0;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce 1");
    mpi_syms.MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    return 0;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce_scatter 1");
    mpi_syms.MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    return 0;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscan 1");
    mpi_syms.MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return 0;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatter 1");
    mpi_syms.MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatterv 1");
    mpi_syms.MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return 0;
}
#endif

void EAR_MPI_Allgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void EAR_MPI_Allgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm);
void EAR_MPI_Allreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void EAR_MPI_Alltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
void EAR_MPI_Alltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
void EAR_MPI_Barrier_enter(MPI_Comm comm);
void EAR_MPI_Bcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
void EAR_MPI_Bsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void EAR_MPI_Bsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Cancel_enter(MPI_Request *request);
void EAR_MPI_Cart_create_enter(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart);
void EAR_MPI_Cart_sub_enter(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm);
void EAR_MPI_Comm_create_enter(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
void EAR_MPI_Comm_dup_enter(MPI_Comm comm, MPI_Comm *newcomm);
void EAR_MPI_Comm_free_enter(MPI_Comm *comm);
void EAR_MPI_Comm_rank_enter(MPI_Comm comm, int *rank);
void EAR_MPI_Comm_size_enter(MPI_Comm comm, int *size);
void EAR_MPI_Comm_spawn_enter(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void EAR_MPI_Comm_spawn_multiple_enter(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
void EAR_MPI_Comm_split_enter(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
void EAR_MPI_File_close_enter(MPI_File *fh);
void EAR_MPI_File_read_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_read_all_enter(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_read_at_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_read_at_all_enter(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_write_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_write_all_enter(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_write_at_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_File_write_at_all_enter(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
void EAR_MPI_Finalize_enter(void);
void EAR_MPI_Gather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void EAR_MPI_Gatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
void EAR_MPI_Get_enter(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void EAR_MPI_Ibsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Init_enter(int *argc, char ***argv);
void EAR_MPI_Init_thread_enter(int *argc, char ***argv, int required, int *provided);
void EAR_MPI_Intercomm_create_enter(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
void EAR_MPI_Intercomm_merge_enter(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
void EAR_MPI_Iprobe_enter(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
void EAR_MPI_Irecv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Irsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Isend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Issend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Probe_enter(int source, int tag, MPI_Comm comm, MPI_Status *status);
void EAR_MPI_Put_enter(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
void EAR_MPI_Recv_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
void EAR_MPI_Recv_init_enter(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Reduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
void EAR_MPI_Reduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void EAR_MPI_Request_free_enter(MPI_Request *request);
void EAR_MPI_Request_get_status_enter(MPI_Request request, int *flag, MPI_Status *status);
void EAR_MPI_Rsend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void EAR_MPI_Rsend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Scan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
void EAR_MPI_Scatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void EAR_MPI_Scatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
void EAR_MPI_Send_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void EAR_MPI_Send_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Sendrecv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void EAR_MPI_Sendrecv_replace_enter(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
void EAR_MPI_Ssend_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
void EAR_MPI_Ssend_init_enter(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Start_enter(MPI_Request *request);
void EAR_MPI_Startall_enter(int count, MPI_Request array_of_requests[]);
void EAR_MPI_Test_enter(MPI_Request *request, int *flag, MPI_Status *status);
void EAR_MPI_Testall_enter(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
void EAR_MPI_Testany_enter(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
void EAR_MPI_Testsome_enter(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
void EAR_MPI_Wait_enter(MPI_Request *request, MPI_Status *status);
void EAR_MPI_Waitall_enter(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
void EAR_MPI_Waitany_enter(int count, MPI_Request *requests, int *index, MPI_Status *status);
void EAR_MPI_Waitsome_enter(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
void EAR_MPI_Win_complete_enter(MPI_Win win);
void EAR_MPI_Win_create_enter(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
void EAR_MPI_Win_fence_enter(int assert, MPI_Win win);
void EAR_MPI_Win_free_enter(MPI_Win *win);
void EAR_MPI_Win_post_enter(MPI_Group group, int assert, MPI_Win win);
void EAR_MPI_Win_start_enter(MPI_Group group, int assert, MPI_Win win);
void EAR_MPI_Win_wait_enter(MPI_Win win);
void EAR_MPI_Iallgather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Iallgatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Iallreduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ialltoall_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ialltoallv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ibarrier_enter(MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ibcast_enter(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Igather_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Igatherv_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ireduce_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Ireduce_scatter_enter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Iscan_enter(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Iscatter_enter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_Iscatterv_enter(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
void EAR_MPI_node_barrier(void);

int main(int argc, char *argv[])
{
	MPI_Allgather(0, 0, 0, 0, 0, 0, 0);
	MPI_Allgatherv(0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Allreduce(0, 0, 0, 0, 0, 0);
	MPI_Alltoall(0, 0, 0, 0, 0, 0, 0);
	MPI_Alltoallv(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Barrier(0);
	MPI_Bcast(0, 0, 0, 0, 0);
	MPI_Bsend(0, 0, 0, 0, 0, 0);
	MPI_Bsend_init(0, 0, 0, 0, 0, 0, 0);
	MPI_Cancel(0);
	MPI_Cart_create(0, 0, 0, 0, 0, 0);
	MPI_Cart_sub(0, 0, 0);
	MPI_Comm_create(0, 0, 0);
	MPI_Comm_dup(0, 0);
	MPI_Comm_free(0);
	MPI_Comm_0(0, 0);
	MPI_Comm_0(0, 0);
	MPI_Comm_split(0, 0, 0, 0);
	MPI_File_close(0);
	MPI_File_read(0, 0, 0, 0, 0);
	MPI_File_read_all(0, 0, 0, 0, 0);
	MPI_File_read_at(0, 0, 0, 0, 0, 0);
	MPI_File_read_at_all(0, 0, 0, 0, 0, 0);
	MPI_File_write(0, 0, 0, 0, 0);
	MPI_File_write_all(0, 0, 0, 0, 0);
	MPI_File_write_at(0, 0, 0, 0, 0, 0);
	MPI_File_write_at_all(0, 0, 0, 0, 0, 0);
	MPI_Finalize();
	MPI_Gather(0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Gatherv(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Get(0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Ibsend(0, 0, 0, 0, 0, 0, 0);
	MPI_Init(0, 0);
	MPI_Init_thread(0, 0, 0, 0);
	MPI_Inte0_create (0, 0, 0, 0, 0, 0);
	MPI_Inte0_merge (0, 0, 0);
	MPI_Iprobe(0, 0, 0, 0, 0);
	MPI_Irecv(0, 0, 0, 0, 0, 0, 0);
	MPI_Irsend(0, 0, 0, 0, 0, 0, 0);
	MPI_Isend(0, 0, 0, 0, 0, 0, 0);
	MPI_Issend(0, 0, 0, 0, 0, 0, 0);
	MPI_Probe(0, 0, 0, 0);
	MPI_Put(0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Recv(0, 0, 0, 0, 0, 0, 0);
	MPI_Recv_init(0, 0, 0, 0, 0, 0, 0);
	MPI_Reduce(0, 0, 0, 0, 0, 0, 0);
	MPI_Reduce_scatter(0, 0, 0, 0, 0, 0);
	MPI_Request_free(0);
	MPI_Request_get_0(0, 0, 0);
	MPI_Rsend(0, 0, 0, 0, 0, 0);
	MPI_Rsend_init(0, 0, 0, 0, 0, 0, 0);
	MPI_Scan(0, 0, 0, 0, 0, 0);
	MPI_Scatter(0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Scatterv(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Send(0, 0, 0, 0, 0, 0);
	MPI_Send_init(0, 0, 0, 0, 0, 0, 0);
	MPI_Sendrecv(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Sendrecv_replace(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MPI_Ssend(0, 0, 0, 0, 0, 0);
	MPI_Ssend_init(0, 0, 0, 0, 0, 0, 0);
	MPI_Start(0);
	MPI_Startall(0, 0);
	MPI_Test(0, 0, 0);
	MPI_Testall(0, 0, 0, 0);
	MPI_Testany(0, 0, 0, 0, 0);
	MPI_Testsome(0, 0, 0, 0, 0);
	MPI_Wait(0, 0);
	MPI_Waitall(0, 0, 0);
	MPI_Waitany(0, 0s, 0, 0);
	MPI_Waitsome(0, 0, 0, 0, 0);
	MPI_Win_complete(0);
	MPI_Win_create(base, 0, 0, 0, 0, 0);
	MPI_Win_fence(0, 0);
	MPI_Win_free(0);
	MPI_Win_post(0, 0, 0);
	MPI_Win_start(0, 0, 0);
	MPI_Win_wait(0);
#if MPI_VERSION >= 3
	MPI_Iallgather(0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Iallgatherv(0, 0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Iallreduce(0, 0, 0, 0, 0, 0, 0);
    MPI_Ialltoall(0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Ialltoallv(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Ibarrier(0, 0);
    MPI_Ibcast(0, 0, 0, 0, 0, 0);
    MPI_Igather(0, 0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Igatherv(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Ireduce(0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Ireduce_scatter(0, 0, 0, 0, 0, 0, 0);
    MPI_Iscan(0, 0, 0, 0, 0, 0, 0);
    MPI_Iscatter(0, 0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Iscatterv(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
	return 0;
}

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgather 1");
	return 0;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Allgatherv 1");
	return 0;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Allreduce 1");
	return 0;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoall 1");
	return 0;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	printf("MPI_Alltoallv 1");
	return 0;
}

int MPI_Barrier(MPI_Comm comm)
{
	printf("MPI_Barrier 1");
	return 0;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	printf("MPI_Bcast 1");
	return 0;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Bsend 1");
	return 0;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Bsend_init 1");
	return 0;
}

int MPI_Cancel(MPI_Request *request)
{
	printf("MPI_Cancel 1");
	return 0;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	printf("MPI_Cart_create 1");
	return 0;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	printf("MPI_Cart_sub 1");
	return 0;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	printf("MPI_Comm_create 1");
	return 0;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	printf("MPI_Comm_dup 1");
	return 0;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	printf("MPI_Comm_free 1");
	return 0;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	printf("MPI_Comm_rank 1");
	return 0;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	printf("MPI_Comm_size 1");
	return 0;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn 1");
	return 0;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	printf("MPI_Comm_spawn_multiple 1");
	return 0;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	printf("MPI_Comm_split 1");
	return 0;
}

int MPI_File_close(MPI_File *fh)
{
	printf("MPI_File_close 1");
	return 0;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read 1");
	return 0;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_all 1");
	return 0;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at 1");
	return 0;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_read_at_all 1");
	return 0;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write 1");
	return 0;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_all 1");
	return 0;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at 1");
	return 0;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	printf("MPI_File_write_at_all 1");
	return 0;
}

int MPI_Finalize(void)
{
	printf("MPI_Finalize 1");
	return 0;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gather 1");
	return 0;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Gatherv 1");
	return 0;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Get 1");
	return 0;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ibsend 1");
	return 0;
}

int MPI_Init(int *argc, char ***argv)
{
	printf("MPI_Init 1");
	return 0;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	printf("MPI_Init_thread 1");
	return 0;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	printf("MPI_Intercomm_create 1");
	return 0;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	printf("MPI_Intercomm_merge 1");
	return 0;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	printf("MPI_Iprobe 1");
	return 0;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irecv 1");
	return 0;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Irsend 1");
	return 0;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Isend 1");
	return 0;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Issend 1");
	return 0;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Probe 1");
	return 0;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	printf("MPI_Put 1");
	return 0;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Recv 1");
	return 0;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Recv_init 1");
	return 0;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	printf("MPI_Reduce 1");
	return 0;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Reduce_scatter 1");
	return 0;
}

int MPI_Request_free(MPI_Request *request)
{
	printf("MPI_Request_free 1");
	return 0;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	printf("MPI_Request_get_status 1");
	return 0;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Rsend 1");
	return 0;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Rsend_init 1");
	return 0;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	printf("MPI_Scan 1");
	return 0;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatter 1");
	return 0;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	printf("MPI_Scatterv 1");
	return 0;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Send 1");
	return 0;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Send_init 1");
	return 0;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv 1");
	return 0;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	printf("MPI_Sendrecv_replace 1");
	return 0;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	printf("MPI_Ssend 1");
	return 0;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	printf("MPI_Ssend_init 1");
	return 0;
}

int MPI_Start(MPI_Request *request)
{
	printf("MPI_Start 1");
	return 0;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	printf("MPI_Startall 1");
	return 0;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	printf("MPI_Test 1");
	return 0;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	printf("MPI_Testall 1");
	return 0;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	printf("MPI_Testany 1");
	return 0;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	printf("MPI_Testsome 1");
	return 0;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	printf("MPI_Wait 1");
	return 0;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitall 1");
	return 0;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	printf("MPI_Waitany 1");
	return 0;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	printf("MPI_Waitsome 1");
	return 0;
}

int MPI_Win_complete(MPI_Win win)
{
	printf("MPI_Win_complete 1");
	return 0;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	printf("MPI_Win_create 1");
	return 0;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	printf("MPI_Win_fence 1");
	return 0;
}

int MPI_Win_free(MPI_Win *win)
{
	printf("MPI_Win_free 1");
	return 0;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_post 1");
	return 0;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	printf("MPI_Win_start 1");
	return 0;
}

int MPI_Win_wait(MPI_Win win)
{
	printf("MPI_Win_wait 1");
	return 0;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgather 1");
    return 0;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallgatherv 1");
    return 0;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iallreduce 1");
    return 0;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoall 1");
    return 0;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ialltoallv 1");
    return 0;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibarrier 1");
    return 0;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ibcast 1");
    return 0;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igather 1");
    return 0;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Igatherv 1");
    return 0;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce 1");
    return 0;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Ireduce_scatter 1");
    return 0;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscan 1");
    return 0;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatter 1");
    return 0;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    printf("MPI_Iscatterv 1");
    return 0;
}
#endif

int main(int argc, char *argv[])
{
	void *sendbuf = 0;

	MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	MPI_Barrier(comm);
	MPI_Bcast(buffer, count, datatype, root, comm);
	MPI_Bsend(buf, count, datatype, dest, tag, comm);
	MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	MPI_Cancel(request);
	MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	MPI_Cart_sub(comm, remain_dims, newcomm);
	MPI_Comm_create(comm, group, newcomm);
	MPI_Comm_dup(comm, newcomm);
	MPI_Comm_free(comm);
	MPI_Comm_rank(comm, rank);
	MPI_Comm_size(comm, size);
	MPI_Comm_split(comm, color, key, newcomm);
	MPI_File_close(fh);
	MPI_File_read(fh, buf, count, datatype, status);
	MPI_File_read_all(fh, buf, count, datatype, status);
	MPI_File_read_at(fh, offset, buf, count, datatype, status);
	MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	MPI_File_write(fh, buf, count, datatype, status);
	MPI_File_write_all(fh, buf, count, datatype, status);
	MPI_File_write_at(fh, offset, buf, count, datatype, status);
	MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	MPI_Finalize();
	MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	MPI_Init(argc, argv);
	MPI_Init_thread(argc, argv, required, provided);
	MPI_Intercomm_create (MPI_Comm local_comm, local_leader, peer_comm, remote_leader, tag, *newintercomm);
	MPI_Intercomm_merge (intercomm, high, newintracomm);
	MPI_Iprobe(source, tag, comm, flag, status);
	MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	MPI_Probe(source, tag, comm, status);
	MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	MPI_Recv(buf, count, datatype, source, tag, comm, status);
	MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	MPI_Request_free(request);
	MPI_Request_get_status(request, flag, status);
	MPI_Rsend(buf, count, datatype, dest, tag, comm);
	MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	MPI_Send(buf, count, datatype, dest, tag, comm);
	MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	MPI_Ssend(buf, count, datatype, dest, tag, comm);
	MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	MPI_Start(request);
	MPI_Startall(count, array_of_requests);
	MPI_Test(request, flag, status);
	MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	MPI_Testany(count, array_of_requests, indx, flag, status);
	MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	MPI_Wait(request, status);
	MPI_Waitall(count, array_of_requests, array_of_statuses);
	MPI_Waitany(count, requests, index, status);
	MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	MPI_Win_complete(win);
	MPI_Win_create(base, size, disp_unit, info, comm, win);
	MPI_Win_fence(assert, win);
	MPI_Win_free(win);
	MPI_Win_post(group, assert, win);
	MPI_Win_start(group, assert, win);
	MPI_Win_wait(win);
#if MPI_VERSION >= 3
	MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    MPI_Ibarrier(comm, request);
    MPI_Ibcast(buffer, count, datatype, root, comm, request);
    MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
#endif
	return 0;
}