#!/bin/bash
if [[ $# -eq 0 ]]
then
	echo "usage: ./ear_daemon_start.sh hostlist def_pstate"
	exit
fi

HOSTLIST=$(echo $(cat $1))
export PAPIROOT=/hpc/base/ctt/packages/papi/5.4.3
export FREEIPMIROOT=/home/xjcorbalan


## Sarting EAR daemon: 1 ear_daemon per node
for i in ${HOSTLIST}
do
echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"
ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMIROOT}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon $2 ${EAR_TMP} ${EAR_VERBOSE} &
done

