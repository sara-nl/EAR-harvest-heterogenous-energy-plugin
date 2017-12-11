#!/bin/bash

export HOSTLIST="$(echo $(cat hostlist))"

for i in ${HOSTLIST}
do
        DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep eard | awk '{print $1}'`
        echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"
        ssh ${i} sudo kill -TERM $DAEMON_PID &
done
