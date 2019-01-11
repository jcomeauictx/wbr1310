<?
/* vi: set sw=4 ts=4: */
fwrite($shell, "#!/bin/sh\n");
fwrite2($shell, "echo [$0] L3Forwarding1:".$action." ... > /dev/console\n");

if	($action == "GetDefaultConnectionService")
{
	echo "\n<NewDefaultConnectionService>".$udn.":WANConnectionDevice:1,urn:upnp-org:serviceId:WANIPConn1</NewDefaultConnectionService>";
}
else
{
	echo "401";
}
?>
