#!/bin/sh
. /etc/config/defines
wpppoe="/wan/rg/inf:1/pppoe"
lkname="wan1"
case "$1" in
start)	# get settings
	CM=`rgdb -g $wpppoe/cloneMac`
	IF=`scut -p WANIF= $layout_config`
	# clone MAC
	[ -f /etc/scripts/misc/wan-clonemac.sh ] && /etc/scripts/misc/wan-clonemac.sh $CM
	# start pppoe
	echo "pppoe" > $wan_config
	/etc/scripts/ppp/ppp-setup.sh pppoe $lkname sync
	/etc/scripts/ppp/ppp.sh start $lkname
	;;
stop)
	/etc/scripts/ppp/ppp.sh stop $lkname
	;;
*)
	echo "Usage: wan-pppoe.sh {start|stop}"
	;;
esac
