/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the ethernet PHY.
 * This code supports a simple 1-port ethernet phy, Realtek RTL8201BL,
 * and compatible PHYs, such as the Kendin KS8721B.
 * All definitions in this file are operating system independent!
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
#include "rtPhy.h"

#define RT_MAX_PORTS 5 /* max addressable ports per MIIM */

#if /* DEBUG */ 1
#define RT_DEBUG_ERROR     0x00000001
#define RT_DEBUG_PHYSETUP  0x00000002
#define RT_DEBUG_PHYCHANGE 0x00000004

/* XXX: must hardcode this since same MIIM for all ethUnits */
const UINT32 phyBase = 0xb8100000;

int rtPhyDebug = RT_DEBUG_ERROR;

#define RT_PRINT(FLG, X)                            \
{                                                   \
    if (rtPhyDebug & (FLG)) {                       \
        DEBUG_PRINTF X;                             \
    }                                               \
}
#else
#define RT_PRINT(FLG, X)
#endif

/*
 * Track per-PHY state.
 */
static BOOL rtPhyAlive[RT_MAX_PORTS];


/******************************************************************************
*
* rt_phySetup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

BOOL
rt_phySetup(int ethUnit, UINT32 phyBaseIgnored)
{
    BOOL    linkAlive = FALSE;

    /* Reset phy */
	if (ethUnit == 0) {
		int i;
		for (i=1; i<5; i++) {
			phyRegWrite(phyBase, i, GEN_ctl, AUTONEGENA);
			sysMsDelay(200);
			if (phyRegRead(phyBase, i, GEN_sts) & (AUTOCMPLT | LINK)) {
				rtPhyAlive[i] = TRUE;
			}
			else {
				rtPhyAlive[i] = FALSE;
			}
		}
	}
	else {
		phyRegWrite(phyBase, 5, GEN_ctl, AUTONEGENA);
		sysMsDelay(200);
		if (phyRegRead(phyBase, 5, GEN_sts) & (AUTOCMPLT | LINK)) {
			rtPhyAlive[5] = TRUE;
		}
		else {
			rtPhyAlive[5] = FALSE;
		}
	}

    return linkAlive;
}

/******************************************************************************
*
* rt_phyIsDuplexFull - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1  --> FULL
*    0 --> HALF
*/
int
rt_phyIsFullDuplex(int ethUnit)
{
	UINT16  phyLpa = 0;

	if (ethUnit == 0) {
		int i;
		/* 4 ports connected. If any are half-duplex report half. */
		for (i=1; i<5; i++) {
			phyLpa = phyRegRead(phyBase, i, AN_lpa);
			if ( (!(phyLpa & (LPA_TXFD | LPA_10FD))) && 
				 (phyLpa & (LPA_TX | LPA_10)) ) {
				return 0;
			}
		}
		return 1;
	}
	else {
		phyLpa = phyRegRead(phyBase, 5, AN_lpa);
		if (phyLpa & (LPA_TXFD | LPA_10FD) ) {
			return 1;
		}
		else {
			return 0;
		}
	}
}

/******************************************************************************
*
* rt_phyIsSpeed100 - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*    TRUE --> 100Mbit
*    FALSE --> 10Mbit
*/
BOOL
rt_phyIsSpeed100(int ethUnit)
{
    UINT16  phyLpa;

	if (ethUnit == 0) {
		int i;
		/* 4 ports connected. If any are not 100 report 10. */
		for (i=1; i<5; i++) {
			phyLpa = phyRegRead(phyBase, i, AN_lpa);
			if ( (!(phyLpa & (LPA_TXFD | LPA_TX))) && 
				 (phyLpa & (LPA_10FD | LPA_10)) ) {
				printk("10\n");
				return FALSE;
			}
		}
		printk("100\n");
		return TRUE;
	}
	else {
		phyLpa = phyRegRead(phyBase, 5, AN_lpa);
		if (phyLpa & (LPA_TXFD | LPA_TX) ) {
			printk("100\n");
			return TRUE;
		}
		else {
			printk("10\n");
			return FALSE;
		}
	}
}

/*****************************************************************************
*
* rt_phyCheckStatusChange -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/
void
rt_phyCheckStatusChange(int ethUnit)
{
    UINT16          phyHwStatus;
	int i, loopLower, loopUpper;
	
	if (ethUnit == 0) {
		loopLower = 1;
		loopUpper = 4;
	}
	else {
		loopLower = 5;
		loopUpper = 5;
	}

	for (i=loopLower; i<=loopUpper; i++) {
		phyHwStatus = phyRegRead(phyBase, i, GEN_sts);

		if (rtPhyAlive[i]) { /* last known status was ALIVE */
			/* See if we've lost link */
			if (!(phyHwStatus & LINK)) {
				RT_PRINT(RT_DEBUG_PHYCHANGE,("\nethmac%d link down\n", ethUnit));
				rtPhyAlive[i] = FALSE;
				phyLinkLost(ethUnit);
			}
		} else { /* last known status was DEAD */
			/* Check for AN complete */
			if ((phyHwStatus & (AUTOCMPLT | LINK)) == (AUTOCMPLT | LINK)) {
				RT_PRINT(RT_DEBUG_PHYCHANGE,("\nethmac%d link up\n", ethUnit));
				rtPhyAlive[i] = TRUE;
				phyLinkGained(ethUnit);
			}
		}
	}
}

#if DEBUG

/* Define the PHY registers of interest for a phyShow command */
struct rtRegisterTable_s {
    UINT32 regNum;
    char  *regIdString;
} rtRegisterTable[] =
{
    {GEN_ctl,    "Basic Mode Control (GEN_ctl)    "},
    {GEN_sts,    "Basic Mode Status (GEN_sts)     "},
    {GEN_id_hi,  "PHY Identifier 1 (GET_id_hi)    "},
    {GEN_id_lo,  "PHY Identifier 2 (GET_id_lo)    "},
    {AN_adv,     "Auto-Neg Advertisement (AN_adv) "},
    {AN_lpa,     "Auto-Neg Link Partner Ability   "},
    {AN_exp,     "Auto-Neg Expansion              "},
};

int rtNumRegs = sizeof(rtRegisterTable) / sizeof(rtRegisterTable[0]);

/*
 * Dump the state of a PHY.
 */
void
rt_phyShow(int phyUnit)
{
    int i;
    UINT16  value;
	int j, loopLower, loopUpper;

    printf("PHY state for ethphy%d\n", phyUnit);
	
	if (phyUnit == 0) {
		loopLower = 1;
		loopUpper = 4;
	}
	else {
		loopLower = 5;
		loopUpper = 5;
	}

	for (j=loopLower; j<=loopUpper; j++) {
		printk("PHY port %d:\n", j);
		for (i=0; i<rtNumRegs; i++) {
			
			value = phyRegRead(phyBase, j, rtRegisterTable[i].regNum);
			
			printf("Reg %02d (0x%02x) %s = 0x%08x\n",
				   rtRegisterTable[i].regNum, rtRegisterTable[i].regNum,
				   rtRegisterTable[i].regIdString, value);
		}
	}
}

#endif
