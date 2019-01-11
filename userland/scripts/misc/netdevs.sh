#!/bin/sh

. /etc/config/defines

netdevs=`scut -p NETDEVS= -n6 $layout_config`

case "$1" in
start)
	echo "Start up Network Devices [$netdevs] ..."
	for i in $netdevs ; do
		macaddr=`scut -p MAC_ADDRESS_$i= $layout_config`
		echo "$i [$macaddr] is bring up ..."
		if [ "$macaddr" != "" ]; then
			ifconfig $i hw ether $macaddr up
		else
			ifconfig $i up
		fi
	done
	;;
stop)
	echo "Shutdown Network Devices [$netdevs] ..."
	for i in $netdevs ; do
		echo "$i is going down ..."
		ifconfig $i down
	done
	;;
*)
	echo "Usage: $0 start|stop"
	;;
esac
