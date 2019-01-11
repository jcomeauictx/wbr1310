/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
* Manage the ethernet PHY switch, Marvell 88E6060.
* 
* This module is intended to be largely OS and platform-independent.
*/

#if defined(linux)
#include <linux/config.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>

#include "ar531xlnx.h"
#endif

#if defined(__ECOS)
#include "ae531xecos.h"
#endif


#include "ae531xmac.h"
#include "ae531xreg.h"
#include "mvPhy.h"

#if /* DEBUG */ 1
#define MV_DEBUG_ERROR     0x00000001
#define MV_DEBUG_PHYSETUP  0x00000002
#define MV_DEBUG_PHYCHANGE 0x00000004

int mvPhyDebug = MV_DEBUG_ERROR;

#define MV_PRINT(FLG, X)                            \
{                                                   \
    if (mvPhyDebug & (FLG)) {                       \
        DEBUG_PRINTF X;                             \
    }                                               \
}
#else
#define MV_PRINT(FLG, X)
#endif

#if CONFIG_VENETDEV
/*
 * On AR5312 with CONFIG_VENETDEV==1,
 *   ports 0..3 are LAN ports  (accessed through ae0)
 *   port 4 is the WAN port.   (accessed through ae1)
 * 
 * The phy switch settings in the mvPhyInfo table are set accordingly.
 */
#define MV_WAN_PORT          4
#define MV_IS_LAN_PORT(port) ((port) <  MV_WAN_PORT)
#define MV_IS_WAN_PORT(port) ((port) == MV_WAN_PORT)
#endif

/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    UINT32 phyBase;
    UINT32 phyAddr;          /* PHY registers associated with this phy port */
    UINT32 switchPortAddr;   /* switch port regs assoc'ed with this phy port */
    UINT32 VLANTableSetting; /* Value to be written to VLAN table */
} mvPhyInfo_t;

/******************************************************************************
 * Per-PHY information, indexed by PHY unit number.
 *
 * This table is board-dependent.  It includes information
 * about which enet MAC controls which PHY port.
 */
mvPhyInfo_t mvPhyInfo[] = {
    /*
     * On AP30/AR5312, all PHYs are associated with MAC0.
     * AP30/AR5312's MAC1 isn't used for anything.
     * CONFIG_VENETDEV==1 (router) configuration:
     *    Ports 0,1,2, and 3 are "LAN ports"
     *    Port 4 is a WAN port
     *    Port 5 connects to MAC0 in the AR5312
     * CONFIG_VENETDEV==0 (bridge) configuration:
     *    Ports 0,1,2,3,4 are "LAN ports"
     *    Port 5 connects to the MAC0 in the AR5312
     */
    {isEnetPort: TRUE,   /* phy port 0 -- LAN port 0 */
     isPhyAlive: FALSE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x10,
     switchPortAddr: 0x18,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x2e
#else
     VLANTableSetting: 0x3e
#endif
    },

    {isEnetPort: TRUE,   /* phy port 1 -- LAN port 1 */
     isPhyAlive: FALSE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x11,
     switchPortAddr: 0x19,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x2d
#else
     VLANTableSetting: 0x3d
#endif
    },

    {isEnetPort: TRUE,   /* phy port 2 -- LAN port 2 */
     isPhyAlive: FALSE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x12, 
     switchPortAddr: 0x1a,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x2b
#else
     VLANTableSetting: 0x3b
#endif
    },

    {isEnetPort: TRUE,   /* phy port 3 -- LAN port 3 */
     isPhyAlive: FALSE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x13, 
     switchPortAddr: 0x1b,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x27
#else
     VLANTableSetting: 0x37
#endif
    },

    {isEnetPort: TRUE,   /* phy port 4 -- WAN port or LAN port 4 */
     isPhyAlive: FALSE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x14, 
     switchPortAddr: 0x1c,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x1020  /* WAN port */
#else
     VLANTableSetting: 0x2f    /* LAN port 4 */
#endif
    },

    {isEnetPort: FALSE,  /* phy port 5 -- CPU port (no RJ45 connector) */
     isPhyAlive: TRUE,
     ethUnit: 0,
     phyBase: 0,
     phyAddr: 0x15, 
     switchPortAddr: 0x1d,
#if CONFIG_VENETDEV
     VLANTableSetting: 0x0f    /* Send only to LAN ports */
#else
     VLANTableSetting: 0x1f    /* Send to all ports */
#endif
    },
};

#define MV_PHY_MAX (sizeof(mvPhyInfo) / sizeof(mvPhyInfo[0]))

/* Range of valid PHY IDs is [MIN..MAX] */
#define MV_ID_MIN 0
#define MV_ID_MAX (MV_PHY_MAX-1)

/* Convenience macros to access myPhyInfo */
#define MV_IS_ENET_PORT(phyUnit) (mvPhyInfo[phyUnit].isEnetPort)
#define MV_IS_PHY_ALIVE(phyUnit) (mvPhyInfo[phyUnit].isPhyAlive)
#define MV_ETHUNIT(phyUnit) (mvPhyInfo[phyUnit].ethUnit)
#define MV_PHYBASE(phyUnit) (mvPhyInfo[phyUnit].phyBase)
#define MV_PHYADDR(phyUnit) (mvPhyInfo[phyUnit].phyAddr)
#define MV_SWITCH_PORT_ADDR(phyUnit) (mvPhyInfo[phyUnit].switchPortAddr)
#define MV_VLAN_TABLE_SETTING(phyUnit) (mvPhyInfo[phyUnit].VLANTableSetting)

#define MV_IS_ETHUNIT(phyUnit, ethUnit) \
    (MV_IS_ENET_PORT(phyUnit) &&        \
    MV_ETHUNIT(phyUnit) == (ethUnit))


/* Forward references */
BOOL       mv_phyIsLinkAlive(int phyUnit);
LOCAL void mv_VLANInit(int ethUnit);
LOCAL void mv_enableConfiguredPorts(int ethUnit);
LOCAL void mv_verifyReady(int ethUnit);
BOOL       mv_phySetup(int ethUnit, UINT32 phyBase);
int        mv_phyIsFullDuplex(int ethUnit);
BOOL       mv_phyIsSpeed100(int phyUnit);
LOCAL BOOL mv_validPhyId(int phyUnit);
void       mv_flushATUDB(int phyUnit);
void       mv_phyCheckStatusChange(int ethUnit);
#if DEBUG
void       mv_phyShow(int phyUnit);
void       mv_phySet(int phyUnit, UINT32 regnum, UINT32 value);
void       mv_switchPortSet(int phyUnit, UINT32 regnum, UINT32 value);
void       mv_switchGlobalSet(int phyUnit, UINT32 regnum, UINT32 value);
void       mv_showATUDB(int phyUnit);
void       mv_countGoodFrames(int phyUnit);
void       mv_countBadFrames(int phyUnit);
void       mv_showFrameCounts(int phyUnit);
#endif


/******************************************************************************
*
* mv_phyIsLinkAlive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/
BOOL
mv_phyIsLinkAlive(int phyUnit)
{
    UINT16 phyHwStatus;
    UINT32 phyBase;
    UINT32 phyAddr;

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    phyHwStatus = phyRegRead(phyBase, phyAddr, MV_PHY_SPECIFIC_STATUS);

    if (phyHwStatus & MV_STATUS_REAL_TIME_LINK_UP) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/******************************************************************************
*
* mv_VLANInit - initialize "port-based VLANs" for the specified enet unit.
*/
LOCAL void
mv_VLANInit(int ethUnit)
{
    int     phyUnit;
    UINT32  phyBase;
    UINT32  switchPortAddr;

    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (MV_ETHUNIT(phyUnit) != ethUnit) {
            continue;
        }

        phyBase        = MV_PHYBASE(phyUnit);
        switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);

        phyRegWrite(phyBase, switchPortAddr, MV_PORT_BASED_VLAN_MAP,
                    MV_VLAN_TABLE_SETTING(phyUnit));
    }
}

#define phyPortConfigured(phyUnit) TRUE /* TBDFREEDOM2 */

/******************************************************************************
*
* mv_enableConfiguredPorts - enable whichever PHY ports are supposed
* to be enabled according to administrative configuration.
*/
LOCAL void
mv_enableConfiguredPorts(int ethUnit)
{
    int     phyUnit;
    UINT32  phyBase;
    UINT32  switchPortAddr;
    UINT16  portControl;
    UINT16  portAssociationVector;

    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (MV_ETHUNIT(phyUnit) != ethUnit) {
            continue;
        }

        phyBase        = MV_PHYBASE(phyUnit);
        switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);

        if (phyPortConfigured(phyUnit)) {

            portControl = MV_PORT_CONTROL_PORT_STATE_FORWARDING;
#if CONFIG_VENETDEV
            if (!MV_IS_ENET_PORT(phyUnit)) {       /* CPU port */
                portControl |= MV_PORT_CONTROL_INGRESS_TRAILER
                            | MV_PORT_CONTROL_EGRESS_MODE;
            }
#endif
            phyRegWrite(phyBase, switchPortAddr, MV_PORT_CONTROL, portControl);

            portAssociationVector = 1 << phyUnit;

            phyRegWrite(phyBase, switchPortAddr,
                MV_PORT_ASSOCIATION_VECTOR, portAssociationVector);
        }
    }
}

/******************************************************************************
*
* mv_verifyReady - validates that we're dealing with the device
* we think we're dealing with, and that it's ready.
*/
LOCAL void
mv_verifyReady(int ethUnit)
{
    int     phyUnit;
    UINT16  globalStatus;
    UINT32  phyBase = 0;
    UINT32  phyAddr;
    UINT32  switchPortAddr;
    UINT16  phyID1;
    UINT16  phyID2;
    UINT16  switchID;

    /*
     * The first read to the Phy port registers always fails and
     * returns 0.   So get things started with a bogus read.
     */
    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }
        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);
    
        (void)phyRegRead(phyBase, phyAddr, MV_PHY_ID1); /* returns 0 */
        break;
    }

    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        /*******************/
        /* Verify phy port */
        /*******************/
        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);
    
        phyID1 = phyRegRead(phyBase, phyAddr, MV_PHY_ID1);
        if (phyID1 != MV_PHY_ID1_EXPECTATION) {
            MV_PRINT(MV_DEBUG_PHYSETUP,
                      ("Invalid PHY ID1 for ethmac%d port%d.  Expected 0x%04x, read 0x%04x\n",
                       ethUnit,
                       phyUnit,
                       MV_PHY_ID1_EXPECTATION,
                       phyID1));
            return;
        }
    
        phyID2 = phyRegRead(phyBase, phyAddr, MV_PHY_ID2);
        if ((phyID2 & MV_OUI_LSB_MASK) != MV_OUI_LSB_EXPECTATION) {
            MV_PRINT(MV_DEBUG_PHYSETUP,
                      ("Invalid PHY ID2 for ethmac%d port %d.  Expected 0x%04x, read 0x%04x\n",
                       ethUnit,
                       phyUnit,
                       MV_OUI_LSB_EXPECTATION,
                       phyID2));
            return;
        }
    
        MV_PRINT(MV_DEBUG_PHYSETUP,
                  ("Found PHY ethmac%d port%d: model 0x%x revision 0x%x\n",
                   ethUnit,
                   phyUnit,
                   (phyID2 & MV_MODEL_NUM_MASK) >> MV_MODEL_NUM_SHIFT,
                   (phyID2 & MV_REV_NUM_MASK) >> MV_REV_NUM_SHIFT));
    
    
        /**********************/
        /* Verify switch port */
        /**********************/
        switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);
    
        switchID = phyRegRead(phyBase, switchPortAddr, MV_SWITCH_ID);
        if ((switchID & MV_SWITCH_ID_DEV_MASK) !=
            MV_SWITCH_ID_DEV_EXPECTATION) {
    
            MV_PRINT(MV_DEBUG_PHYSETUP,
                      ("Invalid switch ID for ethmac%d port %d.  Expected 0x%04x, read 0x%04x\n",
                       ethUnit,
                       phyUnit,
                       MV_SWITCH_ID_DEV_EXPECTATION,
                       switchID));
            return;
        }
    
        MV_PRINT(MV_DEBUG_PHYSETUP,
                  ("Found PHY switch for enet %d port %d deviceID 0x%x revision 0x%x\n",
                    ethUnit,
                    phyUnit,
                    (switchID & MV_SWITCH_ID_DEV_MASK) >> MV_SWITCH_ID_DEV_SHIFT,
                    (switchID & MV_SWITCH_ID_REV_MASK) >> MV_SWITCH_ID_REV_SHIFT))
    }
    
    /*******************************/
    /* Verify that switch is ready */
    /*******************************/
    if (phyBase) {
        globalStatus = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR,
                                  MV_SWITCH_GLOBAL_STATUS);

        if (!(globalStatus & MV_SWITCH_STATUS_READY_MASK)) {
            MV_PRINT(MV_DEBUG_PHYSETUP,
                      ("PHY switch for ethmac%d NOT ready!\n",
                       ethUnit));
        }
    } else {
        MV_PRINT(MV_DEBUG_PHYSETUP,
                  ("No ports configured for ethmac%d\n", ethUnit));
    }
}

/******************************************************************************
*
* mv_phySetup - reset and setup the PHY switch.
*
* Resets each PHY port.
*
* RETURNS:
*    TRUE  --> at least 1 PHY with LINK
*    FALSE --> no LINKs on this ethernet unit
*/
BOOL
mv_phySetup(int ethUnit, UINT32 phyBase)
{
    int     phyUnit;
    int     liveLinks = 0;
    BOOL    foundPhy = FALSE;
    UINT32  phyAddr;
    UINT16  atuControl;

    /*
     * Allow platform-specific code to determine the default Ethernet MAC
     * at run-time.  If phyEthMacDefault returns a negative value, use the
     * static mvPhyInfo table "as is".  But if phyEthMacDefault returns a
     * non-negative value, use it as the default ethernet unit.
     */
    {
        int ethMacDefault = phyEthMacDefault();

        if (ethMacDefault >= 0) {
            for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
                MV_ETHUNIT(phyUnit)=ethMacDefault;
            }
        }
    }

    /*
     * See if there's any configuration data for this enet,
     * and set up phyBase in table.
     */
    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (MV_ETHUNIT(phyUnit) != ethUnit) {
            continue;
        }

        MV_PHYBASE(phyUnit) = phyBase;
        foundPhy = TRUE;
    }

    if (!foundPhy) {
        return FALSE; /* No PHY's configured for this ethUnit */
    }

    /* Verify that the switch is what we think it is, and that it's ready */
    mv_verifyReady(ethUnit);

    /* Initialize global switch settings */
    atuControl  = MV_ATUCTRL_AGE_TIME_DEFAULT << MV_ATUCTRL_AGE_TIME_SHIFT;
    atuControl |= MV_ATUCTRL_ATU_SIZE_DEFAULT << MV_ATUCTRL_ATU_SIZE_SHIFT;
    phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_CONTROL, atuControl);

    /* Reset PHYs and start autonegoation on each. */
    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (MV_ETHUNIT(phyUnit) != ethUnit) {
            continue;
        }

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);

        phyRegWrite(phyBase, phyAddr, MV_PHY_CONTROL,
                    MV_CTRL_SOFTWARE_RESET | MV_CTRL_AUTONEGOTIATION_ENABLE);
    }

#if 0 /* Don't wait -- we'll detect shortly after the link comes up */
{
    int timeout;
    UINT16  phyHwStatus;

    /*
     * Wait 5 seconds for ALL associated PHYs to finish autonegotiation.
     */
    timeout=50;
    for (phyUnit=0; (phyUnit < MV_PHY_MAX) && (timeout > 0); phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }
        for (;;) {
            phyBase = MV_PHYBASE(phyUnit);
            phyAddr = MV_PHYADDR(phyUnit);

            phyHwStatus = phyRegRead(phyBase, phyAddr, MV_PHY_SPECIFIC_STATUS);

            if (MV_AUTONEG_DONE(phyHwStatus)) {
                break;
            }

            if (--timeout == 0) {
                break;
            }

            sysMsDelay(100);
        }
    }
}
#endif

    /*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */
    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (mv_phyIsLinkAlive(phyUnit)) {
            liveLinks++;
            MV_IS_PHY_ALIVE(phyUnit) = TRUE;
        } else {
            MV_IS_PHY_ALIVE(phyUnit) = FALSE;
        }

        MV_PRINT(MV_DEBUG_PHYSETUP,
            ("ethmac%d: Phy Status=%4.4x\n",
            ethUnit, 
            phyRegRead(MV_PHYBASE(phyUnit),
                       MV_PHYADDR(phyUnit),
                       MV_PHY_SPECIFIC_STATUS)));
    }

    mv_VLANInit(ethUnit);

    mv_enableConfiguredPorts(ethUnit);

    return (liveLinks > 0);
}


/******************************************************************************
*
* mv_phyIsDuplexFull - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1 --> at least one associated PHY in FULL DUPLEX
*    0 --> all half duplex
*   -1 --> No links
*/
int
mv_phyIsFullDuplex(int ethUnit)
{
    int     phyUnit;
    UINT32  phyBase;
    UINT32  phyAddr;
    UINT16  phyHwStatus;
    int     oneIsReady=0;

    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (mv_phyIsLinkAlive(phyUnit)) {
	    oneIsReady = 1;

            phyBase = MV_PHYBASE(phyUnit);
            phyAddr = MV_PHYADDR(phyUnit);

            phyHwStatus = phyRegRead(phyBase, phyAddr, MV_PHY_SPECIFIC_STATUS);

            if (phyHwStatus & MV_STATUS_RESOLVED_DUPLEX_FULL) {
                return 1;
            }
        }
    }
    if (oneIsReady)
	return 0;
    else
	return -1;
}

/******************************************************************************
*
* mv_phyIsSpeed100 - Determines the speed of a phy port
*
* RETURNS:
*    TRUE --> PHY operating at 100 Mbit
*    FALSE --> link down, or not operating at 100 Mbit
*/
BOOL
mv_phyIsSpeed100(int phyUnit)
{
    UINT16  phyHwStatus;
    UINT32  phyBase;
    UINT32  phyAddr;

    if (MV_IS_ENET_PORT(phyUnit)) {
        if (mv_phyIsLinkAlive(phyUnit)) {

            phyBase = MV_PHYBASE(phyUnit);
            phyAddr = MV_PHYADDR(phyUnit);

            phyHwStatus = phyRegRead(phyBase, phyAddr, MV_PHY_SPECIFIC_STATUS);

            if (phyHwStatus & MV_STATUS_RESOLVED_SPEED_100) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

#if CONFIG_VENETDEV
/******************************************************************************
*
* mv_phyDetermineSource - Examine a received frame's Egress Trailer
* to determine whether it came from a LAN or WAN port.
*
* RETURNS:
*    Sets *pFromLAN: 1-->LAN, 0-->WAN
*    Modifies *pLen to remove PHY trailer from frame
*/
void
mv_phyDetermineSource(char *data, int len, int *pFromLAN)
{
    unsigned char *phyTrailer;
    unsigned char incomingPort;

    phyTrailer = &data[len - MV_PHY_TRAILER_SIZE];
    ASSERT(phyTrailer[0] == MV_EGRESS_TRAILER_VALID);

    incomingPort = phyTrailer[1];
    if (MV_IS_LAN_PORT(incomingPort)) {
        *pFromLAN = 1;
    } else {
        ASSERT(MV_IS_WAN_PORT(incomingPort));
        *pFromLAN = 0;
    }
}


/******************************************************************************
*
* mv_phySetDestinationPort - Set the Ingress Trailer to force the
* frame to be sent to LAN or WAN, as specified.
*
*/
void
mv_phySetDestinationPort(char *data, int len, int fromLAN)
{
    char *phyTrailer;

    phyTrailer = &data[len];
    if (fromLAN) {
        /* LAN ports: Use default settings, as per mvPhyInfo */
        phyTrailer[0] = 0x00;
        phyTrailer[1] = 0x00;
    } else {
        /* WAN port: Direct to WAN port */
        phyTrailer[0] = MV_INGRESS_TRAILER_OVERRIDE;
        phyTrailer[1] = 1 << MV_WAN_PORT;
    }
    phyTrailer[2] = 0x00;
    phyTrailer[3] = 0x00;
}
#endif


/*****************************************************************************
*
* Validate that the specified PHY unit number is a valid PHY ID.
* Print a message if it is invalid.
* RETURNS
*   TRUE  --> valid
*   FALSE --> invalid
*/
LOCAL BOOL
mv_validPhyId(int phyUnit)
{
    if ((phyUnit >= MV_ID_MIN) && (phyUnit <= MV_ID_MAX)) {
        return TRUE;
    } else {
        PRINTF("PHY unit number must be in the range [%d..%d]\n",
            MV_ID_MIN, MV_ID_MAX);
        return FALSE;
    } 
}


/*****************************************************************************
*
* mv_waitWhileATUBusy - spins until the ATU completes
* its previous operation.
*/
LOCAL void
mv_waitWhileATUBusy(UINT32 phyBase)
{
    BOOL   isBusy;
    UINT16 ATUOperation;

    do {

        ATUOperation = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR,
                                  MV_ATU_OPERATION);

        isBusy = (ATUOperation & MV_ATU_BUSY_MASK) == MV_ATU_IS_BUSY;

    } while(isBusy);
}

/*****************************************************************************
*
* mv_flushATUDB - flushes ALL entries in the Address Translation Unit
* DataBase associated with phyUnit.  [Since we use a single DB for
* all PHYs, this flushes the entire shared DataBase.]
*
* The current implementation flushes even more than absolutely needed --
* it flushes all entries for all phyUnits on the same ethernet as the
* specified phyUnit.
*
* It is called only when a link failure is detected on a port that was
* previously working.  In other words, when the cable is unplugged.
*/
void
mv_flushATUDB(int phyUnit)
{
    UINT32 phyBase;

    if (!mv_validPhyId(phyUnit)) {
        PRINTF("Invalid port number: %d\n", phyUnit);
        return;
    }

    phyBase = MV_PHYBASE(phyUnit);
    
    /* Wait for previous operation (if any) to complete */
    mv_waitWhileATUBusy(phyBase);

    /* Tell hardware to flush all entries */
    phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_OPERATION, 
                MV_ATU_OP_FLUSH_ALL | MV_ATU_IS_BUSY);

    mv_waitWhileATUBusy(phyBase);
}
 
/*****************************************************************************
*
* mv_phyCheckStatusChange -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*/
void
mv_phyCheckStatusChange(int ethUnit)
{
    int           phyUnit;
    UINT16        phyHwStatus;
    mvPhyInfo_t   *lastStatus;
    int           linkCount   = 0;
    int           lostLinks   = 0;
    int           gainedLinks = 0;
    UINT32        phyBase;
    UINT32        phyAddr;

    for (phyUnit=0; phyUnit < MV_PHY_MAX; phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);

        lastStatus = &mvPhyInfo[phyUnit];
        phyHwStatus = phyRegRead(phyBase, phyAddr, MV_PHY_SPECIFIC_STATUS);

        if (lastStatus->isPhyAlive) { /* last known link status was ALIVE */
            /* See if we've lost link */
            if (phyHwStatus & MV_STATUS_REAL_TIME_LINK_UP) {
                linkCount++;
            } else {
                lostLinks++;
                mv_flushATUDB(phyUnit);
                MV_PRINT(MV_DEBUG_PHYCHANGE,("\nethmac%d port%d down\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = FALSE;
            }
        } else { /* last known link status was DEAD */
            /* Check for AutoNegotiation complete */
            if (MV_AUTONEG_DONE(phyHwStatus)) {
                gainedLinks++;
		linkCount++;
                MV_PRINT(MV_DEBUG_PHYCHANGE,("\nethmac%d port%d up\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = TRUE;
            }
        }
    }

    if (linkCount == 0) {
        if (lostLinks) {
            /* We just lost the last link for this MAC */
            phyLinkLost(ethUnit);
        }
    } else {
        if (gainedLinks == linkCount) {
            /* We just gained our first link(s) for this MAC */
            phyLinkGained(ethUnit);
        }
    }
}

#if DEBUG

/* Define the registers of interest for a phyShow command */
typedef struct mvRegisterTableEntry_s {
    UINT32 regNum;
    char  *regIdString;
} mvRegisterTableEntry_t;

mvRegisterTableEntry_t mvPhyRegisterTable[] = {
    {MV_PHY_CONTROL,                 "PHY Control                     "},
    {MV_PHY_STATUS,                  "PHY Status                      "},
    {MV_PHY_ID1,                     "PHY Identifier 1                "},
    {MV_PHY_ID2,                     "PHY Identifier 2                "},
    {MV_AUTONEG_ADVERT,              "Auto-Negotiation Advertisement  "},
    {MV_LINK_PARTNER_ABILITY,        "Link Partner Ability            "},
    {MV_AUTONEG_EXPANSION,           "Auto-Negotiation Expansion      "},
    {MV_NEXT_PAGE_TRANSMIT,          "Next Page Transmit              "},
    {MV_LINK_PARTNER_NEXT_PAGE,      "Link Partner Next Page          "},
    {MV_PHY_SPECIFIC_CONTROL_1,      "PHY-Specific Control Register 1 "},
    {MV_PHY_SPECIFIC_STATUS,         "PHY-Specific Status             "},
    {MV_PHY_INTERRUPT_ENABLE,        "PHY Interrupt Enable            "},
    {MV_PHY_INTERRUPT_STATUS,        "PHY Interrupt Status            "},
    {MV_PHY_INTERRUPT_PORT_SUMMARY,  "PHY Interrupt Port Summary      "},
    {MV_RECEIVE_ERROR_COUNTER,       "Receive Error Counter           "},
    {MV_LED_PARALLEL_SELECT,         "LED Parallel Select             "},
    {MV_LED_STREAM_SELECT_LEDS,      "LED Stream Select               "},
    {MV_PHY_LED_CONTROL,             "PHY LED Control                 "},
    {MV_PHY_MANUAL_LED_OVERRIDE,     "PHY Manual LED Override         "},
    {MV_VCT_CONTROL,                 "VCT Control                     "},
    {MV_VCT_STATUS,                  "VCT Status                      "},
    {MV_PHY_SPECIFIC_CONTROL_2,      "PHY-Specific Control Register 2 "},
};
int mvPhyNumRegs = sizeof(mvPhyRegisterTable) / sizeof(mvPhyRegisterTable[0]);


mvRegisterTableEntry_t mvSwitchPortRegisterTable[] = {
    {MV_PORT_STATUS,              "Port Status             "},
    {MV_SWITCH_ID,                "Switch ID               "},
    {MV_PORT_CONTROL,             "Port Control            "},
    {MV_PORT_BASED_VLAN_MAP,      "Port-Based VLAN Map     "},
    {MV_PORT_ASSOCIATION_VECTOR,  "Port Association Vector "},
    {MV_RX_COUNTER,               "RX Counter              "},
    {MV_TX_COUNTER,               "TX Counter              "},
};
int mvSwitchPortNumRegs =
    sizeof(mvSwitchPortRegisterTable) / sizeof(mvSwitchPortRegisterTable[0]);


mvRegisterTableEntry_t mvSwitchGlobalRegisterTable[] = {
    {MV_SWITCH_GLOBAL_STATUS,  "Switch Global Status  "},
    {MV_SWITCH_MAC_ADDR0,      "Switch MAC Addr 0 & 1 "},
    {MV_SWITCH_MAC_ADDR2,      "Switch MAC Addr 2 & 3 "},
    {MV_SWITCH_MAC_ADDR4,      "Switch MAC Addr 4 & 5 "},
    {MV_SWITCH_GLOBAL_CONTROL, "Switch Global Control "},
    {MV_ATU_CONTROL,           "ATU Control           "},
    {MV_ATU_OPERATION,         "ATU Operation         "},
    {MV_ATU_DATA,              "ATU Data              "},
    {MV_ATU_MAC_ADDR0,         "ATU MAC Addr 0 & 1    "},
    {MV_ATU_MAC_ADDR2,         "ATU MAC Addr 2 & 3    "},
    {MV_ATU_MAC_ADDR4,         "ATU MAC Addr 4 & 5    "},
};
int mvSwitchGlobalNumRegs =
    sizeof(mvSwitchGlobalRegisterTable) / sizeof(mvSwitchGlobalRegisterTable[0]);

void my_mvPhyShow(int ethUnit)
{
    int phyUnit;
    for (phyUnit=0; (phyUnit < MV_PHY_MAX); phyUnit++) {
        if (!MV_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }
	mv_phyShow(phyUnit);
    }
}

/*****************************************************************************
*
* mv_phyShow - Dump the state of a PHY.
* There are two sets of registers for each phy port:
*  "phy registers" and
*  "switch port registers"
* We dump 'em all, plus the switch global registers.
*/
void
mv_phyShow(int phyUnit)
{
    int     i;
    UINT16  value;
    UINT32  phyBase;
    UINT32  phyAddr;
    UINT32  switchPortAddr;

    if (!mv_validPhyId(phyUnit)) {
        return;
    }

    phyBase        = MV_PHYBASE(phyUnit);
    phyAddr        = MV_PHYADDR(phyUnit);
    switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);

    printk("PHY state for PHY%d (ethmac%d, phyBase 0x%8x, phyAddr 0x%x, switchAddr 0x%x)\n",
           phyUnit,
           MV_ETHUNIT(phyUnit),
           MV_PHYBASE(phyUnit),
           MV_PHYADDR(phyUnit),
           MV_SWITCH_PORT_ADDR(phyUnit));

    printk("PHY Registers:\n");
    for (i=0; i < mvPhyNumRegs; i++) {

        value = phyRegRead(phyBase, phyAddr, mvPhyRegisterTable[i].regNum);

        printk("Reg %02d (0x%02x) %s = 0x%08x\n",
               mvPhyRegisterTable[i].regNum,
               mvPhyRegisterTable[i].regNum,
               mvPhyRegisterTable[i].regIdString,
               value);
    }

    printk("Switch Port Registers:\n");
    for (i=0; i < mvSwitchPortNumRegs; i++) {

        value = phyRegRead(phyBase, switchPortAddr,
                           mvSwitchPortRegisterTable[i].regNum);

        printk("Reg %02d (0x%02x) %s = 0x%08x\n",
               mvSwitchPortRegisterTable[i].regNum,
               mvSwitchPortRegisterTable[i].regNum,
               mvSwitchPortRegisterTable[i].regIdString,
               value);
    }

    printk("Switch Global Registers:\n");
    for (i=0; i < mvSwitchGlobalNumRegs; i++) {

        value = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR,
                           mvSwitchGlobalRegisterTable[i].regNum);

        printk("Reg %02d (0x%02x) %s = 0x%08x\n",
               mvSwitchGlobalRegisterTable[i].regNum,
               mvSwitchGlobalRegisterTable[i].regNum,
               mvSwitchGlobalRegisterTable[i].regIdString,
               value);
    }
}

/*****************************************************************************
*
* mv_phySet - Modify the value of a PHY register (debug only).
*/
void
mv_phySet(int phyUnit, UINT32 regnum, UINT32 value)
{
    UINT32  phyBase;
    UINT32  phyAddr;

    if (mv_validPhyId(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);

        phyRegWrite(phyBase, phyAddr, regnum, value);
    }
}


/*****************************************************************************
*
* mv_switchPortSet - Modify the value of a switch port register (debug only).
*/
void
mv_switchPortSet(int phyUnit, UINT32 regnum, UINT32 value)
{
    UINT32  phyBase;
    UINT32  switchPortAddr;

    if (mv_validPhyId(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);
        switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);

        phyRegWrite(phyBase, switchPortAddr, regnum, value);
    }
}
 
/*****************************************************************************
*
* mv_switchGlobalSet - Modify the value of a switch global register
* (debug only).
*/
void
mv_switchGlobalSet(int phyUnit, UINT32 regnum, UINT32 value)
{
    UINT32  phyBase;

    if (mv_validPhyId(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);

        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, regnum, value);
    }
}

/*****************************************************************************
*
* mv_showATUDB - Dump the contents of the Address Translation Unit DataBase
* for the PHY switch associated with the specified phy.
*/
void
mv_showATUDB(int phyUnit)
{
    UINT32 phyBase;
    UINT16 ATUData;
    UINT16 ATUMac0;
    UINT16 ATUMac2;
    UINT16 ATUMac4;
    int portVec;
    int entryState;

    if (!mv_validPhyId(phyUnit)) {
        printk("Invalid port number: %d\n", phyUnit);
        return;
    }

    phyBase = MV_PHYBASE(phyUnit);
    
    /* Wait for previous operation (if any) to complete */
    mv_waitWhileATUBusy(phyBase);

    /* Initialize ATU MAC to all 1's */
    phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR0, 0xffff);
    phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR2, 0xffff);
    phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR4, 0xffff);

    printk("   MAC ADDRESS    EntryState PortVector\n");

    for(;;) {
        /* Tell hardware to get next MAC info */
        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_OPERATION, 
                    MV_ATU_OP_GET_NEXT | MV_ATU_IS_BUSY);

        mv_waitWhileATUBusy(phyBase);

        ATUData = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_DATA);
        entryState = (ATUData & MV_ENTRYSTATE_MASK) >> MV_ENTRYSTATE_SHIFT;

        if (entryState == 0) {
            /* We've hit the end of the list */
            break;
        }

        ATUMac0 = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR0);
        ATUMac2 = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR2);
        ATUMac4 = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR, MV_ATU_MAC_ADDR4);

        portVec    = (ATUData & MV_PORTVEC_MASK) >> MV_PORTVEC_SHIFT;

        printk("%02x:%02x:%02x:%02x:%02x:%02x    0x%02x       0x%02x\n",
               ATUMac0 >> 8,    /* MAC byte 0 */
               ATUMac0 & 0xff,  /* MAC byte 1 */
               ATUMac2 >> 8,    /* MAC byte 2 */
               ATUMac2 & 0xff,  /* MAC byte 3 */
               ATUMac4 >> 8,    /* MAC byte 4 */
               ATUMac4 & 0xff,  /* MAC byte 5 */
               entryState,
               portVec);
    }
}

LOCAL BOOL countingGoodFrames;

/*****************************************************************************
*
* mv_countGoodFrames - starts counting GOOD RX/TX frames per port
*/
void
mv_countGoodFrames(int phyUnit)
{
    UINT32 phyBase;
    UINT16 globalControl;

    if (mv_validPhyId(phyUnit)) {
        /*
         * Guarantee that counters are cleared by
         * forcing CtrMode to toggle and end on GOODFRAMES.
         */

        phyBase = MV_PHYBASE(phyUnit);

        /* Read current Switch Global Control Register */
        globalControl = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR,
                                   MV_SWITCH_GLOBAL_CONTROL);

        /* Set CtrMode to count BAD frames */
        globalControl = ((globalControl & ~MV_CTRMODE_MASK) |
                         MV_CTRMODE_BADFRAMES);

        /* Push new value out to hardware */
        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR,
                    MV_SWITCH_GLOBAL_CONTROL, globalControl);

        /* Now toggle CtrMode to count GOOD frames */
        globalControl = ((globalControl & ~MV_CTRMODE_MASK) |
                         MV_CTRMODE_GOODFRAMES);

        /* Push new value out to hardware */
        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR,
                    MV_SWITCH_GLOBAL_CONTROL, globalControl);

        countingGoodFrames = TRUE;
    }
}

/*****************************************************************************
*
* mv_countBadFrames - starts counting BAD RX/TX frames per port
*/
void
mv_countBadFrames(int phyUnit)
{
    UINT32 phyBase;
    UINT16 globalControl;

    if (mv_validPhyId(phyUnit)) {
        /*
         * Guarantee that counters are cleared by
         * forcing CtrMode to toggle and end on BADFRAMES.
         */

        phyBase = MV_PHYBASE(phyUnit);

        /* Read current Switch Global Control Register */
        globalControl = phyRegRead(phyBase, MV_SWITCH_GLOBAL_ADDR,
                                   MV_SWITCH_GLOBAL_CONTROL);

        /* Set CtrMode to count GOOD frames */
        globalControl = ((globalControl & ~MV_CTRMODE_MASK) |
                         MV_CTRMODE_GOODFRAMES);

        /* Push new value out to hardware */
        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR,
                    MV_SWITCH_GLOBAL_CONTROL, globalControl);

        /* Now toggle CtrMode to count BAD frames */
        globalControl = ((globalControl & ~MV_CTRMODE_MASK) |
                         MV_CTRMODE_BADFRAMES);

        /* Push new value out to hardware */
        phyRegWrite(phyBase, MV_SWITCH_GLOBAL_ADDR,
                    MV_SWITCH_GLOBAL_CONTROL, globalControl);

        countingGoodFrames = FALSE;
    }
}

/*****************************************************************************
*
* mv_showFrameCounts - shows current GOOD/BAD Frame counts
*/
void
mv_showFrameCounts(int phyUnit)
{
    UINT16 rxCounter;
    UINT16 txCounter;
    UINT32 phyBase;
    UINT32 switchPortAddr;

    if (!mv_validPhyId(phyUnit)) {
        return;
    }

    phyBase = MV_PHYBASE(phyUnit);
    switchPortAddr = MV_SWITCH_PORT_ADDR(phyUnit);

    rxCounter = phyRegRead(phyBase, switchPortAddr, MV_RX_COUNTER);

    txCounter = phyRegRead(phyBase, switchPortAddr, MV_TX_COUNTER);

    printk("port%d %s frames: receive: %05d   transmit: %05d\n",
           phyUnit,
           (countingGoodFrames ? "good" : "error"),
           rxCounter,
           txCounter);
}
#endif
