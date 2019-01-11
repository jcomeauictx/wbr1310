/* vi: set sw=4 ts=4: */
/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */


/*
 * Ethernet driver for Atheros' ae531x ethernet MAC.
 */

#if linux
#include <linux/config.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <asm/io.h>

#include "ar531xlnx.h"
#endif /* linux */

#include "ae531xreg.h"
#include "ae531xmac.h"

#ifdef DEBUG
int ae531x_MAC_debug = AE531X_DEBUG_ERROR;
#else
int ae531x_MAC_debug = 0;
#endif

#if 0
extern char *ae531x_enet_mac_address_get(int)
#endif

/* Forward references to local functions */
static void ae531x_QueueDestroy(AE531X_QUEUE *q);


/******************************************************************************
*
* ae531x_ReadMacReg - read AE MAC register
*
* RETURNS: register value
*/
UINT32
ae531x_ReadMacReg(ae531x_MAC_t *MACInfo, UINT32 reg)
{
    UINT32 addr = MACInfo->macBase+reg;
    UINT32 data;

    data = RegRead(addr);
    return data;
}


/******************************************************************************
*
* ae531x_WriteMacReg - write AE MAC register
*
* RETURNS: N/A
*/
void
ae531x_WriteMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 data)
{
    UINT32 addr = MACInfo->macBase+reg;

    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_SetMacReg - set bits in AE MAC register
*
* RETURNS: N/A
*/
void
ae531x_SetMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val)
{
    UINT32 addr = MACInfo->macBase+reg;
    UINT32 data = RegRead(addr);

    data |= val;
    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_ClearMacReg - clear bits in AE MAC register
*
* RETURNS: N/A
*/
void
ae531x_ClearMacReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val)
{
    UINT32 addr = MACInfo->macBase+reg;
    UINT32 data = RegRead(addr);

    data &= ~val;
    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_ReadDmaReg - read AE DMA register
*
* RETURNS: register value
*/
UINT32
ae531x_ReadDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg)
{
    UINT32 addr = MACInfo->dmaBase+reg;
    UINT32 data = RegRead(addr);

    return data;
}


/******************************************************************************
*
* ae531x_WriteDmaReg - write AE DMA register
*
* RETURNS: N/A
*/
void
ae531x_WriteDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 data)
{
    UINT32 addr = MACInfo->dmaBase+reg;

    RegWrite(data, addr);
}


/******************************************************************************
 *
 * ae531x_AckIntr - clear interrupt bits in the status register.
 * Note: Interrupt bits are *cleared* by writing a 1.
 */
void
ae531x_AckIntr(ae531x_MAC_t *MACInfo, UINT32 data)
{
      ae531x_WriteDmaReg(MACInfo, DmaStatus, data);
}


/******************************************************************************
*
* ae531x_SetDmaReg - set bits in an AE DMA register
*
* RETURNS: N/A
*/
void
ae531x_SetDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val)
{
    UINT32 addr = MACInfo->dmaBase+reg;
    UINT32 data = RegRead(addr);

    data |= val;
    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_ClearDmaReg - clear bits in an AE DMA register
*
* RETURNS: N/A
*/
void
ae531x_ClearDmaReg(ae531x_MAC_t *MACInfo, UINT32 reg, UINT32 val)
{
    UINT32 addr = MACInfo->dmaBase+reg;
    UINT32 data = RegRead(addr);

    data &= ~val;
    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_ReadMiiReg - read PHY registers via AE MAC Mii addr/data registers
*
* RETURNS: register value
*/
UINT32
ae531x_ReadMiiReg(UINT32 phyBase, UINT32 reg)
{
    UINT32 data;
    UINT32 addr = phyBase+reg;

    data = RegRead(addr);
    return data;
}


/******************************************************************************
*
* ae531x_WriteMiiReg - write PHY registers via AE MAC Mii addr/data registers
*
* RETURNS: N/A
*/
void
ae531x_WriteMiiReg(UINT32 phyBase, UINT32 reg, UINT32 data)
{
    UINT32 addr = phyBase+reg;

    RegWrite(data, addr);
}


/******************************************************************************
*
* ae531x_MiiRead - read AE Mii register
*
* RETURNS: register value
*/
UINT16
ae531x_MiiRead(UINT32 phyBase, UINT32 phyAddr, UINT8 reg)
{
    UINT32 addr;
    UINT16 data;

    addr = ((phyAddr << MiiDevShift) & MiiDevMask) | ((reg << MiiRegShift) & MiiRegMask);

    ae531x_WriteMiiReg(phyBase, MacMiiAddr, addr );
    do {
        /* nop */
    } while ((ae531x_ReadMiiReg(phyBase, MacMiiAddr ) & MiiBusy) == MiiBusy);

    data = ae531x_ReadMiiReg(phyBase, MacMiiData) & 0xFFFF;

    return data;
}


/******************************************************************************
*
* ae531x_MiiWrite - write AE Mii register
*
* RETURNS: N/A
*/
void
ae531x_MiiWrite(UINT32 phyBase, UINT32 phyAddr, UINT8 reg, UINT16 data)
{
    UINT32 addr;

    ae531x_WriteMiiReg(phyBase, MacMiiData, data );

    addr = ((phyAddr << MiiDevShift) & MiiDevMask) |
        ((reg << MiiRegShift) & MiiRegMask) | MiiWrite;
    ae531x_WriteMiiReg(phyBase, MacMiiAddr, addr );

    do {
        /* nop */
    } while ((ae531x_ReadMiiReg(phyBase, MacMiiAddr ) & MiiBusy) == MiiBusy);
}


/*******************************************************************************
* ae531x_BeginResetMode - enter a special "reset mode" in which
*    -no interrupts are expected from the device
*    -the device will not transmit nor receive
*    -attempts to send or receive will return with an error and
*    -the device will be reset at the next convenient opportunity.
*/
void
ae531x_BeginResetMode(ae531x_MAC_t *MACInfo)
{
    /* Set the reset flag */
    MACInfo->aeProcessRst = 1;
}


/*******************************************************************************
* ae531x_EndResetMode - exit the special "reset mode" entered
* earlier via a call to ae531x_BeginResetMode.
*/
void
ae531x_EndResetMode(ae531x_MAC_t *MACInfo)
{
    MACInfo->aeProcessRst = 0;
}


/*******************************************************************************
* ae531x_IsInResetMode - determine whether or not the device is
* currently in "reset mode" (i.e. that a device reset is pending)
*/
BOOL
ae531x_IsInResetMode(ae531x_MAC_t *MACInfo)
{
    return MACInfo->aeProcessRst;
}


/******************************************************************************
*
* ae531x_DmaRxStart - Start Rx
*
* RETURNS: N/A
*/
static void
ae531x_DmaRxStart(ae531x_MAC_t *MACInfo)
{
    ae531x_SetDmaReg(MACInfo, DmaControl, DmaRxStart);
    sysWbFlush();
}


/******************************************************************************
*
* ae531x_DmaRxStop - Stop Rx
*
* RETURNS: N/A
*/
void
ae531x_DmaRxStop(ae531x_MAC_t *MACInfo)
{
    ae531x_ClearDmaReg(MACInfo, DmaControl, DmaRxStart);
    sysWbFlush();
}


/******************************************************************************
*
* ae531x_DmaTxStart - Start Tx
*
* RETURNS: N/A
*/
void
ae531x_DmaTxStart(ae531x_MAC_t *MACInfo)
{
    ae531x_SetDmaReg(MACInfo, DmaControl, DmaTxStart);
    sysWbFlush();
}


/******************************************************************************
*
* ae531x_DmaTxStop - Stop Tx
*
* RETURNS: N/A
*/
void
ae531x_DmaTxStop(ae531x_MAC_t *MACInfo)
{
    ae531x_ClearDmaReg(MACInfo, DmaControl, DmaTxStart);
    sysWbFlush();
}


/******************************************************************************
*
* ae531x_DmaIntEnable - Enable DMA interrupts
*
* RETURNS: N/A
*/
void
ae531x_DmaIntEnable(ae531x_MAC_t *MACInfo)
{
    ae531x_WriteDmaReg(MACInfo, DmaIntrEnb, DmaIntEnable);
}


/******************************************************************************
*
* ae531x_DmaIntDisable - Disable DMA interrupts
*
* RETURNS: N/A
*/
void
ae531x_DmaIntDisable(ae531x_MAC_t *MACInfo)
{
    ae531x_WriteDmaReg(MACInfo, DmaIntrEnb, DmaIntDisable);
}


/******************************************************************************
*
* ae531x_DmaIntClear - Clear DMA interrupts
*
* RETURNS: N/A
*/
static void
ae531x_DmaIntClear(ae531x_MAC_t *MACInfo)
{
    /* clear all interrupt requests */
    ae531x_WriteDmaReg(MACInfo, DmaStatus,
                      ae531x_ReadDmaReg(MACInfo, DmaStatus));  
}


/******************************************************************************
* Initialize generic queue data
*/
void
ae531x_QueueInit(AE531X_QUEUE *q, char *pMem, int count)
{
    ARRIVE();
    q->firstDescAddr = pMem;
    q->lastDescAddr = (VIRT_ADDR)((UINT32)q->firstDescAddr +
                                  (count - 1) * AE531X_QUEUE_ELE_SIZE);
    q->curDescAddr = q->firstDescAddr;
    q->count = count;
    LEAVE();
}


/******************************************************************************
* ae531x_TxQueueCreate - create a circular queue of descriptors for Transmit
*/
static int
ae531x_TxQueueCreate(ae531x_MAC_t *MACInfo,
                  AE531X_QUEUE *q,
                  char *pMem,
                  int count)
{
    int         i;
    VIRT_ADDR   descAddr;

    ARRIVE();

    ae531x_QueueInit(q, pMem, count);
    q->reapDescAddr = q->lastDescAddr;

    /* Initialize Tx buffer descriptors.  */
    for (i=0, descAddr=q->firstDescAddr;
         i<count;
         i++, descAddr=(VIRT_ADDR)((UINT32)descAddr + AE531X_QUEUE_ELE_SIZE))
    {
        /* Update the size, BUFPTR, and SWPTR fields */

        AE531X_DESC_STATUS_SET(descAddr, 0);
        AE531X_DESC_CTRLEN_SET(descAddr, 0);

        AE531X_DESC_BUFPTR_SET(descAddr, (UINT32)0);
        AE531X_DESC_LNKBUF_SET(descAddr, (UINT32)0);
        AE531X_DESC_SWPTR_SET(descAddr, (void *)0);
    } /* for each desc */

    /* Make the queue circular */
    AE531X_DESC_CTRLEN_SET(q->lastDescAddr,
                       DescEndOfRing|AE531X_DESC_CTRLEN_GET(q->lastDescAddr));

    AE531X_PRINT(AE531X_DEBUG_RESET,
            ("ethmac%d Txbuf begin = %x, end = %x\n",
            MACInfo->unit,
            (UINT32)q->firstDescAddr,
            (UINT32)q->lastDescAddr));

    LEAVE();
    return 0;
}


/******************************************************************************
* ae531x_RxQueueCreate - create a circular queue of Rx descriptors
*/
int
ae531x_RxQueueCreate(ae531x_MAC_t *MACInfo,
                  AE531X_QUEUE *q,
                  char *pMem,
                  int count)
{
    int               i;
    VIRT_ADDR         descAddr;

    ARRIVE();

    ae531x_QueueInit(q, pMem, count);
    q->reapDescAddr = NULL;


    /* Initialize Rx buffer descriptors */
    for (i=0, descAddr=q->firstDescAddr;
         i<count;
         i++, descAddr=(VIRT_ADDR)((UINT32)descAddr + AE531X_QUEUE_ELE_SIZE))
    {
        void *swptr;
        char *rxBuffer;
        int  rxBufferSize;

        swptr = ae531x_rxbuf_alloc(MACInfo, &rxBuffer, &rxBufferSize);
        if (swptr == NULL) {
                AE531X_PRINT(AE531X_DEBUG_RESET,
                          ("ethmac%d RX queue: ae531x_rxbuf_alloc failed\n",
                           MACInfo->unit));
                ae531x_QueueDestroy(q);
                return -1;
        }
        AE531X_DESC_SWPTR_SET(descAddr, swptr);

        AE531X_DESC_STATUS_SET(descAddr, DescOwnByDma);
        AE531X_DESC_CTRLEN_SET(descAddr, rxBufferSize);
        AE531X_DESC_BUFPTR_SET(descAddr, virt_to_bus(rxBuffer));
        AE531X_DESC_LNKBUF_SET(descAddr, (UINT32)0);
    } /* for each desc */

    /* Make the queue circular */
    AE531X_DESC_CTRLEN_SET(q->lastDescAddr,
                       DescEndOfRing|AE531X_DESC_CTRLEN_GET(q->lastDescAddr));

    AE531X_PRINT(AE531X_DEBUG_RESET,
              ("ethmac%d Rxbuf begin = %x, end = %x\n",
              MACInfo->unit,
              (UINT32)q->firstDescAddr,
              (UINT32)q->lastDescAddr));

    LEAVE();
    return 0;
}


/******************************************************************************
* ae531x_QueueDestroy -- Free all buffers and descriptors associated 
* with a queue.
*/
static void
ae531x_QueueDestroy(AE531X_QUEUE *q)
{
    int i;
    int count;
    VIRT_ADDR    descAddr;

    ARRIVE();

    count = q->count;

    for (i=0, descAddr=q->firstDescAddr;
         i<count;
         i++, descAddr=(VIRT_ADDR)((UINT32)descAddr + AE531X_QUEUE_ELE_SIZE)) {

        AE531X_DESC_STATUS_SET(descAddr, 0);
        AE531X_DESC_CTRLEN_SET(descAddr, 0);
        AE531X_DESC_BUFPTR_SET(descAddr, (UINT32)0);
        AE531X_DESC_LNKBUF_SET(descAddr, (UINT32)0);

        ae531x_swptr_free(descAddr); /* Free OS-specific software pointer */
    }

    LEAVE();
}

static void
ae531x_TxQueueDestroy(ae531x_MAC_t *MACInfo)
{
    ae531x_QueueDestroy(&MACInfo->txQueue);
}

static void
ae531x_RxQueueDestroy(ae531x_MAC_t *MACInfo)
{
    ae531x_QueueDestroy(&MACInfo->rxQueue);
}


/******************************************************************************
* ae531x_AllocateQueues - Allocate receive and transmit queues
*/
int
ae531x_AllocateQueues(ae531x_MAC_t *MACInfo)
{
    size_t QMemSize;
    char *pTxBuf = NULL;
    char *pRxBuf = NULL;

    ARRIVE();

    MACInfo->txDescCount = AE531X_TX_DESC_COUNT_DEFAULT;
    QMemSize = AE531X_QUEUE_ELE_SIZE * MACInfo->txDescCount;
    pTxBuf = MALLOC(QMemSize);
    if (pTxBuf == NULL) {
        AE531X_PRINT(AE531X_DEBUG_RESET,
                  ("ethmac%d Failed to allocate TX queue\n", MACInfo->unit));
        goto AllocQFail;
    }

    if (ae531x_TxQueueCreate(MACInfo, &MACInfo->txQueue, pTxBuf,
                          MACInfo->txDescCount) < 0)
    {
        AE531X_PRINT(AE531X_DEBUG_RESET,
                ("ethmac%d Failed to create TX queue\n", MACInfo->unit));
        goto AllocQFail;
    }

    MACInfo->rxDescCount = AE531X_RX_DESC_COUNT_DEFAULT;
    QMemSize = AE531X_QUEUE_ELE_SIZE * MACInfo->rxDescCount;
    pRxBuf = MALLOC(QMemSize);
    if (pRxBuf == NULL) {
        AE531X_PRINT(AE531X_DEBUG_RESET,
                  ("ethmac%d Failed to allocate RX queue\n", MACInfo->unit));
        goto AllocQFail;
    }

    if (ae531x_RxQueueCreate(MACInfo, &MACInfo->rxQueue, pRxBuf,
                          MACInfo->rxDescCount) < 0)
    {
        AE531X_PRINT(AE531X_DEBUG_RESET,
                ("ethmac%d Failed to create RX queue\n", MACInfo->unit));
        goto AllocQFail;
    }

    AE531X_PRINT(AE531X_DEBUG_RESET,
            ("ethmac%d Memory setup complete.\n", MACInfo->unit));

    LEAVE();
    return 0;

AllocQFail:
    MACInfo->txDescCount = 0; /* sanity */
    MACInfo->rxDescCount = 0; /* sanity */

    if (pTxBuf) {
        FREE(pTxBuf);
    }
    if (pRxBuf) {
        FREE(pRxBuf);
    }
    
    LEAVE();
    return -1;
}


/******************************************************************************
*
* ae531x_FreeQueues - Free Transmit & Receive queues
*/
void
ae531x_FreeQueues(ae531x_MAC_t *MACInfo)
{
    ae531x_TxQueueDestroy(MACInfo);
    FREE(MACInfo->txQueue.firstDescAddr);

    ae531x_RxQueueDestroy(MACInfo);
    FREE(MACInfo->rxQueue.firstDescAddr);
}

/******************************************************************************
*
* ae531x_DmaReset - Reset DMA and TLI controllers
*
* RETURNS: N/A
*/
void
ae531x_DmaReset(ae531x_MAC_t *MACInfo)
{
    int        i;
    UINT32     descAddr;

    ARRIVE();

    /* Disable device interrupts prior to any errors during stop */
    intDisable(MACInfo->ilevel);

    /* Disable MAC rx and tx */
    ae531x_ClearMacReg(MACInfo, MacControl, (MacRxEnable | MacTxEnable));

    udelay(1);

    /* Reset dma controller */
    
    ae531x_WriteDmaReg(MACInfo, DmaBusMode, DmaResetOn);

    /* Delay 2 usec */
    sysUDelay(2);

    /* Flush the rx queue */
    descAddr = (UINT32)MACInfo->rxQueue.firstDescAddr;
    MACInfo->rxQueue.curDescAddr = MACInfo->rxQueue.firstDescAddr;
    for (i=0;
         i<(MACInfo->rxDescCount);
         i++, descAddr += AE531X_QUEUE_ELE_SIZE) {
            AE531X_DESC_STATUS_SET(descAddr, DescOwnByDma);
    }

    /* Flush the tx queue */
    descAddr = (UINT32)MACInfo->txQueue.firstDescAddr;
    MACInfo->txQueue.curDescAddr = MACInfo->txQueue.firstDescAddr;
    MACInfo->txQueue.reapDescAddr = MACInfo->txQueue.lastDescAddr;
    for (i=0;
         i<(MACInfo->txDescCount);
         i++, descAddr += AE531X_QUEUE_ELE_SIZE) {
            AE531X_DESC_STATUS_SET (descAddr, 0);
    }

    /* Set init register values  */
    ae531x_WriteDmaReg(MACInfo, DmaBusMode, DmaBusModeInit);

    /* Install the first Tx and Rx queues on the device */
    ae531x_WriteDmaReg(MACInfo, DmaRxBaseAddr,
                      virt_to_bus(MACInfo->rxQueue.firstDescAddr));
    ae531x_WriteDmaReg(MACInfo, DmaTxBaseAddr,
                      virt_to_bus(MACInfo->txQueue.firstDescAddr));


    ae531x_WriteDmaReg(MACInfo, DmaControl, DmaStoreAndForward);

    ae531x_WriteDmaReg(MACInfo, DmaIntrEnb, DmaIntDisable);

    AE531X_PRINT(AE531X_DEBUG_RESET,
              ("ethmac%d: DMA RESET!\n", MACInfo->unit));

    /* Turn on device interrupts -- enable most errors */
    ae531x_DmaIntClear(MACInfo);    /* clear interrupt requests  */
    ae531x_DmaIntEnable(MACInfo);   /* enable interrupts */

    ae531x_EndResetMode(MACInfo);

    intEnable(MACInfo->ilevel);

    LEAVE();
}


/******************************************************************************
*
* ae531x_MACAddressSet - Set the ethernet address
*
* Sets the ethernet address according to settings in flash.
*
* RETURNS: void
*/
static void ae531x_MACAddressSet(ae531x_MAC_t *MACInfo)
{
	unsigned int data;
    UINT8 *macAddr;

    ARRIVE();

#if 0
	macAddr = ae531x_enet_mac_address_get(MACInfo->unit);
#else
	macAddr = MACInfo->dev_addr;
#endif

	/* set our MAC address  */
	data = (macAddr[5]<<8) | macAddr[4];
	ae531x_WriteMacReg(MACInfo, MacAddrHigh, data );

	data = (macAddr[3]<<24) | (macAddr[2]<<16) | (macAddr[1]<<8) | macAddr[0];
	ae531x_WriteMacReg(MACInfo, MacAddrLow, data );

	AE531X_PRINT(AE531X_DEBUG_RESET, ("ethmac%d Verify MAC address %8.8X %8.8X \n",
		MACInfo->unit, ae531x_ReadMacReg(MACInfo, MacAddrLow), ae531x_ReadMacReg(MACInfo, MacAddrHigh)));

	AE531X_PRINT(AE531X_DEBUG_RESET, ("  sb = %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X\n",
		0xff&macAddr[0], 0xff&macAddr[1], 0xff&macAddr[2], 0xff&macAddr[3], 0xff&macAddr[4], 0xff&macAddr[5]));

	LEAVE();
}


/******************************************************************************
*
* ae_SetMACFromPhy - read Phy settings and update Mac
*                    with current duplex and speed.
*
* RETURNS:
*/
static void
ae531x_SetMACFromPhy(ae531x_MAC_t *MACInfo)
{
    UINT32  macCtl;
    BOOL    fullDuplex;
    UINT32  timeout;

    ARRIVE();

    timeout = jiffies+(HZ/1000)*AE531X_NEGOT_TIMEOUT;

    /* Get duplex mode from Phy */
    while (((fullDuplex = phyIsFullDuplex(MACInfo->unit)) == -1) &&
	   (jiffies <= timeout));

    /* Flag is set for full duplex mode, else cleared */
    macCtl = ae531x_ReadMacReg(MACInfo, MacControl);

    if (fullDuplex) {
        /* set values of control registers */
        macCtl &= ~MacDisableRxOwn;
        macCtl |= MacFullDuplex;
        ae531x_WriteMacReg(MACInfo, MacControl, macCtl);
        ae531x_WriteMacReg(MACInfo, MacFlowControl, MacFlowControlInitFdx);
    } else {
	/* set values of control registers */
        ae531x_WriteMacReg(MACInfo, MacFlowControl, MacFlowControlInitHdx);
        macCtl |= MacDisableRxOwn;
        macCtl &= ~MacFullDuplex;
        ae531x_WriteMacReg(MACInfo, MacControl, macCtl);
    }

    LEAVE();
}


/******************************************************************************
* ae531x_MACReset -- sets MAC address and duplex.
*/
void
ae531x_MACReset(ae531x_MAC_t *MACInfo)
{
    ae531x_MACAddressSet(MACInfo);
#ifndef CONFIG_AR5315
    ae531x_SetMACFromPhy(MACInfo);
#endif
}


/******************************************************************************
* ae531x_EnableComm -- enable Transmit and Receive
*/
void
ae531x_EnableComm(ae531x_MAC_t *MACInfo)
{
    ae531x_SetMacReg(MACInfo, MacControl, (MacRxEnable | MacTxEnable));
    ae531x_DmaRxStart(MACInfo);     /* start receiver  */
    ae531x_DmaTxStart(MACInfo);     /* start transmitter */
}


/******************************************************************************
* ae531x_DisableComm -- disable Transmit and Receive
*/
void
ae531x_DisableComm(ae531x_MAC_t *MACInfo)
{
    ae531x_ClearMacReg(MACInfo, MacControl, (MacRxEnable | MacTxEnable));
}


/******************************************************************************
* ae531x_reset -- Cold reset ethernet interface
*/
void
ae531x_reset(ae531x_MAC_t *MACInfo)
{
    UINT32 mask = 0;
    UINT32 regtmp;
#ifndef CONFIG_AR5315
   
    if (MACInfo->unit == 0) {
        mask = AR531X_RESET_ENET0 | AR531X_RESET_EPHY0;
    } else {
        mask = AR531X_RESET_ENET1 | AR531X_RESET_EPHY1;
    }

    /* Put into reset */
    regtmp = sysRegRead(AR531X_RESET);
    sysRegWrite(AR531X_RESET, regtmp | mask);
    sysMsDelay(15);

    /* Pull out of reset */
    regtmp = sysRegRead(AR531X_RESET);
    sysRegWrite(AR531X_RESET, regtmp & ~mask);
    sysUDelay(25);

    /* Enable */
    if (MACInfo->unit == 0) {
        mask = AR531X_ENABLE_ENET0;
    } else {
        mask = AR531X_ENABLE_ENET1;
    }
    regtmp = sysRegRead(AR531X_ENABLE);
    sysRegWrite(AR531X_ENABLE, regtmp | mask);
#else
    if (MACInfo->unit == 0) {
        mask = AR531X_RESET_ENET0 | AR531X_RESET_EPHY0;
    }
    /* Enable Arbitration for Ethernet bus */
    regtmp = sysRegRead(AR531XPLUS_AHB_ARB_CTL);
    regtmp |= ARB_ETHERNET;
    sysRegWrite(AR531XPLUS_AHB_ARB_CTL, regtmp);

    /* Put into reset */
    regtmp = sysRegRead(AR531X_RESET);
    sysRegWrite(AR531X_RESET, regtmp | mask);
    sysMsDelay(10);

    /* Pull out of reset */
    regtmp = sysRegRead(AR531X_RESET);
    sysRegWrite(AR531X_RESET, regtmp & ~mask);
    sysMsDelay(10);

    regtmp = sysRegRead(AR531XPLUS_IF_CTL);
    regtmp |= IF_TS_LOCAL;
    sysRegWrite(AR531XPLUS_IF_CTL, regtmp);
#endif
}


/******************************************************************************
* ae531x_unitLinkLost -- Called from PHY layer to notify the MAC layer
* that there are no longer any live links associated with a MAC.
*/
void
ae531x_unitLinkLost(int ethUnit)
{
    AE531X_PRINT(AE531X_DEBUG_LINK_CHANGE,
             ("enetmac%d link down\n", ethUnit));
}


/******************************************************************************
* ae531x_unitLinkGained -- Called from PHY layer to notify the MAC layer
* that there are 1 or more live links associated with a MAC.
*/
void
ae531x_unitLinkGained(int ethUnit)
{
#if CONFIG_AR5315
#define AE531X_POLL_MILLI_SECONDS 200 
    ae531x_MAC_t *MACInfo = ae531x_getMAcInfo(ethUnit);
    while(!MACInfo || !MACInfo->port_is_up)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout((AE531X_POLL_MILLI_SECONDS * HZ)/1000);
        MACInfo = ae531x_getMAcInfo(ethUnit);
    }
    ae531x_SetMACFromPhy(MACInfo);
#endif
    AE531X_PRINT(AE531X_DEBUG_LINK_CHANGE,
             ("enet%d link up\n", ethUnit));
}

/******************************************************************************
* ae531x_ethMacDefault -- Called from PHY layer to determine the default
* ethernet MAC.  On some "twisted" platforms, the only usable MAC is 1,
* while on others the usable MAC is 0.  Future boards may allow both MACs
* to be used; in this case, return -1 to indicate that there IS NO default
* MAC.
*/
int
ae531x_ethMacDefault(void)
{
    if (ae531x_twisted_enet())
        return 1;

    return 0;

}
