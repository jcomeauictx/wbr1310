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

#include <linux/module.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/tcp.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_helper.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_conntrack_sip.h>
#include <linux/netfilter_ipv4/ip_nat_sip.h>
#include <linux/proc_fs.h>

#if 0 
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

#define MAX_PORTS 8
static int ports[MAX_PORTS];
static int ports_c = 0;
extern struct sip_header_nfo ct_sip_hdrs[];
static unsigned int sip_nat_help(struct ip_conntrack *ct,
			 struct ip_conntrack_expect *exp,
			 struct ip_nat_info *info,
			 enum ip_conntrack_info ctinfo,
			 unsigned int hooknum,
			 struct sk_buff **pskb);
#if 0
static unsigned int sip_nat_callee_help(struct ip_conntrack *ct,
			 struct ip_conntrack_expect *exp,
			 struct ip_nat_info *info,
			 enum ip_conntrack_info ctinfo,
			 unsigned int hooknum,
			 struct sk_buff **pskb);
#endif
static unsigned int
sip_nat_expected(struct sk_buff **pskb,
		 unsigned int hooknum,
		 struct ip_conntrack *ct,
		 struct ip_nat_info *info);
#if 0
static struct ip_nat_helper sip_callee = 
{ 	{ NULL, NULL },
	"SIP_CALLEE",			/* name */
	IP_NAT_HELPER_F_ALWAYS,		/* flags */
	NULL,				/* module */
	{ { 0, { 0 } },			/* tuple */
	{ 0, { 0 }, IPPROTO_UDP } },
	{ { 0 ,{ 0xffff } },		/* mask */
	{ 0, { 0 }, 0xffff } },
	sip_nat_callee_help,		/* help */
	NULL //sip_nat_expected		/* expectfn */
};
#endif
/**
 * called whenever the first packet of a related connection arrives
 */
static unsigned int
sip_nat_expected(struct sk_buff **pskb,
		 unsigned int hooknum,
		 struct ip_conntrack *ct,
		 struct ip_nat_info *info)
{
	unsigned int ret;
	struct ip_nat_multi_range mr;
	struct ip_ct_sip_expect *exp_sip_info = &ct->master->help.exp_sip_info;
	struct ip_conntrack *master = master_ct(ct);
	struct iphdr *iph = (*pskb)->nh.iph;
	struct udphdr *udph = (void *)iph + iph->ihl * 4;
	const struct ip_conntrack_tuple *orig = 
			&master->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
	
	IP_NF_ASSERT(info);
	IP_NF_ASSERT(master);
	IP_NF_ASSERT(!(info->initialized & (1<<HOOK2MANIP(hooknum))));
	DEBUGP("sip_nat_expected: We have a connection!\n");
	DEBUGP("sip_nat_expected: %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n",
		NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip),
		ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.udp.port),
		NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip),
		ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port));
	mr.rangesize = 1;
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;

	if (HOOK2MANIP(hooknum) == IP_NAT_MANIP_SRC) {
		mr.range[0].min_ip = mr.range[0].max_ip = orig->dst.ip; 
		DEBUGP("orig: %u.%u.%u.%u:%u <-> %u.%u.%u.%u:%u "
			"newsrc: %u.%u.%u.%u\n",
                        NIPQUAD((*pskb)->nh.iph->saddr), ntohs(udph->source),
			NIPQUAD((*pskb)->nh.iph->daddr), ntohs(udph->dest),
			NIPQUAD(orig->dst.ip));
	} else {
		mr.range[0].min_ip = mr.range[0].max_ip = orig->src.ip;
		mr.range[0].min.udp.port = mr.range[0].max.udp.port = 
				htons(exp_sip_info->port);
		mr.range[0].flags |= IP_NAT_RANGE_PROTO_SPECIFIED;

		DEBUGP("orig: %u.%u.%u.%u:%u <-> %u.%u.%u.%u:%u "
			"newdst: %u.%u.%u.%u:%u\n",
                        NIPQUAD((*pskb)->nh.iph->saddr), ntohs(udph->source),
                        NIPQUAD((*pskb)->nh.iph->daddr), ntohs(udph->dest),
                        NIPQUAD(orig->src.ip), ntohs(udph->dest));
	}

	ret = ip_nat_setup_info(ct,&mr,hooknum);
//	info->helper = &sip_callee;
	return ret;
}

static unsigned int mangle_sip_packet(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct, 
			const char **dptr, size_t dlen,
			char *buffer, int bufflen,
			struct sip_header_nfo *hnfo)
{
	unsigned int matchlen, matchoff;
	
	if (ct_sip_get_info(*dptr, dlen, &matchoff, &matchlen, hnfo) <= 0)
		return 0;

	if (!ip_nat_mangle_udp_packet(pskb, ct, ctinfo, 
	    matchoff, matchlen, buffer, bufflen)) {
		return 0;
	}
	/* We need to reload this. Thanks Patrick. */
	*dptr = (*pskb)->data + (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr); 
	return 1;
}

static unsigned int sip_fixup(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			const char **dptr)
{
	char buffer[sizeof("nnn.nnn.nnn.nnn:nnnnn")];	
	unsigned int bufflen, dataoff;
	uint32_t ip;
	uint16_t port;
	
	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	
	if ((ctinfo) >= IP_CT_IS_REPLY) {
		ip = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
		port = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.udp.port;
	} else {
		ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
		port = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.udp.port;
	}
	bufflen = sprintf(buffer, "%u.%u.%u.%u:%u", NIPQUAD(ip), ntohs(port));
	DEBUGP("%s: Change to %s\n", __func__, buffer);	
	/* short packet ? */
	if (((*pskb)->len - dataoff) < (sizeof("SIP/2.0") - 1))
		return 0;
	
	/* Basic rules: requests and responses. */
	if (memcmp(*dptr, "SIP/2.0", sizeof("SIP/2.0") - 1) == 0) {
	
		if ((ctinfo) < IP_CT_IS_REPLY) {
			mangle_sip_packet(pskb, ctinfo, ct, dptr, 
				(*pskb)->len - dataoff, buffer, bufflen,
				&ct_sip_hdrs[POS_CONTACT]);
			return 1;
		}

		if (!mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff,
		    buffer, bufflen, &ct_sip_hdrs[POS_VIA])) {
			return 0;
		}
		
		/* This search should ignore case, but later.. */
		const char *aux = ct_sip_search("CSeq:", *dptr, sizeof("CSeq:") - 1, 
								(*pskb)->len - dataoff);
		if (!aux)
			return 0;
		
		if (!ct_sip_search("REGISTER", aux, sizeof("REGISTER"), 
		    ct_sip_lnlen(aux, *dptr + (*pskb)->len - dataoff))) {
			return 1;
		}
		return mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, 
						buffer, bufflen, &ct_sip_hdrs[POS_CONTACT]);
	}
	if ((ctinfo) < IP_CT_IS_REPLY) {
		if (!mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff,
		    buffer, bufflen, &ct_sip_hdrs[POS_VIA])) {
			return 0;
		}
		
		/* Mangle Contact if exists only. - watch udp_nat_mangle()! */
		mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, 
						buffer, bufflen, &ct_sip_hdrs[POS_CONTACT]);
		return 1;
	}
	/* This mangle requests headers. */
	return mangle_sip_packet(pskb, ctinfo, ct, dptr, 
			ct_sip_lnlen(*dptr, *dptr + (*pskb)->len - dataoff),
			buffer, bufflen, &ct_sip_hdrs[POS_REQ_HEADER]);
}

static int mangle_content_len(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			const char *dptr)
{
	unsigned int dataoff, matchoff, matchlen;
	char buffer[sizeof("65536")];
	int bufflen;
	
	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	
	/* Get actual SDP lenght */
	if (ct_sip_get_info(dptr, (*pskb)->len - dataoff, &matchoff, 
	    &matchlen, &ct_sip_hdrs[POS_SDP_HEADER]) > 0) {
		
		/* since ct_sip_get_info() give us a pointer passing 'v='
		   we need to add 2 bytes in this count. */
		int c_len = (*pskb)->len - dataoff - matchoff + 2;
		
		/* Now, update SDP lenght */
		if (ct_sip_get_info(dptr, (*pskb)->len - dataoff, &matchoff, 
		    &matchlen, &ct_sip_hdrs[POS_CONTENT]) > 0) {
		    
			bufflen = sprintf(buffer, "%u", c_len);
			
			return ip_nat_mangle_udp_packet(pskb, ct, ctinfo, matchoff,
							matchlen, buffer, bufflen);
		}
	}
	return 0;
}

static unsigned int mangle_sdp(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			uint32_t newip, uint16_t port,
			const char *dptr)
{
	char buffer[sizeof("nnn.nnn.nnn.nnn")];
	unsigned int dataoff, bufflen;

	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	
	/* Mangle owner and contact info. */
	bufflen = sprintf(buffer, "%u.%u.%u.%u", NIPQUAD(newip));
	if (!mangle_sip_packet(pskb, ctinfo, ct, &dptr, (*pskb)->len - dataoff,
	    buffer, bufflen, &ct_sip_hdrs[POS_OWNER])) {
		return 0;	
	}
	
	if (!mangle_sip_packet(pskb, ctinfo, ct, &dptr, (*pskb)->len - dataoff,
	    buffer, bufflen, &ct_sip_hdrs[POS_CONECTION])) {
		return 0;
	}
	
	/* Mangle media port. */
	bufflen = sprintf(buffer, "%u", port);
	if (!mangle_sip_packet(pskb, ctinfo, ct, &dptr, (*pskb)->len - dataoff,
	    buffer, bufflen, &ct_sip_hdrs[POS_MEDIA])) {
		return 0;
	}
	
	return mangle_content_len(pskb, ctinfo, ct, dptr);
}

/* So, this packet has hit the connection tracking matching code.
   Mangle it, and change the expectation to match the new version. */
static unsigned int sdp_fixup (struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack_expect *exp,
			const char *dptr)
{
	struct ip_conntrack *ct = exp->expectant;
	uint32_t newip;
	uint16_t port;
	struct ip_conntrack_tuple newtuple;
	struct ip_ct_sip_expect *exp_info = &exp->help.exp_sip_info;


	/* Connection will come from reply */
	newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
	//newtuple = ct->tuplehash[IP_CT_DIR_REPLY].tuple;
	memcpy(&newtuple, &ct->tuplehash[IP_CT_DIR_REPLY].tuple, sizeof(newtuple));
	
	DEBUGP("%s: exp_info->port = %u\n", __func__, exp_info->port);
	for (port = exp_info->port; port != 0; port++) {
		newtuple.dst.u.udp.port = htons(port);
		if (ip_conntrack_change_expect(exp, &newtuple) == 0)
			break;
	}
	if (port == 0) {
		DEBUGP("%s: change expect failed!\n", __func__);
		DUMP_TUPLE_UDP(&newtuple);
		DUMP_TUPLE_UDP(&exp->mask);
		return NF_DROP;
	}
	DEBUGP("%s:change expect to:\n", __func__);
	DUMP_TUPLE_UDP(&newtuple);
	DUMP_TUPLE_UDP(&exp->mask);
	if (!mangle_sdp(pskb, ctinfo, ct, newip, port, dptr)) {
		return NF_DROP;
	}

	return NF_ACCEPT;
}
#if 0
static unsigned int sip_nat_callee_help(struct ip_conntrack *ct,
			 struct ip_conntrack_expect *exp,
			 struct ip_nat_info *info,
			 enum ip_conntrack_info ctinfo,
			 unsigned int hooknum,
			 struct sk_buff **pskb)
{
	int dir = CTINFO2DIR(ctinfo);
	struct ip_conntrack_tuple repl;
	unsigned int dataoff, datalen;
	const char *dptr;
	int ret = NF_ACCEPT;
	int matchoff, matchlen;
	uint32_t ipaddr;
	uint16_t port;
	struct ip_conntrack *master = master_ct(ct);
	const struct ip_conntrack_tuple *orig = 
			&master->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
	const struct ip_conntrack_tuple *reply = 
			&master->tuplehash[IP_CT_DIR_REPLY].tuple;
	ct = master;
	DEBUGP("%s: %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", __func__,
		NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip),
		ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.udp.port),
		NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip),
		ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port));

	if (!((hooknum == NF_IP_POST_ROUTING && dir == IP_CT_DIR_ORIGINAL)
	      || (hooknum == NF_IP_PRE_ROUTING && dir == IP_CT_DIR_REPLY))) 
		return NF_ACCEPT;
	if (!exp)
		printk("%s: no exp!\n", __func__);

	/* No Data ? */ 
	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	if (dataoff >= (*pskb)->len) {
		DEBUGP("skb->len = %u\n", (*pskb)->len);
		return NF_ACCEPT;
        }

	if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
		dptr = (*pskb)->data + dataoff;
	else {
		DEBUGP("Copy of skbuff not supported yet.\n");
		goto out;
	}
	ctinfo = 3;	
	if (!sip_fixup(pskb, ctinfo, ct, &dptr)) {
		DEBUGP("sip_fixup failed\n");
		ret = NF_DROP;
		goto out;
	}
out:
	return ret;
}
#endif

static unsigned int sip_nat_help(struct ip_conntrack *ct,
			 struct ip_conntrack_expect *exp,
			 struct ip_nat_info *info,
			 enum ip_conntrack_info ctinfo,
			 unsigned int hooknum,
			 struct sk_buff **pskb)
{
	int dir = CTINFO2DIR(ctinfo);
	unsigned int dataoff, datalen;
	const char *dptr;
	int ret = NF_ACCEPT;

	if (!((hooknum == NF_IP_POST_ROUTING && dir == IP_CT_DIR_ORIGINAL)
	      || (hooknum == NF_IP_PRE_ROUTING && dir == IP_CT_DIR_REPLY))) 
		return NF_ACCEPT;

	/* No Data ? */ 
	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	if (dataoff >= (*pskb)->len) {
		DEBUGP("skb->len = %u\n", (*pskb)->len);
		return NF_ACCEPT;
        }

	if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
		dptr = (*pskb)->data + dataoff;
	else {
		DEBUGP("Copy of skbuff not supported yet.\n");
		goto out;
	}
	
	if (!sip_fixup(pskb, ctinfo, ct, &dptr)) {
		DEBUGP("sip_fixup failed\n");
		ret = NF_DROP;
		goto out;
	}

	if (!exp) {
		DEBUGP("%s: no exp!\n", __func__);
		goto out;
	}

	if ((ctinfo) >= IP_CT_IS_REPLY)
		goto out;

	/* After this point NAT, could have mangled skb, so 
	   we need to recalculate payload lenght. */
	datalen = (*pskb)->len - dataoff;

	if (datalen < (sizeof("SIP/2.0 200") - 1))
		goto out;
	if (!sdp_fixup(pskb, ctinfo, exp, dptr)) {
		return NF_DROP;
	}
out:
	return ret;
}

static struct ip_nat_helper sip[MAX_PORTS];

/* Not __exit: called from init() */
static void fini(void)
{
	int i;

	for (i = 0; i < ports_c; i++) {
		DEBUGP("ip_nat_sip: unregistering port %d\n", ports[i]);
		ip_nat_helper_unregister(&sip[i]);
	}
	
}

static int __init init(void)
{
	int i, ret = 0;
	
	if (ports[0] == 0)
		ports[0] = SIP_PORT;

	for (i = 0; (i < MAX_PORTS) && ports[i]; i++) {

		memset(&sip[i], 0, sizeof(struct ip_nat_helper));

		sip[i].tuple.dst.protonum = IPPROTO_UDP;
		sip[i].tuple.src.u.udp.port = htons(ports[i]);
		sip[i].mask.dst.protonum = 0xFFFF;
		sip[i].mask.src.u.udp.port = 0xFFFF;
		sip[i].help = sip_nat_help;
		sip[i].me = THIS_MODULE;
		sip[i].flags = IP_NAT_HELPER_F_ALWAYS;
		sip[i].expect = sip_nat_expected;

		DEBUGP("ip_nat_sip: Trying to register for port %d\n",
				ports[i]);
		ret = ip_nat_helper_register(&sip[i]);

		if (ret) {
			printk("ip_nat_sip: error registering "
			       "helper for port %d\n", ports[i]);
			fini();
			return ret;
		}
		ports_c++;
	}

	return ret;
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
