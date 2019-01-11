/* vi: set sw=4 ts=4: */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#include <upnp.h>
#include <ixml.h>

#include "upnpigd.h"
#include "lrgbin.h"
#include "md5.h"

/**************************************************************************/

char			g_buffer[GLOBAL_BUFFER_SIZE];

/**************************************************************************/
/* Global configurations */

char *			g_conf_dir_path = NULL;
char *			g_interface = NULL;
unsigned char	g_macaddr[6] = {0};
struct in_addr	g_ipaddr = {0};
int				g_port = INIT_PORT;
char *			g_script = "/etc/templates/upnphelper.sh";
char			g_gate_udn[128] = {0};
char *			g_template = DEFAULT_TEMPLATE;

static void cleanup_exit(int exit_code)
{
	exit(exit_code);
}

static void show_usage(int exit_code)
{
	printf(	"Usage: upnpd [OPTIONS]\n"
			"  -h                  show this help message.\n"
			"  -i {if}             specify interface name.\n"
			"  -p {port}           port to listen.\n"
			"  -s {script}         helper script file.\n"
			"  -t {template}       template directory (default is " DEFAULT_TEMPLATE ")\n"
			"  -n {server name}    Server Info.\n"
		);

	cleanup_exit(exit_code);
}

extern int optind;
extern char * optarg;

static int parse_args(int argc, char * argv[])
{
	int opt, fd, ret = -1;;
	struct ifreq ifr;
	struct sockaddr_in * our_ip;

	while ((opt = getopt(argc, argv, "hi:p:s:t:n:")) > 0)
	{
		switch (opt)
		{
		case 'h':
			show_usage(0);
			break;
		case 'i':
			g_interface = optarg;
			break;
		case 'p':
			g_port = atoi(optarg);
			break;
		case 's':
			g_script = optarg;
			break;
		case 't':
			g_template = optarg;
			break;
		case 'n':
			UpnpSetServerInfo(optarg);
			break;
		default:
			show_usage(-1);
			break;
		}
	}

	if (g_interface)
	{

		/* read interface IP address and MAC address */
		memset(&ifr, 0, sizeof(struct ifreq));
		if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) > 0)
		{
			ifr.ifr_addr.sa_family = AF_INET;
			strcpy(ifr.ifr_name, g_interface);

			/* Get IP */
			if (ioctl(fd, SIOCGIFADDR, &ifr)==0)
			{
				our_ip = (struct sockaddr_in *)&ifr.ifr_addr;
				g_ipaddr.s_addr = our_ip->sin_addr.s_addr;
				dtrace("%s, ipaddr=%s\n", ifr.ifr_name, inet_ntoa(g_ipaddr));
				ret = 0;
			}
			else
			{
				printf("SIOCGIFADDR failed: %s\n", strerror(errno));
			}

			/* Get MAC */
			if (ioctl(fd, SIOCGIFHWADDR, &ifr)==0)
			{
				memcpy(g_macaddr, ifr.ifr_hwaddr.sa_data, 6);
				dtrace("%s, macaddr = %02x:%02x:%02x:%02x:%02x:%02x\n", ifr.ifr_name,
						g_macaddr[0], g_macaddr[1], g_macaddr[2], g_macaddr[3], g_macaddr[4], g_macaddr[5]);
				ret = 0;
			}
			else
			{
				printf("SIOCGIFHWADDR failed: %s!", strerror(errno));
				ret = -1;
			}
		}
		else
		{
			printf("Failed to open socket !\n");
		}
	}
	else
	{
		printf("No interface specified !\n");
	}
	return ret;
}

/**************************************************************************/

#define MAX_SERVICE_COUNT	16

static struct service_table_t
{
	char *		serviceid;
	upnp_cbfn	subscription;
	upnp_cbfn	action;
	upnp_cbfn	statevar;
	upnp_cbfn	notify;
} service_tbl[MAX_SERVICE_COUNT];

static int service_cnt = 0;

int upnp_add_service(const char * serviceid, upnp_cbfn subscription,
		upnp_cbfn action, upnp_cbfn statevar, upnp_cbfn notify)
{
	dassert(serviceid && subscription && action && statevar);

	if (service_cnt < 16)
	{
		service_tbl[service_cnt].serviceid = strdup(serviceid);
		service_tbl[service_cnt].subscription = subscription;
		service_tbl[service_cnt].action = action;
		service_tbl[service_cnt].statevar = statevar;
		service_tbl[service_cnt].notify = notify;
		service_cnt++;
	}
	else
	{
		return -1;
	}
	return service_cnt;
}

static void signal_callback(int sig, void * param)
{
	int i;

	if (sig == SIGUSR1)
	{
		for (i=0; i<service_cnt; i++)
		{
			if (service_tbl[i].notify)
				service_tbl[i].notify(g_gate_udn, service_tbl[i].serviceid, NULL);
		}
	}
}

static void handle_subscription_request(struct Upnp_Subscription_Request * req)
{
	int i;

	//dtrace("Subscribe UDN:%s, SID:%s\n", req->UDN, req->ServiceId);
	
	if (strcmp(req->UDN, g_gate_udn))
	{
		dtrace("Uknown UDN: %s (our is %s)\n", req->UDN, g_gate_udn);
		return;
	}
	
	for (i=0; i<service_cnt; i++)
	{
		if (strcmp(req->ServiceId, service_tbl[i].serviceid)==0)
		{
			dassert(service_tbl[i].subscription);
			service_tbl[i].subscription(req->UDN, req->ServiceId, req);
			return;
		}
	}
	return;
}

static void handle_state_var_request(struct Upnp_State_Var_Request * req)
{
	int i;

	//dtrace("State Var UDN:%s, SID:%s\n", req->DevUDN, req->ServiceID);
	
	if (strcmp(req->DevUDN, g_gate_udn))
	{
		dtrace("Uknown UDN: %s, (our is %s)\n", req->DevUDN, g_gate_udn);
		return;
	}
	
	for (i=0; i<service_cnt; i++)
	{
		if (strcmp(req->ServiceID, service_tbl[i].serviceid)==0)
		{
			dassert(service_tbl[i].statevar);
			service_tbl[i].statevar(req->DevUDN, req->ServiceID, req);
			return;
		}
	}
	return;
}

static void handle_action_request(struct Upnp_Action_Request * req)
{
	int i;

	//dtrace("Action UDN:%s, SID:%s\n", req->DevUDN, req->ServiceID);

	if (strcmp(req->DevUDN, g_gate_udn))
	{
		dtrace("Uknown UDN: %s, (our is %s)\n", req->DevUDN, g_gate_udn);
		return;
	}
	
	for (i=0; i<service_cnt; i++)
	{
		if (strcmp(req->ServiceID, service_tbl[i].serviceid)==0)
		{
			dassert(service_tbl[i].action);
			service_tbl[i].action(req->DevUDN, req->ServiceID, req);
			return;
		}
	}
	return;
}

static int igd_callback(Upnp_EventType EventType, void *Event, void *Cookie)
{
	switch (EventType)
	{
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
		handle_subscription_request((struct Upnp_Subscription_Request *)Event);
		break;
		
	case UPNP_CONTROL_GET_VAR_REQUEST:
		handle_state_var_request((struct Upnp_State_Var_Request *)Event);
		break;
		
	case UPNP_CONTROL_ACTION_REQUEST:
		handle_action_request((struct Upnp_Action_Request *)Event);
		break;

	default:
		dtrace("UNKNOWN Event Type (%d) from library!!\n", EventType);
		break;
	}
	return 0;
}

/**************************************************************************/

#if 0
#define PREFIX(c) { int i; for (i=0; i<c; i++) printf("\t"); }

static walk_node(IXML_Node * node, int depth)
{
	Nodeptr nextnode;

	PREFIX(depth);printf("nodeName:     %s\n", node->nodeName);
	PREFIX(depth);printf("nodeValue:    %s\n", node->nodeValue);
	PREFIX(depth);printf("nodeType:     %d\n", node->nodeType);
	PREFIX(depth);printf("namespaceURI: %s\n", node->namespaceURI);
	PREFIX(depth);printf("prefix:       %s\n", node->prefix);
	PREFIX(depth);printf("localName:    %s\n", node->localName);

	if (node->firstChild) walk_node(node->firstChild, depth+1);
	if (node->nextSibling) walk_node(node->nextSibling, depth);
}
#endif

static char * get_action_variables(struct Upnp_Action_Request * req)
{
	char * buffer = NULL;
	char tmp[512];
	int index = 0;
	const DOMString name;
	const DOMString value;
	IXML_NodeList * nodeList = NULL;
	IXML_Node * node = NULL;

	sprintf(tmp, "%s/%s_action.php\nshell=/var/run/upnp_action.sh\nudn=%s\nserviceid=%s\naction=%s", g_template,
			req->ServiceID, req->DevUDN, req->ServiceID, req->ActionName);
	buffer = strdup(tmp);
	if (!buffer) return NULL;

	nodeList = ixmlDocument_getElementsByTagNameNS(req->ActionRequest, "*", req->ActionName);
	if (nodeList)
	{
		node = ixmlNodeList_item(nodeList, 0);
		if (node)
		{
			node = ixmlNode_getFirstChild(node);
			while (node)
			{
				name = ixmlNode_getLocalName(node);
				value = ixmlNode_getNodeValue(ixmlNode_getFirstChild(node));
				//dtrace("action vars: %s=%s\n", name, value);
				if (name && value)
				{
					index++;
					sprintf(tmp, "\n%s=%s", name, value);
					buffer = realloc(buffer, strlen(buffer)+strlen(tmp)+1);
					if (buffer) strcat(buffer, tmp);
				}
				node = ixmlNode_getNextSibling(node);
			}
			//dtrace("vars : [%s]\n", buffer);
		}
		ixmlNodeList_free(nodeList);
	}
	return buffer;
}

void build_action_result(struct Upnp_Action_Request * req, const char * service)
{
	char * cmd;
	int offset;
	int i;
	int ErrCode = 200;
	char * result;

	cmd = get_action_variables(req);
	if (cmd)
	{
		dtrace("php cmd : [%s]\n", cmd);
		sprintf(g_buffer, "<u:%sResponse xmlns:u=\"%s\">", req->ActionName, service);
		offset = strlen(g_buffer);
		i = lrgdb_do_ephp(cmd, g_buffer+offset, GLOBAL_BUFFER_SIZE-offset);
		free(cmd);
		
		if (i > 0)
		{
			result = lrgbin_eatwhite(g_buffer+offset);
			dtrace("result[%s]\n", result);
			if (strncmp(result, "ErrCode:", 8)==0)
			{
				dtrace("err=[%s]\n", result+8);
				ErrCode = atoi(result+8);
				dtrace("ErrCode = %d !!\n", ErrCode);
			}
			switch (ErrCode)
			{
			case 200:	break;
			case 401:	strcpy(req->ErrStr, "Invalid Action"); break;
			case 402:	strcpy(req->ErrStr, "Invalid Args"); break;
			case 403:	strcpy(req->ErrStr, "Out of Sync"); break;
			case 501:	strcpy(req->ErrStr, "Action Failed"); break;
			case 713:	strcpy(req->ErrStr, "SpecifiedArrayIndexInvalid"); break;
			case 714:	strcpy(req->ErrStr, "NoSuchEntryInArray"); break;
			default:	strcpy(req->ErrStr, "Unknown Error"); break;
			}
			req->ErrCode = ErrCode;
			if (ErrCode == 200)
			{
				offset = strlen(g_buffer);
				sprintf(g_buffer+offset, "</u:%sResponse>", req->ActionName);
				//dtrace("%s\n", g_buffer);
				req->ErrCode = UPNP_E_SUCCESS;
				req->ActionResult = ixmlParseBuffer(g_buffer);
				//walk_node(req->ActionResult, 0);
			}
		}
		lrgbin_system("sh /var/run/upnp_action.sh > /dev/console");
	}
}

static int __property_result(const char * udn, const char * serviceid, int subscription_notify)
{
	char cmd[512];
	int offset;
	int i;

	sprintf(cmd, "%s/%s_notify.php\nsubscription_notify=%d\nudn=%s\nserviceid=%s",
			g_template, serviceid, subscription_notify, udn, serviceid);
	dtrace("php cmd : [%s]\n", cmd);

	sprintf(g_buffer, "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">");
	offset = strlen(g_buffer);
	i = lrgdb_do_ephp(cmd, g_buffer+offset, GLOBAL_BUFFER_SIZE-offset);

	if (i > 0)
	{
		offset = strlen(g_buffer);
		sprintf(g_buffer+offset, "</e:propertyset>");
	}
	return i;
}

void build_property_result(struct Upnp_Subscription_Request * req)
{
	IXML_Document * PropSet;
	UpnpDevice_Handle device_handle;
	int i;

	i = __property_result(req->UDN, req->ServiceId, 1);

	if (i > 0)
	{
		PropSet = ixmlParseBuffer(g_buffer);
		if (PropSet)
		{
			GetDeviceHandle(&device_handle);
			//UpnpAcceptSubscriptionExt(device_handle, req->UDN, req->ServiceId, PropSet, req->Sid);
			UpnpAcceptSubscriptionScheldule(device_handle, req->UDN, req->ServiceId, PropSet, req->Sid);
			//ixmlDocument_free(PropSet);
		}
		else
		{
			dtrace("No PropSet!!!\n");
		}
	}
}

void build_notify_result(const char * udn, const char * serviceid)
{
	int i;
	IXML_Document * PropSet;
	UpnpDevice_Handle device_handle;

	i = __property_result(udn, serviceid, 0);
	if (i > 0)
	{
		PropSet = ixmlParseBuffer(g_buffer);
		if (PropSet)
		{
			GetDeviceHandle(&device_handle);
			UpnpNotifyExt(device_handle, udn, serviceid, PropSet);
			ixmlDocument_free(PropSet);
		}
		else
		{
			dtrace("No PropSet !!!\n");
		}
	}
}

/**************************************************************************/

#if 0
static long _uptime(void)
{
	struct sysinfo info;
	sysinfo(&info);
	return info.uptime;
}
#endif

static void gen_random(char out[16])
{
	MD5_CTX c;
	char tmp[256];
	int i;
#if 0
	long j = _uptime();
#else
	long j = 12345;
#endif

	for (i=0; i<256; i++)
	{
		tmp[i] = g_macaddr[i % 6] ^ (char)(i + j);
	}
	
	MD5Init(&c);
	MD5Update(&c, &j, sizeof(j));
	MD5Update(&c, g_macaddr, 6);
	MD5Update(&c, tmp, sizeof(tmp));
	MD5Final(out, &c);
}

/**************************************************************************/

#ifdef L3FORWARDING1
extern void add_L3Forwarding1(void);
#endif
#ifdef WANCOMMONIFC1
extern void add_WANCommonIFC1(void);
#endif
#ifdef WANETHLINKC1
extern void add_WANEthLinkC1(void);
#endif
#ifdef WANIPCONN1
extern void add_WANIPConn1(void);
#endif
#ifdef WANIPCONN2
extern void add_WANIPConn2(void);
#endif
#ifdef WANPPPCONN1
extern void add_WANPPPConn1(void);
#endif
#ifdef WANPPPCONN2
extern void add_WANPPPConn2(void);
#endif
#ifdef COREGAROUTERCFG1
extern void add_CoregaRouterCfg1(void);
#endif

static void add_services(void)
{
#ifdef WANIPCONN1
	add_WANIPConn1();
#endif
#ifdef WANIPCONN2
	add_WANIPConn2();
#endif
#ifdef WANPPPCONN1
	add_WANPPPConn1();
#endif
#ifdef WANPPPCONN2
	add_WANPPPConn2();
#endif
#ifdef WANCOMMONIFC1
	add_WANCommonIFC1();
#endif
#ifdef WANETHLINKC1
	add_WANEthLinkC1();
#endif
#ifdef L3FORWARDING1
	add_L3Forwarding1();
#endif
#ifdef COREGAROUTERCFG1
	add_CoregaRouterCfg1();
#endif
}
/**************************************************************************/
#ifdef INC_UPNP
int upnpd_main (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
	char *desc_doc_name=NULL;
	unsigned char hash[16];

	/* parse and get arguments */
	if (parse_args(argc, argv) < 0) cleanup_exit(-1);

	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);

	desc_doc_name = INIT_DESC_DOC;
	g_conf_dir_path = INIT_CONF_DIR;

	/* Create UDN string */
	gen_random(hash);
	sprintf(g_gate_udn, "uuid:%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9],
		g_macaddr[0], g_macaddr[1], g_macaddr[2], g_macaddr[3], g_macaddr[4], g_macaddr[5]);

	add_services();

	/*  */
	lrgbin_system("%s start /var/upnp", g_script);
	
	/* upnphelper.sh genxml 192.168.1.1 49152 UPNP-IGD-00123456789A */
	lrgbin_system("%s genxml /var/upnp/%s.xml %s %d %s",
		g_script, desc_doc_name, inet_ntoa(g_ipaddr), g_port, g_gate_udn+5);

	/* show some messages */
	printf("Upnpd starting on %s, %s, port %d\n", g_interface, inet_ntoa(g_ipaddr), g_port);
	printf("Template dir : %s\n", g_template);
	
	//SetDelportCallback( PortMapDelete);
	UpnpInit(inet_ntoa(g_ipaddr), g_port, g_interface,
		desc_doc_name, g_conf_dir_path, igd_callback, NULL, signal_callback);

	UpnpFinish();
	cleanup_exit(0);
	return 0;
}
