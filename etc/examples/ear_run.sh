#!/bin/bash

if [[ $# -ne 4 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with EAR\n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn policy"
  echo -e "\t\t\tbinary: name of local binary to run"
  echo -e "\t\t\tmpi: number of total MPI tasks to use"
  echo -e "\t\t\tppn: number of MPI tasks per node"
  echo -e "\t\t\tpolicy: MIN_ENERGY_TO_SOLUTION | MIN_TIME_TO_SOLUTION | MONITORING_ONLY"
  echo -e "\n\n"
  exit 1
fi

source environment/ear_env_libs.sh
source environment/ear_env_conf.sh
export HOSTLIST="$(echo $(cat hostlist))"

#
if [[ "$4" != "MONITORING_ONLY" ]] && [[ "$4" != "MIN_ENERGY_TO_SOLUTION" ]] && [[ "$4" != "MIN_TIME_TO_SOLUTION" ]]
then
	echo "ERROR: bad policy $4"
	exit 1
fi
export EAR_POWER_POLICY="$4"

if [ ! -f hostlist ]
then
	echo "ERROR: the host list file 'hostlist' does not exist"
	exit 1
fi

# Non-edit region
binary=${1}
mpi=${2}
ppn=${3}
def_freq=1

### BSC-Lenovo  Energy aware library:
a=$(date +%s) ; date

## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=${EAR_LIB_PATH} -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}

b=$(date +%s) ; date
echo "Total elapsed time = $(( b - a )) sec "
