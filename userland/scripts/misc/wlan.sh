#!/bin/sh

. /etc/config/defines
WLIF=`scut -p WLIF= $layout_config`

case "$1" in
start)
	echo "Start WLAN interface $WLIF ..."
	echo "[Dummy wlan scripts, do something to bring up WLAN interface]"
	;;
stop)
	echo "Stop WLAN interface $WLIF ..."
	echo "[Dummy wlan scripts, do something to bring down WLAN interface]"
	;;
restart)
	$0 stop
	$0 start
	;;
status)
	ifconfig $WLIF
	;;
interface)
	echo "$WLIF"
	;;
*)
	echo "Usage: wlan.sh {start|stop|status|interface|restart}"
	;;
esac
