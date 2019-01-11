#!/bin/sh

. /etc/config/defines
WLIF=`scut -p WLIF= $layout_config`

case "$1" in
ssid)
	echo "dummy"
	;;
channel)
	echo "1"
	;;
bssid)
	echo "000102030405"
	;;
privacy)
	echo "Disabled"
	;;
interface)
	echo $WLIF
	;;
*)
	echo "Usage: wlan.sh {ssid|channel|bssid|privacy|interface}"
	;;
esac
