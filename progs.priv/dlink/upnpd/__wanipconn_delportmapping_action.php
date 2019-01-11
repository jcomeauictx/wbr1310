<?
/* vi: set sw=4 ts=4: */
$target = 0;
$count = 0;
for ("/runtime/upnp/wan:".$WID."/entry")
{
	if ($NewProtocol == "TCP")  { $proto=1; }
	else                        { $proto=2; }
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
		if (query("remoteip") != "")    { $sourceip = " -s ".query("remoteip"); }
		else                            { $sourceip = ""; }
		if (query("protocol") == 1)     { $proto = " -p tcp"; }
		else                            { $proto = " -p udp"; }
		$extport = query("port2");
		$intport = query("port1");
		$intclnt = query("ip");

		$wanip = query("wanip");
		if ($wanip != "")
		{
			fwrite2($shell, "echo iptables -t nat -D PRE_UPNP".$sourceip.$proto." -d ".$wanip.
					" --dport ".$extport." -j DNAT --to ".$intclnt.":".$intport." > /dev/console\n");
			fwrite2($shell, "iptables -t nat -D PRE_UPNP".$sourceip.$proto." -d ".$wanip.
					" --dport ".$extport." -j DNAT --to ".$intclnt.":".$intport."\n");
		}
	}
	del("/runtime/upnp/wan:".$WID."/entry:".$target);
	/* kick upnpd to send notify */
	//fwrite2($shell, "killall -16 upnpd\n");
}
?>
