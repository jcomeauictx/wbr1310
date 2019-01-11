<? $WID = 1;  ?>
<e:property><PossibleConnectionTypes>IP_Routed</PossibleConnectionTypes></e:property>
<e:property><ConnectionStatus><?
	map("/runtime/wan/inf:".$WID."/connectstatus","connected","Connected","*","Disconnected");
?></ConnectionStatus></e:property>
<e:property><ExternalIPAddress><?query("/runtime/wan/inf:".$WID."/ip");?></ExternalIPAddress></e:property>
<e:property><PortMappingNumberOfEntries><?
$count = 0;
for ("/runtime/upnp/wan:".$WID."/entry") { $count++; }
echo $count;
?></PortMappingNumberOfEntries></e:property>
<e:property><X_Name><?
$wanmode = query("/wan/rg/inf:".$WID."/mode");
if ($wanmode == 6)
{
	$pid = query("/wan/rg/inf:".$WID."/profileid");
	echo query("/wan/rg/profile:".$pid."/name");
}
else 
{
	echo "WAN Connection";
}
?></X_Name></e:property>
