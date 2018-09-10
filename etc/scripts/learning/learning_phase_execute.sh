#!/bin/bash

if [[ $# -ne 1 ]]
then
        echo -e "Usage: hostlist"
        echo -e "\thostlist: a host name list file"
        exit 1
fi

if [ ! -f $1 ]
then
    echo "Error, this nodelist file doesn't exists"
fi

if [ -z $EAR_INSTALL_PATH ]
then
    echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

# Edit architecture values
export CORES=16
export SOCKETS=2
export CORES_PER_SOCKET=8

# Edit learning phase parameters
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=6
export EAR_TIMES=3

# Edit output files
export ERR = $HOME/lp_err
export OUT = $HOME/lp_out

# Non-edit region
export HOSTLIST="$(echo $(cat $1))"
export EAR_LEARNING_PHASE=1
export BENCHS_MODE="test"

for i in ${HOSTLIST}
do
    echo "Executing learning phase in node in node=${i}"
	sbatch -w ${i} -N 1 -n 1 --exclusive -o $ERR.${i}.txt -e $OUT.${i}.txt $EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_iterator.sh
done
