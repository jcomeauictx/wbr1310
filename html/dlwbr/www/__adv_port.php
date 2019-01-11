<? /* vi: set sw=4 ts=4: */ 
anchor("/nat/vrtsrv/entry:".$index);
?>
		<tr>
	        <td align=middle rowspan="2">
		    	<input type=checkbox id='enable_<?=$index?>' name='enable_<?=$index?>' value="1" <?if (query("enable")=="1") {echo " checked";}?>>
		    </td>
		    <td valign="bottom"><?=$m_name?><br>
			    <input type="text" id='name_<?=$index?>' name='name_<?=$index?>' size="16" maxlength="31" value="<?get("h","description"); ?>">
		    </td>
		    <td align=left valign="bottom">
		  	    <input name='copy_app_<?=$index?>' type=button value="<<" class="button" onClick='copy_application(<?=$index?>)'>
		            <select id='app_<?=$index?>' name='app_<?=$index?>' style="width:120">
		                <option><?=$m_app_name?></option selected> 
		                <option value='FTP'>FTP</option>
						<option value='HTTP'>HTTP</option>
						<option value='HTTPS'>HTTPS</option>
						<option value='DNS'>DNS</option>
						<option value='SMTP'>SMTP</option>
						<option value='POP3'>POP3</option>
						<option value='Telnet'>Telnet</option>
						<option value='IPSec'>IPSec</option>
						<option value='PPTP'>PPTP</option>
						<option value='NetMeeting'>NetMeeting</option>
						<option value='DCS-1000'>DCS-1000</option>
						<option value='DCS-2000/DCS-5300'>DCS-2000/DCS-5300</option>
						<option value='i2eye'>i2eye</option>
			   		</select>
			 </td>
			<td align=middle valign="bottom"><div align="left"><?=$m_start?>
				<input type="text" id='start_port_<?=$index?>' name='start_port_<?=$index?>' size=5 maxlength="5" value='<?
					$sport=query("privateport");
					if($sport!="0")	{echo $sport;} 
				?>'></div>
			</td>	
             <td align=middle rowspan="2"><? $prot = query("protocol"); ?>
                  <select id='protocol_<?=$index?>' name='protocol_<?=$index?>'>
                       <option value=1<? if ($prot == 1) { echo " selected";}?>>TCP</option>
                       <option value=2<? if ($prot == 2) { echo " selected";}?>>UDP</option>
                       <option value=0<? if ($prot != 1 && $prot != 2) { echo " selected";}?>><?=$m_any?></option>
                  </select>
             </td>
<?
if($modelname=="WBR-2310")
{
			echo " <td align=middle rowspan=\"2\">\n";
			echo " 	<select id='schedule_".$index."' name='schedule_".$index."'>\n";
			echo "		<option value=0>".$m_always."</option>\n";
			
			$sel_id=query("schedule/uniqueid");
			for("/sys/schedule/entry")
			{  
				$tmp_id=query("id"); 
				echo "<option value=".$tmp_id; 
				if($sel_id==$tmp_id) {echo " selected";} 
				echo ">".query("name")."</option>";
			}
			
			echo "</select>\n";
			echo "</td>\n";
}
?>
		</tr>   
        <tr>
	        <td valign="bottom"><?=$m_ip?>
		        <input type="text" id='ip_<?=$index?>' name='ip_<?=$index?>' size="16" maxlength="15" value='<?
					$privateip=query("privateip");
					if($privateip!="0")	{echo $privateip;}
				?>'>
		    </td>
	     	<td align=left valign="bottom">
		        <input name="copy_ip_<?=$index?>" type=button value="<<" class="button" onClick="copy_ip(<?=$index?>)">
		            <select id="ip_list_<?=$index?>" name="ip_list_<?=$index?>" style="width:120">
		                <option><?=$m_pc_name?></option selected>
		            	<?
						for("/runtime/dhcpserver/lease")
						{
							$tempname=get(h,"hostname");
							$tempip=query("ip");
							echo "<option value=".$tempip.">".$tempname."</option>";
						}
						?>
					</select>
		     </td>
		     <td align=middle valign="bottom"><div align="left"><?=$m_end?>
		        <input type="text" id='end_port_<?=$index?>' name='end_port_<?=$index?>' size=5 maxlength="5" value='<?
					$end_port=query("publicport");
					$end_port2=query("publicport2");
					if($end_port2!="") {echo $end_port2;} else {echo $end_port;}
				?>'></div>
		     </td>
		
		</tr>
