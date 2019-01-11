#!/bin/sh

. /etc/config/defines

LANIF=`scut -p LANIF= $layout_config`

case "$1" in
start)
	echo "Start LAN interface $LANIF ..."
	ipaddr=`rgdb -g /lan/ethernet/ip`
	subnet=`rgdb -g /lan/ethernet/netmask`
	echo "Static ip on $LANIF $ipaddr/$subnet ..."
	if [ "$subnet" != "" ]; then
		NETMASK="netmask $subnet"
	else
		NETMASK=""
	fi
	ifconfig $LANIF $ipaddr $NETMASK

	rgdb -s /tmp/config/lanif $LANIF
	rgdb -s /tmp/config/lanip $ipaddr
	rgdb -s /tmp/config/lanmask $NETMASK
	rgdb -i -s /runtime/sys/info/lanmac "`/etc/scripts/status/lan.sh hwaddr`"

	/etc/scripts/udhcp/dhcpd.sh $LANIF clear
	/etc/scripts/udhcp/dhcpd.sh $LANIF start > /dev/null 2>&1
	/etc/scripts/misc/dnrd.sh dummy
	[ -f /etc/scripts/diffserv/classify.sh ] && /etc/scripts/diffserv/classify.sh
	;;
stop)
	echo "Stop LAN interface $LANIF ..."
	/etc/scripts/udhcp/dhcpd.sh $LANIF stop > /dev/null 2>&1
	ifconfig $LANIF 0.0.0.0 > /dev/null 2>&1
	;;
restart)
	$0 stop > /dev/null 2>&1
	$0 start > /dev/null 2>&1
	;;
status)
	ifconfig $LANIF
	;;
interface)
	echo "$LANIF"
	;;
*)
	echo "Usage: lan.sh {start|stop|restart|status|interface}"
	;;
esac
