#!/bin/bash

function refresh_vars()
{
source environment/ear_env_libs.sh
source environment/ear_env_conf.sh
export HOSTLIST="$(echo $(cat hostlist))"
}

function daemon_ssh_start()
{
refresh_vars

for i in ${HOSTLIST}
do
        echo "Executing ear_daemon in node=${i} p_state=$2 tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"
        ssh ${i} sudo LD_LIBRARY_PATH="${CPUPOWER_LIB_PATH}:${FREEIPMI_LIB_PATH}:${PAPI_LIB_PATH}:${LD_LIBRARY_PATH}" \
        EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/sbin/eard $1 ${EAR_TMP} ${EAR_VERBOSE} &
done
}

function daemon_ssh_exit()
{
refresh_vars

for i in ${HOSTLIST}
do
        DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep eard | awk '{print $1}'`
        echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"
        ssh $i sudo LD_LIBRARY_PATH="" kill -TERM $DAEMON_PID &
done
}

function mpirun_with_ear() { ### function to execute Intel_MPI code with EAR
if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with EAR\n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn policy"
  echo -e "\t\t\tbinary: name of local binary to run"
  echo -e "\t\t\tmpi: number of total MPI tasks to use"
  echo -e "\t\t\tppn: number of MPI tasks per node"
  echo -e "\t\t\tpolicy: 0 (min_energy) | 1 (min_time) | 2 (monitoring_only)"
  echo -e "\n\n"
return 0
fi

refresh_vars

#
export EAR_POWER_POLICY=MONITORING_ONLY
if [ $4 -eq 0 ]
then
	export EAR_POWER_POLICY=MIN_ENERGY_TO_SOLUTION
fi
if [ $4 -eq 1 ]
then
	export EAR_POWER_POLICY=MIN_TIME_TO_SOLUTION
fi

# Non-edit region
local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=1

### BSC-Lenovo  Energy aware library:
local a=$(date +%s) ; date

## Sarting EAR daemon: 1 ear_daemon per node
daemon_ssh_start ${def_freq}

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=${EAR_LIB_PATH} -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
daemon_ssh_exit

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}
