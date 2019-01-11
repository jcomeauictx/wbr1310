/* ae531xproc.c  proc filesystem for Atheros ae531x
 *
 * Copyright (c) 2004, by Alpha Networks Coporation.
 * All rights reserved.
 * Written by Edward Peng <edward_peng@alphanetworks.com>
 * Created Feb 23, 2004.
 *
 * v1.0		2004/09/30	- Created.
 * v1.1		2004/10/04	- Use phyRegRead() and phyRegWrite() instead of
 * 				  directly accessing Mii address register and 
 * 				  Mii data register.
 * v1.2		2004/10/06	- When parsing command string, use decimal 
 * 				  for phy_id and mii_addr instead of heximal. 
 */
#include <linux/proc_fs.h>
#include "ar531xlnx.h"
#include "ae531xreg.h"
#include "ae531xmac.h"

struct ae531x_proc_private {
	long	phy_base;
	int	mii_phy_id;
	int	mii_addr;
	int	mii_data;
};
struct proc_dir_entry *ae531x_proc_entry;
char ae531x_proc_name[] = "driver/ae531x";

static int get_str (const char *buf, int buf_len, char *str, int str_len);
static int get_value (const char *buf, int buf_len, int *num);
static int get_hex_value (const char *buf, int buf_len, int *num);

/* proc fs for user space */
static int 
ae531x_proc_read (char *buf, char **start, off_t offset,
				int len, int *eof, void *data)
{
	struct ae531x_proc_private *priv = (struct ae531x_proc_private*) data;
	char *p = buf;
	
	p += sprintf(p, "0x%04x\n", priv->mii_data);

	*eof = 1;
	return p - buf;
}

static int 
ae531x_proc_write (struct file *file, const char *buf,
		unsigned long count, void* data)
{
	struct ae531x_proc_private *priv = (struct ae531x_proc_private*) data;
	char cmd[20];
	int value;
	int buf_idx = 0;
	
	if (count < 1) {
		return -EINVAL;
	}
	/* Command string: WRITE or READ */
	buf_idx += get_str(&buf[buf_idx], count - buf_idx, cmd, sizeof(cmd));
	if (!strcmp(cmd, "WRITE")) {
		/* PHY id */
		buf_idx += get_value (&buf[buf_idx], count - buf_idx, &value);
		priv->mii_phy_id = value;
		/* MII address */
		buf_idx += get_value(&buf[buf_idx], count - buf_idx, &value);
		priv->mii_addr = value;
		/* Write MII Value */
		buf_idx += get_hex_value(&buf[buf_idx], count - buf_idx,&value);
		priv->mii_data = value;
/*		printk("WRITE 0x%04x to PHY#%d.%d\n", 
				priv->mii_data, 
				priv->mii_phy_id,
				priv->mii_addr);
*/
		phyRegWrite(priv->phy_base, priv->mii_phy_id, 
				priv->mii_addr, priv->mii_data);
		/* invalid the mii data */
		priv->mii_data = 0xffff;
	}
	if (!strcmp(cmd, "READ")) {
		/* PHY id */
		buf_idx += get_value(&buf[buf_idx], count - buf_idx, &value);
		priv->mii_phy_id = value;
		/* MII address */
		buf_idx += get_value(&buf[buf_idx], count - buf_idx, &value);
		priv->mii_addr = value;
		/* Read MII Value */
		priv->mii_data = phyRegRead(priv->phy_base, priv->mii_phy_id, 
					priv->mii_addr);
		/* Read twice, work around for IP-175C */
		priv->mii_data = phyRegRead(priv->phy_base, priv->mii_phy_id, 
					priv->mii_addr);
/*		printk("READ PHY#%d.%d = 0x%04x\n", 
				priv->mii_phy_id,
				priv->mii_addr,
				priv->mii_data); 
*/
	}
	return count;
}


static int get_str (const char *buf, int buf_len, char *str, int str_len)
{
	int i, j;
	int state = 0;

	for (i = 0, j = 0; i < buf_len; i++) {
		/* ignore space before string */
		if (!state && buf[i] == ' ') {
			j++;
			continue;
		}
		/* valid string [a-z|A-Z] */
		if (!state && 
		   ((buf[i] >= 'a' && buf[i] <= 'z') || 
		    (buf[i] >= 'A' && buf[i] <= 'Z')) ) {
			state++;
		}
		/* end of string */
		if (state && buf[i] == ' ') 
			break;
	}
	if (str_len >= i - j + 1) {
		memcpy (str, &buf[j], i - j);
		str[i - j] = '\0';
	}
	else {
		printk("get_str: input buffer is too small!\n");
	}
	return i;
}

static int get_value (const char *buf, int buf_len, int *num)
{
	int i, j;
	int state = 0;

	*num = 0;
	for (i = 0, j = 0; i < buf_len; i++) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			*num *= 10;
			*num += buf[i] - '0';
			state = 1;
		} else if (!state && buf[i] == ' ') {
			continue;
		} else if (state && buf[i] == ' ')
			break;
	}
	return i;
}

static int get_hex_value (const char *buf, int buf_len, int *num)
{
	int i, j;
	int state = 0;

	*num = 0;
	for (i = 0, j = 0; i < buf_len; i++) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			*num *= 16;
			*num += buf[i] - '0';
			state = 1;
		}
		else if (buf[i] >= 'a' && buf[i] <= 'f') {
			*num *= 16;
			*num += buf[i] + 10 - 'a';
			state = 1;
		} else if (!state && buf[i] == ' ') {
			continue;
		} else if (state && buf[i] == ' ')
			break;
	}
	return i;
}

int ae531x_proc_fs_init (long phy_base)
{
	static struct ae531x_proc_private private;
	memset (&private, 0, sizeof (struct ae531x_proc_private));
	private.phy_base = phy_base;

	ae531x_proc_entry = create_proc_entry (ae531x_proc_name, 0, 0);
	if (ae531x_proc_entry)
	{
		ae531x_proc_entry->read_proc = ae531x_proc_read;
		ae531x_proc_entry->write_proc = ae531x_proc_write;
		ae531x_proc_entry->data = (void*) &private;
	}
		
	return 0;
}

void ae531x_proc_exit (void)
{
	if (ae531x_proc_entry)
		remove_proc_entry (ae531x_proc_name, NULL);
}

