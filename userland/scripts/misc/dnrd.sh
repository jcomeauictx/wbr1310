#!/bin/sh
. /etc/config/defines
PIDFILE=/var/run/dnrd.pid
case "$1" in
start)
	[ `rgdb -g /dnsrelay/mode` = "1" ] && exit 0

	if [ -f $PIDFILE ]; then
		PID=`cat $PIDFILE`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $PIDFILE
	fi

	DNSS=`scut -p nameserver /etc/resolv.conf`
	DNRD="dnrd"
	for i in $DNSS ; do
		DNRD="$DNRD -s $i"
	done

	if [ "$DNRD" != "dnrd" ]; then
		$DNRD &
	fi
	;;
stop)
	if [ -f $PIDFILE ]; then
		PID=`cat $PIDFILE`
		if [ $PID != 0 ]; then
			kill -9 $PID > /dev/null 2>&1
		fi
		rm -f $PIDFILE
	fi
	;;
restart)
	$0 stop > /dev/null 2>&1
	$0 start > /dev/null 2>&1
	;;
dummy)
	$0 stop > /dev/null 2>&1
	[ `rgdb -g /dnsrelay/mode` = "1" ] && exit 0
	dnrd -s 140.113.1.1 &
	;;
*)
	echo "Usage: dnrd.sh {start|stop|restart}"
	;;
esac
