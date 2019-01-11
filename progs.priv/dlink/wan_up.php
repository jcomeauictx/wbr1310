#!/bin/sh
echo "[$0] ..." > /dev/console
rgdb -i -s /runtime/wan/inf:1/uptime `uptime seconly`
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$rg_script=1;

$router = query("/runtime/router/enable");
if ($router != 1)
{
	$wanif = query("/runtime/wan/inf:1/interface");
	if ($wanif != "")
	{
		echo "xmldbc -x /runtime/stats/wan/inf:1/rx/bytes \"get:scut -p ".$wanif.": -f 1 /proc/net/dev\"\n";
		echo "xmldbc -x /runtime/stats/wan/inf:1/rx/packets \"get:scut -p ".$wanif.": -f 2 /proc/net/dev\"\n";
		echo "xmldbc -x /runtime/stats/wan/inf:1/tx/bytes \"get:scut -p ".$wanif.": -f 9 /proc/net/dev\"\n";
		echo "xmldbc -x /runtime/stats/wan/inf:1/tx/packets \"get:scut -p ".$wanif.": -f 10 /proc/net/dev\"\n";
	}
	/* restart klogd to update WAN interface changed. */
	$klogd_only=1;
	require($template_root."/misc/logs_run.php");

	/* update others ... when WAN is up. */
	$generate_start=1;
	require($template_root."/misc/ntp_run.php");

	/* start UPNP. */
	echo $template_root."/upnpd.sh start\n";
}
else
{
	/* Relay the Domain info to LAN's DHCP clients. */
	$generate_start = 0;
	$dhcpd_if=query("/runtime/layout/lanif");
	$dhcpd_clearleases=0;
	require($template_root."/dhcp/dhcpd.php");
	$generate_start = 1;
	require($template_root."/dhcp/dhcpd.php");

	if (query("/runtime/wan/inf:1/connecttype") == 7)
	{
		$bpa_conf	= "/var/bpalogin.conf";
		$bpa_user	= query("/wan/rg/inf:1/bigpond/username");
		$bpa_passwd	= query("/wan/rg/inf:1/bigpond/password");
		$bpa_server	= query("/wan/rg/inf:1/bigpond/server");
		$bpa_domain	= query("/runtime/wan/inf:1/domain");

		fwrite($bpa_conf,  "username ".$bpa_user."\n");
		fwrite2($bpa_conf, "password ".$bpa_passwd."\n");
		fwrite2($bpa_conf, "authserver ".$bpa_server."\n");
		fwrite2($bpa_conf, "authdomain ".$bpa_domain."\n");
		fwrite2($bpa_conf, "localport 5050\n");

		echo "killall bpalogin > /dev/null 2>&1\n";
		echo "bpalogin -c ".$bpa_conf." -D > /dev/console &\n";
	}

	echo $template_root."/rg.sh wanup > /dev/console\n";

	/* Start DNRD when WAN is up ... */
	echo $template_root."/dnrd.sh restart > /dev/console\n";

	/* restart runtimed to update WAN interface changed. */
	if ($VeRsIoN>=2)
	{
		$wanif = query("/runtime/wan/inf:1/interface");
		if ($wanif != "")
		{
			echo "xmldbc -x /runtime/stats/wan/inf:1/rx/bytes \"get:scut -p ".$wanif.": -f 1 /proc/net/dev\"\n";
			echo "xmldbc -x /runtime/stats/wan/inf:1/rx/packets \"get:scut -p ".$wanif.": -f 2 /proc/net/dev\"\n";
			echo "xmldbc -x /runtime/stats/wan/inf:1/tx/bytes \"get:scut -p ".$wanif.": -f 9 /proc/net/dev\"\n";
			echo "xmldbc -x /runtime/stats/wan/inf:1/tx/packets \"get:scut -p ".$wanif.": -f 10 /proc/net/dev\"\n";
		}
	}
	else
	{
		$generate_start=1;
		require($template_root."/misc/runtimed_run.php");
	}

	/* restart klogd to update WAN interface changed. */
	$klogd_only=1;
	require($template_root."/misc/logs_run.php");

	/* update others ... when WAN is up. */
	$generaet_start=1;
	require($template_root."/misc/ntp_run.php");
	require($template_root."/misc/dyndns_run.php");

	/* Kick upnpd to send notify. */
	echo "echo Kicking UPNPD ... > /dev/console\n";
	echo "killall -SIGUSR1 upnpd\n";
}
?>
