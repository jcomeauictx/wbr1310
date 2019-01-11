/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Support for GPIO -- General Purpose Input/Output Pins
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include "ar531xlnx.h"

/* GPIO Interrupt Support */

/* Turn on the specified AR531X_GPIO_IRQ interrupt */
static unsigned int
ar531x_gpio_intr_startup(unsigned int irq)
{
	ar531x_gpio_intr_enable(irq);
	return 0;
}

/* Turn off the specified AR531X_GPIO_IRQ interrupt */
static void
ar531x_gpio_intr_shutdown(unsigned int irq)
{
	ar531x_gpio_intr_disable(irq);
}

u32 gpioIntMask = 0;

/* Enable the specified AR531X_GPIO_IRQ interrupt */
void
ar531x_gpio_intr_enable(unsigned int irq)
{
    u32 reg;
    int gpio;

#ifndef CONFIG_AR5315
    gpio = irq - AR531X_GPIO_IRQ_BASE;
    gpioIntMask |= gpio;

    reg = sysRegRead(AR531X_GPIO_CR);
    reg &= ~(GPIO_CR_M(gpio) | GPIO_CR_UART(gpio) | GPIO_CR_INT(gpio));
    reg |= GPIO_CR_I(gpio);
    reg |= GPIO_CR_INT(gpio);

    sysRegWrite(AR531X_GPIO_CR, reg);
    (void)sysRegRead(AR531X_GPIO_CR); /* flush to hardware */
#endif
}

/* Disable the specified AR531X_GPIO_IRQ interrupt */
void
ar531x_gpio_intr_disable(unsigned int irq)
{
    u32 reg;
    int gpio;

#ifndef CONFIG_AR5315
    gpio = irq - AR531X_GPIO_IRQ_BASE;
    reg = sysRegRead(AR531X_GPIO_CR);
    reg &= ~(GPIO_CR_M(gpio) | GPIO_CR_UART(gpio) | GPIO_CR_INT(gpio));
    reg |= GPIO_CR_I(gpio);
    /* No GPIO_CR_INT bit */

    sysRegWrite(AR531X_GPIO_CR, reg);
    (void)sysRegRead(AR531X_GPIO_CR); /* flush to hardware */

    gpioIntMask &= ~gpio;
#endif
}

static void
ar531x_gpio_intr_ack(unsigned int irq)
{
	ar531x_gpio_intr_disable(irq);
}

static void
ar531x_gpio_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar531x_gpio_intr_enable(irq);
}

static void
ar531x_gpio_intr_set_affinity(unsigned int irq, unsigned long mask)
{
	/* Only 1 CPU; ignore affinity request */
}

int ar531x_gpio_irq_base;

struct hw_interrupt_type ar531x_gpio_intr_controller = {
	"AR531X GPIO",
	ar531x_gpio_intr_startup,
	ar531x_gpio_intr_shutdown,
	ar531x_gpio_intr_enable,
	ar531x_gpio_intr_disable,
	ar531x_gpio_intr_ack,
	ar531x_gpio_intr_end,
	ar531x_gpio_intr_set_affinity,
};

void
ar531x_gpio_intr_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR531X_GPIO_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].handler = &ar531x_gpio_intr_controller;
	}

	ar531x_gpio_irq_base = irq_base;
}

/* ARGSUSED */
void
spurious_gpio_handler(int cpl, void *dev_id, struct pt_regs *regs)
{
    u32 gpioDataIn;
#if CONFIG_AR5315
    gpioDataIn = sysRegRead(AR5315_GPIO_DI) & gpioIntMask;
#else
    gpioDataIn = sysRegRead(AR531X_GPIO_DI) & gpioIntMask;
#endif

    printk("spurious_gpio_handler: 0x%x di=0x%8.8x gpioIntMask=0x%8.8x\n",
           cpl, gpioDataIn, gpioIntMask);
}

struct irqaction spurious_gpio =
	{spurious_gpio_handler, SA_INTERRUPT, 0, "spurious_gpio",
            NULL, NULL};

