#!/bin/sh
. /etc/config/defines
case "$1" in
start)
	echo "Mounting proc and var ..."
	mount -t proc none /proc
	mount -t ramfs ramfs /var
	mkdir -p /var/etc /var/log /var/run /var/state /var/tmp /var/etc/ppp /var/etc/config /var/dnrd /var/etc/iproute2
	echo -n > /var/etc/resolv.conf
	echo -n > /var/TZ

	echo "***************  |  SYS:001" > /var/log/messages

	echo "Inserting modules ..." > /dev/console
	insmod /lib/modules/sw_tcpip.o
	insmod /lib/modules/ifresetcnt.o
	# wireless driver
	insmod /lib/modules/wlan.o
	insmod /lib/modules/wlan_xauth.o
	insmod /lib/modules/wlan_wep.o
	insmod /lib/modules/wlan_tkip.o
	insmod /lib/modules/wlan_scan_sta.o
	insmod /lib/modules/wlan_scan_ap.o
	insmod /lib/modules/wlan_ccmp.o
	insmod /lib/modules/wlan_acl.o
	insmod /lib/modules/ath_hal.o
	insmod /lib/modules/ath_rate_atheros.o
	# get the country code for madwifi, default is fcc.
	ccode=`rgcfg getenv -n $nvram -e countrycode`
	[ "$ccode" = "" ] && ccode="840"
	insmod /lib/modules/ath_ahb.o countrycode=$ccode

	wlanconfig ath0 create wlandev wifi0 wlanmode ap
	env_wlan=`rgcfg getenv -n $nvram -e wlanmac`
	[ "$env_wlan" = "" ] && env_wlan="00:52:40:e1:00:02"
	ifconfig ath0 hw ether $env_wlan
	insmod /lib/modules/ar231x_gpio.o

	# prepare db...
	echo "Start xmldb ..." > /dev/console
	xmldb -n $image_sign -t > /dev/console &
	sleep 1
	/etc/scripts/misc/profile.sh get
	# set fix nodes
	[ -f /etc/scripts/set_nodes.sh ] && /etc/scripts/set_nodes.sh
	# set brand related node value
	[ -f /etc/scripts/brand.sh ] && /etc/scripts/brand.sh

	/etc/templates/timezone.sh set
	/etc/templates/logs.sh

	# bring up network devices
	env_wan=`rgcfg getenv -n $nvram -e wanmac`
	[ "$env_wan" = "" ] && env_wan="00:52:40:e1:00:01"
	ifconfig eth0 hw ether $env_wan up
	rgdb -i -s /runtime/wan/inf:1/mac "$env_wan"
	
	TIMEOUT=`rgdb -g /nat/general/tcpidletimeout`
	[ "$TIMEOUT" = "" ] && TIMEOUT=7200 && rgdb -s /nat/general/tcpidletimeout $TIMEOUT
	echo "$TIMEOUT" > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established

	# Setup VLAN
	vconfig set_name_type VLAN_PLUS_VID_NO_PAD > /dev/null 2>&1
	vconfig add eth0 1	> /dev/null 2>&1
	vconfig add eth0 2	> /dev/null 2>&1
	brctl addbr br0 	> /dev/console
	brctl stp br0 off	> /dev/console
	brctl setfd br0 0	> /dev/console

	# Start up LAN interface & httpd.
	ifconfig br0 up			> /dev/console
	/etc/templates/webs.sh start	> /dev/console

	# setup extended function for runtime node
	[ -f /etc/scripts/setext.sh ] && /etc/scripts/setext.sh
	;;
stop)
	umount /tmp
	umount /proc
	umount /var
	;;
esac
