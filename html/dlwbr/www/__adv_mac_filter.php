<? /* vi: set sw=4 ts=4: */
anchor("/security/macfilter/entry:".$index);
?>			<tr>
				<td>
					<input type=text id=mac_<?=$index?> name=mac_<?=$index?> size=18 maxlength=17 value="<?get(h,"sourcemac");?>">
				</td>
				<td>
					<input type=button id=copy_<?=$index?> name=copy_<?=$index?> value="<<" onclick="copy_mac(<?=$index?>)">
				</td>
				<td width=235>
					<select id='dhcp_<?=$index?>' name='dhcp_<?=$index?>'>
						<option value=0><?=$m_computer_name?></option selected>
<?
for ("/runtime/dhcpserver/lease")
{
echo
"						<option value=\"".query("mac")."\">".get(h,"hostname")."</option>\n";
}

?>					</select>
				</td>
				<td align=left>
					<input type=button name=clear id=clear value="<?=$m_clear?>" onclick='clear_mac(<?=$index?>)'>
				</td>
			</tr>
