<?
/* vi: set sw=4 ts=4: */
$hostapd_pid	= "/var/run/hostapd.pid";
$hostapd_conf	= "/var/run/hostapd.conf";
$wlxmlpatch_pid	= "/var/run/wlxmlpatch.pid";
$wlan_debug		= query("/runtime/wlan_debug");

if(fread($hostapd_pid)=="")
{
	anchor("/wireless");
	$authentication	= query("authentication");
	$wpawepmode		= query("wpa/wepmode");
	$radiusport		= query("wpa/radiusport");
	$radiusserverip	= query("wpa/radiusserver");
	$radiussecret	= query("wpa/radiussecret");
	$wpapsk			= query("wpa/wpapsk");
	$rekeyinterval	= query("wpa/grp_rekey_interval");
	$ssid			= query("/wireless/ssid");

	if		($authentication==2)	{ $wpa_type=1; $use_8021x=1; } 	/* WPA					*/
	else if	($authentication==3)	{ $wpa_type=1; $use_8021x=0; } 	/* WPA-PSK				*/
	else if	($authentication==4)	{ $wpa_type=2; $use_8021x=1; } 	/* WPA2					*/
	else if	($authentication==5)	{ $wpa_type=2; $use_8021x=0; } 	/* WPA2-PSK				*/
	else if	($authentication==6)	{ $wpa_type=3; $use_8021x=1; } 	/* WPA+WPA2				*/
	else if	($authentication==7)	{ $wpa_type=3; $use_8021x=0; } 	/* WPA-PSK + WPA2-PSK	*/

	fwrite($hostapd_conf,
			 "interface=".$wlanif."\n"			."bridge=".$lanif."\n"
			."driver=madwifi\n"					."logger_syslog=0\n"
			."logger_syslog_level=0\n"			."logger_stdout=0\n"
			."logger_stdout_level=0\n"			."debug=0\n"
			."eapol_key_index_workaround=0\n"	."ssid=".$ssid."\n"
			."wpa=".$wpa_type."\n");
			
	if($rekeyinterval!="")	{fwrite2($hostapd_conf, "wpa_group_rekey=".$rekeyinterval."\n");}

	if		($wpawepmode==2) { fwrite2($hostapd_conf, "wpa_pairwise=TKIP\n");		}
	else if	($wpawepmode==3) { fwrite2($hostapd_conf, "wpa_pairwise=CCMP\n");		}
	else if	($wpawepmode==4) { fwrite2($hostapd_conf, "wpa_pairwise=TKIP CCMP\n");	}

	if ($use_8021x == 1)
	{
		fwrite2($hostapd_conf,	 "ieee8021x=1\n"
				."wpa_key_mgmt=WPA-EAP\n"
				."auth_server_addr=".$radiusserverip."\n"
				."auth_server_port=".$radiusport."\n"
				."auth_server_shared_secret=".$radiussecret."\n");
	}
	else
	{
		if(query("wpa/PassPhraseFormat")=="1")
		{	fwrite2($hostapd_conf,	"wpa_key_mgmt=WPA-PSK\n"."wpa_passphrase=".$wpapsk."\n");	}
		else
		{	fwrite2($hostapd_conf,	"wpa_key_mgmt=WPA-PSK\n"."wpa_psk=".$wpapsk."\n");			}
	}
	echo "hostapd ".$hostapd_conf." &\n";	
	echo "echo $! > ".$hostapd_pid."\n";
	if($wlan_debug=="1"){echo "echo hostapd start to run... > /dev/console\n";}
}
else
{
	echo "echo hostapd is running, so not run hostapd again...> /dev/console\n";
}
?>
