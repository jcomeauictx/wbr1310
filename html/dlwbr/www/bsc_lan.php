<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="bsc_lan";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="bsc";
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
$model_name=query("/sys/modelname");
if($router=="1")
{
	if ($ACTION_POST!="" || $del_id!="")
	{
		require("/www/model/__admin_check.php");
		
		if($del_id!="")
		{
			del("/lan/dhcp/server/pool:1/staticdhcp/entry:".$del_id);
			$SUBMIT_STR=";submit DHCPD";
		}
		else
		{
			if ($dnsr == "enable")	{ $dnsr = "2"; } else { $dnsr="1"; }	/* 1:disable, 2:auto */
			if ($dhcpsvr != "1")	{ $dhcpsvr = "0"; }
			$lan_dirty = 0; $dnsr_dirty = 0; $dhcp_dirty = 0;

			/* Check LAN config */
			anchor("/lan/ethernet");
			if (query("ip")!= $ipaddr)			{$lan_dirty++; set("ip", $ipaddr);}
			if (query("netmask")!=$netmask)		{$lan_dirty++; set("netmask", $netmask);}

			/* Check DNS relay config */
			if (query("/dnsrelay/mode")!=$dnsr)	{$dnsr_dirty++; set("/dnsrelay/mode", $dnsr);}

			/* Check DHCP server config */
			anchor("/lan/dhcp/server");
			if (query("enable")!=$dhcpsvr)		{$dhcp_dirty++; set("enable", $dhcpsvr);}
			anchor("/lan/dhcp/server/pool:1");
			if (query("domain")!=$domain)		{$dhcp_dirty++; set("domain", $domain);}
			if ($dhcpsvr == "1")
			{
				if (query("startip")!=$startipaddr)		{$dhcp_dirty++; set("startip",	$startipaddr);}
				if (query("endip")!=$endipaddr)			{$dhcp_dirty++; set("endip",	$endipaddr);}
				if (query("leasetime")!=$lease_seconds)	{$dhcp_dirty++; set("leasetime",$lease_seconds);}
			}
			
			/* ----------------------------- static dhcp --------------------------------------- */
			if($r_name!="" && query("/sys/modelname")=="WBR-2310")
			{
				echo "<!--\n";
				if($r_id!="")
				{
					anchor("/lan/dhcp/server/pool:1/staticdhcp/entry:".$r_id);
					echo "save_id=".$r_id."\n";
				}
				else
				{
					$static_num=1;
					for("/lan/dhcp/server/pool:1/staticdhcp/entry"){$static_num++;}
					$save_id=$staic_num+1;
					anchor("/lan/dhcp/server/pool:1/staticdhcp/entry:".$static_num);
					echo "save_id=".$static_num."\n";
				}
				echo "mac=".$r_mac."\n";
				echo "-->\n";
				if($r_en!="1"){$r_en=0;}
				if(query("enable")!=$r_en)		{$dhcp_dirty++;	set("enable",	$r_en);}
				if(query("hostname")!=$r_name)	{$dhcp_dirty++;	set("hostname",	$r_name);}
				if(query("ip")!=$r_ip)			{$dhcp_dirty++; set("ip",		$r_ip);}
				if(query("mac")!=$r_mac)		{$dhcp_dirty++; set("mac",		$r_mac);}
			}
			/* ------------------------------------------------------------------------------- */
			
			/* Check dirty */
			$SUBMIT_STR="";
			if	($on_lan_change=="1")	{$SUBMIT_STR=$SUBMIT_STR.";submit LAN_CHANGE";}
			if		($lan_dirty > 0)	{$SUBMIT_STR=$SUBMIT_STR.";submit LAN";}
			else if	($dhcp_dirty > 0)	{$SUBMIT_STR=$SUBMIT_STR.";submit DHCPD";}
			if ($dnsr_dirty > 0)		{$SUBMIT_STR=$SUBMIT_STR.";submit DNSR";}
		}
		
		$NEXT_PAGE=$MY_NAME;
		if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
		else				{require($G_NO_CHANGED_URL);}
	}
}
/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
$cfg_ipaddr		= query("/lan/ethernet/ip");
$cfg_netmask	= query("/lan/ethernet/netmask");
$cfg_domain		= get(h,"/lan/dhcp/server/pool:1/domain");
$cfg_dnsr		= query("/dnsrelay/mode");
$cfg_dhcpsvr	= query("/lan/dhcp/server/enable");
$cfg_startip	= query("/lan/dhcp/server/pool:1/startip");
$cfg_endip		= query("/lan/dhcp/server/pool:1/endip");
$cfg_leasetime	= query("/lan/dhcp/server/pool:1/leasetime");
/* --------------------------------------------------------------------------- */
?>

<script>
function on_check_dhcpsvr()
{
	var f = get_obj("frm");
	f.startip.disabled = f.endip.disabled = f.leasetime.disabled = !f.dhcpsvr.checked;
}

function print_daytime(str_second)
{
	var time = second_to_daytime(str_second);
	var str =	(time[0]>0 ? time[0]+" <?=$m_days?> " : "") +
				(time[1]>0 ? time[1]+" <?=$m_hrs?> " : "") +
				(time[2]>0 ? time[2]+" <?=$m_mins?> " : "") +
				(time[3]>0 ? time[3]+" <?=$m_secs?> " : "");
	document.write(str);
}

/* page init functoin */
function init()
{
	var f=get_obj("frm");
	var ipaddr;
	// init here ...
	f.ipaddr.value	= "<?=$cfg_ipaddr?>";
	f.netmask.value	= "<?=$cfg_netmask?>";
	f.domain.value	= "<?=$cfg_domain?>";
	f.dnsr.checked	= <? if ($cfg_dnsr=="1") {echo "false";} else {echo "true";} ?>;

	f.dhcpsvr.checked	= <? if ($cfg_dhcpsvr == "1") {echo "true";} else {echo "false";} ?>;
	ipaddr				= get_ip("<?=$cfg_startip?>");
	f.startip.value		= ipaddr[4];
	ipaddr				= get_ip("<?=$cfg_endip?>");
	f.endip.value		= ipaddr[4];
	f.leasetime.value	= "<?$min_leasetime=$cfg_leasetime/60; echo $min_leasetime;?>";

	on_check_dhcpsvr();
/* -------------------------------- static dhcp start -------------------------------------- */
<?
if(query("/sys/modelname")=="WBR-2310")
{
	echo "get_obj('static_dhcp').style.display = '';\n";
	echo "get_obj('static_dhcp_list').style.display = '';\n";
}
if($edit_id!="")
{
	echo "	var tmp_ip		= get_ip(s_list[".$edit_id."][3]);\n";
	echo "	var tmp_mac		= get_mac(s_list[".$edit_id."][4]);\n";
	echo "	if(s_list[".$edit_id."][1]=='1')	f.r_en.checked = s_list[".$edit_id."][1];\n";
	echo "	f.r_name.value	= s_list[".$edit_id."][2];\n";
	echo "	f.r_ip4.value	= tmp_ip[4];\n";
	echo "	for(i=1; i<=6; i++) eval('f.mac'+i+'.value=tmp_mac['+i+']');\n";
	echo "	self.location.href='#add_client';\n";
	echo "	f.r_name.focus();\n";
}
?>
/* -------------------------------- static dhcp end   -------------------------------------- */
	<?if($router!="1"){echo "fields_disabled(f, true);\n";}?>
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}
/* parameter checking */
function check()
{
	// do check here ....
	var f = get_obj("frm");
	var val, min, max;
	var ip, tmp_ip, tmp_mac;

	if (is_valid_ip(f.ipaddr.value, 0)==false)
	{
		alert("<?=$a_invalid_ip?>");
		field_focus(f.ipaddr, "**");
		return false;
	}
	if (is_valid_mask(f.netmask.value)==false)
	{
		alert("<?=$a_invalid_netmask?>");
		field_focus(f.netmask, "**");
		return false;
	}
	if (!is_blank(f.domain.value) && strchk_hostname(f.domain.value)==false)
	{
		alert("<?=$a_invalid_domain_name?>");
		field_focus(f.domain, "**");
		return false;
	}

	if (f.dhcpsvr.checked)
	{
		ip = get_ip(f.ipaddr.value);
		min = parseInt(f.startip.value, [10]);
		max = parseInt(f.endip.value, [10]);

		if (!is_digit(f.startip.value))
		{
			alert("<?=$a_invalid_ip_range?>");
			field_focus(f.startip, "**");
			return false;
		}
		if(!is_digit(f.endip.value))
		{
			alert("<?=$a_invalid_ip_range?>");
			field_focus(f.endip, "**");
			return false;
		}
		if(!is_digit(f.leasetime.value))
		{
			alert("<?=$a_invalid_lease_time?>");
			field_focus(f.leasetime, "**");
			return false;
		}

		if (!is_in_range(min, 1, 254) || !is_in_range(max, 1, 254) || min > max || is_in_range(ip[4], min, max))
		{
			alert("<?=$a_invalid_ip_range?>");
			field_focus(f.startip, "**");
			return false;
		}
		if(is_blank(f.leasetime.value)||f.leasetime.value==0)
		{
			alert("<?=$a_invalid_lease_time?>");
			field_focus(f.leasetime, "**");
			return false;
		}
	}

	ip = get_ip(f.ipaddr.value);
	f.startipaddr.value = ip[1]+"."+ip[2]+"."+ip[3]+"."+f.startip.value;
	f.endipaddr.value = ip[1]+"."+ip[2]+"."+ip[3]+"."+f.endip.value;
	f.lease_seconds.value = f.leasetime.value * 60;
/* -------------------------------- static dhcp start -------------------------------------- */
	if("<?=$edit_id?>"!="" || f.r_en.checked || !is_blank(f.r_name.value) || !is_blank(f.r_ip4.value)
		|| !is_blank(f.mac1.value) || !is_blank(f.mac2.value) || !is_blank(f.mac3.value)
		|| !is_blank(f.mac4.value) || !is_blank(f.mac5.value) || !is_blank(f.mac6.value) )
	{
		if (is_blank(f.r_name.value) || strchk_hostname(f.r_name.value)==false)
		{
			alert("<?=$a_invalid_computer_name?>");
			field_focus(f.r_name, "**");
			return false;
		}
		var lan_ip=get_ip("<?=$cfg_ipaddr?>");
		if(is_blank(f.r_ip4.value)|| !is_digit(f.r_ip4.value))
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.r_ip4,"**")
			return false;
		}
		if(f.r_ip4.value==lan_ip[4])
		{
			alert("<?=$a_same_with_lan_ip?>");
			field_focus(f.r_ip4,"**")
			return false;
		}
		for(i=1;i<=6;i++)
		{
			tmp_mac=eval("f.mac"+i+".value");
			if(is_blank(tmp_mac) || !is_valid_mac(tmp_mac))
			{
				alert("<?=$a_invalid_mac?>");
				eval("f.mac"+i+".focus()");
				return false;
			}
			if (tmp_mac.length == 1) eval("f.mac"+i+".value= '0'+tmp_mac");
		}
		tmp_mac = f.mac1.value+":"+f.mac2.value+":"+f.mac3.value+":"+f.mac4.value+":"+f.mac5.value+":"+f.mac6.value;
		
		for(i=1;i<s_list.length;i++)
		{
			if("<?=$edit_id?>"!=i)
			{
				tmp_ip	= get_ip(s_list[i][3]);
				if(f.r_name.value==s_list[i][2])
				{
					alert("<?=$a_same_static_hostname?>");
					field_focus(f.r_name, "**");
					return false;
				}
				if(f.r_ip4.value==tmp_ip[4])
				{
					alert("<?=$a_same_static_ip?>");
					field_focus(f.r_ip4, "**");
					return false;
				}
				if(tmp_mac==s_list[i][4])
				{
					alert("<?=$a_same_static_mac?>");
					field_focus(f.mac1, "**");
					return false;
				}
			}
		}
		<?if($edit_id!=""){echo "f.r_id.value=".$edit_id.";\n";}?>
		if(f.r_en.checked)	f.r_en.value="1";
		f.r_ip.value=f.r_ip1.value+"."+f.r_ip2.value+"."+f.r_ip3.value+"."+f.r_ip4.value;
		f.r_mac.value=tmp_mac;
	}
/* -------------------------------- static dhcp end   -------------------------------------- */
	var net1, net2;
	net1 = get_network_id(f.ipaddr.value, f.netmask.value);
	net2 = get_network_id("<?=$cfg_ipaddr?>", f.netmask.value);
	if(net1[0]!=net2[0])	f.on_lan_change.value="1";
	
	return true;
}
/* -------------------------------- static dhcp start -------------------------------------- */
var d_list=[['index','hostname','ip','mac']<?
for("/runtime/dhcpserver/lease")
{
	echo ",\n['".$@."','".get("j","hostname")."','".query("ip")."','".query("mac")."']";
}
?>];
var s_list=[['index','enable','hostname','ip','mac']<?
for("/lan/dhcp/server/pool:1/staticdhcp/entry")
{
	echo ",\n['".$@."','".query("enable")."','".get("j","hostname")."','".query("ip")."','".query("mac")."']";
}
?>];

function cp_computer_info()
{
	var f			= get_obj("frm");
	var tmp_ip		= get_ip(d_list[f.dhcp.value][2]);
	var tmp_mac		= get_mac(d_list[f.dhcp.value][3]);
	f.r_name.value= d_list[f.dhcp.value][1];
	f.r_ip4.value	= tmp_ip[4];
	for(i=1;i<7;i++)	eval("f.mac"+i+".value=tmp_mac["+i+"]");
}
function sync_pc_mac()
{
	var f		= get_obj("frm");
	var tmp_mac	= get_mac("<?=$macaddr?>");
	for(i=1;i<7;i++)	eval("f.mac"+i+".value=tmp_mac["+i+"]");
}
function print_dhcp_sel(n)
{
	var str="<select name="+n+" size=1>";
	str+="<option value='-1'><?=$m_computer_name?></option>";
	for(i=1;i<d_list.length;i++)
	{
		str+="<option value='"+d_list[i][0]+"'>"+d_list[i][1]+"</option>";
	}
	str+="</select>";
	document.write(str);
}
function print_pre_ip(n)
{
	var str="";
	var lan_ip=get_ip("<?=$cfg_ipaddr?>");
	for(i=1;i<4;i++)
		str+="<input type=text readonly name="+n+i+" size=3 maxlength=3 value="+lan_ip[i]+" style='border:0;width=25px;text-align:center'>.";
	document.write(str);
}
function print_mac(n)
{
	var str="";
	for(i=1;i<7;i++)
	{
		str+="<input type=text name="+n+i+" size=1 maxlength=2 value=''>";
		if(i!=6)        str+=" : ";
	}
	document.write(str);
}
function print_edit_del(id)
{
	var str="";
	//edit
	str="<a href='<?=$MY_NAME?>.php?edit_id="+id+"'><img src='/pic/edit.jpg' border=0></a>";
	str+="&nbsp;&nbsp;";

	//del
	str+="<a href='javascript:del_confirm(\""+id+"\")'><img src='/pic/delete.jpg' border=0></a>";

	document.write(str);
}
function del_confirm(id)
{
	if(confirm("<?=$a_del_confirm?>")==false) return;
	self.location.href="<?=$MY_NAME?>.php?del_id="+id;
}
/* -------------------------------- static dhcp end   -------------------------------------- */
</script>


<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
<input type="hidden" name="ACTION_POST" value="SOMETHING">
<input type="hidden" name="lease_seconds" value="">
<?require("/www/model/__banner.php");?>
<?require("/www/model/__menu_top.php");?>
<table <?=$G_MAIN_TABLE_ATTR?> height="100%">
<tr valign=top>
	<td <?=$G_MENU_TABLE_ATTR?>>
	<?require("/www/model/__menu_left.php");?>
	</td>
	<td id="maincontent">
		<div id="box_header">
		<?
		require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php");
		echo $G_APPLY_CANEL_BUTTON;
		?>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
		<div class="box">
			<h2><?=$m_title_router_setting?></h2>
			<?=$m_desc_router_setting?><br><br>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td class="r_tb" width="200"><?=$m_router_ipaddr?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="ipaddr" type="text" id="ipaddr" size="20" maxlength="15" value="">
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_subnet_mask?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="netmask" type="text" id="netmask" size="20" maxlength="15" value="">
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_domain_name?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="domain" type="text" id="domain" size="40" maxlength="30" value="">
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_enable_dnsr?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="dnsr" type=checkbox id="dnsr" value="enable">
				</td>
			</tr>
			</table>
		</div>
		<div class="box">
			<h2><?=$m_title_dhcp_svr?></h2>
			<?=$m_desc_dhcp_svr?><br><br>
			<table width="525" border=0 cellPadding=1 cellSpacing=1>
			<tr>
				<td class="r_tb" width="200"><?=$m_enable_dhcp?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="dhcpsvr" type=checkbox id="dhcpsvr" onClick="on_check_dhcpsvr()" value="1">
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_dhcp_range?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input name="startip" type="text" id="startip" size="3" maxlength="3" value="">
					<input name="startipaddr" type="hidden" id="startipaddr" value="">
					&nbsp;<?=$m_to?>&nbsp;
					<input name="endip" type="text" id="endip" size="3" maxlength="3" value="">
					<input name="endipaddr" type="hidden" id="endipaddr" value="">
					<?=$m_range_comment?>
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_lease_time?> :</td>
				<td class="l_tb">&nbsp;&nbsp;
					<input type="text" id="leasetime" name="leasetime" size="6" maxlength="6" value="">
					<?=$m_minutes?>
				</td>
			</tr>
			</table>
		</div>
	<!-- ________________________________ add static DHCP client start ______________________________ -->
		<input type=hidden name=r_id value="">
		<input name="r_ip" type="hidden" value="">
		<input name="r_mac" type="hidden" value="">
		<input name="on_lan_change" type="hidden" value="">
		<a name="add_client"></a>
		<div class="box" name="static_dhcp" id="static_dhcp" style="display:none">
			<h2><?=$m_title_add_client?></h2>
			<table width="525" border=0 cellPadding=1 cellSpacing=1>
			<tr>
				<td class="r_tb" width="200"><?=$m_enable?> :</td>
				<td class="l_tb"><input type=checkbox name="r_en"></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_computer_name?> :</td>
				<td class="l_tb">
				<input type=text name="r_name" size="19" maxLength=19>
				<input type=button name="bt" value="<<" style="width: 24; height: 24" onClick="cp_computer_info()">
				<script>print_dhcp_sel("dhcp");</script>
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_ipaddr?> :</td>
				<td class="l_tb">
				<script>print_pre_ip("r_ip");</script>
				<input type=text name=r_ip4 size=3 maxlength=3>
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_macaddr?> :</td>
				<td class="l_tb"><script>print_mac("mac");</script></td>
			</tr>
			<tr>
				<td class="r_tb"></td>
				<td class="l_tb"><input type=button name=clone_mac value="<?=$m_clone_mac?>" onclick="sync_pc_mac()"></td>
			</tr>
			</table>
		</div>
		<div class="box" name="static_dhcp_list" id="static_dhcp_list" style="display:none">
			<h2><?=$m_title_static_client_list?></h2>
			<table width="525" border=0 cellPadding=1 cellSpacing=1>
			<tr>
				<td class="c_tb"><?=$m_enable?></td>
				<td class="c_tb"><?=$m_computer_name?></td>
				<td class="c_tb"><?=$m_macaddr?></td>
				<td class="c_tb"><?=$m_ipaddr?></td>
				<td></td>
			</tr>
<?
for("/lan/dhcp/server/pool:1/staticdhcp/entry")
{
	$ck_str=" disabled";
	if(query("enable")=="1"){$ck_str=" checked".$ck_str;}
	if($edit_id==$@){echo "<tr bgcolor=yellow>\n";}
	else{	echo "		<tr>\n";}
	echo "			<td class='c_tb'><input type=checkbox name=ck_en".$@.$ck_str."></td>\n";
	echo "			<td class='c_tb'>".get("h","hostname")."</td>\n";
	echo "			<td class='c_tb'>".query("mac")."</td>\n";
	echo "			<td class='c_tb'>".query("ip")."</td>\n";
	echo "			<td class='c_tb'><script>print_edit_del(".$@.");</script></td>\n";
	echo "		</tr>";
}
?>
			</table>
		</div>
	<!-- ________________________________ add static DHCP client end ___________________________________ -->

		<div class="box">
			<h2><?=$m_title_client_list?></h2>
			<table width="525" border=0 cellPadding=1 cellSpacing=1>
			<tr>
				<td class="l_tb"><?=$m_host_name?></td>
				<td class="l_tb"><?=$m_ipaddr?></td>
				<td class="l_tb"><?=$m_macaddr?></td>
				<td class="l_tb"><?=$m_expired_time?></td>
				<td></td>
			</tr>
			<tr><td></td><td></td><td></td></tr>
<?
for ("/runtime/dhcpserver/lease")
{
echo
"			<tr>\n".
"				<td class=\"l_tb\">".get(h,"hostname")."</td>\n".
"				<td class=\"l_tb\">".query("ip")."</td>\n".
"				<td class=\"l_tb\">".query("mac")."</td>\n".
"				<td class=\"l_tb\"><script>print_daytime(\"".query("expire")."\");</script></td>\n".
"			</tr>\n";
}
?>
			</table>
		</div>
		<input type=hidden name=test>
<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
