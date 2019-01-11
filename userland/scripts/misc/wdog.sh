#!/bin/sh

pidfile="/var/run/wdog.pid"

case "$1" in
start)
	period=`cat /proc/driver/watchdog`
	echo "START" > /proc/driver/watchdog
	watchdog -p $period -d /proc/driver/watchdog &
	echo $! > $pidfile
	;;
stop)
	if [ -f $pidfile ]; then
		pid=`cat $pidfile`
		if [ $pid != 0 ]; then
			kill $pid > /dev/null 2>&1
		fi
		echo "STOP" > /proc/driver/watchdog
		rm -f $pidfile
	fi
	;;
*)
	echo "Usage: wdog.sh {start|stop}"
	;;
esac
