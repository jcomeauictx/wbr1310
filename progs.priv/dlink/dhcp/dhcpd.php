<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$dhcpd_pidf ="/var/run/udhcpd-".$dhcpd_if.".pid";
$dhcpd_conf ="/var/run/udhcpd-".$dhcpd_if.".conf";
$dhcpd_lease="/var/run/udhcpd-".$dhcpd_if.".lease";
$dhcpd_patch="/var/run/dhcppatch-".$dhcpd_if.".pid";

if ($generate_start==1)
{
	echo "echo \"Start DHCP server (".$dhcpd_if.") ...\" > /dev/console\n";
	if ($dhcpd_clearleases==1) { echo "echo -n > ".$dhcpd_lease."\n"; }
	if (query("/runtime/router/enable")!="1")
	{
		echo "echo Router function is off, DHCP server will not be enabled !!! > /dev/console\n";
	}
	else if (query("/lan/dhcp/server/enable") != "1")
	{
		echo "echo DHCP server is disabled! > /dev/console\n";
	}
	else
	{
		$ipaddr=query("/lan/ethernet/ip");
		$netmask=query("/lan/ethernet/netmask");
		anchor("/lan/dhcp/server/pool:1");
		$ipstr=query("startip");
		$ipend=query("endip");
		$dmain=query("domain");
		$wins0=query("primarywins");
		$wins1=query("secondarywins");
		$ltime=query("leasetime");
		if ($ltime == "") { $ltime="8640"; }
		if ($dmain == "") { $dmain=query("/runtime/wan/inf:1/domain"); }

		fwrite( $dhcpd_conf, "start ".		$ipstr."\n");
		fwrite2($dhcpd_conf, "end ".		$ipend."\n");
		fwrite2($dhcpd_conf, "interface ".	$dhcpd_if."\n");
		fwrite2($dhcpd_conf, "lease_file ".	$dhcpd_lease."\n");
		fwrite2($dhcpd_conf, "pidfile ".	$dhcpd_pidf."\n");
		fwrite2($dhcpd_conf, "auto_time ".	"10\n");
		fwrite2($dhcpd_conf, "opt lease ".	$ltime."\n");

		if ($dmain!="")	{ fwrite2($dhcpd_conf, "opt domain ".	$dmain."\n"); }
		if ($netmask!="")	{ fwrite2($dhcpd_conf, "opt subnet ".$netmask."\n"); }
		if ($ipaddr!="")	{ fwrite2($dhcpd_conf, "opt router ".$ipaddr."\n"); }
		if ($wins0!="")		{ fwrite2($dhcpd_conf, "opt wins ".$wins0."\n"); }
		if ($wins1!="")		{ fwrite2($dhcpd_conf, "opt wins ".$wins1."\n"); }

		if (query("/dnsrelay/mode") != "1")
		{
			fwrite2($dhcpd_conf, "opt dns ".$ipaddr."\n");
		}
		else
		{
			$dns = query("/runtime/wan/inf:1/primarydns");
			if ($dns != "")	{ fwrite2($dhcpd_conf, "opt dns ".$dns."\n"); }
			$dns=query("/runtime/wan/inf:1/secondarydns");
			if ($dns != "")	{ fwrite2($dhcpd_conf, "opt dns ".$dns."\n"); }
		}

		if (query("staticdhcp/enable") == 1)
		{
			for ("/lan/dhcp/server/pool:1/staticdhcp/entry")
			{
				if (query("enable") == 1)
				{
					$ip=query("ip");
					$mac=query("mac");
					fwrite2($dhcpd_conf, "static ".$ip." ".$mac."\n");
				}
			}
		}
		
		echo "udhcpd ".$dhcpd_conf." &\n";
		echo "dhcpxmlpatch -f ".$dhcpd_lease." &\n";
		echo "echo $! > ".$dhcpd_patch."\n";
	}
}
else
{
	echo "echo \"Stop DHCP server (".$dhcpd_if.") ...\" > /dev/console\n";

	if (query("/runtime/router/enable")!="1")
	{
		echo "echo DHCP server is not enabled ! > /dev/console\n";
	}
	else
	{
		echo "if [ -f ".$dhcpd_patch." ]; then\n";
		echo "	pid=`cat ".$dhcpd_patch."`\n";
		echo "	if [ $pid != 0 ]; then\n";
		echo "		kill $pid > /dev/console 2>&1\n";
		echo "	fi\n";
		echo "	rm -f ".$dhcpd_patch."\n";
		echo "fi\n";

		echo "if [ -f ".$dhcpd_pidf." ]; then\n";
		echo "	pid=`cat ".$dhcpd_pidf."`\n";
		echo "	if [ $pid != 0 ]; then\n";
		echo "		kill $pid > /dev/console 2>&1\n";
		echo "	fi\n";
		echo "	rm -f ".$dhcpd_pidf."\n";
		echo "fi\n";
	}
}
?>
