
-include ../../config.arch
-include ../../../config.path

all:

install:
	@echo -e "\033[32mInstall runtime/diagnostic ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install diagnostic $(TARGET)/usr/sbin/.
	chmod +x $(TARGET)/usr/sbin/diagnostic

clean:

.PHONY: all install clean
