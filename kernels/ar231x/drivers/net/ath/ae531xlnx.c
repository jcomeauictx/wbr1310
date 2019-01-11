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
 * This is a fairly generic driver, but it's intended
 * for use in typical Atheros products.
 */

#include <linux/config.h>
#include <linux/module.h>
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
#include "ae531x.h"

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#ifdef DEBUG
void my_mvPhyShow(int ethUnit);
#endif

#ifdef READ_BOARD_CONFIG
static struct ar531x_boarddata *ar531x_boardConfig=NULL;
static char *radioConfig=NULL;
#endif

#define AE531X_LAN_PORT 0
#define AE531X_DEV_PER_MAC 1

/*
 * ae531x_MAC_state contains driver-specific linux-specific per-MAC information.
 * The OSinfo member of ae531x_MAC_t points to one of these.
 */
typedef struct ae531x_MAC_state
{
	int                         irq;
	struct tq_struct            restart_task;
	struct net_device_stats     stats;
	struct ae531x_dev_sw_state  *dev_sw_state[AE531X_DEV_PER_MAC];
	int                         primary_dev;
	ae531x_MAC_t                MACInfo; /* hardware state */
} ae531x_MAC_state_t;

/*
 * ae531x_dev_sw_state contains driver-specific linux-specific per-device
 * information.  The net_device priv member points to one of these, and
 * this structure contains a pointer to the associated MAC information.
 */

typedef struct ae531x_dev_sw_state
{
	int                     enetUnit;        /* system unit number "eth%d" */
	int                     unit_on_MAC;     /* MAC-relative unit number */
	struct net_device       *dev;
	ae531x_MAC_state_t      *MAC_state;      /* underlying MAC hw/sw state */
} ae531x_dev_sw_state_t;

/*
 * Driver-independent linux-specific per-ethernet device software information.
 */
static struct net_device * ae531x_MAC_dev[AR531X_NUM_ENET_MAC * AE531X_DEV_PER_MAC];

/* Driver-dependent per-MAC information */
static ae531x_MAC_state_t per_MAC_info[AR531X_NUM_ENET_MAC];

/*
 * Receive buffers need enough room to hold the following:
 * 1) a max MTU-sized packet.  
 * 2) space for an ethernet header
 * 3) room at the beginning of the receive buffer in order
 *    to facilitate cooperating drivers that need to PREpend
 *    data.
 * 4) Depending on configuration, we may need some additional
 *    room at the END of the rx buffer for phy-supplied
 *    trailers (if any). (c.f. CONFIG_VENETDEV)
 *
 * The DMA engine insists on 32-bit aligned RX buffers.
 * TBDXXX: With current code, the IP stack ends up looking
 * at misaligned headers with word operations.  The misaligned
 * reads are software-emulated via handle_adel_int.  We'd
 * rather align the buffers on a 16-bit boundary, but the
 * DMA engine doesn't permit it???
 */
#define ETH_MAX_MTU 1522	/* 1518 + 4 bytes vlan tag */
#define AE531X_RX_BUF_SIZE \
    (((RXBUFF_RESERVE + ETH_HLEN + ETH_MAX_MTU + PHY_TRAILER_SIZE) + 3) & ~3)

/* Forward references to local functions */
static void ae531x_TxReap(ae531x_MAC_state_t *MAC_state);
static int ae531x_phy_poll(void *data);
static int ae531x_MAC_stop(struct net_device *dev);
static int ae531x_MAC_open(struct net_device *dev);

/*******************************************************************************
* ae531x_MAC_poll checks for received packets, and sends data
* up the stack.
*/
int
ae531x_MAC_poll(struct net_device *dev, int *budget)
{
    struct sk_buff *skb;
    struct sk_buff *newskb;
    char *rxBufp;
    int unused_length;
    VIRT_ADDR   rxDesc;
    int length;
    ae531x_dev_sw_state_t *dev_sw_state;
    ae531x_MAC_state_t *MAC_state;
    ae531x_MAC_t *MACInfo;
    u32 cmdsts;
    int rx_limit;
    int rx_received;
    int rxDescCount;
    struct net_device *rxdev;
    int early_stop;
    int retval;
#ifdef DEBUG
	static int rxDescCountMax = 0;
#endif

    ARRIVE();

    dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
    MAC_state = dev_sw_state->MAC_state;
    MACInfo = &MAC_state->MACInfo;
    rx_limit = MAC_state->dev_sw_state[MAC_state->primary_dev]->dev->quota;
    rx_received = 0;

    rxDescCount = 0;

    early_stop = 0;
    do {
        ae531x_AckIntr(MACInfo, DmaIntRxCompleted);

        for(;!early_stop;) {
            rxDesc = MACInfo->rxQueue.curDescAddr;
            cmdsts = AE531X_DESC_STATUS_GET(KSEG1ADDR(rxDesc));

            AE531X_PRINT(AE531X_DEBUG_RX,
                  ("examine rxDesc %p with cmdsts=0x%x\n",
                   (void *)rxDesc, cmdsts));
    
            if (cmdsts & DescOwnByDma) {
                /* There's nothing left to process in the RX ring */
                goto rx_all_done;
            }

            rxDescCount++;

            AE531X_CONSUME_DESC((&MACInfo->rxQueue));
    
            A_DATA_CACHE_INVAL(rxDesc, AE531X_DESC_SIZE);

            /*  Process a packet */
            length = AE531X_DESC_STATUS_RX_SIZE(cmdsts) - ETH_CRC_LEN;
            if ( (cmdsts & (DescRxFirst |DescRxLast | DescRxErrors)) ==
                           (DescRxFirst | DescRxLast) ) {
                /* Descriptor status indicates "NO errors" */
                skb = AE531X_DESC_SWPTR_GET(rxDesc);
    
                /*
                 * Allocate a replacement skb.
                 * We want to get another buffer ready for Rx ASAP.
                 */
                newskb = (struct sk_buff *)ae531x_rxbuf_alloc(MACInfo, &rxBufp, &unused_length);
                if(newskb == NULL ) {
                    /*
                     * Give this descriptor back to the DMA engine,
                     * and drop the received packet.
                     */
                    MAC_state->stats.rx_dropped++;
                    AE531X_PRINT(AE531X_DEBUG_ERROR,
                              ("Can't allocate new skb\n"));
                } else {
                    AE531X_DESC_BUFPTR_SET(rxDesc, virt_to_bus(rxBufp));
                    AE531X_DESC_SWPTR_SET(rxDesc, newskb);
                }

                AE531X_DESC_STATUS_SET(rxDesc, DescOwnByDma);
                rxDesc = NULL; /* sanity -- cannot use rxDesc now */
                sysWbFlush();
    
                if (newskb == NULL) {
                    retval = 1;
                    goto rx_no_skbs;
                } else {
                    /* Sync data cache w.r.t. DMA */
                    A_DATA_CACHE_INVAL(skb->data, length);
        
                    rxdev = dev_sw_state->dev;

                    if (rxdev == NULL) {
                        /*
                         * We received a packet for a virtual enet device
                         * that is no longer up.  Ignore it.
                         */
                        kfree_skb(skb);
                        continue;
                    }

                    /* Advance data pointer to show that there's data here */
                    skb_put(skb, length);
                    skb->protocol = eth_type_trans(skb, rxdev);
                    skb->dev = rxdev;
                    rxdev->last_rx = jiffies;
                    rxdev->quota--;

                    if (rx_limit-- < 0) {
                        early_stop=1;
                        /* We've done enough for now -- more later */
                        AE531X_PRINT(AE531X_DEBUG_RX_STOP,
                            ("Enet%d RX early stop.  Quota=%d rxDescCount=%d budget=%d\n",
                             MACInfo->unit, dev->quota, rxDescCount, *budget));
                    }
                    rx_received++;
        
                    /* Send the data up the stack */
                    AE531X_PRINT(AE531X_DEBUG_RX,
                              ("Send data up stack: skb=%p data=%p length=%d\n",
                               (void *)skb, (void *)skb->data, length));

                    netif_receive_skb(skb);

                    MAC_state->stats.rx_packets++;
                    MAC_state->stats.rx_bytes += length;
                }
            } else {
                /* Descriptor status indicates ERRORS */
                MAC_state->stats.rx_errors++;
    
                if (cmdsts & (DescRxRunt | DescRxLateColl)) {
                    MAC_state->stats.collisions++;
                }
    
                if (cmdsts & DescRxLengthError) {
                    MAC_state->stats.rx_length_errors++;
                }
    
                if (cmdsts & DescRxCrc) {
                    MAC_state->stats.rx_crc_errors++;
                }
    
                if (cmdsts & DescRxDribbling) {
                    MAC_state->stats.rx_frame_errors++;
                }
				
				AE531X_DESC_STATUS_SET(rxDesc, DescOwnByDma);

                AE531X_PRINT(AE531X_DEBUG_ERROR,
                          ("Bad receive.  rxDesc=%p  cmdsts=0x%8.8x\n",
                           (void *)rxDesc, cmdsts));
            }
        }
    } while ((!early_stop) &&
             ae531x_ReadDmaReg(MACInfo, DmaStatus) & DmaIntRxCompleted);

rx_all_done:
    AE531X_PRINT(AE531X_DEBUG_RX, 
             ("rx done (%d)\n", rxDescCount));
    *budget -= rxDescCount;

    if (!early_stop) {
        netif_rx_complete(dev);

        ae531x_SetDmaReg(MACInfo, DmaIntrEnb,
                     DmaIeRxCompleted | DmaIeRxNoBuffer);
        ae531x_WriteDmaReg(MACInfo, DmaRxPollDemand, 0);
    }
    
    retval = early_stop;

rx_no_skbs:

    LEAVE();

#ifdef DEBUG
	if (rxDescCount > rxDescCountMax) {
		printk("max rx %d\n", rxDescCount);
		rxDescCountMax = rxDescCount;
	}
#endif

    return retval;
}

/*******************************************************************************
* ae531x_restart stops all ethernet devices associated with a physical MAC,
* then shuts down the MAC.  Then it re-opens all devices that were in use.
* TBDXXX: needs testing!
*/
static void
ae531x_restart(void *data)
{
    ae531x_MAC_t *MACInfo = (ae531x_MAC_t *)data;
    ae531x_MAC_state_t *MAC_state = (ae531x_MAC_state_t *)MACInfo->OSinfo;
    struct net_device *saved_dev[AE531X_DEV_PER_MAC];
    int i;

    for (i=0; i<AE531X_DEV_PER_MAC; i++) {
        if ((saved_dev[i] = MAC_state->dev_sw_state[i]->dev) != NULL) {
            ae531x_MAC_stop(saved_dev[i]);
        }
    }

    for (i=0; i<AE531X_DEV_PER_MAC; i++) {
        if (saved_dev[i])
            ae531x_MAC_open(saved_dev[i]);
    }
}

/*******************************************************************************
* ae531x_MAC_intr handle interrupts from an ethernet MAC.
* It checks MAC status registers, and dispatches as appropriate.
*/
void
ae531x_MAC_intr(int cpl, void *dev_id, struct pt_regs *regs)
{
	ae531x_MAC_state_t *MAC_state;
	ae531x_MAC_t *MACInfo;
	u32 regIsr;
	u32 regImr;
	u32 pendIntrs;

	ARRIVE();

	MACInfo = (ae531x_MAC_t *)dev_id;
	MAC_state = (ae531x_MAC_state_t *)MACInfo->OSinfo;

	for(;;) {
		/* Clear any unhandled intr causes. */
		ae531x_WriteDmaReg(MACInfo, DmaStatus, UnhandledIntrMask);

		regIsr = ae531x_ReadDmaReg(MACInfo, DmaStatus);
		regImr = ae531x_ReadDmaReg(MACInfo, DmaIntrEnb);
		pendIntrs = regIsr & regImr;

		AE531X_PRINT(AE531X_DEBUG_INT,
					 ("ethmac%d: intIsr=0x%8.8x intImr=0x%8.8x pendIntrs=0x%8.8x\n",
					  MACInfo->unit, regIsr, regImr, pendIntrs ));

		if ((pendIntrs & DmaAllIntCauseMask) == 0)
			break;

		if ((pendIntrs & DmaIntRxCompleted) ||
			(pendIntrs & DmaIntRxNoBuffer)) {
			if (netif_rx_schedule_prep(MAC_state->dev_sw_state[MAC_state->primary_dev]->dev)) {
				ae531x_ClearDmaReg(MACInfo,
								   DmaIntrEnb,
								   DmaIeRxCompleted | DmaIeRxNoBuffer);
				ae531x_AckIntr(MACInfo,
							   DmaIntRxCompleted | DmaIntRxNoBuffer);
				(void)ae531x_ReadDmaReg(MACInfo, DmaIntrEnb);
				__netif_rx_schedule(MAC_state->dev_sw_state[MAC_state->primary_dev]->dev);
			} else {
#if 0
				AE531X_PRINT(AE531X_DEBUG_ERROR,
							 ("%s: Interrupt (0x%8.8x/0x%8.8x) while in poll.  regs@%p, pc=%p, ra=%p\n",
							  __FILE__,
							  regIsr,
							  ae531x_ReadDmaReg(MACInfo, DmaIntrEnb),
							  (void *)regs,
							  (void *)regs->cp0_epc,
							  (void *)regs->regs[31]));
#endif
				ae531x_AckIntr(MACInfo,
							   DmaIntRxCompleted | DmaIntRxNoBuffer);
			}
		}

		if (pendIntrs &
			(DmaIntTxStopped | DmaIntTxJabber | DmaIntTxUnderflow)) {
			AE531X_PRINT(AE531X_DEBUG_ERROR,
						 ("ethmac%d: TX Error Intr (0x%x)\n",
						  MACInfo->unit, pendIntrs));
			ae531x_AckIntr(MACInfo,
						   (DmaIntTxStopped | DmaIntTxJabber | DmaIntTxUnderflow));
		}

		if (pendIntrs & DmaIntBusError) {
			AE531X_PRINT(AE531X_DEBUG_ERROR,
						 ("ethmac%d: DMA Bus Error Intr (0x%x)\n",
						  MACInfo->unit, pendIntrs));
			ae531x_AckIntr(MACInfo, DmaIntBusError);
			/* Reset the chip, if it's not already being done */
			if (ae531x_IsInResetMode(MACInfo)) {
				goto intr_done;
			}
			ae531x_BeginResetMode(MACInfo);
			schedule_task(&MAC_state->restart_task);
		}

		if (pendIntrs & DmaIntRxStopped) {
			AE531X_PRINT(AE531X_DEBUG_ERROR,
						 ("ethmac%d: RX Stopped Intr (0x%x)\n",
						  MACInfo->unit, pendIntrs));
			ae531x_AckIntr(MACInfo, DmaIntRxStopped);
		}
	}

 intr_done:
	LEAVE();
}

/*******************************************************************************
* ae531x_MAC_get_stats returns statistics for a specified device
*/
static struct net_device_stats*
ae531x_MAC_get_stats(struct net_device *dev)
{
        ae531x_dev_sw_state_t *dev_sw_state;
        ae531x_MAC_state_t *MAC_state;

        ARRIVE();
        dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
        MAC_state = dev_sw_state->MAC_state;

        LEAVE();
        return &MAC_state->stats;
}

#define AE531X_PHY_POLL_SECONDS 2

#if CONFIG_AR5315

/*******************************************************************************
* ae531x_getMACInfo returns the MACInfo  of the interface given by unit 
*/
ae531x_MAC_t *ae531x_getMAcInfo(int ethUnit)
{
  int i,j;
  for(i=0;i<AR531X_NUM_ENET_MAC;++i) {
    if(per_MAC_info[i].dev_sw_state) {
      for(j=0;j<AE531X_DEV_PER_MAC;++j) {
	 if(per_MAC_info[i].dev_sw_state[j]
	    && per_MAC_info[i].dev_sw_state[j]->enetUnit == ethUnit)
            return (&(per_MAC_info[i].MACInfo));
      }
    }
  }
  return NULL;
}
    

#endif

/*******************************************************************************
* ae531x_phy_poll periodically checks for changes in phy status
* (e.g. dropped link).
*/
static int
ae531x_phy_poll(void *data)
{
    ae531x_dev_sw_state_t *dev_sw_state = (ae531x_dev_sw_state_t *)data;
    ae531x_MAC_t *MACInfo = &dev_sw_state->MAC_state->MACInfo;
    int unit = dev_sw_state->enetUnit;

    for(;;) {
        if (MACInfo->port_is_up) {
            phyCheckStatusChange(unit);
        }

        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(AE531X_PHY_POLL_SECONDS * HZ);
    }

    return 0;
}


static char invalid_enet_MAC_addr[] = {0, 0, 0, 0, 0, 0};

/*
 * Fetch a pointer to an ethernet's MAC address
 * in the Board Configuration data (in flash).
 */
char * ae531x_enet_mac_address_get(int MACUnit)
{
#ifdef READ_BOARD_CONFIG
	/* XXX: Hack for poorly configured boards.
	 *      Cannot setup bridging properly (brctl) when both enet
	 *      interfaces share the same MAC address.
	 * 
	 */

#ifdef CONFIG_ASK_MULT_MAC_HACK
    static u8  enet0Mac[6] = {0x00, 0x0d, 0x0b, 0x13, 0x6b, 0x16};
    static u8  enet1Mac[6] = {0x00, 0x0d, 0x0b, 0x13, 0x6b, 0x17};
#endif

    if (!ar531x_boardConfig)
        return invalid_enet_MAC_addr;
    if (MACUnit == 0) {
#ifndef CONFIG_ASK_MULT_MAC_HACK
        return ar531x_boardConfig->enet0Mac;
#else
		return enet0Mac;
#endif
    }
    if (MACUnit == 1) {
#ifndef CONFIG_ASK_MULT_MAC_HACK
        return ar531x_boardConfig->enet1Mac;
#else
		return enet1Mac;
#endif
    }
    printk("Invalid ethernet MAC unit number (%d)!\n", MACUnit);
    return invalid_enet_MAC_addr;
#else	/* READ_BOARD_CONFIG */
    static u8  enet0Mac[6] = {0x00, 0x0d, 0x0b, 0x13, 0x6b, 0x16};
    static u8  enet1Mac[6] = {0x00, 0x0d, 0x0b, 0x13, 0x6b, 0x17};
    if (MACUnit == 0) return enet0Mac;
    if (MACUnit == 1) return enet1Mac;
    printk("Invalid ethernet MAC unit number (%d)!\n", MACUnit);
    return invalid_enet_MAC_addr;
#endif	/* READ_BOARD_CONFIG */
}



/*******************************************************************************
* ae531x_MAC_open is the standard Linux open function.  It puts
* hardware into a known good state, allocates queues, starts
* the phy polling task, and arranges for interrupts to be handled.
*/
static int ae531x_MAC_open(struct net_device *dev)
{
	ae531x_dev_sw_state_t *dev_sw_state;
	ae531x_MAC_state_t *MAC_state;
	ae531x_MAC_t *MACInfo;
	u8 *MACAddr;
	int rv;
	struct tq_struct *restart_task;
	pid_t phy_poll_pid;

	ARRIVE();

	dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
	dev_sw_state->dev = dev;
	MAC_state = dev_sw_state->MAC_state;
	MACInfo = &MAC_state->MACInfo;

	restart_task = &MAC_state->restart_task;
	restart_task->routine = ae531x_restart;
	restart_task->data = (void *)MACInfo;

	AE531X_PRINT(AE531X_DEBUG_RESET,
			("ae531x_MAC_open eth%d ethmac%d macBase=0x%x dmaBase=0x%x irq=0x%x\n",
			 dev_sw_state->enetUnit,
			 MACInfo->unit,
			 MACInfo->macBase,
			 MACInfo->dmaBase,
			 MAC_state->irq));

	/* Default MAC address */
#if 0
    MACAddr = ae531x_enet_mac_address_get(MACInfo->unit);
    memcpy(dev->dev_addr, MACAddr, dev->addr_len );
#else
	memcpy(MACInfo->dev_addr, dev->dev_addr, 6);
#endif
 
	if (!MACInfo->port_is_up)
	{
		/* Bring MAC and PHY out of reset */
		ae531x_reset(MACInfo);

		/* Attach interrupt handler */
		rv = request_irq(MAC_state->irq, ae531x_MAC_intr, SA_INTERRUPT,
				"ae531x_MAC_intr", (void *)MACInfo);
		if (rv < 0)
		{
			AE531X_PRINT(AE531X_DEBUG_ERROR, ("request_irq(0x%x) failed (%d)\n", MAC_state->irq, rv));
			goto open_failure;
        }

        /* Initialize PHY */
		AE531X_PRINT(AE531X_DEBUG_RESET, ("\n --- phyBase: %08x\n", MACInfo->phyBase));
		phySetup(MACInfo->unit, MACInfo->phyBase);
#if CONFIG_ICPLUS_ENET_POWER_DOWN_PHY0
		phyPowerDown(MACInfo->phyBase, 0);
#endif
#if CONFIG_ICPLUS_ENET_POWER_DOWN_PHY1
		phyPowerDown(MACInfo->phyBase, 1);
#endif
#if CONFIG_ICPLUS_ENET_POWER_DOWN_PHY2
		phyPowerDown(MACInfo->phyBase, 2);
#endif
#if CONFIG_ICPLUS_ENET_POWER_DOWN_PHY3
		phyPowerDown(MACInfo->phyBase, 3);
#endif
#if CONFIG_ICPLUS_ENET_POWER_DOWN_PHY4
		phyPowerDown(MACInfo->phyBase, 4);
#endif

		/* Start thread to poll for phy link status changes */
		phy_poll_pid = kernel_thread(ae531x_phy_poll, dev_sw_state, 0);
		if (phy_poll_pid < 0)
		{
			AE531X_PRINT(AE531X_DEBUG_ERROR, ("ethmac%d unable to start Phy Poll thread\n", MACInfo->unit));
		}

		/* Allocate RX/TX Queues */
		if (ae531x_AllocateQueues(MACInfo) < 0)
		{
			AE531X_PRINT(AE531X_DEBUG_RESET, ("Queue allocation failed"));
			free_irq(MAC_state->irq, (void *)MACInfo);
			goto open_failure;
		}

		/* Initialize DMA and descriptors */
		ae531x_DmaReset(MACInfo);

		/* Initialize MAC */
		ae531x_MACReset(MACInfo);

		/* Enable Receive/Transmit */
		ae531x_EnableComm(MACInfo);

		MAC_state->primary_dev = dev_sw_state->unit_on_MAC;
		MACInfo->port_is_up = TRUE;
	}

	dev->trans_start = jiffies;
	SET_MODULE_OWNER(dev);

	LEAVE();
	return 0;

open_failure:
	LEAVE();
	return -1;
}

/*
 * Shut down MAC hardware.
 */
static void
ae531x_MAC_shutdown(ae531x_MAC_state_t *MAC_state)
{
    ae531x_MAC_t *MACInfo;

    MACInfo = &MAC_state->MACInfo;
    MACInfo->port_is_up = FALSE;

    /* Disable Receive/Transmit */
    ae531x_DisableComm(MACInfo);

    /* Disable Interrupts */
    ae531x_DmaIntDisable(MACInfo);
    sysWbFlush();
    free_irq(MAC_state->irq, (void *)MACInfo);

    /* Free Transmit & Receive skb's/descriptors */
    ae531x_TxReap(MAC_state); /* one last time */
    ae531x_FreeQueues(MACInfo);
}

/*******************************************************************************
* ae531x_MAC_stop is the standard Linux stop function.  It undoes
* everything set up by ae531x_MAC_open.
*/
static int
ae531x_MAC_stop(struct net_device *dev)
{
    ae531x_dev_sw_state_t *dev_sw_state;
    ae531x_MAC_state_t *MAC_state;
    ae531x_MAC_t *MACInfo;
    int i;

    ARRIVE();

    dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
    MAC_state = dev_sw_state->MAC_state;
    MACInfo = &MAC_state->MACInfo;

    for (i=0; i<AE531X_DEV_PER_MAC; i++) {
        if ((MAC_state->dev_sw_state[i]->dev) &&
            (MAC_state->dev_sw_state[i]->dev != dev_sw_state->dev)) {
            break;
        }
    }

    if (i < AE531X_DEV_PER_MAC) {
        /* Physical MAC is still in use */
        if (MAC_state->primary_dev == dev_sw_state->unit_on_MAC) {
            /*
             * If the primary_dev is being stopped
             * then we need to assign a new one.
             */
            MAC_state->primary_dev = i;
        }
    } else {
        /* Physical MAC is no longer in use */
        ae531x_MAC_shutdown(MAC_state);
    }

    dev_sw_state->dev = NULL;

    LEAVE();
    return 0;
}

/*******************************************************************************
* ae531x_rxbuf_alloc - Allocate an skb to be associated with an RX descriptor.
*
* RETURNS: A pointer to the skb.  Also returns a pointer to the underlying
* buffer and the size of that buffer. 
*/
void *
ae531x_rxbuf_alloc(ae531x_MAC_t *MACInfo, char **rxBuffp, int *rxBuffSizep)
{
    int buf_size;
    struct sk_buff *skb;
    char *rxBuff;
    int rxBuffSize;

    buf_size = AE531X_RX_BUF_SIZE;

    skb = dev_alloc_skb(buf_size);
    if (skb) {
        /* skb->dev = dev; */
        skb_reserve(skb, RXBUFF_RESERVE);

        rxBuffSize = skb_tailroom(skb);
        rxBuff = skb->tail;

        *rxBuffp = rxBuff;
        *rxBuffSizep = rxBuffSize;
    }

    return skb;
}

/*******************************************************************************
* ae531x_swptr_free - Free the skb, if any, associated with a descriptor.
*/
void
ae531x_swptr_free(VIRT_ADDR desc)
{
    struct sk_buff *skb;

    skb = (struct sk_buff *)AE531X_DESC_SWPTR_GET(desc);
    if (skb) {
        AE531X_DESC_SWPTR_SET(desc, NULL);
        kfree_skb(skb);
    }
}

/*******************************************************************************
*
* ae531x_TxReap - the driver Tx completion routine.
*
* This routine reaps sk_buffs which have already been transmitted.
*
*/
static void
ae531x_TxReap(ae531x_MAC_state_t *MAC_state)
{
    AE531X_QUEUE      *txq;
    VIRT_ADDR         txDesc;
    UINT32            cmdsts;
    struct            sk_buff *skb;
    int               reaped;
    ae531x_MAC_t      *MACInfo;
    static int        aeUselessReap = 0;
#ifdef DEBUG
    static int        aeMaxReap = 0;
#endif
    ARRIVE();

    MACInfo = &MAC_state->MACInfo;
    txq = &MACInfo->txQueue;
    reaped = 0;

    while (1) {

        txDesc = AE531X_QUEUE_ELE_NEXT_GET(txq, txq->reapDescAddr);
        if (txDesc == txq->curDescAddr) {
            break;
        }

        cmdsts = AE531X_DESC_STATUS_GET(KSEG1ADDR(txDesc));
        if (cmdsts & DescOwnByDma) {
            break;
        }

        /* Release sk_buff associated with completed transmit */
        skb = (struct sk_buff *)AE531X_DESC_SWPTR_GET(txDesc);
        if (skb) {
            kfree_skb(skb);
            AE531X_DESC_SWPTR_SET(txDesc, NULL);
        }

        /* Update statistics according to completed transmit desc */
        if (cmdsts & DescTxErrors) {
            AE531X_PRINT(AE531X_DEBUG_ERROR,
                    ("enetmac%d Tx prior error: 0x%8.8x <0x%8.8x> 0x%8.8x\n",
                    MACInfo->unit,
                    cmdsts,
                    DescTxErrors,
                    (int)txDesc));
#ifdef DEBUG
			//my_mvPhyShow(MACInfo->unit);
	    printk ("ae531xMacControl: 0x%08x\tMacFlowControl: 0x%08x\n",
		    ae531x_ReadMacReg(MACInfo, MacControl),
		    ae531x_ReadMacReg(MACInfo, MacFlowControl));
#endif
            MAC_state->stats.tx_errors++;
            if (cmdsts & (DescTxLateCollision | DescTxExcCollisions)) {
                MAC_state->stats.tx_aborted_errors++;
            }
            if (cmdsts & (DescTxLostCarrier | DescTxNoCarrier)) {
                MAC_state->stats.tx_carrier_errors++;
            }
        } else {
            MAC_state->stats.tx_bytes += AE531X_DESC_STATUS_RX_SIZE(cmdsts);
            MAC_state->stats.tx_packets++;
        }

        MAC_state->stats.collisions +=
            ((cmdsts & DescTxCollMask) >> DescTxCollShift);

        txq->reapDescAddr = txDesc;
        reaped++;
    }

    if (reaped > 0) {
        int i;

#ifdef DEBUG
		if (reaped > aeMaxReap) {
			aeMaxReap = reaped;
			printk("max reaped = %d\n", reaped);
		}
#endif
        AE531X_PRINT(AE531X_DEBUG_TX_REAP,
             ("reaped %d\n", reaped));

        /*
         * Re-start transmit queues for all ethernet devices
         * associated with this MAC.
         */
        for (i=0; i<AE531X_DEV_PER_MAC; i++) {
            if (MAC_state->dev_sw_state[i]->dev)
                netif_start_queue(MAC_state->dev_sw_state[i]->dev);
        }
    } else {
        aeUselessReap++;
    }

    LEAVE();
}


/*******************************************************************************
* ae531x_MAC_start_xmit sends a packet.
*/
static int
ae531x_MAC_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    ae531x_dev_sw_state_t *dev_sw_state;
    ae531x_MAC_state_t *MAC_state;
    ae531x_MAC_t *MACInfo;
    u32 buf;
    u32 ctrlen;
    u32 length;
    int mtu;
    int max_buf_size;
    VIRT_ADDR txDesc;

    ARRIVE();

    dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
    MAC_state = dev_sw_state->MAC_state;
    MACInfo = &MAC_state->MACInfo;

    length = skb->len;

    /* Check if this port is up, else toss packet */
    if (!MACInfo->port_is_up) {
        buf = virt_to_bus(skb->data);
        AE531X_PRINT(AE531X_DEBUG_ERROR,
                  ("eth%d Tx Down, dropping buf=0x%8.8x, length=0x%8.8x, skb=%p\n",
                   dev_sw_state->enetUnit, buf, length, (void *)skb));

        MAC_state->stats.tx_dropped++;
        MAC_state->stats.tx_carrier_errors++;
        goto dropFrame;
    }

    if (ae531x_IsInResetMode(MACInfo)) {
        AE531X_PRINT(AE531X_DEBUG_ERROR,
                  ("eth%d Tx: In Chip reset - drop frame\n",
                   dev_sw_state->enetUnit));

        MAC_state->stats.tx_dropped++;
        MAC_state->stats.tx_aborted_errors++;
        goto dropFrame;
    }

    /* Check if we can transport this packet */
    length = max((u32)60, length);  /* total length */
    mtu = dev->mtu;
    max_buf_size = mtu + ETH_HLEN;
    if (length > max_buf_size) {
        AE531X_PRINT(AE531X_DEBUG_ERROR,
                  ("eth%d Tx: length %d too long.  mtu=%d, trailer=%d\n",
                   dev_sw_state->enetUnit, length, mtu, PHY_TRAILER_SIZE));

        MAC_state->stats.tx_errors++;
        MAC_state->stats.tx_aborted_errors++;

        goto dropFrame;
    }

	/* Reap any old, completed Tx descriptors */
	ae531x_TxReap(MAC_state);

    txDesc = MACInfo->txQueue.curDescAddr;
    if (txDesc == MACInfo->txQueue.reapDescAddr) {
#if 0
        int i;
#endif

        AE531X_PRINT(AE531X_DEBUG_ERROR,
                  ("eth%d Tx: cannot get txDesc\n",
                   dev_sw_state->enetUnit));

        MAC_state->stats.tx_dropped++;
        MAC_state->stats.tx_fifo_errors++;

        /*
         * Stop transmit queues for any ethernet devices
         * associated with this MAC.
         */
#if 0 /* XXX: no way to recover from queue stop until ae531x_MAC_tx_timeout()
	   *      is rewritten to avoid calls to shedule().
	   */
        for (i=0; i<AE531X_DEV_PER_MAC; i++) {
            if (MAC_state->dev_sw_state[i]->dev)
                netif_stop_queue(MAC_state->dev_sw_state[i]->dev);
        }
#endif
        goto dropFrame;
    }

    /* We won't fail now; so consume this descriptor */
    AE531X_CONSUME_DESC((&MACInfo->txQueue));

    /* Update the descriptor */
    buf = virt_to_bus(skb->data);
    AE531X_DESC_BUFPTR_SET(txDesc, buf);
    AE531X_DESC_SWPTR_SET(txDesc, skb);
    ctrlen = AE531X_DESC_CTRLEN_GET(txDesc);
    ctrlen = (ctrlen & (DescEndOfRing)) |
                            DescTxFirst |
                             DescTxLast |
                        DescTxIntEnable;

    ctrlen |= ((length << DescSize1Shift) & DescSize1Mask);

    AE531X_DESC_CTRLEN_SET(txDesc, ctrlen);
    AE531X_DESC_STATUS_SET(txDesc, DescOwnByDma);

    /* Alert DMA engine to resume Tx */
    ae531x_WriteDmaReg(MACInfo, DmaTxPollDemand, 0);
    sysWbFlush();

    AE531X_PRINT(AE531X_DEBUG_TX,
              ("eth%d Tx: Desc=0x%8.8x, L=0x%8.8x, D=0x%8.8x, d=0x%8.8x, length=0x%8.8x\n",
               dev_sw_state->enetUnit,
               (UINT32)txDesc,
               AE531X_DESC_CTRLEN_GET(txDesc),
               buf,
               AE531X_DESC_LNKBUF_GET(txDesc),
               length));

    /* Tell upper layers to keep it coming */
    dev->trans_start = jiffies;

    LEAVE();

    return 0;

dropFrame:
    kfree_skb(skb);
    LEAVE();
    return 0;
}

#if 0
/*******************************************************************************
* ae531x_MAC_tx_timeout handles transmit timeouts
*/
static void
ae531x_MAC_tx_timeout(struct net_device *dev)
{
    ae531x_dev_sw_state_t *dev_sw_state;
    ae531x_MAC_state_t *MAC_state;
    ae531x_MAC_t *MACInfo;

    ARRIVE();

    dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
    MAC_state = dev_sw_state->MAC_state;
    MACInfo = &MAC_state->MACInfo;

    AE531X_PRINT(AE531X_DEBUG_ERROR,
             ("enet%d: Tx timeout\n", dev_sw_state->enetUnit));

    ae531x_restart(MACInfo);

    LEAVE();
}
#endif


/*******************************************************************************
* ae531x_MAC_do_ioctl is a placeholder for future ioctls.
*/
static int
ae531x_MAC_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
        int rv;
        ae531x_MAC_t *MACInfo;
        struct ioctl_data {
                u32 unit;
                u32 addr;
                u32 data;
        } *req;
        ae531x_dev_sw_state_t *dev_sw_state;
        ae531x_MAC_state_t *MAC_state;

        ARRIVE();

        dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
        MAC_state = dev_sw_state->MAC_state;
        MACInfo = &MAC_state->MACInfo;

        req = (struct ioctl_data *)ifr->ifr_data;

        switch( cmd ) {
        default:
            AE531X_PRINT(AE531X_DEBUG_ERROR,
                     ("Unsupported ioctl: 0x%x\n", cmd));
            rv = -EOPNOTSUPP;
        }

        LEAVE();
        return rv;
}

static int ae531x_change_mtu(struct net_device * dev, int new_mtu)
{
	if ((new_mtu < 48) || (new_mtu > 1518))
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}

static void ae531x_MAC_setup_fntable(struct net_device *dev)
{
	ARRIVE();

	dev->get_stats		= ae531x_MAC_get_stats;
	dev->open			= ae531x_MAC_open;
	dev->stop			= ae531x_MAC_stop;
	dev->hard_start_xmit= ae531x_MAC_start_xmit;
	dev->do_ioctl		= ae531x_MAC_do_ioctl;
	dev->poll			= ae531x_MAC_poll;
	dev->weight			= 16;
#if 0 /* XXX: currently, ae531x_MAC_tx_timeout() will call functions
	   *      that in turn call schedule(). this is BAD, since the
	   *      timeout call runs at interrupt time. until ae531x_MAC_tx_timeout
	   *      is rewritten to avoid schedule() calls, we do not use it. */
	dev->tx_timeout		= ae531x_MAC_tx_timeout;
#else
	dev->tx_timeout		= NULL;
#endif
	dev->features		= NETIF_F_HW_CSUM | NETIF_F_HIGHDMA;
	dev->change_mtu		= ae531x_change_mtu;

	LEAVE();
}

#ifdef READ_BOARD_CONFIG
static void
ar5312EepromRead(char *EepromAddr, u_int16_t id, unsigned int off, 
		 unsigned int nbytes, char *data)
{
	int i;
	
	for (i=0; i<nbytes; i++, off++) {
		data[i] = EepromAddr[off];
	}
}
#endif

int ae531x_get_numMACs(void)
{
#ifdef READ_BOARD_CONFIG
    int devid;
    u16 radioMask;

    /* Probe to find out the silicon revision and enable the
       correct number of macs */
    devid = ((u_int16_t) ((sysRegRead(AR531X_REV) >>8) & 
			  (AR531X_REV_MAJ | AR531X_REV_MIN)));
    switch (devid) {
    case AR5212_AR5312_REV2:
    case AR5212_AR5312_REV7:
        /* Need to determine if we have a 5312 or a 2312 since they
           have the same Silicon Rev ID*/
        ar5312EepromRead(radioConfig,0,2*AR531X_RADIO_MASK_OFF,2,
			 (char *) &radioMask);
        if ((radioMask & AR531X_RADIO0_MASK) != 0) {
            return 2;
        }
        return 1;
    case AR5212_AR2313_REV8:
        return 1;
    }
#endif
    /* default to 1 */
    return 1;
}

BOOL
ae531x_twisted_enet(void)
{
    int wisoc_revision;

    wisoc_revision = (sysRegRead(AR531X_REV) & AR531X_REV_MAJ) >> AR531X_REV_MAJ_S;
    if ( (wisoc_revision == AR531X_REV_MAJ_AR2313) ||
         /* next clause is used to determine AR2312, based on number of MACs. 
          * must do this since revision is same for 5312 and 2312.
          */
         (wisoc_revision == AR531X_REV_MAJ_AR5312 && ae531x_get_numMACs() == 1) ) {
        return TRUE;
    } else {
        return FALSE;
    }
}

#ifdef READ_BOARD_CONFIG
int
ae531x_get_board_config(void)
{
    int dataFound;
    char *bd_config;

    /*
     * Find start of Board Configuration data, using heuristics:
     * Search back from the (aliased) end of flash by 0x1000 bytes
     * at a time until we find the string "5311", which marks the
     * start of Board Configuration.  Give up if we've searched
     * more than 500KB.
     */
    dataFound = 0;
    for (bd_config = (char *)0xbffff000;
         bd_config > (char *)0xbff80000;
         bd_config -= 0x1000)
    {
        if ( *(int *)bd_config == AR531X_BD_MAGIC) {
            dataFound = 1;
            break;
        }
    }

    if (!dataFound) {
        printk("Could not find Board Configuration Data\n");
		bd_config = NULL;
    }
	
    ar531x_boardConfig = (struct ar531x_boarddata *) bd_config;
	
    return(dataFound);
}

int
ae531x_get_radio_config(void)
{
    int dataFound;
    char *radio_config;

    /* 
     * Now find the start of Radio Configuration data, using heuristics:
     * Search forward from Board Configuration data by 0x1000 bytes
     * at a time until we find non-0xffffffff.
     */
    dataFound = 0;
    for (radio_config = ((char *) ar531x_boardConfig) + 0x1000;
         radio_config < (char *)0xbffff000;
         radio_config += 0x1000)
    {
        if (*(int *)radio_config != 0xffffffff) {
            dataFound = 1;
            break;
        }
    }

    if (!dataFound) { /* AR2316 relocates radio config to new location */
	dataFound = 0;
	for (radio_config = ((char *) ar531x_boardConfig) + 0xf8;
	     radio_config < (char *)0xbffff0f8;
	     radio_config += 0x1000)
	{
	    if (*(int *)radio_config != 0xffffffff) {
		dataFound = 1;
		break;
	    }
	}
    }

    if (!dataFound) {
        printk("Could not find Radio Configuration data\n");
	radio_config = NULL;
    }
    radioConfig = radio_config;
    return(dataFound);
}
#endif	/* READ_BOARD_CONFIG */

extern int ae531x_proc_fs_init(long phy_base);

static int __init ae531x_MAC_setup(void)
{
	int next_dev, i;
	struct net_device *dev;
	ae531x_dev_sw_state_t *dev_sw_state;
	ae531x_MAC_state_t *MAC_state;
	ae531x_MAC_t *MACInfo;
	char *addr;

	ARRIVE();
	for (i=0;i<AR531X_NUM_ENET_MAC * AE531X_DEV_PER_MAC; i++)
		ae531x_MAC_dev[i] = NULL;

#ifdef READ_BOARD_CONFIG
    if (!ae531x_get_board_config())
	{
		LEAVE();
		return -1;
	}
	if (!ae531x_get_radio_config())
	{
		LEAVE();
		return(-1);
    }
#endif

	for (i=0, next_dev = AR531X_NUM_ENET_MAC-1;
		 i < ae531x_get_numMACs() && next_dev >= 0;
		 i++, next_dev--)
	{
		/* if MAC is bogus in config data, skip */
		addr = ae531x_enet_mac_address_get(next_dev);
		if ((*(u32 *)addr == 0xffffffff) && (*(u16 *)(addr+4)==0xffff))
		{
			/* bogus MAC config data */
			continue;
		}

		dev = ae531x_MAC_dev[next_dev] =
			init_etherdev(NULL, sizeof(ae531x_dev_sw_state_t));

		if (dev == NULL)
		{
			LEAVE();
			return -1;
		}

		memcpy(dev->dev_addr, addr, 6);

		ae531x_MAC_setup_fntable(dev);

		dev_sw_state = (ae531x_dev_sw_state_t *)dev->priv;
		dev_sw_state->enetUnit = next_dev;
		dev_sw_state->unit_on_MAC = 0;
		MAC_state = &per_MAC_info[next_dev];
		dev_sw_state->MAC_state = MAC_state;
		MAC_state->dev_sw_state[AE531X_LAN_PORT] = dev_sw_state;
		MAC_state->primary_dev = -1;

		/* Initialize per-MAC information */
		MACInfo = &MAC_state->MACInfo;
		MACInfo->unit = next_dev;

		if (MACInfo->unit == 0)
		{
			MACInfo->macBase = (u32)(PHYS_TO_K1(AR531X_ENET0)+AE531X_MAC_OFFSET);
			MACInfo->dmaBase = (u32)(PHYS_TO_K1(AR531X_ENET0)+AE531X_DMA_OFFSET);
			MACInfo->phyBase = (u32)(PHYS_TO_K1(AR531X_ENET0)+AE531X_PHY_OFFSET);
			MAC_state->irq = AR531X_IRQ_ENET0_INTRS;
        }
		else
		{
#ifndef CONFIG_AR5315
			MACInfo->macBase = (u32) (PHYS_TO_K1(AR531X_ENET1)+AE531X_MAC_OFFSET);
			MACInfo->dmaBase = (u32) (PHYS_TO_K1(AR531X_ENET1)+AE531X_DMA_OFFSET);
			if (ae531x_twisted_enet())
			{
				MACInfo->phyBase = (u32)(PHYS_TO_K1(AR531X_ENET0)+AE531X_PHY_OFFSET);
			}
			else
			{
				MACInfo->phyBase = (u32)(PHYS_TO_K1(AR531X_ENET1)+AE531X_PHY_OFFSET);
			}
			MAC_state->irq = AR531X_IRQ_ENET1_INTRS;
#endif
        }

		MACInfo->OSinfo = (void *)MAC_state;
    }

	ae531x_proc_fs_init(MACInfo->phyBase);
	dev->mtu = 1504;  /* set default mtu to 1504 (4 bytes extended for VLAN) */

	LEAVE();
	return 0;
}
module_init(ae531x_MAC_setup);

/*******************************************************************************
* ae531x_MAC_unload is the module unload function
*/
static void __exit ae531x_MAC_unload(void)
{
	int i;

	for (i=0;i<AR531X_NUM_ENET_MAC * AE531X_DEV_PER_MAC; i++)
	{
		if (ae531x_MAC_dev[i] != NULL)
		{
			ae531x_MAC_stop(ae531x_MAC_dev[i]);
			ae531x_MAC_dev[i] = NULL;
		}
	}
}

MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Support for Atheros WiSoC Ethernet device");
#ifdef MODULE_LICENSE
MODULE_LICENSE("Atheros");
#endif
module_exit(ae531x_MAC_unload);
