#!/bin/bash

echo nxt0347 > hostlist
source ear_run.sh
export MKL_NUM_THREADS=28
mpirun_with_ear /home/xjaneas/_ear/bin/kernels/dgemm_example 1 1 MONITORING_ONLY
