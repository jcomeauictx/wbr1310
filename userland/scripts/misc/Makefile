
all:

install:
	@echo -e "\033[32mInstalling /etc/scripts/misc ...\033[0m"
	[ -d $(TARGET)/etc/scripts/misc ] || mkdir -p $(TARGET)/etc/scripts/misc
	cp -a *.sh $(TARGET)/etc/scripts/misc
	chmod +x $(TARGET)/etc/scripts/misc/*

clean:

.PHONY: all install clean
