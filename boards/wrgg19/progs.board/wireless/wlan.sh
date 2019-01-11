#!/bin/sh
echo [$0] $1 ... > /dev/console
TROOT=`rgdb -i -g /runtime/template_root`
[ "$TROOT" = "" ] && TROOT="/etc/templates"

# Stop wlan first.
if [ -f /var/run/wlan_stop.sh ]; then
	sh /var/run/wlan_stop.sh > /dev/console
	rm -f /var/run/wlan_stop.sh
fi

# Start wlan...
if [ "$1" != "stop" ]; then
	rgdb -A $TROOT/wlan_run.php -V generate_start=1 > /var/run/wlan_start.sh
	rgdb -A $TROOT/wlan_run.php -V generate_start=0 > /var/run/wlan_stop.sh
	sh /var/run/wlan_start.sh > /dev/console
fi
exit 0
