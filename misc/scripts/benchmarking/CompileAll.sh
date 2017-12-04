#!/bin/bash

#Update these variables with the required values

export SOCKETS=2
export CORES_SOCKET=14
export TOTAL_CPUS=28

#Compiling the different kernels
mkdir $EAR_INSTALL_PATH/ear_learning_phase/kernels/NPB3.3.1/NPB3.3-MPI/bin
mkdir $EAR_INSTALL_PATH/ear_learning_phase/kernels/NPB3.3.1-MZ/NPB3.3-MZ-MPI/bin
mkdir $EAR_INSTALL_PATH/ear_learning_phase/kernels/NPB3.3.1/NPB3.3-OMP/bin
./compile.sh lu-mpi $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh ep $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh bt-mz $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh sp-mz $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh lu-mz $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh ua $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh dgemm $SOCKETS $CORES_SOCKET $TOTAL_CPUS
./compile.sh stream $SOCKETS $CORES_SOCKET $TOTAL_CPUS
