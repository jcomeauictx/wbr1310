<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		= "adv_mac_filter";
$MY_MSG_FILE	= $MY_NAME.".php";
$CATEGORY		= "adv";
/* --------------------------------------------------------------------------- */
$MAX_RULES		= query("/security/macfilter/max_rules");
if($MAX_RULES==""){$MAX_RULES=10;}
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
	while ($i < $MAX_RULES)
	{
		$i++;
		$target="mac_".$i;
		echo $target."=".$$target."\n";
		$entry="/security/macfilter/entry:".$i."/sourcemac";
		if (query($entry) != $$target)	{$dirty++; set($entry, $$target);}
	}
	anchor("/security/macfilter");
	if (query("enable")!=$enable)	{$dirty++; set("enable", $enable);}
	if (query("action")!=$action)	{$dirty++; set("action", $action);}

	if($dirty > 0)	{$SUBMIT_STR=";submit RG_MAC_FILTER";}
	else			{$SUBMIT_STR="";}

	echo "SUBMIT_STR=".$SUBMIT_STR."\n";
	echo "-->\n";

	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
anchor("/security/macfilter");
$enable=query("enable");
$action=query("action");
if ($enable==1)
{	if ($action==1)	{$mode=1;}
	else			{$mode=2;}
} else				{$mode=0;}

/* --------------------------------------------------------------------------- */
?>

<script>
function copy_mac(index)
{
	var mac = get_obj("mac_"+index);
	var dhcp = get_obj("dhcp_"+index);

	if (dhcp.value == 0)
	{
		alert("<?=$a_no_pc_selected?>");
		return false;
	}
	mac.value = dhcp.value;
}

function clear_mac(index)
{
	var mac = get_obj("mac_"+index);
	mac.value = "";
}

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
	var obj, obj2;
	var mac;
	var count = 0;
	var i, j;
	var saw_me = 0;

	for (i=1; i <= <?=$MAX_RULES?>; i++)
	{
		obj = get_obj("mac_"+i);
		if (!is_blank(obj.value))
		{
			mac = get_mac(obj.value);
			if (is_blank(mac[1]) || !is_valid_mac(mac[1]) ||
				is_blank(mac[2]) || !is_valid_mac(mac[2]) ||
				is_blank(mac[3]) || !is_valid_mac(mac[3]) ||
				is_blank(mac[4]) || !is_valid_mac(mac[4]) ||
				is_blank(mac[5]) || !is_valid_mac(mac[5]) ||
				is_blank(mac[6]) || !is_valid_mac(mac[6]))
			{
				alert("<?=$a_invalid_mac?>");
				field_focus(obj, "**");
				return false;
			}
			else
			{
				for (j=1; j<=6; j++)
				{
					if (mac[j].length == 1) mac[j] = "0"+mac[j];
				}
				obj.value = mac[1]+":"+mac[2]+":"+mac[3]+":"+mac[4]+":"+mac[5]+":"+mac[6];

				if (obj.value == "<?=$macaddr?>")
				{
					saw_me++;
					if (f.mode.value==2)
					{
						if (confirm("<?=$a_blocking_warning?>")==false)
						{
							field_focus(obj, "**");
							return false;
						}
					}
				}
				count++;
			}
		}
	}
	if (count > 0)
	{
		if (f.mode.value == 1 && saw_me == 0)
		{
			if (confirm("<?=$a_blocking_warning?>")==false) return false;
		}
		for (i=1; i < <?=$MAX_RULES?>; i++)
		{
			obj = get_obj("mac_"+i);
			if (!is_blank(obj.value))
			{
				for (j=i+1; j<=<?=$MAX_RULES?>; j++)
				{
					obj2 = get_obj("mac_"+j);
					if (!is_blank(obj2.value) && obj.value == obj2.value)
					{
						alert("<?=$a_macaddr_exist?>");
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

	if		(f.mode.value == 2)	{ f.enable.value = 1; f.action.value = 0; }
	else if	(f.mode.value == 1)	{ f.enable.value = 1; f.action.value = 1; }
	else						{ f.enable.value = 0; }
	return true;
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
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
			<h2><?=$MAX_RULES?> - <?=$m_mac_title?></h2>
			<table cellSpacing=1 cellPadding=2 width=525 border=0>
			<tr><td><?=$m_mac_desc?></td></tr>
			<tr>
				<td>
				<select name="mode">
					<option value="0"<?if ($mode==0){echo " selected";}?>><?=$m_mac_filter_off?></option>
					<option value="1"<?if ($mode==1){echo " selected";}?>><?=$m_mac_filter_allow_entries?></option>
					<option value="2"<?if ($mode==2){echo " selected";}?>><?=$m_mac_filter_deny_entries?></option>
				</select>
				<input type=hidden id=enable name=enable value="<?=$mode?>">
				<input type=hidden id=action name=action>
				</td>
			</tr>
			</table>
			<br>
			<table borderColor=#ffffff cellSpacing=1 cellPadding=4 width=525 bgColor=#dfdfdf border=1>
			<tr>
				<td width="124"><?=$m_macaddr?></td>
				<td width="32">&nbsp;</td>
				<td><?=$m_dhcp_client_list?></td>
				<td width="50" align=middle>&nbsp;</td>
			</tr>
<?
			$index=0;
			while ($index < $MAX_RULES)
			{
				$index++;
				require("/www/__adv_mac_filter.php");
			}

?>			</table>
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
