/* vi: set sw=4 ts=4: */
/*
 * MTD driver for the SPI Flash Memory support.
 *
 * $Id: //depot/sw/linuxsrc/src/kernels/mips-linux-2.4.25/drivers/mtd/devices/spiflash.c#2 $
 *
 *
 * Copyright (c) 2005-2006 Atheros Communications Inc.
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

/*===========================================================================
** !!!!  VERY IMPORTANT NOTICE !!!!  FLASH DATA STORED IN LITTLE ENDIAN FORMAT
**
** This module contains the Serial Flash access routines for the Atheros SOC.
** The Atheros SOC integrates a SPI flash controller that is used to access
** serial flash parts. The SPI flash controller executes in "Little Endian"
** mode. THEREFORE, all WRITES and READS from the MIPS CPU must be
** BYTESWAPPED! The SPI Flash controller hardware by default performs READ
** ONLY byteswapping when accessed via the SPI Flash Alias memory region
** (Physical Address 0x0800_0000 - 0x0fff_ffff). The data stored in the
** flash sectors is stored in "Little Endian" format.
**
** The spiflash_write() routine performs byteswapping on all write
** operations.
**===========================================================================*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/proc_fs.h>
#include <asm/delay.h>
#include <asm/io.h>
#include "spiflash.h"

/* debugging */
#if 0
#define SPIFLASH_DEBUG
#endif

#ifdef SPIFLASH_DEBUG
#define dprintk(x, args...)	printk(x, ##args)
#else
#define dprintk(x, args...)
#endif

#ifndef __BIG_ENDIAN
#error This driver currently only works with big endian CPU.
#endif

static char module_name[] = "spiflash";

#define MIN(a,b)        ((a) < (b) ? (a) : (b))
#define FALSE 	0
#define TRUE 	1

#define ROOTFS_NAME	"rootfs"

static __u32 spiflash_regread32(int reg);
static void spiflash_regwrite32(int reg, __u32 data);
static __u32 spiflash_sendcmd (int op);

int __init spiflash_init (void);
void __exit spiflash_exit (void);
static int spiflash_probe (void);
static int spiflash_erase (struct mtd_info *mtd,struct erase_info *instr);
static int spiflash_read (struct mtd_info *mtd, loff_t from,size_t len,size_t *retlen,u_char *buf);
static int spiflash_write (struct mtd_info *mtd,loff_t to,size_t len,size_t *retlen,const u_char *buf);

/* Flash configuration table */
struct flashconfig
{
	__u32 byte_cnt;
	__u32 sector_cnt;
	__u32 sector_size;
	__u32 cs_addrmask;
}
flashconfig_tbl[MAX_FLASH] =
{
	{ 0, 0, 0, 0},
	{ STM_1MB_BYTE_COUNT, STM_1MB_SECTOR_COUNT, STM_1MB_SECTOR_SIZE, 0x0},
	{ STM_2MB_BYTE_COUNT, STM_2MB_SECTOR_COUNT, STM_2MB_SECTOR_SIZE, 0x0},
	{ STM_4MB_BYTE_COUNT, STM_4MB_SECTOR_COUNT, STM_4MB_SECTOR_SIZE, 0x0}
};

/* Mapping of generic opcodes to STM serial flash opcodes */
struct opcodes
{
	__u16 code;
	__s8 tx_cnt;
	__s8 rx_cnt;
}
stm_opcodes[] =
{
	{STM_OP_WR_ENABLE,		1, 0},
	{STM_OP_WR_DISABLE,		1, 0},
	{STM_OP_RD_STATUS,		1, 1},
	{STM_OP_WR_STATUS,		1, 0},
	{STM_OP_RD_DATA,		4, 4},
	{STM_OP_FAST_RD_DATA,	1, 0},
	{STM_OP_PAGE_PGRM,		8, 0},
	{STM_OP_SECTOR_ERASE,	4, 0},
	{STM_OP_BULK_ERASE,		1, 0},
	{STM_OP_DEEP_PWRDOWN,	1, 0},
	{STM_OP_RD_SIG,			4, 1}
};

/* Driver private data structure */
struct spiflash_data
{
	struct mtd_info *		mtd;
	struct mtd_partition *	parsed_parts;		/* parsed partitions */
	void *					spiflash_readaddr;	/* memory mapped data for read  */
	void *					spiflash_mmraddr;	/* memory mapped register space */
};

static struct spiflash_data *spidata = 0;

#ifdef CONFIG_MTD_REDBOOT_PARTS
extern int parse_redboot_partitions(struct mtd_info *master, struct mtd_partition **pparts);
#endif

/*****************************************************************************/

#ifdef CONFIG_MTD_SPIFLASH_PP
/*
 * With AR2317, WRG-G19, we add the external circuit to implement page
 * programming. The GPIO 0 is used to control the chip select of the SPI
 * interface. The chip select is low active.
 *
 *								david_hsieh@alphanetworks.com
 */

/* The following part is cut from arch/mips/ar531x/ar531x.h */

#include <asm/addrspace.h>

#define AR5315_DSLBASE          0xB1000000      /* RESET CONTROL MMR */

/* GPIO */
#define AR5315_GPIO_DI          (AR5315_DSLBASE + 0x0088)
#define AR5315_GPIO_DO          (AR5315_DSLBASE + 0x0090)
#define AR5315_GPIO_CR          (AR5315_DSLBASE + 0x0098)
#define AR5315_GPIO_INT         (AR5315_DSLBASE + 0x00a0)

typedef unsigned int AR531X_REG;
#define sysRegRead(phys)		(*(volatile AR531X_REG *)KSEG1ADDR(phys))
#define sysRegWrite(phys, val)	((*(volatile AR531X_REG *)KSEG1ADDR(phys)) = (val))

static atomic_t spiflash_cs = ATOMIC_INIT(0);

static inline void chip_select(int value)
{
	__u32 reg;

	/* Set GPIO 0 as output. */
	reg = sysRegRead(AR5315_GPIO_CR);
	reg |= 0x1;
	sysRegWrite(AR5315_GPIO_CR, reg);

	/* Set GPIO 0 data. */
	reg = sysRegRead(AR5315_GPIO_DO);
	if (value) reg |= 0x1;
	else reg &= ~0x1;
	sysRegWrite(AR5315_GPIO_DO, reg);
}

#define SET_SPI_ACTIVITY()						\
{												\
	/*printk("SET_SPI @ %s, line %d\n",__FUNCTION__,__LINE__);*/		\
	while (atomic_read(&spiflash_cs))			\
	{											\
		printk("Woops, shit happens @ %s, line %d !!!\n",__FUNCTION__,__LINE__);	\
		udelay(1);								\
	}											\
	atomic_set(&spiflash_cs, 1);				\
}

#define CLEAR_SPI_ACTIVITY()					\
{												\
	/*printk("CLEAR_SPI @ %s, line %d\n",__FUNCTION__,__LINE__);*/		\
	chip_select(1);								\
	atomic_set(&spiflash_cs, 0);				\
}

#else

#define SET_SPI_ACTIVITY()
#define CLEAR_SPI_ACTIVITY()

#endif

/*****************************************************************************/

/***************************************************************************************************/

static __u32 spiflash_regread32(int reg)
{
	volatile __u32 * data = (__u32 *)(spidata->spiflash_mmraddr + reg);
	return (*data);
}

static void spiflash_regwrite32(int reg, __u32 data)
{
	volatile __u32 *addr = (__u32 *)(spidata->spiflash_mmraddr + reg);
	*addr = data;
	return;
}

static __u32 spiflash_sendcmd(int op)
{
	__u32 reg, mask;
	struct opcodes *ptr_opcode;

	ptr_opcode = &stm_opcodes[op];

	/* wait for CPU spiflash activity. */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();

	/* send the command */
	spiflash_regwrite32(SPI_FLASH_OPCODE, ptr_opcode->code);
	reg = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | ptr_opcode->tx_cnt | (ptr_opcode->rx_cnt << 4) | SPI_CTL_START;
	spiflash_regwrite32(SPI_FLASH_CTL, reg);
 
	/* wait for CPU spiflash activity */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	if (ptr_opcode->rx_cnt > 0)
	{
		reg = (__u32) spiflash_regread32(SPI_FLASH_DATA);

		switch (ptr_opcode->rx_cnt)
		{
		case 1:		mask = 0x000000ff;	break;
		case 2:		mask = 0x0000ffff;	break;
		case 3:		mask = 0x00ffffff;	break;
		default:	mask = 0xffffffff;	break;
		}
		reg &= mask;
	}
	else
	{
		reg = 0;
	}

	/* clean our activity */
	CLEAR_SPI_ACTIVITY();

	return reg;
}

/* Probe SPI flash device
 * Function returns 0 for failure.
 * and flashconfig_tbl array index for success.
 */
static int spiflash_probe(void)
{
	__u32 sig;
	int flash_size;

	/* Read the signature on the flash device */
	sig = spiflash_sendcmd(SPI_RD_SIG);
	switch (sig)
	{
	case STM_8MBIT_SIGNATURE:	flash_size = FLASH_1MB;	break;
	case STM_16MBIT_SIGNATURE:	flash_size = FLASH_2MB;	break;
	case STM_32MBIT_SIGNATURE:	flash_size = FLASH_4MB;	break;
	default:
		printk (KERN_WARNING "%s: Read of flash device signature failed!\n", module_name);
		return (0);
	}
	return (flash_size);
}

static int spiflash_erase(struct mtd_info *mtd,struct erase_info *instr)
{
	struct opcodes *ptr_opcode;
	__u32 temp, reg;
	int finished = FALSE;

	dprintk("%s(addr = 0x%.8x, len = %d)\n",__FUNCTION__,instr->addr,instr->len);

	/* sanity checks */
	if (instr->addr + instr->len > mtd->size) return (-EINVAL);

	/* We are going to do 'sector erase', do 'write enable' first. */
	ptr_opcode = &stm_opcodes[SPI_SECTOR_ERASE];
	spiflash_sendcmd(SPI_WRITE_ENABLE);

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();

	/* we are not really waiting for CPU spiflash activity, just need the value of the register. */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* send our command */
	temp = ((__u32)instr->addr << 8) | (__u32)(ptr_opcode->code);
	spiflash_regwrite32(SPI_FLASH_OPCODE, temp);
	reg = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | ptr_opcode->tx_cnt | SPI_CTL_START;
	spiflash_regwrite32(SPI_FLASH_CTL, reg);

	/* wait for CPU spiflash activity */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* clean our activity */
	CLEAR_SPI_ACTIVITY();

	/* wait for 'write in progress' to clear */
	do
	{
		udelay(1);
		reg = spiflash_sendcmd(SPI_RD_STATUS);
		if (!(reg & SPI_STATUS_WIP)) finished = TRUE;
	} while (!finished);

	instr->state = MTD_ERASE_DONE;
	if (instr->callback) instr->callback(instr);

	dprintk("%s return\n",__FUNCTION__);
	return (0);
}

static int spiflash_read(struct mtd_info *mtd, loff_t from,size_t len,size_t *retlen,u_char *buf)
{
	u_char	*read_addr;

#if 0
	dprintk("%s(from = 0x%.8x, len = %d)\n",__FUNCTION__,(__u32) from,(int)len);
#endif

   	/* sanity checks */
	if (!len) return (0);
	if (from + len > mtd->size) return (-EINVAL);

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();
	
	/* we always read len bytes */
	*retlen = len;

	read_addr = (u_char *)(spidata->spiflash_readaddr + from);
	memcpy(buf, read_addr, len);

	/* clean our activity */
	CLEAR_SPI_ACTIVITY();
	
	return 0;
}

static int spiflash_write(struct mtd_info *mtd,loff_t to,size_t len,size_t *retlen,const u_char *buf)
{
	int done = FALSE, page_offset, bytes_left, finished;
	__u32 xact_len, spi_data = 0, opcode, reg;

	dprintk("%s(to = 0x%.8x, len = %d)\n",__FUNCTION__,(__u32) to,len); 

	*retlen = 0;

	/* sanity checks */
	if (!len) return (0);
	if (to + len > mtd->size) return (-EINVAL);

	opcode = stm_opcodes[SPI_PAGE_PROGRAM].code;
	bytes_left = len;

	while (done == FALSE)
	{
		xact_len = MIN(bytes_left, sizeof(__u32));

		/*
		 * 32-bit writes cannot span across a page boundary
		 * (256 bytes). This types of writes require two page
		 * program operations to handle it correctly. The STM part
		 * will write the overflow data to the beginning of the
		 * current page as opposed to the subsequent page.
		 */
		page_offset = (to & (STM_PAGE_SIZE - 1)) + xact_len;

		if (page_offset > STM_PAGE_SIZE)
		{
			xact_len -= (page_offset - STM_PAGE_SIZE);
		}

		spiflash_sendcmd(SPI_WRITE_ENABLE);

		switch (xact_len)
		{
		case 1:		spi_data = (__u32)buf[0]; break;
		case 2:		spi_data = (buf[1] << 8) | buf[0];	break;
		case 3:		spi_data = (buf[2] << 16) | (buf[1] << 8) | buf[0]; break;
		case 4:		spi_data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0]; break;
		default:	printk("spiflash_write: default case\n");	break;
		}

		/* wait and mark our activity */
		SET_SPI_ACTIVITY();

		/* we are not really waiting for CPU spiflash activity, just need the value of the register. */
		do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

		/* send write command */
		spiflash_regwrite32(SPI_FLASH_DATA, spi_data);
		opcode = (opcode & SPI_OPCODE_MASK) | ((__u32)to << 8);
		spiflash_regwrite32(SPI_FLASH_OPCODE, opcode);
		reg = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | (xact_len + 4) | SPI_CTL_START;
		spiflash_regwrite32(SPI_FLASH_CTL, reg);
		finished = FALSE;

		/* wait CPU spi activity */
		do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

		/* clean our activity */
		CLEAR_SPI_ACTIVITY();
		
		do
		{
			udelay(1);
			reg = spiflash_sendcmd(SPI_RD_STATUS);
			if (!(reg & SPI_STATUS_WIP)) { finished = TRUE; }
		} while (!finished);

		bytes_left -= xact_len;
		to += xact_len;
		buf += xact_len;

		*retlen += xact_len;

		if (bytes_left == 0) { done = TRUE; }
	}

   	return 0;
}

/***************************************************************************************************/
#ifdef CONFIG_MTD_SPIFLASH_PP

static void page_write(loff_t to, const u_char * buf)
{
	__u32	reg, spi_data, opcode;
	int		i;

#if 1
	dprintk("%s(to = 0x%.8x, buf = 0x%.08x)\n",__FUNCTION__,(__u32)to,(__u32)buf);
#endif

	/* We are going to write flash now, do write enable first. */
	spiflash_sendcmd(SPI_WRITE_ENABLE);

	/* we are not really waiting for CPU spiflash activity, just need the value of the register. */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* Prepare SPI opcode, data and control register values. */
	opcode   = (stm_opcodes[SPI_PAGE_PROGRAM].code & SPI_OPCODE_MASK) | ((__u32)to << 8);
	spi_data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0]; buf += 4;
	reg      = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | 0x8 | SPI_CTL_START;

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();
	chip_select(0);

	/* Send out the the first 4 bytes. */
	spiflash_regwrite32(SPI_FLASH_DATA, spi_data);
	spiflash_regwrite32(SPI_FLASH_OPCODE, opcode);
	spiflash_regwrite32(SPI_FLASH_CTL, reg);

	/* 31 loops, each loop send 8 bytes */
	for (i=0; i<31; i++)
	{
		do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

		/*
		 * The sample code from the application node is:
		 *
		 *	spi_data = (UINT32)*((UINT32 *)buf);
		 *	spi_data = cpi2le32(spi_data);
		 *	spi_data_swapped =
		 *			(((spi_data>>8) & 0xff) << 24) |
		 *			(((spi_data>>24)& 0xff) << 8) |
		 *			(spi_data & 0x00ff00ff);
		 */
		opcode   = (buf[3] <<  8) | (buf[2] << 16) | (buf[1] << 24) | buf[0]; buf += 4;
		spi_data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] <<  8) | buf[0]; buf += 4;
		reg      = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | 0x8 | SPI_CTL_START;

		spiflash_regwrite32(SPI_FLASH_DATA, spi_data);
		spiflash_regwrite32(SPI_FLASH_OPCODE, opcode);
		spiflash_regwrite32(SPI_FLASH_CTL, reg);
	}

	/* send out the last 4 bytes */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	opcode   = (buf[3] <<  8) | (buf[2] << 16) | (buf[1] << 24) | buf[0]; buf += 4;
	reg      = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | 0x4 | SPI_CTL_START;

	spiflash_regwrite32(SPI_FLASH_OPCODE, opcode);
	spiflash_regwrite32(SPI_FLASH_CTL, reg);

	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* Deactive chip select */
	chip_select(1);
	/* clean our activity */
	CLEAR_SPI_ACTIVITY();

	for (;;)
	{
		udelay(1);
		reg = spiflash_sendcmd(SPI_RD_STATUS);
		if (!(reg & SPI_STATUS_WIP))
		{
#if 1
			dprintk("%s(to = 0x%.8x, buf = 0x%.08x) done!\n",__FUNCTION__,(__u32)to,(__u32)buf);
#endif
			break;
		}
	}

	return;
}

/* 
 * Do page programming test.
 * The 'block' should be erase already.
 * We try to use page programming mode to write flash,
 * and erase this block again before return.
 */
static int test_page_programming(struct mtd_info * mtd, loff_t block)
{
	unsigned char	buffer[256];
	unsigned char *	flash;
	struct opcodes *ptr_opcode;
	__u32			opcode, reg;
	int				i;

	dprintk("%s(0x%.8x)\n",__FUNCTION__,(__u32)block);

	/* write the flash with known pattern */
	for (i=0; i<256; i++) buffer[i] = (unsigned char)i;
	page_write(block, buffer);

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();
	
	/* read it back and check pattern */
	flash = (unsigned char *)(spidata->spiflash_readaddr + block);
	dprintk("%s(): checking @ 0x%.8x ...\n",__FUNCTION__,(__u32)flash);
	for (i = 0; i < 8; i++)
	{
		if (flash[i*4] != (unsigned char)(i*4))
		{
			dprintk("unexpected value @ %d: 0x%02x !!\n", i*4, flash[i*4]);
			break;
		}
	}

	/* clean our activity */
	CLEAR_SPI_ACTIVITY();
	udelay(10);
	
	/* erase this block before return */
	dprintk("%s(): erasing block 0x%.8x ...\n",__FUNCTION__,(__u32)block);

	/* we are going to erase sector, do write enable first */
	spiflash_sendcmd(SPI_WRITE_ENABLE);

	/* wait and mark our activity */
	SET_SPI_ACTIVITY();

	/* we are not really waiting for CPU spiflash activity, just need the value of the register. */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);

	/* send sector erase op. */
	ptr_opcode = &stm_opcodes[SPI_SECTOR_ERASE];
	opcode = ((__u32)ptr_opcode->code) | ((__u32)block << 8);
	spiflash_regwrite32(SPI_FLASH_OPCODE, opcode);
	reg = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | ptr_opcode->tx_cnt | SPI_CTL_START;
	spiflash_regwrite32(SPI_FLASH_CTL, reg);

	/* wait for CPU spiflash activity */
	do { reg = spiflash_regread32(SPI_FLASH_CTL); } while (reg & SPI_CTL_BUSY);
	/* clean our activity */
	CLEAR_SPI_ACTIVITY();
	udelay(10);

	for (;;)
	{
		udelay(1);
		reg = spiflash_sendcmd(SPI_RD_STATUS);
		if (!(reg & SPI_STATUS_WIP)) break;
	}
	
	printk("SPI flash write test done (%d)!, page programming is %s!\n", i, i<8 ? "disabled":"enabled");
	return (i<8 ? 1:0);
}

static int pp_mode = -1;
static int pp_enable = 0;

/* implementation for spiflash page programing. */
static int spiflash_page_write(struct mtd_info * mtd,
		loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
	size_t bytes_left = len;
	size_t xact_len;
	size_t written;
	size_t offset;

	dprintk("%s(to = 0x%.8x, len = %d)\n",__FUNCTION__,(__u32)to, len);

	/* If we already test page programming and failed,
	 * fall back to spiflash_write() directly. */
	if (pp_mode > 0) return spiflash_write(mtd, to, len, retlen, buf);

	*retlen = 0;
	if (to + len > mtd->size) return (-EINVAL);

	while (bytes_left > 0)
	{
		offset = to % STM_PAGE_SIZE;
		xact_len = MIN(bytes_left, STM_PAGE_SIZE - offset);
		if (offset > 0 || xact_len < STM_PAGE_SIZE)
		{
			spiflash_write(mtd, to, xact_len, &written, buf);
		}
		else
		{
			/* test page program mode, if we did not test it before. */
			if (pp_mode < 0) pp_mode = test_page_programming(mtd, to);

			if (pp_enable && (pp_mode == 0)) page_write(to, buf);
			else spiflash_write(mtd, to, xact_len, &written, buf);
		}
		to += xact_len;
		bytes_left -= xact_len;
		buf += xact_len;
		*retlen += xact_len;
	}

	return 0;
}

static int __my_atoi(const char * buf)
{
	int ret = 0;
	while (*buf)
	{
		if (*buf >= '0' && *buf <= '9') ret += (int)(*buf - '0');
		buf++;
	}
	return ret;
}

static int proc_read_pp_enable(char * buf, char ** start, off_t offset,
		int len, int * eof, void * data)
{
	char * p = buf;
	p += sprintf(p, "%d\n", pp_enable);
	*eof = 1;
	return p - buf;
}

static int proc_write_pp_enable(struct file * file, const char * buf,
		unsigned long count, void * data)
{
	pp_enable = __my_atoi(buf);
	printk("spiflash: %s page programming!\n", pp_enable ? "enable" : "disable");
	if (pp_mode >= 0) printk("spiflash: H/W is %scapable of doing page programming!\n", pp_mode ? "not " : "");
	return count;
}

static struct proc_dir_entry * root = NULL;
static struct proc_dir_entry * pp_enable_entry = NULL;

static int register_spi_proc(void)
{
	root = proc_mkdir("spiflash", NULL);
	if (root == NULL)
	{
		printk("spiflash: fail to create /proc/spiflash !!\n");
		return -1;
	}
	pp_enable_entry = create_proc_entry("pp_enable", 0644, root);
	if (pp_enable_entry == NULL)
	{
		printk("spiflash: fail to create /proc/spiflash/pp_enable !!\n");
		remove_proc_entry("spiflash", root);
		root = NULL;
		return -1;
	}
	pp_enable_entry->data = 0;
	pp_enable_entry->read_proc = proc_read_pp_enable;
	pp_enable_entry->write_proc = proc_write_pp_enable;
	pp_enable_entry->owner = THIS_MODULE;
	printk("spiflash: /proc/spiflash/pp_enable created !!\n");
	return 0;
}

static void remove_spi_proc(void)
{
	if (pp_enable_entry) remove_proc_entry("pp_enable", root);
	if (root) remove_proc_entry("spiflash", root);
	pp_enable_entry = NULL;
	root = NULL;
}

#endif
/***************************************************************************************************/


static int spiflash_setup(void)
{
#ifndef CONFIG_BLK_DEV_INITRD
#ifdef CONFIG_MTD_REDBOOT_PARTS
	struct mtd_info *mtd;
   	int result, i, num_parts;
	struct  mtd_partition  *mtd_parts;
#endif
#endif
	int index;
	struct mtd_info *mtd;

	if (spidata)
	{
		printk("MTD driver for SPI flash is already setup !\n");
		return 0;
	}

	spidata = kmalloc(sizeof(struct spiflash_data), GFP_KERNEL);
	if (!spidata) return (-ENXIO);

	spidata->spiflash_mmraddr = ioremap_nocache(SPI_FLASH_MMR, SPI_FLASH_MMR_SIZE);
	if (!spidata->spiflash_mmraddr)
	{
		printk (KERN_WARNING "%s: Failed to map flash device\n", module_name);
		kfree(spidata); spidata = 0;
		return (-ENXIO);
	}

	mtd = kmalloc(sizeof(struct mtd_info), GFP_KERNEL);
	if (!mtd)
	{
		kfree(spidata); spidata = 0;
		return (-ENXIO);
	}

	memset (mtd,0,sizeof (*mtd));

	printk ("MTD driver for SPI flash.\n");
	printk ("%s: Probing for Serial flash ...\n", module_name);
	if (!(index = spiflash_probe ()))
	{
		printk (KERN_WARNING "%s: Found no serial flash device\n", module_name);
		kfree(mtd); kfree(spidata); spidata = 0;
		return (-ENXIO);
	}
	printk ("%s: Found SPI serial Flash.\n", module_name);
	printk ("%d: size\n", flashconfig_tbl[index].byte_cnt);

	spidata->spiflash_readaddr = ioremap_nocache(SPI_FLASH_READ, flashconfig_tbl[index].byte_cnt);
	if (!spidata->spiflash_readaddr)
	{
		printk (KERN_WARNING "%s: Failed to map flash device\n", module_name);
		kfree(mtd); kfree(spidata); spidata = 0;
		return (-ENXIO);
	}

	mtd->name = module_name;
	mtd->type = MTD_NORFLASH;
	mtd->flags = (MTD_CAP_NORFLASH|MTD_WRITEABLE);
	mtd->size = flashconfig_tbl[index].byte_cnt;
	mtd->erasesize = flashconfig_tbl[index].sector_size;
	mtd->numeraseregions = 0;
	mtd->eraseregions = NULL;
	mtd->module = THIS_MODULE;
	mtd->erase = spiflash_erase;
	mtd->read = spiflash_read;
#ifdef CONFIG_MTD_SPIFLASH_PP
	mtd->write = spiflash_page_write;
#else
	mtd->write = spiflash_write;
#endif

#ifdef SPIFLASH_DEBUG
	dprintk("SPIFLASH:\n"
			"mtd->name = %s\n"
			"mtd->size = 0x%.8x (%uM)\n"
			"mtd->erasesize = 0x%.8x (%uK)\n"
			"mtd->numeraseregions = %d\n",
			mtd->name,
			mtd->size, mtd->size / (1024*1024),
			mtd->erasesize, mtd->erasesize / 1024,
			mtd->numeraseregions);

	if (mtd->numeraseregions)
	{
		int result;
		for (result = 0; result < mtd->numeraseregions; result++)
		{
			dprintk("SPIFLASH:\n\n"
					"mtd->eraseregions[%d].offset = 0x%.8x\n"
					"mtd->eraseregions[%d].erasesize = 0x%.8x (%uK)\n"
					"mtd->eraseregions[%d].numblocks = %d\n",
					result,mtd->eraseregions[result].offset,
					result,mtd->eraseregions[result].erasesize,mtd->eraseregions[result].erasesize / 1024,
					result,mtd->eraseregions[result].numblocks);
		}
	}
#endif

#ifndef CONFIG_BLK_DEV_INITRD
#ifdef CONFIG_MTD_REDBOOT_PARTS
	/* parse redboot partitions */
	num_parts = parse_redboot_partitions(mtd, &spidata->parsed_parts);

   	dprintk("SPIFLASH: Found %d redboot partitions\n", num_parts);

	if (num_parts)
	{
		result = add_mtd_partitions(mtd, spidata->parsed_parts, num_parts);
		/* Find root partition */
		mtd_parts = spidata->parsed_parts;
		for (i=0; i < num_parts; i++)
		{
			if (!strcmp(mtd_parts[i].name, ROOTFS_NAME))
			{
				/* Create root device */
				ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, i);
				break;
			}
		}
	}
	else
	{
		dprintk("SPIFLASH: Did not find any redboot partitions\n");
		kfree(mtd);
		kfree(spidata);
		spidata = NULL;
		return (-ENXIO);
	}
#endif
#endif
#ifdef CONFIG_MTD_SPIFLASH_PP
		register_spi_proc();
#endif

	spidata->mtd = mtd;
   	return 0;
}


struct mtd_info * spiflash_get_mtd_info(void)
{
   	printk("Get mtd_info of SPI flash (spidata=0x%x).\n", (__u32)spidata);
	if (!spidata) spiflash_setup();
	if (spidata) return spidata->mtd;
	return NULL;
}


int __init spiflash_init(void)
{
   	printk("MTD driver for SPI flash initializing (spidata=0x%x).\n", (__u32)spidata);
	if (!spidata) return spiflash_setup();
	return 0;
}

void __exit spiflash_exit(void)
{
	if (spidata && spidata->parsed_parts)
	{
		del_mtd_partitions (spidata->mtd);
		kfree(spidata->mtd);
		kfree(spidata);
	}
#ifdef CONFIG_MTD_SPIFLASH_PP
	remove_spi_proc();
#endif
}

module_init (spiflash_init);
module_exit (spiflash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Atheros Communications Inc");
MODULE_DESCRIPTION("MTD driver for SPI Flash on Atheros SOC");

