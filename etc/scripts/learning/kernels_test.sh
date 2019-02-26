#!/bin/bash

if [[ $# -eq 0 ]]
then
        echo -e "Usage: hostlist"
        echo -e "    hostlist: a hostname list of the nodes"
        exit 1
fi

if [ ! -f $1 ]
then
    echo "Error, thie nodelist file doesn't exists"
fi

if [ -z $EAR_INSTALL_PATH ]
then
    echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

# Edit architecture values
export CORES=24
export SOCKETS=2
export CORES_PER_SOCKET=12

# Edit learning phase parameters
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=1
export EAR_TIMES=1

# Edit output options
export OUT_OUT="$HOME/out"
export OUT_ERR="$HOME/err"

# Non-edit region
export HOSTLIST="$(echo $(cat $1))"
export BENCHS_MODE="test"

for i in ${HOSTLIST}
do
	echo "Executing learning phase in node=${i}"
	sbatch -w ${i} -N 1 -n 1 --exclusive -o $OUT_OUT.${i} -e $OUT_ERR.${i} \
		$EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_iterator.sh
done
