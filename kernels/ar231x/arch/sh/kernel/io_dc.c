/*
 *	$Id: //depot/sw/linuxsrc/src/kernels/mips-linux-2.4.25/arch/sh/kernel/io_dc.c#1 $
 *	I/O routines for SEGA Dreamcast
 */

#include <asm/io.h>
#include <asm/machvec.h>

unsigned long dreamcast_isa_port2addr(unsigned long offset)
{
	return offset + 0xa0000000;
}
