#!/bin/sh
. /etc/config/defines
IF=`scut -p WANIF= $layout_config`
MAC=$1
[ "$MAC" = "" ] && MAC=`rgcfg getenv -n $nvram -e wanmac`
# check if we need to set.
CUR=`ifconfig $IF | scut -p HWaddr`
if [ "$CUR" != "$MAC" ]; then
	ifconfig $IF down
	ifconfig $IF hw ether $MAC up
fi
exit 0
