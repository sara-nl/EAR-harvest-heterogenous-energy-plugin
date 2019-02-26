#!/bin/bash

if [ -z $EAR_INSTALL_PATH ]
then
    echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

if [ -z $EAR_SOURCE_PATH ]
then
    echo -e "ERROR: EAR_SOURCE_PATH environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

# Edit architecture values
export CORES=24
export SOCKETS=2
export CORES_PER_SOCKET=12

# Non-edit region
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=1
export EAR_TIMES=1

# Non-edit region
export BENCHS_MODE="compile"

$EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_iterator.sh
