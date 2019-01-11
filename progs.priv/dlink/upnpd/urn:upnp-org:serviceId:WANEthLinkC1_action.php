<?
/* vi: set sw=4 ts=4: */
fwrite($shell, "#!/bin/sh\n");
fwrite2($shell, "echo [$0] WANEthLinkC1:".$action." ... > /dev/console\n");

if		($action == "GetEthernetLinkStatus")
{
	echo "\n<NewEthernetLinkStatus>";
	map("/runtime/wan/inf:1/linkType","1","Up","*","Down");
	echo "</NewEthernetLinkStatus>";
}
else
{
	echo "401";
}

?>
