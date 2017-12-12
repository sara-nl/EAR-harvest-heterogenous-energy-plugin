#!/bin/bash

# Edit architecture values
export CORES=28
export SOCKETS=2
export CORES_PER_SOCKET=14

# Edit learning phase parameters
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=1

# Non edit region
source $EAR_INSTALL_PATH/etc/scripts/learning/learning_phase_helper.sh
source $EAR_INSTALL_PATH/etc/scripts/environment/lib_vars.sh
source $EAR_INSTALL_PATH/etc/scripts/environment/ear_vars.sh
export EAR_LEARNING_PHASE=1
export BENCHS_MODE="test"

# Running the learning phase
for (( i=$EAR_MIN_P_STATE; i<=$EAR_MAX_P_STATE; i++ ))
do
    export EAR_P_STATE=$i
    learning_phase lu-mpi C
    learning_phase ep D
    learning_phase bt-mz C
    learning_phase sp-mz C
    learning_phase lu-mz C
    learning_phase ua C
    learning_phase dgemm
    learning_phase stream
done

# Calculating coefficients
$EAR_INSTALL_PATH/bin/compute_coefficients "$EAR_DB_PATHNAME.`hostname`.db" \
     $EAR_COEFF_DB_PATHNAME`hostname` 1000000 `hostname`
