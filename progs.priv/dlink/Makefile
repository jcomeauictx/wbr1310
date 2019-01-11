#
# Makefile for Corega
#

-include "../../.config"

TMPS=$(TARGET)/etc/templates

all:

install:
	@echo -e "\033[32mInstall D-Link router ...\033[0m"
#	rm -rf			$(TARGET)/etc/scripts/rg
	# install runtime script first
	cp ./runtime/*		$(TARGET)/usr/sbin/.
	# install /etc/templates
	[ -d $(TMPS) ] || mkdir -p $(TMPS)
	cp dhcpd.sh		$(TMPS)/.
	cp dnrd.sh		$(TMPS)/.
	cp dyndns.sh		$(TMPS)/.
	cp lan.sh		$(TMPS)/.
	cp logs.sh		$(TMPS)/.
	cp rg.sh		$(TMPS)/.
	cp runtimed.sh		$(TMPS)/.
	cp timezone.sh		$(TMPS)/.
	cp upnpd.sh		$(TMPS)/.
	cp wandown.sh		$(TMPS)/.
	cp wan.sh		$(TMPS)/.
	cp wan_ppp.sh		$(TMPS)/.
	cp wanup.sh		$(TMPS)/.
	cp webs.sh		$(TMPS)/.
	cp ntp.sh		$(TMPS)/.
	cp *.php		$(TMPS)/.
	[ -d $(TMPS)/ppp ] || mkdir -p $(TMPS)/ppp
	cp ppp/*		$(TMPS)/ppp/.
	[ -d $(TMPS)/dhcp ] || mkdir -p $(TMPS)/dhcp
	cp dhcp/*		$(TMPS)/dhcp/.
	[ -d $(TMPS)/misc ] || mkdir -p $(TMPS)/misc
	cp misc/*		$(TMPS)/misc/.
	[ -d $(TMPS)/rg ] || mkdir -p $(TMPS)/rg
	cp rg/*			$(TMPS)/rg/.
	[ -d $(TMPS)/httpd ] || mkdir -p $(TMPS)/httpd
	cp httpd/*		$(TMPS)/httpd/.
	[ -d $(TMPS)/upnpd ] || mkdir -p $(TMPS)/upnpd
	cp upnpd/*		$(TMPS)/upnpd/.
ifeq ($(strip $(RGAPPS_PROGS_PRIV_XMLDB_V2)),y)
	rm $(TMPS)/runtimed.sh
	rm $(TMPS)/runtimed.php
	rm $(TMPS)/runtimed_stop.php
endif
ifeq ($(strip $(RGAPPS_PROGS_PRIV_LLD2D)),y)
	cp lld2d.sh		$(TMPS)/.
	cp lld2d_run.php	$(TMPS)/.
endif

clean:

.PHONY: all install clean
