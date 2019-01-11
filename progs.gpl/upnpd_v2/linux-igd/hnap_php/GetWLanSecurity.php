HTTP/1.1 200 OK
Content-Type: text/xml; charset=utf-8

<?
echo "\<\?xml version='1.0' encoding='utf-8'\?\>";
anchor("/wireless");
$auth=query("authentication");
$key="";
$enabled="false";
if(query("wpa/wepMode")=="1")
{
	$enabled="true";
}

if($auth>1)
{
	$auth="WPA";
	$enabled="true";	//-----Always true if WPA
	$key=query("wpa/radiusSecret");
}
else
{
	$auth="WEP";
	$id=query("defkey");
	$key=query("wepkey:entry".$id);
}
?>
<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <GetWLanSecurityResponse xmlns="http://purenetworks.com/HNAP1/">
      <GetWLanSecurityResult>OK</GetWLanSecurityResult>
      <Enabled><?=$enabled?></Enabled>
      <Type><?=$auth?></Type>
      <WEPKeyBits><?query("keyLength");?></WEPKeyBits>
      <SupportedWEPKeyBits>
        <int>64</int>
        <int>128</int>
      </SupportedWEPKeyBits>
      <Key><?=$key?></Key>
      <RadiusIP1><?query("wpa/radiusServer");?></RadiusIP1>
      <RadiusPort1><?map("wpa/radiusport", "", "0");?></RadiusPort1>
      <RadiusIP2><?query("wpa/radiusServer2");?></RadiusIP2>
      <RadiusPort2><?map("wpa/radiusport2", "", "0");?></RadiusPort2>
    </GetWLanSecurityResponse>
  </soap:Body>
</soap:Envelope>
