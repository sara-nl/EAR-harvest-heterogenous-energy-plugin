#!/bin/bash

export BENCHS_SRC_PATH=$EAR_SRC_PATH/src/learning/kernels
export BENCHS_BIN_PATH=$EAR_INSTALL_PATH/bin/kernels
export MPI_SCRIPT_PATH=$EAR_INSTALL_PATH/etc/scripts/running/mpi_exec.sh

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
    # Update srun command for custom SLURM installations
    export SRUN_PATH=/home/xjaneas/slurm/bin/srun

    echo "----------------------------------------------------------"
    echo "Launching $1 $2"
    echo "----------------------------------------------------------"
    $SRUN_PATH -N1 -n$2 -J $1.$EAR_P_STATE -w $SLURMD_NODENAME \
        --jobid=$SLURM_JOBID --ear-policy=MONITORING_ONLY      \
        $BENCHS_BIN_PATH/$1
}

function launching_disabled
{
    # MPI options
    export I_MPI_PIN=1
    export I_MPI_PIN_DOMAIN=auto

    # Non-edit region
    export EAR_APP_NAME=$1
    $MPI_SCRIPT_PATH local $BENCHS_BIN_PATH/$1 $2 $2 MONITORING_ONLY
}

function learning_phase
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

            if [ "$BENCHS_MODE" == "compile" ]; then
                mkdir -p $BENCH_SRC_PATH/bin
                cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS NPROCS=$MPIS
                mv $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.$MPIS $BENCHS_BIN_PATH
            fi
            if [ "$BENCHS_MODE" == "test" ]; then
                export OMP_NUM_THREADS=1
                launching $BENCHMARK.$CLASS.$MPIS $MPIS
            fi
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

            if [ "$BENCHS_MODE" == "compile" ]; then
                mkdir -p $BENCH_SRC_PATH/bin
                cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS NPROCS=$MPIS
                mv $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.$MPIS $BENCHS_BIN_PATH
            fi
            if [ "$BENCHS_MODE" == "test" ]; then
                launching $BENCHMARK.$CLASS.$MPIS $MPIS
            fi
        ;;
        ua)
            configuring $1 $BENCHS_SRC_PATH/NPB3.3.1/NPB3.3-OMP $2 1

            if [ "$BENCHS_MODE" == "compile" ]; then
                mkdir -p $BENCH_SRC_PATH/bin
                cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS
                mv $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.x $BENCHS_BIN_PATH
            fi
            if [ "$BENCHS_MODE" == "test" ]; then
                export OMP_NUM_THREADS=$CORES
                launching $BENCHMARK.$CLASS.x $MPIS
            fi
        ;;
        dgemm)
            configuring mpi $BENCHS_SRC_PATH/DGEMM/mkl_fortran_samples/matrix_multiplication X 1

            if [ "$BENCHS_MODE" == "compile" ]; then
                mkdir -p $BENCH_SRC_PATH/release
                cd $BENCH_SRC_PATH;make $BENCHMARK
                mv $BENCH_SRC_PATH/release/dgemm_example $BENCHS_BIN_PATH
            fi
            if [ "$BENCHS_MODE" == "test" ]; then
                export KMP_AFFINITY=granularity=fine,compact,1,0
                export MKL_NUM_THREADS=$CORES
                launching dgemm_example $MPIS
            fi
        ;;
        stream)
            configuring stream_mpi $BENCHS_SRC_PATH/STREAM X $CORES

            if [ "$BENCHS_MODE" == "compile" ]; then
                cd $BENCH_SRC_PATH;make $BENCHMARK
                mv $BENCH_SRC_PATH/stream_mpi $BENCHS_BIN_PATH
            fi
            if [ "$BENCHS_MODE" == "test" ]; then
                export OMP_NUM_THREADS=1
                launching stream_mpi $MPIS
            fi
        ;;
        *)
            echo "Invalid benchmark"
            exit 1
    esac
}
