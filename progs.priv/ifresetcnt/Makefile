-include ../../config.path
-include ../config.arch

O_TARGET := ifresetcnt.o

all:

clean:

install:
	@echo -e "\033[32mInstalling driver (Network interface reset count)...\033[0m"
	[ -d $(TARGET)/lib/modules ] || mkdir -p $(TARGET)/lib/modules
	cp -a ./$(O_TARGET) $(TARGET)/lib/modules/

.PHONY: all install clean
