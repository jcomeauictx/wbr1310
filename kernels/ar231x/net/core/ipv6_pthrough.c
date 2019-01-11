/*
 * PPPoE passthrough 
 * This module is used to pass through the PPPoE packets
 * 
 * Peter Wu 20050804
 */

#include <linux/netdevice.h>
#include <linux/proc_fs.h>

extern struct net_device *dev_get_by_name(const char *name);
extern void dev_set_promiscuity(struct net_device *dev, int inc);
extern int dev_queue_xmit(struct sk_buff *skb);

int ipv6_pt_enable = 0;

char ipv6_pt_landev[IFNAMSIZ];
char ipv6_pt_wandev[IFNAMSIZ];
#define ETH_TYPE_IPV6	0x86dd

//#define PTABLE_SIZE 	16
//static int pthrough_idx = 0;
//static unsigned char pthrough_table[PTABLE_SIZE][ETH_ALEN];

/* return 1, if we want to handle this packet, or
 * return 0, let other ones do this	 */
int ipv6_pthrough(struct sk_buff *skb)
{
	unsigned short proto;
	unsigned char *smac;
	unsigned char *dmac;

	/* check if ipv6 pass through enabled or not
	 * if not set yet, just return and do nothing */
	if (!ipv6_pt_enable) return 0;
	
	// check and forward packets
	proto = skb->mac.ethernet->h_proto;
	smac = skb->mac.ethernet->h_source;
	dmac = skb->mac.ethernet->h_dest;

	if (proto == ETH_TYPE_IPV6) {
		if (strcmp(skb->dev->name, ipv6_pt_landev) == 0) {
//			printk("PeTeR: OutGoing packet (%s)\n", skb->dev->name);
//			printk("PeTeR: skb->dev (%s->%s)\n", landev_name, wandev_name);
			skb->dev = dev_get_by_name(ipv6_pt_wandev);
			skb_push(skb, ETH_HLEN);
			dev_queue_xmit(skb);
			return 1;
		}
		if (strcmp(skb->dev->name, ipv6_pt_wandev) == 0) {
//			printk("PeTeR: PPPoE Incoming packet (%s)\n", skb->dev->name);
			skb->dev = dev_get_by_name(ipv6_pt_landev);
			skb_push(skb, ETH_HLEN);
			dev_queue_xmit(skb);
			return 1;
		}
	}

	return 0;
}

int proc_ipv6_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int ret=0;
	if (ipv6_pt_enable) {
		ret = sprintf(page, "%s,%s\n", ipv6_pt_landev, ipv6_pt_wandev);
	} else {
		ret = sprintf(page, "null,null\n");
	}

	return ret;
}

#define isCHAR(x) ((x >= 'a') && (x <= 'z')) ? 1:((x >= '0') && (x <= '9')) ? 1:((x >= 'A') && (x <= 'Z')) ? 1:0
int proc_ipv6_write(struct file *file, const char * buffer, unsigned long count, void *data)
{
	char *pt;
	struct net_device *dev;

	if (ipv6_pt_enable) {
		ipv6_pt_enable = 0;
		if ((dev = dev_get_by_name(ipv6_pt_landev)))
			dev_set_promiscuity(dev, -1);
		if ((dev = dev_get_by_name(ipv6_pt_wandev)))
			dev_set_promiscuity(dev, -1);
	}

	/* we expect that buffer contain format of "landev_name,wandev_name" */
	memset(ipv6_pt_landev, 0x0, sizeof (ipv6_pt_landev));
	for (pt=ipv6_pt_landev; *buffer && (*buffer != ','); buffer++) {
		if ((*buffer != ' ') && isCHAR(*buffer)) {
			*pt = *buffer;
			pt++;
		}
	}
	
	if (!(*buffer))	goto ppw_failed;
	
	memset(ipv6_pt_wandev, 0x0, sizeof (ipv6_pt_wandev));
	for (pt=ipv6_pt_wandev, buffer++; *buffer; buffer++) {
		if ((*buffer != ' ') && isCHAR(*buffer)) {
			*pt = *buffer;
			pt++;
		}
	}
	
	if (!(dev = dev_get_by_name(ipv6_pt_landev))) goto ppw_failed;
	else	dev_set_promiscuity(dev, 1);
	if (!(dev = dev_get_by_name(ipv6_pt_wandev))) goto ppw_failed;
	else	dev_set_promiscuity(dev, 1);
	
	ipv6_pt_enable = 1;
	printk("ipv6 pass through (%s<->%s)\n",ipv6_pt_landev, ipv6_pt_wandev);
	return count;
	
ppw_failed:
	ipv6_pt_enable = 0;
	memset(ipv6_pt_landev, 0x0, sizeof (ipv6_pt_landev));
	memset(ipv6_pt_wandev, 0x0, sizeof (ipv6_pt_wandev));
//	printk("failed\n");

	return count;
}

#if 0
#define PT_ROOT		"pthrough"
static struct proc_dir_entry * root;

int pthrough_create_proc_entry(void)
{
	static struct proc_dir_entry * pppoe;

	// create directory 
	root = proc_mkdir(PT_ROOT, NULL);
	if (root == NULL) {
		printk("proc_mkdir return NULL!\n");
		goto pt_out;
	}

	// create entries 
	pppoe = create_proc_entry("pppoe", 0644, root);
	if (pppoe == NULL) {
		printk("create_proc_entry (pppoe) return NULL!\n");
		goto pt_out;
	}
	pppoe->read_proc = proc_pppoe_read;
	pppoe->write_proc = proc_pppoe_write;
	
	return 0;
	
pt_out:
	printk("Unable to create %s !!\n", PT_ROOT);
	return -1;
}

void pthrough_rmove_proc_entry(void)
{
	remove_proc_entry("pppoe", root);
	remove_proc_entry(PT_ROOT, NULL);
	root = NULL;
}
#endif // if 0
