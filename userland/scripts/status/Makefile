
all:

install:
	@echo -e "\033[32mInstalling /etc/scripts/status ...\033[0m"
	[ -d $(TARGET)/etc/scripts/status ] || mkdir -p $(TARGET)/etc/scripts/status
	cp -a ./*.sh $(TARGET)/etc/scripts/status/.
	chmod +x $(TARGET)/etc/scripts/status/*

clean:

.PHONY: all install clean
