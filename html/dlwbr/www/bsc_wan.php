<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="bsc_wan";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="bsc";
$SUB_CATEGORY	="bsc_internet";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	$SUBMIT_STR="";
	$dirty=0;
	$rb_dirty=0;

	echo "<!--\n";
	echo "ACTION_POST=".$ACTION_POST."\n";
	echo "enable_ap_mode=".$enable_ap_mode."\n";

	$bridge_setting = query("/bridge");
	if ($enable_ap_mode!=1) {$enable_ap_mode=0;}
	if ($bridge_setting!=1) {$bridge_setting=0;}
	if ($bridge_setting!=$enable_ap_mode) {$rb_dirty++; set("/bridge", $enable_ap_mode);}

	if ($ACTION_POST == "STATIC")
	{
		echo "ipaddr=".$ipaddr."\n";
		echo "netmask=".$netmask."\n";
		echo "gateway=".$gateway."\n";
		echo "clonemac=".$clonemac."\n";
		echo "dns1=".$dns1."\n";
		echo "dns2=".$dns2."\n";
		echo "mtu=".$mtu."\n";

		$entry="/wan/rg/inf:1/static/";
		if (query("/wan/rg/inf:1/mode")	!="1")	{$dirty++; set("/wan/rg/inf:1/mode", "1");}
		if (query($entry."ip")		!=$ipaddr)	{$dirty++; set($entry."ip", $ipaddr);}
		if (query($entry."netmask")	!=$netmask)	{$dirty++; set($entry."netmask", $netmask);}
		if (query($entry."gateway")	!=$gateway)	{$dirty++; set($entry."gateway", $gateway);}
		if (query($entry."clonemac")!=$clonemac){$dirty++; set($entry."clonemac", $clonemac);}
		if (query($entry."mtu")		!=$mtu)		{$dirty++; set($entry."mtu", $mtu);}
		$entry="/dnsrelay/server/";
		if (query($entry."primarydns")	!=$dns1){$dirty++; set($entry."primarydns", $dns1);}
		if (query($entry."secondarydns")!=$dns2){$dirty++; set($entry."secondarydns", $dns2);}
	}
	else if ($ACTION_POST == "DHCP")
	{
		echo "hostname=".$hostname."\n";
		echo "clonemac=".$clonemac."\n";
		echo "dns1=".$dns1."\n";
		echo "dns2=".$dns2."\n";
		echo "mtu=".$mtu."\n";

		$entry="/wan/rg/inf:1/dhcp/";
		if (query("/wan/rg/inf:1/mode") != "2")	{$dirty++; set("/wan/rg/inf:1/mode", "2");}
		if (query("/sys/hostname")!= $hostname)	{$dirty++; set("/sys/hostname", $hostname);}
		if (query($entry."clonemac")!= $clonemac)	{$dirty++; set($entry."clonemac", $clonemac);}
		if (query($entry."mtu")		!= $mtu)		{$dirty++; set($entry."mtu", $mtu);}
		$entry="/dnsrelay/server/";
		if (query($entry."primarydns")	!=$dns1){$dirty++; set($entry."primarydns", $dns1);}
		if (query($entry."secondarydns")!=$dns2){$dirty++; set($entry."secondarydns", $dns2);}
	}
	else if ($ACTION_POST == "PPPOE")
	{
		echo "mode=".$mode."\n";
		echo "username=".$username."\n";
		echo "password=".$password."\n";
		echo "svc_name=".$svc_name."\n";
		echo "ipaddr=".$ipaddr."\n";
		echo "clonemac=".$clonemac."\n";
		echo "dns1=".$dns1."\n";
		echo "dns2=".$dns2."\n";
		echo "idletime=".$idletime."\n";
		echo "mtu=".$mtu."\n";
		echo "ppp_auto=".$ppp_auto."\n";
		echo "ppp_ondemand=".$ppp_ondemand."\n";

		$entry="/wan/rg/inf:1/pppoe/";
		if (query("/wan/rg/inf:1/mode") != "3")		{$dirty++; set("/wan/rg/inf:1/mode", "3");}
		if (query($entry."mode") != $mode)			{$dirty++; set($entry."mode", $mode);}
		if (query($entry."user") != $username)		{$dirty++; set($entry."user", $username);}
		if ($G_DEF_PASSWORD != $password &&
			query($entry."password") != $password)	{$dirty++; set($entry."password", $password);}
		if (query($entry."acservice") != $svc_name)	{$dirty++; set($entry."acservice", $svc_name);}
		if ($mode == "1")
		{
			if (query("staticip")!=$ipaddr)			{$dirty++; set($entry."staticip", $ipaddr);}
			if (query("autodns") != "0")			{$dirty++; set($entry."autodns", "0");}
			$d_en="/dnsrelay/server/";
			if (query($d_en."primarydns")!=$dns1)	{$dirty++; set($d_en."primarydns", $dns1);}
			if (query($d_en."secondarydns")!=$dns2)	{$dirty++; set($d_en."secondarydns", $dns2);}
		}
		else
		{
			if (query("autodns") != "1")			{$dirty++; set($entry."autodns", "1");}
		}
		if($ppp_ondemand=="1")
		{
			$idletime = $idletime * 60;
			if (query($entry."idletimeout")!=$idletime)	{$dirty++; set($entry."idletimeout", $idletime);}
		}
		if (query($entry."mtu")!=$mtu)					{$dirty++; set($entry."mtd", $mtu);}
		if (query($entry."autoreconnect")!=$ppp_auto)	{$dirty++; set($entry."autoreconnect", $ppp_auto);}
		if (query($entry."ondemand") != $ppp_ondemand)	{$dirty++; set($entry."ondemand", $ppp_ondemand);}
		if (query($entry."clonemac") != $clonemac)		{$dirty++; set($entry."clonemac", $clonemac);}
	}
	else if ($ACTION_POST == "PPTP" || $ACTION_POST == "L2TP")
	{
		echo "mode=".$mode."\n";
		echo "ipaddr=".$ipaddr."\n";
		echo "netmask=".$netmask."\n";
		echo "gateway=".$gateway."\n";
		echo "dns=".$dns."\n";
		echo "server=".$server."\n";
		echo "username=".$username."\n";
		echo "password=".$password."\n";
		echo "idletime=".$idletime."\n";
		echo "mtu=".$mtu."\n";
		echo "ppp_auto=".$ppp_auto."\n";
		echo "ppp_ondemand=".$ppp_ondemand."\n";

		if ($ACTION_POST == "PPTP")
		{
			if (query("/wan/rg/inf:1/mode")!="4")	{$dirty++; set("/wan/rg/inf:1/mode", "4");}
			$entry="/wan/rg/inf:1/pptp/";
		}
		else
		{
			if (query("/wan/rg/inf:1/mode")!="5")	{$dirty++; set("/wan/rg/inf:1/mode", "5");}
			$entry="/wan/rg/inf:1/l2tp/";
		}
		if (query($entry."mode")!=$mode)			{$dirty++; set($entry."mode", $mode);}
		if ($mode == "1")
		{
			if (query($entry."ip")!=$ipaddr)		{$dirty++; set($entry."ip", $ipaddr);}
			if (query($entry."netmask")!=$netmask)	{$dirty++; set($entry."netmask", $netmask);}
			if (query($entry."gateway")!=$gateway)	{$dirty++; set($entry."gateway", $gateway);}
			if (query($entry."dns")!=$dns)			{$dirty++; set($entry."dns", $dns);}
		}
		if (query($entry."serverip")!=$server)		{$dirty++; set($entry."serverip", $server);}
		if (query($entry."user")!=$username)		{$dirty++; set($entry."user", $username);}
		if ($password != $G_DEF_PASSWORD)			{$dirty++; set($entry."password", $password);}
		if($ppp_ondemand=="1")
		{
			$idletime = $idletime * 60;
			if (query($entry."idletimeout")!=$idletime)	{$dirty++; set($entry."idletimeout", $idletime);}
		}
		if (query($entry."mtu")!=$mtu)				{$dirty++; set($entry."mtu", $mtu);}
		if (query($entry."autoreconnect")!=$ppp_auto){$dirty++;set($entry."autoreconnect", $ppp_auto);}
		if (query($entry."ondemand")!=$ppp_ondemand){$dirty++; set($entry."ondemand", $ppp_ondemand);}
	}
	else if ($ACTION_POST == "BIGPOND")
	{
		echo "username=".$username."\n";
		echo "password=".$password."\n";
		echo "authserver=".$authserver."\n";
		echo "login_server=".$login_server."\n";
		echo "clonemac=".$clonemac."\n";

		$entry="/wan/rg/inf:1/bigpond/";
		if (query("/wan/rg/inf:1/mode")!="7")		{$dirty++; set("/wan/rg/inf:1/mode", "7");}
		if (query($entry."username")!=$username)	{$dirty++; set($entry."username", $username);}
		if ($G_DEF_PASSWORD != $password)			{$dirty++; set($entry."password", $password);}
		if (query($entry."server")!=$authserver)	{$dirty++; set($entry."server", $authserver);}
		if (query($entry."loginserver")!=$login_server)	{$dirty++; set($entry."loginserver", $login_server);}
		if (query($entry."clonemac")!=$clonemac)	{$dirty++; set($entry."clonemac", $clonemac);}
	}

	$NEXT_PAGE=$MY_NAME;
	if ($rb_dirty > 0)		{$SUBMIT_STR=";submit SYSTEM";$XGISET_STR="set/runtime/stats/resetCounter=1"; $NEXT_PAGE="bsc_chg_rg_mode";}
	else if ($dirty > 0)	{$SUBMIT_STR=";submit WAN";}
	echo "SUBMIT_STR=".$SUBMIT_STR."\n";
	//$SUBMIT_STR="";
	echo "-->\n";

	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
anchor("/dnsrelay/server");
$cfg_dns1	= query("primarydns");
$cfg_dns2	= query("secondarydns");

anchor("/wan/rg/inf:1");
$cfg_mode = query("mode");

anchor("/wan/rg/inf:1/static");
$cfg_static_ip		= query("ip");
$cfg_static_mask	= query("netmask");
$cfg_static_gw		= query("gateway");
$cfg_static_mac		= query("clonemac");
$cfg_static_mtu		= query("mtu");

anchor("/wan/rg/inf:1/dhcp");
$cfg_dhcp_hostname	= get("h","/sys/hostname");
$cfg_dhcp_mac		= query("clonemac");
$cfg_dhcp_mtu		= query("mtu");

anchor("/wan/rg/inf:1/pppoe");
$cfg_poe_mode		= query("mode");
$cfg_poe_staticip	= query("staticip");
$cfg_poe_user		= get("h","user");
$cfg_poe_service	= get("h","acservice");
$cfg_poe_auto_conn	= query("autoreconnect");
$cfg_poe_on_demand	= query("ondemand");
$cfg_poe_idle_time	= query("idletimeout");
$cfg_poe_mtu		= query("mtu");
$cfg_poe_mac		= query("clonemac");
$cfg_poe_idle_time	= $cfg_poe_idle_time/60;

anchor("/wan/rg/inf:1/pptp");
$cfg_pptp_mode		= query("mode");
$cfg_pptp_ip		= query("ip");
$cfg_pptp_mask		= query("netmask");
$cfg_pptp_gw		= query("gateway");
$cfg_pptp_dns		= query("dns");
$cfg_pptp_server	= query("serverip");
$cfg_pptp_user		= get("h","user");
$cfg_pptp_auto_conn	= query("autoreconnect");
$cfg_pptp_on_demand	= query("ondemand");
$cfg_pptp_idle_time	= query("idletimeout");
$cfg_pptp_mtu		= query("mtu");
$cfg_pptp_idle_time	= $cfg_pptp_idle_time/60;

anchor("/wan/rg/inf:1/l2tp");
$cfg_l2tp_mode		= query("mode");
$cfg_l2tp_ip		= query("ip");
$cfg_l2tp_mask		= query("netmask");
$cfg_l2tp_gw		= query("gateway");
$cfg_l2tp_dns		= query("dns");
$cfg_l2tp_server	= query("serverip");
$cfg_l2tp_user		= get("h","user");
$cfg_l2tp_auto_conn	= query("autoreconnect");
$cfg_l2tp_on_demand	= query("ondemand");
$cfg_l2tp_idle_time	= query("idletimeout");
$cfg_l2tp_mtu		= query("mtu");
$cfg_l2tp_idle_time	= $cfg_l2tp_idle_time/60;

anchor("/wan/rg/inf:1/bigpond");
$cfg_bp_user		= get(h,"username");
$cfg_bp_server		= query("server");
$cfg_bp_mac			= query("clonemac");
$cfg_bp_loginserver	= get(h,"loginserver");

/* --------------------------------------------------------------------------- */
?>
<script>

function on_change_wan_type()
{
	var frm = get_obj("frm");

	get_obj("show_static").style.display = "none";
	get_obj("show_dhcp").style.display = "none";
	get_obj("show_pppoe").style.display = "none";
	get_obj("show_pptp").style.display = "none";
	get_obj("show_l2tp").style.display = "none";
	get_obj("show_bigpond").style.display = "none";

	switch (frm.wan_type.value)
	{
	case "1":	get_obj("show_static").style.display = ""; break;
	case "2":	get_obj("show_dhcp").style.display = ""; break;
	case "3":	get_obj("show_pppoe").style.display = ""; break;
	case "4":	get_obj("show_pptp").style.display = ""; break;
	case "5":	get_obj("show_l2tp").style.display = ""; break;
	case "7":	get_obj("show_bigpond").style.display = ""; break;
	}
}

function set_clone_mac(form)
{
	var f = get_obj(form);
	var addr = get_mac("<?=$macaddr?>");

	f.mac1.value = addr[1];
	f.mac2.value = addr[2];
	f.mac3.value = addr[3];
	f.mac4.value = addr[4];
	f.mac5.value = addr[5];
	f.mac6.value = addr[6];
}

function check_mac(m1,m2,m3,m4,m5,m6, result)
{
	result.value = "";

	if (is_blank(m1.value) && is_blank(m2.value) && is_blank(m3.value) &&
		is_blank(m4.value) && is_blank(m5.value) && is_blank(m6.value))
	{
		return true;
	}

	if (!is_valid_mac(m1.value)) return false;
	if (!is_valid_mac(m2.value)) return false;
	if (!is_valid_mac(m3.value)) return false;
	if (!is_valid_mac(m4.value)) return false;
	if (!is_valid_mac(m5.value)) return false;
	if (!is_valid_mac(m6.value)) return false;

	if (m1.value.length == 1) m1.value = "0"+m1.value;
	if (m2.value.length == 1) m2.value = "0"+m2.value;
	if (m3.value.length == 1) m3.value = "0"+m3.value;
	if (m4.value.length == 1) m4.value = "0"+m4.value;
	if (m5.value.length == 1) m5.value = "0"+m5.value;
	if (m6.value.length == 1) m6.value = "0"+m6.value;

	result.value = m1.value+":"+m2.value+":"+m3.value+":"+m4.value+":"+m5.value+":"+m6.value;
	return true;
}

function on_click_ppp_ipmode(form)
{
	var f = get_obj(form);

	if (form == "frm_pppoe")
	{
		f.ipaddr.disabled = f.dns1.disabled = f.dns2.disabled =
		f.ipmode[0].checked ? true : false;
	}
	else if (form == "frm_pptp" || form == "frm_l2tp")
	{
		f.ipaddr.disabled = f.netmask.disabled = f.gateway.disabled =
		f.dns.disabled = f.ipmode[0].checked ? true : false;
	}
}

/*  static ip -----------------------------------------------------------*/
function init_static()
{
	var f = get_obj("frm_static");
	var addr = get_mac("<?=$cfg_static_mac?>");

	f.mac1.value = addr[1];
	f.mac2.value = addr[2];
	f.mac3.value = addr[3];
	f.mac4.value = addr[4];
	f.mac5.value = addr[5];
	f.mac6.value = addr[6];
}

function check_static()
{
	var f = get_obj("frm_static");

	if (!is_valid_ip(f.ipaddr.value, 0))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.ipaddr, "**");
		return false;
	}
	if (!is_valid_mask(f.netmask.value))
	{
		alert("<?=$a_invalid_netmask?>");
		field_focus(f.netmask, "**");
		return false;
	}
	if (!is_valid_ip(f.gateway.value, 0))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.gateway, "**");
		return false;
	}
	if (check_mac(f.mac1, f.mac2, f.mac3, f.mac4, f.mac5, f.mac6, f.clonemac)==false)
	{
		alert("<?=$a_invalid_mac?>");
		field_focus(f.mac1, "**");
		return false;
	}
	if (!is_valid_ip(f.dns1.value, 0))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.dns1, "**");
		return false;
	}
	if (!is_valid_ip(f.dns2.value, 1))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.dns2, "**");
		return false;
	}
	if (!is_digit(f.mtu.value) || !is_in_range(f.mtu.value, 200, 1500))
	{
		alert("<?=$a_invalid_mtu?>");
		field_focus(f.mtu, "**");
		return false;
	}
	f.submit();
	return true;
}

/*  DHCP  -----------------------------------------------------------*/
function init_dhcp()
{
	var f = get_obj("frm_dhcp");
	var addr = get_mac("<?=$cfg_dhcp_mac?>");

	f.mac1.value = addr[1];
	f.mac2.value = addr[2];
	f.mac3.value = addr[3];
	f.mac4.value = addr[4];
	f.mac5.value = addr[5];
	f.mac6.value = addr[6];
}

function check_dhcp()
{
	var f = get_obj("frm_dhcp");

	if (is_blank(f.hostname.value) || !strchk_hostname(f.hostname.value))
	{
		alert("<?=$a_invalid_hostname?>");
		field_focus(f.hostname, "**");
		return false;
	}
	if (check_mac(f.mac1, f.mac2, f.mac3, f.mac4, f.mac5, f.mac6, f.clonemac)==false)
	{
		alert("<?=$a_invalid_mac?>");
		field_focus(f.mac1, "**");
		return false;
	}
	if (!is_valid_ip(f.dns1.value, 1))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.dns1, "**");
		return false;
	}
	if (!is_valid_ip(f.dns2.value, 1))
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.dns2, "**");
		return false;
	}
	if (!is_digit(f.mtu.value) || !is_in_range(f.mtu.value, 200, 1500))
	{
		alert("<?=$a_invalid_mtu?>");
		field_focus(f.mtu, "**");
		return false;
	}
	f.submit();
	return true;
}

/*  PPPOE  -----------------------------------------------------------*/
function init_pppoe()
{
	var f = get_obj("frm_pppoe");
	var addr = get_mac("<?=$cfg_poe_mac?>");

	f.mac1.value = addr[1];
	f.mac2.value = addr[2];
	f.mac3.value = addr[3];
	f.mac4.value = addr[4];
	f.mac5.value = addr[5];
	f.mac6.value = addr[6];

<?
	echo "\/\/ auto=".$cfg_poe_auto_conn.", ondemand=".$cfg_poe_on_demand."\n";
	if ($cfg_poe_mode != "1")	{ echo "	f.ipmode[0].checked = true;\n";}
	else						{ echo "	f.ipmode[1].checked = true;\n";}
	if ($cfg_poe_auto_conn == "1")
	{	if ($cfg_poe_on_demand == "1")	{echo "	f.ppp_conn_mode[2].checked=true;\n";}
		else							{echo "	f.ppp_conn_mode[0].checked=true;\n";}
	}	else							{echo " f.ppp_conn_mode[1].checked=true;\n";}	?>

	on_click_ppp_ipmode("frm_pppoe");
	chg_ppp_conn_mode(f);
}

function check_pppoe()
{
	var f = get_obj("frm_pppoe");

	f.mode.value = f.ipmode[0].checked ? "2" : "1";
	if (is_blank(f.username.value))
	{
		alert("<?=$a_invalid_username?>");
		field_focus(f.username, "**");
		return false;
	}
	if (f.password.value != f.password_v.value)
	{
		alert("<?=$a_password_mismatch?>");
		field_focus(f.password, "**");
		return false;
	}
	if (f.mode.value == "1")
	{
		if (!is_valid_ip(f.ipaddr.value, 0))
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.ipaddr, "**");
			return false;
		}
		if (!is_valid_ip(f.dns1.value, 0))
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.dns1, "**");
			return false;
		}
		if (!is_valid_ip(f.dns2.value, 1))
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.dns2, "**");
			return false;
		}
	}
	if (check_mac(f.mac1, f.mac2, f.mac3, f.mac4, f.mac5, f.mac6, f.clonemac)==false)
	{
		alert("<?=$a_invalid_mac?>");
		field_focus(f.mac1, "**");
		return false;
	}
	if (!is_digit(f.mtu.value) || !is_in_range(f.mtu.value, 200, 1492))
	{
		alert("<?=$a_invalid_mtu?>");
		field_focus(f.mtu, "**");
		return false;
	}
	if (f.ppp_conn_mode[0].checked)
	{
		f.ppp_auto.value = "1";
		f.ppp_ondemand.value = "0";
	}
	else if (f.ppp_conn_mode[1].checked)
	{
		f.ppp_auto.value = "0";
		f.ppp_ondemand.value = "0";
	}
	else
	{
		if (!is_digit(f.idletime.value))
		{
			alert("<?=$a_invalid_idletime?>");
			field_focus(f.idletime, "**");
			return false;
		}
		f.ppp_auto.value = "1";
		f.ppp_ondemand.value = "1";
	}
	f.submit();
	return true;
}

/*  PPTP/L2TP  -----------------------------------------------------------*/

function init_pptp()
{
	var f = get_obj("frm_pptp");

<?
	if ($cfg_pptp_mode != "1")	{echo "	f.ipmode[0].checked=true;\n";}
	else						{echo "	f.ipmode[1].checked=true;\n";}
	if ($cfg_pptp_auto_conn == "1")
	{	if ($cfg_pptp_on_demand == "1")	{echo "	f.ppp_conn_mode[2].checked=true;\n";}
		else							{echo "	f.ppp_conn_mode[0].checked=true;\n";}
	}	else							{echo "	f.ppp_conn_mode[1].checked=true;\n";}	?>
	on_click_ppp_ipmode("frm_pptp");
	chg_ppp_conn_mode(f);
}

function init_l2tp()
{
	var f = get_obj("frm_l2tp");

<?
	if ($cfg_l2tp_mode != "1")	{echo "	f.ipmode[0].checked=true;\n";}
	else						{echo "	f.ipmode[1].checked=true;\n";}
	if ($cfg_l2tp_auto_conn == "1")
	{	if ($cfg_l2tp_on_demand == "1")	{echo "	f.ppp_conn_mode[2].checked=true;\n";}
		else							{echo "	f.ppp_conn_mode[0].checked=true;\n";}
	}	else							{echo "	f.ppp_conn_mode[1].checked=true;\n";}	?>
	on_click_ppp_ipmode("frm_l2tp");
	chg_ppp_conn_mode(f);
}

function check_pptp_l2tp(form)
{
	var f = get_obj(form);
	var net1, net2;

	f.mode.value = f.ipmode[0].checked ? "2" : "1";
	if (f.mode.value == "1")
	{
		if (!is_valid_ip(f.ipaddr.value, 0))
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.ipaddr, "**");
			return false;
		}
		if (!is_valid_mask(f.netmask.value))
		{
			alert("<?=$a_invalid_netmask?>");
			field_focus(f.netmask, "**");
			return false;
		}
		if (is_blank(f.gateway.value))
		{
			if (!is_valid_ip(f.server.value, 0))
			{
				alert("<?=$a_invalid_ip?>");
				field_focus(f.server, "**");
				return false;
			}
			net1 = get_network_id(f.ipaddr.value, f.netmask.value);
			net2 = get_network_id(f.server.value, f.netmask.value);
			if (net1[0] != net2[0])
			{
				alert("<?=$a_srv_in_different_subnet?>");
				field_focus(f.server, "**");
				return false;
			}
			f.dns.value = "";
		}
		else
		{
			if (!is_valid_ip(f.gateway.value, 0))
			{
				alert("<?=$a_invalid_ip?>");
				field_focus(f.gateway.value, "**");
				return false;
			}
			net1 = get_network_id(f.ipaddr.value, f.netmask.value);
			net2 = get_network_id(f.gateway.value, f.netmask.value);
			if (net1[0] != net2[0])
			{
				alert("<?=$a_gw_in_different_subnet?>");
				field_focus(f.gateway, "**");
				return false;
			}
			if (is_blank(f.dns.value))
			{
				if (!is_valid_ip(f.server.value, 0))
				{
					alert("<?=$a_invalid_ip?>");
					field_focus(f.server, "**");
					return false;
				}
			}
			else
			{
				if (!is_valid_ip(f.dns.value, 0))
				{
					alert("<?=$a_invalid_ip?>");
					field_focus(f.dns, "**");
					return false;
				}
				if (strchk_hostname(f.server.value)==false)
				{
					alert("<?=$a_invalid_hostname?>");
					field_focus(f.server, "**");
					return false;
				}
			}
		}
	}
	else
	{
		if (is_blank(f.server.value))
		{
			alert("<?=$a_server_empty?>");
			field_focus(f.server, "**");
			return false;
		}
		if (strchk_hostname(f.server.value)==false)
		{
			alert("<?=$a_invalid_hostname?>");
			field_focus(f.server, "**");
			return false;
		}
	}
	if (is_blank(f.username.value))
	{
		alert("<?=$a_account_empty?>");
		field_focus(f.username, "**");
		return false;
	}
	if (f.password.value != f.password_v.value)
	{
		alert("<?=$a_password_mismatch?>");
		field_focus(f.password, "**");
		return false;
	}
	if (!is_digit(f.mtu.value) || !is_in_range(f.mtu.value, 200, 1400))
	{
		alert("<?=$a_invalid_mtu?>");
		field_focus(f.mtu, "**");
		return false;
	}
	if (f.ppp_conn_mode[0].checked)
	{
		f.ppp_auto.value = "1";
		f.ppp_ondemand.value = "0";
	}
	else if (f.ppp_conn_mode[1].checked)
	{
		f.ppp_auto.value = "0";
		f.ppp_ondemand.value = "0";
	}
	else
	{
		if (!is_digit(f.idletime.value))
		{
			alert("<?=$a_invalid_idletime?>");
			field_focus(f.idletime, "**");
			return false;
		}
		f.ppp_auto.value = "1";
		f.ppp_ondemand.value = "1";
	}
	f.submit();
	return true;
}

/*  BigPond -----------------------------------------------------------*/

function init_bigpond()
{
	var f = get_obj("frm_bigpond");
	var addr = get_mac("<?=$cfg_bp_mac?>");

	f.mac1.value = addr[1];
	f.mac2.value = addr[2];
	f.mac3.value = addr[3];
	f.mac4.value = addr[4];
	f.mac5.value = addr[5];
	f.mac6.value = addr[6];
}

function check_bigpond()
{
	var f = get_obj("frm_bigpond");

	if (is_blank(f.username.value))
	{
		alert("<?=$a_invalid_username?>");
		field_focus(f.username, "**");
		return false;
	}
	if (f.password.value != f.password_v.value)
	{
		alert("<?=$a_password_mismatch?>");
		field_focus(f.password, "**");
		return false;
	}
	if (check_mac(f.mac1, f.mac2, f.mac3, f.mac4, f.mac5, f.mac6, f.clonemac)==false)
	{
		alert("<?=$a_invalid_mac?>");
		field_focus(f.mac1, "**");
		return false;
	}
	f.submit();
	return true;
}

/*  -----------------------------------------------------------*/

/* page init functoin */
function init()
{
	// init here ...
	f=get_obj("frm");
	f.enable_ap_mode.checked=<?map("/bridge","1","true","*","false");?>;

	init_static();
	init_dhcp();
	init_pppoe();
	init_pptp();
	init_l2tp();
	init_bigpond();

	on_change_wan_type();
	chg_ap_mode();
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	// do check here ....
	if(f.enable_ap_mode.checked)
	{
		f.enable_ap_mode.value=1;
		f.submit();
		return true;
	}
	else
	{
		f.enable_ap_mode.value=0;
		switch (f.wan_type.value)
		{
		case "1":	return check_static();
		case "2":	return check_dhcp();
		case "3":	return check_pppoe();
		case "4":	return check_pptp_l2tp("frm_pptp");
		case "5":	return check_pptp_l2tp("frm_l2tp");
		case "7":	return check_bigpond();
		}
	}
	return false;
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}
function chg_ppp_conn_mode(f)
{
	var dis=true;
	if(f.ppp_conn_mode[2].checked) dis=false;
	f.idletime.disabled=dis;
}
function chg_ap_mode()
{
	var f=get_obj("frm");
	var now_form, i, init_str;
	var dis=false;
	if(f.enable_ap_mode.checked)	dis=true;

	f.wan_type.disabled=dis;
	switch (f.wan_type.value)
	{
	case "1":
		now_form=get_obj("frm_static");
		init_str="init_static()";
		break;
	case "2":
		now_form=get_obj("frm_dhcp");
		init_str="init_dhcp()";
		break;
	case "3":
		now_form=get_obj("frm_pppoe");
		init_str="init_pppoe()";
		break;
	case "4":
		now_form=get_obj("frm_pptp");
		init_str="init_pptp()";
		break;
	case "5":
		now_form=get_obj("frm_l2tp");
		init_str="init_l2tp()";
		break;
	case "7":
		now_form=get_obj("frm_bigpond");
		init_str="init_bigpond()";
		break;
	}
	fields_disabled(now_form, dis);
	eval(init_str);
}
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<?require("/www/model/__banner.php");?>
<?require("/www/model/__menu_top.php");?>
<table <?=$G_MAIN_TABLE_ATTR?> height="100%">
<tr valign=top>
	<td <?=$G_MENU_TABLE_ATTR?>>
	<?require("/www/model/__menu_left.php");?>
	</td>
	<td id="maincontent">
		<div id="box_header">
		<? require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php"); ?>
		<script>apply('check()'); echo("&nbsp;"); cancel('');</script>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
		<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<div class="box">
			<h2><?=$m_title_ap_mode?></h2>
			<p><?=$m_desc_ap_mode?></p>
			<input type="checkbox" name="enable_ap_mode" onclick="chg_ap_mode();"><?=$m_enable_ap_mode?>
			<input type="hidden" name="ACTION_POST" value="AP_MODE">
		</div>
		<div class="box">
			<h2><?=$m_title_wan_type?></h2>
			<p><?=$m_desc_wan_type?></p>
			<table cellSpacing=1 cellPadding=1 width=525 border=0>
			<tbody>
			<tr>
				<td class="r_tb" width=150><?=$m_wan_type?> :</td>
				<td class="l_tb">&nbsp;
					<select name="wan_type" id="wan_type" onChange="on_change_wan_type()">
						<option value="1"<? if ($cfg_mode==1) {echo " selected";} ?>><?=$m_static_ip?></option>
						<option value="2"<? if ($cfg_mode==2) {echo " selected";} ?>><?=$m_dhcp?></option>
						<option value="3"<? if ($cfg_mode==3) {echo " selected";} ?>><?=$m_pppoe?></option>
						<option value="4"<? if ($cfg_mode==4) {echo " selected";} ?>><?=$m_pptp?></option>
						<option value="5"<? if ($cfg_mode==5) {echo " selected";} ?>><?=$m_l2tp?></option>
						<option value="7"<? if ($cfg_mode==7) {echo " selected";} ?>><?=$m_bigpond?></option>
					</select>
				</td>
			</tr>
			</tbody>
			</table>
		</div>
		</form>

	<!-- STATIC IP  -->
		<div class="box" id="show_static" style="display:none">
		<h2><?=$m_title_static?></h2>
		<form name="frm_static" id="frm_static" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="STATIC">
		<p><?=$m_desc_static?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150><?=$m_ipaddr?> :</td>
			<td class="t_tb">&nbsp;
				<input type=text id=ipaddr name=ipaddr size=16 maxlength=15 value="<?=$cfg_static_ip?>">
				<?=$m_comment_isp?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_subnet?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=netmask name=netmask size=16 maxlength=15 value="<?=$cfg_static_mask?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_isp_gateway?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=gateway name=gateway size=16 maxlength=15 value="<?=$cfg_static_gw?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_macaddr?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mac1 name=mac1 size=2 maxlength=2 value=""> -
				<input type=text id=mac2 name=mac2 size=2 maxlength=2 value=""> -
				<input type=text id=mac3 name=mac3 size=2 maxlength=2 value=""> -
				<input type=text id=mac4 name=mac4 size=2 maxlength=2 value=""> -
				<input type=text id=mac5 name=mac5 size=2 maxlength=2 value=""> -
				<input type=text id=mac6 name=mac6 size=2 maxlength=2 value=""> <?=$m_optional?>&nbsp;&nbsp;
				<input type="button" value="<?=$m_clone_mac?>" name="clone" onclick='set_clone_mac("frm_static")'>
			</td>
			<input type=hidden id=clonemac name=clonemac>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_primary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns1 name=dns1 size=16 maxlength=15 value="<?=$cfg_dns1?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_secondary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns2 name=dns2 size=16 maxlength=15 value="<?=$cfg_dns2?>">&nbsp;<?=$m_optional?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_mtu?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mtu name=mtu maxlength=4 size=5 value="<?=$cfg_static_mtu?>">
			</td>
		</tr>
		</table>
		</form>
		</div>

	<!-- DHCP  -->
		<div class="box" id="show_dhcp" style="display:none">
		<h2><?=$m_title_dhcp?></h2>
		<form name="frm_dhcp" id="frm_dhcp" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="DHCP">
		<p><?=$m_desc_dhcp?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150><?=$m_host_name?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=hostname name=hostname size=40 maxlength=39 value="<?=$cfg_dhcp_hostname?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_macaddr?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mac1 name=mac1 size=2 maxlength=2 value=""> -
				<input type=text id=mac2 name=mac2 size=2 maxlength=2 value=""> -
				<input type=text id=mac3 name=mac3 size=2 maxlength=2 value=""> -
				<input type=text id=mac4 name=mac4 size=2 maxlength=2 value=""> -
				<input type=text id=mac5 name=mac5 size=2 maxlength=2 value=""> -
				<input type=text id=mac6 name=mac6 size=2 maxlength=2 value=""> <?=$m_optional?>&nbsp;&nbsp;
				<input type="button" value="<?=$m_clone_mac?>" name="clone" onclick='set_clone_mac("frm_dhcp")'>
			</td>
			<input type=hidden id=clonemac name=clonemac>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_primary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns1 name=dns1 size=16 maxlength=15 value="<?=$cfg_dns1?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_secondary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns2 name=dns2 size=16 maxlength=15 value="<?=$cfg_dns2?>">&nbsp;<?=$m_optional?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_mtu?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mtu name=mtu maxlength=4 size=5 value="<?=$cfg_dhcp_mtu?>">
			</td>
		</tr>
		</table>
		</form>
		</div>

	<!-- PPPoE  -->
		<div class="box" id="show_pppoe" style="display:none">
		<h2><?=$m_title_pppoe?></h2>
		<form name="frm_pppoe" id="frm_pppoe" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="PPPOE">
		<p><?=$m_desc_pppoe?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150></td>
		    <td class="l_tb">&nbsp;
				<input type=radio value=0 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm_pppoe")><?=$m_dynamic_pppoe?>
				<input type=radio value=1 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm_pppoe")><?=$m_static_pppoe?>
				<input type=hidden id=mode name=mode>
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_user_name?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=username name=username size=30 maxlength=63 value="<?=$cfg_poe_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_password?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password name=password size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_retype_pwd?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password_v name=password_v size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_pppoe_svc_name?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=svc_name name=svc_name size=30 maxlength=63 value="<?=$cfg_poe_service?>">&nbsp;<?=$m_optional?>
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ipaddr?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=ipaddr name=ipaddr size=16 maxlength=15 value="<?=$cfg_poe_staticip?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_macaddr?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mac1 name=mac1 size=2 maxlength=2 value=""> -
				<input type=text id=mac2 name=mac2 size=2 maxlength=2 value=""> -
				<input type=text id=mac3 name=mac3 size=2 maxlength=2 value=""> -
				<input type=text id=mac4 name=mac4 size=2 maxlength=2 value=""> -
				<input type=text id=mac5 name=mac5 size=2 maxlength=2 value=""> -
				<input type=text id=mac6 name=mac6 size=2 maxlength=2 value="">&nbsp;<?=$m_optional?>&nbsp;&nbsp;
				<input type="button" value="<?=$m_clone_mac?>" name="clone" onclick='set_clone_mac("frm_pppoe")'>
			</td>
			<input type=hidden id=clonemac name=clonemac>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_primary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns1 name=dns1 size=16 maxlength=15 value="<?=$cfg_dns1?>"></td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_secondary_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns2 name=dns2 size=16 maxlength=15 value="<?=$cfg_dns2?>">&nbsp;<?=$m_optional?></td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_idle_time?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=idletime name=idletime size=5 maxlength=4 value="<?=$cfg_poe_idle_time?>">&nbsp;<?=$m_minutes?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_mtu?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mtu name=mtu maxlength=4 size=5 value="<?=$cfg_poe_mtu?>"></td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_connect_mode?> :</td>
			<td class="l_tb">&nbsp;
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=0 onclick="chg_ppp_conn_mode(this.form);"><?=$m_always_on?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=1 onclick="chg_ppp_conn_mode(this.form);"><?=$m_manual?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=2 onclick="chg_ppp_conn_mode(this.form);"><?=$m_on_demand?>
				<input type=hidden name=ppp_auto id=ppp_auto>
				<input type=hidden name=ppp_ondemand id=ppp_ondemand>
			</td>
		</tr>
		</table>
		</form>
		</div>

	<!-- PPTP  -->
		<div class="box" id="show_pptp" style="display:none">
		<h2><?=$m_title_pptp?></h2>
		<form name="frm_pptp" id="frm_pptp" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="PPTP">
		<p><?=$m_desc_pptp?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150></td>
		    <td class="l_tb">&nbsp;
				<input type=radio value=0 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm_pptp")><?=$m_dynamic_ip?>
				<input type=radio value=1 id=ipmode name=ipmode onClick=on_click_ppp_ipmode("frm_pptp")><?=$m_static_ip?>
				<input type=hidden id=mode name=mode>
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ipaddr?> :</td>
			<td class="t_tb">&nbsp;
				<input type=text id=ipaddr name=ipaddr size=16 maxlength=15 value="<?=$cfg_pptp_ip?>">
				<?=$m_comment_isp?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_subnet?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=netmask name=netmask size=16 maxlength=15 value="<?=$cfg_pptp_mask?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_gateway?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=gateway name=gateway size=16 maxlength=15 value="<?=$cfg_pptp_gw?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns name=dns size=16 maxlength=15 value="<?=$cfg_pptp_dns?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_server_ip?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=server name=server size=32 maxlength=32 value="<?=$cfg_pptp_server?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_pptp_account?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=username name=username size=32 maxlength=63 value="<?=$cfg_pptp_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_pptp_password?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password name=password size=32 maxlength=31 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_pptp_retype_pwd?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password_v name=password_v size=32 maxlength=31 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_idle_time?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=idletime name=idletime size=5 maxlength=4 value="<?=$cfg_pptp_idle_time?>">&nbsp;<?=$m_minutes?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_mtu?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mtu name=mtu maxlength=4 size=5 value="<?=$cfg_pptp_mtu?>"></td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_connect_mode?> :</td>
			<td class="l_tb">&nbsp;
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=0 onclick="chg_ppp_conn_mode(this.form);"><?=$m_always_on?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=1 onclick="chg_ppp_conn_mode(this.form);"><?=$m_manual?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=2 onclick="chg_ppp_conn_mode(this.form);"><?=$m_on_demand?>
				<input type=hidden name=ppp_auto id=ppp_auto>
				<input type=hidden name=ppp_ondemand id=ppp_ondemand>
			</td>
		</tr>
		</table>
		</form>
		</div>

	<!-- L2TP  -->
		<div class="box" id="show_l2tp" style="display:none">
		<h2>L2TP</h2>
		<form name="frm_l2tp" id="frm_l2tp" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="L2TP">
		<p><?=$m_desc_l2tp?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150></td>
		    <td class="l_tb">&nbsp;
				<input type=radio value=0 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm_l2tp")><?=$m_dynamic_ip?>
				<input type=radio value=1 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm_l2tp")><?=$m_static_ip?>
				<input type=hidden id=mode name=mode>
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ipaddr?> :</td>
			<td class="t_tb">&nbsp;
				<input type=text id=ipaddr name=ipaddr size=16 maxlength=15 value="<?=$cfg_l2tp_ip?>">
				<?=$m_comment_isp?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_subnet?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=netmask name=netmask size=16 maxlength=15 value="<?=$cfg_l2tp_mask?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_gateway?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=gateway name=gateway size=16 maxlength=15 value="<?=$cfg_l2tp_gw?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_dns?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=dns name=dns size=16 maxlength=15 value="<?=$cfg_l2tp_dns?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_server_ip?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=server name=server size=32 maxlength=32 value="<?=$cfg_l2tp_server?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_l2tp_account?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=username name=username size=32 maxlength=63 value="<?=$cfg_l2tp_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_l2tp_password?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password name=password size=32 maxlength=31 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_l2tp_retype_pwd?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password_v name=password_v size=32 maxlength=31 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_idle_time?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=idletime name=idletime size=5 maxlength=4 value="<?=$cfg_l2tp_idle_time?>">&nbsp;<?=$m_minutes?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_mtu?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mtu name=mtu maxlength=4 size=5 value="<?=$cfg_l2tp_mtu?>"></td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_ppp_connect_mode?> :</td>
			<td class="l_tb">&nbsp;
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=0 onclick="chg_ppp_conn_mode(this.form);"><?=$m_always_on?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=1 onclick="chg_ppp_conn_mode(this.form);"><?=$m_manual?>
				<input type=radio name=ppp_conn_mode id=ppp_conn_mode value=2 onclick="chg_ppp_conn_mode(this.form);"><?=$m_on_demand?>
				<input type=hidden name=ppp_auto id=ppp_auto>
				<input type=hidden name=ppp_ondemand id=ppp_ondemand>
			</td>
		</tr>
		</table>
		</form>
		</div>

	<!-- BigPond  -->
		<div class="box" id="show_bigpond" style="display:none">
		<h2><?=$m_title_bigpond?></h2>
		<form name="frm_bigpond" id="frm_bigpond" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
		<input type="hidden" name="ACTION_POST" value="BIGPOND">
		<p><?=$m_desc_bigpond?></p>
		<table cellSpacing=1 cellPadding=1 width=525 border=0>
		<tr>
			<td class="r_tb" width=150><?=$m_user_name?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=username name=username size=30 maxlength=63 value="<?=$cfg_bp_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_password?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password name=password size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_retype_pwd?> :</td>
			<td class="l_tb">&nbsp;
				<input type=password id=password_v name=password_v size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_auth_server?> :</td>
			<td class="l_tb">&nbsp;
				<select id="authserver" name="authserver">
					<option value="sm-server"<?if($cfg_bp_server=="sm-server") {echo " selected";}?>>sm-server</option>
					<option value="dce-server"<?if($cfg_bp_server=="dce-server"){echo " selected";}?>>dce-server</option>
				</select>
			</td>
		</tr>
		<tr>
			<td class="r_tb" width=150><?=$m_login_server?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=login_server name=login_server size=32 maxlength=32 value="<?=$cfg_bp_loginserver?>">&nbsp;<?=$m_optional?>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><?=$m_macaddr?> :</td>
			<td class="l_tb">&nbsp;
				<input type=text id=mac1 name=mac1 size=2 maxlength=2 value=""> -
				<input type=text id=mac2 name=mac2 size=2 maxlength=2 value=""> -
				<input type=text id=mac3 name=mac3 size=2 maxlength=2 value=""> -
				<input type=text id=mac4 name=mac4 size=2 maxlength=2 value=""> -
				<input type=text id=mac5 name=mac5 size=2 maxlength=2 value=""> -
				<input type=text id=mac6 name=mac6 size=2 maxlength=2 value="">&nbsp;<?=$m_optional?>&nbsp;&nbsp;
				<input type="button" value="<?=$m_clone_mac?>" name="clone" onclick='set_clone_mac("frm_bigpond")'>
			</td>
			<input type=hidden id=clonemac name=clonemac>
		</tr>
		</table>
		</form>
		</div>
<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</body>
</html>
