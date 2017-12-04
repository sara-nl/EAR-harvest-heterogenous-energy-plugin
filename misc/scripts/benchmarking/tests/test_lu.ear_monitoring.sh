#!/bin/bash

hostname  > hostlist

export NUMSOCKETS=2
export CORES_PER_SOCKET=14
export NUMNODES=1
export MPIS=28
export THREADS=1
export NODENAME=`hostname`

export EAR_APP_NAME=lu
mkdir -p $HOME/.EAR/EAR_OUTS
export EAR_USER_DB_PATHNAME=$HOME/.EAR/EAR_OUTS/${EAR_APP_NAME}

export BENCHMARK=$EAR_INSTALL_PATH/bin/kernels/lu.C.$MPIS
export OMP_NUM_THREADS=$THREADS
echo "Executing lu(with EAR,MIN_ENERGY_TO_SOLUTION) with $MPIS mpis and $THREADS threads in $NODENAME"

# Loading EAR bash file with utils functions
source ./EAR_utils_functions.sh

#This part is system specific
source $EAR_INSTALL_PATH/ear_scripts/Bash_utils.sh
source /hpc/base/intel/parallel_studio_xe_2017.1.043/bin/psxevars.sh intel64
myimpi_default_settings
myimpi_protocol tmi
myimpi_bind_intel2 $NUMSOCKETS 1 $CORES_PER_SOCKET 2 $MPIS $THREADS 2 1
myimpi_io_settings

mpirun_ear_monitoring $BENCHMARK $MPIS $MPIS  

