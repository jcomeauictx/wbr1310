<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="tools_time";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="tools";
/* --------------------------------------------------------------------------- */
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");

	echo "<!--\n";
	echo "tzone=".$tzone."\n";
	echo "daylight=".$daylight."\n";
	echo "sync=".$sync."\n";
	echo "interval=".$interval."\n";
	echo "ntp=".$ntp."\n";
	echo "year=".$year."\n";
	echo "mon=".$mon."\n";
	echo "day=".$day."\n";
	echo "hour=".$hour."\n";
	echo "min=".$min."\n";
	echo "sec=".$sec."\n";

	$dirty=0;
	if (query("/time/timezone")!=$tzone)			{$dirty++; set("/time/timezone", $tzone); }
	if (query("/time/daylightsaving")!=$daylight)	{$dirty++; set("/time/daylightsaving", $daylight); }
	if (query("/time/syncwith")!=$sync)				{$dirty++; set("/time/syncwith", $sync); }
	if ($sync == "2")
	{
		if (query("/time/ntpserver/ip")!=$ntp)				{$dirty++; set("/time/ntpserver/ip", $ntp); }
		if (query("/time/ntpserver/interval")!=$interval)	{$dirty++; set("/time/ntpserver/interval", $interval); }
		if ($dirty > 0) { $SUBMIT_STR=";submit TIME"; }
	}
	else
	{
		$XGISET_STR="setPath=/runtime/time/";
		$XGISET_STR=$XGISET_STR."&date=".$mon."/".$day."/".$year;
		$XGISET_STR=$XGISET_STR."&time=".$hour.":".$min.":".$sec;
		$XGISET_STR=$XGISET_STR."&endSetPath=1";
		if ($dirty > 0) { $SUBMIT_STR=";submit TIME"; }
		$dirty++;
	}
	echo "-->\n";
	$NEXT_PAGE=$MY_NAME;
	if($dirty > 0)	{require($G_SAVING_URL);}
	else			{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
$cfg_date	= query("/runtime/time/date");
$cfg_time	= query("/runtime/time/time");
$cfg_tzone	= query("/time/timezone");
$cfg_ds		= query("/time/daylightsaving");
$cfg_sync	= query("/time/syncwith");
$cfg_interval	= query("/time/ntpserver/interval");
$cfg_ntp_server	= get(h,"/time/ntpserver/ip");

/* --------------------------------------------------------------------------- */
?>

<script>

function on_click_ntp()
{
	var f = get_obj("frm");

	/* ntp part */
	f.interval.disabled = f.ntp.disabled =
	f.ntp_btn.disabled = f.page_ntp_servers.disabled =
	f.time_type.checked ? false : true;
	/* manual part */
	f.year.disabled = f.mon.disabled = f.day.disabled =
	f.hour.disabled = f.min.disabled = f.sec.disabled =
	f.set.disabled = f.time_type.checked ? true : false;
}

function set_time()
{
	var date = new Date();

	select_index(get_obj("year"), date.getFullYear());

	get_obj("mon").selectedIndex = date.getMonth();
	get_obj("day").selectedIndex = date.getDate() - 1;
	get_obj("hour").selectedIndex = date.getHours(); 
	get_obj("min").selectedIndex = date.getMinutes(); 
	get_obj("sec").selectedIndex = date.getSeconds();
}

function copy_ntp_server()
{
	if (get_obj("page_ntp_servers").selectedIndex > 0)
	{
		get_obj("ntp").value = get_obj("page_ntp_servers").value;
	}
}

/* page init functoin */
function init()
{
	var f=get_obj("frm");
	// init here ...

	set_time();
	on_click_ntp();
}
/* parameter checking */
function check()
{
	var f=get_obj("frm");
	// do check here ....
	if (f.time_type.checked)
	{
		f.sync.value = "2";
		if (is_blank(f.ntp.value) || strchk_hostname(f.ntp.value)==false)
		{
			alert("<?=$a_invalid_ntp_server?>");
			field_focus(f.ntp, "**");
			return false;
		}
	}
	else
	{
		f.sync.value = "0";
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
			<h2><?=$m_title_time?></h2>
			<table width="525">
			<tr>
				<td class="r_tb" width="150"><?=$m_time?>&nbsp;:</td>
				<td class="l_tb" width="367">&nbsp;
					<strong><?=$cfg_date?>&nbsp;<?=$cfg_time?></strong>
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_time_zone?>&nbsp;:</td>
				<td class="l_tb">&nbsp;
					<select size=1 name=tzone id=tzone>
<?
						for ("/tmp/tz/zone")
						{
							echo "<option value=".$@;
							if ($cfg_tzone==$@) {echo " selected";}
							echo ">".get(h,"name")."</option>\n";
						}
?>					</select>
				</td>
			</tr>
			<tr>
				<td class="r_tb"><?=$m_enable_daylight_saving?>&nbsp;:</td>
				<td class="l_tb">&nbsp;
					<input type=checkbox name="daylight" id="daylight" value="1"<?if ($cfg_ds=="1"){echo " checked";}?>>
				</td>
			</tr>
			</table>
		</div>
		<div class=box>
			<h2><?=$m_title_ntp?></h2>
			<table width="525">
			<tr>
				<td class="r_tb" width="145"><?=$m_enable_ntp?>&nbsp;:</td>
				<td class="l_tb" width="367">&nbsp;
					<input name="time_type" type=checkbox id="time_type" value="1" onclick="on_click_ntp();"<?
					if ($cfg_sync=="2") {echo " checked";}
					?>>
					<input type=hidden name=sync id=sync>
				</td>
			</tr>
			<tr>
				<td class="r_tb" width="145"><?=$m_interval?>&nbsp;:</td>
				<td class="l_tb">&nbsp;
					<select id="interval" name="interval" size="1">
						<option value="604800"<?if($cfg_interval==604800){echo " selected";}?>>7 <?=$m_days?></option>
						<option value="259200"<?if($cfg_interval==259200){echo " selected";}?>>3 <?=$m_days?></option>
						<option value="86400"<?if($cfg_interval==86400){echo " selected";}?>>1 <?=$m_day?></option>
					</select>
				</td>
			</tr>
			<tr>
				<td class="r_tb" width="145"><?=$m_ntp_server?>&nbsp;:</td>
				<td class="l_tb">&nbsp;
					<input name="ntp" id="ntp" value="<?=$cfg_ntp_server?>">
					<span>
					<input name="ntp_btn" id="ntp_btn" type=button value="<<" onClick="copy_ntp_server()">
					</span>
					<select name=page_ntp_servers id=page_ntp_servers>
						<option value=0><?=$m_select_ntp_server?></option>
						<option value=ntp1.dlink.com>ntp1.dlink.com</option>
						<option value=ntp.dlink.com.tw>ntp.dlink.com.tw</option>
					</select>
				</td>
			</tr>
			</table>
		</div>
		<div class=box>
			<h2><?=$m_title_manual?></h2>
			<P class=box_msg></P>
			<table width="525" cellpadding="1" cellspacing="1">
			<tbody>
			<tr>
				<td class="r_tb" width="145"><?=$m_current_time?>&nbsp;:</td>
				<td class="l_tb"><b>&nbsp;</b></td>
			</tr>
			<tr>
				<td colspan=2>
					<table width="100%" border=0 cellpadding=2 cellspacing=0>
					<tbody>
					<tr>
						<td colspan="2">&nbsp;</td>
						<td colspan="4">&nbsp;</td>
					</tr>
					<tr>
						<td><?=$m_year?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select id="year" name="year" size=1 style="WIDTH: 50px">
<?
								$i=2001;
								while ($i<2012)
								{
									$i++;
									echo "<option value=".$i.">".$i."</option>\n";
								}

?>							</select>
						</font></td>
						<td><?=$m_month?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select id="mon" name="mon" size=1 style="WIDTH: 50px">
								<option value=1>Jan</option>
								<option value=2>Feb</option>
								<option value=3>Mar</option>
								<option value=4>Apr</option>
								<option value=5>May</option>
								<option value=6>Jun</option>
								<option value=7>Jul</option>
								<option value=8>Aug</option>
								<option value=9>Sep</option>
								<option value=10>Oct</option>
								<option value=11>Nov</option>
								<option value=12>Dec</option>
							</select>
						</font></td>
						<td><?=$m_day?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select size=1 id="day" name="day" style="WIDTH: 50px">
<?
								$i=0;
								while ($i<31)
								{
									$i++;
									echo "<option value=".$i.">".$i."</option>\n";
								}
?>
							</select>
						</font></td>
					</tr>
					<tr>
						<td><?=$m_hour?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select size=1 id="hour" name="hour" style="WIDTH: 50px">
<?
								$i=0;
								while ($i<24)
								{
									echo "<option value=".$i.">".$i."</option>\n";
									$i++;
								}
?>
							</select>
						</font></td>
						<td><?=$m_minute?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select size=1 id="min" name="min" style="WIDTH: 50px">
<?
								$i=0;
								while ($i<60)
								{
									echo "<option value=".$i.">".$i."</option>\n";
									$i++;
								}
?>
							</select>
						</font></td>
						<td><?=$m_second?></td>
						<td><font face="Arial, Helvetica, sans-serif" size=2>
							<select size=1 id="sec" name="sec" style="WIDTH: 50px">
<?
								$i=0;
								while ($i<60)
								{
									echo "<option value=".$i.">".$i."</option>\n";
									$i++;
								}
?>
							</select>
						</font></td>
					</tr>
					</tbody>
					</table>
					<br>
					<input name="set" id="set" type=button class=button_submit onClick="set_time()" value="<?=$m_copy_pc_time?>">
				</td>
			</tr>
			</tbody>
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
