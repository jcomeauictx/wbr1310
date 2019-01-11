/* vi: set sw=4 ts=4: */
/*
 * libxmldbc.h
 *
 */

#ifndef __LIBXMLDBC_HEADER__
#define __LIBXMLDBC_HEADER__

#define MAX_CMD_LEN	1024

typedef unsigned long flag_t;
typedef const char * sock_t;

int		lxmldbc_run_shell(char * buf, int size, const char * format, ...);
int		lxmldbc_system(const char * format, ...);
char *	lxmldbc_eatwhite(char * string);
char * 	lxmldbc_reatwhite(char * ptr);

int		xmldbc_get(		sock_t sn, flag_t f, const char * node, FILE * out);
ssize_t	xmldbc_get_wb(	sock_t sn, flag_t f, const char * node, char * buff, size_t size);
int		xmldbc_ephp(	sock_t sn, flag_t f, const char * file, FILE * out);

int		xmldbc_set(		sock_t sn, flag_t f, const char * node, const char * value);
int		xmldbc_setext(	sock_t sn, flag_t f, const char * node, const char * cmd);
int		xmldbc_del(		sock_t sk, flag_t f, const char * node);
int		xmldbc_reload(	sock_t sn, flag_t f, const char * file);
int		xmldbc_dump(	sock_t sn, flag_t f, const char * file);
int		xmldbc_timer(	sock_t sn, flag_t f, const char * cmd);
int		xmldbc_killtimer(sock_t sn,flag_t f, const char * tag);

#endif
