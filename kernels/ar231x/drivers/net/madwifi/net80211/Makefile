#
# Makefile for the 802.11 NET80211 module.
#
O_TARGET := net80211.o

COMPAT=	compat
src=./
EXTRA_CFLAGS+= -DATH_SUPERG_FF=1
EXTRA_CFLAGS+= -DATH_SUPERG_DYNTURBO=1
EXTRA_CFLAGS+= -DATH_TURBO_SCAN=1
EXTRA_CFLAGS+= -DATH_SUPERG_COMP=1
EXTRA_CFLAGS+= -DATH_SUPERG_XR=1
EXTRA_CFLAGS+= -DATH_CAP_TPC=1
EXTRA_CFLAGS+= -DAH_BYTE_ORDER=AH_BIG_ENDIAN
EXTRA_CFLAGS+=	-include ${COMPAT}/compat.h -I${COMPAT} -I${src}/..

#
# There are two authenticator mechanisms: an in-kernel implementation
# (wlan_auth+wlan_radius) and an external implementation (wlan_xauth) that
# requires a user process to manage the authentication process.  By default
# the external authenticator is used.  ieee80211_proto.c has a table of module
# names that defines the default module to auto-load for each authentication
# scheme; to get the in-kernel authenticator by default modify it to load
# wlan_auth instead of wlan_xauth or manually load wlan_auth prior to use.
#
MOD_AUTH	:= wlan_xauth.o
#
# Scanning policy is split into modules.  The default policy modules
# separate station-mode scanning support from ap-mode scanning support.
#
#MOD_SCAN	:= wlan_scan_sta.o wlan_scan_ap.o
MOD_SCAN	:= wlan_scan_ap.o

obj-y += wlan.o wlan_wep.o wlan_tkip.o wlan_ccmp.o wlan_acl.o \
		   $(MOD_AUTH) $(MOD_SCAN)

wlan-objs	:= if_media.o \
		   ieee80211.o ieee80211_beacon.o ieee80211_crypto.o \
		   ieee80211_crypto_none.o ieee80211_input.o ieee80211_node.o \
		   ieee80211_output.o ieee80211_proto.o ieee80211_power.o \
		   ieee80211_scan.o ieee80211_wireless.o ieee80211_linux.o
wlan_wep-objs	:= ieee80211_crypto_wep.o
wlan_tkip-objs	:= ieee80211_crypto_tkip.o
wlan_ccmp-objs	:= ieee80211_crypto_ccmp.o
wlan_xauth-objs	:= ieee80211_xauth.o
wlan_acl-objs	:= ieee80211_acl.o
wlan_scan_sta-objs := ieee80211_scan_sta.o
wlan_scan_ap-objs  := ieee80211_scan_ap.o

export-objs	:= if_media.o \
		   ieee80211.o ieee80211_beacon.o ieee80211_crypto.o \
		   ieee80211_crypto_none.o ieee80211_input.o ieee80211_node.o \
		   ieee80211_output.o ieee80211_power.o ieee80211_proto.o \
		   ieee80211_scan.o ieee80211_wireless.o ieee80211_linux.o \
		   ieee80211_crypto_wep.o ieee80211_crypto_tkip.o \
		   ieee80211_crypto_ccmp.o ieee80211_xauth.o
list-multi	:= wlan.o wlan_wep.o wlan_tkip.o wlan_ccmp.o wlan_acl.o \
		   $(MOD_AUTH) $(MOD_SCAN)

include $(TOPDIR)/Rules.make

wlan.o:	$(wlan-objs)
	$(LD) -o wlan.o -r $(wlan-objs)
wlan_wep.o:	$(wlan_wep-objs)
	$(LD) -o wlan_wep.o -r $(wlan_wep-objs)
wlan_tkip.o:	$(wlan_tkip-objs)
	$(LD) -o wlan_tkip.o -r $(wlan_tkip-objs)
wlan_ccmp.o:	$(wlan_ccmp-objs)
	$(LD) -o wlan_ccmp.o -r $(wlan_ccmp-objs)
wlan_xauth.o:	$(wlan_xauth-objs)
	$(LD) -o wlan_xauth.o -r $(wlan_xauth-objs)
wlan_acl.o:	$(wlan_acl-objs)
	$(LD) -o wlan_acl.o -r $(wlan_acl-objs)
wlan_scan_sta.o: $(wlan_scan_sta-objs)
	$(LD) -o wlan_scan_sta.o -r $(wlan_scan_sta-objs)
wlan_scan_ap.o:	$(wlan_scan_ap-objs)
	$(LD) -o wlan_scan_ap.o -r $(wlan_scan_ap-objs)
