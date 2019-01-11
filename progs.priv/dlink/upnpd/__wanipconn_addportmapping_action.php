<?
/* vi: set sw=4 ts=4: */
if ($NewExternalPort != "" && $NewProtocol != "" && $NewInternalPort != "" && $NewInternalClient != "")
{
	$count = 0;
	for ("/runtime/upnp/wan:".$WID."/entry")
	{
		if ($NewProtocol == "TCP")  { $proto=1; }
		else                        { $proto=2; }
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

	if  ($NewProtocol == "TCP") { set("protocol", 1); $proto=" -p tcp"; }
	else                        { set("protocol", 2); $proto=" -p udp"; }

	set("description", $NewPortMappingDescription);

	if ($NewRemoteHost != "")   { $sourceip = " -s ".$NewRemoteHost; }
	else                        { $sourceip = ""; }

	if ($NewEnabled == "1")
	{
		$wanip = query("/runtime/wan/inf:".$WID."/ip");
		if ($wanip != "")
		{
			set("wanip", $wanip);
			fwrite2($shell, "echo iptables -t nat -A PRE_UPNP".$sourceip.$proto." -d ".$wanip.
				" --dport ".$NewExternalPort." -j DNAT --to ".$NewInternalClient.":".$NewInternalPort." > /dev/console\n");
			fwrite2($shell, "iptables -t nat -A PRE_UPNP".$sourceip.$proto." -d ".$wanip.
				" --dport ".$NewExternalPort." -j DNAT --to ".$NewInternalClient.":".$NewInternalPort."\n");
		}
	}
	/* kick upnpd to send notify */
	//fwrite2($shell, "killall -16 upnpd\n");
}
else
{
	echo "ErrCode:402 ";
}
?>
