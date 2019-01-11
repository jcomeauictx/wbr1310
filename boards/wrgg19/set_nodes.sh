#!/bin/sh
. /etc/config/defines
echo [$0] ... > /dev/console
echo "Set fixed nodes .." > /dev/console
# Update mac address in env to bd. 
env_wan=`rgcfg getenv -n $nvram -e wanmac`
env_wlan=`rgcfg getenv -n $nvram -e wlanmac`
bd_enet=`bdtool get -d $nvram -e enet0Mac`
bd_wlan=`bdtool get -d $nvram -e wlan0Mac`
hwrev=`rgcfg getenv -n $nvram -e hwrev`
[ "$env_wan" != "$bd_enet" ] && bdtool set -d $nvram -e enet0Mac=$env_wan
[ "$env_wlan" != "$bd_wlan" ] && bdtool set -d $nvram -e wlan0Mac=$env_wlan
[ "$hwrev" = "" ] && hwrev="D1"

# Set firmware version
ver=`scut -p BUILD_VERSION= $layout_config`
build=`scut -p BUILD_NUMBER= $layout_config`
bdate=`scut -p BUILD_DATE= $layout_config -n 4`
ccode=`rgcfg getenv -n $nvram -e countrycode`
fspeed=`rgcfg getenv -n $nvram -e flashspeed`

rgdb -i -s /runtime/sys/info/firmwarebuildno "$build"
rgdb -i -s /runtime/sys/info/firmwarebuildate "$bdate"

rgdb -A /etc/scripts/set_nodes.php \
	-V "env_wan=$env_wan" \
	-V "env_wlan=$env_wlan" \
	-V "hwrev=$hwrev" \
	-V "ver=$ver" \
	-V "ccode=$ccode" \
	-V "image_sign=$image_sign" \
	-V "flashspeed=$fspeed"
