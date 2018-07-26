#!/bin/bash

export CORES=24
export CORES_SOCKET=12
export MPIS=$CORES
export PPN=$CORES
export NODES=1

export KERNELS_PATH=../../../../kernels/STREAM/
export kernel=stream_mpi


export I_MPI_PIN=1
export I_MPI_PIN_DOMAIN=auto
export OMP_NUM_THREADS=1
#export KMP_AFFINITY=granularity=fine,compact,1,0

mkdir DB
srun --ear-user-db=DB/stream_mpi. --ear-verbose=1 -J $kernel --mpi=pmi2 -N $NODES -n $MPIS --tasks-per-node=$PPN --cpus-per-task=$OMP_NUM_THREADS $KERNELS_PATH/$kernel 




