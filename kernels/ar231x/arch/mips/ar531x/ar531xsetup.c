/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Initialization for ar531x SOC.
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/serial.h>
#include <linux/types.h>
#include <linux/string.h>

#include <asm/reboot.h>
#include <asm/io.h>
#include <asm/time.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/serial.h>

#include "ar531xlnx.h"

void
ar531x_restart(char *command)
{
    for(;;) {
#if CONFIG_AR5315
    /* 
    ** Cold reset does not work,work around is to use the GPIO reset bit.  
    */
    unsigned int reg;
    reg = sysRegRead(AR5315_GPIO_DO);
    reg &= ~(1 << AR5315_RESET_GPIO);
    sysRegWrite(AR5315_GPIO_DO, reg);
    (void)sysRegRead(AR5315_GPIO_DO); /* flush write to hardware */

    /* Ar2317 */
    sysRegWrite(AR5315_COLD_RESET, AR5317_RESET_SYSTEM);

#else
        sysRegWrite(AR531X_RESET, AR531X_RESET_SYSTEM);
#endif
    }
}

void
ar531x_halt(void)
{
        printk(KERN_NOTICE "\n** You can safely turn off the power\n");
        while (1);
}

void
ar531x_power_off(void)
{
        ar531x_halt();
}

const char *
get_system_type(void)
{
#if CONFIG_AR5315
	return "Atheros AR531X_COBRA";
#else
	return "Atheros AR531X";
#endif
}

/*
 * This table is indexed by bits 5..4 of the CLOCKCTL1 register
 * to determine the predevisor value.
 */
static int CLOCKCTL1_PREDIVIDE_TABLE[4] = {
    1,
    2,
    4,
    5
};

#if CONFIG_AR5315
static int PLLC_DIVIDE_TABLE[5] = {
    2,
    3,
    4,
    6,
    3
};

unsigned int
ar531x_cpu_frequency(void)
{
    static unsigned int ar531x_calculated_cpu_freq=0;
    unsigned int clockCtl,pllcCtrl,cpuDiv;
    unsigned int pllcOut,refdiv,fdiv,divby2;

    if(ar531x_calculated_cpu_freq) 
         return ar531x_calculated_cpu_freq; 

    
    pllcCtrl = sysRegRead(AR5315_PLLC_CTL);
    refdiv = (pllcCtrl & PLLC_REF_DIV_M) >> PLLC_REF_DIV_S;
    refdiv = CLOCKCTL1_PREDIVIDE_TABLE[refdiv];
    fdiv = (pllcCtrl & PLLC_FDBACK_DIV_M) >> PLLC_FDBACK_DIV_S;
    divby2 = (pllcCtrl & PLLC_ADD_FDBACK_DIV_M) >> PLLC_ADD_FDBACK_DIV_S;
    divby2 += 1;
    pllcOut = (40000000/refdiv)*(2*divby2)*fdiv;

    clockCtl = sysRegRead(AR5315_CPUCLK);

    /* clkm input selected */
    if((clockCtl & CPUCLK_CLK_SEL_M) == 0  || (clockCtl & CPUCLK_CLK_SEL_M) == 1 ) {
       unsigned int clkMdiv;
       clkMdiv = (pllcCtrl & PLLC_CLKM_DIV_M) >> PLLC_CLKM_DIV_S;
       clkMdiv = PLLC_DIVIDE_TABLE[clkMdiv];

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (pllcOut/(clkMdiv * cpuDiv)) ;
        
       return ar531x_calculated_cpu_freq; 
    } 
    
    /* clkc input selected */
    if((clockCtl & CPUCLK_CLK_SEL_M) == 2 ) {
       unsigned int clkCdiv;
       clkCdiv = (pllcCtrl & PLLC_CLKC_DIV_M) >> PLLC_CLKC_DIV_S;
       clkCdiv = PLLC_DIVIDE_TABLE[clkCdiv];

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (pllcOut/(clkCdiv * cpuDiv)) ;
        
       return ar531x_calculated_cpu_freq; 
    } else {   /* ref_clk  selected */

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (40000000/(cpuDiv)) ;
       return ar531x_calculated_cpu_freq; 
    }
}

unsigned int
ar531x_apb_frequency(void)
{
    static unsigned int ar531x_calculated_cpu_freq=0;
    unsigned int clockCtl,pllcCtrl,cpuDiv;
    unsigned int pllcOut,refdiv,fdiv,divby2;

    if(ar531x_calculated_cpu_freq) 
         return ar531x_calculated_cpu_freq; 

    
    pllcCtrl = sysRegRead(AR5315_PLLC_CTL);
    refdiv = (pllcCtrl & PLLC_REF_DIV_M) >> PLLC_REF_DIV_S;
    refdiv = CLOCKCTL1_PREDIVIDE_TABLE[refdiv];
    fdiv = (pllcCtrl & PLLC_FDBACK_DIV_M) >> PLLC_FDBACK_DIV_S;
    divby2 = (pllcCtrl & PLLC_ADD_FDBACK_DIV_M) >> PLLC_ADD_FDBACK_DIV_S;
    divby2 += 1;
    pllcOut = (40000000/refdiv)*(2*divby2)*fdiv;

    clockCtl = sysRegRead(AR5315_AMBACLK);

    /* clkm input selected */
    if((clockCtl & CPUCLK_CLK_SEL_M) == 0  || (clockCtl & CPUCLK_CLK_SEL_M) == 1 ) {
       unsigned int clkMdiv;
       clkMdiv = (pllcCtrl & PLLC_CLKM_DIV_M) >> PLLC_CLKM_DIV_S;
       clkMdiv = PLLC_DIVIDE_TABLE[clkMdiv];

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (pllcOut/(clkMdiv * cpuDiv)) ;
        
       return ar531x_calculated_cpu_freq; 
    } 
    
    /* clkc input selected */
    if((clockCtl & CPUCLK_CLK_SEL_M) == 2 ) {
       unsigned int clkCdiv;
       clkCdiv = (pllcCtrl & PLLC_CLKC_DIV_M) >> PLLC_CLKC_DIV_S;
       clkCdiv = PLLC_DIVIDE_TABLE[clkCdiv];

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (pllcOut/(clkCdiv * cpuDiv)) ;
        
       return ar531x_calculated_cpu_freq; 
    } else {   /* ref_clk  selected */

       cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;  
       if(cpuDiv)  cpuDiv *= 2;
       else cpuDiv=1;

       ar531x_calculated_cpu_freq= (40000000/(cpuDiv)) ;
       return ar531x_calculated_cpu_freq; 
    }
}

#else
unsigned int
ar531x_cpu_frequency(void)
{
	static unsigned int ar531x_calculated_cpu_freq;
        unsigned int clockctl1_predivide_mask;
        unsigned int clockctl1_predivide_shift;
        unsigned int clockctl1_multiplier_mask;
        unsigned int clockctl1_multiplier_shift;
        unsigned int clockctl1_doubler_mask;
        int wisoc_revision;

        /*
         * Trust the bootrom's idea of cpu frequency.
         */
        ar531x_calculated_cpu_freq = sysRegRead(AR5312_SCRATCH);
        if (ar531x_calculated_cpu_freq)
	    return ar531x_calculated_cpu_freq;

        wisoc_revision = (sysRegRead(AR531X_REV) & AR531X_REV_MAJ) >> AR531X_REV_MAJ_S;

        if (wisoc_revision == AR531X_REV_MAJ_AR2313) {
            clockctl1_predivide_mask = AR2313_CLOCKCTL1_PREDIVIDE_MASK;
            clockctl1_predivide_shift = AR2313_CLOCKCTL1_PREDIVIDE_SHIFT;
            clockctl1_multiplier_mask = AR2313_CLOCKCTL1_MULTIPLIER_MASK;
            clockctl1_multiplier_shift = AR2313_CLOCKCTL1_MULTIPLIER_SHIFT;
            clockctl1_doubler_mask = AR2313_CLOCKCTL1_DOUBLER_MASK;
        } else { /* AR5312 and AR2312 */
            clockctl1_predivide_mask = AR5312_CLOCKCTL1_PREDIVIDE_MASK;
            clockctl1_predivide_shift = AR5312_CLOCKCTL1_PREDIVIDE_SHIFT;
            clockctl1_multiplier_mask = AR5312_CLOCKCTL1_MULTIPLIER_MASK;
            clockctl1_multiplier_shift = AR5312_CLOCKCTL1_MULTIPLIER_SHIFT;
            clockctl1_doubler_mask = AR5312_CLOCKCTL1_DOUBLER_MASK;
        }

        /*
         * Clocking is derived from a fixed 40MHz input clock.
         *  cpuFreq = InputClock * MULT (where MULT is PLL multiplier)
         *
         *  sysFreq = cpuFreq / 4       (used for APB clock, serial,
         *                               flash, Timer, Watchdog Timer)
         *
         *  cntFreq = cpuFreq / 2       (use for CPU count/compare)
         *
         * So, for example, with a PLL multiplier of 5, we have
         *  cpuFrez = 200MHz
         *  sysFreq = 50MHz
         *  cntFreq = 100MHz
         *
         * We compute the CPU frequency, based on PLL settings.
         */
	if (ar531x_calculated_cpu_freq == 0) {
            unsigned int clockCtl1 = sysRegRead(AR5312_CLOCKCTL1);

            int preDivideSelect = (clockCtl1 & clockctl1_predivide_mask) >>
                                   clockctl1_predivide_shift;

            int preDivisor = CLOCKCTL1_PREDIVIDE_TABLE[preDivideSelect];

            int multiplier = (clockCtl1 & clockctl1_multiplier_mask) >>
                              clockctl1_multiplier_shift;

            if (clockCtl1 & clockctl1_doubler_mask) {
                multiplier = multiplier << 1;
            }

            ar531x_calculated_cpu_freq = (40000000 / preDivisor) * multiplier;
        }

	return ar531x_calculated_cpu_freq;
}
#endif

unsigned int
ar531x_sys_frequency(void)
{
	static unsigned int ar531x_calculated_sys_freq = 0;

	if (ar531x_calculated_sys_freq == 0) {
		ar531x_calculated_sys_freq = ar531x_cpu_frequency() / 4;
	}

	return ar531x_calculated_sys_freq;
}

static void __init
flash_setup(void)
{
    UINT32 flash_ctl;
#ifndef CONFIG_AR5315
    /* Configure flash bank 0 */
    flash_ctl = FLASHCTL_E |
                FLASHCTL_AC_8M |
                FLASHCTL_RBLE |
                (0x01 << FLASHCTL_IDCY_S) |
                (0x07 << FLASHCTL_WST1_S) |
                (0x07 << FLASHCTL_WST2_S) |
                (sysRegRead(AR531X_FLASHCTL0) & FLASHCTL_MW);

    sysRegWrite(AR531X_FLASHCTL0, flash_ctl);

    /* Disable other flash banks */
    sysRegWrite(AR531X_FLASHCTL1,
                sysRegRead(AR531X_FLASHCTL1) & ~(FLASHCTL_E | FLASHCTL_AC));

    sysRegWrite(AR531X_FLASHCTL2,
                sysRegRead(AR531X_FLASHCTL2) & ~(FLASHCTL_E | FLASHCTL_AC));
#endif
}



void __init
serial_setup(void)
{
	struct serial_struct s;

	memset(&s, 0, sizeof(s));

	s.flags = STD_COM_FLAGS;
	s.io_type = SERIAL_IO_MEM;
#if CONFIG_AR5315
	s.baud_base = ar531x_apb_frequency()/16;
#else
	s.baud_base = ar531x_sys_frequency()/16;
#endif
	s.irq = AR531X_MISC_IRQ_UART0;
	s.iomem_reg_shift = 2;
#if CONFIG_AR5315
	s.iomem_base = (u8 *)AR5315_UART0;
#else
	s.iomem_base = (u8 *)AR531X_UART0;
#endif

	if (early_serial_setup(&s) != 0)
		printk(KERN_ERR "early_serial_setup failed\n");
}

extern int setup_irq(unsigned int irq, struct irqaction *irqaction);
static void __init
ar531x_timer_setup(struct irqaction *irq)
{
        unsigned int count;

	/* Usually irq is timer_irqaction (timer_interrupt) */
      	setup_irq(AR531X_IRQ_CPU_CLOCK, irq);

        /* to generate the first CPU timer interrupt */
        count = read_c0_count();
        write_c0_compare(count + 1000);
}

extern void (*board_time_init)(void);

static void __init
ar531x_time_init(void)
{
	mips_hpt_frequency = ar531x_cpu_frequency() / 2;
}

void __init
ar531x_setup(void)
{
	/* Clear any lingering AHB errors */
#if CONFIG_AR5315
	unsigned int config = read_c0_config();
	write_c0_config(config & ~0x3);
	sysRegWrite(AR5315_AHB_ERR0,AHB_ERROR_DET);
	sysRegRead(AR5315_AHB_ERR1);
	sysRegWrite(AR5315_WDC, WDC_IGNORE_EXPIRATION);
#else
	sysRegRead(AR531X_PROCADDR);
	sysRegRead(AR531X_DMAADDR);

	sysRegWrite(AR531X_WD_CTRL, AR531X_WD_CTRL_IGNORE_EXPIRATION);

#endif

        /* Disable data watchpoints */
        write_c0_watchlo0(0);

	board_time_init = ar531x_time_init;
        board_timer_setup = ar531x_timer_setup;

        _machine_restart = ar531x_restart;
        _machine_halt = ar531x_halt;
        _machine_power_off = ar531x_power_off;

        flash_setup();
        serial_setup();
}
