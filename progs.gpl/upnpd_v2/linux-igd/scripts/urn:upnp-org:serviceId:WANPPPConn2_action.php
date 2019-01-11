<?
/* vi: set sw=4 ts=4: */
if ($action == "AddPortMapping")
{
	/* <NewRemoteHost/><NewWxternalPort/><NewProtocol/><NewinternalClient/><NewEnabled/><NewPortMappingDescription/><NewLeaseDuration/> */
	echo "\n";
}
else if ($action == "DeletePortMapping")
{
	/* <NewRemoteHost/><NewExternalPort/><NewProtocol/> */
	echo "\n";
}
else if ($action == "ForceTermination")
{
	echo "\n";
}
else if ($action == "GetConnectionTypeInfo")
{
	echo "\n<NewConnectionType>IP_Routed</NewConnectionType>";
	echo "\n<NewPossibleConnectionTypes>IP_Routed</NewPossibleConnectionTypes>";
}
else if ($action == "GetExternalIPAddress")
{
	echo "\n<NewExternalIPAddress>".query("/runtime/wan/inf:1/ip")."</NewExternalIPAddress>";
}
else if ($action == "GetGenericPortMappingEntry")
{
//	echo "\n<NewPortMappingIndex>1</NewPortMappingIndex>";
	echo "\n<NewRemoteHost>host</NewRemoteHost>";
	echo "\n<NewExternalPort>6000</NewExternalPort>";
	echo "\n<NewProtocol>TCP</NewProtocol>";
	echo "\n<NewInternalPort>3000</NewInternalPort>";
	echo "\n<NewInternalClient>client</NewInternalClient>";
	echo "\n<NewEnabled>1</NewEnabled>";
	echo "\n<NewPortMappingDescription>PortMapping</NewPortMappingDescription>";
	echo "\n<NewLeaseDuration>600</NewLeaseDuration>";
}
else if ($action == "GetNATRSIPStatus")
{
	echo "\n<NewRSIPAvailable>0</NewRSIPAvailable>";
	echo "\n<NewNATEnabled>1</NewNATEnabled>";
}
else if ($action == "GetSpecificPortMappingEntry")
{
	echo "\n<NewRemoteHost>remote host</NewRemoteHost>";
	echo "\n<NewExternalPort>6000</NewExternalPort>";
	echo "\n<NewProtocol>TCP</NewProtocol>";
	echo "\n<NewInternalPort>3000</NewInternalPort>";
	echo "\n<NewInternalClient>interal client</NewInternalClient>";
	echo "\n<NewEnabled>1</NewEnabled>";
	echo "\n<NewPortMappingDescription>port mapping</NewPortMappingDescription>";
	echo "\n<NewLeaseDuration>600</NewLeaseDuration>";
}
else if ($action == "GetStatusInfo")
{
	echo "\n<NewConnectionStatus>Up</NewConnectionStatus>";
	echo "\n<NewLastConnectionError>N/A</NewLastConnectionError>";
	echo "\n<NewUptime>100</NewUptime>";
}
else if ($action == "RequestConnection")
{
	echo "\n";
}
else if ($action == "SetConnectionType")
{
	echo "\n<NewConnectionType>100Full</NewConnectionType>";
}
else
{
	echo "401";
}
?>
