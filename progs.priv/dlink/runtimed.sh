#!/bin/sh
echo [$0] $1 ... > /dev/console
TROOT=`rgdb -i -g /runtime/template_root`
[ "$TROOT" = "" ] && TROOT="/etc/templates"
if [ "$1" = "stop" ]; then
	rgdb -A $TROOT/runtimed_stop.php > /var/run/runtimed.sh
else
	rgdb -A $TROOT/runtimed.php > /var/run/runtimed.sh
fi
sh /var/run/runtimed.sh > /dev/console
