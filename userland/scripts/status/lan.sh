#!/bin/sh
IF=`/etc/scripts/misc/lan.sh interface`
case "$1" in
ip)		ifconfig $IF | scut -p "inet addr:"
		;;
mask)		ifconfig $IF | scut -p "Mask:"
		;;
hwaddr)		ifconfig $IF | scut -p "HWaddr"
		;;
interface)	echo $IF
		;;
dhcpd)		/etc/scripts/udhcp/dhcpd.sh $LANIF status
		;;
*)		echo "Usage: lan.sh {ip|mask|hwaddr|interface|dhcpd}"
		;;
esac
exit 0
