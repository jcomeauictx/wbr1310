/* vi: set sw=4 ts=4: */

#include <ixml.h>
#include <upnp.h>

#include "upnpigd.h"
#include "lrgbin.h"


static void subscription(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Subscription_Request * req = (struct Upnp_Subscription_Request *)param;
	//dtrace("subscription(%s, %s)\n", udn, serviceid);
	lrgbin_system("rgdb -s /runtime/switch/getlinktype 1");
	build_property_result(req);
}

static void action(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Action_Request * req = (struct Upnp_Action_Request *)param;
	//dtrace("action(%s, %s, %s)\n", udn, serviceid, req->ActionName);

	if (strcmp(req->ActionName, "GetCommonLinkProperties")==0)
	{
		lrgbin_system("rgdb -s /runtime/switch/getlinktype 1");
	}

	build_action_result(req, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1");
}

static void statevar(const char * udn, const char * serviceid, void * param)
{
//	struct Upnp_State_Var_Request * req = (struct Upnp_State_Var_Request *)param;
	dtrace("statevar(%s, %s)\n", udn, serviceid);
}

static void notify(const char * udn, const char * serviceid, void * param)
{
	lrgbin_system("rgdb -s /runtime/switch/getlinktype 1");
	build_notify_result(udn, serviceid);
}

void add_WANCommonIFC1(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANCommonIFC1",
			subscription, action, statevar, NULL/*notify*/);
}
