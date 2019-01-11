#!/bin/sh
echo [$0] ... > /dev/console
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$router = query("/runtime/router/enable");
if ($router==1)	{ $interface = query("/runtime/layout/lanif"); }
else			{ $interface = query("/runtime/wan/inf:1/interface"); }

if ($generate_start==1)
{
	if (query("/upnp/enable")==1)
	{
		echo "route add -net 239.0.0.0 netmask 255.0.0.0 ".$interface."\n";
		echo "[ -d /var/upnp ] || mkdir -p /var/upnp\n";
		echo "rm -f /var/upnp/\*\n";
		echo "upnpd -i ".$interface." -s ".$template_root."/upnpd/upnphelper.sh -t ".$template_root."/upnpd > /dev/console &\n";
	}
}
else
{
	echo "echo Stopping UPNPD ... > /dev/console\n";
	echo "killall upnpd\n";
	echo "rm -f /var/miniServerStopSock\n";
	echo "route del -net 239.0.0.0 netmask 255.0.0.0 ".$interface."\n";
}
?>
