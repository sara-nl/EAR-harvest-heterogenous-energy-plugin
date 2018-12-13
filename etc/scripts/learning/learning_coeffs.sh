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

# Edit minimum frequency (in KHz)
export MIN_FREQUENCY=1900000

# Edit output options
export OUT_COE="$HOME/ear_coeffs"
mkdir $OUT_COE

# Non-edit region
export HOSTLIST="$(echo $(cat $1))"

for i in ${HOSTLIST}
do
    echo "Computing coefficients for node=${i}"
	$EAR_INSTALL_PATH/sbin/coeffs_compute $OUT_COE $MIN_FREQUENCY ${i}
done
