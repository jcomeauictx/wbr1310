/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * See README to understand the decomposition of the ethernet driver.
 *
 * This file contains OS-independent pure software definitions for
 * ethernet support on the AR531X platform.
 */

#ifndef _AE531XMAC_H_
#define _AE531XMAC_H_

#include <linux/config.h>
#include <linux/module.h>

/*
 * DEBUG switches to control verbosity.
 * Just modify the value of ae531x_MAC_debug.
 */
#define AE531X_DEBUG_ALL         0xffffffff
#define AE531X_DEBUG_ERROR       0x00000001 /* Unusual conditions and Errors */
#define AE531X_DEBUG_ARRIVE      0x00000002 /* Arrive into a function */
#define AE531X_DEBUG_LEAVE       0x00000004 /* Leave a function */
#define AE531X_DEBUG_RESET       0x00000008 /* Reset */
#define AE531X_DEBUG_TX          0x00000010 /* Transmit */
#define AE531X_DEBUG_TX_REAP     0x00000020 /* Transmit Descriptor Reaping */
#define AE531X_DEBUG_RX          0x00000040 /* Receive */
#define AE531X_DEBUG_RX_STOP     0x00000080 /* Receive Early Stop */
#define AE531X_DEBUG_INT         0x00000100 /* Interrupts */
#define AE531X_DEBUG_LINK_CHANGE 0x00000200 /* PHY Link status changed */

#define AE531X_NEGOT_TIMEOUT	 500        /* ms to wait for autonegotiation */

extern int ae531x_MAC_debug;

#define AE531X_PRINT(FLG, X)                            \
{                                                   \
    if (ae531x_MAC_debug & (FLG)) {                  \
        DEBUG_PRINTF("%s#%d:%s ",                   \
                     __FILE__,                      \
                     __LINE__,                      \
                     __FUNCTION__);                 \
        DEBUG_PRINTF X;                             \
    }                                               \
}

#define ARRIVE() AE531X_PRINT(AE531X_DEBUG_ARRIVE, ("Arrive{\n"))
#define LEAVE() AE531X_PRINT(AE531X_DEBUG_LEAVE, ("}Leave\n"))

#define RegRead(addr)	\
	(*(volatile unsigned int *)(addr))

#define RegWrite(val,addr)	\
	((*(volatile unsigned int *)(addr)) = (val))

/*****************************************************************
 * Phy code is broken out into a separate layer, so that different
 * PHY hardware can easily be supported.
 *
 * These functions are provided by the PHY layer for use by the MAC layer.
 *   phySetup             -- Set phy hardware appropriately for a MAC unit
 *
 *   phyCheckStatusChange -- Look for dropped/initiated links on any
 *                           phy port associated with a MAC unit
 *
 *   phyIsSpeed100        -- Determines whether or not a PHY is up and
 *                           running at 100Mbit
 *
 *   phyIsFullDuplex      -- Determines whether or not a PHY is up and
 *                           running in Full Duplex mode
 *
 */
#if CONFIG_MARVELL_ENET_PHY
/*
 * Mapping of generic phy APIs to Marvell Ethernet Switch phy functions.
 */
#include "mvPhy.h"
#define phySetup(ethUnit, phyBase)      mv_phySetup((ethUnit), (phyBase))
#define phyCheckStatusChange(ethUnit)   mv_phyCheckStatusChange(ethUnit)
#define phyIsSpeed100(ethUnit)          mv_phyIsSpeed100(ethUnit)
#define phyIsFullDuplex(ethUnit)        mv_phyIsFullDuplex(ethUnit)

#if CONFIG_VENETDEV
#define PHY_TRAILER_SIZE    MV_PHY_TRAILER_SIZE
extern void mv_phyDetermineSource(char *data, int len, int *pFromLAN);
extern void mv_phySetDestinationPort(char *data, int len, int fromLAN);
#define phyDetermineSource(data, len, pFromLAN) mv_phyDetermineSource((data), (len), (pFromLAN))
#define phySetDestinationPort(data, len, fromLAN) mv_phySetDestinationPort((data), (len), (fromLAN))
#else
#define PHY_TRAILER_SIZE    0
#endif
#endif /* CONFIG_MARVELL_ENET_PHY */

#if CONFIG_KENDIN_ENET_PHY || CONFIG_REALTEK_ENET_PHY || CONFIG_KENDIN_KS8995XA_ENET_PHY
/*
 * Mapping of generic phy APIs to Kendin KS8721B and RealTek RTL8201BL phys.
 */
#include "rtPhy.h"
#define phySetup(ethUnit, phyBase)      rt_phySetup((ethUnit), (phyBase))
#define phyCheckStatusChange(ethUnit)   rt_phyCheckStatusChange(ethUnit)
#define phyIsSpeed100(ethUnit)          rt_phyIsSpeed100(ethUnit)
#define phyIsFullDuplex(ethUnit)        rt_phyIsFullDuplex(ethUnit)
#endif

#if CONFIG_ICPLUS_ENET_PHY
/*
 * Mapping of generic phy APIs to Icplus phys.
 */
#include "ipPhy.h"
#define phySetup(ethUnit, phyBase)      ip_phySetup((ethUnit), (phyBase))
#define phyCheckStatusChange(ethUnit)   ip_phyCheckStatusChange(ethUnit)
#define phyIsSpeed100(ethUnit)          ip_phyIsSpeed100(ethUnit)
#define phyIsFullDuplex(ethUnit)        ip_phyIsFullDuplex(ethUnit)
#define phyPowerDown(phyBase, phyUnit)  ip_phyPowerDown((phyBase), (phyUnit))
#endif

#if !defined(PHY_TRAILER_SIZE)
#define PHY_TRAILER_SIZE    0
#endif

/*****************************************************************
 * MAC-independent interface to be used by PHY code
 *
 * These functions are provided by the MAC layer for use by the PHY layer.
 */
#define phyRegRead ae531x_MiiRead
#define phyRegWrite ae531x_MiiWrite
#define phyLinkLost(ethUnit) ae531x_unitLinkLost(ethUnit)
#define phyLinkGained(ethUnit) ae531x_unitLinkGained(ethUnit)
#define phyEthMacDefault() ae531x_ethMacDefault()

void ae531x_unitLinkLost(int unit);
void ae531x_unitLinkGained(int unit);
int ae531x_ethMacDefault(void);


/* 
 * RXBUFF_RESERVE enables building header on WLAN-side in place 
 * NB: Divisible by 2 but NOT 4. Otherwise handle_adel_int() will
 *     be used by the ip layer for misaligned word accesses and 
 *     performance will suffer - a lot.
 */
#define ETH_CRC_LEN       4
#define RXBUFF_RESERVE   98
// #define RXBUFF_RESERVE   98

/*****************************************************************
 * Descriptor queue
 */
typedef struct ae531x_queue {
    VIRT_ADDR   firstDescAddr;  /* descriptor array address */
    VIRT_ADDR   lastDescAddr;   /* last descriptor address */
    VIRT_ADDR   curDescAddr;    /* current descriptor address */
    VIRT_ADDR   reapDescAddr;   /* current tail of tx descriptors reaped */
    UINT16      count;          /* number of elements */
} AE531X_QUEUE;

/* Given a descriptor, return the next one in a circular list */
#define AE531X_QUEUE_ELE_NEXT_GET(q, descAddr)                          \
        ((descAddr) == (q)->lastDescAddr) ? (q)->firstDescAddr :    \
        (VIRT_ADDR)((UINT32)(descAddr) + AE531X_QUEUE_ELE_SIZE)

/* Move the "current descriptor" forward to the next one */
#define AE531X_CONSUME_DESC(q)    \
         q->curDescAddr = AE531X_QUEUE_ELE_NEXT_GET(q, q->curDescAddr)

/*****************************************************************
 * Per-ethernet-MAC OS-independent information
 */
typedef struct ae531x_MAC_s {
    u32             unit;          /* MAC unit ID */
    u32             macBase;       /* MAC base address */
    u32             dmaBase;       /* DMA base address */
    u32             phyBase;       /* PHY base address */
    AE531X_QUEUE    txQueue;       /* Transmit descriptor queue */
    AE531X_QUEUE    rxQueue;       /* Receive descriptor queue */
    UINT16          txDescCount;   /* Transmit descriptor count */
    UINT16          rxDescCount;   /* Receive descriptor count */
    BOOL            aeProcessRst;  /* flag to indicate reset in progress */
    BOOL            port_is_up;    /* flag to indicate port is up */
    void            *OSinfo;       /* OS-dependent data */
    unsigned char   dev_addr[6];
} ae531x_MAC_t;

#define	AE531X_TX_DESC_COUNT_DEFAULT	128    /* Transmit descriptors */
#define AE531X_RX_DESC_COUNT_DEFAULT	128    /* Receive descriptors */


/*****************************************************************
 * Interfaces exported by the OS-independent MAC layer
 */
void ae531x_BeginResetMode(ae531x_MAC_t *MACInfo);
void ae531x_EndResetMode(ae531x_MAC_t *MACInfo);
BOOL ae531x_IsInResetMode(ae531x_MAC_t *MACInfo);
int ae531x_RxQueueCreate(ae531x_MAC_t *MACInfo, AE531X_QUEUE *q,
                  char *pMem, int count);
int ae531x_QueueDelete(struct ae531x_queue *q);
void ae531x_DmaReset(ae531x_MAC_t *MACInfo);
void ae531x_MACReset(ae531x_MAC_t *MACInfo);
void ae531x_EnableComm(ae531x_MAC_t *MACInfo);
void ae531x_DisableComm(ae531x_MAC_t *MACInfo);
void ae531x_reset(ae531x_MAC_t *MACInfo);
int ae531x_AllocateQueues(ae531x_MAC_t *MACInfo);
void ae531x_FreeQueues(ae531x_MAC_t *MACInfo);
void ae531x_QueueInit(AE531X_QUEUE *q, char *pMem, int count);
UINT32 ae531x_ReadMacReg(ae531x_MAC_t *MACInfo, UINT32 reg);
void ae531x_WriteMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 data);
void ae531x_SetMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);
void ae531x_ClearMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);
void ae531x_SetDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);
void ae531x_ClearDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val);
UINT32 ae531x_ReadDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg);
void ae531x_WriteDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 data);
UINT32 ae531x_ReadMiiReg(UINT32 phyBase, UINT32 reg);
void ae531x_WriteMiiReg(UINT32 phyBase, UINT32 reg, UINT32 data);
UINT16 ae531x_MiiRead(UINT32 phyBase, UINT32 phyAddr, UINT8 reg);
void ae531x_MiiWrite(UINT32 phyBase, UINT32 phyAddr, UINT8 reg, UINT16 data);
void ae531x_DmaIntEnable(ae531x_MAC_t *MACInfo);
void ae531x_DmaIntDisable(ae531x_MAC_t *MACInfo);
void ae531x_AckIntr(ae531x_MAC_t *MACInfo, UINT32 val);
void *ae531x_rxbuf_alloc(ae531x_MAC_t *MACInfo, char **rxBptr, int *rxBSize);
void ae531x_swptr_free(VIRT_ADDR txDesc);
BOOL ae531x_twisted_enet(void);

#endif /* _AE531XMAC_H_ */
