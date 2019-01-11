<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wlan_1_set_ssid";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		="1_set_ssid";
$WIZ_PREV		="0_flowchart";
$WIZ_NEXT		="2_sel_sec";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	require("/www/__wiz_wlan_action.php");
	$ACTION_POST="";
	require("/www/wiz_wlan_".$WIZ_NEXT.".php");
	exit;
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
$db_ssid_path=$G_WIZ_PREFIX_WLAN."/wireless/ssid";
$db_ssid=get("j",$db_ssid_path);
if($db_ssid=="") {$db_ssid=get("j","/wireless/ssid");}
/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	f.ssid.value="<?=$db_ssid?>";
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(is_blank(f.ssid.value)==true)
	{
		alert("<?=$a_empty_ssid?>");
		f.ssid.focus();
		return false;
	}
	if(strchk_unicode(f.ssid.value)==true)
	{
		alert("<?=$a_invalid_ssid?>");
		f.ssid.select();
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
<form name="frm" id="frm" method="post" action="<?=$POST_ACTION?>" OnSubmit="return check();">
<input type="hidden" name="ACTION_POST" value="<?=$MY_ACTION?>">
<input type="hidden" name="TARGET_PAGE" value="<?=$MY_ACTION?>">
<?require("/www/model/__banner.php");?>
<table <?=$G_MAIN_TABLE_ATTR?>>
<tr valign=top>
	<td width=10%></td>
	<td id="maincontent" width=80%>
		<br>
		<div id="box_header">
		<? require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php"); ?>
<!-- ________________________________ Main Content Start ______________________________ -->
		<br>
		<table width=95%>
		<tr>
			<td class=br_tb width=40%><?=$m_wlan_ssid?></td>
			<td><input type="text" name="ssid" size="20" maxlength="32"></td>
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
