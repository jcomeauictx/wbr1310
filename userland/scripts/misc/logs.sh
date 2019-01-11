#!/bin/sh
. /etc/config/defines
SLOG="/var/run/syslogd.pid"
KLOG="/var/run/klogd.pid"
case "$1" in
start)
    	mailserver=`rgdb -g /sys/log/mailserver`
        email=`rgdb -g /sys/log/email`
	LANIF=`/etc/scripts/misc/lan.sh interface`
	WANIF=`/etc/scripts/misc/wan.sh interface`
	hostname=`rgdb -g /sys/hostName`
	
	opts=""
	[ "1" = `rgdb -g /security/log/systeminfo` ]		&& opts="$opts -F sysact"
	[ "1" = `rgdb -g /security/log/debuginfo` ]		&& opts="$opts -F debug"
	[ "1" = `rgdb -g /security/log/attackinfo` ]		&& opts="$opts -F attack"
	[ "1" = `rgdb -g /security/log/droppacketinfo` ]	&& opts="$opts -F drop"
	[ "1" = `rgdb -g /security/log/noticeinfo` ]		&& opts="$opts -F notice"
	[ "" != "$mailserver" ] && [ "" != "$email" ]		&& opts="$opts -m -S $mailserver -a $email -H \"$hostname\""
	    
	if [ "$opts" != "" ]; then
		syslogd $opts &
		echo $! > $SLOG
		klogd -l $LANIF -w $WANIF &
		echo $! > $KLOG
	fi
	;;
stop)
	if [ -f "$SLOG" ]; then
		PID=`cat $SLOG`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $SLOG
	fi
	if [ -f "$KLOG" ]; then
		PID=`cat $KLOG`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $KLOG
	fi
	;;
restart)
	$0 stop > /dev/null 2>&1
	$0 start > /dev/null 2>&1
	;;
krestart)
	if [ -f "$KLOG" ]; then
		PID=`cat $KLOG`
		if [ $PID != 0 ]; then
			kill $PID > /dev/null 2>&1
		fi
		rm -f $KLOG
	fi
	LANIF=`/etc/scripts/misc/lan.sh interface`
	WANIF=`/etc/scripts/misc/wan.sh interface`
	klogd -l $LANIF -w $WANIF &
	echo $! > $KLOG
	;;
*)
	echo "Usage: logs.sh {start|stop|restart|krestart}"
	;;
esac
exit 0
