#
# Makefile for madwifi driver in rgapp3
#
# by david_hsieh@alphanetworks.com

-include ../../config.path
-include ../config.arch

MYNAME	:= Madwifi-RC5.0.1.20

all:
clean:

install:
	@echo -e "\033[32mInstall $(MYNAME) ...\033[0m"
	$(Q)[ -d $(TARGET)/lib/modules ] || mkdir -p $(TARGET)/lib/modules
	$(Q)[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	$(Q)cp modules/*.o		$(TARGET)/lib/modules/.
	$(Q)cp modules/wlanconfig	$(TARGET)/usr/sbin/.
	

.PHONY: all install clean
