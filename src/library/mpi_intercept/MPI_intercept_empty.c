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
	return MPI_Symbol_load();
}

int MPI_Allgatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Allreduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Alltoall_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Alltoallv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *sdispls, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Barrier(MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Bsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Bsend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Cancel(MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Cart_create(MPI_Comm comm_old, int ndims, MPI3_CONST int dims[], MPI3_CONST int periods[], int reorder, MPI_Comm *comm_cart)
{
	return MPI_Symbol_load();
}

int MPI_Cart_sub(MPI_Comm comm, MPI3_CONST int remain_dims[], MPI_Comm *newcomm)
{
	return MPI_Symbol_load();
}

int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
	return MPI_Symbol_load();
}

int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	return MPI_Symbol_load();
}

int MPI_Comm_free(MPI_Comm *comm)
{
	return MPI_Symbol_load();
}

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
	return MPI_Symbol_load();
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
	return MPI_Symbol_load();
}

int MPI_Comm_spawn_empty(MPI3_CONST char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	return MPI_Symbol_load();
}

int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], MPI3_CONST int array_of_maxprocs[], MPI3_CONST MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])
{
	return MPI_Symbol_load();
}

int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	return MPI_Symbol_load();
}

int MPI_File_close(MPI_File *fh)
{
	return MPI_Symbol_load();
}

int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_write(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_write_all(MPI_File fh, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_write_at(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, MPI3_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Finalize(void)
{
	return MPI_Symbol_load();
}

int MPI_Gather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Gatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int *recvcounts, MPI3_CONST int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Ibsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Init(int *argc, char ***argv)
{
	return MPI_Symbol_load();
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	return MPI_Symbol_load();
}

int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)
{
	return MPI_Symbol_load();
}

int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)
{
	return MPI_Symbol_load();
}

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Irsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Isend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Issend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Put_empty(MPI3_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Reduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Reduce_scatter_empty(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Request_free(MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Rsend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Rsend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Scan_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Scatter_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Scatterv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int *sendcounts, MPI3_CONST int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Send_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Send_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Sendrecv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Ssend_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return MPI_Symbol_load();
}

int MPI_Ssend_init_empty(MPI3_CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Start(MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Startall(int count, MPI_Request array_of_requests[])
{
	return MPI_Symbol_load();
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])
{
	return MPI_Symbol_load();
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])
{
	return MPI_Symbol_load();
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
	return MPI_Symbol_load();
}

int MPI_Waitany(int count, MPI_Request *requests, int *index, MPI_Status *status)
{
	return MPI_Symbol_load();
}

int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)
{
	return MPI_Symbol_load();
}

int MPI_Win_complete(MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	return MPI_Symbol_load();
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Win_free(MPI_Win *win)
{
	return MPI_Symbol_load();
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	return MPI_Symbol_load();
}

int MPI_Win_wait(MPI_Win win)
{
	return MPI_Symbol_load();
}

#if MPI_VERSION >= 3
int MPI_Iallgather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Iallgatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Iallreduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ialltoall_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ialltoallv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Igather_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Igatherv_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, MPI3_CONST int recvcounts[], MPI3_CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ireduce_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Ireduce_scatter_empty(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Iscan_empty(MPI3_CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Iscatter_empty(MPI3_CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}

int MPI_Iscatterv_empty(MPI3_CONST void *sendbuf, MPI3_CONST int sendcounts[], MPI3_CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	return MPI_Symbol_load();
}
#endif