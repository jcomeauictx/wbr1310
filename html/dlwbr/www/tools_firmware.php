<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="tools_firmware";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="tools";

require("/www/model/__html_head.php");
?>

<script>
/* page init functoin */
function init(){}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(is_blank(f.firmware.value))
	{
		alert("<?=$a_blank_fw_file?>");
		f.firmware.focus();
		return false;
	}
	<?
	if($AUTH_GROUP!="0")
	{
		echo "self.location.href='permission_deny.php?NEXT_LINK=".$MY_NAME.".php';\n";
	}
	else
	{
		echo "return true;\n";
	}
	?>
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}

</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="upload_image._int" enctype=multipart/form-data onsubmit="return check();">
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
		<div class="box">
			<h2><?=$m_fw_title?></h2>
			<table>
			<tr>
				<td class=br_tb width=40%><?=$m_cur_fw_ver?></td>
				<td width="5"></td>
				<td class=l_tb><?query("/runtime/sys/info/firmwareversion");?></td>
			</tr>
			<tr>
				<td class=br_tb><?=$m_cur_fw_date?></td>
				<td></td>
				<td><?query("/runtime/sys/info/firmwarebuildate");?></td>
			</tr>
			<tr>
				<td class=br_tb></td>
				<td></td>
				<td><input type=file name=firmware size=20></td>
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
