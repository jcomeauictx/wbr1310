#
#	Makefile for rgbin
#

-include ../config.arch
-include ../../config.path
-include ../../.config

RGBIN=rgbin
all:

export CFLAGS

###################################################################
# module configurations
###################################################################
ifeq (config.rgdb,$(wildcard config.rgdb))
-include config.rgdb
endif

###################################################################
# main object file
###################################################################
CMDS=
DIRS=
OBJS=main.o lrgbin.o
CFLAGS+=-DRGBIN_BOX


###################################################################
# rgdb
###################################################################
ifeq ($(RGAPPS_RGBIN_RGDB),y)
OBJS+=./src.rgdb/rgdb.o
CFLAGS+=-DINC_RGDB
DIRS+=src.rgdb
CMDS+=rgdb
endif

###################################################################
# scut
###################################################################
ifeq ($(RGAPPS_RGBIN_SCUT),y)
OBJS+=./src.scut/scut.o
CFLAGS+=-DINC_SCUT
DIRS+=src.scut
CMDS+=scut
endif

###################################################################
# pfile
###################################################################
ifeq ($(RGAPPS_RGBIN_PFILE),y)
OBJS+=./src.pfile/pfile.o
CFLAGS+=-DINC_PFILE
DIRS+=src.pfile
CMDS+=pfile
endif

###################################################################
# expand
###################################################################
ifeq ($(RGAPPS_RGBIN_EXPAND),y)
OBJS+=./src.expand/expand.o
CFLAGS+=-DINC_EXPAND
DIRS+=src.expand
CMDS+=expand
endif

###################################################################
# profile
###################################################################
ifeq ($(RGAPPS_RGBIN_PROFILE),y)
OBJS+=./src.profile/profile.o
CFLAGS+=-DINC_PROFILE
DIRS+=src.profile
CMDS+=profile
endif

###################################################################
# ntpclient
###################################################################
ifeq ($(RGAPPS_RGBIN_NTPCLIENT),y)
OBJS+=./src.ntpclient/ntpclient.o ./src.ntpclient/phaselock.o
CFLAGS+=-DINC_NTPCLIENT
DIRS+=src.ntpclient
CMDS+=ntpclient
endif

###################################################################
# stationdump
###################################################################
ifeq ($(RGAPS_RGBIN_STATIONDUMP),y)
OBJS+=./src.stationdump/stationdump.o
CFLAGS+=-DINC_STATIONDUMP
DIRS+=src.stationdump
CMDS+=stationdump
endif

###################################################################
# dhcpdump
###################################################################
ifeq ($(RGAPPS_RGBIN_DHCPDUMP),y)
OBJS+=./src.dhcpdump/dhcpdump.o
CFLAGS+=-DINC_DHCPDUMP
DIRS+=src.dhcpdump
CMDS+=dhcpdump
endif

###################################################################
# dhcpserver
###################################################################
ifeq ($(RGAPPS_RGBIN_DHCPLEASES),y)
OBJS+=./src.dhcpleases/dhcpleases.o
CFLAGS+=-DINC_DHCPLEASES
# CFLAGS+=-DREADABLE_EXPIRETIME
DIRS+=src.dhcpleases
CMDS+=dhcpleases
endif

###################################################################
# tcprequest
###################################################################
ifeq ($(RGAPPS_RGBIN_TCPREQUEST),y)
OBJS+=./src.tcprequest/tcprequest.o
CFLAGS+=-DINC_TCPREQUEST
DIRS+=src.tcprequest
CMDS+=tcprequest
endif

###################################################################
# fresetd
###################################################################
ifeq ($(RGAPPS_RGBIN_FRESETD),y)
OBJS+=./src.fresetd/fresetd.o
CFLAGS+=-DINC_FRESETD
DIRS+=src.fresetd
CMDS+=fresetd
endif

###################################################################
# brctl
###################################################################

BRCTL_OBJS = src.brctl/brctl.o src.brctl/brctl_cmd.o src.brctl/brctl_disp.o src.brctl/libbridge/libbridge.a
ifeq ($(RGAPPS_RGBIN_BRCTL),y)
OBJS+=$(BRCTL_OBJS)
CFLAGS+=-DINC_BRCTL
DIRS+=src.brctl
CMDS+=brctl
ifeq ($(RGAPPS_RGBIN_BRCTL_BWCTRL),y)
CFLAGS+=-DMULTICAST_BWCTRL
endif
endif

###################################################################
# base64: b64enc b64enc b64dec
###################################################################
ifeq ($(RGAPPS_RGBIN_BASE64),y)
OBJS+=src.base64/base64.o
CFLAGS+=-DINC_BASE64
DIRS+=src.base64
CMDS+=b64enc b64dec
endif

###################################################################
# smtpclient: smtpclient
###################################################################
ifeq ($(RGAPPS_RGBIN_SMTPCLIENT),y)
OBJS+=src.smtpclient/smtpclient_main.o
CFLAGS+=-DINC_SMTPCLIENT
DIRS+=src.smtpclient
CMDS+=smtpclient
endif

###################################################################
# dyndns: dynamic dns
###################################################################
ifeq ($(RGAPPS_RGBIN_DYNDNS),y)
OBJS+=src.dyndns/dyndns.o
CFLAGS+=-DINC_DYNDNS
DIRS+=src.dyndns
CMDS+=dyndns
endif

###################################################################
# dmzsrvmngr
###################################################################
ifeq ($(RGAPPS_RGBIN_DMZSRVMNGR),y)
OBJS+=src.dmzsrvmngr/dmzsrvmngr.o
CFLAGS+=-DINC_DMZSRVMNGR
DIRS+=src.dmzsrvmngr
CMDS+=dmzsrvmngr
endif

###################################################################
# vrtsrvmngr
###################################################################
ifeq ($(RGAPPS_RGBIN_VRTSRVMNGR),y)
OBJS+=src.vrtsrvmngr/vrtsrvmngr.o
CFLAGS+=-DINC_VRTSRVMNGR
DIRS+=src.vrtsrvmngr
CMDS+=vrtsrvmngr
endif

###################################################################
# passthroughmngr
###################################################################
ifeq ($(RGAPPS_RGBIN_PASSTHROUGHMNGR),y)
OBJS+=src.passthroughmngr/passthroughmngr.o
CFLAGS+=-DINC_PASSTHROUGHMNGR
DIRS+=src.passthroughmngr
CMDS+=passthroughmngr
endif

###################################################################
# macfiltermngr
###################################################################
ifeq ($(RGAPPS_RGBIN_MACFILTERMNGR),y)
OBJS+=src.macfiltermngr/macfiltermngr.o
CFLAGS+=-DINC_MACFILTERMNGR 
DIRS+=src.macfiltermngr
CMDS+=macfiltermngr
endif

###################################################################
# firewallmngr
###################################################################
ifeq ($(RGAPPS_RGBIN_FIREWALLMNGR),y)
OBJS+=src.firewallmngr/firewallmngr.o
CFLAGS+=-DINC_FIREWALLMNGR
DIRS+=src.firewallmngr
CMDS+=firewallmngr
endif

###################################################################
# firewallmisc
###################################################################
ifeq ($(RGAPPS_RGBIN_FIREWALLMISC),y)
OBJS+=src.firewallmisc/firewallmisc.o
CFLAGS+=-DINC_FIREWALLMISC 
DIRS+=src.firewallmisc
CMDS+=firewallmisc
endif

###################################################################
# ipfiltermngr
###################################################################
ifeq ($(RGAPPS_RGBIN_IPFILTERMNGR),y)
OBJS+=src.ipfiltermngr/ipfiltermngr.o
CFLAGS+=-DINC_IPFILTERMNGR 
DIRS+=src.ipfiltermngr
CMDS+=ipfiltermngr
endif

###################################################################
# ifstats
###################################################################
ifeq ($(RGAPPS_RGBIN_IFSTATS),y)
OBJS+=src.ifstats/ifstats.o
CFLAGS+=-DINC_IFSTATS
DIRS+=src.ifstats
CMDS+=ifstats
endif

###################################################################
# ptgen
###################################################################
ifeq ($(RGAPPS_RGBIN_PTGEN),y)
OBJS+=src.ptgen/ptgen.o
CFLAGS+=-DINC_PTGEN
DIRS+=src.ptgen
CMDS+=ptgen
endif

###################################################################
# event4upnp
###################################################################
ifeq ($(RGAPPS_RGBIN_EVENT4UPNP),y)
OBJS+=src.event4upnp/event4upnp.o
CFLAGS+=-DINC_EVENT4UPNP
DIRS+=src.event4upnp
CMDS+=event4upnp
endif

###################################################################
# paedcfg
###################################################################
ifeq ($(RGAPPS_RGBIN_PAEDCFG),y)
OBJS+=src.paedcfg/paedcfg.o
CFLAGS+=-DINC_PAEDCFG
DIRS+=src.paedcfg
CMDS+=paedcfg
endif

###################################################################
# tlogs
###################################################################
ifeq ($(RGAPPS_RGBIN_TLOGS),y)
OBJS+=src.tlogs/tlogs.o
CFLAGS+=-DINC_TLOGS
DIRS+=src.tlogs
CMDS+=tlogs
endif

###################################################################
# vconfig 
###################################################################
ifeq ($(RGAPPS_RGBIN_VCONFIG),y)
OBJS+=src.vconfig/vconfig.o
CFLAGS+=-DINC_VCONFIG
DIRS+=src.vconfig
CMDS+=vconfig
endif

###################################################################
# rgcfg
###################################################################
ifeq ($(RGAPPS_RGBIN_RGCFG),y)
OBJS+=src.rgcfg/rgcfg.o
CFLAGS+=-DINC_RGCFG
DIRS+=src.rgcfg
CMDS+=rgcfg
endif

###################################################################
# asession
###################################################################
ifeq ($(RGAPPS_RGBIN_ASESSION),y)
OBJS+=src.asession/asession.o
CFLAGS+=-DINC_ASESSION
DIRS+=src.asession
CMDS+=asession
endif

#####################################################################
# mpconvert
#####################################################################
ifeq ($(RGAPPS_RGBIN_MPCONVERT),y)
OBJS+=src.mpconvert/mpconvert.o
CFLAGS+=-DINC_MPCONVERT
DIRS+=src.mpconvert
CMDS+=mpconvert
endif

#####################################################################
# dayconvert
#####################################################################
ifeq ($(RGAPPS_RGBIN_DAYCONVERT),y)
OBJS+=src.dayconvert/dayconvert.o
CFLAGS+=-DINC_DAYCONVERT
DIRS+=src.dayconvert
CMDS+=dayconvert
endif

#####################################################################
# chnet
#####################################################################
ifeq ($(RGAPPS_RGBIN_CHNET),y)
OBJS+=src.chnet/chnet.o
CFLAGS+=-DINC_CHNET
DIRS+=src.chnet
CMDS+=chnet
endif

#####################################################################
# routetable
#####################################################################
ifeq ($(RGAPPS_RGBIN_ROUTETABLE),y)
OBJS+=src.routetable/routetable.o
CFLAGS+=-DINC_ROUTETABLE
DIRS+=src.routetable
CMDS+=routetable
endif

#####################################################################
# parseinfo
#####################################################################
ifeq ($(RGAPPS_RGBIN_PARSEINFO),y)
OBJS+=src.parseinfo/parseinfo.o
CFLAGS+=-DINC_PARSEINFO
DIRS+=src.parseinfo
CMDS+=parseinfo
endif

#####################################################################
# login
#####################################################################
ifeq ($(RGAPPS_RGBIN_LOGIN),y)
OBJS+=src.login/login.o
CFLAGS+=-DINC_LOGIN
DIRS+=src.login
CMDS+=login
endif

#####################################################################
# arpping
#####################################################################
ifeq ($(RGAPPS_RGBIN_ARPPING),y)
OBJS+=src.arpping/arpping.o
CFLAGS+=-DINC_ARPPING
DIRS+=src.arpping
CMDS+=arpping
endif

#####################################################################
# gethostip
#####################################################################
ifeq ($(RGAPPS_RGBIN_GETHOSTIP),y)
OBJS+=src.gethostip/gethostip.o
CFLAGS+=-DINC_GETHOSTIP
DIRS+=src.gethostip
CMDS+=gethostip
endif

#####################################################################
# corede
#####################################################################
ifeq ($(RGAPPS_RGBIN_COREDE),y)
OBJS+=src.corede/corede.o
CFLAGS+=-DINC_COREDE
DIRS+=src.corede
CMDS+=corede
endif

#####################################################################
# No need to modify the following section
#####################################################################

install:
	@echo -e "\033[32mInstalling RGBIN ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install $(RGBIN) $(TARGET)/usr/sbin/.
	for i in $(CMDS) ; do [ ! -f $(TARGET)/usr/sbin/$$i ] || rm -f $(TARGET)/usr/sbin/$$i ; ln -s ./$(RGBIN) $(TARGET)/usr/sbin/$$i ; done

clean:

.PHONY: all objects clean install target

