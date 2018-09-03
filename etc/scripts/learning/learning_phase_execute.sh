#!/bin/bash

if [[ $# -ne 1 ]]
then
        echo -e "Usage: hostlist"
        echo -e "\thostlist: a host name list file"
        exit 1
fi

if [ ! -f $1 ]
then
    echo "Error, thie nodelist file doesn't exists"
fi

# Edit architecture values
export CORES=16
export SOCKETS=2
export CORES_PER_SOCKET=8

# Edit learning phase parameters
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=6
export EAR_TIMES=3

# Non-edit region
export HOSTLIST="$(echo $(cat $1))"

for i in ${HOSTLIST}
do
    echo "Executing learning phase in node in node=${i}"
	srun -w ${i} -N 1 -n 1 --exclusive $PWD/helpers/kernels_iterator.sh
done
