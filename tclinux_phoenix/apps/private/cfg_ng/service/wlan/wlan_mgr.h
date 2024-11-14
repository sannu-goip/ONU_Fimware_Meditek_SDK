
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


#ifndef __SVC_WLAN_MGR_H__
#define __SVC_WLAN_MGR_H__

#include <svchost_api.h> 
#include "wlan_cfg.h"

#define SVC_WLAN_MAX_ENTRY_NUM			8
#define ATTR_SIZE						20

#define MAX_WLAN_ENTRY_PATH_LEN			32
#define MAX_WLAN_DEV_NAME_LEN   		64


#define SVC_WLAN_APON_PATH			"/etc/Wireless/WLAN_APOn"
#define SVC_WLAN_NAME				"WLan"
#define SVC_WLAN_SCRIPT_PATH		"/etc/Wireless/RT2860AP/WLAN_exec.sh"
#define SVC_WLAN_RA_NAME			"ra"
#define SVC_WLAN_WDS_NAME			"wds"

#define SVC_WLAN11AC_APON_PATH		"/etc/Wireless/WLAN_APOn_AC"
#define SVC_WLAN11AC_NAME			"WLan11ac"
#define SVC_WLAN11AC_SCRIPT_PATH	"/etc/Wireless/RT2860AP_AC/WLAN_exec.sh"
#define SVC_WLAN11AC_RA_NAME		"rai"
#define SVC_WLAN11AC_WDS_NAME		"wdsi"
#define BSSIDKEYNAME				"Bssid_num=" 
#define CHANNELKEYNAME				"Channel="
#define WHNATKEYNAME				"WHNAT="
#define WLAN_PATH 					"/etc/Wireless/RT2860AP/RT2860AP.dat"
#define WLAN_AC_PATH 				"/etc/Wireless/RT2860AP_AC/RT2860AP.dat"
#define SVC_WLAN_BNDSTRG_SCRIPT_PATH	"/etc/Wireless/RT2860AP/WLAN_exec.sh"
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
#define SVC_WLAN11AC_ENTRY_ATTR_SCRIPT_PATH	"/etc/Wireless/RT2860AP_AC/WLAN_entry_attr_exec.sh"
#endif

#if defined(TCSUPPORT_ECNT_MAP)
#define SVC_WLAN_APCLI_NAME			"apcli"
#define SVC_WLAN11AC_APCLI_NAME		"apclii"
#endif


#define WLAN24GINFO_PATH		"/tmp/wlan_ra0_stats"
#define WLAN5GINFO_PATH		"/tmp/wlan11ac_rai0_stats"

#define WLAN24G_SITESURVEY_CMD		"/tmp/wlanra0_next_info"
#define WLAN5G_SITESURVEY_CMD		"/tmp/wlanrai0_next_info"

#define BEACONPERIODNAME			"BeaconPeriod="
#define DTIMPERIODNAME				"DtimPeriod="	
#define TXPOWERNAME					"TxPower="
#define WIRELESSMODENAME			"WirelessMode="
#define HTEXCHARNAME				"HT_EXTCHA="
#define HTGINAME					"HT_GI="
#define VHTBWNAME					"VHT_BW="
#define VHTSGINAME					"VHT_SGI="
#define HTBSSCOEXNAME				"HT_BSSCoexistence="
#define HTBWNAME					"HT_BW="
#define HTTXSTREAMNAME				"HT_TxStream="
#define HTRXSTREAMNAME				"HT_RxStream="
#define COUNTRYREGION				"CountryRegion="
#define COUNTRYREGIONABAND			"CountryRegionABand="

#define MUOFDMADLENABLENAME			"MuOfdmaDlEnable="
#define MUOFDMAULENABLENAME			"MuOfdmaUlEnable="
#define MUMINODLENABLENAME			"MuMimoDlEnable="
#define MUMINOULENABLENAME			"MuMimoUlEnable="
#define TWTSUPPORTNAME				"TWTSupport="
#define SRENABLENAME				"SREnable="
#define SRMODENAME					"SRMode="
#define SRSDENABLENAME				"SRSDEnable="
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
#define ANTDIS						"ant_dis="
#define ANTCONVERT					"ant_convert="
#define ANTSEL						"ant_sel="
#endif

typedef struct wlan_entry_cfg{
	unsigned short vid;
	unsigned short tid;
	unsigned short mtu;
	unsigned short pdtm1;
	unsigned short pdtm2;
	char active;
}wlan_entry_cfg_t;


typedef struct wlan_switch_cfg_s {
	unsigned int wifi_switch;
#define WLAN_24G_ON		(1<<15)
#define	WLAN_58_ON		(1<31)
	/*  0-14bit for 2.4G ssid1 - ssid14
	16 - 30bit for 5.8G ssid1 - ssid14  */
}wlan_switch_cfg_t;

#if defined(TCSUPPORT_SHCT_SDN)
typedef struct _wifi_name_value_attr_
{
	const char *sdn_attr;
	const char *sdn_value;
	const char *node_name;
	const char *attr_name;
	const char *attr_val;
	const char *attr1_name;
	const char *attr1_val;
}wifi_name_value_attr;
#endif
int svc_wlan_mgr_internal_handle_event(int event, pt_wlan_evt param);

#if 0
int svc_wlan_mgr_external_handle_event(int event, pt_wlan_evt param);
#endif

int svc_wlan_handle_event_update(pt_wlan_evt param);

int svc_wlan_handle_event_wps_update(pt_wlan_evt param);

int svc_wlan_boot(void);

int svc_wlan_execute(char* path);

void wlan_cc(void* unused);
void wlan_MonitorTask(void* unused);
void wlan_driver_event_user(void);
void timer_tool(void* unused);
#if defined(TCSUPPORT_NP_CMCC)
int np_apcli_status_chk();
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
int svc_wlan_apcli_start();
#endif
#endif

