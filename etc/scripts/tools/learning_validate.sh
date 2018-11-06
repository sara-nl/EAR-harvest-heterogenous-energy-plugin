#!/bin/bash

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

#
export HOSTLIST="$(echo $(cat $1))"

#
color="0"

$EAR_INSTALL_PATH/bin/tools/learning_validate nonode -G -H

for i in ${HOSTLIST}
do
	$EAR_INSTALL_PATH/bin/tools/learning_validate -G
done