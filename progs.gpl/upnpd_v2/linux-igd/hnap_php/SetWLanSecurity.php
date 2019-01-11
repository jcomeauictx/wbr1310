HTTP/1.1 200 OK
Content-Type: text/xml; charset=utf-8

<?
echo "\<\?xml version='1.0' encoding='utf-8'\?\>";
$nodebase="/runtime/hnap/SetWLanSecurity/";
$Type=query($nodebase."Type");
$Key=query($nodebase."Key");
$Enabled=query($nodebase."Enabled");
$WEPKeyBits=query($nodebase."WEPKeyBits");

anchor("/wireless");
if($Type=="WPA")
{
	set("authentication", 2);
	anchor("/wireless");
	//----- do not check empty value, because there maybe really empty value
	set("wpa/radiusServer", query($nodebase."RadiusIP1"));
	set("wpa/radiusPort", query($nodebase."RadiusPort1"));
	set("wpa/radiusServer2", query($nodebase."RadiusIP2"));
	set("wpa/radiusPort2", query($nodebase."RadiusPort2"));
	set("wpa/radiusSecret", $Key);
}
else if($Type=="WEP")
{
	set("authentication", 0);
	if($Enabled=="true")
	{
		set("wpa/wepMode", 1);
	}
	else if($Enabled=="false")
	{
		set("wpa/wepMode", 0);
	}

	if($WEPKeyBits!="")
	{
		set("keyLength", $WEPKeyBits);
	}

	$id=query("defkey");
	set("keyFormat", 1);
	set("wepkey:entry".$id, $Key);
}

fwrite($ShellPath, "#!/bin/sh\n");
fwrite2($ShellPath, "echo \"[$0]-->WLan Change\" > /dev/console\n");
fwrite2($ShellPath, "/etc/scripts/misc/profile.sh put > /dev/console\n");
fwrite2($ShellPath, "/etc/templates/wlan.sh restart > /dev/console \n");
fwrite2($ShellPath, "rgdb -i -s /runtime/hnap/dev_status '' > /dev/console");
set("/runtime/hnap/dev_status", "ERROR");
?>
<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <SetWLanSecurityResponse xmlns="http://purenetworks.com/HNAP1/">
      <SetWLanSecurityResult>REBOOT</SetWLanSecurityResult>
    </SetWLanSecurityResponse>
  </soap:Body>
</soap:Envelope>
