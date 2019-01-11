/*------------------------------*/
/* vi: set sw=4 ts=4: */
/* Added by Paul Liu 20050902 */
/* Add support for session control.*/
/*------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include "mathopd.h"
#include "web-session.h"

#if 0
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

static int deltree(char *pStr, int child_only)
{
	int     rlt = 0;
	DIR     *pDir;
	struct  dirent *pDent;
	struct  stat st;
	char    filename[256];

	if(!(pDir = opendir(pStr)))
		return -1;

	while((pDent = readdir(pDir)))
	{
		sprintf(filename, "%s/%s", pStr, pDent->d_name);
		if(!strcmp(pDent->d_name, ".") || !strcmp(pDent->d_name, ".."))
			continue;
dtrace("\t\t-->delchild %s\n", filename);
		if((rlt = stat(filename, &st)) < 0)
			break;

		if(S_ISDIR(st.st_mode))
			deltree(filename, child_only);
		else
			remove(filename);
	}

	closedir(pDir);
	if(!child_only)
		rmdir(pStr);
	return rlt;
}

static int forcedir(char *pStr)
{
	int     endpath = 0;
	char    *pEnd, path[256], curpath[256];
	struct  stat st;
	
	if(!pStr)
		return -1;
	strcpy(path, pStr);
	pEnd = path;
	if(*pEnd == '/') pEnd++;   //-----Ignore leading '/'

dtrace("\t\t-->forcedir %s\n", pStr);
	while(!endpath)
	{
		memset(curpath, 0, sizeof(curpath));
		if((pEnd = strchr(pEnd, '/')))
			strncpy(curpath, path, pEnd-path);
		else
		{
			strcpy(curpath, path);
			endpath = 1;
		}

dtrace("\t\t-->check %s\n", curpath);
		if(0 == stat(curpath, &st))
		{
			if(!S_ISDIR(st.st_mode))
				return -1;
dtrace("\t\t-->%s is dir\n", curpath);
		}
		else if(ENOENT != errno || mkdir(curpath, 0755) < 0)
			return -1;
		pEnd++;
	}
	return 0;
}

static int del_node(int sid, char *pNode)
{
	char path[256];
	if(pNode)
	{
		sprintf(path, "%ssession:%d/%s", NODE_PATH, sid, pNode);
dtrace("\t-->remove %s\n", path);
		remove(path);
	}
	else
	{
		sprintf(path, "%ssession:%d", NODE_PATH, sid);
dtrace("\t-->deltree %s\n", path);
		deltree(path, 1);
	}
	return 0;
}

static int get_val(char *pVal, int sizeVal, const char * format, ...)
{
	char    path[256];
	int     fd, size = -1;
	va_list marker;

	if(pVal)
	{
		va_start(marker, format);
		vsnprintf(path, sizeof(path), format, marker);
		va_end(marker);

		memset(pVal, 0, sizeVal);
		if((fd = open(path, O_RDONLY)) > 0)
		{
			size = read(fd, pVal, sizeVal);
			close(fd);
		}
	}

	return size;
}

static int set_val(char *pVal, int sizeVal, const char * format, ...)
{
	int     fd, size = -1;
	char    path[256], *pEnd;
	struct  stat st;
	va_list marker;

	if(pVal)
	{
		va_start(marker, format);
		vsnprintf(path, sizeof(path), format, marker);
		va_end(marker);

		pEnd = strrchr(path, '/');
		*pEnd = 0;
		if(0 == stat(path, &st))
		{
			if(!S_ISDIR(st.st_mode))
				return -1;
		}
		else if(ENOENT == errno && forcedir(path) < 0)
			return -1;
		*pEnd = '/';
		if((fd = open(path, O_WRONLY|O_CREAT, 0755)) > 0)
		{
			size = write(fd, pVal, sizeVal);
			close(fd);
		}
	}
	return size;
}

int get_session_id(struct request *r)
{
	int     i, AvlSession = -1;
	int     CurTime, LastTime, IdleTime;
	char    Val[32], Addr[20];
	struct  sysinfo info;

	strcpy(Addr, inet_ntoa(r->cn->peer.sin_addr));
	if(get_val(Val, sizeof(Val), "%s/sessiontimeout", NODE_PATH)<0)
		IdleTime=r->c->session_idletime;
	else
		IdleTime=atoi(Val);
dtrace("-->IdleTime=%d\n", IdleTime);
	sysinfo(&info);
	CurTime = (int)info.uptime;
	for(i = 1; i <= r->c->session_max; i++)
	{
		//-----Get IdleTime
		get_val(Val, sizeof(Val), "%ssession:%d/time", NODE_PATH, i);
    	LastTime = atoi(Val);
dtrace("-->%d. LastTime=%d\n", i, LastTime);
		if(LastTime != 0 && (CurTime-LastTime) <= IdleTime)
		{
			//-----Get IP
			get_val(Val, sizeof(Val), "%ssession:%d/ip", NODE_PATH, i);

dtrace("-->%d. ip=%s\n", i, Val);
			if(!strcmp(Addr, Val))
			{
				sprintf(Val, "%d", CurTime);
				set_val(Val, strlen(Val), "%ssession:%d/time", NODE_PATH, i);
				return i;
			}
		}
		else if(AvlSession<1)    //----------Find a availible session
			AvlSession = i;
   }

dtrace("-->AvlSession=%d\n", AvlSession);
 
 if(AvlSession>0)
   {
dtrace("-->AvlSession %d. del_node\n",AvlSession);
    //-----Clear Old Information
	del_node(AvlSession, NULL);

    //-----Set new session info
    sprintf(Val, "%d", CurTime);
dtrace("-->AvlSession %d. set time=%s\n", AvlSession, Val);
	set_val(Val, strlen(Val), "%ssession:%d/time", NODE_PATH, AvlSession);
dtrace("-->AvlSession %d. set ip=%s\n", AvlSession, Addr);
	set_val(Addr, strlen(Addr), "%ssession:%d/ip", NODE_PATH, AvlSession);
   }

 return AvlSession;
}

int get_sessiongrp(struct request *r)
{
	int     grp = -1;
	char    Val[32];
	if((r->sessionid > 0 && r->sessionid <= r->c->session_max) &&
		get_val(Val, sizeof(Val), "%ssession:%d/user/group", NODE_PATH, r->sessionid) > 0)
		grp = atoi(Val);
dtrace("-->Group=%d", grp);
	return grp;
}

int check_session(struct request *r)
{
	int auth = -1;
	char    Val[32];
	if((r->sessionid > 0 && r->sessionid <= r->c->session_max) &&
		get_val(Val, sizeof(Val), "%ssession:%d/user/ac_auth", NODE_PATH, r->sessionid) > 0)
		auth=atoi(Val);
dtrace("-->Auth=%d", auth);
	return auth;
}
