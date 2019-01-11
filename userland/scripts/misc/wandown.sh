#!/bin/sh
WANODE="/runtime/wan/inf:1"

rgdb -i -s "$WANODE/connectstatus" "disconnected"
rgdb -i -s "$WANODE/ip"	"0.0.0.0"
rgdb -i -s "$WANODE/netmask" "0.0.0.0"
rgdb -i -s "$WANODE/gateway" "0.0.0.0"
rgdb -i -s "$WANODE/primarydns" "0.0.0.0"
rgdb -i -s "$WANODE/secondarydns" "0.0.0.0"


/etc/scripts/misc/ntp.sh stop
/etc/scripts/misc/dnrd.sh stop
