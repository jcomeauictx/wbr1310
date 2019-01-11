/* vi: set sw=4 ts=4: */

#include <ixml.h>
#include <upnp.h>

#include "upnpigd.h"
#include "lrgbin.h"

static void subscription(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Subscription_Request * req = (struct Upnp_Subscription_Request *)param;
	build_property_result(req);
}

static void action(const char * udn, const char * serviceid, void * param)
{
	struct Upnp_Action_Request * req = (struct Upnp_Action_Request *)param;

	if (strlen(req->ActionName) > 0) lrgbin_system("%s %s", g_script, req->ActionName);
	build_action_result(req, "urn:schemas-corega-com:service:CoregaRouterConfig:1");
}

static void statevar(const char * udn, const char * serviceid, void * param)
{
	dtrace("statvar(%%s, %s)\n", udn, serviceid);
}

static void notify(const char * udn, const char * serviceid, void * param)
{
	build_notify_result(udn, serviceid);
}

void add_CoregaRouterCfg1(void)
{
	upnp_add_service("urn:corega-com:serviceId:CoregaRouterCfg1",
			subscription, action, statevar, notify);
}
