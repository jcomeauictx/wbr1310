#!/bin/sh
. /etc/config/defines
wstatic="/wan/rg/inf:1/static"
dnssver="/dnsRelay/server"
IF=`scut -p WANIF= $layout_config`

case "$1" in
start)	# get settings	
	IP=`rgdb -g $wstatic/ip`
	NM=`rgdb -g $wstatic/netmask`
	GW=`rgdb -g $wstatic/gateway`
	CM=`rgdb -g $wstatic/cloneMac`
	MT=`rgdb -g $wstatic/mtu`
	echo "Static IP on WAN($IF) $IP/$NM/$GW/$CM/$MT ..."
	# clone MAC
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh $CM
	# netmask
	if [ "$NM" != "" -a "$NM" != "0.0.0.0" ]; then
		NETMASK="netmask $NM"
	else
		NETMASK=""
	fi
	# MTU
	if [ "$MT" != "" -a "$MT" != "0" ]; then
		MTU="mtu $MT"
	else
		MTU=""
	fi
	# setup i/f
	ifconfig $IF $IP $NETMASK $MTU
	# setup default gw
	if [ "$GW" != "" -a "$GW" != "0.0.0.0" ]; then
		while route del default gw 0.0.0.0 dev $IF ; do
			:
		done
		route add default gw $GW dev $IF
	fi
	# setup nameserver
	echo -n > /etc/resolv.conf
	DNS=`rgdb -g $dnssver/primaryDns`
	[ "$DNS" != "" -a "$DNS" != "0.0.0.0" ] && echo "nameserver $DNS" >> /etc/resolv.conf
	DNS=`rgdb -g $dnssver/secondaryDns`
	[ "$DNS" != "" -a "$DNS" != "0.0.0.0" ] && echo "nameserver $DNS" >> /etc/resolv.conf
	# mark configuration
	echo "static" > $wan_config
	[ -f /etc/scripts/misc/wanup.sh ] && /etc/scripts/misc/wanup.sh
	;;
stop)
	/etc/scripts/misc/wandown.sh
	ifconfig $IF 0.0.0.0 > /dev/null 2>&1
	;;
*)
	echo "Usage: wan-static.sh {start|stop}"
	;;
esac
