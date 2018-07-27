#!/bin/bash
#SBATCH -N 1
#SBATCH -e OUTS/test.%j.err
#SBATCH -o OUTS/test.%j.out
#SBTACH --ntasks=24
#SBATCH --tasks-per-node=24
#SBATCH --cpus-per-task=1
#SBATCH --ear-verbose=1 


export CORES=24
export CORES_SOCKET=12
export MPIS=$CORES

export KERNELS_PATH=../../../../kernels/NPB3.3.1-MZ/NPB3.3-MZ-MPI/bin
export kernel=sp-mz.C.$CORES


export I_MPI_PIN=1
export I_MPI_PIN_DOMAIN=auto
export OMP_NUM_THREADS=1
#export KMP_AFFINITY=granularity=fine,compact,1,0


srun -J $kernel --mpi=pmi2  $KERNELS_PATH/$kernel 




