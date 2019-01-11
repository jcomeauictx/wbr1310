# Makefile for klogd

ARCH_CONFIG=../config.arch
-include $(ARCH_CONFIG)
-include ../../config.path

APPLET = klogd

CFLAGS+= -I../../include

all:	$(APPLET)

install:
	@echo -e "\033[32mInstalling klogd ...\033[0m"
	[ -d $(TARGET)/sbin ] || mkdir -p $(TARGET)/sbin
	install $(APPLET) $(TARGET)/sbin/.

##########################################################################

$(APPLET): klogd.o
	$(CC) $(LDFLAGS) klogd.o -o $(APPLET)
	$(STRIP) $(APPLET)

klogd.o: Makefile klogd.c
	$(CC) -c $(CFLAGS) klogd.c

clean:
	rm -f *.o *~ *.gdb *.elf $(APPLET)

.PHONY:	all install clean
