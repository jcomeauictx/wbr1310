/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

#include <linux/module.h>
#include "asm/atheros/ar531xbsp.h"

#ifdef CONFIG_KGDB
EXPORT_SYMBOL(kgdbInit);
EXPORT_SYMBOL(kgdbEnabled);
#endif
EXPORT_SYMBOL(ar531x_sys_frequency);
EXPORT_SYMBOL(get_system_type);
