#!/bin/bash

echo nxt0347 > hostlist
source ear_run.sh
mpirun_with_ear /home/xjaneas/_ear/bin/kernels/bt-mz.C.28 28 28 MONITORING_ONLY
