/* Kernel module to match a string into a packet.
 *
 * Copyright (C) 2000 Emmanuel Roger  <winfield@freegates.be>
 * 
 * ChangeLog
 *	19.02.2002: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed SMP re-entrancy problem using per-cpu data areas
 *		for the skip/shift tables.
 *	02.05.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed kernel panic, due to overrunning boyer moore string
 *		tables. Also slightly tweaked heuristic for deciding what
 * 		search algo to use.
 * 	27.01.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 * 		Implemented Boyer Moore Sublinear search algorithm
 * 		alongside the existing linear search based on memcmp().
 * 		Also a quick check to decide which method to use on a per
 * 		packet basis.
 */

#include <linux/smp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/file.h>
#include <net/sock.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include "ipt_string.h"

MODULE_LICENSE("GPL");

#if 0
#define IPT_DEBUG 1
#endif
#ifdef IPT_DEBUG
#define dprintk(x, args...)	printk(x, ##args)
#else
#define dprintk(x, args...)
#endif

struct string_per_cpu {
	int *skip;
	int *shift;
	int *len;
};

struct string_per_cpu *bm_string_data=NULL;

/* Boyer Moore Sublinear string search - VERY FAST */
char *search_sublinear (char *needle, char *haystack, int needle_len, int haystack_len) 
{
	int M1, right_end, sk, sh;  
	int ended, j, i;

	int *skip, *shift, *len;
	
	/* use data suitable for this CPU */
	shift=bm_string_data[smp_processor_id()].shift;
	skip=bm_string_data[smp_processor_id()].skip;
	len=bm_string_data[smp_processor_id()].len;
	
	/* Setup skip/shift tables */
	M1 = right_end = needle_len-1;
	for (i = 0; i < BM_MAX_HLEN; i++) skip[i] = needle_len;  
	for (i = 0; needle[i]; i++) skip[(int)needle[i]] = M1 - i;  

	for (i = 1; i < needle_len; i++) {   
		for (j = 0; j < needle_len && needle[M1 - j] == needle[M1 - i - j]; j++);  
		len[i] = j;  
	}  

	shift[0] = 1;  
	for (i = 1; i < needle_len; i++) shift[i] = needle_len;  
	for (i = M1; i > 0; i--) shift[len[i]] = i;  
	ended = 0;  
	
	for (i = 0; i < needle_len; i++) {  
		if (len[i] == M1 - i) ended = i;  
		if (ended) shift[i] = ended;  
	}  

	/* Do the search*/  
	while (right_end < haystack_len)
	{
		for (i = 0; i < needle_len && haystack[right_end - i] == needle[M1 - i]; i++);  
		if (i == needle_len) {
			return haystack+(right_end - M1);
		}
		
		sk = skip[(int)haystack[right_end - i]];  
		sh = shift[i];
		right_end = max(right_end - i + sk, right_end + sh);  
	}

	return NULL;
}  

/* Linear string search based on memcmp() */
char *search_linear (char *needle, char *haystack, int needle_len, int haystack_len) 
{
	char *k = haystack + (haystack_len-needle_len);
	char *t = haystack;
	
	while ( t <= k ) {
		if (memcmp(t, needle, needle_len) == 0)
			return t;
		t++;
	}

	return NULL;
}
#if 0
///////////////////////////////////////////////////////////
//+++ joel
////////////////////////////////////////////////////////
#define http_start_patern "Get "
#define http_start_patern_len 4
#define http_end_patern "\r\nAccept:"
#define http_end_patern_len 9 
#define TCP_HEADER_LENTH 20
static int match_HTTP(char **haystack,int *hlen)
{
    char *pend;
    char *pstart=(*haystack)+TCP_HEADER_LENTH;//ponit to the http head
    int datalen=*hlen-TCP_HEADER_LENTH;
    
    if(memcmp(pstart,http_start_patern,http_start_patern_len)!=0)//check if "Get " /www.cc.xxxx
    return 0;
    //find url end
    if((pend=search_linear(http_end_patern,*haystack,http_end_patern_len,datalen))==NULL)
    return 0;
    
    if(pstart>pend)
    return 0;
    
    *hlen=pend-pstart;
    *haystack=pstart;
    return 1;
}

#endif
static char * get_one_line(char *string, int len, char * buffer, int bufflen)
{
	int i;

	for (i=0; i<len && i<bufflen; i++)
	{
		if (string[i] == '\n' || string[i] == '\r')	break;
		else						buffer[i] = string[i];
	}
	buffer[i]='\0';
	while (i < len && (string[i]=='\r' || string[i]=='\n')) i++;
	return &string[i];
}
static int match_http_method(char *haystack, int hlen)
{
	if(strstr(haystack, "get")!=0 || strstr(haystack, "post")!=0 || strstr(haystack, "head")!=0
		|| strstr(haystack,"put")!=0 || strstr(haystack,"delete")!=0 || strstr(haystack, "options")!=0
		|| strstr(haystack,"trace")!=0 ||strstr(haystack,"connect")!=0)
	{
		dprintk("match_http_method match!\n");
		return 1;
	}
	return 0;
}
static int match_HTTP_REQ(char **haystack, int hlen)
{
    	char *pend=(*haystack);
    	char *pstart=(*haystack);//ponit to the http head
	char line[256];
	int line_len		=256;
	int data_len		= hlen;
	int data_line_len;
	int i;

	dprintk("match HTTP REQ -------->\n");
	dprintk("data_len=%d\n\n", data_len);
	if(data_len>16) //16 characters: "GET / HTTP/1.1\r\n"
	{
		memset(line,0,line_len);
		pend = get_one_line(pend, data_len, line, line_len);
		data_line_len = pend - pstart;
		data_len -= data_line_len;
		pstart = pend;
		dprintk("data_line_len=%d; data_len=%d\n", data_line_len,data_len);

		if(data_line_len > 12)	//12 characters: "GET HTTP/1.1"
		{
			for(i=0;i<line_len;i++)	{ if(line[i]>='A' && line[i]<='Z')line[i]+=32;	}

			return match_http_method(line, line_len);
			/*if(match_http_method(line, line_len))
			{
				dprintk("found!\n");
				dprintk("match HTTP REQ <--------\n");
				return 1;
			}
			*/
		}
	}
	dprintk("match HTTP REQ <--------\n");
	return 0;
}
static int match_URL(char **haystack, int hlen, char *needle, int nlen)
{
    	char *pend=(*haystack);
    	char *pstart=(*haystack);//ponit to the http head
	char *p_url_start;
	char url[512];
	char buff[256];
	char line[256];
	int url_len		=512;
	int buff_len		=256;
	int line_len		= 256;
	int data_len		= hlen;
	int data_line_len;
	int data_url_len	=0;
	int data_buff_len	=0;
	int i;

	dprintk("match URL -------->\n");
	if(data_len>16) //16 characters: "GET / HTTP/1.1\r\n"
	{
		dprintk("data_len=%d\n\n", data_len);
		memset(line,0,line_len);
		pend = get_one_line(pend, data_len, line, line_len);
		data_line_len = pend - pstart;
		data_len -= data_line_len;
		pstart = pend;

		if(data_line_len > 12)	//12 characters: "GET HTTP/1.1"
		{
			//dprintk("line before=%s\n", line);
			for(i=0;i<line_len;i++)	{ if(line[i]>='A' && line[i]<='Z')line[i]+=32;	}
			dprintk("line after =%s\n", line);
			if(match_http_method(line, line_len))
			{
				p_url_start=strstr(line," /");
				if(p_url_start!=0) p_url_start++;
				else return 0;
				
				memset(buff,0,buff_len);
				memset(url,0,url_len);
				i=0;
				while((p_url_start[i]!=' ') && (p_url_start[i]!='\0') && i<(buff_len-1))
				{
					buff[i]=p_url_start[i];
					i++;
				}
				buff[i]='\0';
				data_buff_len=i;

				dprintk("uri buff=%s, len=%d\n", buff, data_buff_len);

				if(strstr(buff, "http://")!=0)	// proxy case
				{
					for(i=0;i<data_buff_len;i++)
					{
						url[i]=buff[7+i];
					}
					url[i]='\0';
					data_url_len=i;
					dprintk("proxy case url=%s\n", url);
				}
				else
				{
					dprintk("data_len=%d\n", data_len);
					while(data_len>6)
					{
						memset(line,0,line_len);
						pend = get_one_line(pend, data_len, line, line_len);
						data_line_len = pend - pstart;
						data_len -= data_line_len;
						pstart = pend;
						
						for(i=0;i<line_len;i++)	{ if(line[i]>='A' && line[i]<='Z')line[i]+=32;	}

						if(strstr(line, "host: ")!=0)
						{
							dprintk("line =%s\n", line);
							p_url_start=strstr(line, "host: ");
							if(p_url_start!=0)	p_url_start+=6;
							else return 0;

							i=0;
							while((p_url_start[i]!=' ') && (p_url_start[i]!='\0'))
							{
								url[i]=p_url_start[i];
								i++;
							}
							data_url_len=i;
							for(i=0;i<data_buff_len;i++)
							{
								url[data_url_len+i]=buff[i];
							}
							url[data_url_len+i]='\0';
							data_url_len+=i-1;
							break;
						}
					}
				}
				if(data_url_len >= nlen)
				{
					for(i=0; i< nlen; i++){if(needle[i]>='A' && needle[i]<='Z') needle[i]+=32;}
					dprintk("normal case url=%s, len=%d\n", url, data_url_len);
					dprintk("normal case needle=%s, len=%d\n", needle, nlen);
					if(strstr(url, needle)!=0)
					{
						dprintk("found!\n");
						dprintk("match URL <--------\n");
						return 1;
					}
				}
			}
		}
	}
	dprintk("match URL <--------\n");
	return 0;
}
///////////////////////////////////////////////////
//---joel
static int
match(const struct sk_buff *skb,
      const struct net_device *in,
      const struct net_device *out,
      const void *matchinfo,
      int offset,
      const void *hdr,
      u_int16_t datalen,
      int *hotdrop)
{
	const struct ipt_string_info *info = matchinfo;
	struct iphdr *ip = skb->nh.iph;
	struct tcphdr *tcp=(struct tcphdr *)((char*)ip+(ip->ihl*4));
	int hlen, nlen;
	char *needle, *haystack;
	proc_ipt_search search=search_linear;

#if 0
	unsigned int j;
	unsigned char * data;

	data=(unsigned char *)ip;
	dprintk("ip hdr=");
	for(j=0;j<20;j++) dprintk("%02x ", data[j]);
	dprintk("\n");
	for(;j<40;j++) dprintk("%02x ", data[j]);
	dprintk("\n");
	
	dprintk("nfo->flags=%d; IPT_URL_STRING=%d\n",info->flags,IPT_URL_STRING);
	dprintk("tot_len=%d, ip->ihl=%d, doff=%d; hlen=%d\n",ntohs(ip->tot_len),
			ip->ihl,tcp->doff,(ntohs(ip->tot_len)-(ip->ihl*4)-(tcp->doff*4)));
#endif

	if ( !ip ) return 0;

	/* get lenghts, and validate them */
	nlen=info->len;
	hlen=ntohs(ip->tot_len)-(ip->ihl*4);
	if ( nlen > hlen ) return 0;

	needle=(char *)&info->string;
	haystack=(char *)ip+(ip->ihl*4);	
//+++ joel
	if(info->flags==IPT_URL_STRING)
	{
		hlen=ntohs(ip->tot_len)-(ip->ihl*4)-(tcp->doff*4);
		if(hlen>0)
		{
			dprintk("URL: tot_len=%d, ip_ihl=%d, doff=%d; hlen=%d\n",
				ntohs(ip->tot_len), (ip->ihl*4),(tcp->doff*4),hlen);
			haystack=(char *)tcp+(tcp->doff*4);
			return match_URL(&haystack,hlen, needle, nlen);
		}
		else		return 0;
	}
	else if(info->flags==IPT_HTTP_REQ)
	{
		hlen=ntohs(ip->tot_len)-(ip->ihl*4)-(tcp->doff*4);
		if(hlen>0)
		{
			dprintk("HTTP_REQ: tot_len=%d, ip_ihl=%d, doff=%d; hlen=%d\n",
				ntohs(ip->tot_len), (ip->ihl*4),(tcp->doff*4),hlen);
			haystack=(char *)tcp+(tcp->doff*4);
			return match_HTTP_REQ(&haystack,hlen);
		}
		else		return 0;
	}
	else if(info->flags==IPT_DNS_STRING)
	{

	}
//---joel
	/* The sublinear search comes in to its own
	 * on the larger packets */
	if ( (hlen>IPT_STRING_HAYSTACK_THRESH) &&
	  	(nlen>IPT_STRING_NEEDLE_THRESH) ) {
		if ( hlen < BM_MAX_HLEN ) {
			search=search_sublinear;
		}else{
			if (net_ratelimit())
				printk(KERN_INFO "ipt_string: Packet too big "
					"to attempt sublinear string search "
					"(%d bytes)\n", hlen );
		}
	}
	
    return ((search(needle, haystack, nlen, hlen)!=NULL) ^ info->invert);
}

static int
checkentry(const char *tablename,
           const struct ipt_ip *ip,
           void *matchinfo,
           unsigned int matchsize,
           unsigned int hook_mask)
{

       if (matchsize != IPT_ALIGN(sizeof(struct ipt_string_info)))
               return 0;

       return 1;
}

void string_freeup_data(void)
{
	int c;
	
	if ( bm_string_data ) {
		for(c=0; c<smp_num_cpus; c++) {
			if ( bm_string_data[c].shift ) kfree(bm_string_data[c].shift);
			if ( bm_string_data[c].skip ) kfree(bm_string_data[c].skip);
			if ( bm_string_data[c].len ) kfree(bm_string_data[c].len);
		}
		kfree(bm_string_data);
	}
}

static struct ipt_match string_match
= { { NULL, NULL }, "string", &match, &checkentry, NULL, THIS_MODULE };

static int __init init(void)
{
	int c;
	size_t tlen;
	size_t alen;

	tlen=sizeof(struct string_per_cpu)*smp_num_cpus;
	alen=sizeof(int)*BM_MAX_HLEN;
	
	/* allocate array of structures */
	if ( !(bm_string_data=kmalloc(tlen,GFP_KERNEL)) ) {
		return 0;
	}
	
	memset(bm_string_data, 0, tlen);
	
	/* allocate our skip/shift tables */
	for(c=0; c<smp_num_cpus; c++) {
		if ( !(bm_string_data[c].shift=kmalloc(alen, GFP_KERNEL)) )
			goto alloc_fail;
		if ( !(bm_string_data[c].skip=kmalloc(alen, GFP_KERNEL)) )
			goto alloc_fail;
		if ( !(bm_string_data[c].len=kmalloc(alen, GFP_KERNEL)) )
			goto alloc_fail;
	}
	
	return ipt_register_match(&string_match);

alloc_fail:
	string_freeup_data();
	return 0;
}

static void __exit fini(void)
{
	ipt_unregister_match(&string_match);
	string_freeup_data();
}

module_init(init);
module_exit(fini);
