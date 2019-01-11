#####################################################################

ARCH_CONFIG=../config.arch
-include $(ARCH_CONFIG)
-include ../../config.path

APPLET=telnetd
PREFIX=/usr/sbin

#####################################################################


CFLAGS+= -DELINUX -DHAVE_DEVPTS_FS

all:	$(APPLET)

$(APPLET): telnetd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	[ -z "$(STRIP)" ] || $(STRIP) $(APPLET)

install:
	@echo -e "\033[32mInstalling $(APPLET) ...\033[0m"
	[ -d $(TARGET)$(PREFIX) ] || mkdir -p $(TARGET)$(PREFIX)
	install ./$(APPLET) $(TARGET)$(PREFIX)
	chmod +x $(TARGET)$(PREFIX)/$(APPLET)

clean:
	rm -rf *~ *.o *.gdb core *.elf $(APPLET)

.PHONY:	all install clean
