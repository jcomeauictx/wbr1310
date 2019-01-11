#!/bin/sh
echo [$0] $1 $2 ... > /dev/console
TROOT=`rgdb -i -g /runtime/template_root`
[ "$TROOT" = "" ] && TROOT="/etc/templates"
case "$1" in
genxml)
	rgdb -i -s /runtime/upnp/host "$3"
	rgdb -i -s /runtime/upnp/port "$4"
	rgdb -i -s /runtime/upnp/uuid "$5"
	rgdb -A $TROOT/upnpd/InternetGatewayDevice.php > "$2"
	;;
start)
	mkdir -p /var/upnp
	cp $TROOT/upnpd/*.xml "$2"/.
	;;
esac
