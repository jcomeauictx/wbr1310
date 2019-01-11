<?
$ac_number=0;
for("/wireless/wmm/ap/entry")
{
	if(qeury("ackpolicy")=="1") 	{$noackpolicy=0;}
	else				{$noackpolicy=1;}
	echo "iwpriv ".$wlanif." cwmin "	.$ac_number." 0 ".query("cwmin")."\n";
	echo "iwpriv ".$wlanif." cwmax "	.$ac_number." 0 ".query("cwmax")."\n";
	echo "iwpriv ".$wlanif." aifs "		.$ac_number." 0 ".query("aifs")."\n";
	echo "iwpriv ".$wlanif." txoplimit "	.$ac_number." 0 ".query("txoplimit")."\n";
	echo "iwpriv ".$wlanif." acm "		.$ac_number." 0 ".query("acm")."\n";
	echo "iwpriv ".$wlanif." noackpolicy "	.$ac_number." 0 ".$noackpolicy."\n";

	$ac_number++;
}
$ac_number=0;
for("/wireless/wmm/sta/entry")
{
	echo "iwpriv ".$wlanif." cwmin "	.$ac_number." 1 ".query("cwmin")."\n";
	echo "iwpriv ".$wlanif." cwmax "	.$ac_number." 1 ".query("cwmax")."\n";
	echo "iwpriv ".$wlanif." aifs "		.$ac_number." 1 ".query("aifs")."\n";
	echo "iwpriv ".$wlanif." txoplimit "	.$ac_number." 1 ".query("txoplimit")."\n";
	echo "iwpriv ".$wlanif." acm "		.$ac_number." 1 ".query("acm")."\n";

	$ac_number++;
}
?>
