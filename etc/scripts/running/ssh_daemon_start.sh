#/bin/bash

if [[ $# -ne 3 ]]
then
	echo -e "Usage: ssh_daemon_start.sh hostlist p_state"
	echo -e "\thostlist: a host name list file, a host name list or 'local'"
	echo -e "\tp_state: the maximum P_STATE which the daemon can set of the whole range"
	echo -e "\tdb_pathname: the path where de job DBs will be saved"
	exit 1
fi

if [ ! -f $1 ]
then
        if [ "x$1" != "xlocal" ]
        then
        	export HOSTLIST="$1"
        else 
		export HOSTLIST="$(hostname -s)"
	fi
else
	export HOSTLIST="$(echo $(cat $1))"
fi

if [ -z $EAR_INSTALL_PATH ]
then
	echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
	exit 1
fi

if [ -z $EAR_TMP ]
then
	echo -e "ERROR: EAR_TMP environment variable is not set."
	exit 1
fi

if [ -z $EAR_VERBOSE ]
then
	echo -e "ERROR: EAR_VERBOSE environment variable is not set. Using 0."
	export EAR_VERBOSE=0	
fi

for i in ${HOSTLIST}
do
    echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"

	if [ "x$1" == "xlocal" ]
	then
		mkdir -p ${EAR_TMP}
		sudo LD_LIBRARY_PATH="${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${CPUPOWER_LIB_PATH}:${LD_LIBRARY_PATH}" \
                EAR_DB_PATHNAME=${3} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE}
	else
		ssh ${i} mkdir -p ${EAR_TMP}
	 	ssh ${i} sudo LD_LIBRARY_PATH="${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${CPUPOWER_LIB_PATH}:${LD_LIBRARY_PATH}" \
        	EAR_DB_PATHNAME=${3} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE} &
	fi
done
