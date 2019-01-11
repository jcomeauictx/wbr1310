/* vi: set sw=4 ts=4: */

#include "upnp.h"
#include "upnpigd.h"


static void subscription(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Subscription_Request * req = (struct Upnp_Subscription_Request *)param;
	//dtrace("subsciption(%s, %s)\n", udn, serviceid);
	lrgbin_system("rgdb -s /runtime/switch/getlinktype 1");
	build_property_result(req);
}

static void action(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Action_Request * req = (struct Upnp_Action_Request *)param;
	//dtrace("action(%s, %s)\n", udn, serviceid);
	if (strcmp(req->ActionName, "GetEthernetLinkStatus")==0)
	{
		lrgbin_system("rgdb -s /runtime/switch/getlinktype 1");
	}

	build_action_result(req, "urn:schemas-upnp-org:service:WANEthernetLinkConfig:1");
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


void add_WANEthLinkC1(void)
{
	upnp_add_service("urn:upnp-org:serviceId:WANEthLinkC1",
			subscription, action, statevar, NULL/*notify*/);
}
