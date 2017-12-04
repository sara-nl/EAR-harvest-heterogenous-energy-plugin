#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/xjcorbalan/hwrusage-1.0.11/lib:/home/xjcorbalan/hwrusage-1.0.11/energy-1.0.2/lib
export MANPATH=/hpc/base/ctt/packages/extrae/3.3.0/impi/share/man:/hpc/base/intel/documentation_2016/en/debugger/gdb-ia/man/:/hpc/base/intel/man/common:/hpc/base/intel/documentation_2016/en/debugger/gdb-igfx/man/:/hpc/base/intel/compilers_and_libraries_2016.3.210/linux/mpi/man:/usr/mpi/gcc/openmpi-1.10.0/share/man:
source /hpc/base/intel/compilers_and_libraries/linux/mpi/bin64/mpivars.sh
source /hpc/base/ctt/bin/benchmark_setup.sh
source /hpc/base/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64
$*

