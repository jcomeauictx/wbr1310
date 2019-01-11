#!/bin/sh

en=`rgdb -g /ddns/enable`
[ "$en" != "1" ] && exit 0

pr=`rgdb -g /ddns/provider`
case "$pr" in
"1")
	SERV="members.dyndns.org"
	;;
"2")
	SERV="dynupdate.no-ip.com"
	;;
"3")
	SERV="dup.hn.org"
	;;
"4")
	SERV="dynamic.zoneedit.com"
	;;
*)
	exit 9
	;;
esac

HOST=`rgdb -g /ddns/hostname`
USER=`rgdb -g /ddns/user`
PASS=`rgdb -g /ddns/password`
IPAD=`/etc/scripts/status/wan.sh ip`

PIDF='/var/run/dyndns.pid'
dyndns --host $HOST --ip $IPAD --pass $PASS --serv $SERV --user $USER &
echo $! > $PIDF
