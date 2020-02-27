#include <library/mpi_intercept/loader/mpic_defs.h>

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Allgather...............");
	mpic_ear.Allgather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	int res = mpic_mpi.MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	mpic_ear.Allgather_out();
	debug("<< C MPI_Allgather...............");
	return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Allgatherv...............");
	mpic_ear.Allgatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	int res = mpic_mpi.MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	mpic_ear.Allgatherv_out();
	debug("<< C MPI_Allgatherv...............");
	return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Allreduce...............");
	mpic_ear.Allreduce_in(sendbuf, recvbuf, count, datatype, op, comm);
	int res = mpic_mpi.MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	mpic_ear.Allreduce_out();
	debug("<< C MPI_Allreduce...............");
	return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Alltoall...............");
	mpic_ear.Alltoall_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	int res = mpic_mpi.MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	mpic_ear.Alltoall_out();
	debug("<< C MPI_Alltoall...............");
	return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Alltoallv...............");
	mpic_ear.Alltoallv_in(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	int res = mpic_mpi.MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	mpic_ear.Alltoallv_out();
	debug("<< C MPI_Alltoallv...............");
	return res;
}

int MPI_Barrier(MPI_Comm comm)
{
	debug(">> C MPI_Barrier...............");
	mpic_ear.Barrier_in(comm);
	int res = mpic_mpi.MPI_Barrier(comm);
	mpic_ear.Barrier_out();
	debug("<< C MPI_Barrier...............");
	return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Bcast...............");
	mpic_ear.Bcast_in(buffer, count, datatype, root, comm);
	int res = mpic_mpi.MPI_Bcast(buffer, count, datatype, root, comm);
	mpic_ear.Bcast_out();
	debug("<< C MPI_Bcast...............");
	return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Bsend...............");
	mpic_ear.Bsend_in(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Bsend(buf, count, datatype, dest, tag, comm);
	mpic_ear.Bsend_out();
	debug("<< C MPI_Bsend...............");
	return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Bsend_init...............");
	mpic_ear.Bsend_init_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Bsend_init_out();
	debug("<< C MPI_Bsend_init...............");
	return res;
}

int MPI_Cancel(MPI_Request *request)
{
	debug(">> C MPI_Cancel...............");
	mpic_ear.Cancel_in(request);
	int res = mpic_mpi.MPI_Cancel(request);
	mpic_ear.Cancel_out();
	debug("<< C MPI_Cancel...............");
	return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	debug(">> C MPI_Cart_create...............");
	mpic_ear.Cart_create_in(comm_old, ndims, dims, periods, reorder, comm_cart);
	int res = mpic_mpi.MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	mpic_ear.Cart_create_out();
	debug("<< C MPI_Cart_create...............");
	return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	debug(">> C MPI_Cart_sub...............");
	mpic_ear.Cart_sub_in(comm, remain_dims, newcomm);
	int res = mpic_mpi.MPI_Cart_sub(comm, remain_dims, newcomm);
	mpic_ear.Cart_sub_out();
	debug("<< C MPI_Cart_sub...............");
	return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_create...............");
	mpic_ear.Comm_create_in(comm, group, newcomm);
	int res = mpic_mpi.MPI_Comm_create(comm, group, newcomm);
	mpic_ear.Comm_create_out();
	debug("<< C MPI_Comm_create...............");
	return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_dup...............");
	mpic_ear.Comm_dup_in(comm, newcomm);
	int res = mpic_mpi.MPI_Comm_dup(comm, newcomm);
	mpic_ear.Comm_dup_out();
	debug("<< C MPI_Comm_dup...............");
	return res;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	debug(">> C MPI_Comm_free...............");
	mpic_ear.Comm_free_in(comm);
	int res = mpic_mpi.MPI_Comm_free(comm);
	mpic_ear.Comm_free_out();
	debug("<< C MPI_Comm_free...............");
	return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	debug(">> C MPI_Comm_rank...............");
	mpic_ear.Comm_rank_in(comm, rank);
	int res = mpic_mpi.MPI_Comm_rank(comm, rank);
	mpic_ear.Comm_rank_out();
	debug("<< C MPI_Comm_rank...............");
	return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	debug(">> C MPI_Comm_size...............");
	mpic_ear.Comm_size_in(comm, size);
	int res = mpic_mpi.MPI_Comm_size(comm, size);
	mpic_ear.Comm_size_out();
	debug("<< C MPI_Comm_size...............");
	return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	debug(">> C MPI_Comm_spawn...............");
	mpic_ear.Comm_spawn_in(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	int res = mpic_mpi.MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	mpic_ear.Comm_spawn_out();
	debug("<< C MPI_Comm_spawn...............");
	return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	debug(">> C MPI_Comm_spawn_multiple...............");
	mpic_ear.Comm_spawn_multiple_in(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	int res = mpic_mpi.MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	mpic_ear.Comm_spawn_multiple_out();
	debug("<< C MPI_Comm_spawn_multiple...............");
	return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_split...............");
	mpic_ear.Comm_split_in(comm, color, key, newcomm);
	int res = mpic_mpi.MPI_Comm_split(comm, color, key, newcomm);
	mpic_ear.Comm_split_out();
	debug("<< C MPI_Comm_split...............");
	return res;
}

int MPI_File_close(MPI_File *fh)
{
	debug(">> C MPI_File_close...............");
	mpic_ear.File_close_in(fh);
	int res = mpic_mpi.MPI_File_close(fh);
	mpic_ear.File_close_out();
	debug("<< C MPI_File_close...............");
	return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read...............");
	mpic_ear.File_read_in(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read(fh, buf, count, datatype, status);
	mpic_ear.File_read_out();
	debug("<< C MPI_File_read...............");
	return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_all...............");
	mpic_ear.File_read_all_in(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_all(fh, buf, count, datatype, status);
	mpic_ear.File_read_all_out();
	debug("<< C MPI_File_read_all...............");
	return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_at...............");
	mpic_ear.File_read_at_in(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_at(fh, offset, buf, count, datatype, status);
	mpic_ear.File_read_at_out();
	debug("<< C MPI_File_read_at...............");
	return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_at_all...............");
	mpic_ear.File_read_at_all_in(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	mpic_ear.File_read_at_all_out();
	debug("<< C MPI_File_read_at_all...............");
	return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write...............");
	mpic_ear.File_write_in(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write(fh, buf, count, datatype, status);
	mpic_ear.File_write_out();
	debug("<< C MPI_File_write...............");
	return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_all...............");
	mpic_ear.File_write_all_in(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_all(fh, buf, count, datatype, status);
	mpic_ear.File_write_all_out();
	debug("<< C MPI_File_write_all...............");
	return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_at...............");
	mpic_ear.File_write_at_in(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_at(fh, offset, buf, count, datatype, status);
	mpic_ear.File_write_at_out();
	debug("<< C MPI_File_write_at...............");
	return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_at_all...............");
	mpic_ear.File_write_at_all_in(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	mpic_ear.File_write_at_all_out();
	debug("<< C MPI_File_write_at_all...............");
	return res;
}

int MPI_Finalize(void)
{
	debug(">> C MPI_Finalize...............");
	mpic_ear.Finalize_in();
	int res = mpic_mpi.MPI_Finalize();
	mpic_ear.Finalize_out();
	debug("<< C MPI_Finalize...............");
	return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Gather...............");
	mpic_ear.Gather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpic_ear.Gather_out();
	debug("<< C MPI_Gather...............");
	return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Gatherv...............");
	mpic_ear.Gatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	int res = mpic_mpi.MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	mpic_ear.Gatherv_out();
	debug("<< C MPI_Gatherv...............");
	return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	debug(">> C MPI_Get...............");
	mpic_ear.Get_in(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	int res = mpic_mpi.MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	mpic_ear.Get_out();
	debug("<< C MPI_Get...............");
	return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Ibsend...............");
	mpic_ear.Ibsend_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Ibsend_out();
	debug("<< C MPI_Ibsend...............");
	return res;
}

int MPI_Init(int *argc, char ***argv)
{
	debug(">> C MPI_Init...............");
	mpi_c_symbol_load();
	mpic_ear.Init_in(argc, argv);
	int res = mpic_mpi.MPI_Init(argc, argv);
	mpic_ear.Init_out();
	debug("<< C MPI_Init...............");
	return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	debug(">> C MPI_Init_thread...............");
	mpi_c_symbol_load();
	mpic_ear.Init_thread_in(argc, argv, required, provided);
	int res = mpic_mpi.MPI_Init_thread(argc, argv, required, provided);
	mpic_ear.Init_thread_out();
	debug("<< C MPI_Init_thread...............");
	return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	debug(">> C MPI_Intercomm_create...............");
	mpic_ear.Intercomm_create_in(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	int res = mpic_mpi.MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	mpic_ear.Intercomm_create_out();
	debug("<< C MPI_Intercomm_create...............");
	return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	debug(">> C MPI_Intercomm_merge...............");
	mpic_ear.Intercomm_merge_in(intercomm, high, newintracomm);
	int res = mpic_mpi.MPI_Intercomm_merge(intercomm, high, newintracomm);
	mpic_ear.Intercomm_merge_out();
	debug("<< C MPI_Intercomm_merge...............");
	return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Iprobe...............");
	mpic_ear.Iprobe_in(source, tag, comm, flag, status);
	int res = mpic_mpi.MPI_Iprobe(source, tag, comm, flag, status);
	mpic_ear.Iprobe_out();
	debug("<< C MPI_Iprobe...............");
	return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Irecv...............");
	mpic_ear.Irecv_in(buf, count, datatype, source, tag, comm, request);
	int res = mpic_mpi.MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	mpic_ear.Irecv_out();
	debug("<< C MPI_Irecv...............");
	return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Irsend...............");
	mpic_ear.Irsend_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Irsend_out();
	debug("<< C MPI_Irsend...............");
	return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Isend...............");
	mpic_ear.Isend_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Isend_out();
	debug("<< C MPI_Isend...............");
	return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Issend...............");
	mpic_ear.Issend_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Issend_out();
	debug("<< C MPI_Issend...............");
	return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Probe...............");
	mpic_ear.Probe_in(source, tag, comm, status);
	int res = mpic_mpi.MPI_Probe(source, tag, comm, status);
	mpic_ear.Probe_out();
	debug("<< C MPI_Probe...............");
	return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	debug(">> C MPI_Put...............");
	mpic_ear.Put_in(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	int res = mpic_mpi.MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	mpic_ear.Put_out();
	debug("<< C MPI_Put...............");
	return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Recv...............");
	mpic_ear.Recv_in(buf, count, datatype, source, tag, comm, status);
	int res = mpic_mpi.MPI_Recv(buf, count, datatype, source, tag, comm, status);
	mpic_ear.Recv_out();
	debug("<< C MPI_Recv...............");
	return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Recv_init...............");
	mpic_ear.Recv_init_in(buf, count, datatype, source, tag, comm, request);
	int res = mpic_mpi.MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	mpic_ear.Recv_init_out();
	debug("<< C MPI_Recv_init...............");
	return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	debug(">> C MPI_Reduce...............");
	mpic_ear.Reduce_in(sendbuf, recvbuf, count, datatype, op, root, comm);
	int res = mpic_mpi.MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	mpic_ear.Reduce_out();
	debug("<< C MPI_Reduce...............");
	return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Reduce_scatter...............");
	mpic_ear.Reduce_scatter_in(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	int res = mpic_mpi.MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	mpic_ear.Reduce_scatter_out();
	debug("<< C MPI_Reduce_scatter...............");
	return res;
}

int MPI_Request_free(MPI_Request *request)
{
	debug(">> C MPI_Request_free...............");
	mpic_ear.Request_free_in(request);
	int res = mpic_mpi.MPI_Request_free(request);
	mpic_ear.Request_free_out();
	debug("<< C MPI_Request_free...............");
	return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Request_get_status...............");
	mpic_ear.Request_get_status_in(request, flag, status);
	int res = mpic_mpi.MPI_Request_get_status(request, flag, status);
	mpic_ear.Request_get_status_out();
	debug("<< C MPI_Request_get_status...............");
	return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Rsend...............");
	mpic_ear.Rsend_in(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Rsend(buf, count, datatype, dest, tag, comm);
	mpic_ear.Rsend_out();
	debug("<< C MPI_Rsend...............");
	return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Rsend_init...............");
	mpic_ear.Rsend_init_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Rsend_init_out();
	debug("<< C MPI_Rsend_init...............");
	return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Scan...............");
	mpic_ear.Scan_in(sendbuf, recvbuf, count, datatype, op, comm);
	int res = mpic_mpi.MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	mpic_ear.Scan_out();
	debug("<< C MPI_Scan...............");
	return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Scatter...............");
	mpic_ear.Scatter_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpic_ear.Scatter_out();
	debug("<< C MPI_Scatter...............");
	return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Scatterv...............");
	mpic_ear.Scatterv_in(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	mpic_ear.Scatterv_out();
	debug("<< C MPI_Scatterv...............");
	return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Send...............");
	mpic_ear.Send_in(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Send(buf, count, datatype, dest, tag, comm);
	mpic_ear.Send_out();
	debug("<< C MPI_Send...............");
	return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Send_init...............");
	mpic_ear.Send_init_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Send_init_out();
	debug("<< C MPI_Send_init...............");
	return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Sendrecv...............");
	mpic_ear.Sendrecv_in(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	int res = mpic_mpi.MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	mpic_ear.Sendrecv_out();
	debug("<< C MPI_Sendrecv...............");
	return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Sendrecv_replace...............");
	mpic_ear.Sendrecv_replace_in(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	int res = mpic_mpi.MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	mpic_ear.Sendrecv_replace_out();
	debug("<< C MPI_Sendrecv_replace...............");
	return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Ssend...............");
	mpic_ear.Ssend_in(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Ssend(buf, count, datatype, dest, tag, comm);
	mpic_ear.Ssend_out();
	debug("<< C MPI_Ssend...............");
	return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Ssend_init...............");
	mpic_ear.Ssend_init_in(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	mpic_ear.Ssend_init_out();
	debug("<< C MPI_Ssend_init...............");
	return res;
}

int MPI_Start(MPI_Request *request)
{
	debug(">> C MPI_Start...............");
	mpic_ear.Start_in(request);
	int res = mpic_mpi.MPI_Start(request);
	mpic_ear.Start_out();
	debug("<< C MPI_Start...............");
	return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	debug(">> C MPI_Startall...............");
	mpic_ear.Startall_in(count, array_of_requests);
	int res = mpic_mpi.MPI_Startall(count, array_of_requests);
	mpic_ear.Startall_out();
	debug("<< C MPI_Startall...............");
	return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Test...............");
	mpic_ear.Test_in(request, flag, status);
	int res = mpic_mpi.MPI_Test(request, flag, status);
	mpic_ear.Test_out();
	debug("<< C MPI_Test...............");
	return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	debug(">> C MPI_Testall...............");
	mpic_ear.Testall_in(count, array_of_requests, flag, array_of_statuses);
	int res = mpic_mpi.MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	mpic_ear.Testall_out();
	debug("<< C MPI_Testall...............");
	return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Testany...............");
	mpic_ear.Testany_in(count, array_of_requests, indx, flag, status);
	int res = mpic_mpi.MPI_Testany(count, array_of_requests, indx, flag, status);
	mpic_ear.Testany_out();
	debug("<< C MPI_Testany...............");
	return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	debug(">> C MPI_Testsome...............");
	mpic_ear.Testsome_in(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	int res = mpic_mpi.MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	mpic_ear.Testsome_out();
	debug("<< C MPI_Testsome...............");
	return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	debug(">> C MPI_Wait...............");
	mpic_ear.Wait_in(request, status);
	int res = mpic_mpi.MPI_Wait(request, status);
	mpic_ear.Wait_out();
	debug("<< C MPI_Wait...............");
	return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	debug(">> C MPI_Waitall...............");
	mpic_ear.Waitall_in(count, array_of_requests, array_of_statuses);
	int res = mpic_mpi.MPI_Waitall(count, array_of_requests, array_of_statuses);
	mpic_ear.Waitall_out();
	debug("<< C MPI_Waitall...............");
	return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	debug(">> C MPI_Waitany...............");
	mpic_ear.Waitany_in(count, requests, index, status);
	int res = mpic_mpi.MPI_Waitany(count, requests, index, status);
	mpic_ear.Waitany_out();
	debug("<< C MPI_Waitany...............");
	return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	debug(">> C MPI_Waitsome...............");
	mpic_ear.Waitsome_in(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	int res = mpic_mpi.MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	mpic_ear.Waitsome_out();
	debug("<< C MPI_Waitsome...............");
	return res;
}

int MPI_Win_complete(MPI_Win win)
{
	debug(">> C MPI_Win_complete...............");
	mpic_ear.Win_complete_in(win);
	int res = mpic_mpi.MPI_Win_complete(win);
	mpic_ear.Win_complete_out();
	debug("<< C MPI_Win_complete...............");
	return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	debug(">> C MPI_Win_create...............");
	mpic_ear.Win_create_in(base, size, disp_unit, info, comm, win);
	int res = mpic_mpi.MPI_Win_create(base, size, disp_unit, info, comm, win);
	mpic_ear.Win_create_out();
	debug("<< C MPI_Win_create...............");
	return res;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	debug(">> C MPI_Win_fence...............");
	mpic_ear.Win_fence_in(assert, win);
	int res = mpic_mpi.MPI_Win_fence(assert, win);
	mpic_ear.Win_fence_out();
	debug("<< C MPI_Win_fence...............");
	return res;
}

int MPI_Win_free(MPI_Win *win)
{
	debug(">> C MPI_Win_free...............");
	mpic_ear.Win_free_in(win);
	int res = mpic_mpi.MPI_Win_free(win);
	mpic_ear.Win_free_out();
	debug("<< C MPI_Win_free...............");
	return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	debug(">> C MPI_Win_post...............");
	mpic_ear.Win_post_in(group, assert, win);
	int res = mpic_mpi.MPI_Win_post(group, assert, win);
	mpic_ear.Win_post_out();
	debug("<< C MPI_Win_post...............");
	return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	debug(">> C MPI_Win_start...............");
	mpic_ear.Win_start_in(group, assert, win);
	int res = mpic_mpi.MPI_Win_start(group, assert, win);
	mpic_ear.Win_start_out();
	debug("<< C MPI_Win_start...............");
	return res;
}

int MPI_Win_wait(MPI_Win win)
{
	debug(">> C MPI_Win_wait...............");
	mpic_ear.Win_wait_in(win);
	int res = mpic_mpi.MPI_Win_wait(win);
	mpic_ear.Win_wait_out();
	debug("<< C MPI_Win_wait...............");
	return res;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallgather...............");
    mpic_ear.Iallgather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = mpic_mpi.MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    mpic_ear.Iallgather_out();
    debug("<< C MPI_Iallgather...............");
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallgatherv...............");
    mpic_ear.Iallgatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    int res = mpic_mpi.MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    mpic_ear.Iallgatherv_out();
    debug("<< C MPI_Iallgatherv...............");
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallreduce...............");
    mpic_ear.Iallreduce_in(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    mpic_ear.Iallreduce_out();
    debug("<< C MPI_Iallreduce...............");
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ialltoall...............");
    mpic_ear.Ialltoall_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = mpic_mpi.MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    mpic_ear.Ialltoall_out();
    debug("<< C MPI_Ialltoall...............");
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ialltoallv...............");
    mpic_ear.Ialltoallv_in(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    int res = mpic_mpi.MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    mpic_ear.Ialltoallv_out();
    debug("<< C MPI_Ialltoallv...............");
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ibarrier...............");
    mpic_ear.Ibarrier_in(comm, request);
    int res = mpic_mpi.MPI_Ibarrier(comm, request);
    mpic_ear.Ibarrier_out();
    debug("<< C MPI_Ibarrier...............");
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ibcast...............");
    mpic_ear.Ibcast_in(buffer, count, datatype, root, comm, request);
    int res = mpic_mpi.MPI_Ibcast(buffer, count, datatype, root, comm, request);
    mpic_ear.Ibcast_out();
    debug("<< C MPI_Ibcast...............");
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Igather...............");
    mpic_ear.Igather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpic_ear.Igather_out();
    debug("<< C MPI_Igather...............");
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Igatherv...............");
    mpic_ear.Igatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    mpic_ear.Igatherv_out();
    debug("<< C MPI_Igatherv...............");
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ireduce...............");
    mpic_ear.Ireduce_in(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    int res = mpic_mpi.MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    mpic_ear.Ireduce_out();
    debug("<< C MPI_Ireduce...............");
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ireduce_scatter...............");
    mpic_ear.Ireduce_scatter_in(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    mpic_ear.Ireduce_scatter_out();
    debug("<< C MPI_Ireduce_scatter...............");
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscan...............");
    mpic_ear.Iscan_in(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    mpic_ear.Iscan_out();
    debug("<< C MPI_Iscan...............");
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscatter...............");
    mpic_ear.Iscatter_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpic_ear.Iscatter_out();
    debug("<< C MPI_Iscatter...............");
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscatterv...............");
    mpic_ear.Iscatterv_in(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    mpic_ear.Iscatterv_out();
    debug("<< C MPI_Iscatterv...............");
    return res;
}
#endif
