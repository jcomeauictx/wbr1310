/* vi: set sw=4 ts=4: */

#include "upnp.h"
#include "upnpigd.h"

static char service_IP1[]  = "urn:schemas-upnp-org:service:WANIPConnection:1";
static char service_IP2[]  = "urn:schemas-upnp-org:service:WANIPConnection:2";
static char service_PPP1[] = "urn:schemas-upnp-org:service:WANPPPConnection:1";
static char service_PPP2[] = "urn:schemas-upnp-org:service:WANPPPConnection:2";

static void subscription(const char * udn, const char * serviceid, void * param, const char * service)
{
	struct Upnp_Subscription_Request * req = (struct Upnp_Subscription_Request *)param;
	//dtrace("subsciption(%s, %s)\n", udn, serviceid);
	build_property_result(req);
}

static void action(const char * udn, const char * serviceid, void * param, const char * service)
{
	struct Upnp_Action_Request * req = (struct Upnp_Action_Request *)param;
	//dtrace("action(%s, %s, %s)\n", udn, serviceid, req->ActionName);
	build_action_result(req, service);
}

static void statevar(const char * udn, const char * serviceid, void * param)
{
//	struct Upnp_State_Var_Request * req = (struct Upnp_State_Var_Request *)param;
	dtrace("statevar(%s, %s)\n", udn, serviceid);
}

static void notify(const char * udn, const char * serviceid, void * param)
{
	build_notify_result(udn, serviceid);
}

/*************************************************************************/

static void sub_ip1(const char * udn, const char * serviceid, void * param)
{
	subscription(udn, serviceid, param, service_IP1);
}
static void sub_ip2(const char * udn, const char * serviceid, void * param)
{
	subscription(udn, serviceid, param, service_IP2);
}
static void sub_ppp1(const char * udn, const char * serviceid, void * param)
{
	subscription(udn, serviceid, param, service_PPP1);
}
static void sub_ppp2(const char * udn, const char * serviceid, void * param)
{
	subscription(udn, serviceid, param, service_PPP2);
}
static void action_ip1(const char * udn, const char * serviceid, void * param)
{
	action(udn, serviceid, param, service_IP1);
}
static void action_ip2(const char * udn, const char * serviceid, void * param)
{
	action(udn, serviceid, param, service_IP2);
}
static void action_ppp1(const char * udn, const char * serviceid, void * param)
{
	action(udn, serviceid, param, service_PPP1);
}
static void action_ppp2(const char * udn, const char * serviceid, void * param)
{
	action(udn, serviceid, param, service_PPP2);
}

/*************************************************************************/

void add_WANIPConn1(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANIPConn1",
			sub_ip1, action_ip1, statevar, notify);
}

void add_WANIPConn2(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANIPConn2",
			sub_ip2, action_ip2, statevar, notify);
}

void add_WANPPPConn1(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANPPPConn1",
			sub_ppp1, action_ppp1, statevar, notify);
}

void add_WANPPPConn2(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANPPPConn2",
			sub_ppp2, action_ppp2, statevar, notify);
}
