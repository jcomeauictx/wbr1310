/* vi: set sw=4 ts=4:
 *
 * rgapps_physmap.c
 *
 * flash mapping for RGAPPS
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/config.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#include <linux/squashfs_fs.h>
#endif

#define WINDOW_ADDR	CONFIG_PHYSMAP_RGAPPS_START
#define WINDOW_SIZE	CONFIG_PHYSMAP_RGAPPS_LEN
#define BUSWIDTH	CONFIG_PHYSMAP_RGAPPS_BUSWIDTH
#define KERNEL_SKIP CONFIG_RGAPPS_KERNEL_SKIP

#ifdef CONFIG_MTD_BOOTCODE_SIZE
#define RGAPPS_BOOTCODE_SIZE	CONFIG_MTD_BOOTCODE_SIZE
#else
#define RGAPPS_BOOTCODE_SIZE	0x30000
#endif

#ifdef CONFIG_MTD_REDBOOT_WITHOUT_FIS
#define RGAPPS_NO_FIS	1
#else
#define RGAPPS_NO_FIS	0
#endif

static struct mtd_info *mymtd;

__u8 rgapps_physmap_read8(struct map_info *map, unsigned long ofs)
{
	return __raw_readb(map->map_priv_1 + ofs);
}

__u16 rgapps_physmap_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 rgapps_physmap_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void rgapps_physmap_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	memcpy_fromio(to, map->map_priv_1 + from, len);
}

void rgapps_physmap_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void rgapps_physmap_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
}

void rgapps_physmap_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
}

void rgapps_physmap_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy_toio(map->map_priv_1 + to, from, len);
}

struct map_info rgapps_physmap_map =
{
	name:		"RGAPPS physically mapped flash",
	size:		WINDOW_SIZE,
	buswidth:	BUSWIDTH,
	read8:		rgapps_physmap_read8,
	read16:		rgapps_physmap_read16,
	read32:		rgapps_physmap_read32,
	copy_from:	rgapps_physmap_copy_from,
	write8:		rgapps_physmap_write8,
	write16:	rgapps_physmap_write16,
	write32:	rgapps_physmap_write32,
	copy_to:	rgapps_physmap_copy_to
};

#ifdef CONFIG_MTD_PARTITIONS

#if RGAPPS_NO_FIS
#define SIZE_UPGRADE	(CONFIG_PHYSMAP_RGAPPS_LEN - RGAPPS_BOOTCODE_SIZE - 0x10000)
#else
#define SIZE_UPGRADE	(CONFIG_PHYSMAP_RGAPPS_LEN - RGAPPS_BOOTCODE_SIZE - 0x20000)
#endif

static struct mtd_partition physmap_partitions[] =
{
	{	name: "rootfs",			offset:	0x000000,
								size:	0x000000,
								mask_flags: MTD_WRITEABLE, },

	{	name: "upgrade",		offset:	RGAPPS_BOOTCODE_SIZE,
								size:	SIZE_UPGRADE, },

	{	name: "rgdb",			offset:	CONFIG_PHYSMAP_RGAPPS_LEN - 0x10000,
								size:	0x010000, },

	{	name: "RedBoot",		offset: 0x000000,
								size:	RGAPPS_BOOTCODE_SIZE, },

#if RGAPPS_NO_FIS == 0
	{   name: "FIS directory",	offset:	CONFIG_PHYSMAP_RGAPPS_LEN - 0x20000,
								size:	0x010000, },
#endif

	{   name: "Board/RadioCfg",	offset:	CONFIG_PHYSMAP_RGAPPS_LEN - 0x10000,
								size:	0x010000, },

	{	name: "flash",			offset: 0x000000,
								size:	CONFIG_PHYSMAP_RGAPPS_LEN,
								mask_flags: MTD_WRITEABLE, },

	{	name: NULL, },
};

#if 0
#if (CONFIG_PHYSMAP_RGAPPS_LEN == 0x400000)
static struct mtd_partition physmap_partitions[] =
{
	{	name: "rootfs",  		offset: 0x000000, size: 0x000000, mask_flags: MTD_WRITEABLE, },
	{	name: "upgrade",		offset: 0x030000, size: 0x390000, },
	{	name: "rgdb",			offset: 0x3f0000, size: 0x010000, },
	{	name: "RedBoot",		offset: 0x000000, size: 0x030000, },
	{   name: "FIS directory",	offset: 0x3e0000, size: 0x010000, },
	{   name: "Board/RadioCfg",	offset: 0x3f0000, size: 0x010000, },
	{	name: "flash",			offset: 0x000000, size: 0x400000, mask_flags: MTD_WRITEABLE, },
	{	name: NULL, },
};
#endif
#if (CONFIG_PHYSMAP_RGAPPS_LEN == 0x200000)
static struct mtd_partition physmap_partitions[] =
{
	{	name: "rootfs",  		offset: 0x000000, size: 0x000000, mask_flags: MTD_WRITEABLE, },
	{	name: "upgrade",		offset: 0x020000, size: 0x1c0000, },
	{	name: "rgdb",			offset: 0x1f0000, size: 0x010000, },
	{	name: "RedBoot",		offset: 0x000000, size: 0x030000, },
	{   name: "FIS directory",	offset: 0x1e0000, size: 0x010000, },
	{   name: "Board/RadioCfg",	offset: 0x1f0000, size: 0x010000, },
	{	name: "flash",			offset: 0x000000, size: 0x200000, mask_flags: MTD_WRITEABLE, },
	{	name: NULL, },
};
#endif
#endif

/* the tag is 32 bytes octet,
 * first part is the tag string,
 * and the second half is reserved for future used. */
#define PACKIMG_TAG	"--PaCkImGs--"
struct packtag
{
	char tag[16];
	unsigned long size;
	char reserved[12];
};

static struct mtd_partition * __init init_mtd_partitions(struct mtd_info * mtd, size_t size)
{
	struct squashfs_super_block *squashfsb;
	struct packtag *ptag = NULL;
	unsigned char buf[512];
	int off = physmap_partitions[1].offset;
	size_t len;

	squashfsb = (struct squashfs_super_block *) &(buf[32]);

	/* Skip kernel image, and look at every 64KB boundary */
	for (off += KERNEL_SKIP; off < size; off += (64 * 1024))
	{
		memset(buf, 0xe5, sizeof(buf));

		//printk("look for offset : 0x%x\n", off);

		/* Read block 0 to test for fs superblock */
		if (MTD_READ(mtd, off, sizeof(buf), &len, buf) || len != sizeof(buf))
		{
			continue;
		}

		//printk("%c%c%c%c%c%c%c%c ...\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
		//printk("%02x %02x %02x %02x %02x %02x %02x %02x  ...\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
		
		/* Try to find the tag of packimgs, the tag is always at 64K boundary. */
		if (memcmp(buf, PACKIMG_TAG, 12))
		{
			continue;
		}
		
		/* yes, we found it, check for supported file system */

		/* squashfs is at block zero */
		if (squashfsb->s_magic == SQUASHFS_MAGIC)
		{
			printk(KERN_NOTICE
				   "%s: squashfs filesystem found at block %d\n",
				   mtd->name, off/BLOCK_SIZE);
			ptag = (struct packtag *)buf;
			goto done;
		}
	}

	printk(KERN_NOTICE "%s: Couldn't find valid rootfs image!\n", mtd->name);
	
done:
	if (ptag)
	{
		physmap_partitions[0].offset = off + 32;
		physmap_partitions[0].size = ntohl(ptag->size);
	}
	else
	{
		physmap_partitions[0].offset = 0;
		physmap_partitions[0].size = 0;
	}

	return physmap_partitions;
}

#endif


#ifdef CONFIG_MTD_SPIFLASH
extern struct mtd_info * spiflash_get_mtd_info(void);
#endif

int __init init_rgapps_physmap(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition * parts = 0;
	int nparts, ret;
#endif
	
#ifndef CONFIG_MTD_SPIFLASH
	printk(KERN_NOTICE "RGAPPS physmap flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);
	rgapps_physmap_map.map_priv_1 = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);
	if (!rgapps_physmap_map.map_priv_1)
	{
		printk("Failed to ioremap\n");
		return -EIO;
	}
	mymtd = do_map_probe("cfi_probe", &rgapps_physmap_map);
	if (!mymtd)
	{
		printk("cfi_probe failed!");
		iounmap((void *)rgapps_physmap_map.map_priv_1);
		return -ENXIO;
	}
#else
	printk(KERN_NOTICE "RGAPPS (spiflash) physmap flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);
	rgapps_physmap_map.map_priv_1 = 0;
	mymtd = spiflash_get_mtd_info();
	if (!mymtd)
	{
		printk("no spiflash !!\n");
		return -ENXIO;
	}
#endif

	printk("Got mtd_info : 0x%08x ...\n", mymtd);
	mymtd->module = THIS_MODULE;
	add_mtd_device(mymtd);
	printk("add_mtd_device(0x%08x) done!!\n", mymtd);

#ifdef CONFIG_MTD_PARTITIONS
	parts = init_mtd_partitions(mymtd, WINDOW_SIZE);
	for (nparts = 0; parts[nparts].name; nparts++);
	ret = add_mtd_partitions(mymtd, parts, nparts);
	if (ret)
	{
		printk("add_mtd_partitions failed!\n");
		if (mymtd)
		{
			del_mtd_device(mymtd);
			map_destroy(mymtd);
		}
		if (rgapps_physmap_map.map_priv_1)
		{
			iounmap((void *)rgapps_physmap_map.map_priv_1);
			rgapps_physmap_map.map_priv_1 = 0;
		}
		return ret;
	}
#endif

	return 0;
}

static void __exit cleanup_rgapps_physmap(void)
{
	if (mymtd)
	{
		del_mtd_device(mymtd);
		map_destroy(mymtd);
	}
	if (rgapps_physmap_map.map_priv_1)
	{
		iounmap((void *)rgapps_physmap_map.map_priv_1);
		rgapps_physmap_map.map_priv_1 = 0;
	}
}

module_init(init_rgapps_physmap);
module_exit(cleanup_rgapps_physmap);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Hsieh <david_hsieh@alhpanetworks.com>");
MODULE_DESCRIPTION("RGAPPS physmap flash driver.");
