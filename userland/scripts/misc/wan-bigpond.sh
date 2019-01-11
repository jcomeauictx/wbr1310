#!/bin/sh
. /etc/config/defines
IF=`scut -p WANIF= $layout_config`
wbp="/wan/rg/inf:1/bigpond"

case "$1" in
start)
	IF=`scut -p WANIF= $layout_config`
	MD=`rgdb -g $wbp/mode`
	# Set MAC address back
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh
	echo "bigpond" > $wan_config
	/etc/scripts/udhcp/dhcpc.sh $IF start
	;;
stop)
	/etc/scripts/misc/bigpond.sh stop
	/etc/scripts/udhcp/dhcpc.sh $IF stop
	;;
*)
	echo "Usage: wan-bigpond.sh {start|stop}"
	;;
esac
