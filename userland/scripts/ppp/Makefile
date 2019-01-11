
all:

install:
	@echo -e "\033[32mInstalling /etc/scripts/ppp/conf ...\033[0m"
	[ -d $(TARGET)/etc/scripts/ppp/conf ] || mkdir -p $(TARGET)/etc/scripts/ppp/conf
	cp ./conf/ip-up		$(TARGET)/etc/scripts/ppp/conf/.
	cp ./conf/ip-down	$(TARGET)/etc/scripts/ppp/conf/.
	chmod +x $(TARGET)/etc/scripts/ppp/conf/*

clean:

.PHONY: all install clean
