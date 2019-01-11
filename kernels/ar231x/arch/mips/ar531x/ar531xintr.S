/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

/*
 * Glue code to save registers and get us to the interrupt dispatcher
 */
	.text
	.set	noat
	.align	5
NESTED(ar531x_interrupt_receive, PT_SIZE, sp)
	SAVE_ALL
	CLI
	.set	at

	move	a0, sp
	jal	ar531x_irq_dispatch

	j	ret_from_irq

	END(ar531x_interrupt_receive)
