#!/bin/bash

echo nxt0347 > hostlist
./ear_daemon_start.sh 1
sleep 2
export MKL_NUM_THREADS=28
./ear_run.sh /home/xjaneas/_ear/bin/kernels/dgemm_example 1 1 MONITORING_ONLY
sleep 2
./ear_daemon_exit.sh
