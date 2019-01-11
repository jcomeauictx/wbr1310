#############################################################################

ARCH_CONFIG = ../../config.arch
-include $(ARCH_CONFIG)
-include ../../../config.path

APPLET=dnrd
PREFIX=/usr/sbin

############################################################################

#NOMASTERMODE=y

#sources = args.c cache.c common.c dns.c lib.c main.c query.c relay.c sig.c udp.c
OBJS = main.o args.o cache.o common.o dns.o lib.o query.o relay.o sig.o udp.o

ifndef NOMASTERMODE
#sources += master.c
OBJS += master.o
else
CFLAGS+=-DNOMASTERMODE
endif

#CFLAGS+= -mapcs-32 -mtune=arm9tdmi -mshort-load-bytes

# add DEBUG flag by peterwu
#CFLAGS += -DDEBUG

all: $(APPLET)

clean:
	rm -f dnrd core tags ID *.o *.d *~ *.bak *.gdb *.elf

install:
	@echo -e "\033[32mInstalling $(APPLET) ...\033[0m"
	[ -d $(TARGET)$(PREFIX) ] || mkdir -p $(TARGET)$(PREFIX)
	install ./$(APPLET) $(TARGET)$(PREFIX)
	chmod +x $(TARGET)$(PREFIX)/$(APPLET)

$(APPLET): $(OBJS)
	$(CC) -lc $(LDFLAGS) $(OBJS) -o $(APPLET)
	$(STRIP) $(APPLET)

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

args.o: args.c
	$(CC) -c $(CFLAGS) args.c

cache.o: cache.c
	$(CC) -c $(CFLAGS) cache.c

common.o: common.c
	$(CC) -c $(CFLAGS) common.c
dns.o: dns.c
	$(CC) -c $(CFLAGS) dns.c
lib.o: lib.c
	$(CC) -c $(CFLAGS) lib.c
query.o: query.c
	$(CC) -c $(CFLAGS) query.c
relay.o: relay.c
	$(CC) -c $(CFLAGS) relay.c
sig.o: sig.c
	$(CC) -c $(CFLAGS) sig.c
udp.o: udp.c
	$(CC) -c $(CFLAGS) udp.c
master.o: master.c
	$(CC) -c $(CFLAGS) master.c
