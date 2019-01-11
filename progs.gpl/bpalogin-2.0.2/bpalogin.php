<?
echo "debuglevel 2\n";
echo "username ".query(/wan/rg/inf:1/bigpond/username)."\n";
echo "password ".query(/wan/rg/inf:1/bigpond/password)."\n";
echo "authserver ".query(/wan/rg/inf:1/bigpond/server)."\n";
//authdomain vic.bigpond.net.au
//connectedprog /etc/rc.d/rc.masq
//disconnectedprog /etc/rc.d/rc.masq
//localaddress 0.0.0.0
echo "localport 5050\n";
//logging syslog
//minheartbeatinterval 60
//maxheartbeatinterval 420
?>
