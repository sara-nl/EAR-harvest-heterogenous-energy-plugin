#!/bin/bash

echo nxt0347 > hostlist

./ear_daemon_start.sh 1
sleep 2
./ear_run.sh /home/xjaneas/_ear/bin/kernels/bt-mz.C.28 "28" "28" "MONITORING_ONLY"
sleep 2
./ear_daemon_exit.sh
