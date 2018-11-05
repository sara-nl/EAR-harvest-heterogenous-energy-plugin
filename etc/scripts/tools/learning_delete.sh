#!/bin/bash

# Edit region
export PASSWORD="--password=EAR"

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

for i in ${HOSTLIST}
do
    echo "Cleaning learning phase of node ${i}"
    $EAR_INSTALL_PATH/bin/tools/learning_delete ${i} -p ${PASSWORD}
done