#!/bin/bash
if [[ $# -eq 0 ]]
then
	echo "usage: ./ear_daemon_exit.sh hostlist"
	exit
fi


source ear_env_libs.sh
HOSTLIST=$(echo $(cat $1))

for i in ${HOSTLIST}
do
	DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep eard | awk '{print $1}'`
	echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"
	ssh $i sudo kill -TERM $DAEMON_PID &> /dev/null
done

