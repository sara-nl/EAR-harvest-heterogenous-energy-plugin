#!/bin/bash

echo nxt0347 > hostlist

$EAR_INSTALL_PATH/etc/scripts/running/ssh_daemon_start.sh $PWD/hostlist 1
sleep 2
$EAR_INSTALL_PATH/etc/scripts/running/mpi_exec.sh $PWD/hostlist /home/xjaneas/_ear/bin/kernels/bt-mz.C.28 "28" "28" "MONITORING_ONLY"
sleep 2
$EAR_INSTALL_PATH/etc/scripts/running/ssh_daemon_exit.sh $PWD/hostlist
