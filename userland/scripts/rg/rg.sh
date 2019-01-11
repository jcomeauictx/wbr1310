#!/bin/sh
usage="Usage: rg.sh [start|stop|flush] {firewall|filter|vserver|dmz|passthrough|misc|ptrigger|blocking}"
[ -z "$1" ] && echo $usage && exit 1

# External helper
FW_MNGR=firewallmngr
IPF_MNGR=ipfiltermngr
MACF_MNGR=macfiltermngr
VS_MNGR=vrtsrvmngr
DMZ_MNGR=dmzsrvmngr
PASSTHRU_MNGR=passthroughmngr
FW_MISC_MNGR=firewallmisc
PT_MNGR=ptgen
BLOCK_MNGR=blocking
# Temp folder
RUN=/var/run

# Custom chains
IPF_FOR=Ip_Filter_Forward
MACF_FOR=Mac_Filter_Forward
MACF_IN=Mac_Filter_Input
VS_NAT_PRER=VrtSrv_Nat_Prerouting
VS_NAT_POST=VrtSrv_Nat_Postrouting
VS_FORWARD=VrtSrv_Forwarding
VS_REPLY=VrtSrv_Reply_Forwarding
UPNP_NAT_PRER=Upnp_Nat_Prerouting
UPNP_NAT_POST=Upnp_Nat_Postrouting
UPNP_FORWARD=Upnp_Forwarding
DMZ_NAT=DmzSrv_Nat_Prerouting
DMZ_FORWARD=DmzSrv_Forwarding
PASSTHRU_FOR=Passthrough_Forward
BLOCK_FOR=Block_Forward
BLOCK_IN=Block_Input
FW_FOR=Firewall_Forward
FW_NAT_PRER=Firewall_Nat_Prerouting
FW_IN_BAD_PACKETS=Firewall_Input_Bad_Packets
FW_FOR_BAD_PACKETS=Firewall_Forward_Bad_Packets
FW_IN_SYNCFLOODING=Firewall_Input_SynFlooding
FW_FOR_SYNCFLOODING=Firewall_Forward_SynFlooding
FW_IN_PINGFLOODING=Firewall_Input_PingFlooding
FW_FOR_PINGFLOODING=Firewall_Forward_PingFlooding
FW_IN_PORTSCAN=Firewall_Input_PortScan
FW_FOR_PORTSCAN=Firewall_Forward_PortScan
FW_MISC_IN=Firewall_Misc_Input
FW_MISC_NAT=Firewall_Misc_Nat_Prerouting
NAPT_POSTROUT=Napt_Postrouting

LANIF=`/etc/scripts/misc/lan.sh interface`
LANIP=`ifconfig $LANIF | scut -p "inet addr:"`

case "$1" in
start)
	echo 1 > /proc/sys/net/ipv4/ip_forward
	# Flush all chains
	iptables -F
	iptables -t nat -F
	#Delete all user-defined chains
	iptables -X
	iptables -t nat -X
	# Default policy
	iptables -P INPUT ACCEPT
	iptables -P OUTPUT ACCEPT
	iptables -P FORWARD ACCEPT
	iptables -t nat -P PREROUTING ACCEPT
	iptables -t nat -P POSTROUTING ACCEPT
	# Add custom chains
	iptables -N		$IPF_FOR
	iptables -N		$MACF_FOR
	iptables -N		$MACF_IN
	iptables -N		$PASSTHRU_FOR
	iptables -N		$BLOCK_FOR
	iptables -N		$BLOCK_IN
	iptables -N		$FW_FOR
	iptables -t nat -N	$FW_NAT_PRER
	iptables -N		$FW_IN_BAD_PACKETS
	iptables -N		$FW_FOR_BAD_PACKETS
	iptables -N		$FW_IN_SYNCFLOODING
	iptables -N		$FW_FOR_SYNCFLOODING
	iptables -N		$FW_IN_PINGFLOODING
	iptables -N		$FW_FOR_PINGFLOODING
	iptables -N		$FW_IN_PORTSCAN
	iptables -N		$FW_FOR_PORTSCAN
	iptables -N		$FW_MISC_IN
	iptables -t nat -N	$FW_MISC_NAT
	iptables -t nat -N	$VS_NAT_PRER
	iptables -t nat -N	$VS_NAT_POST
	iptables -N		$VS_FORWARD
	iptables -N		$VS_REPLY
	iptables -t nat -N	$UPNP_NAT_PRER
	iptables -t nat -N	$UPNP_NAT_POST
	iptables -N		$UPNP_FORWARD
	iptables -t nat -N	$DMZ_NAT
	iptables -N		$DMZ_FORWARD
	iptables -t nat -N	$NAPT_POSTROUT

	# Default masquerade rule
	iptables -t nat -A POSTROUTING -j $NAPT_POSTROUT

	# MAC filter
	iptables -A FORWARD -j $MACF_FOR
	iptables -A INPUT   -j $MACF_IN
	# Firewalls (DOS)
	iptables -A FORWARD -j $FW_FOR_BAD_PACKETS
	iptables -A INPUT   -j $FW_IN_BAD_PACKETS
	iptables -A FORWARD -p tcp -m state --state NEW -j $FW_FOR_SYNCFLOODING
	iptables -A INPUT   -p tcp -m state --state NEW -j $FW_IN_SYNCFLOODING
	iptables -A FORWARD -p icmp --icmp-type echo-request -m state --state NEW -j $FW_FOR_PINGFLOODING
	iptables -A INPUT   -p icmp --icmp-type echo-request -m state --state NEW -j $FW_IN_PINGFLOODING
	iptables -A FORWARD -j $FW_FOR_PORTSCAN
	iptables -A INPUT   -j $FW_IN_PORTSCAN
	# Firewalls
	iptables -A FORWARD		-j $VS_REPLY
	iptables -A FORWARD		-j $FW_FOR
	iptables -t nat -A PREROUTING	-j $FW_NAT_PRER
	# IP filter
	iptables -A FORWARD		-j $IPF_FOR
	# Domain and URL blocking
	iptables -A FORWARD		-j $BLOCK_FOR
	iptables -A INPUT		-j $BLOCK_IN
	# PASSTHROUGH
	iptables -A FORWARD		-j $PASSTHRU_FOR
	# Upnp
	iptables -t nat -A PREROUTING	-j $UPNP_NAT_PRER
	iptables -t nat -A POSTROUTING	-j $UPNP_NAT_POST
	iptables -A FORWARD		-j $UPNP_FORWARD
	# Virtual Server
	iptables -t nat -A PREROUTING	-j $VS_NAT_PRER
	iptables -t nat -A POSTROUTING	-j $VS_NAT_POST
	iptables -A FORWARD		-j $VS_FORWARD
	# Firewallmisc
	iptables -A INPUT		-j $FW_MISC_IN
	iptables -t nat -A PREROUTING	-j $FW_MISC_NAT
	# DMZ Server
	iptables -t nat -A PREROUTING	-j $DMZ_NAT
	iptables -A FORWARD		-j $DMZ_FORWARD
	# Another-default rule 
	iptables -t nat -A PREROUTING -i ! $LANIF -p TCP --dport 80 -j LOG --log-level 6 --log-prefix "DRP:Block Remote Management:"
	iptables -t nat -A PREROUTING -i ! $LANIF -p TCP --dport 80 -j DROP
	iptables -A INPUT -i ! $LANIF -m state --state ESTABLISHED,RELATED -j ACCEPT
	iptables -A INPUT -i ! $LANIF -j DROP
	iptables -A FORWARD -i ! $LANIF -m state --state ESTABLISHED,RELATED -j ACCEPT
	iptables -A FORWARD -i ! $LANIF -j DROP
	;;
stop)
	echo 0 > /proc/sys/net/ipv4/ip_forward
	iptables -F
	iptables -t nat -F
	iptables -X
	iptables -t nat -X
	iptables -P INPUT ACCEPT
	iptables -P OUTPUT ACCEPT
	iptables -P FORWARD ACCEPT
	iptables -t nat -P PREROUTING ACCEPT
	iptables -t nat -P POSTROUTING ACCEPT
	;;
flush)
	WANIF=`/etc/scripts/misc/wan.sh interface`
	WANIP=`ifconfig $WANIF | scut -p "inet addr:"`
	case "$2" in
	firewall)
		$FW_MNGR -w $WANIF -l $LANIF > $RUN/fw_mngr.sh
		sh $RUN/fw_mngr.sh
		;;
	filter)
		$IPF_MNGR -w $WANIF -l $LANIF > $RUN/ipf_mngr.sh
		sh $RUN/ipf_mngr.sh
		$MACF_MNGR -w $WANIF -l $LANIF > $RUN/macf_mngr.sh
		sh $RUN/macf_mngr.sh
		;;
	vserver)
		$VS_MNGR -w $WANIF -l $LANIF -W $WANIP -L $LANIP > $RUN/vs_mngr.sh
		sh $RUN/vs_mngr.sh
		;;
	dmz)
		$DMZ_MNGR -w $WANIF -l $LANIF -L $LANIP > $RUN/dmz_mngr.sh
		sh $RUN/dmz_mngr.sh
		;;
	passthrough)
		$PASSTHRU_MNGR -w $WANIF -l $LANIF > $RUN/passthru_mngr.sh
		sh $RUN/passthru_mngr.sh
		;;
	misc)
		$FW_MISC_MNGR -w $WANIF -l $LANIF > $RUN/fw_misc.sh
		sh $RUN/fw_misc.sh
		;;
	ptrigger)
		$PT_MNGR > $RUN/pt_mngr.sh
		sh $RUN/pt_mngr.sh
		;;
	blocking)
		$BLOCK_MNGR $LANIF $WANIF > $RUN/block_mngr.sh
		sh $RUN/block_mngr.sh
		;;
	*)
		# flush all
		iptables -t nat -F $NAPT_POSTROUT
		iptables -t nat -A $NAPT_POSTROUT -o $WANIF -j MASQUERADE

		$FW_MNGR -w $WANIF -l $LANIF > $RUN/fw_mngr.sh
		sh $RUN/fw_mngr.sh
		$IPF_MNGR -w $WANIF -l $LANIF > $RUN/ipf_mngr.sh
		sh $RUN/ipf_mngr.sh
		$MACF_MNGR -w $WANIF -l $LANIF > $RUN/macf_mngr.sh
		sh $RUN/macf_mngr.sh
		$VS_MNGR -w $WANIF -l $LANIF -W $WANIP -L $LANIP > $RUN/vs_mngr.sh
		sh $RUN/vs_mngr.sh
		$DMZ_MNGR -w $WANIF -l $LANIF -L $LANIP > $RUN/dmz_mngr.sh
		sh $RUN/dmz_mngr.sh
		$PASSTHRU_MNGR -w $WANIF -l $LANIF > $RUN/passthru_mngr.sh
		sh $RUN/passthru_mngr.sh
		$FW_MISC_MNGR -w $WANIF -l $LANIF > $RUN/fw_misc.sh
		sh $RUN/fw_misc.sh
		$PT_MNGR -f > $RUN/pt_mngr.sh
		sh $RUN/pt_mngr.sh
		$BLOCK_MNGR $LANIF $WANIF > $RUN/block_mngr.sh
		sh $RUN/block_mngr.sh
		;;
	esac
	;;
*)
	echo $usage
	;;
esac
exit 0
