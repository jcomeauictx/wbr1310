<?
/* vi: set sw=4 ts=4: */
$target = 0;
for ("/runtime/upnp/wan:".$WID."/entry")
{
	if ($NewProtocol == "TCP")  { $proto=1; }
	else                        { $proto=2; }
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
	echo "ErrCode:714 ";
}
?>
