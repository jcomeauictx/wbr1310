#!/bin/sh
type=`rgdb -g /wan/rg/inf:1/mode`
OPTS="/etc/ppp/options.$1"

case $type in
"3")
	AUTO=`rgdb -g /wan/rg/inf:1/pppoe/autoReconnect`
	;;
"4")
	AUTO=`rgdb -g /wan/rg/inf:1/pptp/autoReconnect`
	;;
"5")
	AUTO=`rgdb -g /wan/rg/inf:1/l2tp/autoReconnect`
	;;
esac

pppd file $OPTS
while [ ${AUTO} = "1" ]; do
	pppd file $OPTS
done
