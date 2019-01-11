/* vi: set sw=4 ts=4: */
/*
 * Mini syslogd implementation for busybox
 *
 * Copyright (C) 1999,2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 1999,2000,2001 by Erik Andersen <andersee@debian.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2001 by Gennady Feldman <gfeldman@cachier.com>
 *
 * Maintainer: Gennady Feldman <gena01@cachier.com> as of Mar 12, 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/param.h>

/* SYSLOG_NAMES defined to pull some extra junk from syslog.h */
#define SYSLOG_NAMES
#include <sys/syslog.h>
#include <sys/uio.h>
#define ASYSLOG_NAMES
#include "asyslog.h"

enum   BOOL {FALSE=0, TRUE};
#define BUF_SIZE 8192

static int  proclog=TRUE;
static int 	asyslogflag = 0;
#define defMaxLine 400

/* Path for the file where all log messages are written */
#define __LOG_FILE "/var/log/messages"

/* Path to the unix socket */
static char lfile[MAXPATHLEN];

static char *logFilePath = __LOG_FILE;

/* interval between marks in seconds */
//static int MarkInterval = 20 * 60;

/* localhost's name */
static char LocalHostName[64];

#ifdef REMOTE_LOG
#include <netinet/in.h>
static int  remotefd 		= -1;	 /* udp socket for logging to remote host */
static char *RemoteHost; 			 /* where do we log? */
static int  RemotePort 		= 514;	 /* what port to log to? */
static int  doRemoteLog 	= FALSE; /* To remote log or not to remote log. */
static int  local_logging 	= FALSE;
#endif

/* joanw add for mailing log message. 2004.04.14 */
/* initial the variables of mail				 */
static int  mail_log      = 0;						/* -m , Enable mail log message.		*/
static char *mail_subject = "Log Message.";			/* -s , the subject of the mail.		*/
static char *mail_host    = NULL;					/* -S , SMTP server IP.					*/
static char *mail_port    = "25";					/* -P , Port for contacting via SMTP.	*/
char 		mail_from[128]= "admin@";
char 		*src_host     = "RG-Administrator";		/* -H , The host name of SMTP Client.	*/
static int  mime_style    = 0;						/* -M , Use the MIME style.				*/
static char *email_addr   = NULL;					/* -a , The e-mail address to mail to.	*/

static int	send_mail	  = 0; 
static int	tlog		  = 0;						/* -t , use tlog.						*/
static char *log_path 	  = "/var/log/messages";	/* the path of log message.				*/
static char *temp_log_path= "/var/run/mail.log";	/* the path of the renamed log message.	*/
static char *smtp_path	  = "/usr/sbin/smtpclient";	/* the path of smtp client				*/

#define MAXLINE         	1024					/* maximum line length */

static void show_usage(char **argv)
{
	char *p = argv[0];

	while (strchr(p, '/'))
	{
		p=strchr(p, '/')+1;
	}

	printf("\n");
	printf("\n");
	printf("Usage: %s [-l level] [-d]\n", p);
	printf("\n");
	printf("Syslogd Options:\n");
	printf("\n");
	printf("  -p                    :Proclog\n");
	printf("  -n                    :Don't do fork.\n");
	printf("  -O path               :Log File Path.\n");
#ifdef REMOTE_LOG
	printf("  -R remote_ip:port     :Remote logging.\n");
#endif
	printf("  -L                    :Local logging.\n");
#ifdef IPC_SYSLOG
	printf("  -C                    :Circular logging the message.\n");
#endif
#ifdef ASYSLOG_NAMES
	printf("  -F flag_name          :Alphanetworks System Log message flag name.\n");
	printf("	 sysact : system activity\n");
	printf("	 debug  : debug information\n");
	printf("	 attack : attacks\n");
	printf("	 drop   : dropped packet\n");
	printf("	 notice : notice\n");
	printf("  -m                    :Enable mail log message when message file is full.\n");
	printf("\n");
#endif
	printf("Mail Options:\n");
	printf("  -s mail_subject       :Subject\n");
	printf("  -S mailhost           :SMTP Server IP.\n");
	printf("  -P mail_port          :Port where MTA can be contacted via SMTP.\n");
	printf("  -H src_host_name      :Host name to provide as source of message.\n");
	printf("  -M                    :Use MIME-style translation to quoted-printable.\n");
	printf("  -a email_addr         :The e-mail address you want to mail log to.\n");
	printf("  -t tlog_file_path     :the tlog file path.");
	printf("\n");
	return;
}
static void perror_msg_and_die(const char *s, ...)
{
	va_list p;
	va_start(p, s);

	fflush(stdout);
	fprintf(stderr, "syslogd: ");
	vfprintf(stderr, s, p);
	fprintf(stderr, ": %s\n", strerror(errno));
	va_end(p);
	exit(EXIT_FAILURE);
}

/* circular buffer variables/structures */
#ifdef IPC_SYSLOG
#if __GNU_LIBRARY__ < 5
#error Sorry.  Looks like you are using libc5.
#error libc5 shm support isnt good enough.
#error Please disable IPC_SYSLOG
#endif

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* our shared key */
static const long KEY_ID = 0x414e4547; /*"GENA"*/

/* Semaphore operation structures */
static struct shbuf_ds 
{
	int  size;               /* size of data written	*/
	int  head;               /* start of message list	*/
	int  tail;               /* end of message list		*/
	char data[1];            /* data/messages			*/
} *buf = NULL;               /* shared memory pointer	*/

static struct sembuf SMwup[1] = {{1, -1, IPC_NOWAIT}};		/* set SMwup 			*/
static struct sembuf SMwdn[3] = {{0, 0}, {1, 0}, {1, +1}};	/* set SMwdn 			*/

static int    shmid 			= -1;						/* ipc shared memory id	*/
static int    s_semid 			= -1;						/* ipc semaphore id		*/
int			  data_size 		= 32000;					/* data size			*/
int			  shm_size 			= 32000 + sizeof(*buf);		/* our buffer size		*/
static int	  circular_logging 	= FALSE;


/*
 * sem_up - up()'s a semaphore.
 */
static inline void sem_up(int semid)
{
	if ( semop(semid, SMwup, 1) == -1 )
		perror_msg_and_die("semop[SMwup]");
}

/*
 * sem_down - down()'s a semaphore
 */
static inline void sem_down(int semid)
{
	if ( semop(semid, SMwdn, 3) == -1 )
		perror_msg_and_die("semop[SMwdn]");
}


void ipcsyslog_cleanup(void){
	printf("Exiting Syslogd!\n");
	if (shmid != -1)
		shmdt(buf);

	if (shmid != -1)
		shmctl(shmid, IPC_RMID, NULL);
	if (s_semid != -1)
		semctl(s_semid, 0, IPC_RMID, 0);
}

void ipcsyslog_init(void)
{
	if (buf == NULL)
	{
		if ((shmid = shmget(KEY_ID, shm_size, IPC_CREAT | 1023)) == -1)
			perror_msg_and_die("shmget");

		if ((buf = shmat(shmid, NULL, 0)) == NULL)
			perror_msg_and_die("shmat");

		buf->size=data_size;
		buf->head=buf->tail=0;

		/* we'll trust the OS to set initial semval to 0 (let's hope) */
		if ((s_semid = semget(KEY_ID, 2, IPC_CREAT | IPC_EXCL | 1023)) == -1)
		{
			if (errno == EEXIST)
			{
				if ((s_semid = semget(KEY_ID, 2, 0)) == -1)
					perror_msg_and_die("semget");
			}
			else
			{
				perror_msg_and_die("semget");
			}
		}
	}
	else
	{
		printf("Buffer already allocated just grab the semaphore?");
	}
}

/* write message to buffer */
void circ_message(const char *msg)
{
	int l=strlen(msg)+1; /* count the whole message w/ '\0' included */

	sem_down(s_semid);

	/*
	 * Circular Buffer Algorithm:
	 * --------------------------
	 *
	 * Start-off w/ empty buffer of specific size SHM_SIZ
	 * Start filling it up w/ messages. I use '\0' as separator to break up messages.
	 * This is also very handy since we can do printf on message.
	 *
	 * Once the buffer is full we need to get rid of the first message in buffer and
	 * insert the new message. (Note: if the message being added is >1 message then
	 * we will need to "remove" >1 old message from the buffer). The way this is done
	 * is the following:
	 *      When we reach the end of the buffer we set a mark and start from the beginning.
	 *      Now what about the beginning and end of the buffer? Well we have the "head"
	 *      index/pointer which is the starting point for the messages and we have "tail"
	 *      index/pointer which is the ending point for the messages. When we "display" the
	 *      messages we start from the beginning and continue until we reach "tail". If we
	 *      reach end of buffer, then we just start from the beginning (offset 0). "head" and
	 *      "tail" are actually offsets from the beginning of the buffer.
	 *
	 * Note: This algorithm uses Linux IPC mechanism w/ shared memory and semaphores to provide
	 *       a threasafe way of handling shared memory operations.
	 */
	if ( (buf->tail + l) < buf->size )
	{
		/* before we append the message we need to check the HEAD so that we won't
		 * overwrite any of the message that we still need and adjust HEAD to point
		 * to the next message! 
		 */
		if ( buf->tail < buf->head)
		{
			if ( (buf->tail + l) >= buf->head )
			{
			  /* we need to move the HEAD to point to the next message
			   * Theoretically we have enough room to add the whole message to the
			   * buffer, because of the first outer IF statement, so we don't have
			   * to worry about overflows here!
			   */
				/* we need to know how many bytes we are overwriting to make enough room */	
			 	int k= buf->tail + l - buf->head;
				char *c=memchr(buf->data+buf->head + k,'\0',buf->size - (buf->head + k));
				if (c != NULL) /* do a sanity check just in case! */
				{
					/* we need to convert pointer to offset + skip the '\0' 
					 * since we need to point to the beginning of the next message 
					 */
					buf->head = c - buf->data + 1; 
					/* Note: HEAD is only used to "retrieve" messages, it's not used
					 *when writing messages into our buffer 
					 */
				}
				else /* show an error message to know we messed up? */
				{ 
					printf("Weird! Can't find the terminator token??? \n");
					buf->head=0;
				}
			}
		} /* in other cases no overflows have been done yet, so we don't care! */

		/* we should be ok to append the message now */
		strncpy(buf->data + buf->tail,msg,l); /* append our message */
		buf->tail+=l; /* count full message w/ '\0' terminating char */
	}
	else
	{
		/* we need to break up the message and "circle" it around */
		char *c;
		int  k=buf->tail + l - buf->size; /* count # of bytes we don't fit */

		/* We need to move HEAD! This is always the case since we are going
		 * to "circle" the message.
		 */
		c=memchr(buf->data + k ,'\0', buf->size - k);

		if (c != NULL) /* if we don't have '\0'??? weird!!! */
		{
			/* move head pointer*/
			buf->head=c-buf->data+1;

			/* now write the first part of the message */
			strncpy(buf->data + buf->tail, msg, l - k - 1);

			/* ALWAYS terminate end of buffer w/ '\0' */
			buf->data[buf->size-1]='\0';

			/* now write out the rest of the string to the beginning of the buffer */
			strcpy(buf->data, &msg[l-k-1]);

			/* we need to place the TAIL at the end of the message */
			buf->tail = k + 1;
		}
		else
		{
			printf("Weird! Can't find the terminator token from the beginning??? \n");
			buf->head = buf->tail = 0; /* reset buffer, since it's probably corrupted */
		}
	}
	sem_up(s_semid);
}
#endif  /* IPC_SYSLOG */

static void dbprint(char *pstrFormat, ...)
{
/* Set on the Flag in the Makefile, then the Debug message, bdprint, will be enabled.*/
#ifdef ALPHA_DBG
	va_list valist;
	va_start(valist, pstrFormat);
	vprintf(pstrFormat, valist);
	va_end(valist);
#endif
}

static size_t proc_log(char *pcBuf, size_t iSizebuf, int *MaxLine)
{
	size_t iPos=0;
	int iLines=0;
	if(pcBuf)
	{
		while(iSizebuf--)
		{
			if(*(pcBuf+iPos++)=='\n')
			{
				iLines++;
				if(iLines >= *MaxLine-1)
				break;
			}
		}
	}
	*MaxLine=iLines;
	return iPos;
}

static size_t read_log(char *pstrFilename, char **pcBuffer)
{
	FILE   *pfileLog=NULL;
	size_t iBufLen=0;

	/*-----Open file and read it into memry-----*/
	if(NULL==(pfileLog=fopen(pstrFilename, "r")))
	{
		dbprint("No log file exists. Create one => %s\n", pstrFilename);
	}
	else
	{
		if(fseek(pfileLog, 0, SEEK_END)==0)
		{
			iBufLen=ftell(pfileLog);
			if(*pcBuffer)
				free(*pcBuffer);
			if(iBufLen > 0 && NULL!=(*pcBuffer=(char *)malloc(sizeof(char)*iBufLen)))
			{
				size_t iRead=0;
				fseek(pfileLog, 0, SEEK_SET);
				iRead=fread(*pcBuffer, sizeof(char), iBufLen, pfileLog);
				if(iBufLen > iRead)
					iBufLen = iRead;

				fclose(pfileLog);
				return iBufLen;
			}
		}
		fclose(pfileLog);
	}
	return -1;
}

static void insert_log(const char *pstrMsg)
{
	FILE   *pfileLog	= NULL;
	char   *pcBuf		= NULL;
	char   buf[255];
	int    iLines		= defMaxLine;
	size_t iBufLen		= -1;
	char   mailmsg[255];
	time_t now;
		

	iBufLen=read_log(__LOG_FILE, &pcBuf);

	/*-----Count Lines and delete the oldest data-----*/
	if(iBufLen)
		iBufLen=proc_log(pcBuf, iBufLen, &iLines);

	if(NULL!=(pfileLog=fopen(__LOG_FILE,"w")))
	{
		fprintf(pfileLog, "%s", pstrMsg);

		/*-----Write old data into log-----*/
		if(pcBuf)
		{
			fwrite(pcBuf, iBufLen, sizeof(char), pfileLog);
			free(pcBuf);
		}
		fclose(pfileLog);
		dbprint("Insert log: %s\n(Total lines: %d)\n", pstrMsg, iLines+1);
	}
	else
		fprintf(stderr, "Could not create log file\n");

	/* joanw add for mail the log message. 2004.04.14 */
	if((iLines >= defMaxLine-2) && mail_log)
	{
		if(tlog)
		{
			sprintf(buf, "tlogs -l %s > %s", log_path, temp_log_path);
			system(buf);
			
			send_mail = 1;
		}
		else if(!rename(log_path , temp_log_path)) send_mail = 1;
		
		if(send_mail)
		{
			sprintf(buf, "%s -s \"%s\" -f \"%s\" -r \"%s\" -H \"%s\" -S \"%s\" -P \"%s\" \"%s\" < %s",
						smtp_path , mail_subject, email_addr, email_addr, src_host, mail_host, mail_port, 
						email_addr, temp_log_path);
			system(buf);

			if(NULL!=(pfileLog=fopen(__LOG_FILE,"w")))
			{
				time(&now);
				memcpy(mailmsg, ctime(&now)+4, 15);
				mailmsg[15]='\0';
#ifdef LOGNUM
				fprintf(pfileLog,"%s  |  SYS:003[%s]\n", mailmsg, email_addr);
#else
				fprintf(pfileLog,"%s  |  Log Message is full. Mailed the Log Message file to %s.\n",
							mailmsg, email_addr);
#endif
				fclose(pfileLog);
			}
		}
	}
}

/* Note: There is also a function called "message()" in init.c	*/
/* Print a message to the log file.								*/
static void message (char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
static void message (char *fmt, ...)
{
	int 	fd;
	struct 	flock fl;
	va_list arguments;
	char 	buf[BUF_SIZE];
	int 	len;

	fl.l_whence = SEEK_SET;
	fl.l_start  = 0;
	fl.l_len    = 1;

#ifdef IPC_SYSLOG
	if ((circular_logging == TRUE) && (buf != NULL))
	{
		char b[1024];
		va_start (arguments, fmt);
		vsnprintf (b, sizeof(b)-1, fmt, arguments);
		va_end (arguments);
		circ_message(b);
	}
	else
#endif
	if (proclog)
	{
		/*-----This section will cause crash in samsun S3C2510
					char b[1024];
					va_start (arguments, fmt);
					vsnprintf (b, sizeof(b)-1, fmt, arguments);
					va_end (arguments);
					insert_log(b); -----*/
		va_start (arguments, fmt);
		len = vsprintf(buf, fmt, arguments);
        insert_log(buf);
		va_end (arguments);
	}
	else
	if ((fd = open (logFilePath, O_WRONLY | O_CREAT | O_NOCTTY | O_APPEND | O_NONBLOCK, 0600)) >= 0)	{
		fl.l_type = F_WRLCK;
		fcntl (fd, F_SETLKW, &fl);
		va_start (arguments, fmt);
		len = vsprintf(buf, fmt, arguments);
		write(fd, buf, len);
		va_end (arguments);
		fl.l_type = F_UNLCK;
		fcntl (fd, F_SETLKW, &fl);
		close (fd);
	}
	else
	{
		/* Always send console messages to /dev/console so people will see them. */
		if ((fd = open(_PATH_CONSOLE, O_WRONLY | O_NOCTTY | O_NONBLOCK, 0600)) >= 0) 
		{
			va_start (arguments, fmt);
			len = vsprintf(buf, fmt, arguments);
			write(fd, buf, len);
			va_end (arguments);
			close (fd);
		}
		else
		{
			fprintf (stderr, "Bummer, can't print: ");
			va_start (arguments, fmt);
			vfprintf (stderr, fmt, arguments);
			fflush (stderr);
			va_end (arguments);
		}
	}
}

static void logMessage (int pri, char *msg)
{
	time_t 		now;
	char 		*timestamp;
	static char res[20] = "";
	CODE 		*c_pri, *c_fac;
	char * ptr;

	if (pri != 0)
	{
		for (c_fac = afacilitynames;
				c_fac->c_name && !(c_fac->c_val == LOG_FAC(pri) << 3); c_fac++);
		for (c_pri = prioritynames;
				c_pri->c_name && !(c_pri->c_val == LOG_PRI(pri)); c_pri++);
		if (c_fac->c_name == NULL || c_pri->c_name == NULL)
			snprintf(res, sizeof(res), "<%d>", pri);
		else
			snprintf(res, sizeof(res), "%s.%s", c_fac->c_name, c_pri->c_name);
	}
	
	/* joanw debug */
	dbprint("asyslogflag is set as %d.\n", asyslogflag);
	dbprint("pri=%d\n", pri);
	dbprint("LOG_FAC(pri)=%d\n", LOG_FAC(pri));
	dbprint("If LOG_FAC(pri) is between 24~28, the message might be log.\n");

	/* filter log message */
	if(	((asyslogflag & 0x01) && (24 == LOG_FAC(pri))) ||
		((asyslogflag & 0x02) && (25 == LOG_FAC(pri))) ||
		((asyslogflag & 0x04) && (26 == LOG_FAC(pri))) ||
		((asyslogflag & 0x08) && (27 == LOG_FAC(pri))) ||
		((asyslogflag & 0x10) && (28 == LOG_FAC(pri)))	)
	{
		/* joanw debug */
		dbprint("etner the if loop.\n");		
		if ( strlen(msg) < 16 || msg[3] != ' ' || msg[6] != ' ' || msg[9] != ':' || 
			 msg[12] != ':'   || msg[15] != ' ') 
		{
			time(&now);
			timestamp = ctime(&now) + 4;
			timestamp[15] = '\0';
		}
		else
		{
			timestamp = msg;
			timestamp[15] = '\0';
			msg += 16;
		}

		/* todo: supress duplicates */

#ifdef REMOTE_LOG
		/* send message to remote logger */
		if ( -1 != remotefd){
			static const int IOV_COUNT = 2;
			struct iovec iov[IOV_COUNT];
			struct iovec *v = iov;
	
			memset(&res, 0, sizeof(res));
			/* joanw fixed 
			 * When REMOTE_LOG, we set the all the message's priority as 6,LOG_INFO,
			 * and priority should be between 0 to 7(you can find out the detail in the syslog.h),
			 * or the message will be refused by the remote host.
			 */
			 //snprintf(res, sizeof(res), "<%d>", pri);
			snprintf(res, sizeof(res), "<%d>", LOG_INFO);
			v->iov_base = res ;
			v->iov_len = strlen(res);
			v++;
	
			v->iov_base = msg;
			v->iov_len = strlen(msg);
writev_retry:
			if ( -1 == writev(remotefd,iov, IOV_COUNT))
			{
				printf("err %d: %s\n", errno, strerror(errno));
				if (errno == EINTR) goto writev_retry;
				/* joanw fixed													 */
				/* we don't want syslogd to terminate, so we mark below 2 lines. */
				 //perror_msg_and_die("cannot write to remote file handle on"
				 //		"%s:%d",RemoteHost,RemotePort);
			}
		}
		/* joanw fixed 																  */
		/* whatever remote logging or local logging, we both log the message locally, */
		/* so we mark the below 1 line.												  */
		 //if (local_logging == TRUE)

#endif /* endif of REMOTE_LOG */
		/* now spew out the message to wherever it is supposed to go */
		//message("%s %s %s%s\n", timestamp, res, msg, postfix);
		ptr = strchr(msg, ':');
		if (ptr) msg = ptr + 2;
		message("%s  |  %s\n", timestamp, msg);
	}
}

static void quit_signal(int sig)
{
	logMessage(LOG_SYSLOG | LOG_INFO, "System log daemon exiting.");
	unlink(lfile);
#ifdef IPC_SYSLOG
	ipcsyslog_cleanup();
#endif

	exit(TRUE);
}

#ifdef ORG_CODE
static void domark(int sig)
{
	if (MarkInterval > 0) {
		logMessage(LOG_SYSLOG | LOG_INFO, "-- MARK --");
		alarm(MarkInterval);
	}
}
#endif

/* This must be a #define, since when DODEBUG and BUFFERS_GO_IN_BSS are
 * enabled, we otherwise get a "storage size isn't constant error. */
static int serveConnection (char* tmpbuf, int n_read)
{
	char *p = tmpbuf;
	/* joanw debug */
	dbprint("\n----- from serveConnection ---- \n");
	dbprint("tmpbuf= %s\n\n", tmpbuf);

	while (p < tmpbuf + n_read) 
	{

		int           pri = (LOG_USER | LOG_NOTICE);
		char          line[ MAXLINE + 1 ];
		unsigned char c;

		char *q = line;

		while ( (c = *p) && q < &line[ sizeof (line) - 1 ]) 
		{
			if (c == '<') 
			{
				/* Parse the magic priority number. */
				pri = 0;
				while (isdigit (*(++p))) 
				{
					pri = 10 * pri + (*p - '0');
				}
				if (pri & ~(LOG_FACMASK | LOG_PRIMASK))
				{
					pri = (LOG_USER | LOG_NOTICE);
				}
			} 
			else if (c == '\n') 
			{
				*q++ = ' ';
			} 
			else if (iscntrl (c) && (c < 0177)) 
			{
				*q++ = '^';
				*q++ = c ^ 0100;
			} 
			else 
			{
				*q++ = c;
			}
			p++;
		}
		*q = '\0';
		p++;

		/* joanw debug */
		dbprint("enter logMessage\n");
		/* Now log it */
		logMessage (pri, line);
		dbprint("leave logMessage\n");
		dbprint("----------------------\n");
	}
	return n_read;
}


#ifdef REMOTE_LOG
static void init_RemoteLog (void)
{
	/* joanw debug */
	printf("init_RemotedLog\n");
	struct 	sockaddr_in remoteaddr;
	struct	 hostent *hostinfo;
	int		len = sizeof(remoteaddr);

	memset(&remoteaddr, 0, len);

	remotefd = socket(AF_INET, SOCK_DGRAM, 0);

	if (remotefd < 0) 
	{
    	perror_msg_and_die("cannot create socket");
	}

	if ((hostinfo = gethostbyname(RemoteHost)) == NULL)
		perror_msg_and_die("%s", RemoteHost);

	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
	remoteaddr.sin_port = htons(RemotePort);

	/*
	 *  Since we are using UDP sockets, connect just sets the default host and port
	 *  for future operations
	 */
	if ( 0 != (connect(remotefd, (struct sockaddr *) &remoteaddr, len)))
	{
		perror_msg_and_die("cannot connect to remote host %s:%d", RemoteHost, RemotePort);
	}
}
#endif

//static void doSyslogd (void) __attribute__ ((noreturn));
static void doSyslogd (void)
{
	struct sockaddr_un sunx;
	socklen_t addrLength;

	int sock_fd;
	fd_set fds;

	/* Set up signal handlers. */
	signal (SIGINT,  quit_signal);
	signal (SIGTERM, quit_signal);
	signal (SIGQUIT, quit_signal);
	signal (SIGHUP,  SIG_IGN);
	signal (SIGCHLD, SIG_IGN);
#ifdef SIGCLD
	signal (SIGCLD,  SIG_IGN);
#endif
	/*signal (SIGALRM, domark);
	alarm (MarkInterval);*/

	/* Create the syslog file so realpath() can work. */
	if (realpath (_PATH_LOG, lfile) != NULL)
		unlink (lfile);

	memset (&sunx, 0, sizeof (sunx));
	sunx.sun_family = AF_UNIX;
	strncpy (sunx.sun_path, lfile, sizeof (sunx.sun_path));
	if ((sock_fd = socket (AF_UNIX, SOCK_DGRAM, 0)) < 0)
		perror_msg_and_die("Couldn't get file descriptor for socket " _PATH_LOG);

	addrLength = sizeof (sunx.sun_family) + strlen (sunx.sun_path);
	if (bind(sock_fd, (struct sockaddr *) &sunx, addrLength) < 0)
		perror_msg_and_die("Could not connect to socket " _PATH_LOG);

	if (chmod (lfile, 0666) < 0)
		perror_msg_and_die("Could not set permission on " _PATH_LOG);

#ifdef IPC_SYSLOG
	if (circular_logging == TRUE )
	{
	   ipcsyslog_init();
	}
#endif

#ifdef REMOTE_LOG
	if (doRemoteLog == TRUE)
	{
	  init_RemoteLog();
	}
#endif

	logMessage (LOG_SYSLOG | LOG_INFO, "syslogd started: ");

	for (;;) 
	{
		FD_ZERO (&fds);
		FD_SET (sock_fd, &fds);

		if (select (sock_fd+1, &fds, NULL, NULL, NULL) < 0) 
		{
			if (errno == EINTR) 
			{
				/* alarm may have happened. */
				continue;
			}
			perror_msg_and_die("select error");
		}

		if (FD_ISSET (sock_fd, &fds)) 
		{
			int   i;
			char tmpbuf[BUFSIZ+1];
			memset(tmpbuf, '\0', BUFSIZ+1);
			if ( (i = recv(sock_fd, tmpbuf, BUFSIZ, 0)) > 0) 
			{
				serveConnection(tmpbuf, i);
			} 
			else 
			{
				perror_msg_and_die("UNIX socket error");
			}
		}/* FD_ISSET() */
	} /* for main loop */
}

int main(int argc, char **argv)
{
	int 	opt;
#if ! defined(__uClinux__)
	int 	doFork = TRUE;
#endif

	char 	*p;

	/* do normal option parsing */
	while ((opt = getopt(argc, argv, "pnO:R:LCF:ms:S:f:P:H:Ma:t:")) > 0) 
	{
		switch (opt) 
		{
			case 'p':
				proclog = TRUE;
				break;
#if ! defined(__uClinux__)
			case 'n':
				doFork = FALSE;
				break;
#endif
			case 'O':
				logFilePath = optarg;
				break;
#ifdef REMOTE_LOG
			case 'R':
				RemoteHost = optarg;
				if ( (p = strchr(RemoteHost, ':')))
				{
					RemotePort = atoi(p+1);
					*p = '\0';
				}
				doRemoteLog = TRUE;
				break;
			case 'L':
				local_logging = TRUE;
				break;
#endif
#ifdef IPC_SYSLOG
			case 'C':
				circular_logging = TRUE;
				break;
#endif
#ifdef ASYSLOG_NAMES
			case 'F':
				/* joanw add for Alpha System Log Message filter. 2004.04.22
				 *   	0x01: 00001 system activity
				 * 	 	0x02: 00010 debug information
				 * 	 	0x04: 00100 attacks
				 * 	 	0x08: 01000 dropped packet
				 * 	 	0x10: 10000 notice
				 ************************************************************/
				if(0 == strcmp("sysact", optarg))
					asyslogflag |= 0x01;
				else if(0 == strcmp("debug", optarg))
					asyslogflag |= 0x02;
				else if(0 == strcmp("attack", optarg))
					asyslogflag |= 0x04;
				else if(0 == strcmp("drop", optarg))
					asyslogflag |= 0x08;
				else if(0 == strcmp("notice", optarg))
					asyslogflag |= 0x10;
				else
					show_usage(argv);

				/* joanw debug */
				dbprint("asyslogflag=%d\n", asyslogflag);
				break;
#endif
			/* joanw add for mail log message 2004.04.14 */
			case 'm':
				mail_log = 1;
				break;
			case 's':
				mail_subject = optarg;
				break;
			case 'S':
				mail_host = optarg;
				break;
			case 'P':
				mail_port = optarg;
				break;
			case 'H':
				src_host = optarg;
				strcat(mail_from, optarg);
				strcat(mail_from, ".com");
				break;
			case 'M':
				mime_style = 1;
				break;
			case 'a':
				email_addr = optarg;
				break;
			case 't':
				tlog = 1;
				log_path = optarg;
				break;
			default:
				show_usage(argv);
				break;
		}
	}

#ifdef REMOTE_LOG
	/* If they have not specified remote logging, then log locally */
	if (doRemoteLog == FALSE)
		local_logging = TRUE;
#endif

	/* Store away localhost's name before the fork */
	gethostname(LocalHostName, sizeof(LocalHostName));
	if ((p = strchr(LocalHostName, '.'))) 
	{
		*p++ = '\0';
	}

	umask(0);

#if 0 // ! defined(__uClinux__)
	if (doFork == TRUE) 
	{
		if (daemon(0, 1) < 0)
			perror_msg_and_die("daemon");
	}
#endif
	doSyslogd();

	return EXIT_SUCCESS;
}
/*
Local Variables
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
