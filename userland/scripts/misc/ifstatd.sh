#!/bin/sh

. /etc/config/defines
LANIF=`/etc/scripts/misc/lan.sh interface`
WANIF=`/etc/scripts/misc/wan.sh interface`
WLIF=`/etc/scripts/misc/wlan.sh interface`
pidfile="/var/run/ifstatpatch.pid"

case "$1" in
start)
	PARAM=""
	if [ "$LANIF" != "" ]; then
		PARAM="-l $LANIF"
	fi
	if [ "$WANIF" != "" ]; then
		PARAM="$PARAM -w $WANIF"
	fi
	if [ "$WLIF" != "" ]; then
		PARAM="$PARAM -r $WLIF"
	fi
	ifstatsxmlpatch $PARAM &
	echo $! > $pidfile
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
restart)
	$0 stop > /dev/null 2>&1
	$0 start > /dev/null 2>&1
	;;
*)
	echo "Usage: ifstatd.sh {start|stop|restart}"
	;;
esac
