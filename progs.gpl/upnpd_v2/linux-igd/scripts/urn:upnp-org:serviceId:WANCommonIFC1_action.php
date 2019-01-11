<?
/* vi: set sw=4 ts=4: */
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
	map("/runtime/stats/wan/inf:1/rx/bytes","","0");
	echo "</NewTotalBytesReceived>";
}
else if	($action == "GetTotalBytesSent")
{
	echo "<NewTotalBytesSent>";
	map("/runtime/stats/wan/inf:1/tx/bytes","","0");
	echo "</NewTotalBytesSent>";
}
else if ($action == "GetTotalPacketsReceived")
{
	echo "<NewTotalPacketsReceived>";
	map("/runtime/stats/wan/inf:1/rx/packets","","0");
	echo "</NewTotalPacketsReceived>";
}
else if ($action == "GetTotalPacketsSent")
{
	echo "<NewTotalPacketsSent>";
	map("/runtime/stats/wan/inf:1/tx/packets","","0");
	echo "</NewTotalPacketsSent>";
}
else if	($action == "X_GetICSStatistics")
{
	echo "<TotalBytesSent>"; map("/runtime/stats/wan/inf:1/tx/bytes","","0"); echo "</TotalBytesSent>\n";
	echo "<TotalBytesReceived>"; map("/runtime/stats/wan/inf:1/rx/bytes","","0"); echo "</TotalBytesReceived>\n";
	echo "<TotalPacketsSent>"; map("/runtime/stats/wan/inf:1/tx/packets","","0"); echo "</TotalPacketsSent>\n";
	echo "<TotalPacketsReceived>"; map("/runtime/stats/wan/inf:1/rx/packets","","0"); echo "</TotalPacketsReceived>\n";
	echo "<Layer1DownstreamMaxBitRate>100000000</Layer1DownstreamMaxBitRate>\n";
	echo "<Uptime>".query("/runtime/sys/uptime")."</Uptime>";
}
?>
