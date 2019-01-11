<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

fwrite($shell, "#!/bin/sh\n");
fwrite2($shell, "echo [$0] WANIPConn".$WID.":".$action." ... > /dev/console\n");

$RouterOn = query("/runtime/router/enable");

if ($action == "AddPortMapping")
{
	if ($RouterOn==1) { require($template_root."/upnpd/__wanipconn_addportmapping_action.php"); }
	else { echo "ErrCode:501 "; }
}
else if	($action == "DeletePortMapping")
{
	if ($RouterOn==1) { require($template_root."/upnpd/__wanipconn_delportmapping_action.php"); }
	else { echo "ErrCode:501 "; }
}
else if ($action == "GetConnectionTypeInfo")
{
	if ($RouterOn==1)	{ $ConnectionType="IP_Routed"; }
	else				{ $ConnectionType="IP_Bridged"; }
	echo "\n<NewConnectionType>".$ConnectionType."</NewConnectionType>";
	echo "\n<NewPossibleConnectionTypes>".$ConnectionType."</NewPossibleConnectionTypes>";
}
else if ($action == "GetExternalIPAddress")
{
	if ($RouterOn==1)	{ $ipaddr = query("/runtime/wan/inf:".$WID."/ip"); }
	else				{ $ipaddr = query("/lan/ethernet/ip"); }
	echo "\n<NewExternalIPAddress>".$ipaddr."</NewExternalIPAddress>";
}
else if ($action == "GetGenericPortMappingEntry")
{
	if ($RouterOn==1) { require($template_root."/upnpd/__wanipconn_getgenportmapping_action.php"); }
	else { echo "ErrCode:501 "; }
}
else if ($action == "GetNATRSIPStatus")
{
	echo "\n<NewRSIPAvailable>0</NewRSIPAvailable>";
	echo "\n<NewNATEnabled>".$RouterOn."</NewNATEnabled>";
}
else if ($action == "GetSpecificPortMappingEntry")
{
	if ($RouterOn==1) { require($template_root."/upnpd/__wanipconn_getspecportmapping_action.php"); }
	else { echo "ErrCode:501 "; }
}
else if ($action == "GetStatusInfo")
{
	echo "\n<NewConnectionStatus>";
	if ($RouterOn==1) { map("/runtime/wan/inf:".$WID."/connectstatus","connected","Connected","*","Disconnected"); }
	else { echo "Connected"; }
	echo "</NewConnectionStatus>";
	echo "\n<NewLastConnectionError>ERROR_NONE</NewLastConnectionError>";
	
	$v1 = query("/runtime/sys/uptime");
	$v2 = query("/runtime/wan/inf:".$WID."/uptime");
	$uptime = $v1 - $v2;
	echo "\n<NewUptime>".$uptime."</NewUptime>";
}
else if ($action == "RequestConnection")
{
	if ($RouterOn==1)
	{
		if (query("/upnp/allow_disconnect_wan")==0) { echo "ErrCode:200 "; }
		else
		{
			fwrite2($shell, "echo UPNP connecting WAN".$WID." ... > /dev/console\n");
			fwrite2($shell, "rgdb -s /runtime/wan/inf:".$WID."/connect 1\n");
		}
	}
	else
	{
		echo "ErrCode:501 ";
	}
}
else if ($action == "ForceTermination")
{
	if ($RouterOn==1)
	{
		if (query("/upnp/allow_disconnect_wan")==0)
		{
			echo "ErrCode:200  ";
		}
		else
		{
			fwrite2($shell, "echo UPNP disconnecting WAN".$WID." ... > /dev/console\n");
			fwrite2($shell, "rgdb -s /runtime/wan/inf:".$WID."/disconnect 1");
		}
	}
	else
	{
		echo "ErrCode:501 ";
	}
}
else if ($action == "SetConnectionType")
{
	echo "ErrCode:501     ";
}
else
{
	echo "ErrCode:401     ";
}
?>
