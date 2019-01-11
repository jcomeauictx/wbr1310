<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="adv_wlan";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="adv";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	
	$db_dirty=0;
	anchor("/wireless");
	if(query("txrate")			!=$tx_rate)			{set("txrate", $tx_rate);			$db_dirty++;}
	if(query("txpower")			!=$tx_power)		{set("txpower", $tx_power);			$db_dirty++;}
	if(query("beaconinterval")	!=$bi)				{set("beaconinterval", $bi);		$db_dirty++;}
	if(query("rtslength")		!=$rts)				{set("rtslength", $rts);			$db_dirty++;}
	if(query("fraglength")		!=$frag)			{set("fraglength", $frag);			$db_dirty++;}
	if(query("dtim")			!=$dtim)			{set("dtim", $dtim);				$db_dirty++;}
	if(query("preamble")		!=$preamble_type)	{set("preamble", $preamble_type);	$db_dirty++;}
	if(query("ctsmode")			!=$cts_mode)		{set("ctsmode", $cts_mode);			$db_dirty++;}
	if(query("wmm/enable")		!=$wmm_mode)		{set("wmm/enable", $wmm_mode);		$db_dirty++;}


	if($db_dirty > 0)	{$SUBMIT_STR="submit WLAN";}
	else				{$SUBMIT_STR="";}

	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
anchor("/wireless");
$tx_rate	=query("txrate");
$tx_power	=query("txpower");
$bi			=query("beaconInterval");
$rts		=query("rtsLength");
$frag		=query("fragLength");
$dtim		=query("dtim");
$preamble	=query("preamble");
$cts		=query("ctsmode");
$wmm		=query("wmm/enable");
/* --------------------------------------------------------------------------- */
?>

<script>
/* page init functoin */
function init()
{
	var f=get_obj("frm");
	select_index(f.tx_rate, "<?=$tx_rate?>");
	chg_cts_mode();
	select_index(f.tx_power, "<?=$tx_power?>");
	f.bi.value="<?=$bi?>";
<?
	if($cts!=""){echo "f.cts[".$cts."].checked=true;\n";}
?>	
	f.rts.value="<?=$rts?>";
	f.frag.value="<?=$frag?>";
	f.dtim.value="<?=$dtim?>";
	if("<?=$preamble?>"=="2")	f.preamble[1].checked=true;
	else						f.preamble[0].checked=true;
	if("<?=$wmm?>"=="1")		f.wmm[1].checked=true;
	else						f.wmm[0].checked=true;
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	if(!is_in_range(f.bi.value,20,1000))
	{
		alert("<?=$a_invalid_bi?>");
		if(f.bi.value=="") f.bi.value=100;
		field_select(f.bi);
		return false;
	}
	if(!is_in_range(f.rts.value,256,2346))
	{
		alert("<?=$a_invalid_rts?>");
		if(f.rts.value=="") f.rts.value=2346;
		field_select(f.rts);
		return false;
	}
	if(!is_in_range(f.frag.value,1500,2346))
	{
		alert("<?=$a_invalid_frag?>");
		if(f.frag.value=="") f.frag.value=2346;
		field_select(f.frag);
		return false;
	}
	if(decstr2int(f.frag.value)%2)
	{
		alert("<?=$a_frag_should_be_even?>");
		field_select(f.frag);
		return false;
	}
	if(!is_in_range(f.dtim.value,1,255))
	{
		alert("<?=$a_invalid_dtim?>");
		if(f.dtim.value=="") f.dtim.value=1;
		field_select(f.dtim);
		return false;
	}
	f.preamble_type.value=(f.preamble[0].checked?1:2);
	if(f.cts[0].checked)		f.cts_mode.value=0;
	else if(f.cts[1].checked)	f.cts_mode.value=1;
	else if(f.cts[2].checked)	f.cts_mode.value=2;
	f.wmm_mode.value=(f.wmm[0].checked?0:1);
	return true;
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}
function chg_cts_mode()
{
	var f=get_obj("frm");
	var dis;
	if(f.tx_rate.value!="0")	{	dis=true;	f.cts[0].checked=true;	}
	else						{	dis=false;	f.cts[2].checked=true;	}
	f.cts[0].disabled=(!dis);
	f.cts[1].disabled=dis;
	f.cts[2].disabled=dis;
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
			<h2><?=$m_context_title?></h2>
			<table width=525>
			<tr>
				<td class=r_tb width=27%><?=$m_tx_rate?> :</td>
				<td width=73%>
				<select name="tx_rate" onchange="chg_cts_mode()">
					<option value="0"><?=$m_auto?></option>
					<option value="1">1</option>
					<option value="2">2</option>
					<option value="5.5">5.5</option>
					<option value="6">6</option>
					<option value="9">9</option>
					<option value="11">11</option>
					<option value="12">12</option>
					<option value="18">18</option>
					<option value="24">24</option>
					<option value="36">36</option>
					<option value="48">48</option>
					<option value="54">54</option>
				</select>
				</td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_tx_power?> :</td>
				<td>
				<select name="tx_power">
				<option value="1">100%</option>
				<option value="2">50%</option>
				<option value="3">25%</option>
				<option value="4">12.5%</option>
				</select>
				</td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_beacon_interval?> :</td>
				<td><input maxlength=4 name=bi size=4 value=""><?=$m_dsc_bi?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_rts?> :</td>
				<td><input maxlength=4 name=rts size=4 value=""><?=$m_dsc_rts?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_frag?> :</td>
				<td><input maxlength=4 name=frag size=4 value=""><?=$m_dsc_frag?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_dtim?> :</td>
				<td><input maxlength=3 name=dtim size=4 value=""><?=$m_dsc_dtim?></td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_preamble?> :</td>
				<td>
				<input type=radio name=preamble value=1><?=$m_preamble_short?>
				<input type=radio name=preamble value=0><?=$m_preamble_long?>
				</td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_cts?> :</td>
				<td>
				<input type=radio name=cts value=0><?=$m_none?>
				<input type=radio name=cts value=1><?=$m_always?>
				<input type=radio name=cts value=2><?=$m_auto?>
				</td>
			</tr>
			<tr>
				<td class=r_tb><?=$m_wmm?> :</td>
				<td>
				<input type=radio name=wmm value=0><?=$m_disable?>
				<input type=radio name=wmm value=1><?=$m_enable?>
				</td>
			</tr>
			</table>
		</div>
		<input type=hidden name=preamble_type value="">
		<input type=hidden name=cts_mode value="">
		<input type=hidden name=wmm_mode value="">
<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
