<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="wiz_wlan_3_set_sec";
$MY_MSG_FILE	=$MY_NAME.".php";

$MY_ACTION		="3_set_sec";
$WIZ_PREV		="2_sel_sec";
$WIZ_NEXT		="4_saving";
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
anchor($G_WIZ_PREFIX_WLAN."/wireless");
$db_secret=query("secret");
$db_auth=query("authentication");

/* --------------------------------------------------------------------------- */
?>

<script>
<?require("/www/md5.js");?>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	f.secret.value="<?=$db_secret?>";
	// init here ...
}
function get_random_char()
{
	var number_list = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	var number = Math.round(Math.random()*62);
	
	return(number_list.substring(number, number + 1));
}
function generate_psk(key)
{
	var i = key.length;
	
	if (key.length < 8)
	{
		for (; i < 8; i++)
		{
			key += get_random_char();
		}
	}
	return key;
}
function create_wep_key128(passpharse, pharse_len)
{
	var pseed2 = "";
	var md5_str = "";
	var count;
	
	
	for(var i = 0; i < 64; i++)
	{
		count = i % pharse_len;
		pseed2 += passpharse.substring(count, count+1);
	}
	
	md5_str = calcMD5(pseed2); 
	
	return md5_str.substring(0, 26).toUpperCase(); 
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(f.secret.value.length < 2)
	{
		alert("<?=$a_invliad_secret?>");
		f.secret.focus();
		return false;
	}
<?
if($db_auth=="0")	// wep key
{
	echo "f.full_secret.value=create_wep_key128(f.secret.value, f.secret.value.length);\n";
}
else						// psk
{
	echo "f.full_secret.value=generate_psk(f.secret.value);\n";
}
?>
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
			<table width=95%>
			<tr>
				<td width=40% class=br_tb><?=$m_wlan_password?></td>
				<td><input type=text name=secret maxlength=20 size=25><br><?=$m_wlan_password_dsc?></td>
				<input type=hidden name="full_secret" value="">
			</tr>
			</table>
			<?=$m_note_set_sec?>
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
