<?
/* vi: set sw=4 ts=4: */
if ($ACTION_POST!="")
{
	require("/www/auth/__login.php");
	if		($AUTH_RESULT=="401")	{$HEADER_URL="login_fail.php"; require("/www/comm/__header.php");}
	else if ($AUTH_RESULT=="full")	{$HEADER_URL="session_full.php"; require("/www/comm/__header.php");}

	$HEADER_URL="index.php";
	require("/www/comm/__header.php");
}

/* ------------------------------------------------------------------------ */
$MY_NAME="login";
$MY_MSG_FILE=$MY_NAME.".php";
$NO_NEED_AUTH="1";
$NO_SESSION_TIMEOUT="1";
require("/www/model/__html_head.php");
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	f.LOGIN_USER.focus();
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(f.LOGIN_USER.value=="")
	{
		alert("<?=$a_invalid_user_name?>");
		f.LOGIN_USER.focus();
		return false;
	}
	return true;
}
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="login.php">
<input type="hidden" name="ACTION_POST" value="LOGIN">
<?require("/www/model/__banner.php");?>
<table <?=$G_MAIN_TABLE_ATTR?>>
<tr valign=middle align=center>
	<td>
	<br>
<!-- ________________________________ Main Content Start ______________________________ -->
	<table width=80%>
	<tr>
		<td id="box_header">
			<h1><?=$m_context_title?></h1>
			<?=$m_login_router?>
			<br><br><center>
			<table>
			<tr>
				<td><?=$m_user_name?></td>
				<td><input type=text name="LOGIN_USER"></td>
			</tr>
			<tr>
				<td><?=$m_password?></td>
				<td><input type=password name="LOGIN_PASSWD"></td>
				<td><input type="submit" name="login" value="<?=$m_log_in?>" onclick="return check()"></td>
			</tr>
			</table>
			</center><br>
		</td>
	</tr>
	</table>
<!-- ________________________________  Main Content End _______________________________ -->
	<br>
	</td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
