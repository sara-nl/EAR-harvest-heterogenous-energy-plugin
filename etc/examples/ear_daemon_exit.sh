#!/bin/bash
if [[ $# -eq 0 ]]
then
	echo "usage: ./ear_daemon_exit.sh hostlist"
	exit
fi

HOSTLIST=$(echo $(cat $1))

for i in ${HOSTLIST}
do
	DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep ear_daemon | awk '{print $1}'`
	echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"
	sudo ssh $i kill -TERM $DAEMON_PID &> /dev/null
done

