#!/bin/sh

parseOK="true"

case "$1" in
packets)
	Pattern="packets:"
	;;
bytes)
	Pattern="bytes:"
	NumField="-n 3"
        ;;
*)
	parseOK="false";
	;;
esac

case "$2" in
wlanrx)
	Device=`/etc/scripts/misc/wlan.sh interface`
	Pattern="RX $Pattern"
	;;
wlantx)
	Device=`/etc/scripts/misc/wlan.sh interface`
	Pattern="TX $Pattern"
	;;
lanrx)
	Device=`/etc/scripts/misc/lan.sh interface`
	Pattern="RX $Pattern"
	;;
lantx)
	Device=`/etc/scripts/misc/lan.sh interface`
	Pattern="TX $Pattern"
	;;
wanrx)
	Device=`/etc/scripts/misc/wan.sh interface`
	Pattern="RX $Pattern"
	;;
wantx)
	Device=`/etc/scripts/misc/wan.sh interface`
	Pattern="TX $Pattern"
	;;
*)
	parseOK="false"
	;;
esac

if [ $parseOK = "true" ]; then
	ifconfig $Device | scut -p "$Pattern" $NumField
else
	echo "Usage: stats.sh {packets|bytes} {wlantx|wlanrx|lantx|lanrx|wantx|wanrx}"
fi
