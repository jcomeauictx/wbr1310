###########################################################################
# Makefile for BPALogin v2.0
##

##

##
# Locations of system utilities as determined by configure
#CC=/opt/toolchain_mips_nofpu/bin/mips-linux-gcc
##
#
#CFLAGS=-g -O2 -Wall 
LIBS=-lnsl 
#LDFLAGS=
-include ../config.arch
-include ../../config.path

INCLUDE_DIRS = -I../../include
OBJS = bpalogin.o protocol.o transaction.o gen_md5.o unixmain.o
CFLAGS += $(INCLUDE_DIRS)


bpalogin: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	[ -z "$(STRIP)" ] || $(STRIP) bpalogin

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr *.o bpalogin *~

install:
	@echo -e "\033[32mInstalling bpalogin ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install bpalogin $(TARGET)/usr/sbin/.

ifdef RGAPPS_PROGS_PRIV_COMMON_SCRIPTS
	@echo -e "\033[32mSkip installing bpalogin.php ...\033[0m"
else
	[ -d $(TARGET)/etc/template ] || mkdir -p $(TARGET)/etc/template
	cp bpalogin.php $(TARGET)/etc/template/bpalogin.php
endif

distclean: clean
	rm -f bpalogin config.log config.cache config.status Makefile bpalogin.h

#bpalogin.h: bpalogin.h.in
#	$(SED) 's#@@BPALOGIN_CONF@@#$(ETCPATH)/bpalogin.conf#' < bpalogin.h.in > bpalogin.h

$(OBJS): bpalogin.h

romfs:
	$(ROMFSINST) bpalogin /bin/bpalogin 
