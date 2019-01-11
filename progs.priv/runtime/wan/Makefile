
-include ../../config.arch
-include ../../../config.path

all:

install:
	@echo -e "\33[32mInstall runtime/wan ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install wan $(TARGET)/usr/sbin/.
	chmod +x $(TARGET)/usr/sbin/time

clean:

.PHONY: all install clean
