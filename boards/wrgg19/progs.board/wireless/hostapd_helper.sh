#!/bin/sh
echo [$0] $1 ... > /dev/console
case "$1" in
enter_neg)
	echo 4 > /var/run/gpio_wlan
	killall -SIGUSR1 fresetd > /dev/console 2>&1
	;;
enter_p1)
	echo 2 > /var/run/gpio_wlan
	killall -SIGUSR1 fresetd > /dev/console 2>&1
	;;
enter_p2)
	echo 3 > /var/run/gpio_wlan
	killall -SIGUSR1 fresetd > /dev/console 2>&1
	;;
p1_to_p2)
	echo 3 > /var/run/gpio_wlan
	killall -SIGUSR1 fresetd > /dev/console 2>&1
	jsp2info
	;;
esac

