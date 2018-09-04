#!/bin/bash

# Edit architecture values
export CORES=16
export SOCKETS=2
export CORES_PER_SOCKET=8

# Non edit region
source $EAR_INSTALL_PATH/etc/scripts/learning/helpers/kernels_executor.sh
export BENCHS_MODE="test"
export EAR_P_STATE=1

if [ "$1" != "ear" ] && [ "$1" != "no_ear" ]; then
    echo $"Usage: $0 {ear | no_ear}"
    exit 0
fi

if [ "$1" == "no_ear" ]; then
	export EAR_POWER_POLICY="NO_EAR"
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
