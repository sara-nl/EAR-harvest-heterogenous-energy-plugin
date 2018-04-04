#!/bin/bash

if [[ $# -ne 6 ]]
then
	echo -e "Usage: mpi_exec.sh hostlist binary nmpi ppn policy trace"
	echo -e "\thostlist: a host name list file, a comma separated host name list or 'local'"
	echo -e "\tbinary: name of local binary to run"
	echo -e "\tmpi: number of total MPI tasks to use"
	echo -e "\tppn: number of MPI tasks per node"
  	echo -e "\tpolicy: MIN_ENERGY_TO_SOLUTION | MIN_TIME_TO_SOLUTION | MONITORING_ONLY | NO_EAR"
	echo -e "\ttrace: NO_TRACE | APPLICATION | DYNAIS
  	exit 1
fi

if [ -z $EAR_INSTALL_PATH ]
then
    echo -e "ERROR: EAR_INSTALL_PATH environment variable is not set."
    echo -e "Load the EAR environment module with 'module load ear'."
    exit 1
fi

if [[ "$5" != "MIN_ENERGY_TO_SOLUTION" ]] && [[ "$5" != "MIN_TIME_TO_SOLUTION" ]]  &&
   [[ "$5" != "MONITORING_ONLY" ]] && [[ "$5" != "NO_EAR" ]]
then
	echo "ERROR: bad policy $5."
	exit 1
else
	export EAR_POWER_POLICY="$5"
fi

if [ ! -f $1 ]
then
	if [ "x$1" != "xlocal" ]
	then
		MPI_HOST="-hosts $1"
		export EAR_NUM_NODES=`echo $1 | awk -F "," "{print NF}"`	
	else
		MPI_HOST=""
		export EAR_NUM_NODES=1
	fi
else 
	MPI_HOST="-f $1"
	export EAR_NUM_NODES=`wc -l < $1`
fi

# LD_PRELOAD if NO_EAR policy isn not selected
if [[ "$5" != "NO_EAR" ]]
then
	if [[ "x$5" == "NO_TRACE" ]]
	then
		PRELOAD="-genv LD_PRELOAD=${EAR_LIB_PATH}"
	else
    	PRELOAD="-genv LD_PRELOAD=${EAR_TRACE_LIB_PATH}"
	fi
fi

# Non-edit region

BINARY=${2}
MPI=${3}
PPN=${4}

### BSC-Lenovo  Energy aware library:
A=$(date +%s) ; date

## Starting the application
mpiexec.hydra -l $PRELOAD -genvall ${MPI_HOST} -n ${MPI} -ppn=${PPN} ${BINARY}

B=$(date +%s) ; date
echo "Total elapsed time = $(( B - A )) sec"
