#!/bin/bash

source $EAR_INSTALL_PATH/etc/examples/environment/ear_env_libs.sh
source $EAR_INSTALL_PATH/etc/examples/environment/ear_env_conf.sh
export HOSTLIST="$(echo $(cat $1))"

for i in ${HOSTLIST}
do
        echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"
        ssh ${i} sudo LD_LIBRARY_PATH="${CPUPOWER_LIB_PATH}:${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${LD_LIBRARY_PATH}" \
        EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/sbin/eard $2 ${EAR_TMP} ${EAR_VERBOSE} &
done
