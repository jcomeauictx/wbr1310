<?
/* vi: set sw=4 ts=4: */
if	($action == "GetDefaultConnectionService")
{
	echo "\n<NewDefaultConnectionService>".$udn.":WANConnectionDevice:1,urn:upnp-org:serviceId:WANIPConn1</NewDefaultConnectionService>";
}
else
{
	echo "401";
}
?>
