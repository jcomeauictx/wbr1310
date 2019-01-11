<?
/* vi: set sw=4 ts=4: */
fwrite($shell, "#!/bin/sh\n");
fwrite2($shell, "echo [$0] WANCommonIFC1:".$action." ... > /dev/console\n");

$RouterOn=query("/runtime/router/enable");

if		($action == "GetCommonLinkProperties")
{
	echo "<NewWANAccessType>Ethernet</NewWANAccessType>\n";
	echo "<NewLayer1UpstreamMaxBitRate>100000000</NewLayer1UpstreamMaxBitRate>\n";
	echo "<NewLayer1DownstreamMaxBitRate>100000000</NewLayer1DownstreamMaxBitRate>\n";
	echo "<NewPhysicalLinkStatus>";
	map("/runtime/wan/inf:1/linkType","1","Up","*","Down");
	echo "</NewPhysicalLinkStatus>\n";
}
else if	($action == "GetTotalBytesReceived")
{
	echo "<NewTotalBytesReceived>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/rx/bytes","","0"); }
	else { echo "0"; }
	echo "</NewTotalBytesReceived>";
}
else if	($action == "GetTotalBytesSent")
{
	echo "<NewTotalBytesSent>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/tx/bytes","","0"); }
	else { echo "0"; }
	echo "</NewTotalBytesSent>";
}
else if ($action == "GetTotalPacketsReceived")
{
	echo "<NewTotalPacketsReceived>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/rx/packets","","0"); }
	else { echo "0"; }
	echo "</NewTotalPacketsReceived>";
}
else if ($action == "GetTotalPacketsSent")
{
	echo "<NewTotalPacketsSent>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/tx/packets","","0"); }
	else { echo "0"; }
	echo "</NewTotalPacketsSent>";
}
else if	($action == "X_GetICSStatistics")
{
	echo "<TotalBytesSent>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/tx/bytes","","0"); }
	else { echo "0"; }
	echo "</TotalBytesSent>\n";
	echo "<TotalBytesReceived>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/rx/bytes","","0"); }
	else { echo "0"; }
	echo "</TotalBytesReceived>\n";
	echo "<TotalPacketsSent>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/tx/packets","","0"); }
	else { echo "0"; }
	echo "</TotalPacketsSent>\n";
	echo "<TotalPacketsReceived>";
	if ($RouterOn==1) { map("/runtime/stats/wan/inf:1/rx/packets","","0"); }
	else { echo "0"; }
	echo "</TotalPacketsReceived>\n";
	echo "<Layer1DownstreamMaxBitRate>100000000</Layer1DownstreamMaxBitRate>\n";
	$wanuptime=query("/runtime/wan/inf:1/uptime");
	$sysuptime=query("/runtime/sys/uptime");
	$uptime=$sysuptime - $wanuptime;
	echo "<Uptime>".$uptime."</Uptime>";
}
?>
