#!/bin/sh
. /etc/config/defines
pidfile="/var/run/bpalogin.pid"

case "$1" in
start)
	rgdb -A /etc/template/bpalogin.php  > /var/bpalogin.conf
	/usr/sbin/bpalogin
	;;
stop)
	if [ -f $pidfile ]; then
		pid=`cat $pidfile`
		if [ $pid != 0 ]; then
			kill $pid > /dev/null 2>&1
		fi
		rm -f $pidfile
	fi
	;;
*)
	echo "Usage: bigpond.sh {start|stop}"
	exit 9
	;;
esac
exit 0
