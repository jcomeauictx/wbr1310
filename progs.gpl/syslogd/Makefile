# Makefile for syslogd


ARCH_CONFIG=../config.arch
-include $(ARCH_CONFIG)
-include ../../config.path

SYSLOGD = syslogd

CFLAGS+= -I../../include

# enable syslogd -R remotehost
#CFLAGS+= -DREMOTE_LOG
#CFLAGS+= -DALPHA_DBG

# enable syslogd -C
#CFLAGS+= -DIPC_SYSLOG

all:	$(SYSLOGD)

install:
	@echo -e "\033[32mInstalling syslogd ...\033[0m"
	[ -d $(TARGET)/sbin ] || mkdir -p $(TARGET)/sbin
	install $(SYSLOGD) $(TARGET)/sbin/.

##########################################################################

$(SYSLOGD): syslogd.o
	$(CC) $(LDFLAGS) syslogd.o -o $(SYSLOGD)
	$(STRIP) $(SYSLOGD)

syslogd.o: Makefile syslogd.c
	$(CC) -c $(CFLAGS) syslogd.c

clean:
	rm -f *.o *~ *.gdb *.elf $(SYSLOGD)

.PHONY:	all install clean
