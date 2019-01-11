#!/bin/sh
. /etc/config/defines
WM=`rgdb -g /wan/rg/inf:1/mode`
WANIF=`scut -p WANIF= $layout_config`
case "$1" in
start)
	echo "Start WAN interface $WANIF..."
	[ -f /etc/scripts/misc/setwantype.sh ] && /etc/scripts/misc/setwantype.sh
	case "$WM" in
	"1")
		/etc/scripts/misc/wan-static.sh start
		;;
	"2")
		/etc/scripts/misc/wan-dhcp.sh start
		;;
	"3")
		/etc/scripts/misc/wan-pppoe.sh start
		;;
	"4")
		/etc/scripts/misc/wan-pptp.sh start
		;;
	"5")
		/etc/scripts/misc/wan-l2tp.sh start
		;;
	"7")
		/etc/scripts/misc/wan-bigpond.sh start
		;;
	esac
	;;
stop)
	[ -f $wan_config ] && MD=`cat $wan_config`
	if [ "$MD" != "" ]; then
		/etc/scripts/misc/wan-$MD.sh stop
		echo -n > /etc/resolv.conf
	else
		echo "WAN is not active."
	fi
	;;
restart)
	$0 stop > /dev/console 2>&1
	$0 start > /dev/console 2>&1
	;;
interface)
	[ -f $wan_config ] && MD=`cat $wan_config`
	case "$MD" in
	pppoe|pptp|l2tp)
		/etc/scripts/ppp/pppif.sh wan1
		;;
	*)
		echo $WANIF
		;;
	esac
	;;
*)
	echo "Usage: wan.sh {start|stop|restart|interface}"
	;;
esac
