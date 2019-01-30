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
export MIN_FREQUENCY=1700000

# Edit output options
export OUT_COE="$HOME/EAR/etc/ear/coeffs"
mkdir $OUT_COE
export OUT_OUT="$HOME/OUT/out"
export OUT_ERR="$HOME/ERR/err"
mkdir $HOME/OUT
mkdir $HOME/ERR

# Non-edit region
export HOSTLIST="$(echo $(cat $1))"

for i in ${HOSTLIST}
do
    echo "Computing coefficients for node=${i}"
        sbatch  -w ${i} -N 1 -n 1 --exclusive -o $OUT_OUT.coeffs.${i} -e $OUT_ERR.coeffs.${i} \
        $EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_coeffs.sh

done
