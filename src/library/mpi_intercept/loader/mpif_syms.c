#include <library/mpi_intercept/loader/mpic_defs.h>

void mpi_allgather(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Allgather...............");
	mpif_ear.Allgather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror);
	mpif_mpi.allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror);
	mpif_ear.Allgather_out();
	debug("<< F MPI_Allgather...............");
}
void mpi_allgather_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allgather")));
void mpi_allgather__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allgather")));

void mpi_allgatherv(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Allgatherv...............");
	mpif_ear.Allgatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierror);
	mpif_mpi.allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierror);
	mpif_ear.Allgatherv_out();
	debug("<< F MPI_Allgatherv...............");
}
void mpi_allgatherv_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allgatherv")));
void mpi_allgatherv__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allgatherv")));

void mpi_allreduce(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Allreduce...............");
	mpif_ear.Allreduce_in(sendbuf, recvbuf, count, datatype, op, comm, ierror);
	mpif_mpi.allreduce(sendbuf, recvbuf, count, datatype, op, comm, ierror);
	mpif_ear.Allreduce_out();
	debug("<< F MPI_Allreduce...............");
}
void mpi_allreduce_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allreduce")));
void mpi_allreduce__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_allreduce")));

void mpi_alltoall(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Alltoall...............");
	mpif_ear.Alltoall_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror);
	mpif_mpi.alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror);
	mpif_ear.Alltoall_out();
	debug("<< F MPI_Alltoall...............");
}
void mpi_alltoall_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_alltoall")));
void mpi_alltoall__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_alltoall")));

void mpi_alltoallv(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Alltoallv...............");
	mpif_ear.Alltoallv_in(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, ierror);
	mpif_mpi.alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, ierror);
	mpif_ear.Alltoallv_out();
	debug("<< F MPI_Alltoallv...............");
}
void mpi_alltoallv_ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_alltoallv")));
void mpi_alltoallv__ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_alltoallv")));

void mpi_barrier(MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Barrier...............");
	mpif_ear.Barrier_in(comm, ierror);
	mpif_mpi.barrier(comm, ierror);
	mpif_ear.Barrier_out();
	debug("<< F MPI_Barrier...............");
}
void mpi_barrier_ (MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_barrier")));
void mpi_barrier__ (MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_barrier")));

void mpi_bcast(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Bcast...............");
	mpif_ear.Bcast_in(buffer, count, datatype, root, comm, ierror);
	mpif_mpi.bcast(buffer, count, datatype, root, comm, ierror);
	mpif_ear.Bcast_out();
	debug("<< F MPI_Bcast...............");
}
void mpi_bcast_ (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_bcast")));
void mpi_bcast__ (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_bcast")));

void mpi_bsend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Bsend...............");
	mpif_ear.Bsend_in(buf, count, datatype, dest, tag, comm, ierror);
	mpif_mpi.bsend(buf, count, datatype, dest, tag, comm, ierror);
	mpif_ear.Bsend_out();
	debug("<< F MPI_Bsend...............");
}
void mpi_bsend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_bsend")));
void mpi_bsend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_bsend")));

void mpi_bsend_init(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Bsend_init...............");
	mpif_ear.Bsend_init_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.bsend_init(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Bsend_init_out();
	debug("<< F MPI_Bsend_init...............");
}
void mpi_bsend_init_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_bsend_init")));
void mpi_bsend_init__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_bsend_init")));

void mpi_cancel(MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Cancel...............");
	mpif_ear.Cancel_in(request, ierror);
	mpif_mpi.cancel(request, ierror);
	mpif_ear.Cancel_out();
	debug("<< F MPI_Cancel...............");
}
void mpi_cancel_ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_cancel")));
void mpi_cancel__ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_cancel")));

void mpi_cart_create(MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror)
{
	debug(">> F MPI_Cart_create...............");
	mpif_ear.Cart_create_in(comm_old, ndims, dims, periods, reorder, comm_cart, ierror);
	mpif_mpi.cart_create(comm_old, ndims, dims, periods, reorder, comm_cart, ierror);
	mpif_ear.Cart_create_out();
	debug("<< F MPI_Cart_create...............");
}
void mpi_cart_create_ (MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) __attribute__((alias("mpi_cart_create")));
void mpi_cart_create__ (MPI_Fint *comm_old, MPI_Fint *ndims, MPI3_CONST MPI_Fint *dims, MPI3_CONST MPI_Fint *periods, MPI_Fint *reorder, MPI_Fint *comm_cart, MPI_Fint *ierror) __attribute__((alias("mpi_cart_create")));

void mpi_cart_sub(MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror)
{
	debug(">> F MPI_Cart_sub...............");
	mpif_ear.Cart_sub_in(comm, remain_dims, comm_new, ierror);
	mpif_mpi.cart_sub(comm, remain_dims, comm_new, ierror);
	mpif_ear.Cart_sub_out();
	debug("<< F MPI_Cart_sub...............");
}
void mpi_cart_sub_ (MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) __attribute__((alias("mpi_cart_sub")));
void mpi_cart_sub__ (MPI_Fint *comm, MPI3_CONST MPI_Fint *remain_dims, MPI_Fint *comm_new, MPI_Fint *ierror) __attribute__((alias("mpi_cart_sub")));

void mpi_comm_create(MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_create...............");
	mpif_ear.Comm_create_in(comm, group, newcomm, ierror);
	mpif_mpi.comm_create(comm, group, newcomm, ierror);
	mpif_ear.Comm_create_out();
	debug("<< F MPI_Comm_create...............");
}
void mpi_comm_create_ (MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_create")));
void mpi_comm_create__ (MPI_Fint *comm, MPI_Fint *group, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_create")));

void mpi_comm_dup(MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_dup...............");
	mpif_ear.Comm_dup_in(comm, newcomm, ierror);
	mpif_mpi.comm_dup(comm, newcomm, ierror);
	mpif_ear.Comm_dup_out();
	debug("<< F MPI_Comm_dup...............");
}
void mpi_comm_dup_ (MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_dup")));
void mpi_comm_dup__ (MPI_Fint *comm, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_dup")));

void mpi_comm_free(MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_free...............");
	mpif_ear.Comm_free_in(comm, ierror);
	mpif_mpi.comm_free(comm, ierror);
	mpif_ear.Comm_free_out();
	debug("<< F MPI_Comm_free...............");
}
void mpi_comm_free_ (MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_free")));
void mpi_comm_free__ (MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_free")));

void mpi_comm_rank(MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_rank...............");
	mpif_ear.Comm_rank_in(comm, rank, ierror);
	mpif_mpi.comm_rank(comm, rank, ierror);

	mpif_ear.Comm_rank_out();
	debug("<< F MPI_Comm_rank...............");
}
void mpi_comm_rank_ (MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) __attribute__((alias("mpi_comm_rank")));
void mpi_comm_rank__ (MPI_Fint *comm, MPI_Fint *rank, MPI_Fint *ierror) __attribute__((alias("mpi_comm_rank")));

void mpi_comm_size(MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_size...............");
	mpif_ear.Comm_size_in(comm, size, ierror);
	mpif_mpi.comm_size(comm, size, ierror);
	mpif_ear.Comm_size_out();
	debug("<< F MPI_Comm_size...............");
}
void mpi_comm_size_ (MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) __attribute__((alias("mpi_comm_size")));
void mpi_comm_size__ (MPI_Fint *comm, MPI_Fint *size, MPI_Fint *ierror) __attribute__((alias("mpi_comm_size")));

void mpi_comm_spawn(MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror)
{
debug(">> F MPI_Comm_spawn...............");
mpif_ear.Comm_spawn_in(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes, ierror);
mpif_mpi.comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes, ierror);
mpif_ear.Comm_spawn_out();
debug("<< F MPI_Comm_spawn...............");
}
void mpi_comm_spawn_ (MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) __attribute__((alias("mpi_comm_spawn")));
void mpi_comm_spawn__ (MPI3_CONST char *command, char *argv, MPI_Fint *maxprocs, MPI_Fint *info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) __attribute__((alias("mpi_comm_spawn")));

void mpi_comm_spawn_multiple(MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror)
{
debug(">> F MPI_Comm_spawn_multiple...............");
mpif_ear.Comm_spawn_multiple_in(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes, ierror);
mpif_mpi.comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes, ierror);
mpif_ear.Comm_spawn_multiple_out();
debug("<< F MPI_Comm_spawn_multiple...............");
}
void mpi_comm_spawn_multiple_ (MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) __attribute__((alias("mpi_comm_spawn_multiple")));
void mpi_comm_spawn_multiple__ (MPI_Fint *count, char *array_of_commands, char *array_of_argv, MPI3_CONST MPI_Fint *array_of_maxprocs, MPI3_CONST MPI_Fint *array_of_info, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *intercomm, MPI_Fint *array_of_errcodes, MPI_Fint *ierror) __attribute__((alias("mpi_comm_spawn_multiple")));

void mpi_comm_split(MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror)
{
	debug(">> F MPI_Comm_split...............");
	mpif_ear.Comm_split_in(comm, color, key, newcomm, ierror);
	mpif_mpi.comm_split(comm, color, key, newcomm, ierror);
	mpif_ear.Comm_split_out();
	debug("<< F MPI_Comm_split...............");
}
void mpi_comm_split_ (MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_split")));
void mpi_comm_split__ (MPI_Fint *comm, MPI_Fint *color, MPI_Fint *key, MPI_Fint *newcomm, MPI_Fint *ierror) __attribute__((alias("mpi_comm_split")));

void mpi_file_close(MPI_File *fh, MPI_Fint *ierror)
{
	debug(">> F MPI_File_close...............");
	mpif_ear.File_close_in(fh, ierror);
	mpif_mpi.file_close(fh, ierror);
	mpif_ear.File_close_out();
	debug("<< F MPI_File_close...............");
}
void mpi_file_close_ (MPI_File *fh, MPI_Fint *ierror) __attribute__((alias("mpi_file_close")));
void mpi_file_close__ (MPI_File *fh, MPI_Fint *ierror) __attribute__((alias("mpi_file_close")));

void mpi_file_read(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_read...............");
	mpif_ear.File_read_in(fh, buf, count, datatype, status, ierror);
	mpif_mpi.file_read(fh, buf, count, datatype, status, ierror);
	mpif_ear.File_read_out();
	debug("<< F MPI_File_read...............");
}
void mpi_file_read_ (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read")));
void mpi_file_read__ (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read")));

void mpi_file_read_all(MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_read_all...............");
	mpif_ear.File_read_all_in(fh, buf, count, datatype, status, ierror);
	mpif_mpi.file_read_all(fh, buf, count, datatype, status, ierror);
	mpif_ear.File_read_all_out();
	debug("<< F MPI_File_read_all...............");
}
void mpi_file_read_all_ (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_all")));
void mpi_file_read_all__ (MPI_File *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_all")));

void mpi_file_read_at(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_read_at...............");
	mpif_ear.File_read_at_in(fh, offset, buf, count, datatype, status, ierror);
	mpif_mpi.file_read_at(fh, offset, buf, count, datatype, status, ierror);
	mpif_ear.File_read_at_out();
	debug("<< F MPI_File_read_at...............");
}
void mpi_file_read_at_ (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_at")));
void mpi_file_read_at__ (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_at")));

void mpi_file_read_at_all(MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_read_at_all...............");
	mpif_ear.File_read_at_all_in(fh, offset, buf, count, datatype, status, ierror);
	mpif_mpi.file_read_at_all(fh, offset, buf, count, datatype, status, ierror);
	mpif_ear.File_read_at_all_out();
	debug("<< F MPI_File_read_at_all...............");
}
void mpi_file_read_at_all_ (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_at_all")));
void mpi_file_read_at_all__ (MPI_File *fh, MPI_Offset *offset, void* buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_read_at_all")));

void mpi_file_write(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_write...............");
	mpif_ear.File_write_in(fh, buf, count, datatype, status, ierror);
	mpif_mpi.file_write(fh, buf, count, datatype, status, ierror);
	mpif_ear.File_write_out();
	debug("<< F MPI_File_write...............");
}
void mpi_file_write_ (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write")));
void mpi_file_write__ (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write")));

void mpi_file_write_all(MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_write_all...............");
	mpif_ear.File_write_all_in(fh, buf, count, datatype, status, ierror);
	mpif_mpi.file_write_all(fh, buf, count, datatype, status, ierror);
	mpif_ear.File_write_all_out();
	debug("<< F MPI_File_write_all...............");
}
void mpi_file_write_all_ (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_all")));
void mpi_file_write_all__ (MPI_File *fh, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_all")));

void mpi_file_write_at(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_write_at...............");
	mpif_ear.File_write_at_in(fh, offset, buf, count, datatype, status, ierror);
	mpif_mpi.file_write_at(fh, offset, buf, count, datatype, status, ierror);
	mpif_ear.File_write_at_out();
	debug("<< F MPI_File_write_at...............");
}
void mpi_file_write_at_ (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_at")));
void mpi_file_write_at__ (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_at")));

void mpi_file_write_at_all(MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror)
{
	debug(">> F MPI_File_write_at_all...............");
	mpif_ear.File_write_at_all_in(fh, offset, buf, count, datatype, status, ierror);
	mpif_mpi.file_write_at_all(fh, offset, buf, count, datatype, status, ierror);
	mpif_ear.File_write_at_all_out();
	debug("<< F MPI_File_write_at_all...............");
}
void mpi_file_write_at_all_ (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_at_all")));
void mpi_file_write_at_all__ (MPI_File *fh, MPI_Offset *offset, MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierror) __attribute__((alias("mpi_file_write_at_all")));

void mpi_finalize(MPI_Fint *ierror)
{
	debug(">> F MPI_Finalize...............");
	mpif_ear.Finalize_in(ierror);
	mpif_mpi.finalize(ierror);
	mpif_ear.Finalize_out();
	debug("<< F MPI_Finalize...............");
}
void mpi_finalize_ (MPI_Fint *ierror) __attribute__((alias("mpi_finalize")));
void mpi_finalize__ (MPI_Fint *ierror) __attribute__((alias("mpi_finalize")));

void mpi_gather(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Gather...............");
	mpif_ear.Gather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_mpi.gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_ear.Gather_out();
	debug("<< F MPI_Gather...............");
}
void mpi_gather_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_gather")));
void mpi_gather__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_gather")));

void mpi_gatherv(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Gatherv...............");
	mpif_ear.Gatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierror);
	mpif_mpi.gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierror);
	mpif_ear.Gatherv_out();
	debug("<< F MPI_Gatherv...............");
}
void mpi_gatherv_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_gatherv")));
void mpi_gatherv__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_gatherv")));

void mpi_get(MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Get...............");
	mpif_ear.Get_in(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror);
	mpif_mpi.get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror);
	mpif_ear.Get_out();
	debug("<< F MPI_Get...............");
}
void mpi_get_ (MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_get")));
void mpi_get__ (MPI_Fint *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_get")));

void mpi_ibsend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Ibsend...............");
	mpif_ear.Ibsend_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.ibsend(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Ibsend_out();
	debug("<< F MPI_Ibsend...............");
}
void mpi_ibsend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibsend")));
void mpi_ibsend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibsend")));

void mpi_init(MPI_Fint *ierror)
{
	debug(">> F MPI_Init...............");
	mpi_f_symbol_load();
	mpif_ear.Init_in(ierror);
	mpif_mpi.init(ierror);
	mpif_ear.Init_out();
	debug("<< F MPI_Init...............");
}
void mpi_init_ (MPI_Fint *ierror) __attribute__((alias("mpi_init")));
void mpi_init__ (MPI_Fint *ierror) __attribute__((alias("mpi_init")));

void mpi_init_thread(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror)
{
	debug(">> F MPI_Init_thread...............");
	mpi_f_symbol_load();
	mpif_ear.Init_thread_in(required, provided, ierror);
	mpif_mpi.init_thread(required, provided, ierror);
	mpif_ear.Init_thread_out();
	debug("<< F MPI_Init_thread...............");
}
void mpi_init_thread_ (MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) __attribute__((alias("mpi_init_thread")));
void mpi_init_thread__ (MPI_Fint *required, MPI_Fint *provided, MPI_Fint *ierror) __attribute__((alias("mpi_init_thread")));

void mpi_intercomm_create(MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror)
{
	debug(">> F MPI_Intercomm_create...............");
	mpif_ear.Intercomm_create_in(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm, ierror);
	mpif_mpi.intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm, ierror);
	mpif_ear.Intercomm_create_out();
	debug("<< F MPI_Intercomm_create...............");
}
void mpi_intercomm_create_ (MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) __attribute__((alias("mpi_intercomm_create")));
void mpi_intercomm_create__ (MPI_Fint *local_comm, MPI_Fint *local_leader, MPI_Fint *peer_comm, MPI_Fint *remote_leader, MPI_Fint *tag, MPI_Fint *newintercomm, MPI_Fint *ierror) __attribute__((alias("mpi_intercomm_create")));

void mpi_intercomm_merge(MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror)
{
	debug(">> F MPI_Intercomm_merge...............");
	mpif_ear.Intercomm_merge_in(intercomm, high, newintracomm, ierror);
	mpif_mpi.intercomm_merge(intercomm, high, newintracomm, ierror);
	mpif_ear.Intercomm_merge_out();
	debug("<< F MPI_Intercomm_merge...............");
}
void mpi_intercomm_merge_ (MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) __attribute__((alias("mpi_intercomm_merge")));
void mpi_intercomm_merge__ (MPI_Fint *intercomm, MPI_Fint *high, MPI_Fint *newintracomm, MPI_Fint *ierror) __attribute__((alias("mpi_intercomm_merge")));

void mpi_iprobe(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Iprobe...............");
	mpif_ear.Iprobe_in(source, tag, comm, flag, status, ierror);
	mpif_mpi.iprobe(source, tag, comm, flag, status, ierror);
	mpif_ear.Iprobe_out();
	debug("<< F MPI_Iprobe...............");
}
void mpi_iprobe_ (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_iprobe")));
void mpi_iprobe__ (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_iprobe")));

void mpi_irecv(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Irecv...............");
	mpif_ear.Irecv_in(buf, count, datatype, source, tag, comm, request, ierror);
	mpif_mpi.irecv(buf, count, datatype, source, tag, comm, request, ierror);
	mpif_ear.Irecv_out();
	debug("<< F MPI_Irecv...............");
}
void mpi_irecv_ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_irecv")));
void mpi_irecv__ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_irecv")));

void mpi_irsend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Irsend...............");
	mpif_ear.Irsend_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.irsend(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Irsend_out();
	debug("<< F MPI_Irsend...............");
}
void mpi_irsend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_irsend")));
void mpi_irsend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_irsend")));

void mpi_isend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Isend...............");
	mpif_ear.Isend_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.isend(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Isend_out();
	debug("<< F MPI_Isend...............");
}
void mpi_isend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_isend")));
void mpi_isend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_isend")));

void mpi_issend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Issend...............");
	mpif_ear.Issend_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.issend(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Issend_out();
	debug("<< F MPI_Issend...............");
}
void mpi_issend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_issend")));
void mpi_issend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_issend")));

void mpi_probe(MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Probe...............");
	mpif_ear.Probe_in(source, tag, comm, status, ierror);
	mpif_mpi.probe(source, tag, comm, status, ierror);
	mpif_ear.Probe_out();
	debug("<< F MPI_Probe...............");
}
void mpi_probe_ (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_probe")));
void mpi_probe__ (MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_probe")));

void mpi_put(MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Put...............");
	mpif_ear.Put_in(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror);
	mpif_mpi.put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror);
	mpif_ear.Put_out();
	debug("<< F MPI_Put...............");
}
void mpi_put_ (MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_put")));
void mpi_put__ (MPI3_CONST void *origin_addr, MPI_Fint *origin_count, MPI_Fint *origin_datatype, MPI_Fint *target_rank, MPI_Fint *target_disp, MPI_Fint *target_count, MPI_Fint *target_datatype, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_put")));

void mpi_recv(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Recv...............");
	mpif_ear.Recv_in(buf, count, datatype, source, tag, comm, status, ierror);
	mpif_mpi.recv(buf, count, datatype, source, tag, comm, status, ierror);
	mpif_ear.Recv_out();
	debug("<< F MPI_Recv...............");
}
void mpi_recv_ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_recv")));
void mpi_recv__ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_recv")));

void mpi_recv_init(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Recv_init...............");
	mpif_ear.Recv_init_in(buf, count, datatype, source, tag, comm, request, ierror);
	mpif_mpi.recv_init(buf, count, datatype, source, tag, comm, request, ierror);
	mpif_ear.Recv_init_out();
	debug("<< F MPI_Recv_init...............");
}
void mpi_recv_init_ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_recv_init")));
void mpi_recv_init__ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_recv_init")));

void mpi_reduce(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Reduce...............");
	mpif_ear.Reduce_in(sendbuf, recvbuf, count, datatype, op, root, comm, ierror);
	mpif_mpi.reduce(sendbuf, recvbuf, count, datatype, op, root, comm, ierror);
	mpif_ear.Reduce_out();
	debug("<< F MPI_Reduce...............");
}
void mpi_reduce_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_reduce")));
void mpi_reduce__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_reduce")));

void mpi_reduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Reduce_scatter...............");
	mpif_ear.Reduce_scatter_in(sendbuf, recvbuf, recvcounts, datatype, op, comm, ierror);
	mpif_mpi.reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, ierror);
	mpif_ear.Reduce_scatter_out();
	debug("<< F MPI_Reduce_scatter...............");
}
void mpi_reduce_scatter_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_reduce_scatter")));
void mpi_reduce_scatter__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_reduce_scatter")));

void mpi_request_free(MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Request_free...............");
	mpif_ear.Request_free_in(request, ierror);
	mpif_mpi.request_free(request, ierror);
	mpif_ear.Request_free_out();
	debug("<< F MPI_Request_free...............");
}
void mpi_request_free_ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_request_free")));
void mpi_request_free__ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_request_free")));

void mpi_request_get_status(MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Request_get_status...............");
	mpif_ear.Request_get_status_in(request, flag, status, ierror);
	mpif_mpi.request_get_status(request, flag, status, ierror);
	mpif_ear.Request_get_status_out();
	debug("<< F MPI_Request_get_status...............");
}
void mpi_request_get_status_ (MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_request_get_status")));
void mpi_request_get_status__ (MPI_Fint *request, int *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_request_get_status")));

void mpi_rsend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Rsend...............");
	mpif_ear.Rsend_in(buf, count, datatype, dest, tag, comm, ierror);
	mpif_mpi.rsend(buf, count, datatype, dest, tag, comm, ierror);
	mpif_ear.Rsend_out();
	debug("<< F MPI_Rsend...............");
}
void mpi_rsend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_rsend")));
void mpi_rsend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_rsend")));

void mpi_rsend_init(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Rsend_init...............");
	mpif_ear.Rsend_init_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.rsend_init(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Rsend_init_out();
	debug("<< F MPI_Rsend_init...............");
}
void mpi_rsend_init_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_rsend_init")));
void mpi_rsend_init__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_rsend_init")));

void mpi_scan(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Scan...............");
	mpif_ear.Scan_in(sendbuf, recvbuf, count, datatype, op, comm, ierror);
	mpif_mpi.scan(sendbuf, recvbuf, count, datatype, op, comm, ierror);
	mpif_ear.Scan_out();
	debug("<< F MPI_Scan...............");
}
void mpi_scan_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scan")));
void mpi_scan__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scan")));

void mpi_scatter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Scatter...............");
	mpif_ear.Scatter_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_mpi.scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_ear.Scatter_out();
	debug("<< F MPI_Scatter...............");
}
void mpi_scatter_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scatter")));
void mpi_scatter__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scatter")));

void mpi_scatterv(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Scatterv...............");
	mpif_ear.Scatterv_in(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_mpi.scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror);
	mpif_ear.Scatterv_out();
	debug("<< F MPI_Scatterv...............");
}
void mpi_scatterv_ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scatterv")));
void mpi_scatterv__ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_scatterv")));

void mpi_send(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Send...............");
	mpif_ear.Send_in(buf, count, datatype, dest, tag, comm, ierror);
	mpif_mpi.send(buf, count, datatype, dest, tag, comm, ierror);
	mpif_ear.Send_out();
	debug("<< F MPI_Send...............");
}
void mpi_send_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_send")));
void mpi_send__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_send")));

void mpi_send_init(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Send_init...............");
	mpif_ear.Send_init_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.send_init(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Send_init_out();
	debug("<< F MPI_Send_init...............");
}
void mpi_send_init_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_send_init")));
void mpi_send_init__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_send_init")));

void mpi_sendrecv(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Sendrecv...............");
	mpif_ear.Sendrecv_in(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierror);
	mpif_mpi.sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierror);
	mpif_ear.Sendrecv_out();
	debug("<< F MPI_Sendrecv...............");
}
void mpi_sendrecv_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_sendrecv")));
void mpi_sendrecv__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_sendrecv")));

void mpi_sendrecv_replace(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Sendrecv_replace...............");
	mpif_ear.Sendrecv_replace_in(buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierror);
	mpif_mpi.sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierror);
	mpif_ear.Sendrecv_replace_out();
	debug("<< F MPI_Sendrecv_replace...............");
}
void mpi_sendrecv_replace_ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_sendrecv_replace")));
void mpi_sendrecv_replace__ (void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag, MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_sendrecv_replace")));

void mpi_ssend(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror)
{
	debug(">> F MPI_Ssend...............");
	mpif_ear.Ssend_in(buf, count, datatype, dest, tag, comm, ierror);
	mpif_mpi.ssend(buf, count, datatype, dest, tag, comm, ierror);
	mpif_ear.Ssend_out();
	debug("<< F MPI_Ssend...............");
}
void mpi_ssend_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_ssend")));
void mpi_ssend__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_ssend")));

void mpi_ssend_init(MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Ssend_init...............");
	mpif_ear.Ssend_init_in(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_mpi.ssend_init(buf, count, datatype, dest, tag, comm, request, ierror);
	mpif_ear.Ssend_init_out();
	debug("<< F MPI_Ssend_init...............");
}
void mpi_ssend_init_ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ssend_init")));
void mpi_ssend_init__ (MPI3_CONST void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ssend_init")));

void mpi_start(MPI_Fint *request, MPI_Fint *ierror)
{
	debug(">> F MPI_Start...............");
	mpif_ear.Start_in(request, ierror);
	mpif_mpi.start(request, ierror);
	mpif_ear.Start_out();
	debug("<< F MPI_Start...............");
}
void mpi_start_ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_start")));
void mpi_start__ (MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_start")));

void mpi_startall(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror)
{
	debug(">> F MPI_Startall...............");
	mpif_ear.Startall_in(count, array_of_requests, ierror);
	mpif_mpi.startall(count, array_of_requests, ierror);
	mpif_ear.Startall_out();
	debug("<< F MPI_Startall...............");
}
void mpi_startall_ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) __attribute__((alias("mpi_startall")));
void mpi_startall__ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *ierror) __attribute__((alias("mpi_startall")));

void mpi_test(MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Test...............");
	mpif_ear.Test_in(request, flag, status, ierror);
	mpif_mpi.test(request, flag, status, ierror);
	mpif_ear.Test_out();
	debug("<< F MPI_Test...............");
}
void mpi_test_ (MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_test")));
void mpi_test__ (MPI_Fint *request, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_test")));

void mpi_testall(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror)
{
	debug(">> F MPI_Testall...............");
	mpif_ear.Testall_in(count, array_of_requests, flag, array_of_statuses, ierror);
	mpif_mpi.testall(count, array_of_requests, flag, array_of_statuses, ierror);
	mpif_ear.Testall_out();
	debug("<< F MPI_Testall...............");
}
void mpi_testall_ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_testall")));
void mpi_testall__ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *flag, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_testall")));

void mpi_testany(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Testany...............");
	mpif_ear.Testany_in(count, array_of_requests, index, flag, status, ierror);
	mpif_mpi.testany(count, array_of_requests, index, flag, status, ierror);
	mpif_ear.Testany_out();
	debug("<< F MPI_Testany...............");
}
void mpi_testany_ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_testany")));
void mpi_testany__ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *index, MPI_Fint *flag, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_testany")));

void mpi_testsome(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror)
{
	debug(">> F MPI_Testsome...............");
	mpif_ear.Testsome_in(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror);
	mpif_mpi.testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror);
	mpif_ear.Testsome_out();
	debug("<< F MPI_Testsome...............");
}
void mpi_testsome_ (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_testsome")));
void mpi_testsome__ (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_testsome")));

void mpi_wait(MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Wait...............");
	mpif_ear.Wait_in(request, status, ierror);
	mpif_mpi.wait(request, status, ierror);
	mpif_ear.Wait_out();
	debug("<< F MPI_Wait...............");
}
void mpi_wait_ (MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_wait")));
void mpi_wait__ (MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_wait")));

void mpi_waitall(MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror)
{
	debug(">> F MPI_Waitall...............");
	mpif_ear.Waitall_in(count, array_of_requests, array_of_statuses, ierror);
	mpif_mpi.waitall(count, array_of_requests, array_of_statuses, ierror);
	mpif_ear.Waitall_out();
	debug("<< F MPI_Waitall...............");
}
void mpi_waitall_ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_waitall")));
void mpi_waitall__ (MPI_Fint *count, MPI_Fint *array_of_requests, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_waitall")));

void mpi_waitany(MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror)
{
	debug(">> F MPI_Waitany...............");
	mpif_ear.Waitany_in(count, requests, index, status, ierror);
	mpif_mpi.waitany(count, requests, index, status, ierror);
	mpif_ear.Waitany_out();
	debug("<< F MPI_Waitany...............");
}
void mpi_waitany_ (MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_waitany")));
void mpi_waitany__ (MPI_Fint *count, MPI_Fint *requests, MPI_Fint *index, MPI_Fint *status, MPI_Fint *ierror) __attribute__((alias("mpi_waitany")));

void mpi_waitsome(MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror)
{
	debug(">> F MPI_Waitsome...............");
	mpif_ear.Waitsome_in(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror);
	mpif_mpi.waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror);
	mpif_ear.Waitsome_out();
	debug("<< F MPI_Waitsome...............");
}
void mpi_waitsome_ (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_waitsome")));
void mpi_waitsome__ (MPI_Fint *incount, MPI_Fint *array_of_requests, MPI_Fint *outcount, MPI_Fint *array_of_indices, MPI_Fint *array_of_statuses, MPI_Fint *ierror) __attribute__((alias("mpi_waitsome")));

void mpi_win_complete(MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_complete...............");
	mpif_ear.Win_complete_in(win, ierror);
	mpif_mpi.win_complete(win, ierror);
	mpif_ear.Win_complete_out();
	debug("<< F MPI_Win_complete...............");
}
void mpi_win_complete_ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_complete")));
void mpi_win_complete__ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_complete")));

void mpi_win_create(void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_create...............");
	mpif_ear.Win_create_in(base, size, disp_unit, info, comm, win, ierror);
	mpif_mpi.win_create(base, size, disp_unit, info, comm, win, ierror);
	mpif_ear.Win_create_out();
	debug("<< F MPI_Win_create...............");
}
void mpi_win_create_ (void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_create")));
void mpi_win_create__ (void *base, MPI_Aint *size, MPI_Fint *disp_unit, MPI_Fint *info, MPI_Fint *comm, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_create")));

void mpi_win_fence(MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_fence...............");
	mpif_ear.Win_fence_in(assert, win, ierror);
	mpif_mpi.win_fence(assert, win, ierror);
	mpif_ear.Win_fence_out();
	debug("<< F MPI_Win_fence...............");
}
void mpi_win_fence_ (MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_fence")));
void mpi_win_fence__ (MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_fence")));

void mpi_win_free(MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_free...............");
	mpif_ear.Win_free_in(win, ierror);
	mpif_mpi.win_free(win, ierror);
	mpif_ear.Win_free_out();
	debug("<< F MPI_Win_free...............");
}
void mpi_win_free_ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_free")));
void mpi_win_free__ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_free")));

void mpi_win_post(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_post...............");
	mpif_ear.Win_post_in(group, assert, win, ierror);
	mpif_mpi.win_post(group, assert, win, ierror);
	mpif_ear.Win_post_out();
	debug("<< F MPI_Win_post...............");
}
void mpi_win_post_ (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_post")));
void mpi_win_post__ (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_post")));

void mpi_win_start(MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_start...............");
	mpif_ear.Win_start_in(group, assert, win, ierror);
	mpif_mpi.win_start(group, assert, win, ierror);
	mpif_ear.Win_start_out();
	debug("<< F MPI_Win_start...............");
}
void mpi_win_start_ (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_start")));
void mpi_win_start__ (MPI_Fint *group, MPI_Fint *assert, MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_start")));

void mpi_win_wait(MPI_Fint *win, MPI_Fint *ierror)
{
	debug(">> F MPI_Win_wait...............");
	mpif_ear.Win_wait_in(win, ierror);
	mpif_mpi.win_wait(win, ierror);
	mpif_ear.Win_wait_out();
	debug("<< F MPI_Win_wait...............");
}
void mpi_win_wait_ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_wait")));
void mpi_win_wait__ (MPI_Fint *win, MPI_Fint *ierror) __attribute__((alias("mpi_win_wait")));

#if MPI_VERSION >= 3
void mpi_iallgather(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iallgather...............");
    mpif_ear.Iallgather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request, ierror);
    mpif_mpi.iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request, ierror);
    mpif_ear.Iallgather_out();
    debug("<< F MPI_Iallgather...............");
}
void mpi_iallgather_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallgather")));
void mpi_iallgather__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallgather")));

void mpi_iallgatherv(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iallgatherv...............");
    mpif_ear.Iallgatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, displs, recvtype, comm, request, ierror);
    mpif_mpi.iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcount, displs, recvtype, comm, request, ierror);
    mpif_ear.Iallgatherv_out();
    debug("<< F MPI_Iallgatherv...............");
}
void mpi_iallgatherv_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallgatherv")));
void mpi_iallgatherv__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcount, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallgatherv")));

void mpi_iallreduce(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iallreduce...............");
    mpif_ear.Iallreduce_in(sendbuf, recvbuf, count, datatype, op, comm, request, ierror);
    mpif_mpi.iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request, ierror);
    mpif_ear.Iallreduce_out();
    debug("<< F MPI_Iallreduce...............");
}
void mpi_iallreduce_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallreduce")));
void mpi_iallreduce__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iallreduce")));

void mpi_ialltoall(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Ialltoall...............");
    mpif_ear.Ialltoall_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request, ierror);
    mpif_mpi.ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request, ierror);
    mpif_ear.Ialltoall_out();
    debug("<< F MPI_Ialltoall...............");
}
void mpi_ialltoall_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ialltoall")));
void mpi_ialltoall__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ialltoall")));

void mpi_ialltoallv(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror)
{
    debug(">> F MPI_Ialltoallv...............");
    mpif_ear.Ialltoallv_in(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, request, comm, ierror);
    mpif_mpi.ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, request, comm, ierror);
    mpif_ear.Ialltoallv_out();
    debug("<< F MPI_Ialltoallv...............");
}
void mpi_ialltoallv_ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_ialltoallv")));
void mpi_ialltoallv__ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *sdispls, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *rdispls, MPI_Fint *recvtype, MPI_Fint *request, MPI_Fint *comm, MPI_Fint *ierror) __attribute__((alias("mpi_ialltoallv")));

void mpi_ibarrier(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Ibarrier...............");
    mpif_ear.Ibarrier_in(comm, request, ierror);
    mpif_mpi.ibarrier(comm, request, ierror);
    mpif_ear.Ibarrier_out();
    debug("<< F MPI_Ibarrier...............");
}
void mpi_ibarrier_ (MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibarrier")));
void mpi_ibarrier__ (MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibarrier")));

void mpi_ibcast(void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Ibcast...............");
    mpif_ear.Ibcast_in(buffer, count, datatype, root, comm, request, ierror);
    mpif_mpi.ibcast(buffer, count, datatype, root, comm, request, ierror);
    mpif_ear.Ibcast_out();
    debug("<< F MPI_Ibcast...............");
}
void mpi_ibcast_ (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibcast")));
void mpi_ibcast__ (void *buffer, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ibcast")));

void mpi_igather(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Igather...............");
    mpif_ear.Igather_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_mpi.igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_ear.Igather_out();
    debug("<< F MPI_Igather...............");
}
void mpi_igather_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_igather")));
void mpi_igather__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_igather")));

void mpi_igatherv(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Igatherv...............");
    mpif_ear.Igatherv_in(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request, ierror);
    mpif_mpi.igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request, ierror);
    mpif_ear.Igatherv_out();
    debug("<< F MPI_Igatherv...............");
}
void mpi_igatherv_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_igatherv")));
void mpi_igatherv__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_igatherv")));

void mpi_ireduce(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Ireduce...............");
    mpif_ear.Ireduce_in(sendbuf, recvbuf, count, datatype, op, root, comm, request, ierror);
    mpif_mpi.ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request, ierror);
    mpif_ear.Ireduce_out();
    debug("<< F MPI_Ireduce...............");
}
void mpi_ireduce_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ireduce")));
void mpi_ireduce__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ireduce")));

void mpi_ireduce_scatter(MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Ireduce_scatter...............");
    mpif_ear.Ireduce_scatter_in(sendbuf, recvbuf, recvcounts, datatype, op, comm, request, ierror);
    mpif_mpi.ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request, ierror);
    mpif_ear.Ireduce_scatter_out();
    debug("<< F MPI_Ireduce_scatter...............");
}
void mpi_ireduce_scatter_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ireduce_scatter")));
void mpi_ireduce_scatter__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI3_CONST MPI_Fint *recvcounts, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_ireduce_scatter")));

void mpi_iscan(MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iscan...............");
    mpif_ear.Iscan_in(sendbuf, recvbuf, count, datatype, op, comm, request, ierror);
    mpif_mpi.iscan(sendbuf, recvbuf, count, datatype, op, comm, request, ierror);
    mpif_ear.Iscan_out();
    debug("<< F MPI_Iscan...............");
}
void mpi_iscan_ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscan")));
void mpi_iscan__ (MPI3_CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscan")));

void mpi_iscatter(MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iscatter...............");
    mpif_ear.Iscatter_in(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_mpi.iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_ear.Iscatter_out();
    debug("<< F MPI_Iscatter...............");
}
void mpi_iscatter_ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscatter")));
void mpi_iscatter__ (MPI3_CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscatter")));

void mpi_iscatterv(MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror)
{
    debug(">> F MPI_Iscatterv...............");
    mpif_ear.Iscatterv_in(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_mpi.iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request, ierror);
    mpif_ear.Iscatterv_out();
    debug("<< F MPI_Iscatterv...............");
}
void mpi_iscatterv_ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscatterv")));
void mpi_iscatterv__ (MPI3_CONST void *sendbuf, MPI3_CONST MPI_Fint *sendcounts, MPI3_CONST MPI_Fint *displs, MPI_Fint *sendtype, void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierror) __attribute__((alias("mpi_iscatterv")));
#endif