#!/bin/bash


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
function mpi_openmp_bind() { ## function to bind MPI processes and OpenMP threads with IntelMPI and Intel compilers (NEW version)

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


