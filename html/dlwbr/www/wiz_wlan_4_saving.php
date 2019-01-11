<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wlan_4_saving";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		="4_saving";
anchor($G_WIZ_PREFIX_WLAN."/wireless/");
if(query("authentication")=="0" && query("wpa/wepmode")=="0")	{$WIZ_PREV	="2_sel_sec";}
else															{$WIZ_PREV	="3_set_sec";}
$WIZ_NEXT		="5_done";
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
anchor($G_WIZ_PREFIX_WLAN."/wireless/");
$ssid		=get("h","ssid");
$auth		=query("authentication");
$wepmode	=query("wpa/wepmode");
/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	// init here ...
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	// do check here ....
	return true;
}
function go_prev()
{
	self.location.href="<?=$POST_ACTION?>?TARGET_PAGE=<?=$WIZ_PREV?>";
}
function do_save()
{
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
		<? require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php"); ?>
<!-- ________________________________ Main Content Start ______________________________ -->
		<br>
		<table width=95%>
			<tr>
				<?
				$td_h="22";
				$td_attr="class=br_tb height=".$td_h;
				?>
				<td class=br_tb width=40% height="<?=$td_h?>"><?=$m_wlan_ssid?> :</td>
				<td width="5"></td>
				<td>&nbsp;<?=$ssid?></td>
			</tr>
<?
if($auth=="0" && $wepmode!="0")
{
	echo "<tr><td ".$td_attr.">".$m_wep_length." :</td><td></td><td>".$m_128bits."</td></tr>\n";
	echo "<tr><td ".$td_attr.">".$m_def_wep_index." :</td><td></td><td>1</td></tr>\n";
	echo "<tr><td ".$td_attr.">".$m_auth." :</td><td></td><td>".$m_open."</td></tr>\n";
	echo "<tr><td ".$td_attr.">".$m_wep_key." :</td><td></td><td>".query("full_secret")."</td></tr>\n";
}
else if($auth=="3" || $auth=="5")
{
	echo "<tr><td ".$td_attr.">".$m_encry." :</td><td></t>";
	if($auth=="3")	{echo "<td>".$m_wpa_psk."</td></tr>\n";}
	else			{echo "<td>".$m_wpa2_psk."</td></tr>\n";}
	echo "<tr><td ".$td_attr.">".$m_psk." :</td><td></td><td>".query("full_secret")."</td></tr>\n";
}

?>
		</table>
		<br>
		<center><script>prev("");wiz_save();exit();</script></center>
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
