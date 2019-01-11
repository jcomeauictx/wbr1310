#!/bin/sh
echo [$0] ... > /dev/console
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$wlanif			= query("/runtime/layout/wlanif");
$lanif			= query("/runtime/layout/lanif");

$hostapd_pid	= "/var/run/hostapd.pid";
$hostapd_p1_conf= "/var/run/hostapd_p1.conf";
$hostapd_conf	= "/var/run/hostapd.conf";
$wlxmlpatch_pid	= "/var/run/wlxmlpatch.pid";
$wlan_light_path= "/var/run/gpio_wlan";
$signal_fresetd = "killall -SIGUSR1 fresetd\n";
$wlan_debug		= query("/runtime/wlan_debug");

if ($generate_start==1)
{
	echo "echo Start WLAN interface ".$wlanif." ... > /dev/console\n";

	if (query("/wireless/enable")!=1)
	{
		echo "echo Wireless is disabled ! > /dev/console\n";
		exit;
	}

	/* Get configuration */
	anchor("/wireless");
	$ssid			= query("ssid");
	$channel		= query("channel");
	$beaconinterval	= query("beaconinterval");
	$fraglength		= query("fraglength");
	$rtslength		= query("rtslength");
	$ssidhidden		= query("ssidhidden");
	$wlmode			= query("wlmode");
	$ctsmode		= query("ctsmode");				if($ctsmode=="")		{$ctsmode="2";}
	$w11gprotection	= query("w11gprotection");
	$preamble		= query("preamble");			if($preamble=="")		{$preamble="0";}
	$txrate			= query("txrate");
	$txpower		= query("txpower");
	$dtim			= query("dtim");
	$autochannel	= query("autochannel");
	$supermode		= query("supergmode");			if ($supermode=="2")		{$turbo="1"; $ar="0";}else{$turbo="0";}
	$xr				= query("xr");					if ($xr!="1"||$turbo=="1")	{$xr="0";}
	$wlan2wlan		= query("bridge/wlan2wlan");	if ($wlan2wlan!="0")		{$wlan2wlan="1";	}
	$wlan2lan		= query("bridge/wlan2lan");		if ($wlan2lan!="0")			{$wlan2lan="1";		}
	$wmm			= query("wmm/enable");			if ($wmm!="1")				{$wmm="0";			}	
	
	/* wireless mode: 0: turbo, 1:mix mode(g/b), 2:11g only, 3:11b only */
	if		($wlmode == 0)		{ echo "iwpriv ".$wlanif." mode 0\niwpriv ".$wlanif." pureg 0\n"; }
	else if	($wlmode == 1)		{ echo "iwpriv ".$wlanif." mode 3\niwpriv ".$wlanif." pureg 0\n"; }
	else if	($wlmode == 2)		{ echo "iwpriv ".$wlanif." mode 3\niwpriv ".$wlanif." pureg 1\n"; }
	else if	($wlmode == 3)		{ echo "iwpriv ".$wlanif." mode 2\niwpriv ".$wlanif." pureg 0\n"; }
	else if	($wlmode == 4)		{ echo "iwpriv ".$wlanif." mode 1\niwpriv ".$wlanif." pureg 0\n"; }

	if($wmm=="1")
	{
		echo "iwpriv ".$wlanif." wmm 1\n";
		//require($template_root."/wmm.php");
	}
	else
	{
		echo "iwpriv ".$wlanif." wmm 0\n";
	}
	/* 802.11h, we didn't use it, so disable it. If it enabled, the IE will contain coutry code.*/
	echo "iwpriv ".$wlanif." doth 0\n";
	
	if ($autochannel==1)		{ echo "iwconfig ".$wlanif." channel 0\n";							}
	else						{ echo "iwconfig ".$wlanif." channel "		.$channel."\n";			}
	if ($beaconinterval!="")	{ echo "iwpriv "  .$wlanif." bintval "		.$beaconinterval."\n";	}
	if ($rtslength!="")			{ echo "iwconfig ".$wlanif." rts "			.$rtslength."\n";		}
	if ($fraglength!="")		{ echo "iwconfig ".$wlanif." frag "			.$fraglength."\n";		}
	if ($dtim!="")				{ echo "iwpriv "  .$wlanif." dtim_period "	.$dtim."\n";			}
	if ($ssidhidden!="")		{ echo "iwpriv "  .$wlanif." hide_ssid "	.$ssidhidden."\n";		}

	echo "iwconfig ".$wlanif." rate auto\n";
	if ($txrate!="0" && $txrate!="")	{ echo "iwconfig ".$wlanif." rate ".$txrate."M\n"; }
	
	/* supper mode */
	echo "iwpriv ".$wlanif." ff ".			$supermode."\n";
	echo "iwpriv ".$wlanif." burst ".		$supermode."\n";
	echo "iwpriv ".$wlanif." compression ".	$supermode."\n";
	echo "iwpriv ".$wlanif." turbo ".		$turbo."\n";
	if($ar!="")	{echo "iwpriv ".$wlanif." ar ".$ar."\n";}
	echo "iwpriv ".$wlanif." protmode ".	$ctsmode."\n";

	echo "iwpriv ".$wlanif." xr ".			$xr."\n";
	
	echo "echo ".$wlan2lan." > /proc/net/br_forward_br0\n";
	echo "iwpriv ".$wlanif." ap_bridge ".$wlan2wlan."\n";
	
	/* aclmode 0:disable, 1:allow all of the list, 2:deny all of the list */
	echo "iwpriv ".$wlanif." maccmd 3\n";	// flush the ACL database.
	$aclmode=query("acl/mode");
	if		($aclmode == 1)		{ echo "iwpriv ".$wlanif." maccmd 1\n"; }
	else if	($aclmode == 2)		{ echo "iwpriv ".$wlanif." maccmd 2\n"; }
	else						{ echo "iwpriv ".$wlanif." maccmd 0\n"; }
	if ($aclmode == 1 || $aclmode == 2)
	{
		for("/wireless/acl/mac")
		{
			$mac=query("/wireless/acl/mac:".$@);
			echo "iwpriv ".$wlanif." addmac ".$mac."\n";
		}
	}
	
	anchor("/wireless");
	$js_enable	= query("jumpstart/enable");
	$ssid		= query("/wireless/ssid");

	/* Jump Start */
	if($js_enable=="1")
	{
		require($template_root."/js_enabled.php");
	}
	else
	{
		anchor("/wireless");
		$authentication	= query("authentication");
		$keylength		= query("keylength");
		$defkey			= query("defkey");
		$keyformat		= query("keyformat");
		$wpawepmode		= query("wpa/wepmode");

		if($authentication<=1)
		{
			echo "iwpriv ".$wlanif." authmode 1\n";
			if ($wpawepmode == 1)
			{
				if($keyformat==1)	{$iw_keystring="s:\"".get("s","wepkey:".$defkey)."\" [".$defkey."]";}
				else				{$iw_keystring="\"".query("wepkey:".	$defkey)."\" [".$defkey."]";}
				echo "iwconfig ".$wlanif." key ".$iw_keystring."\n";

				/* shared-key */
				if($authentication==1)
				{
					echo "iwpriv ".$wlanif." authmode 2\n";
					echo "iwconfig ".$wlanif." key ".$iw_keystring."\n";
				}
			}
			echo "iwconfig ".$wlanif." essid \"".get("s","ssid")."\"\n";
			echo "ifconfig ".$wlanif." up\n";
			echo "sleep 1\n";
		}
		else
		{
			require($template_root."/hostapd_used.php");
		}
		echo "echo 5 > ".$wlan_light_path."\n";
		echo $signal_fresetd;
	}
	echo "wlxmlpatch & > /dev/console\n";
	echo "echo $! > ".$wlxmlpatch_pid."\n";
	echo "ifconfig ".$wlanif." up\n";
}
else
{
	echo "echo Stop WLAN interface ".$wlanif." ... > /dev/console\n";

	echo "if [ -f ".$wlxmlpatch_pid." ]; then\n";
	echo "kill \`cat ".$wlxmlpatch_pid."\` > /dev/null 2>&1\n";
	echo "rm -f ".$wlxmlpatch_pid."\n";
	echo "fi\n\n";
	
	echo "if [ -f ".$hostapd_pid." ]; then\n";
	echo "kill \`cat ".$hostapd_pid."\` > /dev/null 2>&1\n";
	echo "rm -f ".$hostapd_pid."\n";
	echo "fi\n\n";

	echo "ifconfig ".$wlanif." down\n";
	echo "iwconfig ".$wlanif." key off\n";
	echo "echo 0 > ".$wlan_light_path."\n";
	echo $signal_fresetd;
}
?>
