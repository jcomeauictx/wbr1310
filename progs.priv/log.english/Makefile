#
#  Makefile for install log template.
#

-include ../config.arch
-include ../../config.path

all:

install:
	@echo -e "\033[32mInstall English log messages ...\033[0m"
	[ -d $(TARGET)/etc/tlogs ] || mkdir -p $(TARGET)/etc/tlogs
	cp *.tmp $(TARGET)/etc/tlogs

clean:

.PHONY: all install clean
