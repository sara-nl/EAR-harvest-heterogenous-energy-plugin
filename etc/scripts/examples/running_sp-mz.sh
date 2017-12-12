#!/bin/bash

$EAR_INSTALL_PATH/etc/scripts/running/ssh_daemon_start.sh nxt0348 1
sleep 2
$EAR_INSTALL_PATH/etc/scripts/running/mpi_exec.sh nxt0348 /home/xjaneas/_ear/bin/kernels/sp-mz.C.28 "28" "28" "MONITORING_ONLY"
sleep 2
$EAR_INSTALL_PATH/etc/scripts/running/ssh_daemon_exit.sh nxt0348
