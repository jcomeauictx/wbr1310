#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
#

# Makefile for Atheros ar531x boards
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $*.o

O_TARGET:= ar531x.o

export-objs = ar531xksyms.o

obj-y    := ar531xdbg_io.o	\
	ar531xsetup.o	\
	ar531xprom.o	\
	ar531xirq.o	\
	ar531xintr.o	\
	ar531xgpio.o	\
	ar531xksyms.o

include $(TOPDIR)/Rules.make
