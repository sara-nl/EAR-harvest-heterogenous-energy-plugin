#!/bin/bash
if [ "$#" -eq "0" ]
then
	echo "./compile.sh [lu-mpi|ep|bt-mz|sp-mz|lu-mz|ua|dgemm|stream] sockets cores_per_socket total_cpus"
	exit 1
fi

export EAR_SRC_PATH=$HOME/git/EAR
export BENCHS_ROOT_SRC_PATH=$EAR_SRC_PATH/ear_learning_phase/kernels
export BENCHS_ROOT_BIN_PATH=$EAR_INSTALL_PATH/bin/kernels

export SOCKETS=$2
export CORES_PER_SOCKET=$3
export CPUS=$4

function compiling 
{
	export BENCHMARK=$1
	echo "-----------------------------"
	echo "Compiling $1"
	echo "-----------------------------"
}

function configuring
{
	export BENCH_SRC_PATH=$1
	export CLASS=$2
	export MPIS=$3
} 

case "$1" in
    lu-mpi|ep)

		case $1 in
			ep)
    			compiling $1
				configuring $BENCHS_ROOT_SRC_PATH/NPB3.3.1/NPB3.3-MPI D $CPUS
			;;
			lu-mpi)
			    compiling lu
				configuring $BENCHS_ROOT_SRC_PATH/NPB3.3.1/NPB3.3-MPI C $CPUS
			;;
		esac

        mkdir -p $BENCH_SRC_PATH/bin
		cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS NPROCS=$MPIS
		cp $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.$MPIS $BENCHS_ROOT_BIN_PATH
	;;
	lu-mz|sp-mz|bt-mz)
		compiling $1

		case $BENCHMARK in
			lu-mz)
				configuring $BENCHS_ROOT_SRC_PATH/NPB3.3.1-MZ/NPB3.3-MZ-MPI C 2
			;;
			*)
				configuring $BENCHS_ROOT_SRC_PATH/NPB3.3.1-MZ/NPB3.3-MZ-MPI C $CPUS
			;;
		esac

        mkdir -p $BENCH_SRC_PATH/bin
		cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS NPROCS=$MPIS
		cp $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.$MPIS $BENCHS_ROOT_BIN_PATH
	;;
	ua)
		compiling $1
		configuring $BENCHS_ROOT_SRC_PATH/NPB3.3.1/NPB3.3-OMP C 1

        mkdir -p $BENCH_SRC_PATH/bin
		cd $BENCH_SRC_PATH;make $BENCHMARK CLASS=$CLASS
		cp $BENCH_SRC_PATH/bin/$BENCHMARK.$CLASS.x $BENCHS_ROOT_BIN_PATH
	;;
	dgemm)
		compiling mpi
        configuring $BENCHS_ROOT_SRC_PATH/DGEMM/mkl_fortran_samples/matrix_multiplication X $SOCKETS

        mkdir -p $BENCH_SRC_PATH/release
		cd $BENCH_SRC_PATH;make $BENCHMARK
		cp $BENCH_SRC_PATH/release/dgemm_example $BENCHS_ROOT_BIN_PATH
	;;
	stream)
		compiling stream_mpi
	    configuring $BENCHS_ROOT_SRC_PATH/STREAM X 1

		cd $BENCH_SRC_PATH;make $BENCHMARK
		cp $BENCH_SRC_PATH/stream_mpi $BENCHS_ROOT_BIN_PATH
	;;
	*)
		echo $"Usage: $0 {lu-mpi|ep|bt-mz|sp-mz|lu-mz|ua|dgemm|stream} version"
        	exit 1
	
esac
