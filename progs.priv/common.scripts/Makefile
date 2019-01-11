#

all:

install:
	@echo -e "\033[32mInstall common scripts ...\033[0m"
	[ -d $(TARGET)/etc/scripts/misc ] || mkdir $(TARGET)/etc/scripts/misc
	cp nreboot.sh		$(TARGET)/etc/scripts/misc/.
	cp profile.sh		$(TARGET)/etc/scripts/misc/.
	cp telnetd.sh		$(TARGET)/etc/scripts/misc/.
	cp ver.sh		$(TARGET)/etc/scripts/misc/.
	cp freset.sh		$(TARGET)/etc/scripts/misc/.

clean:

.PHONY: all install clean
