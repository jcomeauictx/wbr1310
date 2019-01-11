#
# The wlxmlpatch in this folder is not using anymore
# Use progs.priv/wlxmlpatch.madwifi instead.
# JumpStart is not available in DI-524
# David
#
CONFIG.PATH = ../../../config.path
ARCH.CONFIG = ../../config.arch
-include $(CONFIG.PATH)
-include $(ARCH.CONFIG)

all:
	
clean:

install:
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	cp stats $(TARGET)/usr/sbin/.
	[ -d $(TARGET)/etc/templates ] || mkdir -p $(TARGET)/etc/templates
	cp -f wlan.sh 	$(TARGET)/etc/templates/wlan.sh
	cp -f hostapd_helper.sh	$(TARGET)/etc/templates/.
	chmod +x $(TARGET)/etc/templates/wlan.sh $(TARGET)/etc/templates/hostapd_helper.sh
	[ -d $(TARGET)/etc/templates ] || mkdir -p $(TARGET)/etc/templates
	cp *.php $(TARGET)/etc/templates/.

.PHONY: all install clean
