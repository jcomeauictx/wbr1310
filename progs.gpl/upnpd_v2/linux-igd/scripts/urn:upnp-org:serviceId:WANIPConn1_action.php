<?
/* vi: set sw=4 ts=4: */

$WID = 1;
fwrite($shell, "#!/bin/sh\n");

if ($action == "AddPortMapping")
{
	if ($NewExternalPort != "" && $NewProtocol != "" && $NewInternalPort != "" && $NewInternalClient != "")
	{
		$count = 0;
		for ("/runtime/upnp/wan:".$WID."/entry")
		{
			if ($NewProtocol == "TCP")	{ $proto=1; }
			else						{ $proto=2; }
			if ($NewRemoteHost == query("remoteip") &&
				$NewExternalPort == query("port2") &&
				$proto == query("protocol"))
			{
				echo "ErrCode:402 ";
				exit;
			}
			$count++;
		}
		$count++;
		set("/runtime/upnp/wan:".$WID."/entry:".$count."/enable", $NewEnabled);
		anchor("/runtime/upnp/wan:".$WID."/entry:".$count);
		set("remoteip", $NewRemoteHost);
		set("ip", $NewInternalClient);
		set("port1", $NewInternalPort);
		set("port2", $NewExternalPort);

		if	($NewProtocol == "TCP")	{ set("protocol", 1); $proto=" -p tcp"; }
		else						{ set("protocol", 2); $proto=" -p udp"; }

		set("description", $NewPortMappingDescription);

		if ($NewRemoteHost != "")	{ $sourceip = " -s ".$NewRemoteHost; }
		else						{ $sourceip = ""; }

		if ($NewEnabled == "1")
		{
			$wanip = query("/runtime/wan/inf:".$WID."/ip");
			if ($wanip != "")
			{
				fwrite2($shell, "echo iptables -t nat -A PRE_UPNP".$sourceip.$proto." -d ".$wanip.
					" --dport ".$NewExternalPort." -j DNAT --to ".$NewInternalClient.":".$NewInternalPort." > /dev/console\n");
				fwrite2($shell, "iptables -t nat -A PRE_UPNP".$sourceip.$proto." -d ".$wanip.
					" --dport ".$NewExternalPort." -j DNAT --to ".$NewInternalClient.":".$NewInternalPort."\n");
			}
		}
	}
	else
	{
		echo "ErrCode:402 ";
	}
}
else if ($action == "DeletePortMapping")
{
	$target = 0;
	$count = 0;
	for ("/runtime/upnp/wan:".$WID."/entry")
	{
		if ($NewProtocol == "TCP")	{ $proto=1; }
		else						{ $proto=2; }
		if ($NewRemoteHost == query("remoteip") &&
			$NewExternalPort == query("port2") &&
			$proto == query("protocol"))
		{
			$target = $@;
		}
	}
	if ($target != 0)
	{
		anchor("/runtime/upnp/wan:".$WID."/entry:".$target);
		if (query("enable")==1)
		{
			if (query("remoteip") != "")	{ $sourceip = " -s ".query("remoteip"); }
			else							{ $sourceip = ""; }
			if (query("protocol") == 1)		{ $proto = " -p tcp"; }
			else							{ $proto = " -p udp"; }
			$extport = query("port2");
			$intport = query("port1");
			$intclnt = query("ip");

			$wanip = query("/runtime/wan/inf:".$WID."/ip");
			fwrite2($shell, "echo iptables -t nat -D PRE_UPNP".$sourceip.$proto." -d ".$wanip.
				" --dport ".$extport." -j DNAT --to ".$intclnt.":".$intport." > /dev/console\n");
			fwrite2($shell, "iptables -t nat -D PRE_UPNP".$sourceip.$proto." -d ".$wanip.
				" --dport ".$extport." -j DNAT --to ".$intclnt.":".$intport."\n");
		}
		del("/runtime/upnp/wan:".$WID."/entry:".$target);
	}
}
else if ($action == "ForceTermination")
{
	echo "ErrCode:200   ";
}
else if ($action == "GetConnectionTypeInfo")
{
	echo "\n<NewConnectionType>IP_Routed</NewConnectionType>";
	echo "\n<NewPossibleConnectionTypes>IP_Routed</NewPossibleConnectionTypes>";
}
else if ($action == "GetExternalIPAddress")
{
	echo "\n<NewExternalIPAddress>".query("/runtime/wan/inf:".$WID."/ip")."</NewExternalIPAddress>";
}
else if ($action == "GetGenericPortMappingEntry")
{
	$count = 0;
	for ("/runtime/upnp/wan:".$WID."/entry") { $count++; }
	if ($NewPortMappingIndex < $count)
	{
		$index = $NewPortMappingIndex + 1;
		anchor("/runtime/upnp/wan:".$WID."/entry:".$index);
		echo "\n<NewRemoteHost>".query("remoteip")."</NewRemoteHost>";
		echo "\n<NewExternalPort>".query("port2")."</NewExternalPort>";
		echo "\n<NewProtocol>"; map("protocol","1","TCP","*","UDP"); echo "</NewProtocol>";
		echo "\n<NewInternalPort>".query("port1")."</NewInternalPort>";
		echo "\n<NewInternalClient>".query("ip")."</NewInternalClient>";
		echo "\n<NewEnabled>".query("enable")."</NewEnabled>";
		echo "\n<NewPortMappingDescription>".query("description")."</NewPortMappingDescription>";
		echo "\n<NewLeaseDuration>0</NewLeaseDuration>";
	}
	else
	{
		echo "ErrCode:713    ";
	}
}
else if ($action == "GetNATRSIPStatus")
{
	echo "\n<NewRSIPAvailable>0</NewRSIPAvailable>";
	echo "\n<NewNATEnabled>1</NewNATEnabled>";
}
else if ($action == "GetSpecificPortMappingEntry")
{
	$target = 0;
	for ("/runtime/upnp/wan:".$WID."/entry")
	{
		if ($NewProtocol == "TCP")	{ $proto=1; }
		else						{ $proto=2; }
		if (query("remoteip") == $NewRemoteHost &&
			query("port2") == $NewExternalPort &&
			query("protocol") == $proto)
		{
			$target = $@;
		}
	}
	if ($target > 0)
	{
		anchor("/runtime/upnp/wan:".$WID."/entry:".$target);
		echo "\n<NewInternalPort>".query("port1")."</NewInternalPort>";
		echo "\n<NewInternalClient>".query("ip")."</NewInternalClient>";
		echo "\n<NewEnabled>".query("enable")."</NewEnabled>";
		echo "\n<NewPortMappingDescription>".query("description")."</NewPortMappingDescription>";
		echo "\n<NewLeaseDuration>0</NewLeaseDuration>";
	}
	else
	{
		echo "ErrCode:714   ";
	}
}
else if ($action == "GetStatusInfo")
{
	echo "\n<NewConnectionStatus>";
	map("/runtime/wan/inf:".$WID."/connectstatus","connected","Connected","*","Disconnected");
	echo "</NewConnectionStatus>";
	echo "\n<NewLastConnectionError>ERROR_NONE</NewLastConnectionError>";
	
	$v1 = query("/runtime/sys/uptime");
	$v2 = query("/runtime/wan/inf:".$WID."/uptime");
	$uptime = $v1 - $v2;
	echo "\n<NewUptime>".$uptime."</NewUptime>";
}
else if ($action == "RequestConnection")
{
	echo "\n";
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
