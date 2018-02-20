#/bin/bash

if [[ $# -ne 2 ]]
then
	echo -e "Usage: ssh_daemon_start.sh hostlist p_state"
	echo -e "\thostlist: a host name list file, a host name list or 'local'"
	echo -e "\tp_state: the maximum P_STATE which the daemon can set of the whole range"
	exit 1
fi

if [ -z $EAR_INSTALL_PATH ]
then
        echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set. Install EAR and"
        echo -e "load EAR environment module or export EAR_INSTALL_PATH in your .bashrc."
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

source $EAR_INSTALL_PATH/etc/scripts/environment/lib_vars.sh
source $EAR_INSTALL_PATH/etc/scripts/environment/ear_vars.sh

for i in ${HOSTLIST}
do
    echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"

	if [ "x$1" == "xlocal" ]
	then
		mkdir -p ${EAR_TMP}
		sudo LD_LIBRARY_PATH="${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${CPUPOWER_LIB_PATH}:${LD_LIBRARY_PATH}" \
                EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE}
	else
		ssh ${i} mkdir -p ${EAR_TMP}
	 	ssh ${i} sudo LD_LIBRARY_PATH="${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${CPUPOWER_LIB_PATH}:${LD_LIBRARY_PATH}" \
        	EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE} &
	fi
done
