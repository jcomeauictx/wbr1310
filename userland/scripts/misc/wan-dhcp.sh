#!/bin/sh
. /etc/config/defines
IF=`scut -p WANIF= $layout_config`

case "$1" in
start)
	CM=`rgdb -g /wan/rg/inf:1/dhcp/clonemac`
	echo "DHCP on WAN($IF)($CM) ..."
	# clone MAC
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh $CM
	# start dhcp
	echo "dhcp" > $wan_config
	/etc/scripts/udhcp/dhcpc.sh $IF start
	;;
stop)
	/etc/scripts/udhcp/dhcpc.sh $IF stop
	/etc/scripts/misc/wandown.sh
	;;
*)
	echo "Usage: wan-dhcp.sh {start|stop}"
	;;
esac
