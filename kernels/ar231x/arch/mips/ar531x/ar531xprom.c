/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright MontaVista Software Inc
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Prom setup file for ar531x
 */

#include <linux/init.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include "ar531xlnx.h"

#define COMMAND_LINE_SIZE 512

char arcs_cmdline[COMMAND_LINE_SIZE];

void __init prom_init(int argc, char *argv[])
{
    int i;
    unsigned int memcfg1;
    int bank0AC, bank1AC;
    int memsz_in_mb;

#ifdef CONFIG_CMDLINE_BOOL
    strcpy(arcs_cmdline, CONFIG_CMDLINE);
#else
    strcpy(arcs_cmdline, "console=ttyS0,9600");
#endif
    for (i=0; i<argc; i++) {
        strcat(arcs_cmdline, " ");
        strcat(arcs_cmdline, argv[i]);
    }

    mips_machgroup = MACH_GROUP_AR531X;
#ifdef CONFIG_APUNUSED
    mips_machtype = MACH_ATHEROS_UNUSED;
#endif
#ifdef CONFIG_AP30
    mips_machtype = MACH_ATHEROS_AP30;
#endif
#ifdef CONFIG_AP33
    mips_machtype = MACH_ATHEROS_AP33;
#endif
#ifdef CONFIG_AP38
    mips_machtype = MACH_ATHEROS_AP38;
#endif
#ifdef CONFIG_AP43
    mips_machtype = MACH_ATHEROS_AP43;
#endif
#ifdef CONFIG_AP48
    mips_machtype = MACH_ATHEROS_AP48;
#endif
#ifdef CONFIG_PB32
    mips_machtype = MACH_ATHEROS_PB32;
#endif


    /* Determine SDRAM size based on Address Checks done at startup */
#if CONFIG_AR5315
    /* TO-DO : compute the SDRAM size */
    memsz_in_mb=16;
#else
    memcfg1 = sysRegRead(AR531X_MEM_CFG1);
    bank0AC = (memcfg1 & MEM_CFG1_AC0) >> MEM_CFG1_AC0_S;
    bank1AC = (memcfg1 & MEM_CFG1_AC1) >> MEM_CFG1_AC1_S;
    memsz_in_mb = (bank0AC ? (1 << (bank0AC+1)) : 0)
                + (bank1AC ? (1 << (bank1AC+1)) : 0);
#endif

    /*
     * By default, use all available memory.  You can override this
     * to use, say, 8MB by specifying "mem=8M" as an argument on the
     * linux bootup command line.
     */
    add_memory_region(0, memsz_in_mb << 20, BOOT_MEM_RAM);
}

void __init prom_free_prom_memory(void)
{
}
