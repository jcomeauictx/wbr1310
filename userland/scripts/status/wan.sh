#!/bin/sh

. /etc/config/defines

WANIF=`/etc/scripts/misc/wan.sh interface`
PHWIF=`scut -p WANIF= $layout_config`

case "$1" in
ip)
	[ "$WANIF" != "" ] && ifconfig $WANIF | scut -p "inet addr:"
	;;
mask)
	[ "$WANIF" != "" ] && ifconfig $WANIF | scut -p "Mask:"
	;;
hwaddr)
	[ "$WANIF" != "" ] && ifconfig $PHWIF | scut -p "HWaddr"
	;;
interface)
	echo $WANIF
	;;
mode)
	if [ -f $wan_config ]; then
		cat $wan_config
	else
		echo "inactive"
	fi
	;;
status)
	[ -f $wan_config ] && MD=`cat $wan_config`
	case "$MD" in
	pppoe|pptp|l2tp)
		if [ -f /var/run/ppp-wan1.status ]; then
			cat /var/run/ppp-wan1.status
		else
			echo "disconnected"
		fi
		;;
	*)
		ip=`ifconfig $WANIF | scut -p "inet addr:"`
		if [ "$ip" != "" ]; then
			echo "connected"
		else
			echo "disconnected"
		fi
	esac
	;;
defaultgw)
	route > /var/defgw
	cat /var/defgw | scut -n 2 > /var/defgw
	defgw=`scut -p "default" < /var/defgw`

	if [ "$defgw" = "" ]; then
		route -n | scut -p default
	else
		echo $defgw
	fi
	;;
*)
	echo "Usage: wan.sh {ip|mask|hwaddr|mode|status|interface|defaultgw}"
	;;
esac
