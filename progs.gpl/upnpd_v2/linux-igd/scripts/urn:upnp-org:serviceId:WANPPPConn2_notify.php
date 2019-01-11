<e:property><PossibleConnectionTypes>IP_Routed</PossibleConnectionTypes></e:property>
<e:property><ConnectionStatus><?map("/runtime/wan/inf:2/connectstatus","connected","Connected","*","Disconnected");?></ConnectionStatus></e:property>
<e:property><ExternalIPAddress><?query("/runtime/wan/inf:2/ip");?></ExternalIPAddress></e:property>
<e:property><PortMappingNumberOfEntries>0</PortMappingNumberOfEntries></e:property>
<e:property><X_Name><?
$pid = query("/wan/rg/inf:2/profileid");
echo query("/wan/rg/profile:".$pid."/name");
?></X_Name></e:property>
