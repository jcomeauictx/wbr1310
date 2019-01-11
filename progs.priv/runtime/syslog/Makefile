
-include ../../config.arch
-include ../../../config.path

all:

install:
	@echo -e "\033[32mInstalling runtime/syslog ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install syslog $(TARGET)/usr/sbin/.
	chmod +x $(TARGET)/usr/sbin/syslog

clean:

.PHONY: all install clean
