HTTP/1.1 200 OK
Content-Type: text/xml; charset=utf-8

<?
echo "\<\?xml version='1.0' encoding='utf-8'\?\>";
$nodebase="/runtime/hnap/SetWanSettings/";
$mac="";
$rlt="REBOOT";
$Type=query($nodebase."Type");
$MacAddress=query($nodebase."MacAddress");
$IPAddress=query($nodebase."IPAddress");
$SubnetMask=query($nodebase."SubnetMask");
$Gateway=query($nodebase."Gateway");
$MTU=query($nodebase."MTU");
$Username=query($nodebase."Username");
$Password=query($nodebase."Password");
$MaxIdleTime=query($nodebase."MaxIdleTime");
$ServiceName=query($nodebase."ServiceName");
$AutoReconnect=query($nodebase."AutoReconnect");
set("/dnsrelay/server/primaryDns", query($nodebase."DNS/Primary"));
set("/dnsrelay/server/secondaryDns", query($nodebase."DNS/Secondary"));

if($Type == "Static")
{
	$mode=1;
	anchor("/wan/rg/inf:1/static");
	set("cloneMac", $MacAddress);
	set("ip", $IPAddress);
	set("netmask", $SubnetMask);
	set("gateway", $Gateway);
	set("mtu", $MTU);
}
else if($Type == "DHCP")
{
	$mode=2;
	anchor("/wan/rg/inf:1/dhcp");
	set("cloneMac", $MacAddress);
	set("mtu", $MTU);
}
else if($Type == "StaticPPPoE" || $Type == "DHCPPPPoE")
{
	$mode=3;
	anchor("/wan/rg/inf:1/pppoe");
	if($Type == "StaticPPPoE")
	{
		set("mode", 1);
		set("staticip", $IPAddress);
	}
	else
	{
		set("mode", 2);
	}

	set("user", $Username);
	set("password", $Password);
	set("idleTimeout", $MaxIdleTime);
	set("acService", $ServiceName);
	if($MaxIdleTime>0)
	{
		set("ondemand", 1);
	}
	else
	{
		set("ondemand", 0);
	}
	if($AutoReconnect=="true")
	{
		set("autoReconnect", 1);
	}
	else if($AutoReconnect=="false")
	{
		set("autoReconnect", 0);
	}
	set("cloneMac", $MacAddress);
	set("mtu", $MTU);
}
else if($Type == "StaticPPTP" || $Type == "DynamicPPTP")
{
	$mode=4;
	anchor("/wan/rg/inf:1/pptp");
	if($Type == "StaticPPTP")
	{
		set("ip", $IPAddress);
		set("netmask", $SubnetMask);
	}
	else
	{
		$rlt="ERROR";
	}

	set("user", $Username);
	set("password", $Password);
	set("idleTimeout", $MaxIdleTime);
	set("serverip", $ServiceName);
	if($MaxIdleTime>0)
	{
		set("ondemand", 1);
	}
	else
	{
		set("ondemand", 0);
	}
	if($AutoReconnect=="true")
	{
		set("autoReconnect", 1);
	}
	else if($AutoReconnect=="false")
	{
		set("autoReconnect", 0);
	}
//	set("cloneMac", $MacAddress);
	set("mtu", $MTU);
}
else if($Type == "StaticL2TP" || $Type == "StaticL2TP")
{
	$mode=5;
	anchor("/wan/rg/inf:1/l2tp");
	if($Type == "StaticL2TP")
	{
		set("mode", 1);
		set("staticip", $IPAddress);
		set("staticnetmask", $SubnetMask);
	}
	else
	{
		set("mode", 2);
	}

	set("serverip", $ServiceName);
	set("user", $Username);
	set("password", $Password);
	set("idleTimeout", $MaxIdleTime);
	if($MaxIdleTime>0)
	{
		set("ondemand", 1);
	}
	else
	{
		set("ondemand", 0);
	}
	if($AutoReconnect=="true")
	{
		set("autoReconnect", 1);
	}
	else if($AutoReconnect=="false")
	{
		set("autoReconnect", 0);
	}
//	set("cloneMac", $MacAddress);
	set("mtu", $MTU);
}
else if($Type == "BigPond")
{
	$mode=7;
	anchor("/wan/rg/inf:1/bigpond");
	set("username", $Username);
	set("password", $Password);
	set("server", $ServiceName);
	if($AutoReconnect=="true")
	{
		set("autoReconnect", 1);
	}
	else if($AutoReconnect=="false")
	{
		set("autoReconnect", 0);
	}
//	set("cloneMac", $MacAddress);
//	set("mtu", $MTU);
}

set("/wan/rg/inf:1/mode", $mode);

if($rlt=="REBOOT")
{
	fwrite($ShellPath, "#!/bin/sh\n");
	fwrite2($ShellPath, "echo \"[$0]-->Wan Change\" > /dev/console\n");
	fwrite2($ShellPath, "/etc/scripts/misc/profile.sh put > /dev/console\n");
	fwrite2($ShellPath, "/etc/templates/wan.sh restart > /dev/console \n");
	fwrite2($ShellPath, "rgdb -i -s /runtime/hnap/dev_status '' > /dev/console");
	set("/runtime/hnap/dev_status", "ERROR");
}
?>
<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
<soap:Body>
<SetWanSettingsResponse xmlns="http://purenetworks.com/HNAP1/">
<SetWanSettingsResult><?=$rlt?></SetWanSettingsResult>
</SetWanSettingsResponse>
</soap:Body>
</soap:Envelope>
