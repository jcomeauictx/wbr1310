-include ../config.arch
-include ../../config.path

OBJS := sw_tcpip.o

all:
clean:

install:
	@echo -e "\033[32mInstalling driver (REDSonic turbo net)...\033[0m"
	[ -d $(TARGET)/lib/modules ] || mkdir -p $(TARGET)/lib/modules
	cp -a ./$(OBJS) $(TARGET)/lib/modules/$(OBJS)

.PHONY: all install clean
