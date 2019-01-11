-include ../config.arch
-include ../../config.path

all:
clean:

install:
	@echo -e "\033[32mInstalling portt ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	cp portt	$(TARGET)/usr/sbin/.
	cp trigger	$(TARGET)/usr/sbin/.

.PHONY: all install clean
