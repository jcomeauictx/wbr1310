#!/bin/sh
echo [$0] ... > /dev/console
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$lanif=query("/runtime/layout/lanif");

if ($generate_start==1)
{
	if (query("/runtime/router/enable")!=1)
	{
		echo "echo Bridge mode selected, LAN is disabled ! > /dev/console\n";
	}
	else
	{
		$lanmac=query("/runtime/layout/lanmac");
		set("/runtime/sys/info/lanmac", $lanmac);

		anchor("/lan/ethernet");
		$ipaddr = query("ip");
		$subnet = query("netmask");
		echo "echo \"Start LAN (".$lanif."/".$ipaddr."/".$subnet.")...\" > /dev/console\n";
		echo "ifconfig ".$lanif." ".$ipaddr;
		if ($subnet != "") { echo " netmask ".$subnet; }
		echo "\n";

		$dhcpd_if=$lanif;
		$dhcpd_clearleases=1;
		require($template_root."/dhcp/dhcpd.php");
	}
}
else
{
	if (query("/runtime/router/enable")!=1)
	{
		echo "echo Bridge mode selected, LAN is disabled ! > /dev/console\n";
	}
	else
	{
		echo "echo \"Stop LAN (".$lanif.")...\" > /dev/console\n";
		$dhcpd_if=$lanif;
		$dhcpd_clearleases=0;
		require($template_root."/dhcp/dhcpd.php");
		echo "ifconfig ".$lanif." 0.0.0.0 > /dev/console 2>&1\n";
	}
}
?>
