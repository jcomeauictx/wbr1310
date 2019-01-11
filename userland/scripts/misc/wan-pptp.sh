#!/bin/sh
. /etc/config/defines
IF=`scut -p WANIF= $layout_config`
wpptp="/wan/rg/inf:1/pptp"
lname="wan1"
case "$1" in
start)
	IP=`rgdb -g $wpptp/ip`
	NM=`rgdb -g $wpptp/netmask`
	# Set MAC address back
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh
	[ "$IP" = "" -o "$IP" = "0.0.0.0" ] && echo "No ip address for pptp" && exit 9
	if [ "$NM" != "" -a "$NM" != "0.0.0.0" ]; then
		NM="netmask $NM"
	else
		NM=""
	fi
	ifconfig $IF $IP $NM
	echo "pptp" > $wan_config
	/etc/scripts/ppp/ppp-setup.sh pptp $lname
	/etc/scripts/ppp/ppp.sh start $lname
	;;
stop)
	/etc/scripts/ppp/ppp.sh stop $lname
	ifconfig $IF 0.0.0.0 > /dev/null 2>&1
	;;
*)
	echo "Usage: wan-pptp.sh {start|stop}"
	;;
esac
