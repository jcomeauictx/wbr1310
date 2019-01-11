/* vi: set sw=4 ts=4: */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ixml.h>
#include "lrgbin.h"
#include "rgdb.h"
//#define HNAP_STANDALONE 	1
#define	RGDB_NODE_BASE		"/runtime/hnap/"
#define HNAP_PHP_PATH		"/etc/hnap/"
#define HNAP_SHELL_PATH		"/var/run/"
#define HTTPD_PASSWD        "/var/etc/httpasswd"
#define SHELL_ACTION_FILE	".shell_action"
typedef enum { SA_FOREGROUND, SA_BACKGROUND } SHELL_ACTION;
struct	_shell_action
{
	SHELL_ACTION	action;
	char			*action_str;
};
struct	_shell_action shell_action[2] = {
{SA_FOREGROUND, "FOREGROUND"},
{SA_BACKGROUND, "BACKGROUND"}};

#if 1
#include <stdarg.h>
void dtrace(const char * format, ...)
{
	FILE * fd;
	va_list marker;

	fd = fopen("/dev/console", "w");
	if (fd)
	{
		va_start(marker, format);
		vfprintf(fd, format, marker);
		va_end(marker);
		fclose(fd);
	}
}
#else
#define dtrace(x, args...)
#endif
int sock_fd=0;


/*
 *	Mapping of ANSI chars to base64 Mime encoding alphabet (see below)
 */
#define DEF_OUTLEN 128

static char	map64[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static int Decode64(char *outbuf, char *string, int outlen)
{
	unsigned long	shiftbuf;
	char		*cp, *op;
	int		c, i, j, shift;

	op = outbuf;
	*op = '\0';
	cp = string;
	while (cp && *cp && *cp != '=') {
/*
 *		Map 4 (6bit) input bytes and store in a single long (shiftbuf)
 */
		shiftbuf = 0;
		shift = 18;
		for (i = 0; i < 4 && *cp && *cp != '='; i++, cp++) {
			c = map64[*cp & 0xff];
			if (c == -1) {
				dtrace("Bad string: %s at %c index %d\n", string, c, i);
				return -1;
			} 
			shiftbuf = shiftbuf | (c << shift);
			shift -= 6;
		}
/*
 *		Interpret as 3 normal 8 bit bytes (fill in reverse order).
 *		Check for potential buffer overflow before filling.
 */
		--i;
		if ((op + i) >= &outbuf[outlen]) {
			dtrace("String too big\n");
			return -1;
		}
		for (j = 0; j < i; j++) {
			*op++ = (char) ((shiftbuf >> (8 * (2 - j))) & 0xff);
		}
		*op = '\0';
	}
	return 0;
}

static int check_realm(char *Auth)
{
	FILE *fd;
	char buf[128], *p, *q;
	int c, gotcr, rlt=-1;
	size_t bp;
	char str[DEF_OUTLEN+1];

	if (!Auth || strncasecmp(Auth, "basic ", 6))
		return -1;
	Auth+=6;
	while (*Auth == ' ') ++Auth;

	if(Decode64(str, Auth, DEF_OUTLEN)<0)
		return -1;
	if (NULL == (fd = fopen(HTTPD_PASSWD, "r"))) {
		dtrace("cannot open userfile %s", HTTPD_PASSWD);
		return -1;
	}

	q = strchr(str, ':');
	if (q == 0) {
		dtrace("no colon in decoded authorization");
		return -1;
	}
	*q++ = 0;
	bp = 0;
	gotcr = 0;
dtrace("\n----->\n");
dtrace("Login: %s/'%s'\n", str, q);
	while ((c = getc(fd)) != EOF)
		if (c == '\n') {
			if (bp < sizeof buf) {
				buf[bp] = 0;
				p = strchr(buf, ':');
				if (p) {
					*p++ = 0;
dtrace("Allow: %s/'%s'\n", buf, p);
					if (strcmp(str, buf) == 0 && strcmp(p, q) == 0) {
						rlt=0;
						break;
					}
				}
			}
			bp = 0;
			gotcr = 0;
		} else {
			if (gotcr && bp < sizeof buf)
				buf[bp++] = '\r';
			gotcr = c == '\r';
			if (gotcr == 0 && bp < sizeof buf)
				buf[bp++] = c;
		}

	fclose(fd);
	return rlt;
}

SHELL_ACTION check_shell_action(char *pFileName)
{
	int  c, j, i, comm, len;
	char str[256], *pName, *pAction;
	FILE *pFile;
	sprintf(str, "%s%s", HNAP_PHP_PATH, SHELL_ACTION_FILE);

	if((pFile=fopen(str, "r")))
	{
		c=i=j=comm=0;
		do
		{
			c=fgetc(pFile);
			if(c==EOF || c=='\n')	//-----get a line
			{
				str[i]=0;
				//-----Get File name
				pName=str;
				for(j=0; j<i; j++)
				{
					if(isspace(pName[j]))
					{
						pName[j++]=0;
						break;
					}
				}
				pAction=str+j;
				if(strlen(pName) && !strcmp(pName, pFileName))
				{	//-----Get File action
					for(; j<i && isspace(str[j]); j++) {}
					if(j<i)
					{
						pAction=str+j;
						len=sizeof(shell_action)/sizeof(struct _shell_action);
						for(j=0; j<len; j++)
						{
							if(!strcasecmp(pAction, shell_action[j].action_str))
							{
								fclose(pFile);
								return shell_action[j].action;
							}
						}
					}
				}
				i=comm=0;
			}
			else if(!comm)	//-----comment
			{
				if(i || isascii(c))
				{
					if(c=='\'' || c=='#')
						comm=1;
					else
						str[i++]=(char)c;
				}
			}
		}while(c!=EOF);
		fclose(pFile);
	}
	return SA_BACKGROUND;
}

IXML_Node *get_parameter_nodehead(IXML_Node *ixml_node)
{
	IXML_Node * node;
	for (node=ixml_node; node; node = node->firstChild)
	{
		if (strcmp(node->nodeName, "soap:Body")==0)
			return node->firstChild;
	}
	
	dtrace("%s: can not find xml node \"soap:Body\"\n", __func__);
	return NULL;
}

static void parse_child(char *rgdb_nodebase, IXML_Node *pIxml_nodebase)
{
	int			index=0;
	char		rgdb_node[256]={0};
	IXML_Node	*pIxml_node=pIxml_nodebase, *pIxml_nodenext=NULL;
	while(pIxml_node)
	{
		pIxml_nodenext=pIxml_node->nextSibling;
		if(index ||	(pIxml_nodenext && !strcmp(pIxml_node->nodeName, pIxml_nodenext->nodeName)))
			index++;	//-----We got an array

		if(pIxml_node->nodeValue)
		{
			lrgdb_set(sock_fd, 0, rgdb_nodebase, pIxml_node->nodeValue);
			dtrace("%s(%s)\n", rgdb_nodebase, pIxml_node->nodeValue);
		}
		if(index)
			sprintf(rgdb_node, "%s/%s:%d", rgdb_nodebase, pIxml_node->nodeName, index);
		else
			sprintf(rgdb_node, "%s/%s", rgdb_nodebase, pIxml_node->nodeName);
		if(pIxml_node->firstChild)
			parse_child(rgdb_node, pIxml_node->firstChild);
		pIxml_node=pIxml_nodenext;
	}
}

static int parse_node(char *action_name, char *xml_body)
{
	char		rgdb_node[256]={0};
	IXML_Node	*pIxml_node=NULL;

	if(!(pIxml_node=(IXML_Node *)ixmlParseBuffer(xml_body)) ||
	   !(pIxml_node=get_parameter_nodehead(pIxml_node)))
		return -1;

	//-----set action_name and clean old nodes
	sprintf(rgdb_node, "%saction_name", RGDB_NODE_BASE);
	lrgdb_set(sock_fd, 0, rgdb_node, action_name);
	sprintf(rgdb_node, "%s%s", RGDB_NODE_BASE, action_name);
	lrgdb_del(sock_fd, 0, rgdb_node);
	if(pIxml_node->firstChild)
		parse_child(rgdb_node, pIxml_node->firstChild);

	if(pIxml_node)	free(pIxml_node);

	return 0;
}


static void print_unauth()
{
	printf("HTTP/1.1 401 Not Authorized\r\n");
	printf("WWW-Authenticate: Basic realm=\"HNAP 1.0\"\r\n");
	printf("Content-Type: text/html\r\n\r\n");
    printf("<title>401 Not Authorized</title>\r\n");
    printf("<h1>401 Not Authorized</h1>\r\n");
    printf("You need proper authorization to use this resource.\r\n\r\n");
}

#ifndef HNAP_STANDALONE
int hnap_main (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
	int  rlt=0;
	char str[256]={0};
	char *pAuth_str      = getenv("HTTP_AUTHORIZATION");
	char *pSoap_action   = getenv("HTTP_SOAPACTION");
	char *pQuery_str     = getenv("QUERY_STRING");

	do
	{
		//-----Connect to rgdb
		if ((sock_fd=lrgdb_open(RGDB_DEFAULT_UNIXSOCK)) < 0)
		{
			dtrace("%s: Error connecting database...\n", __func__);
			rlt=-1;
			break;
		}

		//-----Use GetDeviceSettings instead "GET /"
		if(!pSoap_action)
		{
			pSoap_action="http://purenetworks.com/HNAP1/GetDeviceSettings";
		}
		else if(check_realm(pAuth_str)<0)
		{
			print_unauth();
			rlt=-2;
			dtrace("%s: auth not pass!\n", __func__);
			break;
		}

		//-----strip url
		if (!(pSoap_action = strrchr (pSoap_action, '/')))
		{
			dtrace("%s: can not get soap_action\n", __func__);
			rlt=-3;
			break;
		}
		pSoap_action++;
		if(*(pSoap_action+strlen(pSoap_action)-1)=='"')
			*(pSoap_action+strlen(pSoap_action)-1)=0;

		if(pQuery_str)
			pQuery_str=strchr(pQuery_str, '<');

		dtrace("SoapAction: %s\n", pSoap_action);
		dtrace("----->\n");
		//-----parse xml node
		parse_node(pSoap_action, pQuery_str);

		sprintf(str, "%s%s.php\nShellPath=%s%s.sh", HNAP_PHP_PATH, pSoap_action, HNAP_SHELL_PATH, pSoap_action);
		lrgdb_ephp(sock_fd, 0, str, stdout); 
		close(sock_fd);

		//-----check shell action type
		sprintf(str, "%s.php", pSoap_action);
		if(check_shell_action(str)==SA_FOREGROUND)
			sprintf(str, "sh %s%s.sh > /dev/console", HNAP_SHELL_PATH, pSoap_action);
		else
			sprintf(str, "sh %s%s.sh > /dev/console &", HNAP_SHELL_PATH, pSoap_action);
		system(str);
	}while(0);
	return rlt;
}
