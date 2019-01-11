#!/bin/sh
. /etc/config/defines
PIDF="/var/run/ntpclient.pid"
case "$1" in
start)
	if [ -f "$PIDF" ]; then
		PID=`cat $PIDF`
		if [ "$PID" != "0" ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $PIDF
	fi

	en=`rgdb -g /time/syncwith`
	if [ "$en" != "2" ]; then
		echo "NTP disabled!"
		exit 0
	fi
	ntps=`rgdb -g /time/ntpserver/ip`
	if [ "$ntps" = "" ]; then
		ntps="clock.isc.org"
	fi
	ntpclient -h $ntps -s &
	echo $! > $PIDF
	;;
stop)
	if [ -f "$PIDF" ]; then
		PID=`cat $PIDF`
		if [ "$PID" != "0" ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $PIDF
	fi
	;;
*)
	echo "Usage: ntp.sh {start|stop}"
	exit 9
	;;
esac
exit 0
