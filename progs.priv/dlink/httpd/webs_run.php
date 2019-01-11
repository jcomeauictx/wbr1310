#!/bin/sh
echo [$0] ... > /dev/console
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

$httpdpid="/var/run/httpd.pid";

if ($generate_start==1)
{
	echo "echo Starting HTTPD ... > /dev/console\n";
	echo "rgdb -A ".$template_root."/httpd/httpd.php > /var/etc/httpd.cfg\n";
	/*-----Add password file for HNAP-----*/
	echo "rgdb -A ".$template_root."/httpd/httpasswd.php > /var/etc/httpasswd\n";
	$session_timeout = query("/sys/sessiontimeout");
	fwrite("/var/proc/web/sessiontimeout", $session_timeout);
	echo "httpd -s ".query("/runtime/layout/image_sign")." -f /var/etc/httpd.cfg &\n";
}
else
{
	echo "echo Stopping HTTPD ... > /dev/console\n";
	echo "if [ -f ".$httpdpid." ]; then\n";
	echo "	pid=`cat ".$httpdpid."`\n";
	echo "	if [ $pid != 0 ]; then\n";
	echo "		kill $pid > /dev/null 2>&1\n";
	echo "	fi\n";
	echo "	rm -f ".$httpdpid."\n";
	echo "fi\n";
}

?>
