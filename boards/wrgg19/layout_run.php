#!/bin/sh
echo [$0] $1 ... > /dev/console
<?
/* vi: set sw=4 ts=4: */
require("/etc/templates/troot.php");
/*
	echo "config vlan ..." > /dev/console
	# PHY 29, MII [28-24] - VLAN ID of Port [4-0]
	echo "WRITE 29 24 3" > /proc/driver/ae531x
	echo "WRITE 29 25 3" > /proc/driver/ae531x
	echo "WRITE 29 26 3" > /proc/driver/ae531x
	echo "WRITE 29 27 3" > /proc/driver/ae531x
	echo "WRITE 29 28 5" > /proc/driver/ae531x
	# PHY 29, MII 30 - default VLAN ID of Port 5 (cpu)
	echo "WRITE 29 30 5" > /proc/driver/ae531x
	# PHY 29, MII 23[10:6] - remove VLAN tags for Port [4-0]
	# bit   5432 1098 7654 3210
	# value 0000 0111 1100 0010 = 07c2
	# [1] add tags for Port 5 (cpu)
	echo "WRITE 29 23 07c2" > /proc/driver/ae531x
	# PHY 30, MII 2[13:8] - VLAN3 output port mask
	echo "WRITE 30 2 2f30" > /proc/driver/ae531x
	# PHY 30, MII 3[13:8] - VLAN5 output port mask
	echo "WRITE 30 3 303f" > /proc/driver/ae531x
	# PHY 30, MII 9
	# bit   2 1098 7654 3210
	# value 1 0000 1000 1001 = 0x1089
	# [12:8] Set Port [4-0] as WAN port
	# [7] Enable tag VLAN function
	# [3] Enable router function
	# [2:0] LAN group number
	echo "WRITE 30 9 1089" > /proc/driver/ae531x
*/
$mii_dev = "/proc/driver/ae531x";
$bridge = query("/bridge");
if ($bridge!=1)	{ $router_on = 1; }
else			{ $router_on = 0; }

if ($router_on==1)
{
	if ($generate_start==1)
	{
		echo "echo Start router layout ...\n";
		if (query("/runtime/router/enable")==1)
		{
			echo "echo Already in router mode!\n";
			exit;
		}
		echo "echo \"WRITE 29 24 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 25 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 26 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 27 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 28 2\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 30 2\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 23 07c2\" > ".$mii_dev."\n";
		echo "echo \"WRITE 30 1 2f00\"  > ".$mii_dev."\n";
		echo "echo \"WRITE 30 2 0030\"  > ".$mii_dev."\n";
		echo "echo \"WRITE 30 9 1089\"  > ".$mii_dev."\n";
		echo "ifconfig vlan1 up\n";
		echo "ifconfig vlan2 up\n";
		echo "brctl addif br0 ath0\n";
		echo "brctl addif br0 vlan1\n";
		echo "brctl setbwctrl br0 ath0 900\n";
		echo "ifconfig br0 up\n";
		echo "rgdb -i -s /runtime/router/enable 1\n";
	}
	else
	{
		echo "brctl delif br0 vlan1\n";
		echo "brctl delif br0 ath0\n";
		echo "ifconfig vlan1 down\n";
		echo "ifconfig vlan2 down\n";
		echo "rgdb -i -s /runtime/router/enable \"\"\n";
	}
}
else
{
	if ($generate_start==1)
	{
		echo "echo Start bridge layout ...\n";
		if (query("/runtime/router/enable")==0)
		{
			echo "echo Already in bridge mode!\n";
			exit;
		}
		echo "echo \"WRITE 29 24 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 25 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 26 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 27 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 28 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 30 1\"    > ".$mii_dev."\n";
		echo "echo \"WRITE 29 23 07c0\" > ".$mii_dev."\n";
		echo "echo \"WRITE 30 9 0000\"  > ".$mii_dev."\n";
		echo "brctl addif br0 eth0\n";
		echo "brctl addif br0 ath0\n";
		echo "brctl setbwctrl br0 ath0 900\n";
		echo "ifconfig br0 up\n";
		echo "rgdb -i -s /runtime/router/enable 0\n";
	}
	else
	{
		echo "echo Stop bridge layout ...\n";
		echo "brctl delif br0 ath0\n";
		echo "brctl delif br0 eth0\n";
		echo "rgdb -i -s /runtime/router/enable \"\"\n";
	}
}
?>
