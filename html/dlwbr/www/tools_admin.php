<?
/* vi: set sw=4 ts=4: --------------------------------------------------------- */
$MY_NAME		="tools_admin";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="tools";
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");

	$httpd_passwd=0;
	$rg_misc=0;
	
	anchor("/sys/user:1");
	if(query("name")		!=$admin_name)		{set("name",$admin_name);			$httpd_passwd++;}
	if(	   $admin_password1	!=$G_DEF_PASSWORD
		&& $admin_password1	!=query("password")){set("password",$admin_password1);	$httpd_passwd++;}

	anchor("/sys/user:2");
	if(query("name")		!=$user_name)		{set("name",$user_name);			$httpd_passwd++;}
	if(	   $user_password1	!=$G_DEF_PASSWORD
		&& $user_password1	!=query("password")){set("password",$user_password1);	$httpd_passwd++;}
	
	if($router=="1")
	{
		anchor("/security/firewall");
		if(query("httpAllow")		!=$rt_enable_h)	{set("httpAllow",$rt_enable_h);		$rg_misc++;}
		if($rt_ipaddr=="*")							{$rt_ipaddr="";}
		if(query("httpRemoteIp")	!=$rt_ipaddr)	{set("httpRemoteIp",$rt_ipaddr);	$rg_misc++;}
		if(query("httpRemotePort")	!=$rt_port)		{set("httpRemotePort",$rt_port);	$rg_misc++;}
		//	if(query("httpRemoteScheduleId") !=$rt_sch)	{set("httpRemoteScheduleId",$rt_sch);	$rg_misc++;}
	}

/*
echo "password1=".$admin_password1."<br>";
echo "user password1=".$user_password1."<br>";
echo "httpAllow=".$rt_enable_h."; httpd_passwd=".$httpd_passwd."<br>";
echo "rg_misc=".$rg_misc."<br>";
echo "SUBMIT_STR=".$SUBMIT_STR;
exit;
*/

	if($httpd_passwd > 0)	{$SUBMIT_STR="submit HTTPD_PASSWD"; $div=";";}
	if($rg_misc > 0)		{$SUBMIT_STR=$SUBMIT_STR.$div."submit RG_MISC";}
	
	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb
anchor("/sys/user:1");
$admin_name=query("name");
//if(query("password")!=""){$admin_password=$G_DEF_PASSWORD;}
$admin_password=$G_DEF_PASSWORD;

anchor("/sys/user:2");
$user_name=query("name");
//if(query("password")!=""){$user_password=$G_DEF_PASSWORD;}
$user_password=$G_DEF_PASSWORD;

/* --------------------------------------------------------------------------- */
?>
<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	f.admin_name.value		="<?=$admin_name?>";
	f.admin_password1.value	=f.admin_password2.value="<?=$admin_password?>";
	f.user_name.value		="<?=$user_name?>";
	f.user_password1.value	=f.user_password2.value	="<?=$user_password?>";
	
	<?anchor("/security/firewall");?>
	f.rt_enable.checked		=<?map("httpAllow","1","true",*,"false");?>;
	f.rt_ipaddr.value		="<?map("httpRemoteIp","","*");?>";
	select_index(f.rt_port,	"<?map("httpRemotePort", "","8080");?>");
//	select_index(f.sch_list,"<?map("httpschedule", "","1");?>");
	<?
	if($router!=1)
	{
		echo "f.rt_enable.disabled = f.rt_enable_h.disabled = f.rt_ipaddr.disabled = f.rt_port.disabled = true;\n";
	}
	?>
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(is_blank(f.admin_name.value))
	{
		alert("<?=$a_empty_login_name?>");
		f.admin_name.select();
		return false;
	}
	else if(strchk_hostname(f.admin_name.value)==false)
	{
		alert("<?=$a_invalid_login_name?>");
		f.admin_name.select();
		return false;
	}
	if(strchk_unicode(f.admin_password1.value)==true)
	{
		alert("<?=$a_invalid_new_password?>");
		f.admin_password1.select();
		return false;
	}
	if(f.admin_password1.value!=f.admin_password2.value)
	{
		alert("<?=$a_password_not_matched?>");
		f.admin_password1.select();
		return false;
	}
	
	if(is_blank(f.user_name.value))
	{
		alert("<?=$a_empty_login_name?>");
		f.user_name.select();
		return false;
	}
	else if(strchk_hostname(f.user_name.value)==false)
	{
		alert("<?=$a_invalid_login_name?>");
		f.user_name.select();
		return false;
	}
	if(strchk_unicode(f.user_password1.value)==true)
	{
		alert("<?=$a_invalid_new_password?>");
		f.user_password1.select();
		return false;
	}
	if(f.user_password1.value!=f.user_password2.value)
	{
		alert("<?=$a_password_not_matched?>");
		f.user_password1.select();
		return false;
	}

	if(f.rt_enable.checked)
	{
		f.rt_enable_h.value="1";
		if(f.rt_ipaddr.value!="*" && is_valid_ip(f.rt_ipaddr.value)==false)
		{
			alert("<?=$a_invalid_ipaddr?>");
			f.rt_ipaddr.select();
			return false;
		}
	}
	else
	{
		f.rt_enable_h.value="0";
	}
//	f.rt_enable_h.value=(f.rt_enable.checked ?"1":"0");
	return true;
}

function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
<input type="hidden" name="ACTION_POST" value="1">
<?require("/www/model/__banner.php");
require("/www/model/__menu_top.php");?>
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
		</div><br>
<!-- ________________________________ Main Content Start __________________________________________ -->
<?
$td_width1="width=\"180\" align=\"right\"";
$td_width2="width=\"368\"";
?>
		<div class="box">
			<h2><?=$m_context_title_admin?></h2>
			<table>
			<tr>
				<td <?=$td_width1?>><?=$m_login_name?>:</td>
				<td <?=$td_width2?>width="368">&nbsp;<input type="text" name="admin_name" size=20 maxlength=15></td>
			</tr>
			<tr>
				<td align="right"><?=$m_new_password?>:</td>
				<td>&nbsp;<input type="password" name="admin_password1" size=20 maxlength=15 onFocus=this.select();></td>
			</tr>
			<tr>
				<td align="right"><?=$m_confirm_password?>:</td>
				<td>&nbsp;<input type="password" name="admin_password2" size=20 maxlength=15 onFocus=this.select();></td>
			</tr>
			</table>
		</div>
		<!-- ------------------------------------------------------------------------------------ -->
		<div class="box">
			<h2><?=$m_context_title_user?></h2>
			<table>
			<tr>
				<td <?=$td_width1?>><?=$m_login_name?>:</td>
				<td <?=$td_width2?>>&nbsp;<input type="text" name="user_name" size=20 maxlength=15></td>
			</tr>
			<tr>
				<td align="right"><?=$m_new_password?>:</td>
				<td>&nbsp;<input type="password" name="user_password1" size=20 maxlength=15 onFocus=this.select();></td>
			</tr>
			<tr>
				<td align="right"><?=$m_confirm_password?>:</td>
				<td>&nbsp;<input type="password" name="user_password2" size=20 maxlength=15 onFocus=this.select();></td>
			</tr>
			</table>
		</div>
		<!-- ------------------------------------------------------------------------------------ -->
		<div class="box">
			<h2><?=$m_context_title_remote?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<td <?=$td_width1?>><?=$m_eable_remote?>:</td>
				<td <?=$td_width2?>>&nbsp;<input type="checkbox" name="rt_enable"></td>
				<input type="hidden" name="rt_enable_h" value=0>
			</tr>
			<tr>
				<td align="right"><?=$m_ipaddr?>:</td>
				<td>&nbsp;<input type="text" name="rt_ipaddr" size=15 maxlength=15></td>
			</tr>
			<tr>
				<td align="right"><?=$m_port?>:</td>
				<td>&nbsp;<select name="rt_port">
					<option value="80">80</option>
					<option value="88">88</option>
					<option value="1080">1080</option>
					<option value="8080">8080</option>
				</select></td>
			</tr>
			<!--tr>
				<td align="right"><?=$m_schedule?>:</td>
				<td>&nbsp;<?require("/www/model/__sch_list.php");?></td>
			</tr-->
			</table>
		</div>

<!-- ________________________________  Main Content End ____________________________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
