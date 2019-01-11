
all:

install:
	@echo -e "\033[32mInstalling /etc/scripts/rg ...\033[0m"
	[ -d $(TARGET)/etc/scripts/rg ] || mkdir -p $(TARGET)/etc/scripts/rg
	cp -a ./*.sh $(TARGET)/etc/scripts/rg/.
	chmod +x $(TARGET)/etc/scripts/rg/*

clean:

.PHONY:	all install clean
