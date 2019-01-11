/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright MontaVista Software Inc
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Basic support for polled character input/output
 * using the AR531X's serial port.
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
#include <asm/gdb-stub.h>

#include "ar531xlnx.h"

#if CONFIG_EARLY_PRINTK_HACK || CONFIG_KGDB
/* base addr of uart and clock timing */
#if CONFIG_AR5315
#define         BASE                    AR5315_UART0 
#else
#define         BASE                    AR531X_UART0
#endif

/* distance in bytes between two serial registers */
#define         REG_OFFSET              4

/*
 * 0 - we need to do serial init
 * 1 - skip serial init
 */
static int serialPortInitialized = 0;

/*
 *  * the default baud rate *if* we do serial init
 *   */
#define         BAUD_DEFAULT            UART16550_BAUD_9600

/* === END OF CONFIG === */

#define         UART16550_BAUD_2400             2400
#define         UART16550_BAUD_4800             4800
#define         UART16550_BAUD_9600             9600
#define         UART16550_BAUD_19200            19200
#define         UART16550_BAUD_38400            38400
#define         UART16550_BAUD_57600            57600
#define         UART16550_BAUD_115200           115200

#define         UART16550_PARITY_NONE           0
#define         UART16550_PARITY_ODD            0x08
#define         UART16550_PARITY_EVEN           0x18
#define         UART16550_PARITY_MARK           0x28
#define         UART16550_PARITY_SPACE          0x38

#define         UART16550_DATA_5BIT             0x0
#define         UART16550_DATA_6BIT             0x1
#define         UART16550_DATA_7BIT             0x2
#define         UART16550_DATA_8BIT             0x3

#define         UART16550_STOP_1BIT             0x0
#define         UART16550_STOP_2BIT             0x4

/* register offset */
#define         OFS_RCV_BUFFER          (0*REG_OFFSET)
#define         OFS_TRANS_HOLD          (0*REG_OFFSET)
#define         OFS_SEND_BUFFER         (0*REG_OFFSET)
#define         OFS_INTR_ENABLE         (1*REG_OFFSET)
#define         OFS_INTR_ID             (2*REG_OFFSET)
#define         OFS_DATA_FORMAT         (3*REG_OFFSET)
#define         OFS_LINE_CONTROL        (3*REG_OFFSET)
#define         OFS_MODEM_CONTROL       (4*REG_OFFSET)
#define         OFS_RS232_OUTPUT        (4*REG_OFFSET)
#define         OFS_LINE_STATUS         (5*REG_OFFSET)
#define         OFS_MODEM_STATUS        (6*REG_OFFSET)
#define         OFS_RS232_INPUT         (6*REG_OFFSET)
#define         OFS_SCRATCH_PAD         (7*REG_OFFSET)

#define         OFS_DIVISOR_LSB         (0*REG_OFFSET)
#define         OFS_DIVISOR_MSB         (1*REG_OFFSET)


/* memory-mapped read/write of the port */
#define         UART16550_READ(y)    (*((volatile u8*)(BASE + y)))
#define         UART16550_WRITE(y, z)  ((*((volatile u8*)(BASE + y))) = z)

void
debugPortInit(u32 baud, u8 data, u8 parity, u8 stop)
{
	/* Pull UART out of reset */
#if CONFIG_AR5315
	sysRegWrite(AR5315_RESET,
		sysRegRead(AR5315_RESET) & ~(RESET_UART0));
#else
	sysRegWrite(AR531X_RESET,
		sysRegRead(AR531X_RESET) & ~(AR531X_RESET_UART0));
#endif

	/* disable interrupts */
        UART16550_WRITE(OFS_LINE_CONTROL, 0x0);
	UART16550_WRITE(OFS_INTR_ENABLE, 0);

	/* set up buad rate */
	{ 
		u32 divisor;
#if CONFIG_AR5315 
		u32 uart_clock_rate = ar531x_apb_frequency();
#else
		u32 uart_clock_rate = ar531x_cpu_frequency() / 4;
#endif
		u32 base_baud = uart_clock_rate / 16;
       
		/* set DIAB bit */
	        UART16550_WRITE(OFS_LINE_CONTROL, 0x80);
        
	        /* set divisor */
	        divisor = base_baud / baud;
        	UART16550_WRITE(OFS_DIVISOR_LSB, divisor & 0xff);
	        UART16550_WRITE(OFS_DIVISOR_MSB, (divisor & 0xff00)>>8);

        	/* clear DIAB bit */
	        UART16550_WRITE(OFS_LINE_CONTROL, 0x0);
	}

	/* set data format */
	UART16550_WRITE(OFS_DATA_FORMAT, data | parity | stop);
}

u8
getDebugChar(void)
{
        if (!serialPortInitialized) {
                serialPortInitialized = 1;
                debugPortInit(BAUD_DEFAULT,
                              UART16550_DATA_8BIT,
                              UART16550_PARITY_NONE, UART16550_STOP_1BIT);
        }

	while((UART16550_READ(OFS_LINE_STATUS) & 0x1) == 0);
	return UART16550_READ(OFS_RCV_BUFFER);
}

#if CONFIG_KGDB
/*
 * Peek at the most recently received character.
 * Don't wait for a new character to be received.
 */
u8
peekDebugChar(void)
{
	return UART16550_READ(OFS_RCV_BUFFER);
}

static int kgdbInitialized = 0;

void
kgdbInit(void)
{
#if CONFIG_AR5315
    sysRegWrite(AR5315_WDC, WDC_IGNORE_EXPIRATION);
#else
    sysRegWrite(AR531X_WD_CTRL, AR531X_WD_CTRL_IGNORE_EXPIRATION);
#endif

    if (!kgdbInitialized) {
        printk("Setting debug traps - please connect the remote debugger.\n");
        set_debug_traps();
        kgdbInitialized = 1;
    }
    breakpoint();
}

int
kgdbEnabled(void)
{
    return kgdbInitialized;
}

#define DEBUG_CHAR '\001';

int
kgdbInterrupt(void)
{
    if (!kgdbInitialized) {
        return 0;
    }

    /* 
     * Try to avoid swallowing too much input: Only consume
     * a character if nothing new has arrived.  Yes, there's
     * still a small hole here, and we may lose an input
     * character now and then.
     */
    if (UART16550_READ(OFS_LINE_STATUS) & 1) {
        return 0;
    } else {
        return UART16550_READ(OFS_RCV_BUFFER) == DEBUG_CHAR;
    }
}
#endif


void
putDebugChar(char byte)
{
        if (!serialPortInitialized) {
                serialPortInitialized = 1;
                debugPortInit(BAUD_DEFAULT,
                              UART16550_DATA_8BIT,
                              UART16550_PARITY_NONE, UART16550_STOP_1BIT);
        }

	while ((UART16550_READ(OFS_LINE_STATUS) &0x20) == 0);
	UART16550_WRITE(OFS_SEND_BUFFER, byte);
 }
#endif /* CONFIG_EARLY_PRINTK_HACK || CONFIG_KGDB */
