#!/bin/bash

if [[ $# -ne 1 ]]
then
  echo -e "Usage: ./ssh_daemon_exit.sh hostlist p_state"
  echo -e "\thostlist: a file containing the list of hosts"
  exit 1
fi

if [ ! -f $1 ]
then
        if [ "x$1" != "xlocal" ]
        then
                export HOSTLIST="$1"
        else
                export HOSTLIST="$(hostname -s)"
        fi
else
        export HOSTLIST="$(echo $(cat $1))"
fi

for i in ${HOSTLIST}
do
        DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep eard | awk '{print $1}'`
        echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"
        ssh ${i} sudo kill -TERM $DAEMON_PID &
done
