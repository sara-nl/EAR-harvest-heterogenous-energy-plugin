#!/bin/bash

export PAPIROOT=/hpc/base/ctt/packages/papi/5.4.3
export FREEIPMIROOT=/home/xjcorbalan


function mpirun_min_energy() { ### function to execute Intel_MPI code with EAR

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with EAR (energy policy=min_energy_to_solution) \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=1


### BSC-Lenovo  Energy aware library:
local a=$(date +%s) ; date

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf.sh
export EAR_POWER_POLICY=MIN_ENERGY_TO_SOLUTION
export EAR_PERFORMANCE_PENALTY=0.1

export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`

HOSTLIST=$(echo $(cat ./hostlist))

## Sarting EAR daemon: 1 ear_daemon per node
for i in ${HOSTLIST}
do
echo "Executing ear_daemon in node=${i} p_state=${def_freq} tmpdir=${EAR_TMP} verbose=${EAR_VERBOSE}"
ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMIROOT}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &
done
sleep 1

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_GUI_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
for i in ${HOSTLIST}
do
ssh ${i} $EAR_INSTALL_PATH/bin/ear_daemon_exit ${EAR_TMP}
done

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}  #############################

function mpirun_min_time() { ### function to execute Intel_MPI code with EAR

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with EAR (energy policy=min_time_to_solution)\n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=4


### BSC-Lenovo  Energy aware library:
local a=$(date +%s) ; date

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf.sh
export EAR_POWER_POLICY=MIN_TIME_TO_SOLUTION
export EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN=0.75
export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`

HOSTLIST=$(echo $(cat ./hostlist))

## Sarting EAR daemon: 1 ear_daemon per node
for i in ${HOSTLIST}
do
ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMIROOT}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &
done
sleep 1

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
for i in ${HOSTLIST}
do
ssh ${i} $EAR_INSTALL_PATH/bin/ear_daemon_exit ${EAR_TMP}
done

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}  #############################

function mpirun_ear_monitoring() { ### function to execute Intel_MPI code with EAR

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with EAR (energy policy=min_time_to_solution)\n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=4


### BSC-Lenovo  Energy aware library:
local a=$(date +%s) ; date

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf.sh
export EAR_POWER_POLICY=MONITORING_ONLY
export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`

HOSTLIST=$(echo $(cat ./hostlist))

## Sarting EAR daemon: 1 ear_daemon per node
for i in ${HOSTLIST}
do
ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMIROOT}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &
done
sleep 1

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_GUI_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
for i in ${HOSTLIST}
do
echo "ear_daemon_exit"
ssh ${i} $EAR_INSTALL_PATH/bin/ear_daemon_exit ${EAR_TMP}
done

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}  #############################


function mpirun_noear() { ### function to execute Intel_MPI code without EAR, just to compare

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code  \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn def_freq "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}

local a=$(date +%s) ; date 
## Starting the application
mpiexec.hydra -l  -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
local b=$(date +%s) ; date 
echo "Total elapsed time = $(( b - a )) sec "
}  #############################





