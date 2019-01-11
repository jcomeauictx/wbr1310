<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wan_4_bigpond";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		= "4_bigpond";
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
anchor($G_WIZ_PREFIX_WAN."/bigpond");
$cfg_bp_server		= query("authserver");
$cfg_bp_loginserver	= get(h,"login_server");
$cfg_bp_user		= get(h,"username");
anchor("/wan/rg/inf:1/bigpond");
if ($cfg_bp_user=="")			{$cfg_bp_user = get(h,"username");}
if ($cfg_bp_server=="")			{$cfg_bp_server = query("server");}
if ($cfg_bp_loginserver=="")	{$cfg_bp_loginserver = get(h,"loginserver");}
/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");

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
		<table width="536" align="center">
		<tr>
			<td class="r_tb" width="250"><strong><?=$m_auth_server?>&nbsp;:</strong></td>
			<td class="l_tb">
				<select id="authserver" name="authserver">
				<option value="sm-server"<?if($cfg_bp_server=="sm-server") {echo " selected";}?>>sm-server</option>
				<option value="dce-server"<?if($cfg_bp_server=="dce-server"){echo " selected";}?>>dce-server</option>
				</select>
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_bp_server?>&nbsp;:</strong></td>
			<td class="l_tb">
				<input type=text id=login_server name=login_server size=32 maxlength=15 value="<?=$cfg_bp_loginserver?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_bp_user_name?>&nbsp;:</strong></td>
			<td class="l_tb">
				<input type=text id=username name=username size=32 maxlength=63 value="<?=$cfg_bp_user?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_bp_password?>&nbsp;:</strong></td>
			<td class="l_tb">
				<input type=password id=password name=password size=32 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
			</td>
		</tr>
		<tr>
			<td class="r_tb"><strong><?=$m_bp_verify_pwd?>&nbsp;:</strong></td>
			<td class="l_tb">
				<input type=password id=password_v name=password_v size=32 maxlength=63 value="<?=$G_DEF_PASSWORD?>">
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
