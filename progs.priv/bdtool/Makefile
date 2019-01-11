# Makefile for bdtool
# david_hsieh@alphanetworks.com

-include ../config.arch
-include ../../config.path


all:

clean:

install:
	@echo -e "\033[32mInstalling bdtool ...\033[0m"
	[ -d $(TARGET)/usr/sbin] || mkdir -p $(TARGET)/usr/sbin
	[ -z "$(STRIP)" ] || $(STRIP) bdtool
	cp bdtool $(TARGET)/usr/sbin

.PHONY: all install clean
