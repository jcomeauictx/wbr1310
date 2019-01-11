/* SIP extension for IP connection tracking.
 *
 * Ported to kernel 2.4 by Edward Peng <edward_peng@alphanetworks.com>
 * (C) 2005 by Christian Hentschel <chentschel@arnet.com.ar>
 * based on RR's ip_conntrack_ftp.c and other modules.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>

#include <linux/netfilter_ipv4/lockhelp.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_sip.h>

DECLARE_LOCK(sipbf_lock);
struct module *ip_conntrack_sip = THIS_MODULE;

#define MAX_PORTS 8
static int ports[MAX_PORTS];

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif
static struct ip_conntrack_helper sip[MAX_PORTS];

int digits_len(const char *dptr, const char *limit, int *shift);
int epaddr_len(const char *dptr, const char *limit, int *shift);
int skp_digits_len(const char *dptr, const char *limit, int *shift);
int skp_epaddr_len(const char *dptr, const char *limit, int *shift);
int ct_sip_get_info(const char *dptr, size_t dlen, 
		unsigned int *matchoff, 
		unsigned int *matchlen,
		struct sip_header_nfo *hnfo);
EXPORT_SYMBOL(ct_sip_get_info);

struct sip_header_nfo ct_sip_hdrs[] = {
	{ 	/* Via header */
		"Via:",		sizeof("Via:") - 1,
		"\r\nv:",	sizeof("\r\nv:") - 1, /* rfc3261 "\r\n" */
		"UDP ", 	sizeof("UDP ") - 1,
		epaddr_len
	},
	{ 	/* Contact header */
		"Contact:",	sizeof("Contact:") - 1,
		"\r\nm:",	sizeof("\r\nm:") - 1,
		"sip:",		sizeof("sip:") - 1,
		skp_epaddr_len
	},
	{ 	/* Content length header */
		"Content-Length:", sizeof("Content-Length:") - 1,
		"\r\nl:",	sizeof("\r\nl:") - 1,
		":",		sizeof(":") - 1, 
		skp_digits_len
	},
	{	/* SDP media info */
		"\nm=",		sizeof("\nm=") - 1,	
		"\rm=",		sizeof("\rm=") - 1,
		"audio ",	sizeof("audio ") - 1,
		digits_len
	},
	{ 	/* SDP owner address*/	
		"\no=",		sizeof("\no=") - 1, 
		"\ro=",		sizeof("\ro=") - 1,
		"IN IP4 ",	sizeof("IN IP4 ") - 1,
		epaddr_len
	},
	{ 	/* SDP connection info */
		"\nc=",		sizeof("\nc=") - 1, 
		"\rc=",		sizeof("\rc=") - 1,
		"IN IP4 ",	sizeof("IN IP4 ") - 1,
		epaddr_len
	},
	{ 	/* Requests headers */
		"sip:",		sizeof("sip:") - 1,
		"sip:",		sizeof("sip:") - 1, /* yes, i know.. ;) */
		"@", 		sizeof("@") - 1, 
		epaddr_len
	},
	{ 	/* SDP version header */
		"\nv=",		sizeof("\nv=") - 1,
		"\rv=",		sizeof("\rv=") - 1,
		"=", 		sizeof("=") - 1, 
		digits_len
	}
};
EXPORT_SYMBOL(ct_sip_hdrs);


int digits_len(const char *dptr, const char *limit, int *shift)
{
	int len = 0;	
	while (dptr <= limit && isdigit(*dptr)) {
		dptr++;
		len++;
	}
	return len;
} 

/* get digits lenght, skiping blank spaces. */
int skp_digits_len(const char *dptr, const char *limit, int *shift)
{
	for (; dptr <= limit && *dptr == ' '; dptr++)
		(*shift)++;
		
	return digits_len(dptr, limit, shift);
}

/* Simple ipaddr parser.. */
int parse_ipaddr(const char *cp,	const char **endp, 
			uint32_t *ipaddr, const char *limit)
{
	unsigned long int val;
	int i, digit = 0;
	
	for (i = 0, *ipaddr = 0; cp <= limit && i < 4; i++) {
		digit = 0;
		if (!isdigit(*cp))
			break;
		
		val = simple_strtoul(cp, (char **)&cp, 10);
		if (val > 0xFF)
			return -1;
	
		((uint8_t *)ipaddr)[i] = val;	
		digit = 1;
	
		if (*cp != '.')
			break;
		cp++;
	}
	if (!digit)
		return -1;
	
	if (endp)
		*endp = cp;

	return 0;
}

/* skip ip address. returns it lenght. */
int epaddr_len(const char *dptr, const char *limit, int *shift)
{
	const char *aux = dptr;
	uint32_t ip;
	
	if (parse_ipaddr(dptr, &dptr, &ip, limit) < 0) {
		DEBUGP("ip: %s parse failed.!\n", dptr);
		return 0;
	}

	/* Port number */
	if (*dptr == ':') {
		dptr++;
		dptr += digits_len(dptr, limit, shift);
	}
	return dptr - aux;
}

/* get address lenght, skiping user info. */
int skp_epaddr_len(const char *dptr, const char *limit, int *shift)
{
	for (; dptr <= limit && *dptr != '@'; dptr++)
		(*shift)++;	
	
	if (*dptr == '@') {
		dptr++;
		(*shift)++;
		return epaddr_len(dptr, limit, shift);
	}	
	return  0;
}

/* Returns 0 if not found, -1 error parsing. */
int ct_sip_get_info(const char *dptr, size_t dlen, 
		unsigned int *matchoff, 
		unsigned int *matchlen,
		struct sip_header_nfo *hnfo)
{
	const char *limit, *aux, *k = dptr;
	int shift = 0;
	
	limit = dptr + (dlen - hnfo->lnlen);

	while (dptr <= limit) {
		if ((strncmp(dptr, hnfo->lname, hnfo->lnlen) != 0) &&
			(strncmp(dptr, hnfo->sname, hnfo->snlen) != 0))
		{
			dptr++;
			continue;
		}
		aux = ct_sip_search(hnfo->ln_str, dptr, hnfo->ln_strlen, 
						ct_sip_lnlen(dptr, limit));
		if (!aux) {
			DEBUGP("'%s' not found in '%s'.\n", hnfo->ln_str, hnfo->lname);
			return -1;
		}
		aux += hnfo->ln_strlen;
		
		*matchlen = hnfo->match_len(aux, limit, &shift);
		if (!*matchlen)
			return -1;

		*matchoff = (aux - k) + shift; 
		
		DEBUGP("%s match succeeded! - len: %u\n", hnfo->lname, *matchlen);
		return 1;
	}
	DEBUGP("%s header not found.\n", hnfo->lname);
	return 0;
}


static int set_expected ( struct ip_conntrack *ct,
			uint32_t ipaddr, uint16_t port)
{
	struct ip_conntrack_expect exp;
	int ret;
#if 1	
	exp.tuple = ((struct ip_conntrack_tuple)
		{ { ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip, { 0 } },
		  { ipaddr, { .udp = { htons(port) } }, IPPROTO_UDP }});
#else
	memset(&exp, 0, sizeof(exp));
	exp.tuple = ct->tuplehash[IP_CT_DIR_REPLY].tuple;
#endif
	exp.mask = ((struct ip_conntrack_tuple) 
		{ { 0xFFFFFFFF, { 0 } },
		  { 0xFFFFFFFF, { .udp = { 0xFFFF } }, 0xFF }});
	
	exp.expectfn = NULL;
	exp.help.exp_sip_info.port = port;
	// ip_conntrack_expect_related failed is ok!
	ip_conntrack_expect_related(ct, &exp);
#if 0
	if (ip_conntrack_expect_related(ct, &exp) != 0) {
		printk("%s:ip_conntrack_expect_related() failed!\n", __func__);
		ret = NF_DROP;
	} else {
		ret = NF_ACCEPT;
	}
#endif
	DEBUGP("%s: expect related:\n", __func__);
	DUMP_TUPLE_UDP(&exp.tuple);
	DUMP_TUPLE_UDP(&exp.mask);

	return ret;
}

static int sip_help(const struct iphdr *iph, size_t len,
		struct ip_conntrack *ct,
		enum ip_conntrack_info ctinfo)
{
	unsigned int dataoff, datalen;
	const char *dptr;
	const char *dptr2;
	int ret = NF_ACCEPT;
	int matchoff, matchlen;
	uint32_t ipaddr;
	uint16_t port;
	
	dataoff = iph->ihl*4 + sizeof(struct udphdr);
	dptr =  (void*)iph + dataoff;
	datalen = len - dataoff;
        
	spin_lock_bh(&sipbf_lock);
	
	if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, 
	    &ct_sip_hdrs[POS_VIA]) > 0) {
		/* We'll drop only if there are parse problems. */
		if (parse_ipaddr(dptr + matchoff, &dptr2, &ipaddr, 
		    dptr + datalen) < 0) {
			ret = NF_DROP;
			goto out;
		}
		port = simple_strtoul(dptr2+1, NULL, 10);
		DEBUGP("via ip: %u.%u.%u.%u  port: %d\n", NIPQUAD(ipaddr), port);
		if (port < 1024) {
			ret = NF_DROP;
			goto out;
		}
//		ret = set_expected(ct, ipaddr, port);
	}
#if 1
	/* Only parse original direction for SDP */	
	if ((ctinfo) >= IP_CT_IS_REPLY)
		goto out;
	
	/* RTP info only in some SDP pkts */
	if (memcmp(dptr, "INVITE", sizeof("INVITE") - 1) != 0 && 
	    memcmp(dptr, "SIP/2.0 200", sizeof("SIP/2.0 200") - 1) != 0) {
		DEBUGP("%s: \"INVITE\" or \"SIP/2.0 200 not found!\"\n", __func__);
		goto out;
	}
	/* Get ip and port address from SDP packet. */
	if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, 
	    &ct_sip_hdrs[POS_CONECTION]) > 0) {

		/* We'll drop only if there are parse problems. */
		if (parse_ipaddr(dptr + matchoff, NULL, &ipaddr, 
		    dptr + datalen) < 0) {
			DEBUGP("%s:parse_ipaddr failed!\n", __func__);
			ret = NF_DROP;
			goto out;
		}
		if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, 
		    &ct_sip_hdrs[POS_MEDIA]) > 0) {

			port = simple_strtoul(dptr + matchoff, NULL, 10);
			if (port < 1024) {
			DEBUGP("%s:port < 1024!\n", __func__);
				ret = NF_DROP;
				goto out;
			}
			ret = set_expected(ct, ipaddr, port);
		}
	}
#endif
out:	spin_unlock_bh(&sipbf_lock);
	return ret;
}


/* Not __exit: called from init() */
static void fini(void)
{
	int i;
	for (i = 0; i < MAX_PORTS && ports[i]; i++) {
		DEBUGP("ip_ct_sip: unregistering helper for port %d\n",
				ports[i]);
		ip_conntrack_helper_unregister(&sip[i]);
	}
}

static int __init init(void)
{
	int i, ret;
	if (ports[0] == 0)
		ports[0] = SIP_PORT;

	for (i = 0; (i < MAX_PORTS) && ports[i]; i++) {
		memset(&sip[i], 0, sizeof(struct ip_conntrack_helper));
		sip[i].tuple.src.u.udp.port = htons(ports[i]);
		sip[i].tuple.dst.protonum = IPPROTO_UDP;
		sip[i].mask.src.u.udp.port = 0xFFFF;
		sip[i].mask.dst.protonum = 0xFFFF;
		sip[i].max_expected = 8;	
		sip[i].timeout = 60; 
		sip[i].flags = IP_CT_HELPER_F_REUSE_EXPECT;
		sip[i].me = ip_conntrack_sip;
		sip[i].help = sip_help;

		DEBUGP("ip_ct_sip: registering helper for port %d\n", 
				ports[i]);
		ret = ip_conntrack_helper_register(&sip[i]);

		if (ret) {
			fini();
			return ret;
		}
	}
	return 0;
}


MODULE_LICENSE("GPL");
module_init(init);
module_exit(fini);
