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

for i in ${HOSTLIST}
do
    color=$[$color + 1]

    if [[ -n $2 ]]
    then
        if [[ -n $3 ]]
        then
            $EAR_INSTALL_PATH/bin/tools/learning_show ${i} -P ${color} | grep -E "$2.*$3|$3.*$2"
        else
            $EAR_INSTALL_PATH/bin/tools/learning_show ${i} -P ${color} | grep "$2"
        fi
    else
        $EAR_INSTALL_PATH/bin/tools/learning_show ${i} -P ${color}
    fi
done
