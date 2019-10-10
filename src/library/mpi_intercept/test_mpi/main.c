#include <stdio.h>
#include <dlfcn.h>
#include "test0.h"

int main(int argc, char *argv[])
{
    MPI_Init(0, 0);
#if 1
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
    MPI_Comm_rank(0, 0);
    MPI_Comm_size(0, 0);
    MPI_Comm_spawn(0, 0, 0, 0, 0, 0, 0, 0);
    MPI_Comm_spawn_multiple(0, 0, 0, 0, 0, 0, 0, 0, 0);
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
    MPI_Init_thread(0, 0, 0, 0);
    MPI_Intercomm_create (0, 0, 0, 0, 0, 0);
    MPI_Intercomm_merge (0, 0, 0);
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
    MPI_Request_get_status(0, 0, 0);
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
    MPI_Waitany(0, 0, 0, 0);
    MPI_Waitsome(0, 0, 0, 0, 0);
    MPI_Win_complete(0);
    MPI_Win_create(0, 0, 0, 0, 0, 0);
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
#endif
    return 0;
}
