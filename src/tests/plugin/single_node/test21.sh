#!/bin/bash

export CORES=24
export CORES_SOCKET=12
export MPIS=$CORES
export PPN=$CORES
export NODES=1

export KERNELS_PATH=../../../../kernels/NPB3.3.1-MZ/NPB3.3-MZ-MPI/bin
export KERNELS_PATH=/home/xjcorbalan/git/benchmarks/NPB3.3.1-MZ/NPB3.3-MZ-MPI/bin
export kernel=sp-mz.openmpi.C.$CORES
module load mpi/openmpi.3.0.0 


export I_MPI_PIN=1
export I_MPI_PIN_DOMAIN=auto
export OMP_NUM_THREADS=1
#export KMP_AFFINITY=granularity=fine,compact,1,0


srun --mpi=pmi2 --ear-mpi-dist=openmpi --ear-verbose=1 -J $kernel  -N $NODES -n $MPIS --tasks-per-node=$PPN --cpus-per-task=$OMP_NUM_THREADS $KERNELS_PATH/$kernel 



