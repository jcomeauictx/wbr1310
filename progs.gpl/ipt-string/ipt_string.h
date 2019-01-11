/* vi: set sw=4 ts=4:
 *
 * This function should be synchronous with kernel header file
 *	include/linux/netfilter_ipv4/ipt_string.h
 *
 */

#ifndef _IPT_STRING_H
#define _IPT_STRING_H

/* *** PERFORMANCE TWEAK ***
 * Packet size and search string threshold,
 * above which sublinear searches is used. */
#define IPT_STRING_HAYSTACK_THRESH	100
#define IPT_STRING_NEEDLE_THRESH	20

#define BM_MAX_NLEN 256
#define BM_MAX_HLEN 1024

typedef char *(*proc_ipt_search) (char *, char *, int, int);
//joel add for block url 
enum ipt_string_flags
{
	IPT_GEN_STRING,
	IPT_URL_STRING,
	IPT_DNS_STRING,
	IPT_HTTP_REQ
}; 

struct ipt_string_info
{
    char string[BM_MAX_NLEN];
    u_int16_t invert;
    u_int16_t len;
    u_int8_t flags;
};

#endif /* _IPT_STRING_H */
