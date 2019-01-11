
all:

install:
	@echo -e "\033[32mInstalling runtime/stats ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install stats $(TARGET)/usr/sbin
	chmod +x $(TARGET)/usr/sbin/stats

clean:

.PHONY: all install clean
