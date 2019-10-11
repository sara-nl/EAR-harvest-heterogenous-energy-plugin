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

#include <common/symplug.h>
#include <common/output/debug.h>
#include <library/mpi_intercept/MPI_intercept_new.h>
#include <library/mpi_intercept/MPI_intercept_empty.h>

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

mpi_syms_t mpi_syms = {
	.MPI_Allgather = MPI_Allgather_empty,
	.MPI_Allgatherv = MPI_Allgatherv_empty,
	.MPI_Allreduce = MPI_Allreduce_empty,
	.MPI_Alltoall = MPI_Alltoall_empty,
	.MPI_Alltoallv = MPI_Alltoallv_empty,
	.MPI_Barrier = MPI_Barrier_empty,
	.MPI_Bcast = MPI_Bcast_empty,
	.MPI_Bsend = MPI_Bsend_empty,
	.MPI_Bsend_init = MPI_Bsend_init_empty,
	.MPI_Cancel = MPI_Cancel_empty,
	.MPI_Cart_create = MPI_Cart_create_empty,
	.MPI_Cart_sub = MPI_Cart_sub_empty,
	.MPI_Comm_create = MPI_Comm_create_empty,
	.MPI_Comm_dup = MPI_Comm_dup_empty,
	.MPI_Comm_free = MPI_Comm_free_empty,
	.MPI_Comm_rank = MPI_Comm_rank_empty,
	.MPI_Comm_size = MPI_Comm_size_empty,
	.MPI_Comm_spawn = MPI_Comm_spawn_empty,
	.MPI_Comm_spawn_multiple = MPI_Comm_spawn_multiple_empty,
	.MPI_Comm_split = MPI_Comm_split_empty,
	.MPI_File_close = MPI_File_close_empty,
	.MPI_File_read = MPI_File_read_empty,
	.MPI_File_read_all = MPI_File_read_all_empty,
	.MPI_File_read_at = MPI_File_read_at_empty,
	.MPI_File_read_at_all = MPI_File_read_at_all_empty,
	.MPI_File_write = MPI_File_write_empty,
	.MPI_File_write_all = MPI_File_write_all_empty,
	.MPI_File_write_at = MPI_File_write_at_empty,
	.MPI_File_write_at_all = MPI_File_write_at_all_empty,
	.MPI_Finalize = MPI_Finalize_empty,
	.MPI_Gather = MPI_Gather_empty,
	.MPI_Gatherv = MPI_Gatherv_empty,
	.MPI_Get = MPI_Get_empty,
	.MPI_Ibsend = MPI_Ibsend_empty,
	.MPI_Init = MPI_Init_empty,
	.MPI_Init_thread = MPI_Init_thread_empty,
	.MPI_Intercomm_create = MPI_Intercomm_create_empty,
	.MPI_Intercomm_merge = MPI_Intercomm_merge_empty,
	.MPI_Iprobe = MPI_Iprobe_empty,
	.MPI_Irecv = MPI_Irecv_empty,
	.MPI_Irsend = MPI_Irsend_empty,
	.MPI_Isend = MPI_Isend_empty,
	.MPI_Issend = MPI_Issend_empty,
	.MPI_Probe = MPI_Probe_empty,
	.MPI_Put = MPI_Put_empty,
	.MPI_Recv = MPI_Recv_empty,
	.MPI_Recv_init = MPI_Recv_init_empty,
	.MPI_Reduce = MPI_Reduce_empty,
	.MPI_Reduce_scatter = MPI_Reduce_scatter_empty,
	.MPI_Request_free = MPI_Request_free_empty,
	.MPI_Request_get_status = MPI_Request_get_status_empty,
	.MPI_Rsend = MPI_Rsend_empty,
	.MPI_Rsend_init = MPI_Rsend_init_empty,
	.MPI_Scan = MPI_Scan_empty,
	.MPI_Scatter = MPI_Scatter_empty,
	.MPI_Scatterv = MPI_Scatterv_empty,
	.MPI_Send = MPI_Send_empty,
	.MPI_Send_init = MPI_Send_init_empty,
	.MPI_Sendrecv = MPI_Sendrecv_empty,
	.MPI_Sendrecv_replace = MPI_Sendrecv_replace_empty,
	.MPI_Ssend = MPI_Ssend_empty,
	.MPI_Ssend_init = MPI_Ssend_init_empty,
	.MPI_Start = MPI_Start_empty,
	.MPI_Startall = MPI_Startall_empty,
	.MPI_Test = MPI_Test_empty,
	.MPI_Testall = MPI_Testall_empty,
	.MPI_Testany = MPI_Testany_empty,
	.MPI_Testsome = MPI_Testsome_empty,
	.MPI_Wait = MPI_Wait_empty,
	.MPI_Waitall = MPI_Waitall_empty,
	.MPI_Waitany = MPI_Waitany_empty,
	.MPI_Waitsome = MPI_Waitsome_empty,
	.MPI_Win_complete = MPI_Win_complete_empty,
	.MPI_Win_create = MPI_Win_create_empty,
	.MPI_Win_fence = MPI_Win_fence_empty,
	.MPI_Win_free = MPI_Win_free_empty,
	.MPI_Win_post = MPI_Win_post_empty,
	.MPI_Win_start = MPI_Win_start_empty,
	.MPI_Win_wait = MPI_Win_wait_empty
#if MPI_VERSION >= 3
	,
	.MPI_Iallgather = MPI_Iallgather_empty,
	.MPI_Iallgatherv = MPI_Iallgatherv_empty,
	.MPI_Iallreduce = MPI_Iallreduce_empty,
	.MPI_Ialltoall = MPI_Ialltoall_empty,
	.MPI_Ialltoallv = MPI_Ialltoallv_empty,
	.MPI_Ibarrier = MPI_Ibarrier_empty,
	.MPI_Ibcast = MPI_Ibcast_empty,
	.MPI_Igather = MPI_Igather_empty,
	.MPI_Igatherv = MPI_Igatherv_empty,
	.MPI_Ireduce = MPI_Ireduce_empty,
	.MPI_Ireduce_scatter = MPI_Ireduce_scatter_empty,
	.MPI_Iscan = MPI_Iscan_empty,
	.MPI_Iscatter = MPI_Iscatter_empty,
	.MPI_Iscatterv = MPI_Iscatterv_empty
#endif
	,
};

int MPI_Symbol_load()
{
	debug("MPI_Symbol_load");

	// Looking for next symbols
	int syms_n = (int) (sizeof(mpi_names) / sizeof(mpi_names[0]));
	symplug_join(RTLD_NEXT, (void *) &mpi_syms, mpi_names, syms_n);

	return 0;
}

int MPI_Allgather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
}

int MPI_Allgatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
}

int MPI_Allreduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
}

int MPI_Alltoall_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
}

int MPI_Alltoallv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
}

int MPI_Barrier_empty(MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Barrier(comm);
}

int MPI_Bcast_empty(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Bcast(buffer, count, datatype, root, comm);
}

int MPI_Bsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Bsend(buf, count, datatype, dest, tag, comm);
}

int MPI_Bsend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Cancel_empty(MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Cancel(request);
}

int MPI_Cart_create_empty(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
}

int MPI_Cart_sub_empty(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Cart_sub(comm, remain_dims, newcomm);
}

int MPI_Comm_create_empty(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_create(comm, group, newcomm);
}

int MPI_Comm_dup_empty(MPI_Comm comm, MPI_Comm *newcomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_dup(comm, newcomm);
}

int MPI_Comm_free_empty(MPI_Comm *comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_free(comm);
}

int MPI_Comm_rank_empty(MPI_Comm comm, int *rank)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_rank(comm, rank);
}

int MPI_Comm_size_empty(MPI_Comm comm, int *size)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_size(comm, size);
}

int MPI_Comm_spawn_empty(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
}

int MPI_Comm_spawn_multiple_empty(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
}

int MPI_Comm_split_empty(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Comm_split(comm, color, key, newcomm);
}

int MPI_File_close_empty(MPI_File *fh)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_close(fh);
}

int MPI_File_read_empty(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_read(fh, buf, count, datatype, status);
}

int MPI_File_read_all_empty(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_read_all(fh, buf, count, datatype, status);
}

int MPI_File_read_at_empty(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_read_at(fh, offset, buf, count, datatype, status);
}

int MPI_File_read_at_all_empty(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
}

int MPI_File_write_empty(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_write(fh, buf, count, datatype, status);
}

int MPI_File_write_all_empty(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_write_all(fh, buf, count, datatype, status);
}

int MPI_File_write_at_empty(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_write_at(fh, offset, buf, count, datatype, status);
}

int MPI_File_write_at_all_empty(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
}

int MPI_Finalize_empty(void)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Finalize();
}

int MPI_Gather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
}

int MPI_Gatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
}

int MPI_Get_empty(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
}

int MPI_Ibsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Init_empty(int *argc, char ***argv)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Init(argc, argv);
}

int MPI_Init_thread_empty(int *argc, char ***argv, int required, int *provided)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Init_thread(argc, argv, required, provided);
}

int MPI_Intercomm_create_empty(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
}

int MPI_Intercomm_merge_empty(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Intercomm_merge(intercomm, high, newintracomm);
}

int MPI_Iprobe_empty(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iprobe(source, tag, comm, flag, status);
}

int MPI_Irecv_empty(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Irecv(buf, count, datatype, source, tag, comm, request);
}

int MPI_Irsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Irsend(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Isend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Isend(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Issend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Issend(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Probe_empty(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Probe(source, tag, comm, status);
}

int MPI_Put_empty(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
}

int MPI_Recv_empty(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Recv(buf, count, datatype, source, tag, comm, status);
}

int MPI_Recv_init_empty(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Recv_init(buf, count, datatype, source, tag, comm, request);
}

int MPI_Reduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
}

int MPI_Reduce_scatter_empty(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
}

int MPI_Request_free_empty(MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Request_free(request);
}

int MPI_Request_get_status_empty(MPI_Request request, int *flag, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Request_get_status(request, flag, status);
}

int MPI_Rsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Rsend(buf, count, datatype, dest, tag, comm);
}

int MPI_Rsend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Scan_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
}

int MPI_Scatter_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
}

int MPI_Scatterv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
}

int MPI_Send_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Send(buf, count, datatype, dest, tag, comm);
}

int MPI_Send_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Send_init(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Sendrecv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}

int MPI_Sendrecv_replace_empty(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
}

int MPI_Ssend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ssend(buf, count, datatype, dest, tag, comm);
}

int MPI_Ssend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
}

int MPI_Start_empty(MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Start(request);
}

int MPI_Startall_empty(int count, MPI_Request array_of_requests[])
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Startall(count, array_of_requests);
}

int MPI_Test_empty(MPI_Request *request, int *flag, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Test(request, flag, status);
}

int MPI_Testall_empty(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Testall(count, array_of_requests, flag, array_of_statuses);
}

int MPI_Testany_empty(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Testany(count, array_of_requests, indx, flag, status);
}

int MPI_Testsome_empty(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
}

int MPI_Wait_empty(MPI_Request *request, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Wait(request, status);
}

int MPI_Waitall_empty(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Waitall(count, array_of_requests, array_of_statuses);
}

int MPI_Waitany_empty(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Waitany(count, requests, index, status);
}

int MPI_Waitsome_empty(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
}

int MPI_Win_complete_empty(MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_complete(win);
}

int MPI_Win_create_empty(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_create(base, size, disp_unit, info, comm, win);
}

int MPI_Win_fence_empty(int assert, MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_fence(assert, win);
}

int MPI_Win_free_empty(MPI_Win *win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_free(win);
}

int MPI_Win_post_empty(MPI_Group group, int assert, MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_post(group, assert, win);
}

int MPI_Win_start_empty(MPI_Group group, int assert, MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_start(group, assert, win);
}

int MPI_Win_wait_empty(MPI_Win win)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Win_wait(win);
}

#if MPI_VERSION >= 3
int MPI_Iallgather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
}

int MPI_Iallgatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
}

int MPI_Iallreduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
}

int MPI_Ialltoall_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
}

int MPI_Ialltoallv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
}

int MPI_Ibarrier_empty(MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ibarrier(comm, request);
}

int MPI_Ibcast_empty(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ibcast(buffer, count, datatype, root, comm, request);
}

int MPI_Igather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
}

int MPI_Igatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
}

int MPI_Ireduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
}

int MPI_Ireduce_scatter_empty(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
}

int MPI_Iscan_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
}

int MPI_Iscatter_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
}

int MPI_Iscatterv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	MPI_Symbol_load();
	return mpi_syms.MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
}
#endif
