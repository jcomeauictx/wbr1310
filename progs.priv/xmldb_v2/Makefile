#
# Makefile for xmldb_v2
#

-include ../config.arch
-include ../../config.path

APPLET:=xmldb

all:
clean:

install:
	@echo -e "\03[32mInstalling xmldb version 2...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	[ -z "$(STRIP)" ] || $(STRIP) $(APPLET)
	cp $(APPLET) $(TARGET)/usr/sbin
	ln -s ./xmldb $(TARGET)/usr/sbin/xmldbc

.PHONY: all clean install

