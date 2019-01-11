/* vi: set sw=4 ts=4: */
/*
 * Mini klogd implementation for busybox
 *
 * Copyright (C) 2001 by Gennady Feldman <gfeldman@cachier.com>.
 * Changes: Made this a standalone busybox module which uses standalone
 * 					syslog() client interface.
 *
 * Copyright (C) 1999,2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 1999,2000,2001 by Erik Andersen <andersee@debian.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2000 by Gennady Feldman <gfeldman@mail.com>
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
#include <signal.h> /* for our signal() handlers */
#include <string.h> /* strncpy() */
#include <errno.h>  /* errno and friends */
#include <unistd.h>
#include <ctype.h>
#include <sys/syslog.h>
// joanw add for port trigger
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#if __GNU_LIBRARY__ < 5
# ifdef __alpha__
#   define klogctl syslog
# endif
#else
# include <sys/klog.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static char * LanIf = "";
static char * WanIf = "";
// add for port trigger
static char * o_sockname = "/var/run/portt.unixsocket";
static char * ptr_path = "/var/porttrigger/";

// port trigger
static int sendto_unsock(const char * unsock, const char * message)
{
	int sock, ret;
	struct sockaddr_un sunix;

	printf("sendto_unsock(%s, %s)\n", unsock, message);

	if (!message) return 0;

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) return -1;

	memset(&sunix, 0, sizeof(struct sockaddr_un));
	sunix.sun_family = AF_UNIX;
	strncpy(sunix.sun_path, unsock, sizeof(sunix.sun_path));

	ret = sendto(sock, (const void *)message, strlen(message)+1, 0,
			(struct sockaddr *)&sunix, sizeof(sunix.sun_family) + strlen(sunix.sun_path));
	close(sock);
	return ret;
}

static void klogd_signal(int sig)
{
	klogctl(7, NULL, 0);
	klogctl(0, 0, 0);
	exit(TRUE);
}

static void doKlogd (void)
{
	//int priority = LOG_INFO;
	char log_buffer[4096], msg[256],tmp[256], tmp1[256];
	char protocol[10], reason[50], src_ip[25], src_port[6], dst_ip[25], dst_port[6];
	int n, lastc;
	char *start=NULL, *interface=NULL;
	int facility=8;
	// add for port trigger
	FILE * fp;

	/* Set up sig handlers */
	signal(SIGINT, klogd_signal);
	signal(SIGKILL, klogd_signal);
	signal(SIGTERM, klogd_signal);
	signal(SIGHUP, SIG_IGN);

	/* "Open the log. Currently a NOP." */
	klogctl(1, NULL, 0);

	while (1)
	{
		/* Use kernel syscalls */
		memset(log_buffer,	'\0', sizeof(log_buffer));
		memset(msg,			'\0', sizeof(msg));
		memset(protocol,	'\0', sizeof(protocol));
		memset(reason,		'\0', sizeof(reason));
		memset(src_ip,		'\0', sizeof(src_ip));
		memset(src_port,	'\0', sizeof(src_port));
		memset(dst_ip,		'\0', sizeof(dst_ip));
		memset(dst_port,	'\0', sizeof(dst_port));

		n = klogctl(2, log_buffer, sizeof(log_buffer));
		if (n < 0)
		{
			//char message[80];

			if (errno == EINTR) continue;
#if 0
			snprintf(message, 79, "klogd: Error return from sys_sycall: %d - %s.\n", 
												errno, strerror(errno));
			syslog_msg(LOG_DAEMON, LOG_SYSLOG | LOG_ERR, message);
#endif
			exit(1);
		}

		/* klogctl buffer parsing modelled after code in dmesg.c */
		start = &log_buffer[0];
		lastc = '\0';
		// joanw add on 2004.06.14
		if (strcspn(start, ">") > 0)	start += strcspn(start, ">")+1;
		if (strncmp(start, "ATT:", 4)==0)
		{
			facility = LOG_MAKEPRI(26, 1);
			
			// get the reason
			start +=4;
			// joanw fixed for new log rules.
			memcpy(reason, start, strcspn(start, ":"));
			// get the interface
			start = strstr(start, "IN=");
			interface = start + 3;
			if		(strncmp(interface, WanIf, (strcspn(start, " ")-4))==0)	interface = "WAN";
			else if (strncmp(interface, LanIf, (strcspn(start, " ")-4))==0)	interface = "LAN";
			else 															interface = "N/A";
			
			// get the souce ip
			start = strstr(start, "SRC=");
			if (start)	memcpy(src_ip, start+4, (strcspn(start, " ")-4));

			// joanw fixed for new log rules.
#ifndef LOGNUM
			snprintf(msg, 255, "ATTACK Detected: %s attack from %s (ip:%s) detected. Packet dropped.",
					reason, interface, src_ip);
#else
			// message format: code[name|type][interface][src_ip]
			// ex: ATT:001[Xmas][WAN][1.2.3.4]
			// (reason = code[name|type])
			// The following is the same of the log message read from kernel. 
			// ATT:001[Xmas]:IN=vlan2 OUT= MAC=00:62:40:a1:05:02:00:11:2f:f8:e2:26:08:00:45:00:00:30 SRC=192.168.20.13 DST=192.168.20.14 LEN=48 TOS=0x00 PREC=0x00 TTL=128 ID=861
			snprintf(msg, 255, "ATT:%s[%s][%s]", reason, interface, src_ip);
#endif
			syslog(facility, "%s", msg);
		}
		else if(strncmp(start, "DRP:", 4)==0)
		{
			facility = LOG_MAKEPRI(27, 1);
			// get the reason
			start += 4;
			// joanw fixed for new log rules.
			memcpy(reason, start, strcspn(start, ":"));
			//memcpy(reason, start, strcspn(start, " "));

			// get the interface
			start = strstr(start, "IN=");
			interface = start + 3;
			if 		(strncmp(interface, WanIf, (strcspn(start, " ")-4))==0)	interface = "WAN";
			else if (strncmp(interface, LanIf, (strcspn(start, " ")-4))==0)	interface = "LAN";
			else															interface = "N/A";
			
			// get the souce ip
			start = strstr(start, "SRC=");
			if (start)	memcpy(src_ip, start+4, (strcspn(start, " ")-4));

			// get the destination ip
			start = strstr(start, "DST=");
			if (start)	memcpy(dst_ip, start+4, (strcspn(start, " ")-4));

			// get the protocol
			start = strstr(start, "PROTO=");
			if (start)	memcpy(protocol, start+6, (strcspn(start, " ")-6));

			if((strncmp(protocol, "TCP",3)==0) || (strncmp(protocol, "UDP", 3)==0))
			{
				// get the source port
				start = strstr(start, "SPT=");
				if (start)	memcpy(src_port, start+4, (strcspn(start, " ")-4));
			
				// get the destination port
				start = strstr(start, "DPT=");
				if (start)	memcpy(dst_port, start+4, (strcspn(start, " ")-4));
				// joanw fixed for new log rules.
				// message format: code[protocol][interface][src_ip][src_port][dst_ip][dst_port]
				// ex: DRP:001[TCP][WAN][1.2.3.4][1234][2.3.4.5][80]
				// The following is the same of the log message read from kernel. 
				// DRP:001:IN=vlan2 OUT= MAC=00:62:40:a1:05:02:00:11:2f:f8:e2:26:08:00:45:00:00:30 SRC=192.168.20.13 DST=192.168.20.14 LEN=48 TOS=0x00 PREC=0x00 TTL=128 ID=861
#ifndef LOGNUM
				snprintf(msg, 255, "DROP: %s.  Drop %s Packet from %s, src:%s:%s, dst:%s:%s.",
							reason, protocol, interface, src_ip, src_port, dst_ip, dst_port);
#else
				snprintf(msg, 255, "DRP:%s[%s][%s][%s][%s][%s][%s]",
							reason, protocol, interface, src_ip, src_port, dst_ip, dst_port);
#endif
			}
			else
			{
				// joanw fixed for new log rules.
				// message format: code[protocol][interface][src_ip][src_port][dst_ip][dst_port]
				// ex: DRP:001[TCP][WAN][1.2.3.4][1234][2.3.4.5][80]
#ifndef LOGNUM
				snprintf(msg, 255, "DROP: %s.  Drop %s Packet from %s, src:%s, dst:%s.",
							reason, protocol, interface, src_ip, dst_ip);
#else
				snprintf(msg, 255, "DRP:%s[%s][%s][%s][][%s][]",
							reason, protocol, interface, src_ip, dst_ip);
#endif
			}

			syslog(facility, "%s", msg);
				
		}
		else if(strncmp(start, "PTR:", 4)==0)
		{
			start += 4;
			memcpy(reason, start, strcspn(start, ":"));
			
			// get the souce ip
			start = strstr(start, "SRC=");
			if (start)	memcpy(src_ip, start+4, (strcspn(start, " ")-4));

			snprintf(msg, 255, "cat %s%d", ptr_path, atoi(reason));
			fp=popen(msg, "r");
			if(fp)
			{
				memset(msg,'\0', sizeof(msg));
				memset(tmp,'\0', sizeof(tmp));
				memset(tmp1,'\0', sizeof(tmp1));

				fgets(tmp, sizeof(tmp), fp);
				memcpy(tmp1, tmp, strcspn(tmp, "\n"));

				// msg format: "file_index,src_ip,file_msg"
				snprintf(msg, 255, "%d,%s,%s", atoi(reason), src_ip, tmp1);
				sendto_unsock(o_sockname, msg);
				pclose(fp);
			}
		}
	}
}

static void show_usage(void)
{
	printf("Usage: klogd [options]\n");
	printf("  -w interface      wan interface.\n");
	printf("  -l interface      lan interface.\n");
}

int main(int argc, char **argv)
{
	/* no options, no getopt */
	int opt;

	/* do normal option parsing */
	while ((opt = getopt(argc, argv, "l:w:")) > 0)
	{
		switch (opt)
		{
		case 'l': LanIf = optarg; break;
		case 'w': WanIf = optarg; break;
		default:  show_usage();   break;
		}
	}

	doKlogd();
	return EXIT_SUCCESS;
}
