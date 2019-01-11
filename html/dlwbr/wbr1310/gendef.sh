#!/bin/sh

RGDB="./tools/alpha/rgdb/rgdb -S ./rgapps_xmldb_gendef"

WANIF="/wan/rg/inf:1"
WSTAT="$WANIF/static"
WDHCP="$WANIF/dhcp"
WPPOE="$WANIF/pppoe"
WPPTP="$WANIF/pptp"
WL2TP="$WANIF/l2tp"
$RGDB -s $WANIF/mode		2
$RGDB -s $WANIF/etherlinktype	0
$RGDB -s $WSTAT/ip		""
$RGDB -s $WSTAT/netmask		""
$RGDB -s $WSTAT/gateway		""
$RGDB -s $WSTAT/cloneMac	""
$RGDB -s $WSTAT/mtu		1500

$RGDB -s $WDHCP/cloneMac	""
$RGDB -s $WDHCP/autoDns		1
$RGDB -s $WDHCP/mtu		1500

$RGDB -s $WPPOE/mode		2
$RGDB -s $WPPOE/staticIp	""
$RGDB -s $WPPOE/user		""
$RGDB -s $WPPOE/password	""
$RGDB -s $WPPOE/acName		""
$RGDB -s $WPPOE/acService	""
$RGDB -s $WPPOE/autoReconnect	1
$RGDB -s $WPPOE/onDemand	1
$RGDB -s $WPPOE/idleTimeout	300
$RGDB -s $WPPOE/autoDns		1
$RGDB -s $WPPOE/mtu		1492
$RGDB -s $WPPOE/cloneMac	""

$RGDB -s $WPPTP/mode		2
$RGDB -s $WPPTP/ip		""
$RGDB -s $WPPTP/netmask		""
$RGDB -s $WPPTP/gateway		""
$RGDB -s $WPPTP/dns		""
$RGDB -s $WPPTP/serverip	""
$RGDB -s $WPPTP/user		""
$RGDB -s $WPPTP/password	""
$RGDB -s $WPPTP/autoReconnect	1
$RGDB -s $WPPTP/onDemand	1
$RGDB -s $WPPTP/idleTimeout	300
$RGDB -s $WPPTP/autodns		1
$RGDB -s $WPPTP/mtu		1400

$RGDB -s $WL2TP/mode		2
$RGDB -s $WL2TP/ip		""
$RGDB -s $WL2TP/netmask		""
$RGDB -s $WL2TP/gateway		""
$RGDB -s $WL2TP/dns		""
$RGDB -s $WL2TP/serverip	""
$RGDB -s $WL2TP/user		""
$RGDB -s $WL2TP/password	""
$RGDB -s $WL2TP/autoReconnect	1
$RGDB -s $WL2TP/onDemand	1
$RGDB -s $WL2TP/idleTimeout	300
$RGDB -s $WL2TP/autodns		1
$RGDB -s $WL2TP/mtu		1400

LANIF="/lan"
LETH="$LANIF/ethernet"
LDYN="$LANIF/dhcp"
LDYNS="$LDYN/server"
LDYNS1="$LDYNS/pool:1"
LDYNR="$LDYN/relay"

$RGDB -s $LETH/ip			192.168.0.1
$RGDB -s $LETH/netmask			255.255.255.0

$RGDB -s $LDYN/mode			1
$RGDB -s $LDYNS/enable			1
$RGDB -s $LDYNS1/startIp		192.168.0.100
$RGDB -s $LDYNS1/endIp			192.168.0.199
$RGDB -s $LDYNS1/netmask		255.255.255.0
$RGDB -s $LDYNS1/domain			""
$RGDB -s $LDYNS1/leaseTime		604800
$RGDB -s $LDYNS1/staticDhcp/enable	1

DNSR="/dnsRelay"
$RGDB -s $DNSR/mode			2
$RGDB -s $DNSR/server/primaryDns	""
$RGDB -s $DNSR/server/secondaryDns	""

NAT="/nat"
NATGEN="$NAT/general"
NATVSR="$NAT/vrtsrv"
NATPTR="$NAT/portTrigger"
NATDMZ="$NAT/dmzsrv"
NATPST="$NAT/passthrough"
$RGDB -s $NAT/enable				1
$RGDB -s $NATGEN/gamingMode			1

$RGDB -s $NATVSR/entry:1/enable			0
$RGDB -s $NATVSR/entry:1/schedule/enable	0

$RGDB -s $NATVSR/entry:2/enable			0
$RGDB -s $NATVSR/entry:2/schedule/enable	0

$RGDB -s $NATVSR/entry:3/enable			0
$RGDB -s $NATVSR/entry:3/schedule/enable	0

$RGDB -s $NATVSR/entry:4/enable			0
$RGDB -s $NATVSR/entry:4/schedule/enable	0

$RGDB -s $NATVSR/entry:5/enable			0
$RGDB -s $NATVSR/entry:5/schedule/enable	0

$RGDB -s $NATVSR/entry:6/enable			0
$RGDB -s $NATVSR/entry:6/schedule/enable	0

$RGDB -s $NATVSR/entry:7/enable			0
$RGDB -s $NATVSR/entry:7/schedule/enable	0

$RGDB -s $NATVSR/entry:8/enable			0
$RGDB -s $NATVSR/entry:8/schedule/enable	0

$RGDB -s $NATVSR/entry:9/enable			0
$RGDB -s $NATVSR/entry:9/schedule/enable	0

$RGDB -s $NATVSR/entry:10/enable		0
$RGDB -s $NATVSR/entry:10/schedule/enable	0

$RGDB -s $NATPTR/entry:1/enable			0
$RGDB -s $NATPTR/entry:2/enable			0
$RGDB -s $NATPTR/entry:3/enable			0
$RGDB -s $NATPTR/entry:4/enable			0
$RGDB -s $NATPTR/entry:5/enable			0
$RGDB -s $NATPTR/entry:6/enable			0
$RGDB -s $NATPTR/entry:7/enable			0
$RGDB -s $NATPTR/entry:8/enable			0
$RGDB -s $NATPTR/entry:9/enable			0
$RGDB -s $NATPTR/entry:10/enable		0

$RGDB -s $NATDMZ/dmzAction	0
$RGDB -s $NATDMZ/ipAddress	""

$RGDB -s $NATPST/pppoe		1
$RGDB -s $NATPST/ipsec		1
$RGDB -s $NATPST/pptp		1

SEC="/security"
SDOS="$SEC/dos"
SLOG="$SEC/log"
SGLB="$SEC/global"
SFWL="$SEC/firewall"
DFWL="$SEC/di624/firewall"
SIPF="$SEC/di624/ipFilter"
SMAC="$SEC/macFilter"
SDBK="$SEC/domainBlocking"
SUBK="$SEC/urlBlocking"
SCFL="$SEC/contentFiltering"
$RGDB -s $SDOS/enable		1

$RGDB -s $SLOG/systemInfo	1
$RGDB -s $SLOG/debugInfo	0
$RGDB -s $SLOG/attackInfo	1
$RGDB -s $SLOG/dropPacketInfo	0
$RGDB -s $SLOG/noticeInfo	1

$RGDB -s $SFWL/dosAction	0
$RGDB -s $SFWL/pingAllow	1
$RGDB -s $SFWL/telnetAllow	0
$RGDB -s $SFWL/ftpAllow		0
$RGDB -s $SFWL/httpAllow	0
$RGDB -s $SFWL/snmpAllow	0
$RGDB -s $SFWL/httpRemoteIp	""
$RGDB -s $SFWL/httpRemotePort	8080

$RGDB -s $SMAC/action 0
$RGDB -s $SDBK/enable 0
$RGDB -s $SMAC/enable 0
$RGDB -s $SMAC/entry:1/sourcemac	""
$RGDB -s $SMAC/entry:2/sourcemac	""
$RGDB -s $SMAC/entry:3/sourcemac	""
$RGDB -s $SMAC/entry:4/sourcemac	""
$RGDB -s $SMAC/entry:5/sourcemac	""
$RGDB -s $SMAC/entry:6/sourcemac	""
$RGDB -s $SMAC/entry:7/sourcemac	""
$RGDB -s $SMAC/entry:8/sourcemac	""
$RGDB -s $SMAC/entry:9/sourcemac	""
$RGDB -s $SMAC/entry:10/sourcemac	""

$RGDB -s $SUBK/enable 0
$RGDB -s $SUBK/string:1		""
$RGDB -s $SUBK/string:2		""
$RGDB -s $SUBK/string:3		""
$RGDB -s $SUBK/string:4		""
$RGDB -s $SUBK/string:5		""
$RGDB -s $SUBK/string:6		""
$RGDB -s $SUBK/string:7		""
$RGDB -s $SUBK/string:8		""
$RGDB -s $SUBK/string:9		""
$RGDB -s $SUBK/string:10	""
$RGDB -s $SUBK/string:11	""
$RGDB -s $SUBK/string:12	""
$RGDB -s $SUBK/string:13	""
$RGDB -s $SUBK/string:14	""
$RGDB -s $SUBK/string:15	""
$RGDB -s $SUBK/string:16	""
$RGDB -s $SUBK/string:17	""
$RGDB -s $SUBK/string:18	""
$RGDB -s $SUBK/string:19	""
$RGDB -s $SUBK/string:20	""

UPNP="/upnp"
$RGDB -s $UPNP/enable			1
$RGDB -s $UPNP/log			1
$RGDB -s $UPNP/pppoesession		0
$RGDB -s $UPNP/allow_disconnect_wan	1

TIME="/time"
NTPS="$TIME/ntpServer"
$RGDB -s $TIME/syncWith		0
$RGDB -s $TIME/timeZone		5
$RGDB -s $TIME/daylightSaving	0
$RGDB -s $NTPS/ip		""
$RGDB -s $NTPS/interval		604800

SYS="/sys"
$RGDB -s $SYS/hostName "WBR-1310"
ACC="$SYS/user"
$RGDB -s $ACC:1/name		admin
$RGDB -s $ACC:1/password	""
$RGDB -s $ACC:1/group		0
$RGDB -s $ACC:2/name		user
$RGDB -s $ACC:2/password	""
$RGDB -s $ACC:2/group		1

WLAN_COMM="/wlan_comm"
$RGDB -s $WLAN_COMM/wlmode	4

WLAN="/wireless"
$RGDB -s $WLAN/country		"fcc"
$RGDB -s $WLAN/enable		1
$RGDB -s $WLAN/ssid		"dlink"
$RGDB -s $WLAN/autochannel	0
$RGDB -s $WLAN/channel		6
$RGDB -s $WLAN/authentication	0
$RGDB -s $WLAN/wpa/wepmode	0
$RGDB -s $WLAN/wpa/passphraseformat	1
$RGDB -s $WLAN/keylength	64
$RGDB -s $WLAN/keyformat	2
$RGDB -s $WLAN/defkey		1
$RGDB -s $WLAN/beaconinterval	100
$RGDB -s $WLAN/fraglength	2346
$RGDB -s $WLAN/rtslength	2346
$RGDB -s $WLAN/ssidhidden	0
$RGDB -s $WLAN/wlmode		1
$RGDB -s $WLAN/ctsmode		2
$RGDB -s $WLAN/w11gprotection	0
$RGDB -s $WLAN/preamble		2
$RGDB -s $WLAN/txrate		0
$RGDB -s $WLAN/txpower		1
$RGDB -s $WLAN/dtim		1
$RGDB -s $WLAN/supergmode	2
$RGDB -s $WLAN/ctsmode		2

WMMAT=$WLAN/WMM

