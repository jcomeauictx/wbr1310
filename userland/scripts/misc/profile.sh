#!/bin/sh
. /etc/config/defines
case "$1" in
get)
	rgcfg get -n $nvram -c /var/run/rgdb.xml.gz
	if [ "$?" != "0" ]; then
		echo "Can't get config from nvram, generate default!"
		/etc/scripts/misc/profile.sh reset
		/etc/scripts/misc/profile.sh put
		exit 0
	fi
	gunzip /var/run/rgdb.xml.gz
	rgdb -l /var/run/rgdb.xml
	if [ "$?" != "0" ]; then
		echo "Invalid config, generate default!"
		/etc/scripts/misc/profile.sh reset
		/etc/scripts/misc/profile.sh put
	fi
	rm -f /var/run/rgdb.xml
	;;
put)
	rgdb -D /var/run/rgdb.xml
	gzip /var/run/rgdb.xml
	rgcfg save -n $nvram -c /var/run/rgdb.xml.gz
	if [ "$?" = "0" ]; then
		echo "ok"
	else
		echo "failed"
	fi
	rm -f /var/run/rgdb.xml.gz
	cd
	;;
reset)
	if [ "$2" != "" ]; then
		cp $2 /var/run/rgdb.xml.gz
		rm -f $2
	else
		cp $defaultconfig /var/run/rgdb.xml.gz
	fi
	gunzip /var/run/rgdb.xml.gz
	rgdb -l /var/run/rgdb.xml
	rm -f rgdb.xml
	cd
	;;
*)
	echo "Usage: $0 get/put/reset"
esac
