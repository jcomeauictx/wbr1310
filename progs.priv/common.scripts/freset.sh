#!/bin/sh
. /etc/config/defines
echo [$0] ... > /dev/console
sleep 5
/etc/templates/wan.sh stop > /dev/console
rgcfg save -n $nvram -c $defaultconfig -v
echo "[$0] reset config done !" > /dev/console
reboot
