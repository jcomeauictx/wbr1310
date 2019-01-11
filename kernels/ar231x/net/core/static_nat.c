/* static_nat.c : Static NAT for virtual server with multi-to-multi port range mapping
 * Written by Edward Peng <edward_peng@alphanetworks.com>
 * 
 * Changes:
 * 1.0		2005/12/01	Created.
 * 1.1		2005/12/07	Handle packets replied from LAN to WAN.
 * 				Also fix a bug which mac addresses are not correct.
 * 1.2		2006/01/11	Fix incorrect source IP if output device is ppp.
 * 				Add debug messages. 
 */
/* Usuage:
 * 1. Add rules:
 * echo "add <input ifname> <port begin> <port end> <out ifname> <dest ip> <port begin> <port end>" > /proc/net/static_nat
 *
 * Example:
 * echo "add vlan2 6881 6889 br0 192.168.0.100 7001 7009" > /proc/net/static_nat
 *
 * 2. Flush rules:
 * echo "flush" > /proc/net/static_nat
 */

#include <asm/checksum.h>
#include <asm/atomic.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/slab.h>

#define PROC_NAME 		"net/static_nat"
struct nat_tbl_t {
	struct net_device *rx_dev;	/* pointer to input net device structure */
	u16 rx_portrange_begin;		/* begin of input port range */
	u16 rx_portrange_end;		/* end of input port range */
	struct net_device *tx_dev;	/* pointer to output net device structure */
	u32 tx_new_ip;			/* destination IP */
	u16 tx_portrange_begin;		/* begin of output port range */
	u16 tx_portrange_end;		/* end of output port range */
	int replied;			/* this field would be filled by 
					   find_matched_rules() to indicate this rule is 
					   for packets that relied from LAN */
	struct list_head link;		/* the double link list */
};
LIST_HEAD(_list_root);
static struct list_head *list_root = &_list_root;
static struct proc_dir_entry *entry;
static atomic_t _lock = ATOMIC_INIT(0), *lock = &_lock;
#define NAT_TBL_LOCK(p)		atomic_inc(p)
#define NAT_TBL_UNLOCK(p)	atomic_dec(p)
#define NAT_TBL_ISLOCK(p)	atomic_read(p)
#ifdef STATIC_NAT_DEBUG
static int debug = 0;
#define DPRINTK(...) do { if (debug) printk(__VA_ARGS__); } while(0)
#endif

static int add_rule (struct list_head *parent, struct nat_tbl_t *nat_tbl);
//static int del_rule (struct nat_tbl_t *tbl);
static void static_nat_remove_proc_entry (void);
static struct nat_tbl_t * find_matched_rule (struct sk_buff * skb);
/* fast check in ip_nat_core.c */
extern u_int16_t
ip_nat_cheat_check(u_int32_t oldvalinv, u_int32_t newval, u_int16_t oldcheck);

/* do_static_nat()	do NAT with customized static NAT rules.
 * Return value:
 * 	non-zero	indicate this packet was routed by me, OS should ignore it.
 * 	zero		put this packet back to OS to walk its original way.
 */
int do_static_nat (struct sk_buff *skb)
{
	struct nat_tbl_t 	*tbl;
	struct iphdr		*iph;
	struct tcphdr		*tcph;
	struct udphdr		*udph;
	struct in_device 	*in_dev;
	struct rtable		*rt;
	u32			old_ip, new_ip;
	u16			old_port, new_port;
	u16			*cksum;
	u16			check;

	tbl = find_matched_rule (skb);
	if (tbl == NULL) {
		return 0; /* kick this packet back to OS */
	}
#ifdef STATIC_NAT_DEBUG
	u16 sport = 0, dport = 0;
	iph = (struct iphdr *) skb->data;
	if (iph->protocol == IPPROTO_TCP) {
		tcph = (struct tcphdr *) 
			(((unsigned char *)iph) + (iph->ihl << 2));
		sport = tcph->source;
		dport = tcph->dest;
	} else if (iph->protocol == IPPROTO_UDP) {
		udph = (struct udphdr *) 
			(((unsigned char *)iph) + (iph->ihl<<2));
		sport = udph->source;
		dport = udph->dest;
	}

	DPRINTK("%d.%d.%d.%d:%d => %s:%d.%d.%d.%d:%d\n",
		(iph->saddr >> 24) & 0xff,
		(iph->saddr >> 16) & 0xff,
		(iph->saddr >> 8) & 0xff,
		iph->saddr & 0xff,
		sport,
		skb->dev->name, 
		(iph->daddr >> 24) & 0xff,
		(iph->daddr >> 16) & 0xff,
		(iph->daddr >> 8) & 0xff,
		iph->daddr & 0xff,
		dport
		);
#endif
	/* here skb->data start from IP header */
	iph = (struct iphdr *) skb->data;
	if (tbl->replied == 0) {
		/* new device interface */
		skb->dev = tbl->tx_dev;
		/* backup the old destination IP address and write new one */
		old_ip = iph->daddr;
		new_ip = tbl->tx_new_ip;
		iph->daddr = new_ip;
	} else {
		/* new device interface */
		skb->dev = tbl->rx_dev;
		in_dev = skb->dev->ip_ptr;
		/* backup the old source IP address and write new one */
		old_ip = iph->saddr;
		/* don't use indev->ifa_list->ifa_address here, it would be the 
		 * P-t-P address for ppp device */
		new_ip = in_dev->ifa_list->ifa_local;
		iph->saddr = new_ip;
	}
	iph->ttl--;
	/* re-calculate IP checksum */
	iph->check = 0;	/* clear this field before new checksum */
	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
	switch (iph->protocol) {
	case IPPROTO_TCP:
		tcph = (struct tcphdr *) 
			(((unsigned char *)iph) + (iph->ihl << 2));
		cksum = (u16*)&tcph->check;
		if ((u8*)(cksum + 1) > skb->tail)
			goto drop_this_packet;
		/* backup old port and write new one */
		if (tbl->replied == 0) {
			old_port = tcph->dest;
			new_port= tbl->tx_portrange_begin + 
				(tcph->dest - tbl->rx_portrange_begin);
			tcph->dest = new_port;
		} else {
			old_port = tcph->source;
			new_port = tbl->rx_portrange_begin + 
				(tcph->source - tbl->tx_portrange_begin);
			tcph->source = new_port;
		}
		/* RFC1071: C' = C + (m' - m) */
		check = *cksum;
		check = ip_nat_cheat_check (~old_ip, new_ip, check);
		check = ip_nat_cheat_check (~old_port, new_port, check);
		*cksum = check;
		break;
	case IPPROTO_UDP:
		udph = (struct udphdr *) 
			(((unsigned char *)iph) + (iph->ihl<<2));
		cksum = (u16*)&udph->check;
		if ((u8*)(cksum + 1) > skb->tail)
			goto drop_this_packet;
		/* backup old port and write new one */
		if (tbl->replied == 0) {
			old_port = udph->dest;
			new_port = tbl->tx_portrange_begin + 
				(udph->dest - tbl->rx_portrange_begin);
			udph->dest = new_port;
		} else {
			old_port = udph->source;
			new_port = tbl->rx_portrange_begin + 
				(udph->source - tbl->tx_portrange_begin);
			udph->source = new_port;
		}
		check = *cksum;
		/* checksum of UDP is optional. If original checksum is zero, keep it. */
		if (check  != 0) {
			/* RFC1071: C' = C + (m' - m) */
			check = ip_nat_cheat_check (~old_ip, new_ip, check);
			check = ip_nat_cheat_check (~old_port, new_port, check);
			*cksum = check ? : 0xffff;
		}
		break;
	};
	/* Here need to build the routing table first. ip_route_output will fill routing 
	 * information and return it via the pointer "rt". Later ip_finish_output would 
	 * fill correct destination mac address by refering to the mac inside it. 
	 * Note that source address should be zero if this packet is not from local. */
	ip_route_output(&rt, iph->daddr, 0, RT_TOS(iph->tos), 0);
	/* release the old routing table and chain the new one */
	dst_release(skb->dst);
	skb->dst = &rt->u.dst;
#ifdef STATIC_NAT_DEBUG
	DPRINTK("[MOD]%d.%d.%d.%d:%d => %s:%d.%d.%d.%d:%d\n",
		(iph->saddr >> 24) & 0xff,
		(iph->saddr >> 16) & 0xff,
		(iph->saddr >> 8) & 0xff,
		iph->saddr & 0xff,
		sport,
		skb->dev->name, 
		(iph->daddr >> 24) & 0xff,
		(iph->daddr >> 16) & 0xff,
		(iph->daddr >> 8) & 0xff,
		iph->daddr & 0xff,
		dport
		);
#endif
	/* fire it! */
	ip_finish_output(skb);
	return 1;
drop_this_packet:
	/* TODO:count this packet in drop statistics */
	kfree_skb (skb);
	return 1;
}

static struct nat_tbl_t * find_matched_rule (struct sk_buff * skb)
{
	struct nat_tbl_t 	*tbl = 0;
	struct iphdr		*iph;
	struct tcphdr		*tcph;
	struct udphdr		*udph;
	struct list_head 	*cur_list;

	if (!list_root || !skb || !skb->data) 
		goto not_found;
	if (NAT_TBL_ISLOCK(lock))
		goto not_found;
	iph = (struct iphdr *) skb->data;
	switch (iph->protocol) {
	case IPPROTO_TCP:
		tcph = (struct tcphdr *) 
			(((unsigned char *)iph) + (iph->ihl << 2));
		list_for_each (cur_list, list_root) {
			tbl = list_entry (cur_list, struct nat_tbl_t, link);
			if (skb->dev == tbl->rx_dev &&
			    tcph->dest >= tbl->rx_portrange_begin &&
			    tcph->dest <= tbl->rx_portrange_end) {
				tbl->replied = 0;
				return tbl;
			} else if (skb->dev == tbl->tx_dev &&
			    iph->saddr == tbl->tx_new_ip &&
			    tcph->source >= tbl->tx_portrange_begin &&
			    tcph->source <= tbl->tx_portrange_end) {
				tbl->replied =1 ;
				return tbl;
			}
		}
	case IPPROTO_UDP:
		udph = (struct udphdr *) 
			(((unsigned char *)iph) + (iph->ihl<<2));
		list_for_each (cur_list, list_root) {
			tbl = list_entry (cur_list, struct nat_tbl_t, link);
			if (skb->dev == tbl->rx_dev &&
			    udph->dest >= tbl->rx_portrange_begin &&
			    udph->dest <= tbl->rx_portrange_end) {
				tbl->replied = 0;
				return tbl;
			} else if (skb->dev == tbl->tx_dev &&
			    iph->saddr == tbl->tx_new_ip &&
			    udph->source >= tbl->tx_portrange_begin &&
			    udph->source <= tbl->tx_portrange_end) {
				tbl->replied = 1;
				return tbl;
			}
		}
	}
not_found:
	return NULL;
}

static int flush_rules (struct list_head *parent)
{
	struct nat_tbl_t *tbl;
	struct list_head *cur_list, *temp_list;
	if (!parent)
		goto exception;
	list_for_each_safe (cur_list, temp_list, parent) {
		tbl = list_entry (cur_list, struct nat_tbl_t, link);
		if (tbl) {
			list_del (cur_list);
			kfree (tbl);
		}
	}
	return 0;
exception:
	return 0;
}

static int add_rule (struct list_head *parent, struct nat_tbl_t *nat_tbl)
{
	int ret = 0;
	struct nat_tbl_t *tbl;
	
	tbl = (struct nat_tbl_t *) kmalloc (sizeof(struct nat_tbl_t), GFP_KERNEL);
	if (tbl == NULL) {
		ret = -1;
		goto exception;
	}
	memcpy (tbl, nat_tbl, sizeof(struct nat_tbl_t));
	list_add_tail (&tbl->link, parent);
	return 0;
exception:
	printk("%s:error, ret=%d\n", __func__, ret);
	return ret;
}
#if 0
static int del_rule (struct nat_tbl_t *tbl)
{
	int ret = 0;
	if (tbl == NULL) {
		ret = -1;
		goto exception;
	}
	list_del (&tbl->link);
	free(tbl);
	return 0;
exception:
	printk("%s:error, ret=%d\n", __func__, ret);
	return ret;
}
#endif

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
		/* valid string [a-z|A-Z|\.] */
		if (!state && 
		   ((buf[i] >= 'a' && buf[i] <= 'z') || 
		    (buf[i] >= 'A' && buf[i] <= 'Z') ||
		    (buf[i] >= '0' && buf[i] <= '9') ||
		     buf[i] == '.')) {
			state++;
		} 	
		/* end of string */
		if (state && (buf[i] == ' ' || buf[i] == 0x0a)) 
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

static int
string_to_number(const char *s, unsigned int min, unsigned int max,
		 unsigned int *ret)
{
	int errno;
	long number;
	char *end;

	/* Handle hex, octal, etc. */
	errno = 0;
	number = simple_strtol(s, &end, 0);
	if (*end == '\0' && end != s) {
		/* we parsed a number, let's see if we want this */
		if (errno != ERANGE && min <= number && number <= max) {
			*ret = number;
			return 0;
		}
	}
	return errno;
}

static struct in_addr *
dotted_to_addr(const char *dotted)
{
	static struct in_addr addr;
	unsigned char *addrp;
	char *p, *q;
	unsigned int onebyte;
	int i;
	char buf[20];

	/* copy dotted string, because we need to modify it */
	strncpy(buf, dotted, sizeof(buf) - 1);
	addrp = (unsigned char *) &(addr.s_addr);

	p = buf;
	for (i = 0; i < 3; i++) {
		if ((q = strchr(p, '.')) == NULL)
			return (struct in_addr *) NULL;

		*q = '\0';
		if (string_to_number(p, 0, 255, &onebyte) == -1)
			return (struct in_addr *) NULL;

		addrp[i] = (unsigned char) onebyte;
		p = q + 1;
	}

	/* we've checked 3 bytes, now we check the last one */
	if (string_to_number(p, 0, 255, &onebyte) == -1)
		return (struct in_addr *) NULL;

	addrp[3] = (unsigned char) onebyte;

	return &addr;
}
	
static int 
static_nat_read_proc (char *buf, char **start, off_t off, int len, int *eof, void *data)
{
	struct nat_tbl_t *tbl;
	struct list_head *cur_list;
	char *p = buf;

	if (!list_root) 
		goto exception;
	/* print out all rules */
	list_for_each (cur_list, list_root) {
		tbl = list_entry (cur_list, struct nat_tbl_t, link);
		if (tbl) {
			p += snprintf(p, len - (p - buf), 
				"%s:%d~%d => %s %d.%d.%d.%d:%d~%d\n",
				tbl->rx_dev->name, 
				tbl->rx_portrange_begin, 
				tbl->rx_portrange_end,
				tbl->tx_dev->name,
				(tbl->tx_new_ip >> 24) & 0xff,
				(tbl->tx_new_ip >> 16) & 0xff,
				(tbl->tx_new_ip >> 8) & 0xff,
				tbl->tx_new_ip & 0xff,
				tbl->tx_portrange_begin,
				tbl->tx_portrange_end);
		}
	}
	return p - buf;
exception:
	return 0;
}

/* sample format: 
 * ADD vlan2 6881 6889 br0 192.168.0.100 7001 7009 */
static int 
static_nat_write_proc (struct file *file, const char * buf, unsigned long count, void *data)
{
	int buf_idx = 0;
	struct nat_tbl_t nat_tbl;
	char cmd[8];
	char ifname[8];
	int rx_portrange_begin;
	int rx_portrange_end;
	char tx_ip_str[32];
	int tx_portrange_begin;
	int tx_portrange_end;
	struct in_addr *ipv4_addr;
	
	if (count < 1) {
		return -EINVAL;
	}
	NAT_TBL_LOCK(lock);
	buf_idx += get_str (&buf[buf_idx], count - buf_idx, cmd, sizeof(cmd));
	if (strncmp (cmd, "ADD", sizeof(cmd)) == 0 || 
	    strncmp (cmd, "add", sizeof(cmd)) == 0 ) {
		/* incoming interface name */
		buf_idx += get_str (&buf[buf_idx], count - buf_idx, ifname, sizeof(ifname));
		nat_tbl.rx_dev = dev_get_by_name (ifname);
		if (nat_tbl.rx_dev == NULL) 
			goto out;
		/* incoming port range (begin) */
		buf_idx += get_value (&buf[buf_idx], count - buf_idx, &rx_portrange_begin);
		nat_tbl.rx_portrange_begin = rx_portrange_begin;
		/* incoming port range (end) */
		buf_idx += get_value (&buf[buf_idx], count - buf_idx, &rx_portrange_end);
		nat_tbl.rx_portrange_end = rx_portrange_end;
		/* outgoing interface name */
		buf_idx += get_str (&buf[buf_idx], count - buf_idx, ifname, sizeof(ifname));
		nat_tbl.tx_dev = dev_get_by_name (ifname);
		if (nat_tbl.tx_dev == NULL) 
			goto out;
		/* dest IP (string with dot) */
		buf_idx += get_str (&buf[buf_idx], count - buf_idx, tx_ip_str, sizeof(tx_ip_str));
		ipv4_addr = dotted_to_addr (tx_ip_str);
		nat_tbl.tx_new_ip = ipv4_addr->s_addr;
		/* dest port range (begin) */
		buf_idx += get_value (&buf[buf_idx], count - buf_idx, &tx_portrange_begin);
		nat_tbl.tx_portrange_begin = tx_portrange_begin;
		/* dest port range (end) */
		buf_idx += get_value (&buf[buf_idx], count - buf_idx, &tx_portrange_end);
		nat_tbl.tx_portrange_end = tx_portrange_end;
		/* TODO: add restriction against invalid rules here */
		add_rule (list_root, &nat_tbl);
		printk("added rule: %s:%d~%d => %s %d.%d.%d.%d:%d~%d\n",
				nat_tbl.rx_dev->name, 
				nat_tbl.rx_portrange_begin, 
				nat_tbl.rx_portrange_end,
				nat_tbl.tx_dev->name,
				(nat_tbl.tx_new_ip >> 24) & 0xff,
				(nat_tbl.tx_new_ip >> 16) & 0xff,
				(nat_tbl.tx_new_ip >> 8) & 0xff,
				nat_tbl.tx_new_ip & 0xff,
				nat_tbl.tx_portrange_begin,
				nat_tbl.tx_portrange_end);
	} else if (strncmp (cmd, "FLUSH", sizeof(cmd)) == 0 ||
		   strncmp (cmd, "flush", sizeof(cmd)) == 0) {
		printk("static_nat: Flush all static_nat rules.\n");
		flush_rules (list_root);
	}
#ifdef STATIC_NAT_DEBUG
	else if (strncmp (cmd, "debug", sizeof(cmd)) == 0) {
		debug = 1;
		printk("static_nat: debug = %d\n", debug);
	} else if (strncmp (cmd, "nodebug", sizeof(cmd)) == 0) {
		debug = 0;
		printk("static_nat: debug = %d\n", debug);
	}
#endif
	/* "DEL" is still not implemented yet */	
out:	
	NAT_TBL_UNLOCK(lock);
	return count;
}

int static_nat_create_proc_entry (void)
{
	int ret;

	/* create entries */
	entry = create_proc_entry(PROC_NAME, 0644, 0);
	if (entry == NULL) {
		ret = -2;
		goto exception;
	}
	entry->read_proc = static_nat_read_proc;
	entry->write_proc = static_nat_write_proc;
	
	return 0;
	
exception:
	static_nat_remove_proc_entry();
	printk("Unable to create proc dir/entry ret=%d.\n", ret);
	return -1;
}

static void static_nat_remove_proc_entry (void)
{
	if (entry) {
		remove_proc_entry(PROC_NAME, entry);
		entry = NULL;
	}
	return;
}
