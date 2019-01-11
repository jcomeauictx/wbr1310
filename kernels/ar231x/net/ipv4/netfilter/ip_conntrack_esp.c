/* ESP connection tracking. */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/ctype.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/lockhelp.h>

DECLARE_RWLOCK(ip_esp_lock);
#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_esp_lock)
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ip_esp_lock)

#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_esp.h>
#include <linux/netfilter_ipv4/listhelp.h>

LIST_HEAD(esp_db_list);

inline int
esp_find_ospi_cmp_fn(const struct ip_ct_esp_db *i,
		    u_int32_t spi)
{
	return i->ospi == spi;
}

inline int
esp_find_outgoing_cmp_fn(const struct ip_ct_esp_db *i,
		    u_int32_t src, u_int32_t dst)
{
	return i->src== src
		&& i->dst == dst;
}

inline int
esp_find_outgoing_spi_cmp_fn(const struct ip_ct_esp_db *i,
		    u_int32_t src, u_int32_t dst,u_int32_t spi)
{
	return i->src== src && 
		i->dst == dst &&
		i->ospi == spi;
}

inline int
esp_find_incoming_seen_cmp_fn(const struct ip_ct_esp_db *i,
		    u_int32_t src ,u_int32_t spi)
{
	return  i->dst == src &&
		i->rspi == spi;
}

inline int
esp_find_incoming_noseen_cmp_fn(const struct ip_ct_esp_db *i,
		    u_int32_t src )
{
	return  i->dst == src &&
		i->state == 0;
}

void ip_conntrack_esp_destroy_db(struct ip_conntrack *ct)
{
	struct ip_ct_esp_db  *esp_db_entry;

	WRITE_LOCK(&ip_esp_lock);
	esp_db_entry = LIST_FIND(&esp_db_list, esp_find_outgoing_spi_cmp_fn,
		        struct ip_ct_esp_db *,  (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip),
		        (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip),
		        ntohl(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.esp.spi));

	if  (esp_db_entry)
	{	
		LIST_DELETE(&esp_db_list, &esp_db_entry->list);

		esp_db_entry = LIST_FIND(&esp_db_list, esp_find_outgoing_spi_cmp_fn,
			        struct ip_ct_esp_db *,  (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip),
			        (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip),
			        ntohl(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.esp.spi));
	}

	WRITE_UNLOCK(&ip_esp_lock);

}

void ip_conntrack_find_esp_db_get(u_int32_t src, u_int32_t dst,u_int32_t spi,u_int32_t *ospi,u_int32_t *rspi,u_int16_t *esp_direction, u_int16_t *esp_state,u_int16_t *esp_find)
{
	struct ip_ct_esp_db  *esp_db_entry;

	WRITE_LOCK(&ip_esp_lock);
	esp_db_entry = LIST_FIND(&esp_db_list, esp_find_outgoing_cmp_fn,
		        struct ip_ct_esp_db *, src,dst);

	if  (esp_db_entry)
	{	//outgoing
		*esp_find=1;
		*esp_direction = 0;

		if (esp_db_entry->ospi == spi)
			*esp_state=1;
		else
			*esp_state=2;

		*ospi=*rspi=esp_db_entry->ospi;
	}
	else	
	{
		esp_db_entry = LIST_FIND(&esp_db_list, esp_find_incoming_seen_cmp_fn,
			        struct ip_ct_esp_db *, src,spi);

		if ( esp_db_entry)
		{	//have been reply
			*esp_find=1;
			*esp_direction = 1;
			*esp_state = 1;
			esp_db_entry->state=1;
			*ospi=*rspi=esp_db_entry->ospi;
		}
		else
		{
			esp_db_entry = LIST_FIND(&esp_db_list, esp_find_incoming_noseen_cmp_fn,
				        struct ip_ct_esp_db *, src);

			if (! esp_db_entry)
			{//new
				*esp_find=0;
			}
			else
			{//incoming first
				*esp_find=1;
				*esp_direction = 1;
				*esp_state =0;
				esp_db_entry->rspi=spi;
				esp_db_entry->state=1;
				*ospi=*rspi=esp_db_entry->ospi;
			}
		}
	}	
	
	WRITE_UNLOCK(&ip_esp_lock);
	
	return;
}

void ip_conntrack_find_esp_db_spi_get(u_int32_t spi,u_int32_t *ospi,u_int32_t *rspi)
{
	struct ip_ct_esp_db  *esp_db_entry;

	WRITE_LOCK(&ip_esp_lock);
	esp_db_entry = LIST_FIND(&esp_db_list, esp_find_ospi_cmp_fn,
		        struct ip_ct_esp_db *, spi);

	if ( esp_db_entry)
	{
		*ospi=esp_db_entry->ospi;
		*rspi=esp_db_entry->rspi;
	}
	else
	{
		*ospi=spi;
		*rspi=0;
	}
	
	WRITE_UNLOCK(&ip_esp_lock);

}

EXPORT_SYMBOL(ip_conntrack_find_esp_db_get);
EXPORT_SYMBOL(esp_find_ospi_cmp_fn);
EXPORT_SYMBOL(ip_conntrack_find_esp_db_spi_get);
EXPORT_SYMBOL(ip_conntrack_esp_destroy_db);
EXPORT_SYMBOL(esp_find_outgoing_spi_cmp_fn);
EXPORT_SYMBOL(esp_db_list);

