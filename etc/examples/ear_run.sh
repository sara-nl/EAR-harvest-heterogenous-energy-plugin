#!/bin/bash

function mpirun_with_ear() { ### function to execute Intel_MPI code with EAR
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

# Edit region
export EAR_POWER_POLICY=MONITORING_ONLY
export EAR_LIBRARY_GUI_PATH=$EAR_INSTALL_PATH/lib/libear.so

# Non-edit region
source ear_env_libs.sh
source ear_env_conf.sh
HOSTLIST=$(echo $(cat ./hostlist))

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local def_freq=1

### BSC-Lenovo  Energy aware library:
local a=$(date +%s) ; date


## Sarting EAR daemon: 1 ear_daemon per node
source ear_daemon_start.sh hostlist ${def_freq}
sleep 4

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=$EAR_LIBRARY_GUI_PATH -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

## Just in case the app fails, we finish EAR demon
##source ear_daemon_exit.sh hostlist

local b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
}
