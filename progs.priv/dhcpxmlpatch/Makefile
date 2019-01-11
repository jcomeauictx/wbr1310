#
#

-include ../config.arch
-include ../../config.path

all:

clean:

PREFIX:=/usr/sbin


install:
	@echo -e "\033[32mInstalling dhcpxmlpatch ...\033[0m"
	[ -d $(TARGET)$(PREFIX) ] || mkdir -p $(TARGET)$(PREFIX)
	install ./dhcpxmlpatch $(TARGET)$(PREFIX)
	chmod +x $(TARGET)$(PREFIX)/dhcpxmlpatch

.PHONY: all objects clean
