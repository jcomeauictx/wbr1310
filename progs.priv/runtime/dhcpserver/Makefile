
-include ../../config.arch
-include ../../../config.path

all:

install:
	@echo -e "\033[32mInstalling runtime/dhcpserver ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install dhcpserver $(TARGET)/usr/sbin
	chmod +x $(TARGET)/usr/sbin/dhcpserver

clean:

.PHONY: all install clean
