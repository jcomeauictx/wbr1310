<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="st_device";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="st";
/* --------------------------------------------------------------------------- */
anchor("/runtime/wan/inf:1");
if(query("connectstatus")!="connected")
{
	if ($refresh=="") {$refresh=0;}
	if ($refresh<3)
	{
		$refresh++;
		$OTHER_META="<META HTTP-EQUIV=Refresh CONTENT=\"3;URL=st_device.php?refresh=".$refresh."\">";
	}
}
/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
if(query("/lan/dhcp/server/enable")==1) {$dhcp_en=$m_enabled;}
else                                    {$dhcp_en=$m_disabled;}

anchor("/runtime/wan/inf:1");
if (query("connectstatus") == "connected")
{
	$wan_status = $m_connected;
	$wanipaddr	= query("ip");
	$wansubnet	= query("netmask");
	$wangateway	= query("gateway");
	$wandns		= query("primarydns")."&nbsp;".query("secondarydns");
}
else
{
	$wan_status = $m_disconnected;
	$wanipaddr	= $m_null_ip;
	$wansubnet	= $m_null_ip;
	$wangateway	= $m_null_ip;
	$wandns		= $m_null_ip;
}

$wanmode = query("/wan/rg/inf:1/mode");
if ($wanmode == 1)
{
	$msg_wanmode	= $m_static_ip;
	$msg_wanstatus	= "";
	$msg_connect	= "";
	$msg_disconnect	= "";
}
else if ($wanmode == 2)
{
	$msg_wanmode	= $m_dhcp_client;
	$msg_wanstatus	= $wan_status;
	$msg_connect	= $m_dhcp_renew;
	$msg_disconnect	= $m_dhcp_release;
}
else if ($wanmode == 3)
{
	$msg_wanmode	= $m_pppoe;
	$msg_wanstatus	= "";
	$msg_connect	= $m_connect;
	$msg_disconnect	= $m_disconnect;
}
else if ($wanmode == 4)
{
	$msg_wanmode	= $m_pptp;
	$msg_wanstatus	= $wan_status;
	$msg_connect	= $m_connect;
	$msg_disconnect	= $m_disconnect;
}
else if ($wanmode == 5)
{
	$msg_wanmode	= $m_l2tp;
	$msg_wanstatus	= $wan_status;
	$msg_connect	= $m_connect;
	$msg_disconnect	= $m_disconnect;
}
else if ($wanmode == 7)
{
	$msg_wanmode	= $m_bigpond;
	$msg_wanstatus	= $wan_status;
	$msg_connect	= $m_dhcp_renew;
	$msg_disconnect	= $m_dhcp_release;
}

$router=query("/runtime/router/enable");
/* wireless part */
$no_jumpstart=query("/function/no_jumpstart");
anchor("/wireless");
$wlan_en	= query("enable");
$wep_len	= query("keylength");
$js_enable	= query("jumpstart/enable");
$js_phase	= query("jumpstart/phase");

if($js_enable==1 && $js_phase==1)	{$ssid=get(h,"/runtime/wireless/ssid");}
else								{$ssid=get(h,"ssid");}

if($js_enable == 1)	{$jumpstart=$m_enabled;  $jsstatus=$m_protocol."&nbsp;".$js_phase;}
else				{$jumpstart=$m_disabled; $jsstatus=$m_disabled;}

$str_sec = "";
$auth	= query("authentication");
$sec	= query("wpa/wepmode");

if (query("autochannel")!=1 || $wlan_en==0)	{$channel = query("channel");}
else {$channel = query("/runtime/stats/wireless/channel");}

if ($js_enable==1 && $js_phase==1)	{$str_sec = $str_sec."&nbsp;".$m_disabled;}
else
{
	if		($sec==1)   {$str_sec = $str_sec."&nbsp;".$wep_len."&nbsp;".$m_bits; }
	else if	($sec==2)   {$str_sec = $str_sec."&nbsp;".$m_tkip; }
	else if	($sec==3)   {$str_sec = $str_sec."&nbsp;".$m_aes; }
	else if	($sec==4)   {$str_sec = $str_sec."&nbsp;".$m_cipher_auto; }
	else				{$str_sec = $str_sec."&nbsp;".$m_disabled; }
}
?>

<script>

function do_connect()
{
    self.location.href="<?=$MY_NAME?>.xgi?set/runtime/wan/inf:1/connect=1"
}

function do_disconnect()
{
    self.location.href="<?=$MY_NAME?>.xgi?set/runtime/wan/inf:1/disconnect=1"
}

/* page init functoin */
function init()
{
		<?	if($router!="1"){echo "get_obj('show_lan').style.display = 'none';\n";}	?>
}
/* parameter checking */
function check()
{
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}

</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php">
<input type="hidden" name="ACTION_POST" value="SOMETHING">
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
		<p align="center"><strong>
			<?=$m_fw_ver?>&nbsp;:&nbsp;<?query("/runtime/sys/info/firmwareversion");?>&nbsp;,
			&nbsp;<?query("/runtime/sys/info/firmwarebuildate");?>
		</strong></p>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
		<div class="box" id="show_lan" style="display:">
			<h2><?=$m_lan?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td class="r_tb" width="200"><?=$m_macaddr?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?query("/runtime/sys/info/lanmac");?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_ipaddr?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?query("/lan/ethernet/ip");?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_netmask?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?query("/lan/ethernet/netmask");?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_dhcp_server?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$dhcp_en?></td>
			</tr>
			</table>
		</div>
		<div class="box">
			<h2><?if($router!="1"){echo $m_wired;}else{echo $m_wan;}?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td class="r_tb" width="198"><?=$m_macaddr?>&nbsp;:</td>
				<td class="l_tb" width=320>&nbsp;<?query("/runtime/wan/inf:1/mac");?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_connection?>&nbsp;:</td>
				<td class="l_tb">&nbsp;
				<?
					echo $msg_wanmode."&nbsp;".$msg_wanstatus;
					if ($AUTH_GROUP=="0" && $msg_connect!="" && $router=="1")
					{
						echo "<br>\n";
						echo "<input type=button onclick=do_connect(); value=\"".$msg_connect."\">&nbsp;\n";
						echo "<input type=button onclick=do_disconnect(); value=\"".$msg_disconnect."\">\n";
					}
				?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_ipaddr?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$wanipaddr?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_netmask?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$wansubnet?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_default_gw?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$wangateway?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_dns?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$wandns?></td>
			</tr>
			</table>
		</div>
<?if($no_jumpstart=="1"){echo "<!--\n";}?>
		<div class="box">
			<h2><?=$m_wireless_jumpstart?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td class="r_tb" width="200"><?=$m_jumpstart?>&nbsp;:&nbsp;</td>
				<td class="l_tb"><?=$jumpstart?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_status?>&nbsp;:&nbsp;</td>
				<td class="l_tb"><?=$jsstatus?></td>
			</tr>
			</table>
		</div>
<?if($no_jumpstart=="1"){echo "-->\n";}?>
		<div class="box">
			<h2><?=$m_wlan?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td class="r_tb" width="200"><?=$m_ssid?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$ssid?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_channel?>&nbsp;:</td>
				<td class="l_tb">&nbsp;<?=$channel?></td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_privacy?>&nbsp;:</td>
				<td class="l_tb"><?=$str_sec?></td>
			</tr>
			</table>
		</div>
<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
