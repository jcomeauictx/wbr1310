<?
/* vi: set sw=4 ts=4: ---------------------------------------------------------*/
$MY_NAME		="bsc_wlan";
$MY_MSG_FILE	=$MY_NAME.".php";
$CATEGORY		="bsc";
/* --------------------------------------------------------------------------- */
$def_password	="XxXxXxXx";
if ($ACTION_POST!="")
{
	require("/www/model/__admin_check.php");
	echo "<!--\n";
	echo "f_enable=".			$f_enable.			"\n";
	echo "f_ssid=".				$f_ssid.			"\n";
	echo "f_channel=".			$f_channel.			"\n";
	echo "f_auto_channel=".		$f_auto_channel.	"\n";
	echo "f_g_only=".			$f_g_only.			"\n";
	echo "f_super_g=".			$f_super_g.			"\n";
	echo "f_xr=".				$f_xr.				"\n";
	echo "f_ap_hidden=".		$f_ap_hidden.		"\n";
	echo "f_authentication=".	$f_authentication.	"\n";
	echo "f_cipher=".			$f_cipher.			"\n";
	echo "f_wep_len=".			$f_wep_len.			"\n";
	echo "f_wep_format=".		$f_wep_format.		"\n";
	echo "f_wep_def_key=".		$f_wep_def_key.		"\n";
	echo "f_wep1=".				$f_wep1.			"\n";
	echo "f_wep2=".				$f_wep2.			"\n";
	echo "f_wep3=".				$f_wep3.			"\n";
	echo "f_wep4=".				$f_wep4.			"\n";
	echo "f_wpa_psk=".			$f_wpa_psk.			"\n";
	echo "f_radius_ip1=".		$f_radius_ip1.		"\n";
	echo "f_radius_port1=".		$f_radius_port1.	"\n";
	echo "f_radius_secret1=".	$f_radius_secret1.	"\n";
	echo "f_radius_ip2=".		$f_radius_ip2.		"\n";
	echo "f_radius_port2=".		$f_radius_port2.	"\n";
	echo "f_radius_secret2=".	$f_radius_secret2.	"\n";
	echo "-->\n";
	
	/*
	if($db_dirty > 0)	{$SUB_STR="submit xxxx";	$div="";}
	if($db_dirty2 > 0)	{$SUB_STR=$SUB_STR.$div."submit oooo";}
	*/
	anchor("/wireless");
	$db_dirty=0;
	if(query("enable")!=$f_enable)	{set("enable",$f_enable); $db_dirty++;	}
	if($f_enable=="1")
	{
		if(query("jumpstart/enable")== "1")	{set("jumpstart/enable","0"); set("jumpstart/phase","1");	$db_dirty++;}
		if(query("enable")			!= $f_enable)			{set("enable",			$f_enable);			$db_dirty++;}
		if(query("ssid")			!= $f_ssid)				{set("ssid",			$f_ssid);			$db_dirty++;}
		if(query("channel")			!= $f_channel)			{set("channel",			$f_channel);		$db_dirty++;}
		if(query("autochannel")		!= $f_auto_channel)		{set("autochannel",		$f_auto_channel);	$db_dirty++;}
		if(query("wlmode")			!= $f_g_only)			{set("wlmode",			$f_g_only);			$db_dirty++;}
		if(query("ssidhidden")		!= $f_ap_hidden)		{set("ssidhidden",		$f_ap_hidden);		$db_dirty++;}
		if(query("authentication")	!= $f_authentication)	{set("authentication",	$f_authentication);	$db_dirty++;}
		if(query("wpa/wepmode")		!= $f_cipher)			{set("wpa/wepmode",		$f_cipher);			$db_dirty++;}

		if(query("/function/normal_g")!="1")
		{
			if($f_super_g=="")	{$f_super_g="0";}
			if($f_xr!="1")		{$f_xr="0";}
			if(query("supergmode")!=$f_super_g)	{set("supergmode",	$f_super_g);	$db_dirty++;}
			if(query("xr")!=$f_xr)				{set("xr",			$f_xr);			$db_dirty++;}
		}

		if($f_cipher=="1")	//wep key
		{
			if(query("keylength")	!= $f_wep_len)		{set("keylength",	$f_wep_len);	$db_dirty++;}
			if(query("keyformat")	!= $f_wep_format)	{set("keyformat",	$f_wep_format);	$db_dirty++;}
			if(query("defkey")		!= $f_wep_def_key)	{set("defkey",		$f_wep_def_key);$db_dirty++;}
			if(query("wepkey:1")	!= $f_wep1)			{set("wepkey:1",	$f_wep1); 		$db_dirty++;}
			if(query("wepkey:2")	!= $f_wep2)			{set("wepkey:2",	$f_wep2); 		$db_dirty++;}
			if(query("wepkey:3")	!= $f_wep3)			{set("wepkey:3",	$f_wep3);		$db_dirty++;}
			if(query("wepkey:4")	!= $f_wep4)			{set("wepkey:4",	$f_wep4);		$db_dirty++;}
		}
		
		if($f_authentication=="2" || $f_authentication=="4" || $f_authentication=="6")	// wpa series
		{
			if(query("wpa/radiusserver")	!= $f_radius_ip1)	{set("wpa/radiusserver",	$f_radius_ip1);		$db_dirty++;}
			if(query("wpa/radiusport")		!= $f_radius_port1)	{set("wpa/radiusport",		$f_radius_port1);	$db_dirty++;}
			if($f_radius_secret1 != $def_password)
			{ if(query("wpa/radiussecret")	!= $f_radius_secret1){set("wpa/radiussecret",	$f_radius_secret1);	$db_dirty++;}}

			if(query("wpa/radiusserver2")	!= $f_radius_ip2)	{set("wpa/radiusserver2",	$f_radius_ip2);		$db_dirty++;}
			if(query("wpa/radiusport2")		!= $f_radius_port2)	{set("wpa/radiusport2",		$f_radius_port2);	$db_dirty++;}
			if($f_radius_secret2 != $def_password)
			{ if(query("wpa/radiussecret2")	!= $f_radius_secret2){set("wpa/radiussecret2",	$f_radius_secret2);	$db_dirty++;}}
		}
		else if($f_authentication=="3" || $f_authentication=="5" || $f_authentication=="7")// wpa psk series
		{
			if($f_wpa_psk!=$def_password)
			{
				if(query("wpa/wpapsk") != $f_wpa_psk)	{set("wpa/wpapsk", $f_wpa_psk);		$db_dirty++;}
			}
		}
	}

	if($db_dirty > 0)	{$SUBMIT_STR="submit WLAN";}
	$NEXT_PAGE=$MY_NAME;
	if($SUBMIT_STR!="")	{require($G_SAVING_URL);}
	else				{require($G_NO_CHANGED_URL);}
}

/* --------------------------------------------------------------------------- */
require("/www/model/__html_head.php");
require("/www/comm/__js_ip.php");
/* --------------------------------------------------------------------------- */
// get the variable value from rgdb.
anchor("/wireless");
$cfg_enable		= query("enable");
$cfg_ssid		= get("j", "ssid");
if(query("/wireless/jumpstart/enable")=="1" && query("/wireless/jumpstart/phase")=="1")
{$cfg_ssid		= get("j","/runtime/wireless/ssid");}
$cfg_channel	= query("channel");
$cfg_autochann	= query("autochannel");
$cfg_wlmode		= query("wlmode");
$cfg_ssidhidden	= query("ssidhidden");

$cfg_auth		= query("authentication");
$cfg_cipher		= query("wpa/wepmode");
if		($cfg_cipher=="0")					{$security_type="0";}
else if	($cfg_cipher=="1")					{$security_type="1";}
if		($cfg_auth=="2" || $cfg_auth=="3")	{$security_type="2";}
else if ($cfg_auth=="4" || $cfg_auth=="5")	{$security_type="3";}
else if ($cfg_auth=="6" || $cfg_auth=="7")	{$security_type="4";}
$psk_eap_type="psk";
if($cfg_auth=="2" || $cfg_auth=="4" || $cfg_auth=="6"){$psk_eap_type="eap";}

$cfg_wep_length = query("keylength");
$cfg_wep_format = query("keyformat");
$wep_prefix="hex_wep_64_";
if($cfg_wep_length=="128")
{
	if($cfg_wep_format=="1"){$wep_prefix="asc_wep_128_";}
	else{$wep_prefix="hex_wep_128_";}
}
else
{
	if($cfg_wep_format=="1"){$wep_prefix="asc_wep_64_";}
	else{$wep_prefix="hex_wep_64_";}
}

$cfg_wep_index	= query("defkey");
$cfg_wep1		= get("j","wepkey:1");
$cfg_wep2		= get("j","wepkey:2");
$cfg_wep3		= get("j","wepkey:3");
$cfg_wep4		= get("j","wepkey:4");
$cfg_radius_ip1		= query("wpa/radiusserver");
$cfg_radius_port1	= query("wpa/radiusport");
if(query("wpa/radiussecret")=="")	{$cfg_radius_sec1= "";}
else								{$cfg_radius_sec1=$def_password;}
$cfg_radius_ip2		= query("wpa/radiusserver2");
$cfg_radius_port2	= query("wpa/radiusport2");
if(query("wpa/radiussecret2")=="")	{$cfg_radius_sec2= "";}
else								{$cfg_radius_sec2=$def_password;}

if(query("wpa/wpapsk")=="")			{$cfg_wpapsk="";}
else								{$cfg_wpapsk=$def_password;}

$td1			="<td class='r_tb' width='200'>";
$td2			="<td class='l_tb'>";
$td3			="<td class='r_tb' width='120'>";
$symbol			=" :&nbsp;";
$symbol2		=" :&nbsp;&nbsp;";
/* --------------------------------------------------------------------------- */
?>

<script>
function print_keys(key_name, max_length)
{
	var str="";
	var field_size=decstr2int(max_length)+5;
	var i=1;
	str="<table>";
	for(i=1; i<=4; i++)
	{
		str+="	<tr>";
		str+="		<?=$td1?><?=$m_wep_key?> "+i+" :</td>";
		str+="		<?=$td2?><input type='text' id='"+key_name+i+"' name='"+key_name+i+"' maxlength='"+max_length+"' size='"+field_size+"' value=''></td>";
		str+="	</tr>";
	}
	str+="</table>";
	document.write(str);
}
function on_change_security_type()
{
	var sec_type = get_obj("security_type");

	get_obj("show_wep").style.display = "none";
	get_obj("show_wpa").style.display = "none";

	if (sec_type.value == 1)
	{
		get_obj("show_wep").style.display = "";
		chg_wep_type();
	}
	else if(sec_type.value >= 2)
	{
		get_obj("show_wpa").style.display = "";
		
		get_obj("title_wpa").style.display			= "none";
		get_obj("title_wpa2").style.display			= "none";
		get_obj("title_wpa2_auto").style.display	= "none";
		if(sec_type.value == 2)		get_obj("title_wpa").style.display		= "";
		if(sec_type.value == 3)		get_obj("title_wpa2").style.display		= "";
		if(sec_type.value == 4)		get_obj("title_wpa2_auto").style.display= "";
		chg_psk_eap();
	}
}
function chg_wep_type()
{
	var f=get_obj("frm_wep");
	get_obj("hex_wep_64").style.display		= "none";
	get_obj("hex_wep_128").style.display	= "none";
	get_obj("asc_wep_64").style.display		= "none";
	get_obj("asc_wep_128").style.display	= "none";

	if(f.wep_key_len.value=="128")
	{
		if(f.wep_key_type.value=="1")	get_obj("asc_wep_128").style.display	= "";
		else	get_obj("hex_wep_128").style.display = "";
	}
	else
	{
		if(f.wep_key_type.value=="1")	get_obj("asc_wep_64").style.display	= "";
		else	get_obj("hex_wep_64").style.display	= "";
	}
}
function chg_psk_eap()
{
	var wpa_type = get_obj("psk_eap");
	get_obj("psk_setting").style.display = "none";
	get_obj("eap_setting").style.display = "none";
	if(wpa_type.value=="psk")	{	get_obj("psk_setting").style.display = "";	}
	else						{	get_obj("eap_setting").style.display = "";	}
}
function on_check_enable()
{
	var f = get_obj("frm");
	if (f.enable.checked)
	{
		f.ssid.disabled = false;
		if (f.autochann.checked) f.channel.disabled = true;
		else f.channel.disabled = false;
		f.autochann.disabled = false;
		f.gonly.disabled = false;
		f.aphidden.disabled = false;
		f.security_type.disabled =false;
		<?
		if(query("/function/normal_g")!="1")
		{
			echo "f.super_g_mode.disabled=f.en_xr.disabled=false;\n";
			echo "chg_super_g();\n";
		}
		?>
		select_index(f.security_type, "<?=$security_type?>");
		on_change_security_type();
	}
	else
	{
		select_index(f.security_type,"0");
		on_change_security_type();
		fields_disabled(f, true);
		f.enable.disabled=false;
	}
}

function on_check_autochann()
{
	var f = get_obj("frm");
	f.channel.disabled = f.autochann.checked;
}

/* page init functoin */
function init()
{
	var f=get_obj("frm");
	var f_wep = get_obj("frm_wep");
	var f_wpa = get_obj("frm_wpa");
	// init here ...
	f.enable.checked = <? if ($cfg_enable=="1") {echo "true";} else {echo "false";} ?>;
	f.ssid.value = "<?=$cfg_ssid?>";
	select_index(f.channel, "<?=$cfg_channel?>");
	f.autochann.checked = <? if ($cfg_autochann=="1") {echo "true";} else {echo "false";}?>;
	f.gonly.checked = <? if ($cfg_wlmode=="2") {echo "true";} else {echo "false";}?>;
	f.aphidden.checked = <? if ($cfg_ssidhidden=="1") {echo "true";} else {echo "false";}?>;
	select_index(f.security_type, "<?=$security_type?>");
	
	//wep
	select_index(f_wep.auth_type,	"<?=$cfg_auth?>");
	select_index(f_wep.wep_key_len,	"<?=$cfg_wep_length?>");
	select_index(f_wep.wep_key_type,"<?=$cfg_wep_format?>");
	select_index(f_wep.wep_def_key, "<?=$cfg_wep_index?>");
	f_wep.<?=$wep_prefix?>1.value=	"<?=$cfg_wep1?>";
	f_wep.<?=$wep_prefix?>2.value=	"<?=$cfg_wep2?>";
	f_wep.<?=$wep_prefix?>3.value=	"<?=$cfg_wep3?>";
	f_wep.<?=$wep_prefix?>4.value=	"<?=$cfg_wep4?>";

	//wpa
	select_index(f_wpa.cipher_type,	"<?=$cfg_cipher?>");
	select_index(f_wpa.psk_eap,		"<?=$psk_eap_type?>");
	f_wpa.wpapsk1.value		="<?=$cfg_wpapsk?>";
	f_wpa.wpapsk2.value		="<?=$cfg_wpapsk?>";
	
	f_wpa.srv_ip1.value		="<?=$cfg_radius_ip1?>";
	f_wpa.srv_port1.value	="<?=$cfg_radius_port1?>";
	f_wpa.srv_sec1.value	="<?=$cfg_radius_sec1?>";
	f_wpa.srv_ip2.value		="<?=$cfg_radius_ip2?>";
	f_wpa.srv_port2.value	="<?=$cfg_radius_port2?>";
	f_wpa.srv_sec2.value	="<?=$cfg_radius_sec2?>";

	on_check_autochann();
	on_change_security_type();

	<?
	if(query("/function/normal_g")!="1")
	{
		echo "get_obj('show_super_g').style.display = '';\n";
		echo "select_index(f.super_g_mode, '".query("/wireless/supergmode")."');\n";
		echo "f.en_xr.checked=";	map("/wireless/xr","1","true",*,"false");	echo "\n";
	}
	?>
	on_check_enable();
}
function append_zero(len)
{
	var x_zero="";
	var i=0;
	for(i=0; i<len;i++)
	{
		x_zero+="0";
	}
	return x_zero;	
}
/* parameter checking */
function check()
{
	var f		=get_obj("frm");
	var f_wep	=get_obj("frm_wep");
	var f_wpa	=get_obj("frm_wpa");
	var f_final	=get_obj("final_form");

	var err_len_hex_wep_128_1	= "<?=$a_invalid_len_h_128_wep1?>";
	var err_len_hex_wep_128_2	= "<?=$a_invalid_len_h_128_wep2?>";
	var err_len_hex_wep_128_3	= "<?=$a_invalid_len_h_128_wep3?>";
	var err_len_hex_wep_128_4	= "<?=$a_invalid_len_h_128_wep4?>";
	
	var err_len_hex_wep_64_1	= "<?=$a_invalid_len_h_64_wep1?>";
	var err_len_hex_wep_64_2	= "<?=$a_invalid_len_h_64_wep2?>";
	var err_len_hex_wep_64_3	= "<?=$a_invalid_len_h_64_wep3?>";
	var err_len_hex_wep_64_4	= "<?=$a_invalid_len_h_64_wep4?>";

	var err_len_asc_wep_128_1	= "<?=$a_invalid_len_a_128_wep1?>";
	var err_len_asc_wep_128_2	= "<?=$a_invalid_len_a_128_wep2?>";
	var err_len_asc_wep_128_3	= "<?=$a_invalid_len_a_128_wep3?>";
	var err_len_asc_wep_128_4	= "<?=$a_invalid_len_a_128_wep4?>";

	var err_len_asc_wep_64_1	= "<?=$a_invalid_len_a_64_wep1?>";
	var err_len_asc_wep_64_2	= "<?=$a_invalid_len_a_64_wep2?>";
	var err_len_asc_wep_64_3	= "<?=$a_invalid_len_a_64_wep3?>";
	var err_len_asc_wep_64_4	= "<?=$a_invalid_len_a_64_wep4?>";

	var err_h_format_wep1	="<?=$a_invalid_wep1?>  <?=$a_valid_hex_char?>";
	var err_h_format_wep2	="<?=$a_invalid_wep2?>  <?=$a_valid_hex_char?>";
	var err_h_format_wep3	="<?=$a_invalid_wep3?>  <?=$a_valid_hex_char?>";
	var err_h_format_wep4	="<?=$a_invalid_wep4?>  <?=$a_valid_hex_char?>";
	
	var err_a_format_wep1	="<?=$a_invalid_wep1?>  <?=$a_valid_asc_char?>";
	var err_a_format_wep2	="<?=$a_invalid_wep2?>  <?=$a_valid_asc_char?>";
	var err_a_format_wep3	="<?=$a_invalid_wep3?>  <?=$a_valid_asc_char?>";
	var err_a_format_wep4	="<?=$a_invalid_wep4?>  <?=$a_valid_asc_char?>";
	
	f_final.f_enable.value			="";
	f_final.f_ssid.value			="";
	f_final.f_channel.value			="";
	f_final.f_auto_channel.value	="";
	f_final.f_super_g.value			="";
	f_final.f_xr.value				="";
	f_final.f_g_only.value			="";
	f_final.f_ap_hidden.value		="";
	f_final.f_authentication.value	="";
	f_final.f_cipher.value			="";
	f_final.f_wep_len.value			="";
	f_final.f_wep_format.value		="";
	f_final.f_wep_def_key.value		="";
	f_final.f_wep1.value			="";
	f_final.f_wep2.value			="";
	f_final.f_wep3.value			="";
	f_final.f_wep4.value			="";
	f_final.f_wpa_psk.value			="";
	f_final.f_radius_ip1.value		="";
	f_final.f_radius_port1.value	="";
	f_final.f_radius_secret1.value	="";
	f_final.f_radius_ip2.value		="";
	f_final.f_radius_port2.value	="";
	f_final.f_radius_secret2.value	="";

	if(f.enable.checked)	{	f_final.f_enable.value="1";	}
	else					{	f_final.f_enable.value="0";		return f_final.submit();}
	
	if(is_blank(f.ssid.value))
	{
		alert("<?=$a_empty_ssid?>");
		f.ssid.focus();
		return false;
	}
	if(strchk_unicode(f.ssid.value))
	{
		alert("<?=$a_invalid_ssid?>");
		f.ssid.select();
		return false;
	}

	// assign final post variables
	f_final.f_ssid.value			=f.ssid.value;
	f_final.f_channel.value			=f.channel.value;
	f_final.f_auto_channel.value	=(f.autochann.checked ? "1":"0");
	f_final.f_g_only.value			=(f.gonly.checked? "2":"1");
	f_final.f_ap_hidden.value		=(f.aphidden.checked ? "1":"0");
	
	if(<?map("/function/normal_g","1","0",*,"1");?>)
	{
		f_final.f_super_g.value=f.super_g_mode.value;
		f_final.f_xr.value=(f.en_xr.checked?1:0);
	}
	
	//open
	if(f.security_type.value=="0")
	{
		// assign final post variables
		f_final.f_authentication.value="0";
		f_final.f_cipher.value="0";
	}
	// open+wep / shared key
	else if(f.security_type.value=="1")
	{
		var test_wep_prefix;
		var test_len=10;
		var test_wep_obj;
		if(f_wep.wep_key_len.value=="128")
		{
			if(f_wep.wep_key_type.value=="1")	{test_wep_prefix="asc_wep_128_";	test_len=13;}
			else								{test_wep_prefix="hex_wep_128_";	test_len=26;}
		}
		else
		{
			if(f_wep.wep_key_type.value=="1")	{test_wep_prefix="asc_wep_64_";		test_len=5;}
			else								{test_wep_prefix="hex_wep_64_";		test_len=10;}
		}
		
		var i,j;
		for(i=1;i<=4; i++)
		{
			test_wep_obj=get_obj(test_wep_prefix+i);
			
			if(test_wep_obj.value!="")
			{
				// check wep length
				if(test_wep_obj.value.length!=test_len)
				{
					alert(eval("err_len_"+test_wep_prefix+i));
					test_wep_obj.focus();
					return false;
				}
				// check wep format
				if(f_wep.wep_key_type.value=="1")	//ascii
				{
					if(strchk_unicode(test_wep_obj.value))
					{
						alert(eval("err_a_format_wep"+i));
						test_wep_obj.select();
						return false;
					}
					eval("f_final.f_wep"+i+".value=test_wep_obj.value");
				}
				else	//hex
				{
					var test_char;
					for(j=0; j<test_wep_obj.value.length; j++)
					{
						test_char=test_wep_obj.value.charAt(j);
						if( (test_char >= '0' && test_char <= '9') ||
							(test_char >= 'a' && test_char <= 'h') ||
							(test_char >= 'A' && test_char <= 'H'))
							continue;
					
						alert(eval("err_h_format_wep"+i));
						test_wep_obj.select();
						return false;
					}
					eval("f_final.f_wep"+i+".value=test_wep_obj.value");
				}
			}
			else if(i==f_wep.wep_def_key.value)
			{
				alert("<?=$a_empty_defkey?>");
				test_wep_obj.focus();
				return false;
			}
			else
			{
				// if wep key field is empty, assign default key as 000...
				test_wep_obj.value=append_zero(test_len);
			}
		}
		
		// assign final post variables
		f_final.f_authentication.value	=f_wep.auth_type.value;
		f_final.f_cipher.value			="1";
		f_final.f_wep_len.value			=f_wep.wep_key_len.value;
		f_final.f_wep_format.value		=f_wep.wep_key_type.value;
		f_final.f_wep_def_key.value		=f_wep.wep_def_key.value;
	}
	// wpa series 
	else if(f.security_type.value>="2")
	{
		if(f_wpa.psk_eap.value=="eap")
		{
			if(!is_valid_ip(f_wpa.srv_ip1.value,0))
			{
				alert("<?=$a_invalid_radius_ip1?>");
				f_wpa.srv_ip1.select();
				return false;
			}
			if(is_blank(f_wpa.srv_port1.value))
			{
				alert("<?=$a_invalid_radius_port1?>");
				f_wpa.srv_port1.focus();
				return false;
			}
			if(!is_valid_port_str(f_wpa.srv_port1.value))
			{
				alert("<?=$a_invalid_radius_port1?>");
				f_wpa.srv_port1.select();
				return false;
			}
			if(is_blank(f_wpa.srv_sec1.value))
			{
				alert("<?=$a_empty_radius_sec1?>");
				f_wpa.srv_sec1.focus();
				return false;
			}
			if(strchk_unicode(f_wpa.srv_sec1.value))
			{
				alert("<?=$a_invalid_radius_sec1?>");
				f_wpa.srv_sec1.select();
				return false;
			}
			if(f_wpa.srv_ip2.value!="")
			{
				if(!is_valid_ip(f_wpa.srv_ip2.value,0))
				{
					alert("<?=$a_invalid_radius_ip2?>");
					f_wpa.srv_ip2.select();
					return false;
				}
				if(is_blank(f_wpa.srv_port2.value))
				{
					alert("<?=$a_invalid_radius_port2?>");
					f_wpa.srv_port2.focus();
					return false;
				}
				if(!is_valid_port_str(f_wpa.srv_port2.value))
				{
					alert("<?=$a_invalid_radius_port2?>");
					f_wpa.srv_port2.select();
					return false;
				}
				if(is_blank(f_wpa.srv_sec2.value))
				{
					alert("<?=$a_empty_radius_sec2?>");
					f_wpa.srv_sec2.focus();
					return false;
				}
				if(strchk_unicode(f_wpa.srv_sec2.value))
				{
					alert("<?=$a_invalid_radius_sec2?>");
					f_wpa.srv_sec2.select();
					return false;
				}
			}
			else
			{
				f_wpa.srv_port2.value="";
				f_wpa.srv_sec2.value="";
			}
			
			// assign final post variables
			switch (f.security_type.value)
			{
			case "2":	f_final.f_authentication.value="2";		break;
			case "3":	f_final.f_authentication.value="4";		break;
			case "4":	f_final.f_authentication.value="6";		break;
			default	:	break;
			}
			f_final.f_cipher.value			=f_wpa.cipher_type.value;
			f_final.f_radius_ip1.value		=f_wpa.srv_ip1.value;
			f_final.f_radius_port1.value	=f_wpa.srv_port1.value;
			f_final.f_radius_secret1.value	=f_wpa.srv_sec1.value;
			f_final.f_radius_ip2.value		=f_wpa.srv_ip2.value;
			f_final.f_radius_port2.value	=f_wpa.srv_port2.value;
			f_final.f_radius_secret2.value	=f_wpa.srv_sec2.value;
		}
		else
		{
			if(f_wpa.wpapsk1.value.length <8 || f_wpa.wpapsk1.value.length > 63)
			{
				alert("<?=$a_invalid_psk_len?>");
				f_wpa.wpapsk2.value="";
				f_wpa.wpapsk1.select();
				return false;
			}
			if(f_wpa.wpapsk1.value!=f_wpa.wpapsk2.value)
			{
				alert("<?=$a_psk_not_match?>");
				f_wpa.wpapsk1.select();
				return false;
			}
			if(strchk_unicode(f_wpa.wpapsk1.value))
			{
				alert("<?=$a_invalid_psk?>");
				f_wpa.wpapsk1.select();
				return false;
			}
			
			// assign final post variables
			switch (f.security_type.value)
			{
			case "2":	f_final.f_authentication.value="3";		break;
			case "3":	f_final.f_authentication.value="5";		break;
			case "4":	f_final.f_authentication.value="7";		break;
			default	:	break;
			}
			f_final.f_cipher.value=f_wpa.cipher_type.value;
			f_final.f_wpa_psk.value=f_wpa.wpapsk1.value;
		}
	}
	else
	{
		alert("Unknown Authentication Type.");
		return false;
	}
	f_final.submit();
}
/* cancel function */
function do_cancel()
{
	self.location.href="<?=$MY_NAME?>.php?random_str="+generate_random_str();
}
function chg_super_g()
{
	f=get_obj("frm");
	var dis=false;
	if(f.super_g_mode.value=="2")
	{
		dis=true;
		select_index(f.channel,"6");
		f.autochann.checked=f.en_xr.checked=false;
	}
	f.channel.disabled=	f.autochann.disabled=f.en_xr.disabled=dis;
	if(f.autochann.checked)	f.channel.disabled=true;
}
</script>
<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="final_form" id="final_form" method="post" action="<?=$MY_NAME?>.php">
<input type="hidden" name="ACTION_POST"			value="final">
<input type="hidden" name="f_enable"			value="">
<input type="hidden" name="f_ssid"				value="">
<input type="hidden" name="f_channel"			value="">
<input type="hidden" name="f_auto_channel"		value="">
<input type="hidden" name="f_super_g"			value="">
<input type="hidden" name="f_xr"				value="">
<input type="hidden" name="f_g_only"			value="">
<input type="hidden" name="f_ap_hidden"			value="">
<input type="hidden" name="f_authentication"	value="">
<input type="hidden" name="f_cipher"			value="">
<input type="hidden" name="f_wep_len"			value="">
<input type="hidden" name="f_wep_format"		value="">
<input type="hidden" name="f_wep_def_key"		value="">
<input type="hidden" name="f_wep1"				value="">
<input type="hidden" name="f_wep2"				value="">
<input type="hidden" name="f_wep3"				value="">
<input type="hidden" name="f_wep4"				value="">
<input type="hidden" name="f_wpa_psk"			value="">
<input type="hidden" name="f_radius_ip1"		value="">
<input type="hidden" name="f_radius_port1"		value="">
<input type="hidden" name="f_radius_secret1"	value="">
<input type="hidden" name="f_radius_ip2"		value="">
<input type="hidden" name="f_radius_port2"		value="">
<input type="hidden" name="f_radius_secret2"	value="">
</form>
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
		?>
		<script>apply('check()'); echo ("&nbsp;"); cancel('');</script>
		</div>
<!-- ________________________________ Main Content Start ______________________________ -->
		<form name="frm" id="frm" method="post" action="<?=$MY_NAME?>.php" onsubmit="return false;">
		<div class="box">
			<h2><?=$m_title_wireless_setting?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<?=$td1?><?=$m_enable_wireless?></td>
				<?=$td2?><?=$symbol?>
					<input name="enable" id="enable" type="checkbox" onclick="on_check_enable()" value="1">
				<td>
			</tr>
			<tr>
				<?=$td1?><?=$m_wlan_name?></td>
				<?=$td2?><?=$symbol2?>
					<input name="ssid" id="ssid" type="text" size="20" maxlength="32" value="">
					&nbsp;<?=$m_wlan_name_comment?>
				</td>
			</tr>
			<tr>
				<?=$td1?><?=$m_wlan_channel?></td>
				<?=$td2?><?=$symbol2?>
					<select name="channel" id="channel">
						<?require("/www/bsc_wlan_channel.php");?>
					</select>
				</td>
			</tr>
			<tr>
				<?=$td1?><?=$m_enable_auto_channel?></td>
				<?=$td2?><?=$symbol?>
					<input name="autochann" id="autochann" type="checkbox" onclick="on_check_autochann()" value="1">
				<td>
			</tr>
			</table>
			<div id="show_super_g" style="display:none">
				<table cellpadding="1" cellspacing="1" border="0" width="525">
				<tr>
					<?=$td1?><?=$m_super_g?></td>
					<?=$td2?><?=$symbol2?>
					<select name=super_g_mode onchange='chg_super_g();'>
						<option value='0'><?=$m_disabled?></option>
						<option value='1'><?=$m_super_g_without_turbo?></option>
						<option value='2'><?=$m_super_g_with_d_turbo?></option>
					</select>
					</td>
				</tr>
				<tr>
					<?=$td1?><?=$m_xr?></td>
					<?=$td2?><?=$symbol?>
						<input type=checkbox name=en_xr>
					</td>
				</tr>
				</table>
			</div>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<?=$td1?><?=$m_11g_only?></td>
				<?=$td2?><?=$symbol?>
					<input name='gonly' id='gonly' type='checkbox' value='1'>
				<td>
			</tr>
			<tr>
				<?=$td1?><?=$m_enable_ap_hidden?></td>
				<?=$td2?><?=$symbol?>
					<input name="aphidden" id="aphidden" type="checkbox" value="1">&nbsp;<?=$m_ap_hidden_comment?>
				<td>
			</tr>
			</table>
		</div>
		<div class="box">
			<h2><?=$m_title_wireless_security?></h2>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<?=$td1?><?=$m_security_mode?></td>
				<?=$td2?><?=$symbol2?>
					<select id="security_type" name="security_type" onChange="on_change_security_type()">
						<option value="0" selected><?=$m_disable_security?></option>
						<option value="1"><?=$m_enable_wep?></option>
						<option value="2"><?=$m_wpa_security?></option>
						<option value="3"><?=$m_wpa2_security?></option>
						<option value="4"><?=$m_wpa2_auto_security?></option>
					</select>
				</td>
			</tr>
			</table>
		</div>
		</form>
		<!-- ****************************no wep*******************************************-->
		<form name="frm_nowep" id="frm_nowep" method="post" action="<?=$MY_NAME?>.php">
		<input type="hidden" name="ACTION_POST" value="NOWEP">
		</form>

		<!-- **************************** wep  *******************************************-->
		<form name="frm_wep" id="frm_wep" method="post" action="<?=$MY_NAME?>.php">
		<input type="hidden" name="ACTION_POST" value="WEP">
		<div class="box" id="show_wep" style="display:none">
			<h2><?=$m_title_wep?></h2>
			<?require($LOCALE_PATH."/bsc_wlan_msg1.php");?>
			<table cellpadding="1" cellspacing="1" border="0" width="525">
			<tr>
				<?=$td1?><?=$m_auth_type?></td>
				<?=$td2?><?=$symbol?>
					<select name="auth_type" id="auth_type">
						<option value="0"><?=$m_open?></option>
						<option value="1"><?=$m_shared_key?></option>
					</select>
				</td>
			</tr>
			<tr>
				<?=$td1?><?=$m_wep_key_len?></td>
				<?=$td2?><?=$symbol?>
					<select id="wep_key_len" name="wep_key_len" size=1 onChange="chg_wep_type()">
						<option value="64"><?=$m_64bit_wep?></option>
						<option value="128"><?=$m_128bit_wep?></option>
					</select>
				</td>
			</tr>
			<tr>
				<?=$td1?><?=$m_key_type?></td>
				<?=$td2?><?=$symbol?>
					<select id="wep_key_type" name="wep_key_type" onChange="chg_wep_type()">
						<option value="2"><?=$m_hex?></option>
						<option value="1"><?=$m_ascii?></option>
					</select>
				</td>
			</tr>
			<tr>
				<?=$td1?><?=$m_default_wep_key?></td>
				<?=$td2?><?=$symbol?>
					<select name="wep_def_key" id="wep_def_key">
						<option value="1" selected><?=$m_wep_key?> 1</option>
						<option value="2"><?=$m_wep_key?> 2</option>
						<option value="3"><?=$m_wep_key?> 3</option>
						<option value="4"><?=$m_wep_key?> 4</option>
					</select>
				</td>
			</tr>
			</table>
			<div id="hex_wep_64"	style="display:none"><script>print_keys("hex_wep_64_","10");</script></div>
			<div id="hex_wep_128"	style="display:none"><script>print_keys("hex_wep_128_","26");</script></div>
			<div id="asc_wep_64"	style="display:none"><script>print_keys("asc_wep_64_","5");</script></div>
			<div id="asc_wep_128"	style="display:none"><script>print_keys("asc_wep_128_","13");</script></div>
		</div>
		</form>
		<!-- **************************** WPA, WPA2, WPA2-auto *********************************-->
		<form name="frm_wpa" id="frm_wpa" method="post" action="<?=$MY_NAME?>.php">
		<input type="hidden" name="ACTION_POST" value="WPA">
		<div class="box" id="show_wpa" style="display:none">
			<div id="title_wpa"		style="display:none"><h2><?=$m_title_wpa?></h2>		 <p><?=$m_dsc_wpa?></p>		</div>
			<div id="title_wpa2"	style="display:none"><h2><?=$m_title_wpa2?></h2>	 <p><?=$m_dsc_wpa2?></p>	</div>
			<div id="title_wpa2_auto" style="display:none"><h2><?=$m_title_wpa2_auto?></h2><p><?=$m_dsc_wpa2_auto?></p></div>
			<div>
				<table>
				<tr>
					<?=$td1?><?=$m_cipher_type?></td>
					<?=$td2?><?=$symbol?>
						<select name="cipher_type">
						<option value="2"><?=$m_tkip?></option>
						<option value="3"><?=$m_aes?></option>
						<option value="4"><?=$m_auto?></option>
						</select>
					</td>
				</tr>
				<tr>
					<?=$td1?><?=$m_psk_eap?></td>
					<?=$td2?><?=$symbol?>
						<select name="psk_eap" id="psk_eap" onchange="chg_psk_eap()">
						<option value="psk"><?=$m_psk?></option>
						<option value="eap"><?=$m_eap?></option>
						</select>
					</td>
				</tr>
				</table>
			</div>
			<!-- **************************** PSK *********************************-->
			<div id="psk_setting" style="display:none">
				<table>
					<tr>
						<?=$td1?><?=$m_passphrase?></td>
						<?=$td2?><?=$symbol?>
							<input type="password" id="wpapsk1" name="wpapsk1" size="40" maxlength="64" value="">
						</td>
					</tr>
					<tr>
						<?=$td1?><?=$m_confirm_passphrase?></td>
						<?=$td2?><?=$symbol?>
							<input type="password" id="wpapsk2" name="wpapsk2" size="40" maxlength="64" value="">
						</td>
					</tr>
				</table>
			</div>
			<!-- **************************** EAP *********************************-->
			<div id="eap_setting" style="display:none">
				<table>
					<tr><td class=l_tb><?=$m_8021x?></td></tr>
					<tr>
						<td>
						<table>
							<tr>
								<?=$td3?><?=$m_radius1?>&nbsp;</td><?=$td2?><?=$m_ipaddr?></td>
								<?=$td2?><?=$symbol?>
									<input id="srv_ip1" name="srv_ip1" maxlength=15 size=15 value="">
								</td>
							</tr>
							<tr>
								<?=$td3?></td><?=$td2?><?=$m_port?></td>
								<?=$td2?><?=$symbol?>
								<input type="text" id="srv_port1" name="srv_port1" size="8" maxlength="5" value="">
								</td>
							</tr>
							<tr>
								<?=$td3?></td><?=$td2?><?=$m_shared_sec?></td>
								<?=$td2?><?=$symbol?>
								<input type="password" id="srv_sec1" name="srv_sec1" size="50" maxlength="64" value="">
								</td>
							</tr>
							<tr>
								<?=$td3?><?=$m_radius2?>&nbsp;</td>	<?=$td2?><?=$m_ipaddr?></td>
								<?=$td2?><?=$symbol?>
								<input id="srv_ip2" name="srv_ip2" maxlength=15 size=15 value=""></td>
							</tr>
							<tr>
								<?=$td3?></td><?=$td2?><?=$m_port?></td>
								<?=$td2?><?=$symbol?>
								<input type="text" id="srv_port2" name="srv_port2" size="8" maxlength="5" value="">
								</td>
							</tr>
							<tr>
								<?=$td3?></td><?=$td2?><?=$m_shared_sec?></td>
								<?=$td2?><?=$symbol?>
								<input type="password" id="srv_sec2" name="srv_sev2" size="50" maxlength="64" value="">
								</td>
							</tr>
						</table>
						</td>
					</tr>
				</table>
			</div>
			<!-- **************************** end of EAP *********************************-->
		</div>
		<!-- **************************** end of WPA, WPA2, WPA2-auto *********************************-->
		</form>

<!-- ________________________________  Main Content End _______________________________ -->
	</td>
	<td <?=$G_HELP_TABLE_ATTR?>><?require($LOCALE_PATH."/help/h_".$MY_NAME.".php");?></td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</body>
</html>
