#!/bin/bash
#SBATCH -N 1
#SBATCH -e OUTS/test.%j.err
#SBATCH -o OUTS/test.%j.out
#SBTACH --ntasks=24
#SBATCH --tasks-per-node=24
#SBATCH --cpus-per-task=1
#SBATCH --ear-verbose=1 
#SBATCH --ear-mpi-dist=openmpi-fortran



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


srun --ntasks=24  --mpi=pmi2  -J $kernel   $KERNELS_PATH/$kernel 



