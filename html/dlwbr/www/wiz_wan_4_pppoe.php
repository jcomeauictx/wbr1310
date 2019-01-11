<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wan_4_pppoe";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		= "4_pppoe";
$WIZ_PREV		= "3_sel_wan";
$WIZ_NEXT		= "5_saving";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	require("/www/__wiz_wan_action.php");
	$ACTION_POST="";
	require("/www/wiz_wan_".$WIZ_NEXT.".php");
	exit;
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
$cfg_poe_mode		= query($G_WIZ_PREFIX_WAN."/pppoe/mode");
$cfg_poe_staticip	= query($G_WIZ_PREFIX_WAN."/pppoe/ipaddr");
$cfg_poe_user		= get(h, $G_WIZ_PREFIX_WAN."/pppoe/username");
$cfg_poe_service	= get(h, $G_WIZ_PREFIX_WAN."/pppoe/svc_name");

anchor("/wan/rg/inf:1/pppoe");
if ($cfg_poe_mode=="")		{$cfg_poe_mode		= query("mode");}
if ($cfg_poe_staticip=="")	{$cfg_poe_staticip	= query("staticip");}
if ($cfg_poe_user=="")		{$cfg_poe_user		= get("h","user");}
if ($cfg_poe_service=="")	{$cfg_poe_service	= get("h","acservice");}
/* --------------------------------------------------------------------------- */
?>

<script>
function on_click_ppp_ipmode(form)
{
	var f = get_obj(form);

	f.ipaddr.disabled = f.ipmode[0].checked ? true : false;
}

/* page init functoin */
function init()
{
	var f=get_obj("frm");
	// init here ...
<?
	 if ($cfg_poe_mode != "1")   { echo "	f.ipmode[0].checked = true;\n";}
     else                        { echo "	f.ipmode[1].checked = true;\n";}
?>
	on_click_ppp_ipmode("frm");
}
/* parameter checking */
function check()
{
	var f = get_obj("frm");

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
	}

	return true;
}
function go_prev()
{
	self.location.href="<?=$POST_ACTION?>?TARGET_PAGE=<?=$WIZ_PREV?>";
}
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$POST_ACTION?>" onsubmit="return check();">
<input type="hidden" name="ACTION_POST" value="<?=$MY_ACTION?>">
<input type="hidden" name="TARGET_PAGE" value="<?=$MY_ACTION?>">
<?require("/www/model/__banner.php");?>
<table <?=$G_MAIN_TABLE_ATTR?>>
<tr valign=top>
	<td width=10%></td>
	<td id="maincontent" width=80%>
		<br>
		<div id="box_header">
<!-- ________________________________ Main Content Start ______________________________ -->
		<? require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php"); ?>
		<table>
		<tr>
			<td class="r_tb" width="167"><strong><?=$m_address_mode?> :</strong></td>
			<td class="l_tb" width="443">
				<input type=radio value=0 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm")><?=$m_dynamic_ip?>
				<input type=radio value=1 id=ipmode name=ipmode onclick=on_click_ppp_ipmode("frm")><?=$m_static_ip?>
				<input type=hidden id=mode name=mode>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_ipaddr?> :</strong></td>
			<td class="l_tb">
				<input type=text id=ipaddr name=ipaddr size=16 maxlength=15 value="<?=$cfg_poe_staticip?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_user_name?> :</strong></td>
			<td class="l_tb">
				<input type=text id=username name=username size=30 maxlength=63 value="<?=$cfg_poe_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_password?> :</strong></td>
			<td class="l_tb">
				<input type=password id=password name=password size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_verify_pwd?> :</strong></td>
			<td class="l_tb">
				<input type=password id=password_v name=password_v size=30 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_service_name?> :</strong></td>
			<td class="l_tb">
				<input type=text id=svc_name name=svc_name size=30 maxlength=63 value="<?=$cfg_poe_service?>">&nbsp;<?=$m_optional?>
			</td>
		</tr>
		<tr>
			<td class="r_tb">&nbsp;</td>
			<td class="l_tb"><?=$m_service_name_note?>
			</td>
		</tr>
		</table>
		<br>
		<center><script>prev("");next("");exit();</script></center>
		<br>
<!-- ________________________________  Main Content End _______________________________ -->
		</div>
		<br>
	</td>
	<td width=10%></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
