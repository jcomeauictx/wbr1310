<?
/* vi: set sw=4 ts=4: */
$hostapd_pid    = "/var/run/hostapd.pid";
$hostapd_p1_conf= "/var/run/hostapd_p1.conf";
$hostapd_conf   = "/var/run/hostapd.conf";
$wlan_light_path= "/var/run/gpio_wlan";
$signal_fresetd = "killall -SIGUSR1 fresetd\n";

anchor("/wireless/jumpstart");
$js_phase		= query("phase");
$rekeyinterval	= query("/wireless/wpa/grp_rekey_interval");
if($rekeyinterval!=""){$rekeyinterval="wpa_group_rekey=".$rekeyinterval."\n");}

if($js_phase=="1")
{
	$hostapd_conf_path=$hostapd_p1_conf;
	/* The psk value is fake, when jumpstart is running, it will generate the really psk. */
	$psk="0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
	$js_phase_dsc="jumpstart_p1=1";
}
else
{
	$hostapd_conf_path=$hostapd_conf;
	$psk			= query("psk");
	$js_passhash	= query("passhash");
	$js_salt		= query("salt");
	$js_phase_dsc	="jumpstart_p2=1";
}

fwrite($hostapd_conf_path,
		 "interface=".$wlanif."\n"			."bridge=".$lanif."\n"
		."driver=madwifi\n"					."logger_syslog=0\n"
		."logger_syslog_level=0\n"			."logger_stdout=0\n"
		."logger_stdout_level=0\n"			."debug=0\n"
		."dump_file=/tmp/hostapd.dump\n"	."eapol_key_index_workaround=0\n"
		."wpa=3\n"							."wpa_psk=".$psk."\n"
		."wpa_key_mgmt=WPA-PSK\n"			."wpa_pairwise=CCMP TKIP\n"
		.$js_phase_dsc."\n");

if($js_phase=="2")
{
	fwrite2($hostapd_conf_path,
			."jumpstart_passHash=".$js_passhash."\n"
			."jumpstart_salt=".$js_salt."\n");
}
echo "hostapd -S /etc/templates/hostapd_helper.sh ".$hostapd_conf_path." &\n";
echo "echo $! > ".$hostapd_pid."\n";
if($wlan_debug=="1"){echo "echo hostapd start to run... > /dev/console\n";}

// in phase 1, it allows the station uses open without wep to asscoicate.
if($js_phase=="1")
{
	echo "echo set authmode as open ... > /dev/console\n";
	/* sleep several seconds to wait the hostapd running up. */
	echo "sleep 3\n";
	echo "iwpriv ".$wlanif." authmode 1\n";/* open-system */
	echo "echo 2 > ".$wlan_light_path."\n";
	echo $signal_fresetd;
}
else
{
	echo "echo 3 > ".$wlan_light_path."\n";
	echo $signal_fresetd;
}
?>
