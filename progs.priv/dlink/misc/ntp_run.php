# ntp_run.php >>>
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");

if ($VeRsIoN >= 2)
{
	echo "xmldbc -k ntp\n";
	echo "killall ntpclient\n";
}
else
{
	$ntp_loop_file = "/var/run/ntp_loop.sh";
	$ntp_loop_pid = "/var/run/ntp_loop.pid";

	echo "echo Stop NTP client ... > /dev/console\n";
	echo "if [ -f ".$ntp_loop_pid." ]; then\n";
	echo "	PID=`cat ".$ntp_loop_pid."`\n";
	echo "	if [ $PID != 0 ]; then\n";
	echo "		kill $PID > /dev/console\n";
	echo "	fi\n";
	echo "  rm -f ".$ntp_loop_pid."\n";
	echo "fi\n";
	echo "killall ntpclient > /dev/null 2>&1\n";
	echo "killall sleep > /dev/null 2>&1\n";
}

if ($generate_start==1)
{
	if (query("/time/syncwith")!=2)
	{
		echo "echo NTP client is disabled ... > /dev/console\n";
	}
	else
	{
		$ntps=query("/time/ntpserver/ip");
		$ntpt=query("/time/ntpserver/interval");
		if ($ntpt=="") { $ntpt="604800"; }
		if ($ntps=="") { $ntps="ntp1.dlink.com"; }
		/*if ($ntps=="") { $ntps="ntp.dlink.com.tw"; }*/

		if ($VeRsIoN >= 2)
		{
			$ntp_run = "/var/run/ntp_run.sh";
			fwrite( $ntp_run, "#!/bin/sh\n");
			fwrite2($ntp_run, "echo Run NTP client ... > /dev/console\n");
			fwrite2($ntp_run, "xmldbc -t \"ntp:".$ntpt.":".$ntp_run."\"\n");
			fwrite2($ntp_run, "ntpclient -h ".$ntps." -i 5 -s > /dev/console\n");
			fwrite2($ntp_run, "if [ $? != 1 ]; then\n");
			fwrite2($ntp_run, "	xmldbc -k ntp\n");
			fwrite2($ntp_run, "	xmldbc -t \"ntp:10:".$ntp_run."\"\n");
			fwrite2($ntp_run, "	echo NTP will run in 10 seconds! > /dev/console\n");
			fwrite2($ntp_run, "else\n");
			fwrite2($ntp_run, "	echo NTP will run in ".$ntpt." seconds! > /dev/console\n");
			fwrite2($ntp_run, "fi\n");
			echo "chmod +x ".$ntp_run."\n";
			echo $ntp_run." > /dev/console &\n";
		}
		else
		{
			fwrite($ntp_loop_file, "#!/bin/sh\n");
			fwrite2($ntp_loop_file, "while [ 1 = 1 ]; do\n");
			fwrite2($ntp_loop_file, "	ntpclient -h ".$ntps." -i 5 -s\n");
			fwrite2($ntp_loop_file, "	if [ $? != 1 ]; then\n");
			fwrite2($ntp_loop_file, "		sleep 10\n");
			fwrite2($ntp_loop_file, "	else\n");
			fwrite2($ntp_loop_file, "		sleep ".$ntpt."\n");
			fwrite2($ntp_loop_file, "	fi\n");
			fwrite2($ntp_loop_file, "done\n");
			echo "echo Start NTP client ... > /dev/console\n";
			echo "sh ".$ntp_loop_file." > /dev/console &\n";
			echo "echo $! > ".$ntp_loop_pid."\n";
		}
	}
}
?>
# ntp_run.php <<<
