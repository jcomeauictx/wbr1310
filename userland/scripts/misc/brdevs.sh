#!/bin/sh

. /etc/config/defines

brdevs=`scut -p BRDEVS= -n6 $layout_config`
stp=`rgdb -g bridge.stp`

if [ -n $brdevs ]; then
case "$1" in
start)
	echo "Starting brdevs [$brdevs] ..."
	for i in $brdevs ; do
		brifs=`scut -p BRIFS_$i= -n6 $layout_config`
		echo "create $i [$brifs]..."
		brctl addbr $i
		ifconfig $i 192.168.0.1
		for j in $brifs ; do
			echo "add $j into $i ..."
			brctl addif $i $j
		done
		echo "Spanning Tree Protocol : $stp"
		if [ "$stp" = "true" ]; then
			brctl stp $i on
		else
			brctl stp $i off
		fi
		echo "bringing up $i ..."
		ifconfig $i up
	done
	;;
stop)
	echo "Stopping brdevs [$brdevs] ..."
	for i in $brdevs ; do
		echo "shutdown $i ..."
		ifconfig $i down
		echo "destroy $i ..."
		brctl delbr $i
	done
	;;
*)
	echo "Usage: $0 start|stop"
	;;
esac
fi
