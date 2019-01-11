<?
require("/www/model/__g_var.php");
$__LOCALE_LANGUAGE=query("/sys/locale");
if ($__LOCALE_LANGUAGE=="") {$__LOCALE_LANGUAGE="en";}
$LOCALE_PATH="/www/locale/".$__LOCALE_LANGUAGE;
$CHARSET=fread($LOCALE_PATH."/charset");
if ($CHARSET=="") {$CHARSET="utf-8";}
if ($MY_MSG_FILE!="")
{
	require($LOCALE_PATH."/msg_comm.php");
	require($LOCALE_PATH."/msg_menu.php");
	require($LOCALE_PATH."/".$MY_MSG_FILE);
}
?>
