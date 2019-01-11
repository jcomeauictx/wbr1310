# Makefile for dyndns
# Kevin Huang @ Alpha Networks, Feb. 2004.

-include ../config.arch
-include ../../config.path

all:

clean:

install:
	@echo -e "\033[32mInstalling DynDNS ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	[ -z "$(STRIP)" ] || $(STRIP) dyndns
	cp dyndns $(TARGET)/usr/sbin

.PHONY: all install clean
