/* vi: set sw=4 ts=4: */
///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2003 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/************************************************************************
* Purpose: This file implements the functionality and utility functions
* used by the Miniserver module.
************************************************************************/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "unixutil.h"

#include "sys/un.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ssdplib.h"

#include "util.h"
#include "miniserver.h"
#include "httpreadwrite.h"
#include "statcodes.h"
#include "upnp.h"
#include "upnpapi.h"

#include <signal.h>

#include <net/if.h>

#define APPLICATION_LISTENING_PORT 49152
//#define DEFAULT_WEB_DIR "./web"
#define _VARRUN_PATH "/var/"

extern int UpnpSdkInit;
extern char LOCAL_HOST[180];

UpnpDevice_Handle device_handle = -1;

int default_advr_expire = 1800;

struct mserv_request_t
{
	int connfd;                 // connection handle
	struct in_addr foreign_ip_addr;
	unsigned short foreign_ip_port;
};

typedef enum { MSERV_IDLE, MSERV_RUNNING, MSERV_STOPPING } MiniServerState;

unsigned short miniStopSockPort;

////////////////////////////////////////////////////////////////////////////
// module vars
static MiniServerCallback	gGetCallback = NULL;
static MiniServerCallback	gSoapCallback = NULL;
static MiniServerCallback	gGenaCallback = NULL;
static MiniServerState		gMServState = MSERV_IDLE;

static SignalCallback	gSignalCallback = NULL;

/************************************************************************
*	Function :	SetHTTPGetCallback
*
*	Parameters :
*		MiniServerCallback callback ; - HTTP Callback to be invoked 
*
*	Description :	Set HTTP Get Callback
*
*	Return :	void
*
*	Note :
************************************************************************/
void
SetHTTPGetCallback( MiniServerCallback callback )
{
    gGetCallback = callback;
}

/************************************************************************
*	Function :	SetSoapCallback
*
*	Parameters :
*		MiniServerCallback callback ; - SOAP Callback to be invoked 
*
*	Description :	Set SOAP Callback
*
*	Return :	void
*
*	Note :
************************************************************************/
void
SetSoapCallback( MiniServerCallback callback )
{
    gSoapCallback = callback;
}

/************************************************************************
*	Function :	SetGenaCallback
*
*	Parameters :
*		MiniServerCallback callback ; - GENA Callback to be invoked
*
*	Description :	Set GENA Callback
*
*	Return :	void
*
*	Note :
************************************************************************/
void SetGenaCallback( MiniServerCallback callback )
{
    gGenaCallback = callback;
}

void SetSignalCallback(SignalCallback callback)
{
	gSignalCallback = callback;
}

/************************************************************************
*	Function :	dispatch_request
*
*	Parameters :
*		IN SOCKINFO *info ;		 Socket Information object.
*		http_parser_t* hparser ; HTTP parser object.
*
*	Description :	Based on the type pf message, appropriate callback 
*		is issued
*
*	Return : int ;
*		0 - On Success
*		HTTP_INTERNAL_SERVER_ERROR - Callback is NULL
*
*	Note :
************************************************************************/
static int dispatch_request(IN SOCKINFO * info, http_parser_t * hparser)
{
	MiniServerCallback callback;

	switch (hparser->msg.method)
	{
		//Soap Call
		case SOAPMETHOD_POST:
		case HTTPMETHOD_MPOST:
			dtrace("miniserver %d: got SOAP msg (%d)\n", info->socket, hparser->msg.method);
			callback = gSoapCallback;
			break;

			//Gena Call
		case HTTPMETHOD_NOTIFY:
		case HTTPMETHOD_SUBSCRIBE:
		case HTTPMETHOD_UNSUBSCRIBE:
			dtrace("miniserver %d: got GENA msg (%d)\n", info->socket, hparser->msg.method);
			callback = gGenaCallback;
			break;

			//HTTP server call
		case HTTPMETHOD_GET:
		case HTTPMETHOD_POST:
		case HTTPMETHOD_HEAD:
		case HTTPMETHOD_SIMPLEGET:
			dtrace("miniserver %d: got HTPP msg (%d)\n", info->socket, hparser->msg.method);
			callback = gGetCallback;
			break;

		default:
			callback = NULL;
	}

	if (callback == NULL) return HTTP_INTERNAL_SERVER_ERROR;

	callback(hparser, &hparser->msg, info);
	return 0;
}

/************************************************************************
*	Function :	handle_error
*
*	Parameters :
*		
*		IN SOCKINFO *info ;		Socket Inforamtion Object
*		int http_error_code ;	HTTP Error Code
*		int major ;				Major Version Number
*		int minor ;				Minor Version Number
*
*	Description :	Send Error Message
*
*	Return : void;
*
*	Note :
************************************************************************/
static XINLINE void handle_error(
		IN SOCKINFO * info,
		int http_error_code,
		int major,
		int minor
		)
{
	http_SendStatusResponse( info, http_error_code, major, minor );
}

/************************************************************************
*	Function :	free_handle_request_arg
*
*	Parameters :
*		void *args ; Request Message to be freed
*
*	Description :	Free memory assigned for handling request and unitial-
*	-ize socket functionality
*
*	Return :	void
*
*	Note :
************************************************************************/
/*
static void
free_handle_request_arg( void *args )
{
    struct mserv_request_t *request = ( struct mserv_request_t * )args;

    shutdown( request->connfd, SD_BOTH );
    UpnpCloseSocket( request->connfd );
    free( request );
}
*/
#if 0
void handle_request_down(void)
{
	int http_error_code;
	int ret_code;
	//int major = 1;
	//int minor = 1;
	http_parser_t parser;
	http_message_t *hmsg = NULL;
	int timeout = HTTP_DEFAULT_TIMEOUT;

	//parser_request_init( &parser ); ////LEAK_FIX_MK
	hmsg = &parser.msg;

	// read
	ret_code = http_RecvMessage_down(&parser, HTTPMETHOD_UNKNOWN, &timeout, &http_error_code);
	if( ret_code != 0 ) {
		// goto error_handler;
	}

	// dispatch
	http_error_code = dispatch_request_down(&parser);
	if (http_error_code != 0)
	{
		//goto error_handler;
	}

	http_error_code = 0;

}
#endif


/************************************************************************
 *	Function :	handle_request
 *
 *	Parameters :
 *		void *args ;	Request Message to be handled
 *
 *	Description :	Receive the request and dispatch it for handling
 *
 *	Return :	void
 *
 *	Note :
 ************************************************************************/
static void handle_request(void *args)
{
	SOCKINFO info;
	int http_error_code;
	int ret_code;
	int major = 1;
	int minor = 1;
	http_parser_t parser;
	http_message_t *hmsg = NULL;
	int timeout = HTTP_DEFAULT_TIMEOUT;
	struct mserv_request_t *request = ( struct mserv_request_t * )args;
	int connfd = request->connfd;

	dtrace("\n*************************************\n");
	dtrace("miniserver %d: READING\n", connfd);
	//parser_request_init( &parser ); ////LEAK_FIX_MK
	hmsg = &parser.msg;

	if (sock_init_with_ip(&info, connfd, request->foreign_ip_addr,
				request->foreign_ip_port) != UPNP_E_SUCCESS)
	{
		free(request);
		httpmsg_destroy(hmsg);
		return;
	}
	// read
	ret_code = http_RecvMessage(&info, &parser, HTTPMETHOD_UNKNOWN,
			&timeout, &http_error_code);
	if (ret_code != 0)
	{
		goto error_handler;
	}

	dtrace("miniserver %d: PROCESSING...\n", connfd);
	// dispatch
	http_error_code = dispatch_request(&info, &parser);
	if (http_error_code != 0)
	{
		goto error_handler;
	}

	http_error_code = 0;

error_handler:
	if (http_error_code > 0)
	{
		if (hmsg)
		{
			major = hmsg->major_version;
			minor = hmsg->minor_version;
		}
		handle_error(&info, http_error_code, major, minor);
	}

	dtrace("miniserver %d: COMPLETE\n", connfd);
	dtrace("***********************************************\n");
	sock_destroy(&info, SD_BOTH); //should shutdown completely

	httpmsg_destroy(hmsg);
	free(request);
	return;
}

/************************************************************************
*	Function :	schedule_request_job
*
*	Parameters :
*		IN int connfd ;	Socket Descriptor on which connection is accepted
*		IN struct sockaddr_in* clientAddr ;	Clients Address information
*
*	Description :	Initilize the thread pool to handle a request.
*		Sets priority for the job and adds the job to the thread pool
*
*
*	Return :	void
*
*	Note :
************************************************************************/
static XINLINE void schedule_request_job(
		IN int connfd, IN struct sockaddr_in *clientAddr )
{
	struct mserv_request_t *request;

	request = (struct mserv_request_t *)malloc(sizeof(struct mserv_request_t));
	if (request == NULL)
	{
		dtrace("mserv %d: out of memory\n", connfd);
		shutdown( request->connfd, SD_BOTH );
		UpnpCloseSocket( connfd );
		return;
	}

	request->connfd = connfd;
	request->foreign_ip_addr = clientAddr->sin_addr;
	request->foreign_ip_port = ntohs( clientAddr->sin_port );
	handle_request(( void * )request);

#if 0
	TPJobInit( &job, ( start_routine ) handle_request, ( void * )request );
	TPJobSetFreeFunction( &job, free_handle_request_arg );
	TPJobSetPriority( &job, MED_PRIORITY );

	if( ThreadPoolAdd( &gRecvThreadPool, &job, NULL ) != 0 ) {
		DBGONLY( UpnpPrintf
				( UPNP_INFO, MSERV, __FILE__, __LINE__,
				  "mserv %d: cannot schedule request\n", connfd );
			   )
			free( request );
		shutdown( connfd, SD_BOTH );
		UpnpCloseSocket( connfd );
		return;
	}
#endif
}

int alarm_flag = 0;

void handlertimer()
{
	alarm_flag = 1;
	return;
}
//     signal(SIGALRM, handlertimer);
#if 0
void advertise()
{
	alarm_flag = 0;
	MYDBG(printf("hi\n");)
	signal(SIGALRM, advertise);
	alarm(5);
	//return;
     //UpnpSendAdvertisement(device_handle, default_advr_expire);
  //   alarm(0); 
}
#endif

static int signal_pipe[2];
static void signal_handler(int sig)
{
	dtrace("signal = %d\n", sig);
	switch (sig)
	{
	case SIGUSR1:	dtrace("SIGUSR1\n"); break;
	case SIGTERM:	dtrace("SIGTERM\n"); break;
	case SIGINT:	dtrace("SIGINT\n"); break;
	case SIGQUIT:	dtrace("SIGQUIT\n"); break;
	case SIGABRT:	dtrace("SIGABRT\n"); break;
	default:		dtrace("UNKOWN\n"); break;
	}
	if (send(signal_pipe[1], &sig, sizeof(sig), MSG_DONTWAIT) < 0)
	{
		dtrace("Cound not send signal: %d\n", strerror(errno));
	}
}


/************************************************************************/

struct schedule_callback
{
	struct schedule_callback * next;
	void (*func)(void *);
	void * param;
};

static struct schedule_callback * callback_list = NULL;

void miniserv_add_callback(void (*func)(void *), void * param)
{
	struct schedule_callback * entry;
	struct schedule_callback * list;

	entry = (struct schedule_callback *)malloc(sizeof(struct schedule_callback));
	if (entry)
	{
		entry->func = func;
		entry->param = param;
		entry->next = NULL;

		dtrace("callback: add entry:0x%x, func:0x%x, param:0x%x\n", entry, func, param);
		
		if (callback_list)
		{
			list = callback_list;
			while (list->next) list = list->next;
			list->next = entry;
		}
		else
		{
			callback_list = entry;
		}
	}
}

static void call_callback(void)
{
	struct schedule_callback * entry;

	while (callback_list)
	{
		entry = callback_list;
		callback_list = entry->next;
		if (entry->func)
		{
			dtrace("callback: process entry:0x%x, func:0x%x, param:0x%x\n", entry, entry->func, entry->param);
			entry->func(entry->param);
		}
		free(entry);
	}
}

static void destroy_callback_list(void)
{
	struct schedule_callback * entry;
	while (callback_list)
	{
		entry = callback_list;
		callback_list = entry->next;
		free(entry);
	}
}

/************************************************************************/


/************************************************************************
*	Function :	RunMiniServer
*
*	Parameters :
*		MiniServerSockArray *miniSock ;	Socket Array
*
*	Description :	Function runs the miniserver. The MiniServer accepts a 
*		new request and schedules a thread to handle the new request.
*		Checks for socket state and invokes appropriate read and shutdown 
*		actions for the Miniserver and SSDP sockets 
*
*	Return :	void
*
*	Note :
************************************************************************/
void RunMiniServer(MiniServerSockArray * miniSock)
{
	struct sockaddr_in clientAddr;
	struct sockaddr_un  clientAddr_stop;
	int clientLen,stop_break=0;
	SOCKET miniServSock, connectHnd,connectHnd_stop;
	SOCKET miniServStopSock;
	SOCKET ssdpSock;
#ifdef INCLUDE_CLIENT_APIS
	SOCKET ssdpReqSock;
#endif
	fd_set expSet;
	fd_set rdSet;
	unsigned int maxMiniSock;
	int byteReceived;
	char requestBuf[256];

	time_t delay_pre=0, delay_post=0;
	struct timeval timeout;
	int result,time_in = 1797,one=0;
	int sig;

	dtrace("RunMiniServer >>>>\n");
	socketpair(AF_UNIX, SOCK_STREAM, 0, signal_pipe);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT,  signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGABRT, signal_handler);
	dtrace("SIGUSR1 = %d\n", SIGUSR1);

	UpnpSendAdvertisement(device_handle, default_advr_expire);

	miniServSock = miniSock->miniServerSock;
	miniServStopSock = miniSock->miniServerStopSock;

	ssdpSock = miniSock->ssdpSock;

#ifdef INCLUDE_CLIENT_APIS
	ssdpReqSock = miniSock->ssdpReqSock;
#endif

	gMServState = MSERV_RUNNING;
	maxMiniSock = max(miniServSock, miniServStopSock);
	maxMiniSock = max(maxMiniSock, (SOCKET)(ssdpSock));

#ifdef INCLUDE_CLIENT_APIS
	maxMiniSock = max(maxMiniSock, (SOCKET)(ssdpReqSock));
#endif

	maxMiniSock = max(maxMiniSock, signal_pipe[0]);
	++maxMiniSock;

	dtrace("\nEnter RunMiniServer loop .........\n\n");
	while (TRUE)
	{
		delay_post = time((time_t)0);
		if (one==0) one++;
		else time_in = time_in - (int)(delay_post-delay_pre);
		delay_pre = delay_post;

		if (time_in < 0) time_in=0;
		timeout.tv_sec = time_in;
		timeout.tv_usec = 0;

		FD_ZERO(&rdSet);
		FD_ZERO(&expSet);
		FD_SET(miniServStopSock, &expSet);
		FD_SET(miniServSock, &rdSet);
		FD_SET(miniServStopSock, &rdSet);
		FD_SET(ssdpSock, &rdSet);
#ifdef INCLUDE_CLIENT_APIS
		FD_SET(ssdpReqSock, &rdSet);
#endif
		FD_SET(signal_pipe[0], &rdSet);
		result = select(maxMiniSock, &rdSet, NULL, &expSet, &timeout);

		if (result == 0)
		{
			UpnpSendAdvertisement(device_handle, default_advr_expire);
			time_in=1797;
			continue;
		}
		else if (result < 0)
		{
			dtrace("Error in select call !!!\n");
			continue;
		}

		if (FD_ISSET(signal_pipe[0], &rdSet))
		{
			dtrace("signal_pipe is SET\n");
			if (read(signal_pipe[0], &sig, sizeof(sig)) < 0)
			{
				dtrace("Can not read from signal_pipe !!!!\n");
			}
			else
			{
				if (sig == SIGUSR1 || sig == SIGUSR2)
				{
					if (gSignalCallback) gSignalCallback(sig, NULL);
				}
				else
				{
					dtrace("Got SIGTERM !!!!\n");
					break;
				}
			}
		}
		
		if (FD_ISSET(miniServSock, &rdSet))
		{
			clientLen = sizeof(struct sockaddr_in);
			connectHnd = accept(miniServSock, (struct sockaddr *)&clientAddr, &clientLen);
			if (connectHnd == UPNP_INVALID_SOCKET)
			{
				dtrace("miniserver: Error in accepting connection\n");
				continue;
			}
			schedule_request_job(connectHnd, &clientAddr);
		}
#ifdef INCLUDE_CLIENT_APIS
		//ssdp
		if (FD_ISSET(ssdpReqSock, &rdSet)) readFromSSDPSocket(ssdpReqSock);
#endif

		if (FD_ISSET(ssdpSock, &rdSet))
		{
			readFromSSDPSocket(ssdpSock);
		}

		if (FD_ISSET(miniServStopSock, &rdSet))
		{
			clientLen = sizeof(clientAddr_stop);
			connectHnd_stop = accept(miniServStopSock,
					(struct sockaddr *)&clientAddr_stop, &clientLen);

			byteReceived = recvfrom(connectHnd_stop, requestBuf, 25, 0,
					(struct sockaddr *)&clientAddr_stop, &clientLen);
			if (byteReceived > 0)
			{
				requestBuf[byteReceived] = '\0';

				dtrace("Received response !!!  %s From host %s \n", requestBuf, inet_ntoa(clientAddr.sin_addr));

				if (NULL != strstr(requestBuf, "ShutDown"))
				{
					gMServState = MSERV_STOPPING;

#if 0
					shutdown( miniServSock, SD_BOTH );
					UpnpCloseSocket( miniServSock );
#endif

					stop_break=1;
					break;

				}				
			}
		}
		if (stop_break==1) break;
		call_callback();
	}

	destroy_callback_list();
	printf("stoped\n");
	return;

}

/************************************************************************
*	Function :	get_port
*
*	Parameters :
*		int sockfd ; Socket Descriptor 
*
*	Description :	Returns port to which socket, sockfd, is bound.
*
*	Return :	int, 
*		-1 on error; check errno
*		 > 0 means port number
*
*	Note :
************************************************************************/
static int get_port(int sockfd)
{
	struct sockaddr_in sockinfo;
	int len;
	int code;
	int port;

	len = sizeof(struct sockaddr_in);
	code = getsockname(sockfd, (struct sockaddr *)&sockinfo, &len);
	if (code == -1) return -1;

	port = ntohs(sockinfo.sin_port);

#ifdef DEBUG
	UpnpPrintf(UPNP_INFO,MSERV,__FILE__,__LINE__, "sockfd = %d, .... port = %d\n", sockfd, port);
#endif

	return port;
}

/************************************************************************
*	Function :	get_miniserver_sockets
*
*	Parameters :
*		MiniServerSockArray *out ;	Socket Array
*		unsigned short listen_port ; port on which the server is listening 
*									for incoming connections	
*
*	Description :	Creates a STREAM socket, binds to INADDR_ANY and 
*		listens for incoming connecttions. Returns the actual port which 
*		the sockets sub-system returned. 
*		Also creates a DGRAM socket, binds to the loop back address and 
*		returns the port allocated by the socket sub-system.
*
*	Return :	int : 
*		UPNP_E_OUTOF_SOCKET - Failed to create a socket
*		UPNP_E_SOCKET_BIND - Bind() failed
*		UPNP_E_LISTEN	- Listen() failed	
*		UPNP_E_INTERNAL_ERROR - Port returned by the socket layer is < 0
*		UPNP_E_SUCCESS	- Success
*		
*	Note :
************************************************************************/
static int get_miniserver_sockets(
		MiniServerSockArray * out,
		const char * ifname,
		unsigned short listen_port
		)
{
	struct sockaddr_in serverAddr;
	struct sockaddr_un serverAddr_stop;
	int listenfd;
	int success;
	unsigned short actual_port;
	int reuseaddr_on = 1;
	int sockError = UPNP_E_SUCCESS;
#if 0
	int errCode = 0;
#endif
	int miniServerStopSock;
	struct ifreq interface;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd <= 0)
	{
		dtrace("error creating socket !!!\n");
		return UPNP_E_OUTOF_SOCKET; // error creating socket
	}

	/* As per the IANA specifications for the use of ports by applications
	 * override the listen port passed in with the first available */
	if (listen_port < APPLICATION_LISTENING_PORT)
	{
		dtrace("Adjust the listen port from %d to %d\n", listen_port, APPLICATION_LISTENING_PORT);
		listen_port = APPLICATION_LISTENING_PORT;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (ifname)
	{
		strncpy(interface.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);
		setsockopt(listenfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface));
	}

#if 0
	/* Getting away with implementation of re-using address:port and instead
	 * choosing to increment port numbers.
	 * Keeping the re-use address code as an optional behaviour that can be
	 * turned on if necessary.
	 * TURN ON the reuseaddr_on option to use the option. */
	if (reuseaddr_on)
	{	/* THIS IS ALLOWS US TO BIND AGAIN IMMEDIATELY
		 * AFTER OUR SERVER HAS BEEN CLOSED
		 * THIS MAY CAUSE TCP TO BECOME LESS RELIABLE
		 * HOWEVER IT HAS BEEN SUGESTED FOR TCP SERVERS */
		sockError = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
				(const char *)&reuseaddr_on, sizeof(int));
		if (sockError == UPNP_SOCKETERROR)
		{
			dtrace("setsockopt fail !!! (%s)\n", strerror(errno));
			shutdown(listenfd, SD_BOTH);
			UpnpCloseSocket(listenfd);
			return UPNP_E_SOCKET_BIND;
		}

		serverAddr.sin_port = htons(listen_port);
		sockError = bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
	}
	else
	{
		do
		{
			serverAddr.sin_port = htons(listen_port++);
			sockError = bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
			if (sockError == UPNP_SOCKETERROR)
			{
				if (errno == EADDRINUSE) errCode = 1;
			} else
				errCode = 0;

		} while(errCode != 0);
	}

	if (sockError == UPNP_SOCKETERROR)
	{
		dtrace("bind fail (%s) !\n", strerror(errno));
		shutdown( listenfd, SD_BOTH );
		UpnpCloseSocket( listenfd );
		return UPNP_E_SOCKET_BIND;  // bind failed
	}
#else
	/* set reuseaddr flags */
	/* THIS IS ALLOWS US TO BIND AGAIN IMMEDIATELY
	 * AFTER OUR SERVER HAS BEEN CLOSED
	 * THIS MAY CAUSE TCP TO BECOME LESS RELIABLE
	 * HOWEVER IT HAS BEEN SUGESTED FOR TCP SERVERS */
	sockError = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
				(const char *)&reuseaddr_on, sizeof(int));
	if (sockError == UPNP_SOCKETERROR)
	{
		dtrace("setsockopt fail !!! (%s)\n", strerror(errno));
		shutdown(listenfd, SD_BOTH);
		UpnpCloseSocket(listenfd);
		return UPNP_E_SOCKET_BIND;
	}

	serverAddr.sin_port = htons(listen_port);

	if (1)
	{
		int retry=5;
		do
		{
			sockError = bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
			if (sockError != UPNP_SOCKETERROR) break;
			retry--;
			dtrace("binding fail, retry = %d\n", retry);
			sleep(2);
		} while (retry>0);

		if (sockError == UPNP_SOCKETERROR)
		{
			dtrace("bind fail (%s) !\n", strerror(errno));
			shutdown( listenfd, SD_BOTH );
			UpnpCloseSocket( listenfd );
			return UPNP_E_SOCKET_BIND;  // bind failed
		}
	}
#endif

	success = listen(listenfd, SOMAXCONN);
	if (success == UPNP_SOCKETERROR)
	{
		shutdown( listenfd, SD_BOTH );
		UpnpCloseSocket( listenfd );
		return UPNP_E_LISTEN;   // listen failed
	}

	actual_port = get_port(listenfd);
	if (actual_port <= 0)
	{
		shutdown( listenfd, SD_BOTH );
		UpnpCloseSocket( listenfd );
		return UPNP_E_INTERNAL_ERROR;
	}

	out->miniServerPort = actual_port;
	unlink(_VARRUN_PATH"miniServerStopSock");
	if ((miniServerStopSock = socket(AF_UNIX, SOCK_STREAM, 0)) == UPNP_INVALID_SOCKET)
	{
		dtrace("Error in socket operation !!!\n");
		shutdown( listenfd, SD_BOTH );
		UpnpCloseSocket( listenfd );
		return UPNP_E_OUTOF_SOCKET;
	}

	// bind to local socket
	// memset( ( char * )&serverAddr, 0, sizeof( struct sockaddr_in ) );
	serverAddr_stop.sun_family = AF_UNIX;
	strcpy(serverAddr_stop.sun_path,_VARRUN_PATH"miniServerStopSock");
	if (bind(miniServerStopSock, (struct sockaddr *)&serverAddr_stop, sizeof(serverAddr_stop))
			== UPNP_SOCKETERROR)
	{
		dtrace("Error in binding localhost!!!\n");
		shutdown( listenfd, SD_BOTH );
		UpnpCloseSocket( listenfd );
		shutdown( miniServerStopSock, SD_BOTH );
		UpnpCloseSocket( miniServerStopSock );
		return UPNP_E_SOCKET_BIND;
	}
	listen(miniServerStopSock,5);

	out->miniServerSock = listenfd;
	out->miniServerStopSock = miniServerStopSock;

	return UPNP_E_SUCCESS;
}

/************************************************************************
*	Function :	StartMiniServer
*
*	Parameters :
*		unsigned short listen_port ; Port on which the server listens for 
*									incoming connections
*
*	Description :	Initialize the sockets functionality for the 
*		Miniserver. Initialize a thread pool job to run the MiniServer
*		and the job to the thread pool. If listen port is 0, port is 
*		dynamically picked
*
*		Use timer mechanism to start the MiniServer, failure to meet the 
*		allowed delay aborts the attempt to launch the MiniServer.
*
*	Return : int ;
*		Actual port socket is bound to - On Success: 
*		A negative number UPNP_E_XXX - On Error   			
*	Note :
************************************************************************/
int StartMiniServer(
		const char *	interface,
		unsigned short	listen_port,
		IN const char *	desc_doc_name,
		IN const char *	web_dir_path,
		IN Upnp_FunPtr	Fun,
		IN Upnp_InitPtr	Fun2
		)
{
	int ret = UPNP_E_SUCCESS;
	int success;
	char desc_doc_url[200];
	MiniServerSockArray * miniSocket;

	dtrace("StartMiniServer >>>> desc:%s, web:%s\n", desc_doc_name, web_dir_path);
	
	if (gMServState != MSERV_IDLE) return UPNP_E_INTERNAL_ERROR;   // miniserver running
	miniSocket = (MiniServerSockArray *)malloc(sizeof(MiniServerSockArray));
	if (miniSocket == NULL)
	{
		dtrace("StartMiniServer OUTOF_MEMORY!!!\n");
		return UPNP_E_OUTOF_MEMORY;
	}

	if ((success = get_miniserver_sockets(miniSocket, interface, listen_port))
			!= UPNP_E_SUCCESS)
	{
		dtrace("get_miniserver_sockets fail!\n");
		free(miniSocket);
		return success;
	}
	if ((success = get_ssdp_sockets(miniSocket)) != UPNP_E_SUCCESS)
	{
		dtrace("get_ssdp_sockets fail!!\n");
		shutdown( miniSocket->miniServerSock, SD_BOTH );
		UpnpCloseSocket( miniSocket->miniServerSock );
		shutdown( miniSocket->miniServerStopSock, SD_BOTH );
		UpnpCloseSocket( miniSocket->miniServerStopSock );
		free( miniSocket );
		return success;
	}

	LOCAL_PORT = miniSocket->miniServerPort;
#if EXCLUDE_WEB_SERVER == 0
	if ((ret = UpnpEnableWebserver(WEB_SERVER_ENABLED)) != UPNP_E_SUCCESS)
	{
		dtrace("UpnpEnableWebserver fail !!\n");
		UpnpFinish();
	}
#endif
	dassert(desc_doc_name);
	dassert(web_dir_path);

	printf("Specifying the webserver root directory -- %s\n", web_dir_path);
	if ((ret = UpnpSetWebServerRootDir(web_dir_path)) != UPNP_E_SUCCESS)
	{
		printf("Error specifying webserver root directory -- %s: %d\n", web_dir_path, ret);
		UpnpFinish();
	}


#if 0
	snprintf(desc_doc_url, 200, "http://%s:%d/%s.xml", LOCAL_HOST, LOCAL_PORT, desc_doc_name);
	printf("Registering the RootDevice\n\t with desc_doc_url: %s\n", desc_doc_url);
	if ((ret = UpnpRegisterRootDevice(desc_doc_url, Fun, &device_handle, &device_handle))
		!= UPNP_E_SUCCESS)
#else
	snprintf(desc_doc_url, 200, "%s/%s.xml", web_dir_path, desc_doc_name);
	if ((ret = UpnpRegisterRootDevice2(UPNPREG_FILENAME_DESC,
					desc_doc_url, 0, 1, Fun, &device_handle, &device_handle))
			!= UPNP_E_SUCCESS)
#endif
	{
		UpnpFinish();
	}

	if (Fun2)
	{
		snprintf(desc_doc_url, 200, "http://%s:%d/%s.xml", LOCAL_HOST, LOCAL_PORT, desc_doc_name);
		Fun2(desc_doc_url);
	}

	RunMiniServer((void*)miniSocket);

	UpnpFinish();
	shutdown( miniSocket->miniServerSock, SD_BOTH );
	UpnpCloseSocket( miniSocket->miniServerSock );
	shutdown( miniSocket->miniServerStopSock, SD_BOTH );
	UpnpCloseSocket( miniSocket->miniServerStopSock );

	if (miniSocket->ssdpSock)
	{
		shutdown(miniSocket->ssdpSock, SD_BOTH);
		UpnpCloseSocket(miniSocket->ssdpSock);
	}

#ifdef INCLUDE_CLIENT_APIS
	if (miniSocket->ssdpReqSock)
	{
		shutdown(miniSocket->ssdpReqSock, SD_BOTH);
		UpnpCloseSocket(miniSocket->ssdpReqSock);
	}
#endif
	free( miniSocket );

	gMServState = MSERV_IDLE;
	
	return miniSocket->miniServerPort;
}

#if 0
/************************************************************************
*	Function :	StopMiniServer
*
*	Parameters :
*		void ;	
*
*	Description :	Stop and Shutdown the MiniServer and free socket 
*		resources.
*
*	Return : int ;
*		Always returns 0 
*
*	Note :
************************************************************************/
int StopMiniServer(void)
{
	int socklen = sizeof(struct sockaddr_in), sock;
	struct sockaddr_in ssdpAddr;
	char buf[256] = "ShutDown";
	int bufLen = strlen(buf);

	if (gMServState == MSERV_RUNNING) gMServState = MSERV_STOPPING;
	else return 0;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == UPNP_INVALID_SOCKET)
	{
#ifdef DEBUG
		UpnpPrintf(UPNP_INFO,SSDP,__FILE__,__LINE__,"SSDP_SERVER:StopSSDPServer: Error in socket operation !!!\n");
#endif
		return 0;
	}

	while (gMServState != MSERV_IDLE)
	{
		ssdpAddr.sin_family = AF_INET;
		ssdpAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		ssdpAddr.sin_port = htons(miniStopSockPort);
		sendto(sock, buf, bufLen, 0, ( struct sockaddr * )&ssdpAddr, socklen);
		usleep(1000);
		if (gMServState == MSERV_IDLE) break;
		isleep(1);
	}
	shutdown(sock, SD_BOTH);
	UpnpCloseSocket(sock);
	return 0;
}
#endif
