#!/bin/sh
echo [$0] $1 ... > /dev/console
TROOT=`rgdb -i -g /runtime/template_root`
[ "$TROOT" = "" ] && TROOT="/etc/templates"
case "$1" in
start|restart)
	[ -f /var/run/wan_stop.sh ] && sh /var/run/wan_stop.sh > /dev/console
	rgdb -A $TROOT/wan_start.php > /var/run/wan_start.sh
	rgdb -A $TROOT/wan_stop.php > /var/run/wan_stop.sh
	sh /var/run/wan_start.sh > /dev/console
	;;
stop)
	if [ -f /var/run/wan_stop.sh ]; then
		sh /var/run/wan_stop.sh > /dev/console
		rm -f /var/run/wan_stop.sh
	fi
	;;
*)
	echo "usage: $0 {start|stop|restart}"
	;;
esac
