#!/bin/bash

export NUMNODES=1
export NUMSOCKETS=2
export CORES_PER_SOCKET=14
export MPIS=28
export THREADS=1
export NODENAME=`hostname`
export BENCHMARK=$EAR_INSTALL_PATH/bin/kernels/ep.D.$MPIS

# #adding bash tool library
source $EAR_INSTALL_PATH/ear_scripts/Bash_utils.sh
source /hpc/base/intel/parallel_studio_xe_2017.1.043/bin/psxevars.sh intel64

export I_MPI_PMI_LIBRARY=/home/xjaneas/slurm/lib/libpmi.so

## INTELMPI Environment variables :
myimpi_default_settings

## IntelMPI protocol settings:
myimpi_protocol tmi

## IntelMPI Binding
## NumberOfSocketsPerNode NumberOfNumaNodesPerSocket NumberOfCoresPerSocket HyperThreads NumberOfMpiPerNode NumberOfOpenmpPerMPI NumberOfSocketsUsed HyperThreadsUsed
myimpi_bind_intel2 $NUMSOCKETS 1 $CORES_PER_SOCKET 2 $MPIS $THREADS 2 1

## ROMIO tuning
myimpi_io_settings


/home/xjaneas/slurm/bin/srun -n 28 ${BENCHMARK}
