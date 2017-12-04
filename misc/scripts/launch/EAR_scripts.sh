#!/bin/bash

export PAPIROOT=/hpc/base/ctt/packages/papi/5.4.3
export FREEIPMIROOT=/home/xjcorbalan

function myimpi_execute_with_ear() { ### function to execute Intel_MPI code with EAR

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with BSC-Lenovo EAR \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn def_freq "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\t\t\tdef_freq : default_pstate "
  echo -e "\n\t Please note:"
  echo -e "\t\t - IntelMPI environment, binding... MUST be set before calling this function."
  echo -e "\t\t - standard and error outputs will not be redirected "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=${4:-"1"}

which mpiexec.hydra >/dev/null
if [[ $? -eq 1 ]]
then
  echo "Please load IntelMPI environment, mpiexec.hydra not found !"  
  return
fi

### BSC-Lenovo  Energy aware library:
## save environment used during the run
env > log.env 2>&1
local a=$(date +%s) ; date

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf.sh
export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`

HOSTLIST=$(echo $(cat ./hostlist))

## Sarting EAR daemon: 1 ear_daemon per node
echo "starting ear daemon"
for i in ${HOSTLIST}
do
echo "Executing ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMI}/lib $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &"

ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMI}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &
done
sleep 2
echo "starting ${binary} in $EAR_NUM_NODES nodes"

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
echo "forcing exiting ear daemon"
for i in ${HOSTLIST}
do
ssh ${i} $EAR_INSTALL_PATH/bin/ear_daemon_exit ${EAR_TMP}
done

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}  #############################

function myimpi_execute_baseline() { ### function to execute Intel_MPI code without EAR, just to compare

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code  \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn def_freq "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\t\t\tdef_freq : default_frequency "
  echo -e "\n\t Please note:"
  echo -e "\t\t - IntelMPI environment, binding... MUST be set before calling this function."
  echo -e "\t\t - standard and error outputs will not be redirected "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=${4:-"1"}

which mpiexec.hydra >/dev/null
if [[ $? -eq 1 ]] 
then
  echo "Please load IntelMPI environment, mpiexec.hydra not found !"  
  return
fi

env > log.env 2>&1 
local a=$(date +%s) ; date 
## Starting the application
mpiexec.hydra -l  -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
local b=$(date +%s) ; date 
echo "Total elapsed time = $(( b - a )) sec "
}  #############################

function myimpi_execute_LP() { ### function to execute Intel_MPI code with EAR to compute the learning phase

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with BSC-Lenovo EAR \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn def_freq "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\t\t\tdef_freq : default_frequency "
  echo -e "\n\t Please note:"
  echo -e "\t\t - IntelMPI environment, binding... MUST be set before calling this function."
  echo -e "\t\t - standard and error outputs will not be redirected "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=${4:-"1"}

which mpiexec.hydra >/dev/null
if [[ $? -eq 1 ]] 
then
  echo "Please load IntelMPI environment, mpiexec.hydra not found !"  
  return
fi

### BSC-Lenovo  Energy aware library:
## save environment used during the run
env > log.env 2>&1 
local a=$(date +%s) ; date 

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf_learning_phase.sh
export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`

HOSTLIST=$(echo $(cat ./hostlist))

## Sarting EAR daemon
for i in ${HOSTLIST}
do
ssh ${i} sudo LD_LIBRARY_PATH=${PAPIROOT}/lib:${FREEIPMIROOT}/lib EAR_DB_PATHNAME=${EAR_DB_PATHNAME} $EAR_INSTALL_PATH/bin/ear_daemon ${def_freq} ${EAR_TMP} ${EAR_VERBOSE} &
done
sleep 1
## Starting the application
export EAR_P_STATE=${def_freq}
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
## Just in case the app fails, we finish EAR demon
for i in ${HOSTLIST}
do
ssh ${i} $EAR_INSTALL_PATH/bin/ear_daemon_exit ${EAR_TMP} 
done

local b=$(date +%s) ; date 
echo "Total elapsed time = $(( b - a )) sec "
}  #############################

function myimpi_execute_LP_turbo() { ### function to execute Intel_MPI code with EAR to compute the learning phase for turbo

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with BSC-Lenovo EAR \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn def_freq "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\t\t\tdef_freq : default_frequency "
  echo -e "\n\t Please note:"
  echo -e "\t\t - IntelMPI environment, binding... MUST be set before calling this function."
  echo -e "\t\t - standard and error outputs will not be redirected "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=${4:-"1"}

which mpiexec.hydra >/dev/null
if [[ $? -eq 1 ]] 
then
  echo "Please load IntelMPI environment, mpiexec.hydra not found !"  
  return
fi

### BSC-Lenovo  Energy aware library:
## save environment used during the run
env > log.env 2>&1 
local a=$(date +%s) ; date 

### EAR variables
source $EAR_INSTALL_PATH/conf/ear_conf_learning_phase_turbo.sh
export EAR_NUM_NODES=`expr ${mpi} / ${ppn}`
HOSTLIST=$(echo $(cat ./hostlist))
## Sarting EAR daemon
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
ssh ${i} ${EAR_INSTALL_PATH}/bin/ear_daemon_exit ${EAR_TMP}
done

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}  #############################



