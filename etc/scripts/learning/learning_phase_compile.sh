#!/bin/bash

# Edit architecture values
export CORES=16
export SOCKETS=2
export CORES_PER_SOCKET=8

# Update paths
export EAR_SRC_PATH=$HOME/git/EAR

# Non edit region
source $EAR_INSTALL_PATH/etc/scripts/learning/helpers/kernels_executor.sh
export BENCHS_MODE="compile"

# Compiling or executing the different kernels
learning_phase lu-mpi C
learning_phase ep D
learning_phase bt-mz C
learning_phase sp-mz C
learning_phase lu-mz C
learning_phase ua C
learning_phase dgemm
learning_phase stream
