#!/bin/sh
. /etc/config/defines
case "$1" in
start)
	echo "setup layout ..."		> /dev/console
	/etc/scripts/layout.sh start	> /dev/console
	echo "start LAN ..."		> /dev/console
	/etc/templates/lan.sh start	> /dev/console
	echo "start fresetd ..."	> /dev/console
	fresetd &
	# Power on all LAN ports (0~3)
	echo "Power on LAN ports ..."	> /dev/console
	/etc/scripts/enlan.sh		> /dev/console
	echo "start WLAN ..."		> /dev/console
	/etc/templates/wlan.sh start	> /dev/console
	echo "start RG ..."		> /dev/console
	/etc/templates/rg.sh start	> /dev/console
	echo "start DNRD ..."		> /dev/console
	/etc/templates/dnrd.sh start	> /dev/console
	# start telnet daemon
	/etc/scripts/misc/telnetd.sh	> /dev/console
	# Start UPNP igd.
	if [ "`rgdb -i -g /runtime/router/enable`" = "1" ]; then
	  echo "start UPNPD ..."	> /dev/console
	  /etc/templates/upnpd.sh start	> /dev/console
	fi
	echo "start WAN ..."		> /dev/console
	/etc/scripts/misc/setwantype.sh	> /dev/console
	/etc/templates/wan.sh start	> /dev/console
	echo "start LLD2D ..."		> /dev/console
	/etc/templates/lld2d.sh start	> /dev/console
	
	;;
stop)
	echo "stop LLD2D ..."		> /dev/console
	/etc/templates/lld2d.sh stop	> /dev/console
	echo "stop WAN ..."		> /dev/console
	/etc/templates/wan.sh stop	> /dev/console
	if [ "`rgdb -i -g /runtime/router/enable`" = "1" ]; then
	  echo "stop UPNPD ..."		> /dev/console
	  /etc/templates/upnpd.sh stop	> /dev/console
	fi
	echo "stop fresetd ..."		> /dev/console
	killall fresetd			> /dev/console
	echo "stop DNRD ..."		> /dev/console
	/etc/templates/dnrd.sh stop	> /dev/console
	echo "stop RG ..."		> /dev/console
	/etc/templates/rg.sh stop	> /dev/console
	echo "stop WLAN ..."		> /dev/console
	/etc/templates/wlan.sh stop	> /dev/console
	echo "stop LAN ..."		> /dev/console
	/etc/templates/lan.sh stop	> /dev/console
	echo "reset layout ..."		> /dev/console
	/etc/scripts/layout.sh stop	> /dev/console
	;;
restart)
	sleep 3
	$0 stop
	$0 start
	;;
*)
	echo "Usage: system.sh {start|stop|restart}"
	;;
esac
exit 0
