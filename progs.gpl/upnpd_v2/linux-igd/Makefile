#
# Makefile for upnpd_v2
#

-include ../../config.arch
-include ../../../config.path
-include ../../../.config

#########################################################################
LIBS= libupnp.a libixml.a

OPT =  -O2 -pipe -Wall
CFLAGS += -g $(OPT) -I../libupnp-1.2.1/upnp/inc -I../libupnp-1.2.1/ixml/inc

APPS = upnpd

#########################################################################
OBJS = upnpigd.o lrgbin.o

ifeq ($(RGAPS_IGD_L3FORWARDING1),y)
OBJS+=L3Forwarding1.o
CFLAGS+=-DL3FORWARDING1
endif
ifeq ($(RGAPPS_IGD_WANCOMMONIFC1),y)
OBJS+=WANCommonIFC1.o
CFLAGS+=-DWANCOMMONIFC1
endif
ifeq ($(RGAPPS_IGD_WANETHLINKC1),y)
OBJS+=WANEthLinkC1.o
CFLAGS+=-DWANETHLINKC1
endif
ifeq ($(RGAPPS_IGD_WANIPCONN1),y)
CFLAGS+=-DWANIPCONN1
ADD_WANCONN=y
endif
ifeq ($(RGAPPS_IGD_WANIPCONN2),y)
CFLAGS+=-DWANIPCONN2
ADD_WANCONN=y
endif
ifeq ($(RGAPPS_IGD_WANPPPCONN1),y)
CFLAGS+=-DWANPPPCONN1
ADD_WANCONN=y
endif
ifeq ($(RGAPPS_IGD_WANPPPCONN2),y)
CFLAGS+=-DWANPPPCONN2
ADD_WANCONN=y
endif
ifeq ($(ADD_WANCONN),y)
OBJS+=WANConnection.o
endif
ifeq ($(RGAPPS_IGD_COREGAROUTERCFG1),y)
CFLAGS+=-DCOREGAROUTERCFG1
OBJS+=CoregaRouterCfg1.o
endif

###################################################################
# hnap
###################################################################
ifeq ($(RGAPPS_PROGS_GPL_HNAP),y)
OBJS+=main.o hnap.o
CFLAGS+=-DINC_HNAP -DINC_UPNP
endif

all: $(APPS)

upnpd: Makefile $(OBJS) $(LIBS)
	$(CC)  $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o  $@
	$(STRIP) $@
	@echo "make $@ finished on `date`"

%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *.o *.gdb *.elf $(APPS)

install: upnpd
	install upnpd $(TARGET)/usr/sbin
ifeq ($(RGAPPS_PROGS_GPL_HNAP),y)
	ln -s ./upnpd $(TARGET)/usr/sbin/hnap
	[ -d $(TARGET)/etc/hnap ] || mkdir -p $(TARGET)/etc/hnap
	cp ./hnap_php/* $(TARGET)/etc/hnap
	cp ./hnap_php/.shell_action $(TARGET)/etc/hnap
	[ -d $(TARGET)/www/HNAP1 ] || mkdir -p $(TARGET)/www/HNAP1
	touch $(TARGET)/www/HNAP1/index.hnap
endif

.PHONY: all clean install
