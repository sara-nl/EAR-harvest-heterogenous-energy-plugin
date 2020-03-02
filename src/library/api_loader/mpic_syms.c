/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#include <common/output/debug.h>
#include <library/api_loader/mpic_syms.h>

extern mpic_mpi_t mpic_mpi;
extern mpic_ear_t mpic_ear;
extern int mpic_ear_loaded;

#define IF \
	if (mpic_ear_loaded == 1)

int MPI_Allgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Allgather...............");
	IF mpic_ear.Allgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	int res = mpic_mpi.MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	IF mpic_ear.Allgather_leave();
	debug("<< C MPI_Allgather...............");
	return res;
}

int MPI_Allgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Allgatherv...............");
	IF mpic_ear.Allgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	int res = mpic_mpi.MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	IF mpic_ear.Allgatherv_leave();
	debug("<< C MPI_Allgatherv...............");
	return res;
}

int MPI_Allreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Allreduce...............");
	IF mpic_ear.Allreduce_enter(sendbuf, recvbuf, count, datatype, op, comm);
	int res = mpic_mpi.MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
	IF mpic_ear.Allreduce_leave();
	debug("<< C MPI_Allreduce...............");
	return res;
}

int MPI_Alltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Alltoall...............");
	IF mpic_ear.Alltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	int res = mpic_mpi.MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	IF mpic_ear.Alltoall_leave();
	debug("<< C MPI_Alltoall...............");
	return res;
}

int MPI_Alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	debug(">> C MPI_Alltoallv...............");
	IF mpic_ear.Alltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	int res = mpic_mpi.MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	IF mpic_ear.Alltoallv_leave();
	debug("<< C MPI_Alltoallv...............");
	return res;
}

int MPI_Barrier(MPI_Comm comm)
{
	debug(">> C MPI_Barrier...............");
	IF mpic_ear.Barrier_enter(comm);
	int res = mpic_mpi.MPI_Barrier(comm);
	IF mpic_ear.Barrier_leave();
	debug("<< C MPI_Barrier...............");
	return res;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Bcast...............");
	IF mpic_ear.Bcast_enter(buffer, count, datatype, root, comm);
	int res = mpic_mpi.MPI_Bcast(buffer, count, datatype, root, comm);
	IF mpic_ear.Bcast_leave();
	debug("<< C MPI_Bcast...............");
	return res;
}

int MPI_Bsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Bsend...............");
	IF mpic_ear.Bsend_enter(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Bsend(buf, count, datatype, dest, tag, comm);
	IF mpic_ear.Bsend_leave();
	debug("<< C MPI_Bsend...............");
	return res;
}

int MPI_Bsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Bsend_init...............");
	IF mpic_ear.Bsend_init_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Bsend_init_leave();
	debug("<< C MPI_Bsend_init...............");
	return res;
}

int MPI_Cancel(MPI_Request *request)
{
	debug(">> C MPI_Cancel...............");
	IF mpic_ear.Cancel_enter(request);
	int res = mpic_mpi.MPI_Cancel(request);
	IF mpic_ear.Cancel_leave();
	debug("<< C MPI_Cancel...............");
	return res;
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	debug(">> C MPI_Cart_create...............");
	IF mpic_ear.Cart_create_enter(comm_old, ndims, dims, periods, reorder, comm_cart);
	int res = mpic_mpi.MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
	IF mpic_ear.Cart_create_leave();
	debug("<< C MPI_Cart_create...............");
	return res;
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	debug(">> C MPI_Cart_sub...............");
	IF mpic_ear.Cart_sub_enter(comm, remain_dims, newcomm);
	int res = mpic_mpi.MPI_Cart_sub(comm, remain_dims, newcomm);
	IF mpic_ear.Cart_sub_leave();
	debug("<< C MPI_Cart_sub...............");
	return res;
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_create...............");
	IF mpic_ear.Comm_create_enter(comm, group, newcomm);
	int res = mpic_mpi.MPI_Comm_create(comm, group, newcomm);
	IF mpic_ear.Comm_create_leave();
	debug("<< C MPI_Comm_create...............");
	return res;
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_dup...............");
	IF mpic_ear.Comm_dup_enter(comm, newcomm);
	int res = mpic_mpi.MPI_Comm_dup(comm, newcomm);
	IF mpic_ear.Comm_dup_leave();
	debug("<< C MPI_Comm_dup...............");
	return res;
}

int MPI_Comm_free(MPI_Comm *comm)
{
	debug(">> C MPI_Comm_free...............");
	IF mpic_ear.Comm_free_enter(comm);
	int res = mpic_mpi.MPI_Comm_free(comm);
	IF mpic_ear.Comm_free_leave();
	debug("<< C MPI_Comm_free...............");
	return res;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	debug(">> C MPI_Comm_rank...............");
	IF mpic_ear.Comm_rank_enter(comm, rank);
	int res = mpic_mpi.MPI_Comm_rank(comm, rank);
	IF mpic_ear.Comm_rank_leave();
	debug("<< C MPI_Comm_rank...............");
	return res;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	debug(">> C MPI_Comm_size...............");
	IF mpic_ear.Comm_size_enter(comm, size);
	int res = mpic_mpi.MPI_Comm_size(comm, size);
	IF mpic_ear.Comm_size_leave();
	debug("<< C MPI_Comm_size...............");
	return res;
}

int MPI_Comm_spawn(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	debug(">> C MPI_Comm_spawn...............");
	IF mpic_ear.Comm_spawn_enter(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	int res = mpic_mpi.MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
	IF mpic_ear.Comm_spawn_leave();
	debug("<< C MPI_Comm_spawn...............");
	return res;
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	debug(">> C MPI_Comm_spawn_multiple...............");
	IF mpic_ear.Comm_spawn_multiple_enter(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	int res = mpic_mpi.MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
	IF mpic_ear.Comm_spawn_multiple_leave();
	debug("<< C MPI_Comm_spawn_multiple...............");
	return res;
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	debug(">> C MPI_Comm_split...............");
	IF mpic_ear.Comm_split_enter(comm, color, key, newcomm);
	int res = mpic_mpi.MPI_Comm_split(comm, color, key, newcomm);
	IF mpic_ear.Comm_split_leave();
	debug("<< C MPI_Comm_split...............");
	return res;
}

int MPI_File_close(MPI_File *fh)
{
	debug(">> C MPI_File_close...............");
	IF mpic_ear.File_close_enter(fh);
	int res = mpic_mpi.MPI_File_close(fh);
	IF mpic_ear.File_close_leave();
	debug("<< C MPI_File_close...............");
	return res;
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read...............");
	IF mpic_ear.File_read_enter(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read(fh, buf, count, datatype, status);
	IF mpic_ear.File_read_leave();
	debug("<< C MPI_File_read...............");
	return res;
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_all...............");
	IF mpic_ear.File_read_all_enter(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_all(fh, buf, count, datatype, status);
	IF mpic_ear.File_read_all_leave();
	debug("<< C MPI_File_read_all...............");
	return res;
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_at...............");
	IF mpic_ear.File_read_at_enter(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_at(fh, offset, buf, count, datatype, status);
	IF mpic_ear.File_read_at_leave();
	debug("<< C MPI_File_read_at...............");
	return res;
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_read_at_all...............");
	IF mpic_ear.File_read_at_all_enter(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
	IF mpic_ear.File_read_at_all_leave();
	debug("<< C MPI_File_read_at_all...............");
	return res;
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write...............");
	IF mpic_ear.File_write_enter(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write(fh, buf, count, datatype, status);
	IF mpic_ear.File_write_leave();
	debug("<< C MPI_File_write...............");
	return res;
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_all...............");
	IF mpic_ear.File_write_all_enter(fh, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_all(fh, buf, count, datatype, status);
	IF mpic_ear.File_write_all_leave();
	debug("<< C MPI_File_write_all...............");
	return res;
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_at...............");
	IF mpic_ear.File_write_at_enter(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_at(fh, offset, buf, count, datatype, status);
	IF mpic_ear.File_write_at_leave();
	debug("<< C MPI_File_write_at...............");
	return res;
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	debug(">> C MPI_File_write_at_all...............");
	IF mpic_ear.File_write_at_all_enter(fh, offset, buf, count, datatype, status);
	int res = mpic_mpi.MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
	IF mpic_ear.File_write_at_all_leave();
	debug("<< C MPI_File_write_at_all...............");
	return res;
}

int MPI_Finalize(void)
{
	debug(">> C MPI_Finalize...............");
	IF mpic_ear.Finalize_enter();
	int res = mpic_mpi.MPI_Finalize();
	IF mpic_ear.Finalize_leave();
	debug("<< C MPI_Finalize...............");
	return res;
}

int MPI_Gather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Gather...............");
	IF mpic_ear.Gather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	IF mpic_ear.Gather_leave();
	debug("<< C MPI_Gather...............");
	return res;
}

int MPI_Gatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Gatherv...............");
	IF mpic_ear.Gatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	int res = mpic_mpi.MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	IF mpic_ear.Gatherv_leave();
	debug("<< C MPI_Gatherv...............");
	return res;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	debug(">> C MPI_Get...............");
	IF mpic_ear.Get_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	int res = mpic_mpi.MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	IF mpic_ear.Get_leave();
	debug("<< C MPI_Get...............");
	return res;
}

int MPI_Ibsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Ibsend...............");
	IF mpic_ear.Ibsend_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Ibsend_leave();
	debug("<< C MPI_Ibsend...............");
	return res;
}

int MPI_Init(int *argc, char ***argv)
{
	debug(">> C MPI_Init...............");
	mpi_c_symbol_load();
	IF mpic_ear.Init_enter(argc, argv);
	int res = mpic_mpi.MPI_Init(argc, argv);
	IF mpic_ear.Init_leave();
	debug("<< C MPI_Init...............");
	return res;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	debug(">> C MPI_Init_thread...............");
	mpi_c_symbol_load();
	IF mpic_ear.Init_thread_enter(argc, argv, required, provided);
	int res = mpic_mpi.MPI_Init_thread(argc, argv, required, provided);
	IF mpic_ear.Init_thread_leave();
	debug("<< C MPI_Init_thread...............");
	return res;
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	debug(">> C MPI_Intercomm_create...............");
	IF mpic_ear.Intercomm_create_enter(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	int res = mpic_mpi.MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
	IF mpic_ear.Intercomm_create_leave();
	debug("<< C MPI_Intercomm_create...............");
	return res;
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	debug(">> C MPI_Intercomm_merge...............");
	IF mpic_ear.Intercomm_merge_enter(intercomm, high, newintracomm);
	int res = mpic_mpi.MPI_Intercomm_merge(intercomm, high, newintracomm);
	IF mpic_ear.Intercomm_merge_leave();
	debug("<< C MPI_Intercomm_merge...............");
	return res;
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Iprobe...............");
	IF mpic_ear.Iprobe_enter(source, tag, comm, flag, status);
	int res = mpic_mpi.MPI_Iprobe(source, tag, comm, flag, status);
	IF mpic_ear.Iprobe_leave();
	debug("<< C MPI_Iprobe...............");
	return res;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Irecv...............");
	IF mpic_ear.Irecv_enter(buf, count, datatype, source, tag, comm, request);
	int res = mpic_mpi.MPI_Irecv(buf, count, datatype, source, tag, comm, request);
	IF mpic_ear.Irecv_leave();
	debug("<< C MPI_Irecv...............");
	return res;
}

int MPI_Irsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Irsend...............");
	IF mpic_ear.Irsend_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Irsend_leave();
	debug("<< C MPI_Irsend...............");
	return res;
}

int MPI_Isend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Isend...............");
	IF mpic_ear.Isend_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Isend(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Isend_leave();
	debug("<< C MPI_Isend...............");
	return res;
}

int MPI_Issend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Issend...............");
	IF mpic_ear.Issend_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Issend(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Issend_leave();
	debug("<< C MPI_Issend...............");
	return res;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Probe...............");
	IF mpic_ear.Probe_enter(source, tag, comm, status);
	int res = mpic_mpi.MPI_Probe(source, tag, comm, status);
	IF mpic_ear.Probe_leave();
	debug("<< C MPI_Probe...............");
	return res;
}

int MPI_Put(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	debug(">> C MPI_Put...............");
	IF mpic_ear.Put_enter(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	int res = mpic_mpi.MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
	IF mpic_ear.Put_leave();
	debug("<< C MPI_Put...............");
	return res;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Recv...............");
	IF mpic_ear.Recv_enter(buf, count, datatype, source, tag, comm, status);
	int res = mpic_mpi.MPI_Recv(buf, count, datatype, source, tag, comm, status);
	IF mpic_ear.Recv_leave();
	debug("<< C MPI_Recv...............");
	return res;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Recv_init...............");
	IF mpic_ear.Recv_init_enter(buf, count, datatype, source, tag, comm, request);
	int res = mpic_mpi.MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
	IF mpic_ear.Recv_init_leave();
	debug("<< C MPI_Recv_init...............");
	return res;
}

int MPI_Reduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	debug(">> C MPI_Reduce...............");
	IF mpic_ear.Reduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm);
	int res = mpic_mpi.MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	IF mpic_ear.Reduce_leave();
	debug("<< C MPI_Reduce...............");
	return res;
}

int MPI_Reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Reduce_scatter...............");
	IF mpic_ear.Reduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	int res = mpic_mpi.MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
	IF mpic_ear.Reduce_scatter_leave();
	debug("<< C MPI_Reduce_scatter...............");
	return res;
}

int MPI_Request_free(MPI_Request *request)
{
	debug(">> C MPI_Request_free...............");
	IF mpic_ear.Request_free_enter(request);
	int res = mpic_mpi.MPI_Request_free(request);
	IF mpic_ear.Request_free_leave();
	debug("<< C MPI_Request_free...............");
	return res;
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Request_get_status...............");
	IF mpic_ear.Request_get_status_enter(request, flag, status);
	int res = mpic_mpi.MPI_Request_get_status(request, flag, status);
	IF mpic_ear.Request_get_status_leave();
	debug("<< C MPI_Request_get_status...............");
	return res;
}

int MPI_Rsend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Rsend...............");
	IF mpic_ear.Rsend_enter(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Rsend(buf, count, datatype, dest, tag, comm);
	IF mpic_ear.Rsend_leave();
	debug("<< C MPI_Rsend...............");
	return res;
}

int MPI_Rsend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Rsend_init...............");
	IF mpic_ear.Rsend_init_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Rsend_init_leave();
	debug("<< C MPI_Rsend_init...............");
	return res;
}

int MPI_Scan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	debug(">> C MPI_Scan...............");
	IF mpic_ear.Scan_enter(sendbuf, recvbuf, count, datatype, op, comm);
	int res = mpic_mpi.MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
	IF mpic_ear.Scan_leave();
	debug("<< C MPI_Scan...............");
	return res;
}

int MPI_Scatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Scatter...............");
	IF mpic_ear.Scatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
	IF mpic_ear.Scatter_leave();
	debug("<< C MPI_Scatter...............");
	return res;
}

int MPI_Scatterv(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	debug(">> C MPI_Scatterv...............");
	IF mpic_ear.Scatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	int res = mpic_mpi.MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	IF mpic_ear.Scatterv_leave();
	debug("<< C MPI_Scatterv...............");
	return res;
}

int MPI_Send(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Send...............");
	IF mpic_ear.Send_enter(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Send(buf, count, datatype, dest, tag, comm);
	IF mpic_ear.Send_leave();
	debug("<< C MPI_Send...............");
	return res;
}

int MPI_Send_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Send_init...............");
	IF mpic_ear.Send_init_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Send_init_leave();
	debug("<< C MPI_Send_init...............");
	return res;
}

int MPI_Sendrecv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Sendrecv...............");
	IF mpic_ear.Sendrecv_enter(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	int res = mpic_mpi.MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
	IF mpic_ear.Sendrecv_leave();
	debug("<< C MPI_Sendrecv...............");
	return res;
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	debug(">> C MPI_Sendrecv_replace...............");
	IF mpic_ear.Sendrecv_replace_enter(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	int res = mpic_mpi.MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
	IF mpic_ear.Sendrecv_replace_leave();
	debug("<< C MPI_Sendrecv_replace...............");
	return res;
}

int MPI_Ssend(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	debug(">> C MPI_Ssend...............");
	IF mpic_ear.Ssend_enter(buf, count, datatype, dest, tag, comm);
	int res = mpic_mpi.MPI_Ssend(buf, count, datatype, dest, tag, comm);
	IF mpic_ear.Ssend_leave();
	debug("<< C MPI_Ssend...............");
	return res;
}

int MPI_Ssend_init(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	debug(">> C MPI_Ssend_init...............");
	IF mpic_ear.Ssend_init_enter(buf, count, datatype, dest, tag, comm, request);
	int res = mpic_mpi.MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
	IF mpic_ear.Ssend_init_leave();
	debug("<< C MPI_Ssend_init...............");
	return res;
}

int MPI_Start(MPI_Request *request)
{
	debug(">> C MPI_Start...............");
	IF mpic_ear.Start_enter(request);
	int res = mpic_mpi.MPI_Start(request);
	IF mpic_ear.Start_leave();
	debug("<< C MPI_Start...............");
	return res;
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	debug(">> C MPI_Startall...............");
	IF mpic_ear.Startall_enter(count, array_of_requests);
	int res = mpic_mpi.MPI_Startall(count, array_of_requests);
	IF mpic_ear.Startall_leave();
	debug("<< C MPI_Startall...............");
	return res;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Test...............");
	IF mpic_ear.Test_enter(request, flag, status);
	int res = mpic_mpi.MPI_Test(request, flag, status);
	IF mpic_ear.Test_leave();
	debug("<< C MPI_Test...............");
	return res;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	debug(">> C MPI_Testall...............");
	IF mpic_ear.Testall_enter(count, array_of_requests, flag, array_of_statuses);
	int res = mpic_mpi.MPI_Testall(count, array_of_requests, flag, array_of_statuses);
	IF mpic_ear.Testall_leave();
	debug("<< C MPI_Testall...............");
	return res;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	debug(">> C MPI_Testany...............");
	IF mpic_ear.Testany_enter(count, array_of_requests, indx, flag, status);
	int res = mpic_mpi.MPI_Testany(count, array_of_requests, indx, flag, status);
	IF mpic_ear.Testany_leave();
	debug("<< C MPI_Testany...............");
	return res;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	debug(">> C MPI_Testsome...............");
	IF mpic_ear.Testsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	int res = mpic_mpi.MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	IF mpic_ear.Testsome_leave();
	debug("<< C MPI_Testsome...............");
	return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	debug(">> C MPI_Wait...............");
	IF mpic_ear.Wait_enter(request, status);
	int res = mpic_mpi.MPI_Wait(request, status);
	IF mpic_ear.Wait_leave();
	debug("<< C MPI_Wait...............");
	return res;
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	debug(">> C MPI_Waitall...............");
	IF mpic_ear.Waitall_enter(count, array_of_requests, array_of_statuses);
	int res = mpic_mpi.MPI_Waitall(count, array_of_requests, array_of_statuses);
	IF mpic_ear.Waitall_leave();
	debug("<< C MPI_Waitall...............");
	return res;
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	debug(">> C MPI_Waitany...............");
	IF mpic_ear.Waitany_enter(count, requests, index, status);
	int res = mpic_mpi.MPI_Waitany(count, requests, index, status);
	IF mpic_ear.Waitany_leave();
	debug("<< C MPI_Waitany...............");
	return res;
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	debug(">> C MPI_Waitsome...............");
	IF mpic_ear.Waitsome_enter(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	int res = mpic_mpi.MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
	IF mpic_ear.Waitsome_leave();
	debug("<< C MPI_Waitsome...............");
	return res;
}

int MPI_Win_complete(MPI_Win win)
{
	debug(">> C MPI_Win_complete...............");
	IF mpic_ear.Win_complete_enter(win);
	int res = mpic_mpi.MPI_Win_complete(win);
	IF mpic_ear.Win_complete_leave();
	debug("<< C MPI_Win_complete...............");
	return res;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	debug(">> C MPI_Win_create...............");
	IF mpic_ear.Win_create_enter(base, size, disp_unit, info, comm, win);
	int res = mpic_mpi.MPI_Win_create(base, size, disp_unit, info, comm, win);
	IF mpic_ear.Win_create_leave();
	debug("<< C MPI_Win_create...............");
	return res;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	debug(">> C MPI_Win_fence...............");
	IF mpic_ear.Win_fence_enter(assert, win);
	int res = mpic_mpi.MPI_Win_fence(assert, win);
	IF mpic_ear.Win_fence_leave();
	debug("<< C MPI_Win_fence...............");
	return res;
}

int MPI_Win_free(MPI_Win *win)
{
	debug(">> C MPI_Win_free...............");
	IF mpic_ear.Win_free_enter(win);
	int res = mpic_mpi.MPI_Win_free(win);
	IF mpic_ear.Win_free_leave();
	debug("<< C MPI_Win_free...............");
	return res;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	debug(">> C MPI_Win_post...............");
	IF mpic_ear.Win_post_enter(group, assert, win);
	int res = mpic_mpi.MPI_Win_post(group, assert, win);
	IF mpic_ear.Win_post_leave();
	debug("<< C MPI_Win_post...............");
	return res;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	debug(">> C MPI_Win_start...............");
	IF mpic_ear.Win_start_enter(group, assert, win);
	int res = mpic_mpi.MPI_Win_start(group, assert, win);
	IF mpic_ear.Win_start_leave();
	debug("<< C MPI_Win_start...............");
	return res;
}

int MPI_Win_wait(MPI_Win win)
{
	debug(">> C MPI_Win_wait...............");
	IF mpic_ear.Win_wait_enter(win);
	int res = mpic_mpi.MPI_Win_wait(win);
	IF mpic_ear.Win_wait_leave();
	debug("<< C MPI_Win_wait...............");
	return res;
}

#if MPI_VERSION >= 3
int MPI_Iallgather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallgather...............");
    IF mpic_ear.Iallgather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = mpic_mpi.MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    IF mpic_ear.Iallgather_leave();
    debug("<< C MPI_Iallgather...............");
    return res;
}

int MPI_Iallgatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallgatherv...............");
    IF mpic_ear.Iallgatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    int res = mpic_mpi.MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    IF mpic_ear.Iallgatherv_leave();
    debug("<< C MPI_Iallgatherv...............");
    return res;
}

int MPI_Iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iallreduce...............");
    IF mpic_ear.Iallreduce_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    IF mpic_ear.Iallreduce_leave();
    debug("<< C MPI_Iallreduce...............");
    return res;
}

int MPI_Ialltoall(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ialltoall...............");
    IF mpic_ear.Ialltoall_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    int res = mpic_mpi.MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    IF mpic_ear.Ialltoall_leave();
    debug("<< C MPI_Ialltoall...............");
    return res;
}

int MPI_Ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ialltoallv...............");
    IF mpic_ear.Ialltoallv_enter(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    int res = mpic_mpi.MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    IF mpic_ear.Ialltoallv_leave();
    debug("<< C MPI_Ialltoallv...............");
    return res;
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ibarrier...............");
    IF mpic_ear.Ibarrier_enter(comm, request);
    int res = mpic_mpi.MPI_Ibarrier(comm, request);
    IF mpic_ear.Ibarrier_leave();
    debug("<< C MPI_Ibarrier...............");
    return res;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ibcast...............");
    IF mpic_ear.Ibcast_enter(buffer, count, datatype, root, comm, request);
    int res = mpic_mpi.MPI_Ibcast(buffer, count, datatype, root, comm, request);
    IF mpic_ear.Ibcast_leave();
    debug("<< C MPI_Ibcast...............");
    return res;
}

int MPI_Igather(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Igather...............");
    IF mpic_ear.Igather_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    IF mpic_ear.Igather_leave();
    debug("<< C MPI_Igather...............");
    return res;
}

int MPI_Igatherv(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Igatherv...............");
    IF mpic_ear.Igatherv_enter(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    IF mpic_ear.Igatherv_leave();
    debug("<< C MPI_Igatherv...............");
    return res;
}

int MPI_Ireduce(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ireduce...............");
    IF mpic_ear.Ireduce_enter(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    int res = mpic_mpi.MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    IF mpic_ear.Ireduce_leave();
    debug("<< C MPI_Ireduce...............");
    return res;
}

int MPI_Ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Ireduce_scatter...............");
    IF mpic_ear.Ireduce_scatter_enter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    IF mpic_ear.Ireduce_scatter_leave();
    debug("<< C MPI_Ireduce_scatter...............");
    return res;
}

int MPI_Iscan(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscan...............");
    IF mpic_ear.Iscan_enter(sendbuf, recvbuf, count, datatype, op, comm, request);
    int res = mpic_mpi.MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    IF mpic_ear.Iscan_leave();
    debug("<< C MPI_Iscan...............");
    return res;
}

int MPI_Iscatter(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscatter...............");
    IF mpic_ear.Iscatter_enter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    IF mpic_ear.Iscatter_leave();
    debug("<< C MPI_Iscatter...............");
    return res;
}

int MPI_Iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
    debug(">> C MPI_Iscatterv...............");
    IF mpic_ear.Iscatterv_enter(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    int res = mpic_mpi.MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    IF mpic_ear.Iscatterv_leave();
    debug("<< C MPI_Iscatterv...............");
    return res;
}
#endif
