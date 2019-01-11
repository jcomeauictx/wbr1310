# vi: set sw=4 ts=4:

all:

install:
	@echo -e "\033[32mInstalling board independent scripts/config ...\033[0m"
	cp -ra ./init.d $(TARGET)/etc/init.d
	find $(TARGET)/etc/init.d -type d -name CVS | xargs rm -rf
	ln -s /var/etc/hosts $(TARGET)/etc/hosts
	ln -sf /var/etc/ppp $(TARGET)/etc/ppp
	ln -sf /var/etc/resolv.conf $(TARGET)/etc/resolv.conf
	ln -sf /var/logs $(TARGET)/dev/log
ifdef RGAPPS_PROGS_PRIV_COMMON_SCRIPTS
	[ -d $(TARGET)/etc/scripts/misc ] || mkdir -p $(TARGET)/etc/scripts/misc
else
	make -C misc install
	make -C ppp install
	make -C rg install
	make -C status install
endif
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin

clean:

.PHONY: all install clean
