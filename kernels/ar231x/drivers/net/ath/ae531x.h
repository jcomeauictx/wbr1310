#ifndef __AE531X_H
#define __AE531X_H

#include <linux/config.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <asm/io.h>

#include "ar531xlnx.h"
#include "ae531xreg.h"
#include "ae531xmac.h"

extern void *ae531x_rxbuf_alloc(ae531x_MAC_t *MACInfo, char **rxBuffp, 
				int *rxBuffSizep);
extern void ae531x_swptr_free(VIRT_ADDR desc);
extern BOOL ae531x_twisted_enet(void);
extern void ae531x_MiiWrite(UINT32 phyBase, UINT32 phyAddr, UINT8 reg, 
			    UINT16 data);
extern UINT16 ae531x_MiiRead(UINT32 phyBase, UINT32 phyAddr, UINT8 reg);
extern void ae531x_unitLinkGained(int ethUnit);
extern void ae531x_unitLinkLost(int ethUnit);
extern void ae531x_WriteDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 data);
extern void ae531x_MACReset(ae531x_MAC_t *MACInfo);
extern void ae531x_DisableComm(ae531x_MAC_t *MACInfo);
extern void ae531x_FreeQueues(ae531x_MAC_t *MACInfo);
extern void ae531x_reset(ae531x_MAC_t *MACInfo);
extern int  ae531x_AllocateQueues(ae531x_MAC_t *MACInfo);
extern void ae531x_EnableComm(ae531x_MAC_t *MACInfo);
extern void ae531x_DmaIntEnable(ae531x_MAC_t *MACInfo);
extern void ae531x_DmaIntDisable(ae531x_MAC_t *MACInfo);
extern void ae531x_DmaReset(ae531x_MAC_t *MACInfo);
extern void ae531x_BeginResetMode(ae531x_MAC_t *MACInfo);
extern void ae531x_AckIntr(ae531x_MAC_t *MACInfo, UINT32 data);
extern void ae531x_SetDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);
extern BOOL ae531x_IsInResetMode(ae531x_MAC_t *MACInfo);
extern UINT32 ae531x_ReadDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg);
extern void ae531x_ClearDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);

#endif   /* __AE531X_H */
