HTTP/1.1 200 OK
Content-Type: text/xml; charset=utf-8

<?
echo "\<\?xml version='1.0' encoding='utf-8'\?\>";
anchor("/runtime/wan/inf:1");
$Type="";
$Username="";
$Password="";
$MaxIdletime=0;
$ServiceName="";
$AutoReconnect="false";
if(query("connectstatus")=="connected")
{
	$ipaddr=query("ip");
	$gateway=query("gateway");
	$mask=query("netmask");
	$dns1=query("primarydns");
	$dns2=query("secondaryDns");
}
$MTU=1500;
$mac=query("mac");
if($ipaddr=="0.0.0.0")
{
	$ipaddr="";
}
if($mask=="0.0.0.0")
{
	$mask="";
}
if($gateway=="0.0.0.0")
{
	$gateway="";
}
if($dns1=="0.0.0.0")
{
	$dns1="";
}
if($dns2=="0.0.0.0")
{
	$dns2="";
}

$mode=query("/wan/rg/inf:1/mode");
if($mode < 3)
{
	anchor("/wan/rg/inf:1");
	if($mode == 1)
	{
		$Type="Static";
		$ipaddr=query("static/ip");
		$gateway=query("static/gateway");
		$mask=query("static/netmask");
		$MTU=query("static/mtu");
		$dns1=query("/dnsrelay/server/primarydns");
		$dns2=query("/dnsrelay/server/secondarydns");
		if(query("static/clonemac")!="")
		{
			$mac=query("static/clonemac");
		}
		$mac=query("static/clonemac");
	}
	else if($mode == 2)
	{
		$Type="DHCP";
		$MTU=query("dhcp/mtu");
		if(query("dhcp/clonemac")!="")
		{
			$mac=query("dhcp/clonemac");
		}
	}
}
else if($mode == 3)
{
	anchor("/wan/rg/inf:1/pppoe");
	if(query("mode") == 1)
	{
		$Type="StaticPPPoE";
		$ipaddr=query("staticip");
		$dns1=query("/dnsrelay/server/primarydns");
		$dns2=query("/dnsrelay/server/secondarydns");
	}
	else
	{
		$Type="DHCPPPPoE";
	}
	$Username=query("user");
	$Password=query("password");
	$MaxIdletime=query("idleTimeout");
	$ServiceName=query("acService");
	if(query("clonemac")!="")
	{
		$mac=query("clonemac");
	}
	if(query("autoReconnect") == 1)
	{
		$AutoReconnect="true";
	}
	$MTU=query("mtu");
}
else if($mode == 4)	//-----PPTP
{
	anchor("/wan/rg/inf:1/pptp");

//	DI only support StaticPPTP
//	if(query("mode") == 1)
//	{
		$Type="StaticPPTP";
		$ipaddr=query("ip");
//		$gateway=query("staticGW");
		$mask=query("netmask");
		$dns1=query("/dnsrelay/server/primarydns");
		$dns2=query("/dnsrelay/server/secondarydns");
/*	}
	else
	{
	}
*/
	$Username=query("user");
	$Password=query("password");
	$MaxIdletime=query("idleTimeout");
	$ServiceName=query("serverip");
/*	if(query("clonemac")!="")
	{
		$mac=query("clonemac");
	}*/
	if(query("autoReconnect") == 1)
	{
		$AutoReconnect="true";
	}
	$MTU=query("mtu");
}
else if($mode == 5)	//-----L2TP
{
	anchor("/wan/rg/inf:1/l2tp");
	if(query("mode") == 1)
	{
		$Type="StaticL2TP";
		$ipaddr=query("staticip");		
		$gateway=query("staticGW");
		$mask=query("staticnetmask");
		$dns1=query("/dnsrelay/server/primarydns");
		$dns2=query("/dnsrelay/server/secondarydns");
	}
	else
	{
		$Type="DynamicL2TP";
	}
	$Username=query("user");
	$Password=query("password");
	$MaxIdletime=query("idleTimeout");
	$ServiceName=query("serverip");
/*	if(query("clonemac")!="")
	{
		$mac=query("clonemac");
	}*/
	if(query("autoReconnect") == 1)
	{
		$AutoReconnect="true";
	}
	$MTU=query("mtu");
}
else if($mode == 7)	//-----BigPond
{
	anchor("/wan/rg/inf:1/bigpond");
	$Username=query("username");
	$Password=query("password");
	$ServiceName=query("serverip");
	if(query("autoReconnect") == 1)
	{
		$AutoReconnect="true";
	}
}
?>
<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
<soap:Body>
<GetWanSettingsResponse xmlns="http://purenetworks.com/HNAP1/">
<GetWanSettingsResult>OK</GetWanSettingsResult>	
<Type><?=$Type?></Type>
<Username><?=$Username?></Username>
<Password><?=$Password?></Password>
<MaxIdleTime><?=$MaxIdletime?></MaxIdleTime>
<ServiceName><?=$ServiceName?></ServiceName>
<AutoReconnect><?=$AutoReconnect?></AutoReconnect>
<IPAddress><?=$ipaddr?></IPAddress>
<SubnetMask><?=$mask?></SubnetMask>
<Gateway><?=$gateway?></Gateway>
<DNS>
<Primary><?=$dns1?></Primary>
<Secondary><?=$dns2?></Secondary>
</DNS>
<MacAddress><?=$mac?></MacAddress>
<MTU><?=$MTU?></MTU>
</GetWanSettingsResponse>
</soap:Body></soap:Envelope>
