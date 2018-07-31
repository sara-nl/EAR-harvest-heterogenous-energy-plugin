#!/bin/bash

export CORES=24
export CORES_SOCKET=12
export MPIS=1
export PPN=1
export NODES=1

export KERNELS_PATH=../../../../kernels/NPB3.3.1/NPB3.3-OMP/bin
export kernel=bt.B.x 



export I_MPI_PIN=1
export I_MPI_PIN_DOMAIN=auto
export OMP_NUM_THREADS=$CORES
#export KMP_AFFINITY=granularity=fine,compact,1,0

srun  -J $kernel --mpi=pmi2 -N $NODES -n $MPIS --tasks-per-node=$PPN --cpus-per-task=$OMP_NUM_THREADS $KERNELS_PATH/$kernel 




