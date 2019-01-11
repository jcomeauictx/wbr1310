<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="adv_port";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="adv";
/* --------------------------------------------------------------------------- */
/* local used variables */
$MAX_RULES      = query("/nat/vrtsrv/max_rules");
if($MAX_RULES==""){$MAX_RULES=10;}
$modelname=query("/sys/modelname");
/* --------------------------------------------------------------------------- */
$router=query("/runtime/router/enable");
if ($ACTION_POST!="" && $router=="1")
{
	require("/www/model/__admin_check.php");
/*
	echo "<!--\n";
 */

	$dirty=0;
	$index=0;
	while($index < $MAX_RULES)
	{
		$index++;
		$cfg_en     ="enable".$index;
		$cfg_name	="name".$index;
		$cfg_ip		="ip".$index;
		$cfg_sport	="start_port".$index;		
		$cfg_eport	="end_port".$index;
		$cfg_prot	="protocol".$index;
		if($modelname=="WBR-2310")	{	$cfg_sch	="schedule".$index; }

/*
		echo "cfg_en=".$cfg_en."=".$$cfg_en."\n";
		echo "cfg_name=".$cfg_name."=".$$cfg_name."\n";
		echo "cfg_ip=".$cfg_ip."=".$$cfg_ip."\n";
		echo "cfg_sport=".$cfg_sport."=".$$cfg_sport."\n";
		echo "cfg_eport=".$cfg_eport."=".$$cfg_eport."\n";
		echo "cfg_prot=".$cfg_prot."=".$$cfg_prot."\n";
		echo "cfg_sch=".$cfg_sch."=".$$cfg_sch."\n";
 */
		$entry="/nat/vrtsrv/entry:".$index."/";
		if ($$cfg_sport==$$cfg_eport)	{$$cfg_eport="";}
			
		if (query($entry."enable")		!= $$cfg_en)	{$dirty++; set($entry."enable",			$$cfg_en);}
		if (query($entry."protocol")	!= $$cfg_prot)	{$dirty++; set($entry."protocol",		$$cfg_prot);}				
		if (query($entry."privateip")	!= $$cfg_ip)	{$dirty++; set($entry."privateip",		$$cfg_ip);}
		if (query($entry."privateport")	!= $$cfg_sport)	{$dirty++; set($entry."privateport",	$$cfg_sport);}
		if (query($entry."publicport")	!= $$cfg_sport)	{$dirty++; set($entry."publicport",		$$cfg_sport);}
		if (query($entry."privateport2")!= $$cfg_eport)	{$dirty++; set($entry."privateport2",	$$cfg_eport);}
		if (query($entry."publicport2")	!= $$cfg_eport)	{$dirty++; set($entry."publicport2",	$$cfg_eport);}
		if (query($entry."description")	!= $$cfg_name)	{$dirty++; set($entry."description",	$$cfg_name);}
		if($modelname=="WBR-2310")
		{
			if (query($entry."schedule/uniqueid"))	!= $$cfg_sch)		
				{$dirty++; set($entry."schedule/uniqueid", $$cfg_sch); set($entry."schedule/enable", $$cfg_sch);}
		}
	}
		
	if($dirty > 0)	{$SUBMIT_STR=";submit RG_VSVR";}
	else			{$SUBMIT_STR="";}
/*
	echo "SUBMIT_STR=".$SUBMIT_STR."\n";
	echo "-->\n";
 */
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
var port_list = new Array("21","80","443","53","25","110","23","500","1723","1720","80","800","1720");
var prot_list = new Array("0","0","0","1","0","0","0","1","0","0","0","0","0");
var rules=<?=$MAX_RULES?>+1;
function copy_application(index)
{
	var name=get_obj("name_"+index);
	var app=get_obj("app_"+index);
	var sport=get_obj("start_port_"+index);
	var eport=get_obj("end_port_"+index)
	var prot=get_obj("protocol_"+index);
	
    if (app.selectedIndex > 0)
    {
		name.value = app.value;
		sport.value=port_list[app.selectedIndex-1];
		eport.value=port_list[app.selectedIndex-1];
		prot.selectedIndex=prot_list[app.selectedIndex-1];
	}	
	else
	{ alert('<?=$a_no_app_name?>');	}
}	

function copy_ip(index)
{
	var ip=get_obj("ip_"+index);
	var hostname=get_obj("ip_list_"+index);
	if (hostname.selectedIndex > 0)	
		{ ip.value=hostname.value; }
	else
		{ alert('<?=$a_no_host_name?>'); }
}
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	var s=get_obj("postfrm");
	<?
	if($router!=1)
	{
		echo "fields_disabled(f, true);\n";
		echo "fields_disabled(s, true);\n";
	}
	?>
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	var len=0;
	var tmp_ip;
	var tmp_sport;
	var tmp_eport;
	var start, end;
	var data= new Array(rules);

	for(j=0;j<rules;j++)  data[j]=new Array(7);
	
	for(i=1; i < rules ; i++)
	{
		if (!is_blank(get_obj("name_"+i).value))
		{
			len++;
			tmp_ip		= eval("f.ip_"+i+".value");
			tmp_sport	= eval("f.start_port_"+i+".value");
			tmp_eport	= eval("f.end_port_"+i+".value");

			if (is_blank(tmp_ip) || is_valid_ip(tmp_ip,0)==false)
			{
				alert("<?=$a_invalid_ip?>");
				field_focus(get_obj("ip_"+i), "**");
				return false;
			}
			if (is_blank(tmp_sport))
			{
				alert('<?=$a_cant_blank?>');
				field_focus(get_obj("start_port_"+i), "**");
				return false;
			}
			if (is_blank(tmp_eport))
			{
				alert('<?=$a_cant_blank?>');
				field_focus(get_obj("end_port_"+i), "**");
				return false;
			}
			if (!is_valid_port_str(tmp_sport))
			{
				alert('<?=$a_invalid_port?>');
				field_focus(get_obj("start_port_"+i), "**");
				return false;
			}
			if (!is_valid_port_str(tmp_eport))
			{
				alert('<?=$a_invalid_port?>');
				field_focus(get_obj("end_port_"+i), "**");
				return false;
			}

			start = parseInt(tmp_sport, [10]);
			end = parseInt(tmp_eport, [10]);
			if (start > end)
			{
				alert('<?=$a_end_big_start?>');
				field_focus(get_obj("end_port_"+i), "**");
				return false;
			}

			data[len][0] = get_obj("enable_"+i).checked ? "1" : "";
			data[len][1] = eval("f.name_"+i+".value");
			data[len][2] = eval("f.ip_"+i+".value");
			data[len][3] = eval("f.start_port_"+i+".value");
			data[len][4] = eval("f.end_port_"+i+".value");
			data[len][5] = eval("f.protocol_"+i+".value");

			<?if($modelname=="WBR-2310"){echo "data[len][6] = eval(\"f.schedule_\"+i+\".value\");\n";}?>
		}
	}
	
	g = get_obj("postfrm");
	for(k=1; k<rules; k++)
	{
		if(k<=len)
		{
			eval("g.enable"+k).value=data[k][0];
			eval("g.name"+k).value=data[k][1];
			eval("g.ip"+k).value=data[k][2];
			eval("g.start_port"+k).value=data[k][3];
			eval("g.end_port"+k).value=data[k][4];
			eval("g.protocol"+k).value=data[k][5];
			<?if($modelname=="WBR-2310"){echo "eval(\"g.schedule\"+k).value=data[k][6];\n";}?>
		}
		else
		{	
			eval("g.enable"+k).value="";
			eval("g.name"+k).value="";
			eval("g.ip"+k).value="";
			eval("g.start_port"+k).value="";
			eval("g.end_port"+k).value="";
			eval("g.protocol"+k).value="0";
			<?if($modelname=="WBR-2310"){echo "eval(\"g.schedule\"+k).value=\"0\";\n";}?>
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
<form name="postfrm" id="postfrm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return check()">
<input type="hidden" name="ACTION_POST" value="SOMETHING">
<script>
for(i=1; i<rules; i++)
{
	document.write("<input type=hidden name='enable"+i+"' value=''>");
	document.write("<input type=hidden name='name"+i+"' value=''>");
	document.write("<input type=hidden name='ip"+i+"' value=''>");
	document.write("<input type=hidden name='start_port"+i+"' value=''>");
	document.write("<input type=hidden name='end_port"+i+"' value=''>");
	document.write("<input type=hidden name='protocol"+i+"' value=''>");
	<?if($modelname=="WBR-2310"){echo "document.write(\"<input type=hidden name='schedule\"+i+\"' value=''>\");\n";}?>
}
</script>
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
</form>
<!-- ________________________________ Main Content Start ______________________________ -->
<form name="frm" id="frm">
		<div class="box">
			<h2><?=$MAX_RULES?> - <?=$m_title_app_rules?></h2>
			<table borderColor=#ffffff cellSpacing=1 cellPadding=2 width=525 bgColor=#dfdfdf border=1>
			<tbody>
			<tr>
				<td align=middle width=20>&nbsp;</td>
				<td width="130">&nbsp;</td>
				<td width="226" align=left><div align="left">&nbsp;</div></td>
				<td width="44" align=middle><div align="center"><?=$m_port?></div></td>
				<td width="67" align=middle><div align="center"><?=$m_traffic_type?></div></td>
				<?if($modelname=="WBR-2310")
				{echo "<td width=\"69\" align=middle><div align=\"center\">".$m_schedule."</div></td>\n";}?>
			</tr><?
			
			$index=0;
			while($index < $MAX_RULES)
			{		
				$index++;
				require("/www/__adv_port.php");	
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
