/* vi: set sw=4 ts=4: */
#ifndef __UPNPIGD_H__
#define __UPNPIGD_H__

#define INIT_PORT			49152
#define INIT_DESC_DOC		"DeviceDescription"
#define INIT_CONF_DIR		"/var/upnp"

#define DEFAULT_TEMPLATE	"/etc/templates/upnp"
#define GLOBAL_BUFFER_SIZE	2048

extern char g_buffer[];
extern char * g_script;
extern char * g_template;

typedef void (* upnp_cbfn)(const char * udn, const char * serviceid, void * req);

int upnp_add_service(const char * serviceid, upnp_cbfn subscription,
		upnp_cbfn action, upnp_cbfn statevar, upnp_cbfn notify);
void build_action_result(struct Upnp_Action_Request * req, const char * service);
void build_property_result(struct Upnp_Subscription_Request * req);
void build_notify_result(const char * udn, const char * serviceid);
#endif
