/*------------------------------*/
/* vi: set sw=4 ts=4: */
/* Added by Paul Liu 20040326 */
/* Process Internal Call */
/*------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mathopd.h"

extern upload upload_file;

extern int  pool_print(struct pool *p, const char *fmt, ...);
extern int  upload_image(struct request *r, struct pool *p);
extern int  upload_config(struct request *r, struct pool *p);
extern void rlt_page(struct pool *p, char *pFName);

int process_internal(struct request *r)
{
	int  rlt=-1;
	char Name[128]={0};
	char *pArgs=r->path_translated+strlen(r->path_translated);
	char *pEnd=NULL;
	struct pool *p=NULL;

	while(--pArgs>r->path_translated && *(pArgs-1)!='/');

	if ((pEnd=strchr(pArgs, '.'))) *pEnd=0;

	p=&r->cn->output;
	pool_print(p, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");
	r->forked=1;
	r->cn->keepalive=0;

	if (0==Name[0] || !strncmp(r->user, Name, strlen(r->user)))
	{
		if(!strncasecmp(pArgs, "upload_image", 12))
			rlt=upload_image(r, p);
		else if(!strncasecmp(pArgs, "upload_config", 13))
			rlt=upload_config(r, p);
		else
			rlt_page(p, r->c->error_fwup_file);
	}

	//-----CleanUp Memory
	if(rlt<0)
	{
		if(upload_file.data) free(upload_file.data);
		memset(&upload_file, 0, sizeof(upload_file));
		//-----Restore service
		system("/etc/scripts/misc/preupgrade.sh restore");
	}

	return 0;
}
