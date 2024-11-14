
/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef __SVC_GLEBAL_H__
#define __SVC_GLEBAL_H__

#if !defined(TCSUPPORT_MULTI_USER_ITF) && !defined(TCSUPPORT_MULTI_SWITCH_EXT)
struct bindlist_map
{
    char *bindif;
    char *realif;
    char  bit;
};

struct ifPolicyRoute
{
    char *ifName;
    char *mark;
    char *tabid;
};

#define LANIF_MASK			0xf0000000
/*******************************************************/
/*******************************************************/
/* support four lan port and four wlan port */
static const struct bindlist_map bl_map_data[] = 
{
    {"LAN1",        "eth0.1",   0},
    {"LAN2",        "eth0.2",   1},
    {"LAN3",        "eth0.3",   2},
    {"LAN4",        "eth0.4",   3},
    {"SSID1",       "ra0",      4},
    {"SSID2",       "ra1",      5},
    {"SSID3",       "ra2",      6},
    {"SSID4",       "ra3",      7},
#ifdef TCSUPPORT_WLAN_AC
    {"USB",         "usb0",     8},
    {"WDS",         "wds0",     9},
    {"SSIDAC1",     "rai0",     10},
    {"SSIDAC2",     "rai1",     11},
    {"SSIDAC3",     "rai2",     12},
    {"SSIDAC4",     "rai3",     13},
#endif
};

/*******************************************************/
/*******************************************************/
static const struct ifPolicyRoute ifpr[10] = 
{
    /* ethernet interface */
    /* for four ports */
    {"eth0.1",      "0x10000000",       "204"},
    {"eth0.2",      "0x20000000",       "205"},
    {"eth0.3",      "0x30000000",       "206"},
    {"eth0.4",      "0x40000000",       "207"},
    /* wireless interface */
    {"ra0",         "0x50000000",       "208"},
    {"ra1",         "0x60000000",       "209"},
    {"ra2",         "0x70000000",       "210"},
    {"ra3",         "0x80000000",       "211"},
    /* usb interface */
    {"usb0",        "0x90000000",       "212"}
};
#endif

#endif
