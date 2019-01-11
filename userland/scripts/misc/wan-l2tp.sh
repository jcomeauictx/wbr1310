#!/bin/sh
. /etc/config/defines
IF=`scut -p WANIF= $layout_config`
wl2tp="/wan/rg/inf:1/l2tp"
lname="wan1"
case "$1" in
start)
	IF=`scut -p WANIF= $layout_config`
	MD=`rgdb -g $wl2tp/mode`
	# Set MAC address back
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh
	if [ "$MD" != "2" ]; then
		IP=`rgdb -g $wl2tp/staticIp`
		NM=`rgdb -g $wl2tp/staticNetmask`
		if [ "$IP" = "" -o "$IP" = "0.0.0.0" ]; then
			echo "No ip address for l2tp"
			exit 9
		fi
		if [ "$NM" != "" -a "$NM" != "0.0.0.0" ]; then
			NM="netmask $NM"
		else
			NM=""
		fi
		ifconfig $IF $IP $NM
		echo "l2tp" > $wan_config
		/etc/scripts/ppp/ppp-setup.sh l2tp $lname
		/etc/scripts/ppp/ppp.sh start $lname
	else
		echo "l2tp" > $wan_config
		/etc/scripts/udhcp/dhcpc.sh $IF start
	fi
	;;
stop)
	/etc/scripts/ppp/ppp.sh stop $lname
	/etc/scripts/udhcp/dhcpc.sh $IF stop
	;;
*)
	echo "Usage: wan-l2tp.sh {start|stop}"
	;;
esac
