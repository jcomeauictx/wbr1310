<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="adv_url_filter";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="adv";
/* --------------------------------------------------------------------------- */
$MAX_RULES		=query("/security/urlblocking/max_rules");
if($MAX_RULES==""){$MAX_RULES=20;}
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
if ($ACTION_POST!="" && $router=="1")
{
	require("/www/model/__admin_check.php");
	echo "<!--\n";
	echo "enable=".$enable."\n";
	echo "action=".$action."\n";

	$i=0;
	$dirty=0;
	anchor("/security/urlblocking");
	while ($i < $MAX_RULES)
	{
		$i++;
		$target="url_".$i;
		echo $target."=".$$target."\n";

		if (query("string:".$i)!=$$target) {$dirty++; set("string:".$i, $$target);}
	}
	if (query("enable")!=$enable)	{$dirty++; set("enable", $enable);}
	if (query("action")!=$action)	{$dirty++; set("action", $action);}

	if($dirty  > 0)	{$SUBMIT_STR=";submit RG_BLOCKING";}
	else			{$SUBMIT_STR="";}

	echo "SUBMIT_STR=".$SUBMIT_STR."\n";
	echo "-->\n";

	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
anchor("/security/urlblocking");
$enable=query("enable");
$action=query("action");
if ($enable==1)
{	if ($action==1)	{$mode=1;}
	else			{$mode=2;}
} else				{$mode=0;}

/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	// init here ...
	<?if($router!=1){echo "fields_disabled(f, true);\n";}?>
}
/* parameter checking */
function check()
{
	var f = get_obj("frm");
	var i, j;
	var obj, obj2;
	var count=0;

	for (i = 1; i <= <?=$MAX_RULES?>; i++)
	{	
		obj = get_obj("url_"+i);
		if (!is_blank(obj.value))
		{
			if(strchk_url(obj.value)==false)
			{
				alert("<?=$a_invalid_url?>");
	 			return false;
	 		}
			count++;
		}
	}
	if (count > 0)
	{
		for (i=1; i <= <?=$MAX_RULES?>; i++)
		{
			obj = get_obj("url_"+i);
			if (!is_blank(obj.value))
			{
				for (j = i+1; j <= <?=$MAX_RULES?>; j++)
				{
					obj2 = get_obj("url_"+j);
					if (!is_blank(obj2.value) && obj.value == obj2.value)
					{
						alert("<?=$a_same_url_entry_exists?>");
						field_focus(obj2, "**");
						return false;
					}
				}
			}
		}
	}
	else
	{
		f.mode.value = 0;
	}

	if (f.mode.value == 2)
	{
		f.enable.value = 1;
		f.action.value = 0;
	}
	else if (f.mode.value == 1)
	{
		f.enable.value = 1;
		f.action.value = 1;
	}
	else
	{
		f.enable.value = 0;
	}

	return true;
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}

function clear_url()
{
	var f=get_obj("frm");
	for (var i = 1; i <= <?=$MAX_RULES?>; i++)
	{
		eval("f.url_"+i).value = "";
	}
}
		
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check();">
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
			<h2><?=$MAX_RULES?> - <?=$m_title_url_filter?></h2>
			<table cellSpacing=1 cellPadding=2 width=525 border=0>
			<tr>
				<td><?=$m_desc_url_filter?></td>
			</tr>
			<tr>
				<td>
					<select name="mode">
						<option value="0"<?if ($mode==0){echo " selected";}?>><?=$m_disable_url_filter?></option>
						<option value="1"<?if ($mode==1){echo " selected";}?>><?=$m_allow_entries_only?></option>
						<option value="2"<?if ($mode==2){echo " selected";}?>><?=$m_deny_entries_only?></option>
					</select>
					<input type=hidden id=enable name=enable>
					<input type=hidden id=action name=action>
				</td>
			</tr>
			<tr>
			<td><input name="clear" type=button value="<?=$m_clear_list_below?>" onClick="clear_url()"></td>
			</tr>
			</table>
			<br>
			<table borderColor=#ffffff cellSpacing=1 cellPadding=2 width=525 bgColor=#dfdfdf border=1>
			<tr>
				<td width="255"><?=$m_website_url_domain?></td>
				<td width="253"><?=$m_website_url_domain?></td>
			</tr>
			<?
				$index=1;
				while($index<=$MAX_RULES)
				{
					$string=query("/security/urlblocking/string:".$index);
					echo	"<tr>\n";
		 			echo	"<td>";
					echo	"<input id=url_".$index." name=url_".$index." maxLength=40 size=41 value=".$string.">\n";
		  			echo	"</td>\n";
					$index++;
					$string=query("/security/urlblocking/string:".$index);
		   			echo	"<td><input id=url_".$index." name=url_".$index." maxLength=40 size=41 value=".$string.">\n";
		    		echo	"</td>\n";
					echo	"</tr>\n";
		    		$index++;		
				}
			?>
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
