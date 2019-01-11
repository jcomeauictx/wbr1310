#!/bin/sh
LIF=`/etc/scripts/status/lan.sh interface`
WIF=`/etc/scripts/status/wan.sh interface`
WIP=`/etc/scripts/status/wan.sh ip`
WANODE="/runtime/wan/inf:1"

rgdb -s /tmp/config/lanif "$LIF"
rgdb -s /tmp/config/wanif "$WIF"
rgdb -s /tmp/config/wanip "$WIP"
rgdb -i -s "$WANODE/connectstatus" "`/etc/scripts/status/wan.sh status`"
rgdb -i -s "$WANODE/mac" "`/etc/scripts/status/wan.sh hwaddr`"
rgdb -i -s "$WANODE/ip"	"`/etc/scripts/status/wan.sh ip`"
rgdb -i -s "$WANODE/netmask" "`/etc/scripts/status/wan.sh mask`"
rgdb -i -s "$WANODE/gateway" "`/etc/scripts/status/wan.sh defaultgw`"
rgdb -i -s "$WANODE/primarydns" "`scut -p nameserver /etc/resolv.conf | pfile -l1`"
rgdb -i -s "$WANODE/secondarydns" "`scut -p nameserver /etc/resolv.conf | pfile -l2`"

[ -f /etc/scripts/misc/route.sh ] && /etc/scripts/misc/route.sh start
/etc/scripts/rg/rg.sh flush
[ -f /etc/scripts/diffserv/policy.sh ] && /etc/scripts/diffserv/policy.sh

/etc/scripts/udhcp/dhcpd.sh $LIF restart
/etc/scripts/misc/dnrd.sh start
/etc/scripts/misc/logs.sh krestart
/etc/scripts/misc/runtimed.sh restart
/etc/scripts/misc/ntp.sh start
/etc/scripts/misc/dyndns.sh
[ "`rgdb -g /tr069/enable`" = "1" ] && /etc/scripts/misc/tr069.sh start
rgdb -i -s "/runtime/dev_status" ""

