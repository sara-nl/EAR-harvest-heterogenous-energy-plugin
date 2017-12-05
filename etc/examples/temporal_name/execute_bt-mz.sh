#!/bin/bash
if [ "$#" -eq "0" ]
then
echo "$0 freq sockets cores_per_socket total_cpus mode[ear|baseline|lp|lp_turbo]"
exit 1
fi



hostname  > hostlist

export NUMSOCKETS=$2
export CORES_PER_SOCKET=$3
export NUMNODES=1
export MPIS=$4
export THREADS=1
export NODENAME=`hostname`
export FREQ=$1
export EAR_POWER_POLICY=MIN_ENERGY_TO_SOLUTION
export EAR_PERFORMANCE_PENALTY=0.1
export EAR_APP_NAME=bt-mz
export BENCHMARK=$EAR_INSTALL_PATH/bin/kernels/bt-mz.C.$MPIS
export MKL_NUM_THREADS=$THREADS
export OMP_NUM_THREADS=$THREADS

mkdir -p EAR_OUTS
export EAR_USER_DB_PATHNAME=EAR_OUTS/${EAR_APP_NAME}.${FREQ}.${EAR_POWER_POLICY}.${EAR_PERFORMANCE_PENALTY}
echo "Executing bt-mz(with EAR) with $MPIS mpis and $THREADS threads in $NODENAME at f=$FREQ"

# #adding bash tool library
source $EAR_INSTALL_PATH/conf/EAR_scripts.sh
source $EAR_INSTALL_PATH/ear_scripts/Bash_utils.sh

source /hpc/base/intel/parallel_studio_xe_2017.1.043/bin/psxevars.sh intel64

## INTELMPI Environment variables :
myimpi_default_settings

## IntelMPI protocol settings:
myimpi_protocol tmi

## IntelMPI Binding
## NumberOfSocketsPerNode NumberOfNumaNodesPerSocket NumberOfCoresPerSocket HyperThreads NumberOfMpiPerNode NumberOfOpenmpPerMPI NumberOfSocketsUsed HyperThreadsUsed
myimpi_bind_intel2 $NUMSOCKETS 1 $CORES_PER_SOCKET 2 $MPIS $THREADS 2 1

## ROMIO tuning
myimpi_io_settings


echo "-------------------${EAR_PERFORMANCE_PENALTY} policy ${EAR_POWER_POLICY}-----------------------"  
echo "Executing $BENCHMARK with $MPIS mpis and $MPIS ppn , starting at freq=$FREQ"
case $5 in
	ear)
	myimpi_execute_with_ear $BENCHMARK $MPIS $MPIS  ${FREQ}
	;;
	baseline)
	myimpi_execute_baseline $BENCHMARK $MPIS $MPIS  ${FREQ}
	;;
	lp)
	myimpi_execute_LP $BENCHMARK $MPIS $MPIS  ${FREQ}
	;;
	lp_turbo)
	myimpi_execute_LP_turbo $BENCHMARK $MPIS $MPIS  ${FREQ}
	;;
	*) 
	echo "Invalid option"
	;;
esac
	
	

