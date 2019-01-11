<?
/*
 * set layout
 *
 * For old GUI, /sys/regdomain is used to determine the regdomain for wireless.
 * New GUI design should use /runtime/layout/countrycode instead.
 */
if ($ccode == 392)	{set("/sys/regdomain", "mkk");}
else			{set("/sys/regdomain", "fcc");}
set("/runtime/layout/countrycode",	"dummy");
set("/runtime/layout/countrycode",	$ccode);
anchor("/runtime/layout");
set("image_sign",	$image_sign);
set("wanif",		"vlan2");
set("lanif",		"br0");
set("wlanif",		"ath0");
set("wanmac",		$env_wan);
set("lanmac",		$env_wlan);
set("wlanmac",		$env_wlan);

/* flash programming speed */
if ($flashspeed == "")	{ $flashspeed=4000; }
if ($flashspeed < 1800) { $flashspeed=1800; }
$flashspeed = $flashspeed * 13 / 10;
set("/runtime/sys/info/fptime", $flashspeed);
set("/runtime/sys/info/bootuptime", 50);

/* firmware version info. */
set("/runtime/sys/info/firmwareversion",$ver);
set("/runtime/sys/info/hardwareversion","rev ".$hwrev);

/* others */
set("/sys/telnetd",		"true");
set("/sys/sessiontimeout",	"600");

set("/proc/web/sessionum",	"8");
set("/proc/web/authnum",	"6");

set("/function/normal_g",	"1");
set("/function/no_jumpstart",	"1");
set("/function/bridgemode",	"1");
?>
