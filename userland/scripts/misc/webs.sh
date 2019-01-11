#!/bin/sh
. /etc/config/defines
PIDF="/var/run/httpd.pid"
case "$1" in
password)
	if [ -f "/etc/httpasswd.php" ]; then
		rgdb -A /etc/httpasswd.php > /var/etc/httpasswd
	else
		name=`rgdb -g sys/account/superusername`
		passwd=`rgdb -g sys/account/superuserpassword`
		echo "$name:$passwd" > /var/etc/httpasswd
		name=`rgdb -g sys/account/normalusername`
		passwd=`rgdb -g sys/account/normaluserpassword`
		echo "$name:$passwd" >> /var/etc/httpasswd
	fi
	;;
start)
	if [ -f $PIDF ]; then
		PID=`cat $PIDF`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $PIDF
	fi

	if [ -f "/etc/httpd.php" ]; then
		rgdb -A /etc/httpd.php > /var/etc/httpd.cfg
	else
		rgdb -p /etc/httpd.cfg -e js > /var/etc/httpd.cfg
	fi

	$0 password > /dev/null 2>&1

	httpd -s $image_sign -f /var/etc/httpd.cfg &
	;;
stop)
	if [ -f $PIDF ]; then
		PID=`cat $PIDF`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $PIDF
	fi
	;;
restart)
	$0 stop > /dev/null 2>&1
	$0 start > /dev/null 2>&1
	;;
*)
	echo "Usage: webs.sh {start|stop|restart|password}"
	;;
esac
