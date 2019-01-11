################################################################################
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright © 2004 Atheros Communications, Inc.,  All Rights Reserved.
#
# Makefile for Atheros ar531x ethernet driver
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
################################################################################

#
# Makefile for the Atheros ar531x ethernet driver
#

obj= .

obj-m		 += ae531x.o
ae531x-objs	 := ae531xlnx.o ae531xmac.o ae531xproc.o
export-objs	 := ae531xlnx.o
list-multi	 := ae531x.o

ifeq ($(CONFIG_KENDIN_ENET_PHY),y)
  ae531x-objs += rtPhy.o
endif
ifeq ($(CONFIG_KENDIN_KS8995XA_ENET_PHY),y)
  ae531x-objs += kendSwitchPhy.o
endif
ifeq ($(CONFIG_MARVELL_ENET_PHY),y)
  ae531x-objs += mvPhy.o
endif
ifeq ($(CONFIG_ICPLUS_ENET_PHY),y)
  ae531x-objs += ipPhy.o
endif

#
# If building directly into kernel
#
ifneq ($(MAKING_MODULES),1)
obj-$(CONFIG_NET_ATHEROS_ETHER) := ae531x.o $(ae531x-objs)
O_TARGET := ae531x.o
endif

INCS += -I.

EXTRA_CFLAGS+=$(INCS) ${COPTS} -g
ifeq ($(DEBUG_BUILD),1)
EXTRA_CFLAGS+=-DDEBUG
endif

# release tag versioning
-include $(KERNELPATH)/ath_version.mk

-include $(TOPDIR)/Rules.make
STRIP=	${TOOLPREFIX}strip

ifndef MODPATH
MODPATH = ${KERNELPATH}/arch/mips/ar531x/ROOTDISK/rootdir/lib/modules/${KERNELRELEASE}/
endif

all:
	$(MAKE)	-C ${KERNELPATH} SUBDIRS=$(shell pwd) modules

install: all
	$(STRIP) -S ae531x.o
	cp ae531x.o ${KERNELPATH}/arch/mips/ar531x/ROOTDISK/rootdir/lib/modules/$(strip $(shell head -n 1 $(KERNELPATH)/Makefile | cut -f 2 -d'=')).$(strip $(shell head -n 2 $(KERNELPATH)/Makefile | tail -1 | cut -f 2 -d'=')).$(strip $(shell head -n 3 $(KERNELPATH)/Makefile | tail -1 | cut -f 2 -d'='))${EXTRAVERSION}/net/.

clean:
	-rm -f *~ *.o *.ko *.mod.c
	-rm -f .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd

ae531x.o : $(ae531x-objs)
	$(LD) -o ae531x.o -r $(ae531x-objs)
