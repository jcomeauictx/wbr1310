/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Interrupt support for AR531X WiSOC.
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/reboot.h>

#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/gdb-stub.h>

#include "ar531xlnx.h"
#include <asm/irq_cpu.h>

extern int setup_irq(unsigned int irq, struct irqaction *irqaction);

static void ar531x_misc_intr_enable(unsigned int irq);
static void ar531x_misc_intr_disable(unsigned int irq);

/* Turn on the specified AR531X_MISC_IRQ interrupt */
static unsigned int
ar531x_misc_intr_startup(unsigned int irq)
{
	ar531x_misc_intr_enable(irq);
	return 0;
}

/* Turn off the specified AR531X_MISC_IRQ interrupt */
static void
ar531x_misc_intr_shutdown(unsigned int irq)
{
	ar531x_misc_intr_disable(irq);
}

/* Enable the specified AR531X_MISC_IRQ interrupt */
static void
ar531x_misc_intr_enable(unsigned int irq)
{
	unsigned int imr;

#if CONFIG_AR5315
	imr = sysRegRead(AR5315_IMR);
        switch(irq)
        {
           case AR531X_MISC_IRQ_TIMER:
             imr |= IMR_TIMER;
	     break;

	   case AR531X_MISC_IRQ_AHB_PROC:
             imr |= IMR_AHB;
	     break;

           case AR531X_MISC_IRQ_AHB_DMA:
             imr |= 0/* ?? */;
	     break;
	     /*
	   case	AR531X_ISR_GPIO:
             imr |= IMR_GPIO;
             break;
             */

	   case AR531X_MISC_IRQ_UART0:
             imr |= IMR_UART0;
             break;


           case	AR531X_MISC_IRQ_WATCHDOG:
             imr |= IMR_WD;
             break;

	   case AR531X_MISC_IRQ_LOCAL:
             imr |= 0/* ?? */;
             break;

        }
	sysRegWrite(AR5315_IMR, imr);
	imr=sysRegRead(AR5315_IMR); /* flush write buffer */
        //printk("enable Interrupt irq 0x%x imr 0x%x \n",irq,imr);

#else
	imr = sysRegRead(AR531X_IMR);
	imr |= (1 << (irq - AR531X_MISC_IRQ_BASE - 1));
	sysRegWrite(AR531X_IMR, imr);
	sysRegRead(AR531X_IMR); /* flush write buffer */
#endif
}

/* Disable the specified AR531X_MISC_IRQ interrupt */
static void
ar531x_misc_intr_disable(unsigned int irq)
{
	unsigned int imr;

#if CONFIG_AR5315
	imr = sysRegRead(AR5315_IMR);
        switch(irq)
        {
           case AR531X_MISC_IRQ_TIMER:
             imr &= (~IMR_TIMER);
	     break;

	   case AR531X_MISC_IRQ_AHB_PROC:
             imr &= (~IMR_AHB);
	     break;

           case AR531X_MISC_IRQ_AHB_DMA:
        //     imr &= 0/* ?? */;
             imr &= ~0/* ?? */;
	     break;
	     /*
	   case	AR531X_ISR_GPIO:
             imr &= ~IMR_GPIO;
             break;
             */

	   case AR531X_MISC_IRQ_UART0:
             imr &= (~IMR_UART0);
             break;

           case	AR531X_MISC_IRQ_WATCHDOG:
             imr &= (~IMR_WD);
             break;

	   case AR531X_MISC_IRQ_LOCAL:
             imr &= ~0/* ?? */;
             break;

        }
	sysRegWrite(AR5315_IMR, imr);
	sysRegRead(AR5315_IMR); /* flush write buffer */
#else
	imr = sysRegRead(AR531X_IMR);
	imr &= ~(1 << (irq - AR531X_MISC_IRQ_BASE - 1));
	sysRegWrite(AR531X_IMR, imr);
	sysRegRead(AR531X_IMR); /* flush write buffer */
#endif
}

static void
ar531x_misc_intr_ack(unsigned int irq)
{
	ar531x_misc_intr_disable(irq);
}

static void
ar531x_misc_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar531x_misc_intr_enable(irq);
}

static void
ar531x_misc_intr_set_affinity(unsigned int irq, unsigned long mask)
{
	/* Only 1 CPU; ignore affinity request */
}

struct hw_interrupt_type ar531x_misc_intr_controller = {
	"AR531X MISC",
	ar531x_misc_intr_startup,
	ar531x_misc_intr_shutdown,
	ar531x_misc_intr_enable,
	ar531x_misc_intr_disable,
	ar531x_misc_intr_ack,
	ar531x_misc_intr_end,
	ar531x_misc_intr_set_affinity,
};

int ar531x_misc_irq_base;

/*
 * Determine interrupt source among interrupts that use IP6
 */
void
ar531x_misc_intr_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR531X_MISC_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].handler = &ar531x_misc_intr_controller;
	}

	ar531x_misc_irq_base = irq_base;
}

/* ARGSUSED */
void
spurious_irq_handler(int cpl, void *dev_id, struct pt_regs *regs)
{
    /* 
    printk("spurious_irq_handler: %d  cause=0x%8.8x  status=0x%8.8x\n",
           cpl, cause_intrs, status_intrs); 
    */
}

/* ARGSUSED */
void
spurious_misc_handler(int cpl, void *dev_id, struct pt_regs *regs)
{
    /*
    printk("spurious_misc_handler: 0x%x isr=0x%8.8x imr=0x%8.8x\n",
           cpl, ar531x_isr, ar531x_imr);
    */
}

void
ar531x_timer_handler(int cpl, void *dev_id, struct pt_regs *regs)
{
#if CONFIG_AR5315
	(void)sysRegRead(AR5315_TIMER); /* clear interrupt */
#else
	(void)sysRegRead(AR531X_TIMER); /* clear interrupt */
#endif
}

void
ar531x_ahb_proc_handler(int cpl, void *dev_id, struct pt_regs *regs)
{
    u32 procAddr;
    u32 proc1;
    u32 dmaAddr;
    u32 dma1;
#if CONFIG_AR5315
    sysRegWrite(AR5315_AHB_ERR0,AHB_ERROR_DET);
    sysRegRead(AR5315_AHB_ERR1);
#else
    proc1 = sysRegRead(AR531X_PROC1);
    procAddr = sysRegRead(AR531X_PROCADDR); /* clears error state */
    dma1 = sysRegRead(AR531X_DMA1);
    dmaAddr = sysRegRead(AR531X_DMAADDR);   /* clears error state */
#endif

    printk("AHB interrupt: PROCADDR=0x%8.8x  PROC1=0x%8.8x  DMAADDR=0x%8.8x  DMA1=0x%8.8x\n",
        procAddr, proc1, dmaAddr, dma1);
        
    //machine_restart("AHB error"); /* Catastrophic failure */
}

static struct irqaction cascade  =
	{no_action, SA_INTERRUPT, 0, "cascade",
            NULL, NULL};

static struct irqaction spurious_irq =
	{spurious_irq_handler, SA_INTERRUPT, 0, "spurious_irq",
            NULL, NULL};

static struct irqaction spurious_misc =
	{spurious_misc_handler, SA_INTERRUPT, 0, "spurious_misc",
            NULL, NULL};

static struct irqaction ar531x_timer_interrupt =
	{ar531x_timer_handler, SA_INTERRUPT, 0, "ar531x_timer_interrupt",
            NULL, NULL};

static struct irqaction ar531x_ahb_proc_interrupt =
	{ar531x_ahb_proc_handler, SA_INTERRUPT, 0, "ar531x_ahb_proc_interrupt",
            NULL, NULL};

extern asmlinkage void ar531x_interrupt_receive(void);

/*
 * Called when an interrupt is received, this function
 * determines exactly which interrupt it was, and it
 * invokes the appropriate handler.
 *
 * Implicitly, we also define interrupt priority by
 * choosing which to dispatch first.
 */
extern void dump_uart(void *);

#if CONFIG_AR5315

void
ar531x_irq_dispatch(struct pt_regs *regs)
{
	int cause_intrs = regs->cp0_cause;
	int status_intrs = regs->cp0_status;
	int pending = cause_intrs & status_intrs;

	if (pending & CAUSEF_IP3) {
		do_IRQ(AR531X_IRQ_WLAN0_INTRS, regs);
	}		
	else if (pending & CAUSEF_IP4) {
		do_IRQ(AR531X_IRQ_ENET0_INTRS, regs);
	}
	else if (pending & CAUSEF_IP2) {
		AR531X_REG ar531x_isr = sysRegRead(AR5315_ISR);
		AR531X_REG ar531x_imr = sysRegRead(AR5315_IMR);
		unsigned int ar531x_misc_intrs = ar531x_isr & ar531x_imr;

             	if (ar531x_misc_intrs & ISR_TIMER)
                        		  do_IRQ(AR531X_MISC_IRQ_TIMER, regs);
		else if (ar531x_misc_intrs & ISR_AHB)
					  do_IRQ(AR531X_MISC_IRQ_AHB_PROC, regs);
		else if (ar531x_misc_intrs & ISR_GPIO)
                {
                    int i;
                    u32 gpioIntPending;

                    gpioIntPending = sysRegRead(AR5315_GPIO_DI) & gpioIntMask;
                    for (i=0; i<AR531X_GPIO_IRQ_COUNT; i++) {
                        if (gpioIntPending & (1 << i))
                        		      do_IRQ(AR531X_GPIO_IRQ(i), regs);
                    }
                }
		else if (ar531x_misc_intrs & ISR_UART0) {
				do_IRQ(AR531X_MISC_IRQ_UART0, regs);
#if CONFIG_KGDB
                        if (kgdbInterrupt()) {
                                if (!user_mode(regs))
		                    set_async_breakpoint((unsigned long *)&regs->cp0_epc);
                        }
#endif 	/* CONFIG_KGDB */
                }
		else if (ar531x_misc_intrs & ISR_WD)
			do_IRQ(AR531X_MISC_IRQ_WATCHDOG, regs);
		else
			do_IRQ(AR531X_MISC_IRQ_NONE, regs);
	} else if (pending & CAUSEF_IP7) {
		do_IRQ(AR531X_IRQ_CPU_CLOCK, regs);
        }
	else {
		do_IRQ(AR531X_IRQ_NONE, regs);
        }
}

#else

void
ar531x_irq_dispatch(struct pt_regs *regs)
{
	int cause_intrs = regs->cp0_cause;
	int status_intrs = regs->cp0_status;
	int pending = cause_intrs & status_intrs;

	if (pending & CAUSEF_IP2) {
		do_IRQ(AR531X_IRQ_WLAN0_INTRS, regs);
	}		
	else if (pending & CAUSEF_IP3) {
		do_IRQ(AR531X_IRQ_ENET0_INTRS, regs);
	}
	else if (pending & CAUSEF_IP4) {
		do_IRQ(AR531X_IRQ_ENET1_INTRS, regs);
	}
	else if (pending & CAUSEF_IP5) {
		do_IRQ(AR531X_IRQ_WLAN1_INTRS, regs);
	}
	else if (pending & CAUSEF_IP6) {
		AR531X_REG ar531x_isr = sysRegRead(AR531X_ISR);
		AR531X_REG ar531x_imr = sysRegRead(AR531X_IMR);
		unsigned int ar531x_misc_intrs = ar531x_isr & ar531x_imr;

		if (ar531x_misc_intrs & AR531X_ISR_TIMER)
			do_IRQ(AR531X_MISC_IRQ_TIMER, regs);
		else if (ar531x_misc_intrs & AR531X_ISR_AHBPROC)
			do_IRQ(AR531X_MISC_IRQ_AHB_PROC, regs);
		else if (ar531x_misc_intrs & AR531X_ISR_AHBDMA)
			do_IRQ(AR531X_MISC_IRQ_AHB_DMA, regs);
		else if (ar531x_misc_intrs & AR531X_ISR_GPIO)
                {
                    int i;
                    u32 gpioIntPending;

                    gpioIntPending = sysRegRead(AR531X_GPIO_DI) & gpioIntMask;
                    for (i=0; i<AR531X_GPIO_IRQ_COUNT; i++) {
                        if (gpioIntPending & (1 << i))
                            do_IRQ(AR531X_GPIO_IRQ(i), regs);
                    }
                }
		else if ((ar531x_misc_intrs & AR531X_ISR_UART0) ||
		         (ar531x_misc_intrs & AR531X_ISR_UART0DMA)) {
			do_IRQ(AR531X_MISC_IRQ_UART0, regs);
#if CONFIG_KGDB
                        if (kgdbInterrupt()) {
                                if (!user_mode(regs))
		                    set_async_breakpoint((unsigned long *)&regs->cp0_epc);
                        }
#endif 	/* CONFIG_KGDB */
                }
		else if (ar531x_misc_intrs & AR531X_ISR_WD)
			do_IRQ(AR531X_MISC_IRQ_WATCHDOG, regs);
		else if (ar531x_misc_intrs & AR531X_ISR_LOCAL)
			do_IRQ(AR531X_MISC_IRQ_LOCAL, regs);
		else
			do_IRQ(AR531X_MISC_IRQ_NONE, regs);
	} else if (pending & CAUSEF_IP7) {
		do_IRQ(AR531X_IRQ_CPU_CLOCK, regs);
	} else
		do_IRQ(AR531X_IRQ_NONE, regs);
}

#endif

void __init init_IRQ(void)
{
	init_generic_irq();
	set_except_vector(0, ar531x_interrupt_receive);

	/* Initialize interrupt controllers */
	mips_cpu_irq_init(MIPS_CPU_IRQ_BASE);
	ar531x_misc_intr_init(AR531X_MISC_IRQ_BASE);
        ar531x_gpio_intr_init(AR531X_GPIO_IRQ_BASE);
	setup_irq(AR531X_IRQ_MISC_INTRS, &cascade);
	/*
         * AR531X_IRQ_CPU_CLOCK is setup by ar531x_timer_setup.
         */

	/* Default "spurious interrupt" handlers */
	setup_irq(AR531X_IRQ_NONE, &spurious_irq);
	setup_irq(AR531X_MISC_IRQ_NONE, &spurious_misc);
	setup_irq(AR531X_GPIO_IRQ_NONE, &spurious_gpio);
#ifndef CONFIG_AR5315
	setup_irq(AR531X_MISC_IRQ_TIMER, &ar531x_timer_interrupt);
#endif
	setup_irq(AR531X_MISC_IRQ_AHB_PROC, &ar531x_ahb_proc_interrupt);
        setup_irq(AR531X_MISC_IRQ_GPIO, &cascade);

#ifdef CONFIG_KGDB
#if CONFIG_EARLY_STOP
        kgdbInit();
#endif
#endif
}
