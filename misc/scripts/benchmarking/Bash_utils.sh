#!/bin/bash

function myusage() {

cat <<EOF
########################################################################
########################################################################
###    This Easy_Bashlib.sh is a library of BASH functions
###    Purpose is make life easier when benchmarking HPC codes
###    Authors : 
###      Eric Michel      - emichel@lenovo.com
###      Olivier Lagrasse - olagrasse@lenovo.com
########################################################################
current content of this library:
$(grep ^function ${EAR_INSTALL_PATH}/ear_scripts/Bash_utils.sh | sed s:{::g)
EOF

}
myusage      ## print useful information about functions available in this Easy_Bashlib

## function to set processors clock frequency on all nodes in LSF job
## /home/lsfadmin/bin/set_freq_lsf.sh -p $myclock
## Input  : clock speed filled by user
## Output : runs the scripts on local node or allocated nodes by LSF.
function setmyfreq() {  ## function to set processors clock frequency on all nodes in LSF job

if [[ $# -ne 1 ]]
then
  echo -e "\n\tfunction $FUNCNAME to set processors clock frequency on all nodes in LSF job\n"
  echo -e "\t\tUsage: $FUNCNAME clockspeed NODES"
  echo -e "\t\t\tclockspeed   : clock speed filled by user "
  echo -e "\n\n"
return 0
fi

local myfreq=$1
local PROC=$(myproc_type)

case $PROC in
  "E5-2697v3")   nominalclock="2600000"; turboclock="2601000" ;;
  "E5-2650Lv4")  nominalclock="1700000"; turboclock="1701000" ;;
  "E5-2640v4")   nominalclock="2400000"; turboclock="2401000" ;;
  "E5-2680v4")   nominalclock="2400000"; turboclock="2401000" ;;
  "E5-2690v4")   nominalclock="2600000"; turboclock="2601000" ;;
  "E5-2697v4")   nominalclock="2300000"; turboclock="2301000" ;;
  "E5-2695v4")   nominalclock="2100000"; turboclock="2101000" ;;
  "E5-2697Av4")  nominalclock="2600000"; turboclock="2601000" ;;
  "E5-2667v4")  nominalclock="3200000"; turboclock="3201000" ;;
  "E5-2660v4")  nominalclock="2000000"; turboclock="2001000" ;;
  "E5-2643v4")  nominalclock="3400000"; turboclock="3401000" ;;
  *) echo "This $PROC processor is not yet supported by this script, sorry !" ; return ;;
esac
echo "Setting clock frequency for processor ${PROC}"
case ${myfreq} in
  "turbo"|"Turbo"|"TURBO") export myclock="${turboclock}" ;;
  "3400"|"3.4"|"3.4GHz")   export myclock="3400000" ;;
  "3300"|"3.3"|"3.3GHz")   export myclock="3300000" ;;
  "3200"|"3.2"|"3.2GHz")   export myclock="3200000" ;;
  "3100"|"3.1"|"3.1GHz")   export myclock="3100000" ;;
  "3000"|"3.0"|"3.0GHz")   export myclock="3000000" ;;
  "2900"|"2.9"|"2.9GHz")   export myclock="2900000" ;;
  "2800"|"2.8"|"2.8GHz")   export myclock="2800000" ;;
  "2700"|"2.7"|"2.7GHz")   export myclock="2700000" ;;
  "2600"|"2.6"|"2.6GHz")   export myclock="2600000" ;;
  "2500"|"2.5"|"2.5GHz")   export myclock="2500000" ;;
  "2400"|"2.4"|"2.4GHz")   export myclock="2400000" ;;
  "2300"|"2.3"|"2.3GHz")   export myclock="2300000" ;;
  "2200"|"2.2"|"2.2GHz")   export myclock="2200000" ;;
  "2100"|"2.1"|"2.1GHz")   export myclock="2100000" ;;
  "2000"|"2.0"|"2.0GHz")   export myclock="2000000" ;;
  "1900"|"1.9"|"1.9GHz")   export myclock="1900000" ;;
  "1800"|"1.8"|"1.8GHz")   export myclock="1800000" ;;
  "1700"|"1.7"|"1.7GHz")   export myclock="1700000" ;;
  "1600"|"1.6"|"1.6GHz")   export myclock="1600000" ;;
  "1500"|"1.5"|"1.5GHz")   export myclock="1500000" ;;
  "1400"|"1.4"|"1.4GHz")   export myclock="1400000" ;;
  "1300"|"1.3"|"1.3GHz")   export myclock="1300000" ;;
  "1200"|"1.2"|"1.2GHz")   export myclock="1200000" ;;
  *) echo "$1 not supported !" ;  return ;;
esac

if [[ $(( myclock > turboclock )) -eq 1 ]]; then
  echo "You entered higher clock frequency than the nominal frequency of ${PROC} processor..."
  echo "Using nominal clock frequency ${nominalclock} for this run"
  export myclock=${nominalclock}
fi

if [[ ! ${LSB_JOBID+x} ]]; then
  case ${NODES} in
  " ")
    echo "we are not inside LSF job and NODES variable is empty so setting clock frequency for host node to $myclock"
    sudo /home/lsfadmin/sic-scripts/set_node_freq.sh -p $myclock 
  ;;
  *)
    echo "we are not inside LSF job so setting clock frequency to $myclock for user defined nodes: ${NODES}"
    sudo /home/lsfadmin/sic-scripts/set_node_freq.sh -p $myclock 
    parallel -j 32  ssh {} "sudo /home/lsfadmin/sic-scripts/set_node_freq.sh -p $myclock" ::: $(echo ${NODES}|uniq)
  ;;
  esac

else
   echo "we are inside LSF job so setting clock frequency for all allocated nodes to $myclock ($(cat $LSB_DJOB_HOSTFILE|uniq))"
  #/home/lsfadmin/bin/set_freq_lsf.sh -p $myclock 
  parallel -j 32  ssh {} "sudo /home/lsfadmin/sic-scripts/set_node_freq.sh -p $myclock" ::: $(cat $LSB_DJOB_HOSTFILE|sort -u)
fi

}  #############################



## function to set default settings for OpenMP
function myomp_default_settings() {  ## function to set default settings for OpenMP

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to define default settings for OpenMP programs.\n"
  echo -e "\t\tUsage: $FUNCNAME OMP COMPILER"
  echo -e "\t\t\tOMP                    : number of OpenMP threads to be used"
  echo -e "\t\t\tCOMPILER               : name of compiler used : intel, gnu"
  echo -e "\n"
return 0
fi

OMP=$1
COMP=${2:-"intel"}

export OMP_NUM_THREADS=${OMP}
export OMP_DYNAMIC=FALSE
export SPINLOOPTIME=5000
export YIELDLOOPTIME=5000
export OMP_SCHEDULE=STATIC
export OMP_STACKSIZE=32M    #can be 300M for instance
#export OMP_WAIT_POLICY=ACTIVE  # similar to KMP_LIBRARY=turnaround with intel compiler

case "$COMP" in
Intel|intel|INTEL)
#  export KMP_LIBRARY=turnaround
  #export KMP_BLOCKTIME=5s  #to be tested, may depend on the application
  #export KMP_DETERMINISTIC_REDUCTION=true  # to ensure reductions will always give exact same results 
  export KMP_SETTINGS=true
  export KMP_WARNINGS=true
  export KMP_STACKSIZE=${OMP_STACKSIZE}
  export MKL_NUM_THREADS=${OMP_NUM_THREADS}
  export MYHPL_THREADS=${OMP_NUM_THREADS}

;;
Gnu|gnu|GNU)
;;
*) echo "${COMP} case is not yet implemented ! please try with intel, gnu." ;;
esac

}  #############################

## function to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers.
## input : #OMP , # of physical cores per socket, # of cores to use per socket, # of sockets to use (1 or 2)
function myimpi_bind_intel() {  ## function to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers.

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers.\n"
  echo -e "\t\tUsage: $FUNCNAME OMP PHYSICALCORESPERSOCKET USEDCORESPERSOCKET SOCKETSUSED "
  echo -e "\t\t\tOMP                    : number of OpenMP threads to be used"
  echo -e "\t\t\tPHYSICALCORESPERSOCKET : number of physical cores on each socket"
  echo -e "\t\t\tUSEDCORESPERSOCKET     : number of physical cores to be used on each socket"
  echo -e "\t\t\tSOCKETSUSED            : number of sockets (processors) to be used"
  echo -e "\n"
return 0
fi

OMP=$1
PHYSICALCORESPERSOCKET=$2
USEDCORESPERSOCKET=$3
SOCKETSUSED=2

export I_MPI_PIN=1

case $OMP in
0|1)
  export OMP_NUM_THREADS=1
  echo "We are not using OpenMP threads, binding for pure MPI mode."
    case $PHYSICALCORESPERSOCKET in
    14)
    ## assumming 4 numa nodes (CoD mode enabled = 4 NUMA nodes 0-6,7-13,14-20,21-27)
      #export I_MPI_PIN_PROCESSOR_LIST="0-$((USEDCORESPERSOCKET-1))";;
      case $USEDCORESPERSOCKET in
      5)  export I_MPI_PIN_PROCESSOR_LIST="0-2,7-8,14-16,21-22";;
      6)  export I_MPI_PIN_PROCESSOR_LIST="0-2,7-9,14-16,21-23";;
      #7)  export I_MPI_PIN_PROCESSOR_LIST="0-3,7-9,14-17,21-23";;
      7)  export I_MPI_PIN_PROCESSOR_LIST="0-13";;
      8)  export I_MPI_PIN_PROCESSOR_LIST="0-3,7-10,14-17,21-24";;
      9)  export I_MPI_PIN_PROCESSOR_LIST="0-4,7-10,14-18,21-24";;
      10) export I_MPI_PIN_PROCESSOR_LIST="0-4,7-11,14-18,21-25";;
      11) export I_MPI_PIN_PROCESSOR_LIST="0-5,7-11,14-19,21-25";;
      12) export I_MPI_PIN_PROCESSOR_LIST="0-5,7-12,14-19,21-26" ;;
      14) export I_MPI_PIN_PROCESSOR_LIST="0-6,7-13,14-20,21-27" ;;
      #14) export I_MPI_PIN_PROCESSOR_LIST="0,2,4,6,8,10,12,14,16,18,20,22,24,26" ;;
      28) export I_MPI_PIN_PROCESSOR_LIST="0-55" ;;
      *) echo "not yet implemented !" ;;
      esac
    ;;
    16)
    ## assumming 4 numa nodes (CoD mode enabled = 4 NUMA nodes 0-7,8-15,16-23,24-31)
      case $USEDCORESPERSOCKET in
      10) export I_MPI_PIN_PROCESSOR_LIST="0-4,8-12,16-20,24-28";;
      12) export I_MPI_PIN_PROCESSOR_LIST="0-5,8-13,16-21,24-29" ;;
      14) export I_MPI_PIN_PROCESSOR_LIST="0-6,8-14,16-22,24-30" ;;
      16) export I_MPI_PIN_PROCESSOR_LIST="0-7,8-15,16-23,24-31" ;;
      *) echo "not yet implemented !" ;;
      esac
    ;;
    *) echo "not yet implemented !" ;;
    esac
;;
*)
  echo "We are using OpenMP threads ! binding for mixed mode."
  ### OPENMP Variables
  export OMP_NUM_THREADS=${OMP}
  export OMP_DYNAMIC=FALSE
  export SPINLOOPTIME=5000
  export YIELDLOOPTIME=5000
  export OMP_SCHEDULE=STATIC
  export OMP_STACKSIZE=300M
  ## assumming ALL cores can be used , to be managed with OMP threads number
  #export I_MPI_PIN_DOMAIN=omp
  export KMP_AFFINITY=compact
  case $PHYSICALCORESPERSOCKET in
  14)
  ## assumming 4 numa nodes (CoD mode enabled = 4 NUMA nodes 0-6,7-13,14-20,21-27)
    case $TASKS_PER_NODE in
    1)  export I_MPI_PIN_DOMAIN="[FFFFFFF]";;                                                                         ## OMP = 28
    2)  export I_MPI_PIN_DOMAIN="[3FFF,FFFC000]";;                                                                    ## OMP = 14
    4)  export I_MPI_PIN_DOMAIN="[7F,3F80,1FC000,FE00000]";;                                                          ## OMP =  7
    7)  export I_MPI_PIN_DOMAIN="[F,F0,F00,F000,F0000,F00000,F000000]";;                                              ## OMP =  4
    14) export I_MPI_PIN_DOMAIN="[3,C,30,C0,300,C00,3000,C000,30000,C0000,300000,C00000,3000000,C000000]";;           ## OMP =  2
    28) export I_MPI_PIN_PROCESSOR_LIST="0-6,7-13,14-20,21-27" ;;                                                     ## OMP =  1
    *) echo "not yet implemented !" ;;
    esac
  ;;
  16)
  ## assumming 4 numa nodes (CoD mode enabled = 4 NUMA nodes 0-7,8-15,16-23,24-31)
    case $TASKS_PER_NODE in
    1)  export I_MPI_PIN_PROCESSOR_LIST="0";;                        ## OMP = 32
    2)  export I_MPI_PIN_PROCESSOR_LIST="0,16";;                     ## OMP = 16
    4)  export I_MPI_PIN_PROCESSOR_LIST="0,8,16,24";;                ## OMP =  8
    8)  export I_MPI_PIN_PROCESSOR_LIST="0,4,8,12,16,20,24,28";;     ## OMP =  4
    32) export I_MPI_PIN_PROCESSOR_LIST="0-7,8-15,16-23,24-31" ;;    ## OMP =  1
    *) echo "not yet implemented !" ;;
    esac
  ;;
  *) echo "not yet implemented !" ;;
  esac
;;
esac


}  #############################
## function to provide Hex mask for IntelMPI mixed mode pinning
function myimpi_bind_domain() { ## function to provide Hex mask for IntelMPI mixed mode pinning

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to provide Hex mask for IntelMPI mixed mode pinning.\n"
  echo -e "\t\tUsage: $FUNCNAME list of numbers of cores to bind on"
  echo -e "\t\tExample: $FUNCNAME 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 --> FFFFFFF "
  echo -e "\n"
return 0
fi
local res=0
local i_hex=0
local i
for i in $@; do
 if [ -z "$i" ]; then break; fi
 i_hex=`echo "obase=16;2^$i" | bc`;
 res=`echo "obase=ibase=16;$res + $i_hex" | bc`;
done

echo $res
}

## function to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers.
## input : #OMP , # of physical cores per socket, # of cores to use per socket, # of sockets to use (1 or 2)
function myimpi_bind_intel2() { ## function to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers (NEW version)

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers.\n"
  echo -e "\tUsage:\n $FUNCNAME NumberOfSocketsPerNode NumberOfNumaNodesPerSocket NumberOfCoresPerSocket HyperThreads NumberOfMpiPerNode NumberOfOpenmpPerMPI NumberOfSocketsUsed HyperThreadsUsed"
  echo -e "\n"
return 0
fi

## Hardware description:
local NumberOfSocketsPerNode=$1
local NumberOfNumaNodesPerSocket=$2
local NumberOfCoresPerSocket=$3
local HyperThreads=$4    ##  1 = HT OFF, 2 = HT ON
## Software description
local NumberOfMpiPerNode=$5
local NumberOfOpenmpPerMPI=$6
local NumberOfSocketsUsed=$7
local HyperThreadsUsed=$8

#Print some useful information
printf "System Information:\n"
printf "%20s %20s %20s %20s\n" "NumberOfSocketsPerNode" "NumberOfNumaNodesPerSocket" "NumberOfCoresPerSocket" "HyperThreads" 
printf "%20s %20s %20s %20s\n" "$NumberOfSocketsPerNode" "$NumberOfNumaNodesPerSocket" "$NumberOfCoresPerSocket" "$HyperThreads"
printf "Software Information:\n"
printf "%20s %20s %20s %20s \n" "NumberOfMpiPerNode" "NumberOfOpenmpPerMPI" "NumberOfSocketsUsed" "HyperThreadsUsed"
printf "%20s %20s %20s %20s \n" "$NumberOfMpiPerNode" "$NumberOfOpenmpPerMPI" "$NumberOfSocketsUsed" "$HyperThreadsUsed"
echo

local NumberOfCoresPerNumaNode NumberOfNumaNodePerNode NumberOfCoresPerNode
local NumberOfMpiPerNumaNode ListOfCoresFull ListOfCoresBind NumberOfNumaNodesUsed
local NumaNodes NumaNodeStart NumaNodeEnd LNumaNodeStart LNumaNodeEnd

(( NumberOfCoresPerNumaNode = NumberOfCoresPerSocket     / NumberOfNumaNodesPerSocket )) 
(( NumberOfNumaNodePerNode  = NumberOfNumaNodesPerSocket * NumberOfSocketsPerNode ))
(( NumberOfCoresPerNode     = NumberOfCoresPerSocket     * NumberOfSocketsPerNode ))
(( NumberOfNumaNodesUsed    = NumberOfSocketsUsed        * NumberOfNumaNodesPerSocket ))
(( NumberOfMpiPerNumaNode   = NumberOfMpiPerNode         / NumberOfNumaNodesUsed ))
(( rest                     = NumberOfMpiPerNode         % NumberOfNumaNodesUsed ))

echo -e "Hardware description of a node: (as understood by this script)"
echo -e "- ${NumberOfSocketsPerNode} sockets with ${NumberOfCoresPerSocket} cores on each for a total of ${NumberOfCoresPerNode} cores with ${HyperThreads} logical cores per physical core"
echo -e "- ${NumberOfNumaNodePerNode} numa nodes with ${NumberOfCoresPerNumaNode} cores per numa node, and $(( ${NumberOfCoresPerNumaNode} * ${HyperThreads} )) logical cores per numa node"
echo -e "- Using ${NumberOfSocketsUsed} socket(s) over ${NumberOfSocketsPerNode} in the node, hence numa node(s): $(echo $(seq 0 $(( ${NumberOfNumaNodesUsed} -1)) ))"
echo

## Verifications if it is possible to calculate everything correctly
if [ $(( $HyperThreadsUsed > $HyperThreads )) -eq 1 ]
then 
  echo "You want to use HyperThreads (HyperThreadsUsed=${HyperThreadsUsed}) but they are not active (HyperThreads=${HyperThreads})... Exiting..."; return 0; 
fi
if [ $(( $NumberOfMpiPerNode*$NumberOfOpenmpPerMPI > $NumberOfCoresPerNode*$HyperThreadsUsed )) -eq 1 ]
then 
  echo "You are requesting more MPI*OMP than total number of cores per node... Exiting..."; return 0; 
fi
if [ $(( $NumberOfMpiPerNode*$NumberOfOpenmpPerMPI > $NumberOfNumaNodesUsed*NumberOfCoresPerNumaNode*$HyperThreadsUsed )) -eq 1 ]
then 
  echo "You are requesting more MPI*OMP than available on the selected numa nodes"; return 0; 
fi

#######################################
NumaNodes=( $(seq 0 $((NumberOfNumaNodePerNode -1)) ) )
for i in ${NumaNodes[@]}
do
  (( NumaNodeStart[$i] = ${NumaNodes[$i]} * ${NumberOfCoresPerNumaNode} ))
  (( NumaNodeEnd[$i] = ${NumaNodeStart[$i]} + ${NumberOfCoresPerNumaNode} -1 ))
  if (( ${HyperThreadsUsed} > 1 )) ; then
    (( LNumaNodeStart[$i] = ${NumaNodeStart[$i]} + ${NumberOfCoresPerNode} ))
    (( LNumaNodeEnd[$i] = ${NumaNodeEnd[$i]} + ${NumberOfCoresPerNode} ))
  fi
done
  
case $NumberOfOpenmpPerMPI in
0|1)
  echo "No OpenMP threads here, switching to pure MPI binding"
  local a=0 b=0 c=0 d=0 e=0
  for i in $(seq 0 $(( ${NumberOfNumaNodesUsed} -1 )) )
  do
    if (( i == 0 ))
    then 
      if (( $rest > 0 )) ; then c=0 ; (( rest-- )) ; else c=1; fi
      a="${NumaNodeStart[${i}]}-${NumaNodeEnd[${i}]}"
      b="${NumaNodeStart[${i}]}-$(( ${NumaNodeStart[${i}]} + ${NumberOfMpiPerNumaNode} -${c} ))"
      d="${LNumaNodeStart[${i}]}-${LNumaNodeEnd[${i}]}"
      e="${LNumaNodeStart[${i}]}-$(( ${LNumaNodeStart[${i}]} + ${NumberOfMpiPerNumaNode} -${c} ))"
    else
      if (( $rest > 0 )) ; then c=0 ; (( rest-- )) ; else c=1; fi
      a="$a,${NumaNodeStart[${i}]}-${NumaNodeEnd[${i}]}"
      b="$b,${NumaNodeStart[${i}]}-$(( ${NumaNodeStart[${i}]} + ${NumberOfMpiPerNumaNode} -${c} ))"
      d="$d,${LNumaNodeStart[${i}]}-${LNumaNodeEnd[${i}]}"
      e="$e,${LNumaNodeStart[${i}]}-$(( ${LNumaNodeStart[${i}]} + ${NumberOfMpiPerNumaNode} -${c} ))"
    fi
  done

  #echo ListOfCoresFull=$a
  #echo ListOfCoresBind=$b
  #echo ListOfLCoresFull=$d
  #echo ListOfLCoresBind=$e
  export I_MPI_PIN=1
  if [ $(( ${HyperThreadsUsed} > 1 )) -eq 1 ] 
  then
    export I_MPI_PIN_PROCESSOR_LIST="$b,$e"
  else
    export I_MPI_PIN_PROCESSOR_LIST="$b"
  fi
  echo -e "We are using I_MPI_PIN_PROCESSOR_LIST=$I_MPI_PIN_PROCESSOR_LIST \n"

;;
*)
  echo "We are using OpenMP threads, switching to mixed mode MPI*OpenMP binding."
  if (( NumberOfMpiPerNumaNode * NumberOfOpenmpPerMPI > NumberOfCoresPerNumaNode * HyperThreadsUsed ))
  then 
    echo "Number of MPI and OpenMP do not match hardware: $NumberOfMpiPerNumaNode * $NumberOfOpenmpPerMPI > $NumberOfCoresPerNumaNode * $HyperThreadsUsed !!  Aborting... "
    return;
  fi

  local OmpNodeStart=() OmpNodeEnd=() OmpNodeMask=() FullMask="[" FullOmpBind="[" LOmpNodeStart=() LOmpNodeEnd=() OmpNodes=()
  local NumberOfCoresPerOmpNode=0 MaxOmp=0 RestOmp=0 a=0 b=0 
  OmpNodes=( $(seq 0 $((NumberOfMpiPerNode -1)) ) )
  (( NumberOfCoresPerOmpNode = NumberOfCoresPerNode / NumberOfMpiPerNode ))
  (( MaxOmp = NumberOfOpenmpPerMPI ))
  echo "OmpNodes=${OmpNodes[@]} MaxOmp=$MaxOmp NumberOfCoresPerOmpNode=$NumberOfCoresPerOmpNode"
  if (( ${HyperThreadsUsed} > 1 )) ; then
    MaxOmp=$(( NumberOfOpenmpPerMPI / HyperThreadsUsed ))
    RestOmp=$(( NumberOfOpenmpPerMPI % HyperThreadsUsed ))
     if (( $RestOmp > 0 )) ; then 
       echo "Number of OpenMP threads is not divisible by Hyperthreads number... Aborting." 
       return;
     fi
    echo "MaxOmp=$MaxOmp RestOmp=$RestOmp"
  fi

  a=0 b=${NumberOfCoresPerNode};
  for i in ${OmpNodes[@]}
  do
    (( OmpNodeStart[$i] = a ))
    (( OmpNodeEnd[$i] = OmpNodeStart[$i] + MaxOmp -1 ))
    (( a = a + NumberOfCoresPerOmpNode ))
    #echo "OmpNode[$i]=[ $(echo -n $(seq ${OmpNodeStart[$i]} ${OmpNodeEnd[$i]}))"
    OmpNodeCoresUsed[$i]="$(echo -n $(seq ${OmpNodeStart[$i]} ${OmpNodeEnd[$i]}))"
    OmpNodeMask[${i}]="$(myimpi_bind_domain $(seq ${OmpNodeStart[${i}]} ${OmpNodeEnd[${i}]}))"
    if (( ${HyperThreadsUsed} > 1 )) ; then
      (( LOmpNodeStart[$i] = b ))
      (( LOmpNodeEnd[$i] = LOmpNodeStart[$i] + MaxOmp -1 ))
      (( b = b + NumberOfCoresPerOmpNode ))
      #echo " $(echo $(seq ${LOmpNodeStart[$i]} ${LOmpNodeEnd[$i]}))"
      OmpNodeMask[${i}]="$(myimpi_bind_domain $(seq ${OmpNodeStart[${i}]} ${OmpNodeEnd[${i}]}) $(seq ${LOmpNodeStart[${i}]} ${LOmpNodeEnd[${i}]}) )"
      OmpNodeCoresUsed[$i]="${OmpNodeCoresUsed[$i]} $(echo -n $(seq ${LOmpNodeStart[$i]} ${LOmpNodeEnd[$i]}))"
    fi
    FullMask="${FullMask}${OmpNodeMask[${i}]},"
    FullOmpBind="${FullOmpBind}${OmpNodeCoresUsed[$i]},"
  done
  FullMask="$(echo \"${FullMask}]\"|sed s:,]:]:g)"
  FullOmpBind="$(echo \"${FullOmpBind}]\"|sed s:,]:]:g)"
  echo FullOmpBind=${FullOmpBind}
  echo -e "We are using I_MPI_PIN_DOMAIN=$FullMask \n"

  ### OPENMP Variables
  export OMP_NUM_THREADS=${NumberOfOpenmpPerMPI}
  export OMP_DYNAMIC=FALSE
  export SPINLOOPTIME=5000
  export YIELDLOOPTIME=5000
  export OMP_SCHEDULE=STATIC
  export OMP_STACKSIZE=300M
  ## assumming ALL cores can be used , to be managed with OMP threads number
  #export I_MPI_PIN_DOMAIN=omp
  export KMP_AFFINITY=compact
  export I_MPI_PIN_DOMAIN=$FullMask
;;
esac


}  #############################

## function to set Intel MPI settings depending on protocol used.
function myimpi_protocol() {  ## function to set Intel MPI settings depending on protocol used.

IMPIPROTOCOL=$1
echo -e "Using IntelMPI protocol ${IMPIPROTOCOL}"

case ${IMPIPROTOCOL} in
SHM|Shm|shm)
  export I_MPI_FABRICS=shm
;;
OFA|ofa|Ofa)
  export I_MPI_FABRICS=shm:ofa
#  export I_MPI_FABRICS=ofa:ofa
;;
DAPL|dapl|Dapl)
  export I_MPI_FABRICS=shm:dapl
  export I_MPI_DAPL_UD=off
  export DAPL_IB_MTU=4096
;;
DAPLNOSHM|daplnoshm|Daplnoshm)
  export I_MPI_FABRICS=dapl
  export I_MPI_DAPL_UD=off
  export DAPL_IB_MTU=4096
;;
DAPLUD|daplud|Daplud)
  export I_MPI_FABRICS=shm:dapl
  export DAPL_IB_MTU=4096
  export I_MPI_DAPL_UD=on
#  export I_MPI_DAPL_UD_RNDV_DYNAMIC_CONNECTION=off  #[default is on]
#  export I_MPI_DAPL_UD_EAGER_DYNAMIC_CONNECTION=off #[default is on for NPROCS>64]
  export I_MPI_ADJUST_ALLTOALL=3
;;
DAPLUDMIX|dapludmix|Dapludmix)
  export I_MPI_FABRICS=shm:dapl
  export DAPL_IB_MTU=4096
  export I_MPI_DAPL_UD=on
  export I_MPI_DAPL_UD_RDMA_MIXED=on
#  export I_MPI_DAPL_UD_RNDV_DYNAMIC_CONNECTION=off  #[default is on]
#  export I_MPI_DAPL_UD_EAGER_DYNAMIC_CONNECTION=off #[default is on for NPROCS>64]
  export I_MPI_ADJUST_ALLTOALL=3
;;
TMI|tmi|Tmi)
  export I_MPI_FABRICS=shm:tmi
;;
TCP|tcp|Tcp)
  export I_MPI_FABRICS=tcp:tcp
;;
esac

}  #############################

## function to set default settings for IntelMPI
function myimpi_default_settings() {  ## function to load and set default settings for IntelMPI

#module load compiler/intel/16.0.3 mpi/intel/5.1.3
#source /hpc/base/intel/parallel_studio_xe_2017.0.035/bin/psxevars.sh >/dev/null

export I_MPI_FALLBACK=0
export I_MPI_JOB_FAST_STARTUP=enable
export I_MPI_SCALABLE_OPTIMIZATION=enable
export I_MPI_TIMER_KIND=rdtsc
export I_MPI_DEBUG=1
export I_MPI_PLATFORM=bdw
export I_MPI_PLATFORM_CHECK=0
export I_MPI_HYDRA_PMI_CONNECT=alltoall
export I_MPI_THREAD_LEVEL_DEFAULT=FUNNELED  ##(only master thread can make MPI calls)

}  #############################

## function to create hostlist used by MPI libraries
## creates ./hostlist file as output
function mybuildhostfile() {  ## function to create hostlist used by MPI libraries

if [[ ! ${LSB_DJOB_HOSTFILE+x} ]]; then
  case ${NODES} in
  " ")
     echo "NODES variable is empty, so using local node"
     echo $(hostname) > ./hostlist
  ;;
  *)
     echo "Submitting on user specified node list: ${NODES}"
     rm -f ./hostlist
     for i in $(echo ${NODES}|uniq); do echo ${i} >> ./hostlist ; done
  ;;
  esac
else
   echo "we are inside LSF job so using LSB_DJOB_HOSTFILE variable"
   cat ${LSB_DJOB_HOSTFILE} | sort -u > ./hostlist
fi

}  #############################

## function to change the default order of nodes inside hostfiles
## uses ./hostlist file as input and output
## requires parallel command to be in the PATH
function myimpi_processhostfile() {  ## function to change the default order of nodes inside hostfiles

  nodelist=$(cat hostlist |uniq|sort)
  nmpi=$(cat hostlist|wc -l)
  nnodes=$(cat hostlist |uniq|wc -l)
  ppn=$(grep "$(head -n1 hostlist)" hostlist|wc -l)
  
  
  case $1 in
  "roundrobin"|"ROUNDROBIN"|"Roundrobin")
     echo "Creating round robin fashion of hostlist file: ${nmpi} mpi, ${nnodes} nodes, ppn=${ppn}"
     parallel -k -j1 echo {2} ::: $(seq 1 ${ppn}) :::  ${nodelist} |head -n ${nmpi} > hostlist
  ;;
  *) echo "Doing nothing to hostlist !"; exit;;
  esac
}  #############################

## function to set default settings for Lenovo MPItrace library
function mympitrace_settings() {  ## function to set default settings for Lenovo MPItrace library

case $1 in
"Static"|"STATIC"|"static")
  ## code is staticcally linked with mpitrace library , so do not need LD_PRELOAD 
  echo "Using statically linked version of mpitrace library"
;;
"Dynamic"|"DYNAMIC"|"dynamic")
  ## code is not staticcally linked with mpitrace library , so need LDPRELOAD:
  export LD_PRELOAD=/hpc/base/ctt/mpi_trace/lib/impi5/libmpitrace.so
  echo "Using dynamic version of mpitrace library thru LD_PRELOAD"
;;
"no"|"NO"|"No")
  echo "Not using mpitrace library"
;;
esac

## Environment variables to manage execution/profiling
export PROFILE_IMBALANCE=${PROFILE_IMBALANCE:-"no"}
export PROFILE_COMMUNICATORS=${PROFILE_COMMUNICATORS:-"no"}
export PROFILE_VPROF=${PROFILE_VPROF:-"yes"}
export PROFILE_JIO=${PROFILE_JIO:-"no"}
export PROFILE_HPM=${PROFILE_HPM:-"no"}
export PROFILE_ENERGY=${PROFILE_ENERGY:-"no"}
export PROFILE_XML=${PROFILE_XML:-"yes"}
export PROFILE_ALL_TASKS=${PROFILE_ALL_TASKS:-"no"}
export SYNC_COLLECTIVES=${SYNC_COLLECTIVES:-"no"}

}  #############################

## function to transform mpitrace CPU profiling data vmon.out* to bfdprof.out* test files
## requires parallel command to be in the PATH
function mympitrace_tobfdprof() {  ## function to transform mpitrace CPU profiling data vmon.out* to bfdprof.out* test files

EXE=$1
if [ -e "${EXE}" ]
then
  if [ -e "vmon.out.0" ]
  then
    export PATH=/hpc/base/ctt/mpi_trace/bin/:$PATH
    ls vmon* |sed s:vmon.out.::g | parallel "bfdprof ${EXE} vmon.out.{} > bfdprof.out.{}"
  else
    echo "No vmon.out.0 file available for post processing... Aborting..."
  fi
else
  echo "no ${EXE} binary found... Aborting..."
fi

}  #############################

## function to set default settings for ROMIO adn create hint text file.
function myimpi_io_settings() {  ## function to set default settings for ROMIO adn create hint text file.

export I_MPI_EXTRA_FILESYSTEM=on
export I_MPI_EXTRA_FILESYSTEM_LIST=gpfs

export ROMIO_PRINT_HINTS=yes
export ROMIO_HINTS=./romio_hints.txt
cat > $ROMIO_HINTS  << EOOOF
romio_cb_read disable
romio_cb_write enable
romio_ds_read disable
romio_ds_write disable
#striping_unit $(( 4 * 1024 * 1024 ))
#ind_wr_buffer_size $((16 * 1024 * 1024 ))
#ind_rd_buffer_size $(( 4 * 1024 * 1024 ))
#cb_buffer_size $(( 128 * 1024 * 1024 ))
#cb_nodes 24
EOOOF
}  #############################

## function to gather some numbers out of file :
## $1 = expression to be searched 
## $2 file to be used for seaching inside
## $3 = number of column corresponding to the needed value
## example: toto=$(mygrep "real" outfile 2)

function mygrep() {  ## function to gather some numbers out of file

  if [[ $# -ne 3 ]]
  then
    echo -e "\n\tfunction $FUNCNAME to help grepping files \n"
    echo -e "\t\tUsage: $FUNCNAME \$1 \$2 \$3 "
    echo -e "\t\t \$1 = expression to be searched"
    echo -e "\t\t \$2 = file to be used for seaching inside"
    echo -e "\t\t \$3 = number of column corresponding to the needed value"
    echo -e "\n\n"
  return 0
  fi
  a=( $(grep "$1" $2|tail -n 1) )
  val=${a[$(($3-1))]}
  if [ "${val}x" == "x" ] ;then val=0; fi
  echo "$val"
}  #############################

function mympitrace_extract_bfdprof_data() { ## function to extract data from bfdprof profiling files from mpitrace library

if [[ $# -eq 0 ]] 
then 
  echo -e "\n\tfunction $FUNCNAME to extract data from bfdprof profiling files from mpitrace library \n"
  echo -e "\t\tUsage: $FUNCNAME FILES "
  echo -e "\t\t\tFILES : list of files to search bfdprof symbols in it \n"
  echo -e "\t\texport NUM=40 : number of lines to be processed (= number of symbol from flat profile to use) default = 30"
  echo -e "\t\tRemark: the symbols are taken from first bfdprof.out file in \$FILES"
  echo -e "\n\n"
return 0
fi

NUM=${NUM:-"30"}
(( END = NUM ))
(( START = NUM - 9 ))

FILES=( "$@" )

LIST=$(echo $(cat ${FILES[1]} |head -n${END} |tail -n${START}|awk '{print $2}'))

echo "${LIST} "
for i in ${LIST}
  do
  a=""
  for j in ${FILES[*]}
    do
      a="$a;$(grep " ${i}" ${j}|head -n1| awk '{print $1}')"
    done
  echo $i$a
done
}  #############################



function mympitrace_extract_loadimbalance() { ##function to extract load imbalance data from mpitrace profiling files 

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to extract load imbalance data from profiling files from mpitrace library \n"
  echo -e "\t\tUsage: $FUNCNAME FILES "
  echo -e "\t\t\tDIRS : list of directories to search in it \n"
  echo -e "\n\n"
return 0
fi

local DIRS=( "$@" )
local mintask

for i in ${DIRS[@]}
do
  mintask=$(grep "minimum communication time =" ${i}/mpi_profile*.0|awk '{print $9}')
  totalload=$(grep _barrier ${i}/mpi_profile*.${mintask}|grep -v "MPI_Barrier " | awk 'BEGIN{ sum=0.0}{ sum +=$4}END{ print  sum }')
  echo "total load imbalance = ${totalload} sec for ${i} folder"
done


}  #############################


function myimpi_execute() { ### function to execute Intel_MPI code with specified profiling tool

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to execute Intel_MPI code with specified profiling tool \n"
  echo -e "\t\tUsage: $FUNCNAME binary nmpi ppn profiler "
  echo -e "\t\t\tbinary : name of local binary to run "
  echo -e "\t\t\tmpi : number of total MPI tasks to use "
  echo -e "\t\t\tppn : number of MPI tasks per node "
  echo -e "\t\t\tprofiler : profiling tools to be used: "
  echo -e "\t\t\t\t mpitrace : Lenovo mpitrace tool "
  echo -e "\t\t\t\t perf : Linux perf tool "
  echo -e "\t\t\t\t report : Allinea Report tool "
  echo -e "\t\t\t\t map : Allinea Map tool "
  echo -e "\t\t\t\t snapshots : Intel Snapshots tool "
  echo -e "\t\t\t\t vtune : Intel Vtune tool "
  echo -e "\n\t Please note:"
  echo -e "\t\t - IntelMPI environment, binding... MUST be set before calling this function."
  echo -e "\t\t - standard and error outputs will not be redirected "
  echo -e "\n\n"
return 0
fi

local binary=${1:-"1"}
local mpi=${2:-"1"}
local ppn=${3:-"1"}
local PROF=${4:-"none"}

which mpiexec.hydra >/dev/null
if [[ $? -eq 1 ]]
then
  echo "Please load IntelMPI environment, mpiexec.hydra not found !"  
  return
fi

source ~/.bashrc 


case ${PROF} in
mpitrace|MPITRACE|Mpitrace)
  #### MPITRACE Environment variables :
  mympitrace_settings dynamic
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  time mpiexec.hydra -genvall -machinefile ./hostlist -n ${mpi} -ppn=${ppn}   ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
  ## CPU PROFILING stuff:
  mympitrace_tobfdprof ${binary}
;;

mpitraces|MPITRACES|Mpitraces)
  #### MPITRACE Environment variables :
  mympitrace_settings static
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date
  time mpiexec.hydra -genvall -machinefile ./hostlist -n ${mpi} -ppn=${ppn}   ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
  ## CPU PROFILING stuff:
  mympitrace_tobfdprof ${binary}
;;

perf|PERF|Perf)
  ##### Profiling using Linux Perf  SINGLE NODE #####
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  export TMPDIR=$(pwd)
  time perf record mpiexec.hydra -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
;;

report|REPORT|Report)
  ##### Profiling using Allinea Report #####
  #module load allinea/6.0.4/rh6/reports
  module load allinea/6.0.4/rh7/reports
  export ALLINEA_MPI_WRAPPER=/home/emichel/.allinea/wrapper/libmap-sampler-pmpi-login0401.sic-hpc.lenovo.com.so
  ## start the IPMI agent for Power measurements
  myallinea_ipmi_driver load
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  export TMPDIR=$(pwd)
  time perf-report mpiexec.hydra -genvall -f ./hostlist -n ${mpi} -ppn=${ppn}   ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
  ## stop the IPMI agent for Power measurements
  myallinea_ipmi_driver unload
;;

map|MAP|Map)
  ##### Profiling using Allinea Map #####
  #module load allinea/6.0.4/rh6/forge
  module load allinea/6.0.4/rh7/forge
  export ALLINEA_MPI_WRAPPER=/home/emichel/.allinea/wrapper/libmap-sampler-pmpi-login0401.sic-hpc.lenovo.com.so
  ## start the IPMI agent for Power measurements
  myallinea_ipmi_driver load
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  export TMPDIR=$(pwd)
  time map --profile mpiexec.hydra -genvall -f ./hostlist -n ${mpi} -ppn=${ppn}  ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
  ## stop the IPMI agent for Power measurements
  myallinea_ipmi_driver unload
;;

snapshots|SNAPSHOTS|Snapshots)
  ##### Profiling using IMPI snapshot  ##### Fails...
  source /hpc/base/intel/vtune_amplifier_xe_2016.2.0.444464/amplxe-vars.sh
  source /hpc/base/intel//itac/9.1.2.024/bin/mpsvars.sh
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  time mpiexec.hydra -mps -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
;;

vtune|VTUNE|Vtune)
  ##### Profiling using Intel Vtune  #####
  #source /hpc/base/intel/vtune_amplifier_xe_2016.2.0.444464/amplxe-vars.sh
  source /hpc/base/intel/vtune_amplifier_xe_2017/amplxe-vars.sh
  export INTEL_LICENSE_FILE=/hpc/base/intel/compilers_and_libraries_2017.0.042/linux/Licenses
  export analysis=hotspots
  #export analysis=advanced-hotspots
  #export analysis=hpc-performance
  #export analysis=memory-access
  
  myvtune_sep_driver load 
  export TMPDIR=$(pwd)  ## small trick to prevent vtune to fail due to /tmp full dunring run

  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  time mpirun -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} amplxe-cl -data-limit=0 -result-dir myvtune -collect ${analysis} -- ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "

  myvtune_sep_driver unload
;;

*)
  ##### Execution without profiling  #####
  ## save environment used during the run
  env > log.env 2>&1
  local a=$(date +%s) ; date 
  time mpiexec.hydra -genvall -f ./hostlist -n ${mpi} -ppn=${ppn} ${binary}
  local b=$(date +%s) ; date
  echo "Total elapsed time = $(( b - a )) sec "
;;
esac  # case PROF

}  #############################

function myvtune_sep_driver () {  ## function to load / unload SEP driver for Vtune tool

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to load / unload SEP driver for Vtune tool"
  echo -e "\t\tUsage: $FUNCNAME  action"
  echo -e "\t\t\taction : load / unload "
  echo -e "\n\n"
return 0
fi

local action=${1:-"none"}

case ${action} in
load|Load|LOAD)
  echo "Loading Vtune SEP driver:"
  if [[ ! ${LSB_JOBID+x} ]]; then
    echo -e "\twe are not inside LSF job so starting SEP driver on host node $(hostname)"
    sudo /home/lsfadmin/sic-scripts/ctrl_vtune_sep_driver.sh load
  else
     echo -e "\twe are inside LSF job so starting SEP driver on all allocated nodes ($(cat $LSB_DJOB_HOSTFILE|uniq))"
    /home/lsfadmin/bin/vtune_lsf.sh load
  fi
  echo "Done"
;;
unload|Unload|UNLOAD)
  echo "Unloading Vtune SEP driver:"
  if [[ ! ${LSB_JOBID+x} ]]; then
    echo -e "\twe are not inside LSF job so unloading SEP driver on host node $(hostname)"
    sudo /home/lsfadmin/sic-scripts/ctrl_vtune_sep_driver.sh unload
  else
     echo -e "\twe are inside LSF job so starting SEP driver on all allocated nodes ($(cat $LSB_DJOB_HOSTFILE|uniq))"
    /home/lsfadmin/bin/vtune_lsf.sh unload
  fi
  echo "Done"

;;
*) echo "no action required" ;;
esac

}

function myallinea_ipmi_driver () {  ## function to load / unload ipmi agent for Allinea tool for Power measurements

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to load / unload ipmi agent for Allinea tool"
  echo -e "\t\tUsage: $FUNCNAME  action"
  echo -e "\t\t\taction : load / unload "
  echo -e "\n\n"
return 0
fi

local action=${1:-"none"}

case ${action} in
load|Load|LOAD)
  echo "Loading IPMI agent:"
  if [[ ! ${LSB_JOBID+x} ]]; then
    echo -e "\twe are not inside LSF job so starting IPMI agent on host node $(hostname)"
    sudo /home/lsfadmin/sic-scripts/ctrl_allinea_agent.sh start
  else
     echo -e "\twe are inside LSF job so starting IPMI agent on all allocated nodes ($(cat $LSB_DJOB_HOSTFILE|uniq))"
    /home/lsfadmin/bin/allinea_lsf.sh start
  fi
  echo "Done"
;;
unload|Unload|UNLOAD)
  echo "Unloading IPMI Agent:"
  if [[ ! ${LSB_JOBID+x} ]]; then
    echo -e "\twe are not inside LSF job so unloading IPMI agent on host node $(hostname)"
    sudo /home/lsfadmin/sic-scripts/ctrl_allinea_agent.sh stop

  else
     echo -e "\twe are inside LSF job so starting IPMI agent on all allocated nodes ($(cat $LSB_DJOB_HOSTFILE|uniq))"
    /home/lsfadmin/bin/allinea_lsf.sh stop
  fi
  echo "Done"

;;
*) echo "no action required" ;;
esac

}


function mylsf_submit () {  ## function to submit jobs to LSF

if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to submit jobs to LSF"
  echo -e "\t\tUsage: $FUNCNAME jobfile"
  echo -e "\t\t\tjobfile : LSF job file to be submitted "
  echo -e "\t\tNODES: environment variable used to select specific nodes "
  echo -e "\n\n"
return 0
fi

local job=${1:-"nulljob"}
local nodes=${NODES:-" "}

case $nodes in
" ")
    bsub  < ${job}
;;
*)
   echo "Submitting on user specified node list: ${nodes}"
   bsub -m "${nodes}" < ${job}
;;
esac

}

function myproc_type () {  ## function to return processor type

if [[ $# -ne 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to check processor type (to be called from compute node"
  echo -e "\t\tUsage: $FUNCNAME "
  echo -e "\n\n"
return 0
fi

cpu=$(grep "model name" /proc/cpuinfo |tail -n1|awk '{print $7$8}'|sed s:@::g)
echo $cpu

}

function myproc_details () {  ## function to print performance data for specific SKU

if [[ $# -ne 2 ]]
then
  echo -e "\n\tfunction $FUNCNAME to print performance data for specific SKU"
  echo -e "\t\tUsage: $FUNCNAME Processor_type Perfdata"
  echo -e "\t\t\tProcessor_type : SKU to get data for: E5-2697v3, E5-2650Lv4, ... "
  echo -e "\t\t\tPerfdata : Performance data to get printed for the selected SKU. Can be:"
  echo -e "\t\t\t      1 TDP_W"
  echo -e "\t\t\t      2 Number_of_Cores"
  echo -e "\t\t\t      3 nominal_frequency_GHz"
  echo -e "\t\t\t      4 AVX_frequency_GHz"
  echo -e "\t\t\t      5 DP_FLOPs_per_cycle"
  echo -e "\t\t\t      6 Peak_Perf_GFs"
  echo -e "\t\t\t      7 HPL_2Sockets_from_Intel_GFs"
  echo -e "\t\t\t      8 SPECINT_rate_base2006"
  echo -e "\t\t\t      9 SPECFP_rate_base2006"
  echo -e "\t\t\t     10 STREAM_Triad_GBs"
  echo -e "\t\t\t     11 Price_in_dollars_from_Ark"
  echo -e "\n\n"
return 0
fi

local sku=$1
local data=$2
#local myfile=./Intel_Proc_table.txt
local myfile=/dev/shm/Intel_Proc_table.${USER}.txt
#local myfile=/tmp/Intel_Proc_table.txt
local col

rm -f ${myfile}
cat > ${myfile} <<'_EOF'
## This data is available in Excel format in HPC Community (link to be provided here)
Proc_Name	Sku_Type	TDP_W	Number_of_Cores	nominal_frequency_GHz	AVX_frequency_GHz	DP_FLOPs_per_cycle	Peak_Perf_GFs	HPL_2Sockets_from_Intel_GFs	SPECINT_rate_base2006	SPECFP_rate_base2006	STREAM_Triad_GBs	Price_in_dollars_from_Ark
Broadwell	E5-2637v4	135	4	3.5	3.2	16	204.8	0	463	428	0	996
Broadwell	E5-2623v4	85	4	2.6	2.2	16	140.8	0	n/a	n/a	0	444
Broadwell	E5-2603v4	85	6	1.7	1.7	16	163.2	0	284	304	0	213
Broadwell	E5-2643v4	135	6	3.4	2.8	16	268.8	0	674	584	0	1552
Broadwell	E5-2620v4	85	8	2.1	1.8	16	230.4	0	607	502	0	417
Broadwell	E5-2609v4	85	8	1.7	1.7	16	217.6	0	369	367	0	306
Broadwell	E5-2667v4	135	8	3.2	2.6	16	332.8	0	844	669	0	2057
Broadwell	E5-2640v4	90	10	2.4	2	16	320	0	827	680	0	939
Broadwell	E5-2630v4	85	10	2.2	1.8	16	288	0	780	640	0	667
Broadwell	E5-2630Lv4	55	10	1.8	1.3	16	208	0	616	534	0	612
Broadwell	E5-2650v4	105	12	2.2	1.8	16	345.6	0	955	788	0	1166
Broadwell	E5-2687Wv4	160	12	3	2.6	16	499.2	0	1130	857	0	2141
Broadwell	E5-2690v4	135	14	2.6	2.1	16	470.4	1050	1290	913	125.3	2090
Broadwell	E5-2680v4	120	14	2.4	1.9	16	425.6	0	1170	866	0	1745
Broadwell	E5-2660v4	105	14	2	1.7	16	380.8	0	1050	823	0	1445
Broadwell	E5-2650Lv4	65	14	1.7	1.2	16	268.8	0	805	623	0	1329
Broadwell	E5-2697Av4	145	16	2.6	2.2	16	563.2	0	1430	976	126.4	2891
Broadwell	E5-2683v4	120	16	2.1	1.7	16	435.2	0	1210	888	0	1846
Broadwell	E5-2697v4	145	18	2.3	2	16	576	1278	1480	1000	126.6	2702
Broadwell	E5-2695v4	120	18	2.1	1.7	16	489.6	0	1340	940	0	2424
Broadwell	E5-2698v4	135	20	2.2	1.8	16	576	0	1460	1010	0	3226
Broadwell	E5-2699v4	145	22	2.2	1.8	16	633.6	1446	1580	1030	127.7	4115
Haswell	E5-2637v3	135	4	3.5	3.2	16	204.8	0	457	422	0	996
Haswell	E5-2623v3	105	4	3	2.7	16	172.8	0	412	378	0	444
Haswell	E5-2620v3	85	6	2.4	2.1	16	201.6	0	509	466	0	417
Haswell	E5-2609v3	85	6	1.9	1.9	16	182.4	0	306	321	0	306
Haswell	E5-2603v3	85	6	1.6	1.3	16	124.8	0	266	280	0	213
Haswell	E5-2643v3	135	6	3.4	2.8	16	268.8	0	669	571	0	1552
Haswell	E5-2640v3	90	8	2.6	2.2	16	281.6	536	703	578	0	939
Haswell	E5-2630v3	85	8	2.4	2.1	16	268.8	0	666	561	0	667
Haswell	E5-2667v3	135	8	3.2	2.7	16	345.6	0	815	653	0	2057
Haswell	E5-2630Lv3	55	8	1.8	1.5	16	192	0	556	471	0	612
Haswell	E5-2660v3	105	10	2.6	2.2	16	352	673	884	689	0	1445
Haswell	E5-2650v3	105	10	2.3	2	16	320	663	824	675	0	1166
Haswell	E5-2687Wv3	160	10	3.1	2.7	16	432	0	954	741	0	2141
Haswell	E5-2690v3	135	12	2.6	2.3	16	441.6	824	1080	792	114.8	2090
Haswell	E5-2680v3	120	12	2.5	2.1	16	403.2	758	1030	763	0	1745
Haswell	E5-2670v3	120	12	2.3	2	16	384	751	970	754	0	1589
Haswell	E5-2685v3	120	12	2.6	2.2	16	422.4	0	880	747	0	2090
Haswell	E5-2650Lv3	65	12	1.8	1.5	16	288	0	775	619	0	1329
Haswell	E5-2697v3	145	14	2.6	2.2	16	492.8	950	1190	838	119.047	2702
Haswell	E5-2695v3	120	14	2.3	1.9	16	425.6	806	1080	791	0	2424
Haswell	E5-2683v3	120	14	2	1.7	16	380.8	821	1070	782	0	1846
Haswell	E5-2698v3	135	16	2.3	1.9	16	486.4	901	1230	860	0	0
Haswell	E5-2699v3	145	18	2.3	1.9	16	547.2	1096	1360	911	118.7	0
Skylake	??							0			0	0

_EOF

case ${data} in
  1|TDP_W)                           col=3 ;;
  2|Number_of_Cores)                 col=4 ;;
  3|nominal_frequency_GHz)           col=5 ;;
  4|AVX_frequency_GHz)               col=6 ;;
  5|DP_FLOPs_per_cycle)              col=7 ;;
  6|Peak_Perf_GFs)                   col=8 ;;
  7|HPL_2Sockets_from_Intel_GFs)     col=9 ;;
  8|SPECINT_rate_base2006)           col=10 ;;
  9|SPECFP_rate_base2006)            col=11 ;;
 10|STREAM_Triad_GBs)                col=12 ;;
 11|Price_in_dollars_from_Ark)       col=13 ;;
esac

echo $(grep $sku ${myfile} | awk -v a=$col '{print $a}')
rm -f ${myfile}

}

function myhpl_eff () {  ## function to return HPL efficiency (in %) against peak AVX clock frequency

if [[ $# -ne 3 ]]
then
  echo -e "\n\tfunction $FUNCNAME to return HPL efficiency (in %) against peak AVX clock frequency"
  echo -e "\t\tUsage: $FUNCNAME SKU_type Number_of_SKU HPL_measured_GFs"
  echo -e "\n\n"
return 0
fi
local sku=$1 ; local nsku=$2 ; local GFs=$3

local skupeak=$(myproc_details $sku Peak_Perf_GFs)
local GFspeak=$(echo "${skupeak}*${nsku}"|bc -l)
local eff=$(echo "${GFs}/${GFspeak}*100"|bc -l)
printf  "%6.2f\n" ${eff}

}

function myrun_linpack_on_core () {  ## function to run serial linpack on single core

[[ $# -ne 1 ]] && echo "please add core number to run linpack on." && return
if [[ $# -ne 1 ]]
then
  echo -e "\n\tfunction $FUNCNAME to run serial linpack on single core"
  echo -e "\t\tUsage: $FUNCNAME core# "
  echo -e "\n\n"
return 0
fi

local CORE=$1

## load Intel environment for compiler, binaries, tools...
source /hpc/base/intel/parallel_studio_xe_2017.0.035/bin/psxevars.sh >/dev/null

local BIN="${MKLROOT}/benchmarks/linpack/xlinpack_xeon64"
local INPUT="./lininput.$RANDOM"

cat << EOF > ${INPUT}
Sample Intel(R) Optimized LINPACK Benchmark data file (lininput_xeon64)
Intel(R) Optimized LINPACK Benchmark data
1                     # number of tests
10000       # problem sizes
10000       # leading dimensions
1000000000  # times to run a test
4           # alignment values (in KBytes)
EOF

## execution
export OMP_NUM_THREADS=1
taskset -c ${CORE} ${BIN} ${INPUT}
}

function myrun_linpack_on_nodes () {  ## function to run serial linpack on single cores on a remote node

[[ $# -eq 0 ]] && echo "please add core numbers to run linpack on." && return
if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to run serial linpack on single cores on remote nodes (from hostlist file in current directory)"
  echo -e "\t\tUsage: $FUNCNAME cores# "
  echo -e "\n\n"
  return 0
fi
local CORES=$@
local outfile=hostlist
if [[ ! -e ${outfile} ]]
then
  echo -e "Please put node names into ${outfile} !"
  return 0
fi
local NODES=( $(cat ${outfile}|sort|uniq) )
local HERE=$(pwd)

echo CORES=$CORES
echo NODES=${NODES[@]}
echo HERE=$HERE

for i in ${NODES[@]}
do
## creation of script
cat << EOF > sclinpack_${i}.sh
cd $HERE
source /home/emichel/ASSETS/Easy_Bashlib/Easy_Bashlib.sh >/dev/null 2>&1
for k in ${CORES}
do
  myrun_linpack_on_core \${k} > ./log.singlecorelinpack.${i}.\${k} 2>&1 &
  sleep 1
done
wait
EOF
chmod +x ./sclinpack_${i}.sh
## execution
ssh $i "cd $HERE;./sclinpack_${i}.sh &" &
done

}

function myclean_linpack_on_nodes () {  ## function to clean serial linpack on single cores on remote nodes

if [[ $# -ne 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to clean serial linpack on single cores on remote nodes (from hostlist file in current directory)"
  echo -e "\t\tUsage: $FUNCNAME "
  echo -e "\n\n"
  return 0
fi
local outfile=hostlist
if [[ ! -e ${outfile} ]]
then
  echo -e "Please put node names into ${outfile} !"
  return 0
fi
local NODES=( $(cat ${outfile}|sort|uniq) )
echo NODES=${NODES[@]}

for i in ${NODES[@]}
do
  ## execution  ## 4 times just to be sure ..
  ssh $i "killall xlinpack_xeon64"
  ssh $i "killall xlinpack_xeon64"
  ssh $i "killall xlinpack_xeon64"
  ssh $i "killall xlinpack_xeon64"
done

}


function mystats_from_column () {  ## function to calculate a few stats from single number column 
if [[ $# -eq 0 ]]
then
  echo -e "\n\tfunction $FUNCNAME to calculate a few stats from single number column"
  echo -e "\t\tUsage: $FUNCNAME "
  echo -e "\n\n"
  return 0
fi
local NUMBERS="$*"

echo ${NUMBERS[@]}

}

function mysetup_params () {  ## function to calculate some useful parameters based on some values

if [[ $# -ne 5 ]]
then
  echo -e "\n\tfunction $FUNCNAME to calculate some useful parameters based on some values"
  echo -e "\t\tUsage: $FUNCNAME USEDCORESPERSOCKET PHYSICALCORESPERSOCKET MPI OMP HT "
  echo -e "\t\tOUT: NBNODES TASKSPERNODE "
  echo -e "\n\n"
  return 0
fi
local USEDCORESPERSOCKET=$1
local PHYSICALCORESPERSOCKET=$2
local MPI=$3
local OMP=$4
local HT=$5

local NBSOCKETSPERNODE=2
local USEDCORESPERNODE=$((USEDCORESPERSOCKET * NBSOCKETSPERNODE))

if [ $OMP -eq 0 ]; then OMP=1;fi
(( NBNODES = MPI*OMP/HT / USEDCORESPERNODE + ( MPI*OMP/HT % USEDCORESPERNODE > 0) )) 
(( TASKSPERNODE = USEDCORESPERNODE*HT / OMP  ))

echo "Using $NBNODES nodes for $MPI mpi and $OMP omp with tpn*omp = $TASKSPERNODE*$OMP = $((TASKSPERNODE*OMP)) cores used per node "
export NBNODES TASKSPERNODE

}

## function to set default settings for IntelMPI
function myompi_default_settings() {  ## function to load and set default settings for OpenMPI

echo "Under construction..."

}  #############################

function mysubmitjob () {  ## function to submit jobs on LSF scheduler or interactively

if [[ $# -ne 4 ]]
then
  echo -e "\n\tfunction $FUNCNAME to submit jobs on LSF scheduler or interactively"
  echo -e "\t\tUsage: $FUNCNAME EXE MYJOBFILE LSFQUEUE NODES "
  echo -e "\t\tOUT: nothing : launch jobs "
  echo -e "\n\n"
  return 0
fi
local EXE=$1
local MYJOBFILE=$2
local LSFQUEUE=$3
local NODES=$4
local i

## Submission of job script ## to be uncommented for effective submission
case ${LSFQUEUE} in
"inter")
  echo "Launching job interactively:"
  case $NODES in
  " ")
     echo "NODES variable is empty, so executing on local node"
     echo $(hostname) > ./hostlist
  ;;
  *)
     echo "Submitting on user specified node list: ${NODES}"
     rm -f ./hostlist
     for i in $(echo ${NODES}|uniq); do echo ${i} >> ./hostlist ; done
  ;;
  esac
  chmod +x ${MYJOBFILE}
  nohup ./${MYJOBFILE} 2> ${EXE}.err 1> ${EXE}.out &
;;
*)
  echo "Launching job using LSF job sheduler:"
  case $NODES in
  " ")
      bsub  < ${MYJOBFILE}
  ;;
  *)
     echo "Submitting on user specified node list: ${NODES}"
     bsub -m "${NODES}" < ${MYJOBFILE}
  ;;
  esac
;;
esac

}
