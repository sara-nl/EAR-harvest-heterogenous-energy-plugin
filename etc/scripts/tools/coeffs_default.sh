#!/bin/bash

# Edit region
export COEFFS_PATH="${EAR_ETC}/ear/coeffs/island0/"

if [[ $# -ne 1 ]]
then
        echo -e "Usage: hostlist"
        echo -e "\thostlist: a host name list file"
        exit 1
fi

if [ ! -f $1 ]
then
    echo -e "Error, the nodelist file doesn't exists"
	exit -1
fi

if [ -z $EAR_INSTALL_PATH ]
then
    echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

if [ -z $EAR_ETC ]
then
    echo -e "ERROR: EAR_ETC environment variable is not set."
    echo -e "TIP! Load the EAR environment module."
    exit 1
fi

#
export HOSTLIST="$(echo $(cat $1))"
export HOSTLIST_PLAIN=""

# Converting a line break list into a space splitted list
for i in ${HOSTLIST}
do
	export HOSTLIST_PLAIN="${i} $HOSTLIST_PLAIN"
done

$EAR_INSTALL_PATH/bin/tools/coeffs_default ${COEFFS_PATH} ${HOSTLIST_PLAIN}