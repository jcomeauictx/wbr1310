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

#include "config.h"
#if EXCLUDE_SSDP == 0

#include "membuffer.h"
#include "ssdplib.h"
#include <stdio.h>
#include "miniserver.h"

#include "upnpapi.h"
#include "httpparser.h"
#include "httpreadwrite.h"

#define MAX_TIME_TOREAD  45

#ifdef INCLUDE_CLIENT_APIS
SOCKET gSsdpReqSocket = 0;
#endif

void RequestHandler(void);
Event ErrotEvt;

enum Listener { Idle, Stopping, Running };

unsigned short ssdpStopPort;

struct SSDPSockArray
{
	int ssdpSock;		/* socket for incoming advertisments and search requests */
#ifdef INCLUDE_CLIENT_APIS
	int ssdpReqSock;	/* socket for sending search requests and receiving search replies */
#endif
};

#ifdef INCLUDE_DEVICE_APIS
#if EXCLUDE_SSDP == 0

/************************************************************************
* Function : AdvertiseAndReply									
*																	
* Parameters:														
*	IN int AdFlag: -1 = Send shutdown, 0 = send reply, 
*					1 = Send Advertisement
*	IN UpnpDevice_Handle Hnd: Device handle
*	IN enum SsdpSearchType SearchType:Search type for sending replies
*	IN struct sockaddr_in *DestAddr:Destination address
*   IN char *DeviceType:Device type
*	IN char *DeviceUDN:Device UDN
*   IN char *ServiceType:Service type
*	IN int Exp:Advertisement age
*
* Description:														
*	This function sends SSDP advertisements, replies and shutdown messages.
*
* Returns: int
*	UPNP_E_SUCCESS if successful else appropriate error
***************************************************************************/
int AdvertiseAndReply(
		IN int AdFlag,
		IN UpnpDevice_Handle Hnd,
		IN enum SsdpSearchType SearchType,
		IN struct sockaddr_in *DestAddr,
		IN char *DeviceType,
		IN char *DeviceUDN,
		IN char *ServiceType,
		int Exp
		)
{
	int i, j;
	int defaultExp = DEFAULT_MAXAGE;
	struct Handle_Info *SInfo = NULL;
	char UDNstr[100], devType[100], servType[100];
	IXML_NodeList *nodeList = NULL;
	IXML_NodeList *tmpNodeList = NULL;
	IXML_Node *tmpNode = NULL;
	IXML_Node *tmpNode2 = NULL;
	IXML_Node *textNode = NULL;
	DOMString tmpStr;
	char SERVER[200];
	const DOMString dbgStr;

	dtrace("Inside AdvertiseAndReply with AdFlag = %d\n", AdFlag);

	if (GetHandleInfo(Hnd, &SInfo) != HND_DEVICE) return UPNP_E_INVALID_HANDLE;
	defaultExp = SInfo->MaxAge;

	/* Modifed to prevent more than one thread from accessing the
	 * UpnpDocument stored with the handle at the same time */
	/* HandleUnlock(); */

	/* get server info */
	get_sdk_info(SERVER);

	// parse the device list and send advertisements/replies 
	for (i=0;; i++)
	{
		tmpNode = ixmlNodeList_item(SInfo->DeviceList, i);
		if (tmpNode == NULL) break;

		dtrace("device type once for '%s'\n", ixmlNode_getNodeName(tmpNode));

		// extract device type 
		ixmlNodeList_free(nodeList);
		nodeList = ixmlElement_getElementsByTagName((IXML_Element *)tmpNode, "deviceType");
		if (nodeList == NULL) continue;
		if ((tmpNode2 = ixmlNodeList_item(nodeList, 0)) == NULL) continue;
		if ((textNode = ixmlNode_getFirstChild(tmpNode2)) == NULL) continue;
		if ((tmpStr = ixmlNode_getNodeValue(textNode)) == NULL) continue;
		strcpy(devType, tmpStr);
		if (devType[0] == 0) continue;

		dtrace("  deviceType = %s\n", devType);

		// extract UDN
		ixmlNodeList_free(nodeList);
		nodeList = ixmlElement_getElementsByTagName((IXML_Element *)tmpNode, "UDN");
		if (nodeList == NULL) continue;
		if ((tmpNode2 = ixmlNodeList_item(nodeList, 0)) == NULL) continue;
		if ((textNode = ixmlNode_getFirstChild(tmpNode2)) == NULL) continue;
		if ((tmpStr = ixmlNode_getNodeValue(textNode)) == NULL) continue;
		strcpy(UDNstr, tmpStr);
		if (UDNstr[0] == 0) continue;

		dtrace("  UDNStr = %s\n", UDNstr);

		if (AdFlag)
		{
			// send the device advertisement 
			if (AdFlag==1)	DeviceAdvertisement(devType, i == 0, UDNstr, SInfo->DescURL, Exp);
			else			DeviceShutdown(devType, i == 0, UDNstr, SERVER, SInfo->DescURL, Exp);
		}
		else
		{
			switch (SearchType)
			{
			case SSDP_ALL:
				DeviceReply(DestAddr, devType, i == 0, UDNstr, SInfo->DescURL, defaultExp);
				break;

			case SSDP_ROOTDEVICE:
				if (i == 0) SendReply(DestAddr, devType, 1, UDNstr, SInfo->DescURL, defaultExp, 0);
				break;

			case SSDP_DEVICEUDN:
				if (DeviceUDN != NULL && strlen(DeviceUDN) != 0)
				{
					if (strcasecmp(DeviceUDN, UDNstr))
					{
						dtrace("DeviceUDN=%s and search UDN=%s did not match\n", UDNstr, DeviceUDN);
						break;
					}
					else
					{
						dtrace("DeviceUDN=%s and search UDN=%s MATCH\n", UDNstr, DeviceUDN);
						SendReply(DestAddr, devType, 0, UDNstr, SInfo->DescURL, defaultExp, 0);
						break;
					}
				}

			case SSDP_DEVICETYPE:
				if (!strncasecmp(DeviceType, devType, strlen(DeviceType)))
				{
					dtrace("DeviceType=%s and search devType=%s MATCH\n", devType, DeviceType);
					SendReply(DestAddr, devType, 0, UDNstr, SInfo->DescURL, defaultExp, 1);
				}
				else
				{
					dtrace("DeviceType=%s and search devType=%s DID NOT MATCH\n", devType, DeviceType);
				}
				break;

			default:
				break;
			}
		}

		/* send service advertisements for services corresponding
		 * to the same device */
		dtrace("Sending service Advertisement\n");

		if ((tmpNode = ixmlNodeList_item(SInfo->ServiceList, i)) == NULL) continue;
		ixmlNodeList_free(nodeList);
		if ((nodeList = ixmlElement_getElementsByTagName((IXML_Element *)tmpNode, "service")) == NULL)
		{
			dtrace("Service not found !\n");
			continue;
		}
		for (j = 0;; j++)
		{
			if ((tmpNode = ixmlNodeList_item(nodeList, j)) == NULL) break;
			ixmlNodeList_free(tmpNodeList);
			tmpNodeList = ixmlElement_getElementsByTagName((IXML_Element *)tmpNode, "serviceType");
			if (tmpNodeList == NULL)
			{
				dtrace("ServiceType not found !\n");
				continue;
			}
			if ((tmpNode2 = ixmlNodeList_item(tmpNodeList, 0)) == NULL) continue;
			if ((textNode = ixmlNode_getFirstChild(tmpNode2)) == NULL) continue;
			// servType is of format Servicetype:ServiceVersion
			if ((tmpStr = ixmlNode_getNodeValue(textNode)) == NULL) continue;
			strcpy(servType, tmpStr);
			if (servType[0] == 0) continue;

			dtrace("ServiceType = %s\n", servType);

			if (AdFlag)
			{
				if (AdFlag==1)	ServiceAdvertisement(UDNstr, servType, SInfo->DescURL, Exp);
				else			ServiceShutdown(UDNstr, servType, SInfo->DescURL, Exp );
			}
			else
			{
				switch (SearchType)
				{
				case SSDP_ALL:
					ServiceReply(DestAddr, servType, UDNstr, SInfo->DescURL, defaultExp);
					break;

				case SSDP_SERVICE:
					if (ServiceType != NULL)
					{
						if (!strncasecmp(ServiceType, servType, strlen(ServiceType)))
							ServiceReply(DestAddr, servType, UDNstr, SInfo->DescURL, defaultExp);
					}
					break;

				default:
					break;
				}               
			}
		}
		ixmlNodeList_free(tmpNodeList);
		tmpNodeList = NULL;
		ixmlNodeList_free(nodeList);
		nodeList = NULL;
	}
	dtrace("Exiting AdvertiseAndReply : \n");
	return UPNP_E_SUCCESS;
}

#endif
#endif

/************************************************************************
* Function : Make_Socket_NoBlocking									
*																	
* Parameters:														
*	IN int sock: socket 
*
* Description:														
*	This function makes socket non-blocking.
*
* Returns: int
*	0 if successful else -1 
***************************************************************************/
int Make_Socket_NoBlocking(int sock)
{
	int val;

	val = fcntl(sock, F_GETFL, 0);
	if (fcntl(sock, F_SETFL, val | O_NONBLOCK) == -1) return -1;
	return 0;
}

/************************************************************************
* Function : unique_service_name								
*																	
* Parameters:														
*	IN char *cmd: Service Name string 
*	OUT SsdpEvent *Evt: The SSDP event structure partially filled 
*						by all the function.
*
* Description:														
*	This function fills the fields of the event structure like DeviceType,
*	Device UDN and Service Type
*
* Returns: int
*	0 if successful else -1 
***************************************************************************/
int unique_service_name(IN char *cmd, IN SsdpEvent * Evt)
{
	char *TempPtr, TempBuf[COMMAND_LEN], *Ptr, *ptr1, *ptr2, *ptr3;
	int CommandFound = 0;

	if ((TempPtr = strstr(cmd, "uuid:schemas")) != NULL)
	{
		ptr1 = strstr(cmd, ":device");
		if (ptr1 != NULL) ptr2 = strstr(ptr1 + 1, ":");
		else return -1;

		if (ptr2 != NULL) ptr3 = strstr(ptr2 + 1, ":");
		else return -1;

		if (ptr3 != NULL) sprintf(Evt->UDN, "uuid:%s", ptr3 + 1);
		else return -1;

		ptr1 = strstr( cmd, ":" );
		if (ptr1 != NULL)
		{
			strncpy(TempBuf, ptr1, ptr3 - ptr1);
			TempBuf[ptr3 - ptr1] = '\0';
			sprintf(Evt->DeviceType, "urn%s", TempBuf);
		}
		else
		{
			return -1;
		}
		return 0;
	}

	if ((TempPtr = strstr(cmd, "uuid")) != NULL)
	{
		if ((Ptr = strstr(cmd, "::")) != NULL)
		{
			strncpy(Evt->UDN, TempPtr, Ptr - TempPtr);
			Evt->UDN[Ptr - TempPtr] = '\0';
		}
		else
		{
			strcpy(Evt->UDN, TempPtr);
		}
		CommandFound = 1;
	}

	if (strstr(cmd, "urn:") && strstr(cmd, ":service:"))
	{
		if ((TempPtr = strstr(cmd, "urn")) != NULL)
		{
			strcpy(Evt->ServiceType, TempPtr);
			CommandFound = 1;
		}
	}

	if (strstr(cmd, "urn:") && strstr(cmd, ":device:"))
	{
		if ((TempPtr = strstr(cmd, "urn")))
		{
			strcpy(Evt->DeviceType, TempPtr);
			CommandFound = 1;
		}
	}

	if (CommandFound == 0) return -1;
	return 0;
}

/************************************************************************
* Function : ssdp_request_type1								
*																	
* Parameters:														
*	IN char *cmd: command came in the ssdp request
*
* Description:														
*	This function figures out the type of the SSDP search in the 
*	in the request.
*
* Returns: enum SsdpSearchType
*	return appropriate search type else returns SSDP_ERROR
***************************************************************************/
enum SsdpSearchType ssdp_request_type1(IN char *cmd)
{
	if (strstr(cmd, ":all"))								return SSDP_ALL;
	if (strstr(cmd, ":rootdevice"))							return SSDP_ROOTDEVICE;
	if (strstr(cmd, "uuid:"))								return SSDP_DEVICEUDN;
	if (strstr(cmd, "urn:") && strstr(cmd, ":device:"))		return SSDP_DEVICETYPE;
	if (strstr(cmd, "urn:") && strstr(cmd, ":service:"))	return SSDP_SERVICE;
	return SSDP_SERROR;
}

/************************************************************************
* Function : ssdp_request_type								
*																	
* Parameters:														
*	IN char *cmd: command came in the ssdp request
*	OUT SsdpEvent *Evt: The event structure partially filled by
*		 this function.
*
* Description:														
*	This function starts filling the SSDP event structure based upon the 
*	request received. 
*
* Returns: int
*	0 on success; -1 on error
***************************************************************************/
int ssdp_request_type(IN char *cmd, OUT SsdpEvent * Evt)
{
	// clear event
	memset(Evt, 0, sizeof(SsdpEvent));
	unique_service_name(cmd, Evt);
	Evt->ErrCode = NO_ERROR_FOUND;

	if ((Evt->RequestType = ssdp_request_type1(cmd)) == SSDP_SERROR)
	{
		Evt->ErrCode = E_HTTP_SYNTEX;
		return -1;
	}
	return 0;
}

/************************************************************************
* Function : free_ssdp_event_handler_data								
*																	
* Parameters:														
*	IN void *the_data: ssdp_thread_data structure. This structure contains
*			SSDP request message.
*
* Description:														
*	This function frees the ssdp request
*
* Returns: VOID
*	
***************************************************************************/
static void free_ssdp_event_handler_data(void *the_data)
{
	ssdp_thread_data *data = (ssdp_thread_data *)the_data;
	if (data)
	{
		http_message_t *hmsg = &data->parser.msg;
		// free data
		httpmsg_destroy(hmsg);
		free(data);
	}
}

/************************************************************************
* Function : valid_ssdp_msg								
*																	
* Parameters:														
*	IN void *the_data: ssdp_thread_data structure. This structure contains
*			SSDP request message.
*
* Description:														
*	This function do some quick checking of the ssdp msg
*
* Returns: xboolean
*	returns TRUE if msg is valid else FALSE
***************************************************************************/
static XINLINE xboolean valid_ssdp_msg(IN http_message_t * hmsg)
{
	memptr hdr_value;

	// check for valid methods - NOTIFY or M-SEARCH
	if (hmsg->method != HTTPMETHOD_NOTIFY &&
			hmsg->method != HTTPMETHOD_MSEARCH &&\
			hmsg->request_method != HTTPMETHOD_MSEARCH)
	{
		return FALSE;
	}
	if (hmsg->request_method != HTTPMETHOD_MSEARCH)
	{
		// check PATH == *
		if (hmsg->uri.type != RELATIVE ||
				strncmp("*", hmsg->uri.pathquery.buff, hmsg->uri.pathquery.size) != 0)
		{
			return FALSE;
		}
		// check HOST header
		if ((httpmsg_find_hdr(hmsg, HDR_HOST, &hdr_value ) == NULL) ||
				(memptr_cmp(&hdr_value, "239.255.255.250:1900") != 0))
		{
			return FALSE;
		}
	}
	return TRUE;                // passed quick check
}

/************************************************************************
* Function : start_event_handler								
*																	
* Parameters:														
*	IN void *the_data: ssdp_thread_data structure. This structure contains
*			SSDP request message.
*
* Description:														
*	This function parses the message and dispatches it to a handler 
*	which handles the ssdp request msg
*
* Returns: int
*	0 if successful -1 if error
***************************************************************************/
static XINLINE int start_event_handler(void *Data)
{
	http_parser_t *parser = NULL;
	parse_status_t status;
	ssdp_thread_data *data = (ssdp_thread_data *)Data;

	parser = &data->parser;

	status = parser_parse(parser);
	if (status == PARSE_FAILURE)
	{
		if (parser->msg.method != HTTPMETHOD_NOTIFY ||
			!parser->valid_ssdp_notify_hack)
		{
			dtrace("SSDP recvd bad msg code = %d\n", status);
			// ignore bad msg, or not enuf mem
			goto error_handler;
		}
		// valid notify msg
	}
	else if (status != PARSE_SUCCESS)
	{
		dtrace("SSDP recvd bad msg code = %d\n", status);
		goto error_handler;
	}
	// check msg
	if (!valid_ssdp_msg(&parser->msg))
	{
		goto error_handler;
	}
	return 0;                   //////// done; thread will free 'data'

error_handler:
	free_ssdp_event_handler_data(data);
	return -1;
}

/************************************************************************
* Function : ssdp_event_handler_thread								
*																	
* Parameters:														
*	IN void *the_data: ssdp_thread_data structure. This structure contains
*			SSDP request message.
*
* Description:														
*	This function is a thread that handles SSDP requests.
*
* Returns: void
*	
***************************************************************************/
static void ssdp_event_handler_thread(void * the_data)
{
	ssdp_thread_data *data = (ssdp_thread_data *)the_data;
	http_message_t *hmsg = &data->parser.msg;

	if (start_event_handler(the_data) != 0) return;

	/* send msg to device or ctrlpt */
	if ((hmsg->method == HTTPMETHOD_NOTIFY) || (hmsg->request_method == HTTPMETHOD_MSEARCH))
	{
#ifdef INCLUDE_CLIENT_APIS
		ssdp_handle_ctrlpt_msg(hmsg, &data->dest_addr, FALSE, NULL);
#endif
	}
	else
	{
#ifdef INCLUDE_DEVICE_APIS
		ssdp_handle_device_request(hmsg, &data->dest_addr);
#endif
	}

	// free data
	free_ssdp_event_handler_data(data);
}

/************************************************************************
* Function : readFromSSDPSocket								
*																	
* Parameters:														
*	IN SOCKET socket: SSDP socket
*
* Description:														
*	This function reads the data from the ssdp socket.
*
* Returns: void
*	
***************************************************************************/
void readFromSSDPSocket(SOCKET socket)
{
	char *requestBuf = NULL;
	char staticBuf[BUFSIZE];
	struct sockaddr_in clientAddr;
	ssdp_thread_data *data = NULL;
	int socklen = 0;
	int byteReceived = 0;

	requestBuf = staticBuf;

	//in case memory
	//can't be allocated, still drain the 
	//socket using a static buffer

	socklen = sizeof( struct sockaddr_in );

	data = (ssdp_thread_data *)malloc(sizeof(ssdp_thread_data));
	if (data != NULL)
	{
		/* initialize parser */
#ifdef INCLUDE_CLIENT_APIS
		if (socket == gSsdpReqSocket) parser_response_init(&data->parser, HTTPMETHOD_MSEARCH);
		else parser_request_init(&data->parser);
#else
		parser_request_init(&data->parser);
#endif

		/* set size of parser buffer */
		if (membuffer_set_size(&data->parser.msg.msg, BUFSIZE) == 0)
		{
			/* use this as the buffer for recv */
			requestBuf = data->parser.msg.msg.buf;
		}
		else
		{
			free(data);
			data = NULL;
		}
	}

	byteReceived = recvfrom(socket, requestBuf, BUFSIZE - 1, 0,
			(struct sockaddr *)&clientAddr, &socklen);

	if (byteReceived > 0 && (LOCAL_HOST_IP.s_addr != clientAddr.sin_addr.s_addr))
	{
		requestBuf[byteReceived] = '\0';

//		dtrace("Received response from host %s,\nPacket:\n%s\n", inet_ntoa(clientAddr.sin_addr), requestBuf);
		dtrace("Received response from host %s\n", inet_ntoa(clientAddr.sin_addr));

		/* add thread pool job to handle request */
		if (data != NULL)
		{
			data->parser.msg.msg.length += byteReceived;
			/* null-terminate */
			data->parser.msg.msg.buf[byteReceived] = 0;
			data->dest_addr = clientAddr;
			ssdp_event_handler_thread(data);

#if 0
			TPJobInit( &job, ( start_routine )
					ssdp_event_handler_thread, data );
			TPJobSetFreeFunction( &job, free_ssdp_event_handler_data );
			TPJobSetPriority( &job, MED_PRIORITY );

			if( ThreadPoolAdd( &gRecvThreadPool, &job, NULL ) != 0 ) {
				free_ssdp_event_handler_data( data );
			}
#endif
		}
	}
	else
	{
		free_ssdp_event_handler_data(data);
	}
}

/************************************************************************
* Function : get_ssdp_sockets								
*																	
* Parameters:														
*	OUT MiniServerSockArray *out: Arrays of SSDP sockets
*
* Description:														
*	This function creates the ssdp sockets. It set their option to listen 
*	for multicast traffic.
*
* Returns: int
*	return UPNP_E_SUCCESS if successful else returns appropriate error
***************************************************************************/
int get_ssdp_sockets( MiniServerSockArray * out )
{
	SOCKET ssdpSock;
#ifdef INCLUDE_CLIENT_APIS
	SOCKET ssdpReqSock;
#endif
	int onOff = 1;
	u_char ttl = 4;
	struct ip_mreq ssdpMcastAddr;
	struct sockaddr_in ssdpAddr;
	int option = 1;

#ifdef INCLUDE_CLIENT_APIS
	if ((ssdpReqSock = socket(AF_INET, SOCK_DGRAM, 0)) == UPNP_INVALID_SOCKET)
	{
		dtrace("Error in socket operation !!!\n");
		return UPNP_E_OUTOF_SOCKET;
	}

	setsockopt(ssdpReqSock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	// just do it, regardless if fails or not.
	Make_Socket_NoBlocking(ssdpReqSock);
	gSsdpReqSocket = ssdpReqSock;
#endif

	if ((ssdpSock = socket(AF_INET, SOCK_DGRAM, 0)) == UPNP_INVALID_SOCKET)
	{
		dtrace("Error in socket operation !!!\n");
#ifdef INCLUDE_CLIENT_APIS
		shutdown(ssdpReqSock, SD_BOTH);
		UpnpCloseSocket( ssdpReqSock );
#endif
		return UPNP_E_OUTOF_SOCKET;
	}

	onOff = 1;
	if (setsockopt(ssdpSock, SOL_SOCKET, SO_REUSEADDR, (char *)&onOff, sizeof(onOff)) != 0)
	{
		dtrace("Error in set reuse addr !!!\n");

#ifdef INCLUDE_CLIENT_APIS
		shutdown( ssdpReqSock, SD_BOTH );
		UpnpCloseSocket( ssdpReqSock );
#endif
		shutdown( ssdpSock, SD_BOTH );
		UpnpCloseSocket( ssdpSock );
		return UPNP_E_SOCKET_ERROR;
	}

	memset((void *)&ssdpAddr, 0, sizeof(struct sockaddr_in));
	ssdpAddr.sin_family = AF_INET;
	ssdpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ssdpAddr.sin_port = htons(SSDP_PORT);
	if (bind(ssdpSock, (struct sockaddr *)&ssdpAddr, sizeof(ssdpAddr)) != 0)
	{
		dtrace("Error in binding !!!\n");
		shutdown( ssdpSock, SD_BOTH );
		UpnpCloseSocket( ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
		shutdown( ssdpReqSock, SD_BOTH );
		UpnpCloseSocket( ssdpReqSock );
#endif
		return UPNP_E_SOCKET_BIND;
	}

	memset((void *)&ssdpMcastAddr, 0, sizeof(struct ip_mreq));
	ssdpMcastAddr.imr_interface.s_addr = htonl(INADDR_ANY);
	ssdpMcastAddr.imr_multiaddr.s_addr = inet_addr(SSDP_IP);

	if (setsockopt(ssdpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
					(char *)&ssdpMcastAddr, sizeof(struct ip_mreq)) != 0)
	{
		dtrace("Error in joining" " multicast group !!!\n");
		shutdown( ssdpSock, SD_BOTH );
#ifdef INCLUDE_CLIENT_APIS
		shutdown( ssdpReqSock, SD_BOTH );
#endif
		UpnpCloseSocket( ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
		UpnpCloseSocket( ssdpReqSock );
#endif
		return UPNP_E_SOCKET_ERROR;
	}

	// result is not checked becuase it will fail in WinMe and Win9x.
	setsockopt(ssdpSock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	if (setsockopt(ssdpSock, SOL_SOCKET, SO_BROADCAST, (char *)&option, sizeof(option)) != 0)
	{
		dtrace("Error in setting broadcast !!!\n");
		shutdown( ssdpSock, SD_BOTH );
#ifdef INCLUDE_CLIENT_APIS
		shutdown( ssdpReqSock, SD_BOTH );
#endif
		UpnpCloseSocket( ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
		UpnpCloseSocket( ssdpReqSock );
#endif
		return UPNP_E_NETWORK_ERROR;
	}
#ifdef INCLUDE_CLIENT_APIS
	out->ssdpReqSock = ssdpReqSock;
	//dtrace("ssdp req socket : %d\n", ssdpReqSock);
#endif
	out->ssdpSock = ssdpSock;
	//dtrace("ssdp socket: %d\n", ssdpSock);
	return UPNP_E_SUCCESS;
}

#endif // EXCLUDE_SSDP
