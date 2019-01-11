Umask 026

Tuning {
	NumConnections 5
	BufSize 12288
	InputBufSize 4096
	ScriptBufSize 4096
	NumHeaders 100
	Timeout 60
	ScriptTimeout 200
	MaxUploadSize 3735552
}

PIDFile /var/run/httpd.pid
LogGMT On

Control {
	Types {
		text/html { html htm }
		text/plain { txt }
		image/gif { gif }
		image/jpeg { jpg }
		text/css { css }
		application/ocstream { * }
	}
	Specials {
		Dump { /dump }
		CGI { cgi }
		Imagemap { map }
		Redirect { url }
		Internal { _int }
	}
	External {
		/sbin/atp { php }
		/sbin/xgi { xgi bin }
	}
	IndexNames { index.html }
}

Server {
	Virtual {
		AnyHost
		Control {
<?
if (query("/sys/authtype")!="s")
{
echo "			Realm \"".query("/sys/hostname")."\"\n";
echo "			UserFile /var/etc/httpasswd\n";
echo "			Error401File		/www/sys/not_auth.php\n";
}
?>			SessionControl On
			SessionIdleTime <?query("/sys/sessiontimeout");?>
			SessionMax <?query("/proc/web/sessionum");?>
			SessionFilter { php xgi _int }
			ErrorFWUploadFile	/www/sys/wrongImg.htm
			ErrorCfgUploadFile	/www/sys/wrongImg.htm
			InfoFWRestartFile	/www/sys/restart.htm
			InfoCfgRestartFile	/www/sys/restart2.htm
			Alias /
			Location /www
		}
		Control {
                        Alias /HNAP1
			Location /www/HNAP1
			External {
				/usr/sbin/hnap { hnap }
			}
			IndexNames { index.hnap }
		}
	}
}

