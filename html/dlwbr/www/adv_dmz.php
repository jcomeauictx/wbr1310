<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="adv_dmz";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="adv";
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
if ($ACTION_POST!="" && $router=="1")
{
	require("/www/model/__admin_check.php");
	echo "<!--\n";
	echo "dmz_enable=".$dmz_enable."\n";
	echo "dmz_ipaddr=".$dmz_ipaddr."\n";
	echo "pptp_enable=".$pptp_enable."\n";
	echo "l2tp_enable=".$l2tp_enable."\n";
	echo "ipsec_enable=".$ipsec_enable."\n";
	
	if ($dmz_enable		!="1") {$dmz_enable="0";}
	if ($pptp_enable	!="1") {$pptp_enable="0";}
	if ($l2tp_enable	!="1") {$l2tp_enable="0";}
	if ($ipsec_enable	!="1") {$ipsec_enable="0";}
	
	$db_dirty=0;
	$db_dirty2=0;
	anchor("/nat/dmzsrv");
	if(query("dmzaction")	!=$dmz_enable)	{set("dmzaction", $dmz_enable);	$db_dirty++;}
	if ($dmz_enable == "1")
	{
		if(query("ipaddress")!=$dmz_ipaddr)	{set("ipaddress", $dmz_ipaddr); $db_dirty++;}
	}
	
	anchor("/nat/passthrough");
	if(query("pptp")	!=$pptp_enable)		{set("pptp",  $pptp_enable);	$db_dirty2++;}
	if(query("l2tp")	!=$l2tp_enable)		{set("l2tp",  $l2tp_enable);	$db_dirty2++;}
	if(query("ipsec")	!=$ipsec_enable)	{set("ipsec", $ipsec_enable);	$db_dirty2++;}
	
    $SUBMIT_STR="";
	if($db_dirty > 0)	{$SUBMIT_STR=$SUBMIT_STR.";submit RG_DMZ";}
	if($db_dirty2 > 0)	{$SUBMIT_STR=$SUBMIT_STR.";submit RG_MISC";}

	echo "SUBMIT_STR=".$SUBMIT_STR."\n";
	echo "-->\n";

	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
$cfg_pptp = query("/nat/passthrough/pptp");
$cfg_l2tp = query("/nat/passthrough/l2tp");
$cfg_ipsec = query("/nat/passthrough/ipsec");
$cfg_dmz_enable = query("/nat/dmzsrv/dmzaction");
$cfg_dmz_ipaddr = query("/nat/dmzsrv/ipaddress");
$dhcp_sum = query("/runtime/dhcpserver/lease");
/* --------------------------------------------------------------------------- */
?>

<script>

function on_click_enable()
{
	var f = get_obj("frm");

	f.dmz_ipaddr.disabled = f.copy_ip.disabled = f.dhcp.disabled =
	f.dmz_enable.checked ? false : true;
}

function copy_ipaddr()
{
    var IP = get_obj("dmz_ipaddr");
    var dhcp = get_obj("dhcp");
	
	if (dhcp.value == 0)
	{
		alert("<?=$a_no_ip_selected?>");
		return false;
	}
	IP.value = dhcp.value;
}

/* page init functoin */
function init()
{
	var f=get_obj("frm");
	// init here ...
	
	f.dmz_enable.checked = <? if ($cfg_dmz_enable == "1") {echo "true";} else {echo "false";} ?>;
    f.dmz_ipaddr.value = "<?=$cfg_dmz_ipaddr?>";
	
	f.pptp_enable.checked = <? if ($cfg_pptp == "1") {echo "true";} else {echo "false";} ?>;
	f.l2tp_enable.checked = <? if ($cfg_l2tp == "1") {echo "true";} else {echo "false";} ?>;
	f.ipsec_enable.checked = <? if ($cfg_ipsec == "1") {echo "true";} else {echo "false";} ?>;

	on_click_enable();
	
	<?if($router!=1){echo "fields_disabled(f, true);\n";}?>
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	// do check here ....

	if (f.dmz_enable.checked)
	{
		if (is_valid_ip(f.dmz_ipaddr.value, 0)==false)
		{
			alert("<?=$a_invalid_ip?>");
			field_focus(f.dmz_ipaddr, "**");
			return false;
		}
	}
	return true;
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}

</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
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
		<?
		require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php");
		echo $G_APPLY_CANEL_BUTTON;
		?>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
<?
$td_width0="width=\"180\" align=\"right\"";
$td_width1="width=\"160\" align=\"right\"";
$td_width2="width=\"150\"";
?>
		<div class="box">
			<h2><?=$m_title_dmz_rule?></h2>
			<?=$m_desc_dmz?><br><br>
			<?=$m_note_dmz?>
			<table border=0>
			<tr>
				<td <?=$td_width0?>><?=$m_enable_dmz_host?>&nbsp;:</td>
				<td <?=$td_width2?>>&nbsp;<input type="checkbox" name="dmz_enable" value="1" onclick="on_click_enable()"></td>
				<td></td>
			</tr>
			
			<tr>
				<td align="right"><?=$m_ip_addr_dmz?>&nbsp;:</td>
				<td>&nbsp;<input type="text" id="dmz_ipaddr" name="dmz_ipaddr" size=15 maxlength=15></td>
				<td>
					<input id='copy_ip' name='copy_ip' type=button value='<<' class=button onClick='copy_ipaddr()'>
					<select id='dhcp' name='dhcp'>
						<option value=0><?=$m_computer_name?></option selected>
<?
for("/runtime/dhcpserver/lease")
{
echo
"						<option value=\"".query("ip")."\">".get(h,"hostname")."</option>\n";
}
?>					</select>
				</td>
			</tr>

			</table>
		</div>
		<div class="box">
			<h2><?=$m_title_vpn_passthrough?></h2>
			<?=$m_desc_vpn?><br><br>
			<table border=0>
			<tr>
				<td <?=$td_width1?>><?=$m_enable_pptp_passthrough?>&nbsp;:</td>
				<td <?=$td_width2?>>&nbsp;<input type="checkbox" name="pptp_enable" value=1></td>
			</tr>
			<tr>
				<td <?=$td_width1?>><?=$m_enable_l2tp_passthrough?>&nbsp;:</td>
				<td <?=$td_width2?>>&nbsp;<input type="checkbox" name="l2tp_enable" value=1></td>
			</tr>
			<tr>
				<td <?=$td_width1?>><?=$m_enable_ipsec_passthrough?>&nbsp;:</td>
				<td <?=$td_width2?>>&nbsp;<input type="checkbox" name="ipsec_enable" value=1></td>
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
