#!/bin/sh
echo [$0] ... > /dev/console
# Stop LLD2D
/etc/templates/lld2d.sh stop	> /dev/console
# Stop WAN
/etc/templates/wan.sh stop	> /dev/console
# Stop UPNP ...
/etc/templates/upnpd.sh stop	> /dev/console
# stop fresetd ...
killall fresetd			> /dev/console
# stop DNRD
/etc/templates/dnrd.sh stop	> /dev/console
# Stop RG
/etc/templates/rg.sh stop	> /dev/console
# Stop WLAN
/etc/templates/wlan.sh stop	> /dev/console
# Stop LAN
/etc/templates/lan.sh stop	> /dev/console
# enable Page programming mode
echo 1 > /proc/spiflash/pp_enable
