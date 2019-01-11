<?
/* vi: set sw=4 ts=4: */
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
	echo "ErrCode:713  ";
}
?>
