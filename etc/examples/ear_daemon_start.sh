#!/bin/bash
if [[ $# -eq 0 ]]
then
	echo "usage: ./ear_daemon_start.sh hostlist def_pstate"
	exit
fi

source ear_env.sh
HOSTLIST=$(echo $(cat $1))

## Sarting EAR daemon: 1 ear_daemon per node
for i in ${HOSTLIST}
do
	echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"
	ssh ${i} sudo LD_LIBRARY_PATH="$CPUPOWER_LIB_PATH:$FREEIPMI_LIB_PATH:$PAPI_LIB_PATH:$LD_LIBRARY_PATH" \
	EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE} &
done
