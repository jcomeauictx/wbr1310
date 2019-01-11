<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wlan_2_sel_sec";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		="2_sel_sec";
$WIZ_PREV		="1_set_ssid";
//$WIZ_NEXT will be assigned again in __wiz_wlan_action.php
//$WIZ_NEXT		="3_set_sec"; 
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
$db_sec_type= query($G_WIZ_PREFIX_WLAN."/wireless/authentication");
if($db_sec_type=="")
{
	$db_sec_type=query("/wireless/authentication");
	$db_wep_type=query("/wireless/wpa/wepmode");
}
else
{
	$db_wep_type=query($G_WIZ_PREFIX_WLAN."/wireless/wpa/wepmode");
}
if($db_sec_type==""){$db_sec_type=0;}
if($db_wep_type==""){$db_wep_type=0;}
/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	var db_sec_type=<?=$db_sec_type?>;
	var db_wep_type=<?=$db_wep_type?>;
	if	   (db_sec_type==5)	f.sec_type[0].checked=true;
	else if(db_sec_type==3)	f.sec_type[1].checked=true;
	else if(db_wep_type==1)	f.sec_type[2].checked=true;
	else 					f.sec_type[3].checked=true;
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
			<table>
			<tr>
				<td width=25% class=r_tb><?=$m_best?></td>
				<td width=10><input type=radio name=sec_type value=5></td>
				<td><?=$m_best_dsc?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_better?></td>
				<td width=10><input type=radio name=sec_type value=3></td>
				<td><?=$m_better_dsc?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_good?></td>
				<td width=10><input type=radio name=sec_type value=1></td>
				<td><?=$m_good_dsc?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_none?></td>
				<td width=10><input type=radio name=sec_type value=0></td>
				<td><?=$m_none_dsc?></td>
			</tr>
			</table>
			<?=$m_note_sel_sec?>
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
