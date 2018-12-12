#!/bin/bash

export BENCHS_SRC_PATH=$EAR_SOURCE_PATH/kernels
export BENCHS_BIN_PATH=$EAR_INSTALL_PATH/bin/kernels

function configuring
{
	echo "----------------------------------------------------------"
	echo "Configuring $1 $3 $4"
	echo "----------------------------------------------------------"
	export BENCHMARK=$1
	export BENCH_SRC_PATH=$2
	export CLASS=$3
	export MPIS=$4
}

function launching
{
    echo "----------------------------------------------------------"
    echo "Launching (SLURM) $1 $2 "
    echo "----------------------------------------------------------"

    srun -N 1 -n $2 -J $1 -w $SLURMD_NODENAME --jobid=$SLURM_JOBID \
         $BENCHS_BIN_PATH/$1
}

function test_phase
{
    case "$1" in
        lu-mpi|ep)
            case $1 in
                ep)
                    configuring $1 $BENCHS_SRC_PATH/NPB3.3.1/NPB3.3-MPI $2 $CORES
                ;;
                lu-mpi)
                    configuring lu $BENCHS_SRC_PATH/NPB3.3.1/NPB3.3-MPI $2 $CORES
                ;;
            esac
        	export OMP_NUM_THREADS=1
            launching $BENCHMARK.$CLASS.$MPIS $MPIS
        ;;
        lu-mz|sp-mz|bt-mz)
            case $1 in
                lu-mz)
                    configuring $1 $BENCHS_SRC_PATH/NPB3.3.1-MZ/NPB3.3-MZ-MPI $2 $SOCKETS
                    export OMP_NUM_THREADS=$CORES_PER_SOCKET
                ;;
                *)
                    configuring $1 $BENCHS_SRC_PATH/NPB3.3.1-MZ/NPB3.3-MZ-MPI $2 $CORES
                    export OMP_NUM_THREADS=1
                ;;
            esac

            launching $BENCHMARK.$CLASS.$MPIS $MPIS
        ;;
        ua)
            configuring $1 $BENCHS_SRC_PATH/NPB3.3.1/NPB3.3-OMP $2 1

            export OMP_NUM_THREADS=$CORES
            launching $BENCHMARK.$CLASS.x $MPIS
        ;;
        dgemm)
            configuring mpi $BENCHS_SRC_PATH/DGEMM/mkl_fortran_samples/matrix_multiplication X 1

            export KMP_AFFINITY=granularity=fine,compact,1,0
            export MKL_NUM_THREADS=$CORES
            launching dgemm_example $MPIS
        ;;
        stream)
            configuring stream_mpi $BENCHS_SRC_PATH/STREAM X $CORES

			ulimit -s unlimited
            export OMP_NUM_THREADS=1
            launching stream_mpi $MPIS
        ;;
        *)
            echo "Invalid benchmark"
            exit 1
    esac
}
