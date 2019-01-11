
-include ../../config.arch
-include ../../../config.path

all:

install:
	@echo -e "\033[32mInstall runtime/sys ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install sys $(TARGET)/usr/sbin/.
	chmod +x $(TARGET)/usr/sbin/sys

clean:

.PHONY: all install clean
