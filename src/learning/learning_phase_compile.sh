#!/bin/bash

# Edit architecture values
export CORES=28
export SOCKETS=2
export CORES_PER_SOCKET=14

# Update paths
export EAR_SRC_PATH=$HOME/git/EAR

# Non edit region
source $EAR_INSTALL_PATH/etc/scripts/learning/learning_phase_helper.sh
export BENCHS_MODE=$1
export EAR_P_STATE=1

if [ "$1" != "compile" ] && [ "$1" != "test" ]; then
    echo $"Usage: $0 {compile|test}"
    exit 0
fi

# Compiling or executing the different kernels
learning_phase lu-mpi C
learning_phase ep D
learning_phase bt-mz C
learning_phase sp-mz C
learning_phase lu-mz C
learning_phase ua C
learning_phase dgemm
learning_phase stream
