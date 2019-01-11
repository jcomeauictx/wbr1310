<?
/* vi: set sw=4 ts=4: */
$MY_NAME	= "version";
$MY_MSG_FILE= $MY_NAME.".php";
/* ------------------------------------------------------------------------- */
$NO_NEED_AUTH="1";
$NO_SESSION_TIMEOUT="1";
require("/www/model/__html_head.php");
?>
<script>
function shortTime()
{
	t="<?query("/runtime/sys/uptime");?>";

	var str=new String("");
	var tmp=parseInt(t, [10]);
	var sec=0,min=0,hr=0,day=0;
	sec=t % 60;  //sec
	min=parseInt(t/60, [10]) % 60; //min
	hr=parseInt(t/(60*60), [10]) % 24; //hr
	day=parseInt(t/(60*60*24), [10]); //day

	if(day>=0 || hr>=0 || min>=0 || sec >=0)
		str=(day >0? day+" <?=$m_days?>, ":"0 <?=$m_days?>, ")+(hr >0? hr+":":"00:")+(min >0? min+":":"00:")+(sec >0? sec :"00");
	return str;
}

function init()
{
	var f=get_obj("frm");
	f.bt.focus();
}
function click_bt()
{
	self.location.href="<?=$G_HOME_PAGE?>.php";
}
</script>
<?
$USE_BUTTON="1";
require("/www/model/__show_info.php");
?>
