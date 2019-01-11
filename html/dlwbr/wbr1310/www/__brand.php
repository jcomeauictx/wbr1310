<?
set("/sys/modelname",	"WBR-1310");
anchor("/sys");
set("devicename",	"D-Link Wireless Router");
set("modeldescription",	"Wireless G Router");
set("vendor",		"D-Link");
set("url",		"http:\/\/www.dlink.com");
set("ipaddr",		"192.168.0.1");
set("netmask",		"255.255.255.0");
set("startip",		"192.168.0.100");
set("endip",		"192.168.0.199");
set("ssid",		"dlink");

set("authtype",		"s");

set("/function/no_jumpstart", "1");
set("/function/normal_g", "1");
set("/function/bridgemode", "1");

set("/nat/vrtsrv/max_rules",		"10");
set("/nat/porttrigger/max_rules",	"10");
set("/security/macfilter/max_rules",	"10");
set("/security/urlblocking/max_rules",	"20");
?>
