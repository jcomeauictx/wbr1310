<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="tools_system";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="tools";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	anchor("/wireless/jumpstart");
	if($ACTION_POST=="enable_js")
	{
		if($en_js!="1")	{$en_js="0";}
		if(query("/wireless/jumpstart/enable")!=$en_js)
		{
			set("/wireless/jumpstart/enable",$en_js);
			set("/wireless/jumpstart/phase", "1");
			$SUBMIT_STR="submit WLAN";
		}
	}
	else if($ACTION_POST=="reset_js")
	{
			set("/wireless/jumpstart/enable","1");
			set("/wireless/jumpstart/phase", "1");
			$SUBMIT_STR="submit WLAN";
	}
echo "<!--\n";
echo "enable=".query("/wireless/jumpstart/enable")."\n";
echo "phase=".query("/wireless/jumpstart/phase")."\n";
echo "ACTION_POST=".$ACTION_POST."\n";
echo "-->\n";
	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.

/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function save_cfg()
{
	self.location.href="../config.bin";
}
function load_cfg(f)
{
	var f=get_obj("configuration");
	if(f.value=="")
	{
		alert("<?=$a_empty_cfg_file_path?>");
		return false;
	}
	if(!confirm("<?=$a_sure_to_reload_cfg?>")) return false;
}
function do_factory_reset()
{
	if(!confirm("<?=$a_sure_to_factory_reset?>")) return;
	var str="/sys_cfg_valid.xgi?";
	str+=exe_str("submit FRESET");
	self.location.href=str;
}
function do_reboot()
{
	if(!confirm("<?=$a_sure_to_reboot?>")) return;
	var str="";
	str="/sys_cfg_valid.xgi?";
	str+=exe_str("submit REBOOT");
	self.location.href=str;
}
<?
if(query("/function/no_jumpstart")!="1")
{
	echo "function enable_js()\n";
	echo "{\n";
	echo "	f=get_obj('frm');\n";
	echo "	f.ACTION_POST.value='enable_js';\n";
	echo "	f.en_js.value=(f.enable_jumpstart.checked ? 1 : 0);\n";
	echo "}\n";
	echo "function reset_js()\n";
	echo "{\n";
	echo "	if(confirm('".$a_sure_to_reset_js."')==false) return;\n";
	echo "	f=get_obj('frm');\n";
	echo "	f.ACTION_POST.value='reset_js';\n";
	echo "}\n";
}
?>
</script>
<body <?=$G_BODY_ATTR?>>
<?require("/www/model/__banner.php");?>
<?require("/www/model/__menu_top.php");?>
<table <?=$G_MAIN_TABLE_ATTR?> height="100%">
<tr valign=top>
	<td <?=$G_MENU_TABLE_ATTR?>>
	<?require("/www/model/__menu_left.php");?>
	</td>
	<td id="maincontent">
		<div id="box_header">
		<?require($LOCALE_PATH."/dsc/dsc_".$MY_NAME.".php");?>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
		<div class="box">
			<h2><?=$m_context_title?></h2>
			<table width=95%>
			<form>
			<tr>
				<td class=r_tb width=45%><?=$m_save_cfg?> :</td>
				<td><input type=button value="<?=$m_save?>" onclick="save_cfg()"></td>
			</tr>
			</form>
			<form method=POST action="upload_config._int" enctype=multipart/form-data onsubmit="return load_cfg(this.form);">
			<tr>
				<td class=r_tb><?=$m_load_cfg?> :</td>
				<td>
				<input type=file name="configuration" id="configuration" size=20><br>
				<input type="submit" value="<?=$m_b_load?>" name=load>
				</td>
			</tr>
			</form>
			<form>
			<tr>
				<td class=r_tb><?=$m_factory_reset?> :</td>
				<td><input type="button" value="<?=$m_b_restore?>" name=restore onclick="do_factory_reset(this.form)"></td>
			</tr>
			</form>
			<form>
			<tr>
				<td class=r_tb><?=$m_reboot?> :</td>
				<td><input type=button value="<?=$m_b_reboot?>" onclick="do_reboot()"></td>
			</tr>
			</form>
			</table>
		</div>
<?
if(query("/function/no_jumpstart")!="1")
{
	echo "	<div class=\"box\">\n";
	echo "		<h2>".$m_jumpstart_title."</h2>\n";
	echo "		<form name='frm' method=post action='".$MY_NAME.".php'>\n";
	echo "		<input type=hidden name=ACTION_POST value=''>\n";
	echo "		<input type=hidden name='en_js' value=''>\n";
	echo "		<table width=95%>\n";
	echo "		<tr>\n";
	echo "			<td class=r_tb width=45%>".$m_enable_js_fn." : </td>\n";
	if(query("/wireless/jumpstart/enable")=="1"){$js_checked="checked";}
	echo "			<td><input type=checkbox name='enable_jumpstart' ".$js_checked."> &nbsp; <input type=submit name='bt_apply' value='".$m_apply."' onclick='enable_js()'></td>\n";
	echo "		</tr>\n";
	echo "		<tr>\n";
	echo "			<td class=r_tb>".$m_reset_js." : </td>\n";
	echo "			<td><input type=submit name=bt_reset value='".$m_jumpstart."' onclick='reset_js()'></td>\n";
	echo "		</tr>\n";
	echo "		</table>\n";
	echo "		</form>\n";
	echo "	</div>\n";
}
?>
<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>>
	<?
	require($LOCALE_PATH."/help/h_".$MY_NAME.".php");
	echo "<br><br>";
	require($LOCALE_PATH."/help/h_".$MY_NAME."_js.php");
	?>
	</td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</body>
</html>
