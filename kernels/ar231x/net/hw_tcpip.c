/* vi: set sw=4 ts=4: */
/*
 * CONFIG_HARDWARE_TURBO
 *
 * NAT accelerator support.
 */

#include <linux/hw_tcpip.h>
#include <linux/netdevice.h>

struct hw_tcpip_helpers *hw_tcpip = NULL;

int register_hw_tcpip(struct hw_tcpip_helpers *tcpip)
{
	MOD_INC_USE_COUNT;
	if (!tcpip || hw_tcpip) return -1;
	hw_tcpip = tcpip;
	return 0;
}

void unregister_hw_tcpip(struct hw_tcpip_helpers *tcpip)
{
	if (tcpip == NULL) hw_tcpip = NULL;
	if (tcpip == hw_tcpip) hw_tcpip = NULL;
	MOD_DEC_USE_COUNT;
	return;
}
