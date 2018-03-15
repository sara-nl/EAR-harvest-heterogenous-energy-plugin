#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo -e "Usage: ./ssh_daemon_exit.sh hostlist"
	echo -e "\thostlist: a host name list file, a host name list or 'local'"
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
	echo "Closing ear_daemon in ${i} with pid $DAEMON_PID"

	if [ "x$1" == "xlocal" ]
        then
        	DAEMON_PID=`ps -eo pid,comm | grep eard | awk '{print $1}'`
        	sudo kill -TERM $DAEMON_PID
        else
        	DAEMON_PID=`sudo ssh $i ps -eo pid,comm | grep eard | awk '{print $1}'`
        	ssh ${i} sudo kill -TERM $DAEMON_PID
		fi
done
