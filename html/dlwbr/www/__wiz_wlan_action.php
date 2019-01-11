<?
echo "<!--\n";
echo "$ACTION_POST=".$ACTION_POST."\n";
if($ACTION_POST=="1_set_ssid")
{
	set($G_WIZ_PREFIX_WLAN."/wireless/ssid",$ssid);
}
else if($ACTION_POST=="2_sel_sec")
{
	anchor($G_WIZ_PREFIX_WLAN."/wireless/");
	/*authentication : 0:open system, 1:share key, 2: WPA, 3: WPA-PSK, 4: WPA2, 5: WPA2-PSK, 6: WPA-AUTO, 7: WPA-AUTO-PSK,
			   8:802.1X
	  wepmode: 0:disable, 1:wep, 2:tkip, 3: aes, 4: tkip+aes
	*/
	if($sec_type==5)	{set("authentication","5"); $WIZ_NEXT="3_set_sec";	}
	else if($sec_type==3)	{set("authentication","3"); $WIZ_NEXT="3_set_sec";	}
	else if($sec_type==1)	{set("authentication","0"); $WIZ_NEXT="3_set_sec";	}
	else			{set("authentication","0"); set("wpa/wepmode","0");	$WIZ_NEXT="4_saving";	}
}
else if($ACTION_POST=="3_set_sec")
{
	anchor($G_WIZ_PREFIX_WLAN."/wireless");
	set("secret",$secret);
	set("full_secret",$full_secret);
	
}
else if($ACTION_POST=="4_saving")
{
	anchor($G_WIZ_PREFIX_WLAN."/wireless");
	$ssid=query("ssid");
	$auth=query("authentication");
	$full_secret=query("full_secret");

	anchor("/wireless");
	set("ssid",$ssid);
	if($auth=="0")
	{
		if($full_secret=="")	//open with none wep
		{
			set("authentication","0");	set("wpa/wepmode","0");
		}
		else			//open with 128 bits wep
		{
			set("authentication", "0");	set("wpa/wepmode", "1");
			set("keylength", "128");	set("keyformat", "2");
			set("defkey","1");		set("wepkey:1", $full_secret);
		}
	}
	else if($auth=="3" || $auth=="5")
	{
		set("authentication",$auth);	set("wpa/wepmode","2");	set("wpa/wpapsk",$full_secret);
	}
}
echo "-->\n";
?>
