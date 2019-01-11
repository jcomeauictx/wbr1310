<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		= "adv_app";
$MY_MSG_FILE	= $MY_NAME.".php";
$CATEGORY		= "adv";
/* --------------------------------------------------------------------------- */
/* local used variables */
$MAX_RULES		=query("/nat/porttrigger/max_rules");
if($MAX_RULES==""){$MAX_RULES=10;}
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
if ($ACTION_POST!="" && $router=="1")
{
	require("/www/model/__admin_check.php");
	echo "<!--\n";
	echo "ACTION_POST=".$ACTION_POST."\n";

	$dirty=0; $index=0;
	while ($index < $MAX_RULES)
	{
		$index++;
		$cfg_en		= "enable_".$index;
		$cfg_desc	= "desc_".$index;
		$cfg_tstart	= "triggerstart_".$index;
		$cfg_tend	= "triggerend_".$index;
		$cfg_tprot	= "trigger_prot_".$index;
		$cfg_pport	= "pub_port_".$index;
		$cfg_pprot	= "pub_prot_".$index;
		if ($$cfg_en!="1") {$$cfg_en="0";}

		echo $cfg_en.   "=".$$cfg_en.", ".     $cfg_desc.  "=".$$cfg_desc.", ";
		echo $cfg_tprot."=".$$cfg_tprot.", ".  $cfg_tstart."=".$$cfg_tstart.", ".$cfg_tend."=".$$cfg_tend.", ";
		echo $cfg_pprot."=".$$cfg_pubprot.", ".$cfg_pport. "=".$$cfg_pport."\n";

		$entry="/nat/porttrigger/entry:".$index."/";
		if (query($entry."enable")			!= $$cfg_en)	{$dirty++; set($entry."enable",				$$cfg_en);}
		if (query($entry."description")		!= $$cfg_desc)	{$dirty++; set($entry."description",		$$cfg_desc);}
		if (query($entry."triggerprotocol")	!= $$cfg_tprot)	{$dirty++; set($entry."triggerprotocol",	$$cfg_tprot);}
		if (query($entry."triggerportbegin")!= $$cfg_tstart){$dirty++; set($entry."triggerportbegin",	$$cfg_tstart);}
		if (query($entry."triggerportend")	!= $$cfg_tend)	{$dirty++; set($entry."triggerportend",		$$cfg_tend);}
		if (query($entry."publicprotocol")	!= $$cfg_pprot)	{$dirty++; set($entry."publicprotocol",		$$cfg_pprot);}
		if (query($entry."publicport")		!= $$cfg_pport)	{$dirty++; set($entry."publicport",			$$cfg_pport);}
	}
	echo "-->\n";

	if($dirty > 0)	{$SUBMIT_STR=";submit RG_APP";}
	else			{$SUBMIT_STR="";}

	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.

/* --------------------------------------------------------------------------- */
?>

<script>
var prot_list = [
	["6112,0,0",	"6112"],
	["7175,0,0",	"51200-51201,51210"],
	["2019,0,0",	"2000-2038,2050-2051,2069,2085,3010-3030"],
	["47624,0,0",	"2300-2400,28800-29000"],
	["12053,0,0",	"12120,12122,24150-24220"],
	["554,0,0",		"6970-6999"]
];

function copy_application(index)
{
	var desc = get_obj("desc_"+index);
	var app = get_obj("app_"+index);
	var trigger_port = get_obj("trigger_port_"+index);
	var trigger_prot = get_obj("trigger_prot_"+index);
	var pub_port = get_obj("pub_port_"+index);
	var pub_prot = get_obj("pub_prot_"+index);
	var data;

	if (app.selectedIndex > 0)
	{
		desc.value = app.value;
		data = prot_list[app.selectedIndex - 1][0].split(",");
		pub_port.value = prot_list[app.selectedIndex - 1][1];

		trigger_port.value = data[0];
		select_index(trigger_prot, data[1]);
		select_index(pub_prot, data[2]);
	}
	else
	{
		alert('<?=$a_no_app_name?>');
	}
}

function check_trigger_port(index)
{
	var trigger_port_n = get_obj("trigger_port_"+index);
	var obj;

	var trigger = (trigger_port_n.value).split(",");
	if (trigger.length != 1) return false;

	var ports = trigger[0].split("-");
	if (ports.length == 1)
	{
		if (is_valid_port_str(ports[0]))
		{
			obj = get_obj("triggerstart_"+index);
			obj.value = ports[0];
			obj = get_obj("triggerend_"+index);
			obj.value = "";
			return true;
		}
	}
	else if (ports.length == 2)
	{
		if (is_valid_port_range_str(ports[0], ports[1]))
		{
			obj = get_obj("triggerstart_"+index);
			obj.value = ports[0];
			obj = get_obj("triggerend_"+index);
			obj.value = ports[1];
			return true;
		}
	}
	return false;
}

function check_public_port(index)
{
	var ports;
	var pub_port_n = get_obj("pub_port_"+index);

	var pubport = (pub_port_n.value).split(",");
	if (pubport.length < 1) return false;

	for (i=0; i<pubport.length; i++)
	{
		ports = pubport[i].split("-");
		if (ports.length == 1)
		{
			if (!is_valid_port_str(ports[0])) return false;
		}
		else if (ports.length == 2)
		{
			if (!is_valid_port_range_str(ports[0], ports[1])) return false;
		}
		else
		{
			return false;
		}
	}
	return true;
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
	var enable;
	var desc;
	var trigger_port;
	var trigger_prot;
	var pub_port;
	var pub_prot;

	// do check here ....
	for (var i=1; i<=<?=$MAX_RULES?>; i++)
	{
		enable = get_obj("enable_"+i);
		desc = get_obj("desc_"+i);
		trigger_port = get_obj("trigger_port_"+i);
		trigger_start = get_obj("triggerstart_"+i);
		trigger_end = get_obj("triggerend_"+i);
		trigger_prot = get_obj("trigger_prot_"+i);
		pub_port = get_obj("pub_port_"+i);
		pub_prot = get_obj("pub_prot_"+i);

		if (is_blank(desc.value))
		{
			enable.checked = false;
			trigger_port.value = "";
			trigger_start.value = "";
			trigger_end.value = "";
			pub_port.value = "";
			select_index(trigger_prot, 0);
			select_index(pub_prot, 0);
		}
		else
		{
			if (check_trigger_port(i)==false)
			{
				alert("<?=$a_invalid_trigger_port?>");
				field_focus(trigger_port, "**");
				return false;
			}
			if (check_public_port(i)==false)
			{
				alert("<?=$a_invalid_firewall_port?>");
				field_focus(pub_port, "**");
				return false;
			}
		}
	}

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
			<h2><?=$MAX_RULES?> - <?=$m_title_app_rules?></h2>
			<table borderColor=#ffffff cellSpacing=1 cellPadding=2 width=525 bgColor=#dfdfdf border=1>
			<tbody>
			<tr>
				<td align=middle width=20>&nbsp;</td>
				<td width="75">&nbsp;</td>
				<td width="193" align=left><div align="left">&nbsp;</div></td>
				<td width="141" align=middle><div align="center"><?=$m_port?></div></td>
				<td width="58" align=middle><div align="center"><?=$m_traffic_type?></div></td>
			</tr><?

			$index=0;
			while ($index < $MAX_RULES)
			{
				$index++;
				require("/www/__adv_app.php");
			}

?>			</tbody>
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
