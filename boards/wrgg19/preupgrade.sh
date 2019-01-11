#!/bin/sh
echo [$0] $1 ... > /dev/console
if [ "$1" = "restore" ]; then
	echo RESTORE > /dev/console
	/etc/templates/upnp.sh restart > /dev/console
	/etc/templates/lld2d.sh restart > /dev/console

else
	echo PREUPGRADE > /dev/console
	/etc/templates/upnpd.sh stop > /dev/console
	/etc/templates/lld2d.sh stop > /dev/console
fi
