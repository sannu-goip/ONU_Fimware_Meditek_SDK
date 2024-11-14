
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <malloc.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h>
#include <libapi_lib_wifimgr.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include "wlan_mgr.h"
#include "wlan_cfg.h"
#include <cfg_type_info.h>
#include <cfg_type_wlan.h>
#include <utility.h>
#include <cfg_msg.h>
#include <semaphore.h>
#include "wlan_driver_event_user.h"
#include <sys/time.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include <ecnt_event_global/ecnt_event_wifi.h>
#include "wlan_utils.h"
#if defined(TCSUPPORT_CT_UBUS)
#include <notify.h>
#endif
#if defined(TCSUPPORT_SHCT_SDN)
static int sdn_conf_update_to_node(int wifi_type);
#endif
#define IF_2G    22
#ifndef MAX_KEYWORD_LEN
#define MAX_KEYWORD_LEN    		    160
#endif
#define ECNT_SEND_EVENT_TEST_ENABLE "/tmp/ecnt_send_enable"
#ifdef TCSUPPORT_WLAN_VENDIE
#define ECNT_BEACONVSIE_DBG_ENABLE "/tmp/ecnt_beaconvsie_dbg_enable"

#define PRINT_MAC(addr)	\
	addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
#define VSIE_EL(addr)	\
		addr[0], addr[1], addr[2], addr[3]

#define	OID_GET_SET_TOGGLE							0x8000

#define OID_VENDOR_IE_BASE		0x1200

#define BEACONVSIE_COMMIT 0
#define BEACONVSIE_UPDATE 1

#define FINISHED 0
#define BUSY     2
#define STOP     3

#define _BUSY_     "2"
#define _STOP_     "3"

#define RCPI_TO_RSSI(RCPI) ((RCPI == 255) ? (-127) : (RCPI - 220)/2)


#define MAX_INDEX_NUM  16
#define MAX_ENTRY_NUM  8

#define BEACONVSIE_STATUS_FINISHED_MESSAGE "/com/ctc/igd1/VSIE/BeaconTxVSIE/%d;com.ctc.igd1.VSIE.BeaconTxVSIE;Status:y:0"
#define BEACONVSIE_STATUS_BUSY_MESSAGE     "/com/ctc/igd1/VSIE/BeaconTxVSIE/%d;com.ctc.igd1.VSIE.BeaconTxVSIE;Status:y:2"
#define BEACONVSIE_STATUS_STOP_MESSAGE 	   "/com/ctc/igd1/VSIE/BeaconTxVSIE/%d;com.ctc.igd1.VSIE.BeaconTxVSIE;Status:y:3"


enum vendor_ie_subcmd_oid {
	OID_SUBCMD_AP_VENDOR_IE_SET,
	OID_SUBCMD_AP_VENDOR_IE_DEL,

	NUM_OID_SUBCMD_VENDOR_IE,
	MAX_NUM_OID_SUBCMD_VENDOR_IE = NUM_OID_SUBCMD_VENDOR_IE - 1
};

#define OID_AP_VENDOR_IE_SET		(OID_VENDOR_IE_BASE | OID_SUBCMD_AP_VENDOR_IE_SET)/*0x1200*/
#define OID_AP_VENDOR_IE_DEL		(OID_VENDOR_IE_BASE | OID_SUBCMD_AP_VENDOR_IE_DEL)
#define RT_OID_AP_VENDOR_IE_SET		(OID_GET_SET_TOGGLE | OID_AP_VENDOR_IE_SET)/*0x9200*/
#define RT_OID_AP_VENDOR_IE_DEL		(OID_GET_SET_TOGGLE | OID_AP_VENDOR_IE_DEL)
#endif

int iswpsstart		= 0;
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) || defined(TCSUPPORT_WPS_5G_BTN)
int iswps5gstart	= 0;
#endif
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
int wps_both_start 	= 0;
int wps24g_done 	= 0;
int wps5g_done  	= 0;
#endif

wlan_switch_cfg_t g_wlan_switch_cfg;
int WscOOBSeted		= 0;
int WscOOBSeted_ac	= 0;

extern char svc_wlan_common_path[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_entry_path[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_apon_path[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_name[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_script_path[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_ra_name[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_script_prefix[SVC_WLAN_PATH_SIZE];
extern char svc_wlan_wds_name[SVC_WLAN_PATH_SIZE];

extern int type;
static int cnt = 0;

#ifdef TCSUPPORT_MTD_ENCHANCEMENT
int isFirstWrite	= 1;
int isFirstWrite_ac	= 1;
#endif

#if defined(TCSUPPORT_CT)
#define WIFI_DEV_TXQUEUE_LEN 500
#endif
sem_t SEM_CCRSSI;
#if defined(TCSUPPORT_NP_CMCC)
int wlan_last_apon = 0;
int wlan11ac_last_apon = 0;
#endif

#if defined(TCSUPPORT_WLAN_DOT11K_RRM)||defined(TCSUPPORT_WLAN_DOT11V_WNM)
void kv_daemon_check()
{
	char cmd[128] = {0};
        int ch;
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"ps | grep kv -c > /tmp/kv.log");
	system(cmd);
	FILE* fp = fopen("/tmp/kv.log","r");
	if(fp == NULL)
		return;
	ch = fgetc(fp);
	if(ch - '0' <= 3)
	{
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd,"/userfs/bin/kv -d3");
		system(cmd);
	}
	fclose(fp);
}
#endif

static int svc_wlan_update_opt_cfg(char* para)
{
	char ant_value[4]	= {0};
	FILE* fp 			= NULL;
	char APOn[4]		= {0};
	char Bssid_Num[4]	= {0};
	char tmp[128]		= {0};
	char node_name[128] = {0};
#if defined(TCSUPPORT_CT_WLAN_NODE)
    char EnableSSID[4]	= {0};
    int i 				= 0;
#endif
	char path[64]		= {0};

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	char *tmp_path = NULL;
	char *tmp_index = NULL;
	int  entry_index = 0;
#endif
	int ret = 0;

	strncpy(path, para, sizeof(path) - 1);	
	cfg_str_to_lower(path);	
	if(NULL != strstr(path, "wlan11ac")){
		type = WIFI_5G;
		sprintf(svc_wlan_common_path, 	WLAN11AC_COMMON_NODE);
		sprintf(svc_wlan_entry_path, 	WLAN11AC_ENTRY_NODE);
		sprintf(svc_wlan_apon_path, 	SVC_WLAN11AC_APON_PATH);	
		sprintf(svc_wlan_name, 			SVC_WLAN11AC_NAME);
		sprintf(svc_wlan_script_path, 	SVC_WLAN11AC_SCRIPT_PATH);
		sprintf(svc_wlan_ra_name,		SVC_WLAN11AC_RA_NAME);
		wifimgr_lib_get_WLAN_SCRIPT(type, SVC_WLAN_PATH_SIZE, svc_wlan_script_prefix);
		sprintf(svc_wlan_wds_name,		SVC_WLAN11AC_WDS_NAME);
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		if((tmp_path = strstr(path, "entry")) != NULL){
			if((tmp_index = strstr(tmp_path, ".")) != NULL ){
				entry_index = atoi(tmp_index+1) - 1;
				 if(cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "wlan_ac_id",itoa(entry_index)) < 0){ 
						tcdbg_printf("\r\n%s,%d,set wlan_id error\r\n",__FUNCTION__,__LINE__);
				}
			}
		}
#endif
	}
	else if(NULL != strstr(path, "wlan")){
		type = WIFI_2_4G;
		sprintf(svc_wlan_common_path, 	WLAN_COMMON_NODE);
		sprintf(svc_wlan_entry_path, 	WLAN_ENTRY_NODE);
		sprintf(svc_wlan_apon_path, 	SVC_WLAN_APON_PATH);
		sprintf(svc_wlan_name, 			SVC_WLAN_NAME);
		sprintf(svc_wlan_script_path, 	SVC_WLAN_SCRIPT_PATH);
		sprintf(svc_wlan_ra_name,		SVC_WLAN_RA_NAME);
		wifimgr_lib_get_WLAN_SCRIPT(type, SVC_WLAN_PATH_SIZE, svc_wlan_script_prefix);
		sprintf(svc_wlan_wds_name,		SVC_WLAN_WDS_NAME);
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		if((tmp_path = strstr(path, "entry")) != NULL){
			if((tmp_index = strstr(tmp_path, ".")) != NULL ){
				entry_index = atoi(tmp_index+1) - 1;
				 if(cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id",itoa(entry_index)) < 0){	
    					tcdbg_printf("\r\n%s,%d,set wlan_id error\r\n",__FUNCTION__,__LINE__);
				}
			}
		}
#endif
	}
	else{
		return CFG2_RET_FAIL;
	}
#if defined(TCSUPPORT_SHCT_SDN)
		sdn_conf_update_to_node(type);
#endif

	if((cfg_get_object_attr(svc_wlan_common_path, "APOn", APOn, sizeof(APOn)) < 0) || 
	(cfg_get_object_attr(svc_wlan_common_path, "BssidNum", Bssid_Num, sizeof(Bssid_Num)) < 0)){
		return CFG2_RET_FAIL;
	}
	fp = fopen(svc_wlan_apon_path, "w");
	if(fp != NULL){
		snprintf(tmp, sizeof(tmp), "APOn=%s\nBssid_num=%s\n", APOn, Bssid_Num);
		fputs_escape(tmp, fp);
#if defined(TCSUPPORT_CT_WLAN_NODE)
		for(i = 0; i < atoi(Bssid_Num); i++){
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "%s.%d", svc_wlan_entry_path, i+1);
			memset(EnableSSID, 0, sizeof(EnableSSID));
			if((cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, sizeof(EnableSSID)) > 0) && (!strcmp(EnableSSID, WLAN_SSID_ON))){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ssid%d=%s\n", i, EnableSSID);
 				fputs_escape(tmp, fp);
			}
			else{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ssid%d=%s\n", i, "0");
 				fputs_escape(tmp, fp);
			}
			if (type == WIFI_2_4G)
			{
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_dis", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan_ant_dis=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_convert", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan_ant_convert=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_sel", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan_ant_sel=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}
			}
			else if(type == WIFI_5G)
			{
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_dis", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan11ac_ant_dis=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_convert", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan11ac_ant_convert=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}
				memset(ant_value , 0, sizeof(ant_value));
				if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_sel", ant_value, sizeof(ant_value)) >= 0){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "wlan11ac_ant_sel=%s\n", ant_value);
					fputs_escape(tmp,fp);
				}		
			}
		}
#endif
#ifdef TCSUPPORT_WLAN_WDS	
		write_wds_attr(type, fp);
#endif
		fclose(fp);
	}
	if(!strcmp(APOn,WLAN_APOFF)){
		unlink(svc_wlan_script_path);
	}else if(!strcmp(APOn, WLAN_APON)){
#ifdef WSC_AP_SUPPORT
		memset(node_name, 0, sizeof(node_name));
		sprintf(node_name, "root.Info.%s", svc_wlan_name);
		if(1 == run_wps()){/*first check if wps should run*/	
			cfg_set_object_attr(node_name, "WPSActiveStatus", "0");
			ret = wifimgr_lib_set_11N_CMDflag(1);
			ret = wifimgr_lib_set_11AC_CMDflag(1);
			return CFG2_RET_SUCCESS;
		}
		
		if(1 == wps_oob()){
			cfg_set_object_attr(node_name, "WPSOOBActive", "0");
			ret = wifimgr_lib_set_11N_CMDflag(1);
			ret = wifimgr_lib_set_11AC_CMDflag(1);
			return CFG2_RET_SUCCESS;
		}
#endif
#ifdef TCSUPPORT_WLAN_AC
		if(1 == reset_ac_counter()){
			cfg_set_object_attr(node_name, "ReCounterActive", "0");
			ret = wifimgr_lib_set_11AC_CMDflag(1);
			return CFG2_RET_SUCCESS;
		}
#endif

#ifdef WSC_AP_SUPPORT
		if(1 == wps_genpincode()){
			cfg_set_object_attr(node_name, "WPSGenPinCode", "0");
			ret = wifimgr_lib_set_11N_CMDflag(1);
			ret = wifimgr_lib_set_11AC_CMDflag(1);
			return CFG2_RET_SUCCESS;
		}
#endif
		/*write WLAN_ant.sh*/
		write_wlan_ant_sh();
		/*write xml data to WLAN_sh*/
		if(WIFI_2_4G == type){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
			wifimgr_lib_get_version(0, tmp, sizeof(tmp));
			if(strncmp(tmp, "7915",4))
				write_wlan_entry_7915D_sh(atoi(Bssid_Num));
			else
#endif
				write_wlan_exe_sh(atoi(Bssid_Num));

		}		
#if defined(TCSUPPORT_WLAN_AC)	
		else if(WIFI_5G == type){
			write_wlan11ac_exe_sh(atoi(Bssid_Num));
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
			wifimgr_lib_get_version(1, tmp, sizeof(tmp));
			if(strncmp(tmp, "7915",4))
				write_wlan11ac_entry_7915D_sh(atoi(Bssid_Num));
			else
				write_wlan11ac_entry_attr_exe_sh(atoi(Bssid_Num));
#endif

		}
#endif
		else{
			printf("The wifi type is invalid.\n");
			return CFG2_RET_FAIL;
		}	
	}
	
	/*write xml data to RT2860AP.dat*/
	if(WIFI_2_4G == type){
		if(write_wlan_config(path, atoi(Bssid_Num)) < 0){
			printf("write_wlan_config fail.\n");
		}
#ifdef TCSUPPORT_MTD_ENCHANCEMENT		
		if(isFirstWrite){/*load from flash*/
			if(wifimgr_lib_get_wifi_calibration	(type) != 0){
				printf("get wifi calibration failed.\n");
			}
			isFirstWrite=0;
		}
#endif	
	}
#if defined(TCSUPPORT_WLAN_AC)
	else if(WIFI_5G == type){
		if(write_wlan11ac_config(path, atoi(Bssid_Num)) < 0){
			printf("write_wlan11ac_config fail.\n");
		}
#ifdef TCSUPPORT_MTD_ENCHANCEMENT			
		if(isFirstWrite_ac){
			if(wifimgr_lib_get_wifi_calibration	(type) != 0){
				printf("set wifi calibration failed.\n");
			}
			isFirstWrite_ac=0;
		}
#endif				
	}
#endif
	else{
		printf("The wifi type is invalid.\n");
		return CFG2_RET_FAIL;
	}

	return CFG2_RET_SUCCESS;
}
#ifdef TCSUPPORT_WLAN_LED_BY_SW
void wlan_led_action(){
	char wifi24g_apon[4] = {0};
	int i_24gwifi_apon = 0;
	char wifi5g_apon[4] = {0};
	int i_5gwifi_apon = 0;

	cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", wifi24g_apon, sizeof(wifi24g_apon));
	i_24gwifi_apon = atoi(wifi24g_apon);
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", wifi5g_apon, sizeof(wifi5g_apon));
	i_5gwifi_apon = atoi(wifi5g_apon);
	if(i_24gwifi_apon || i_5gwifi_apon)
		wifimgr_lib_set_LED_WLAN("1");
	else
		wifimgr_lib_set_LED_WLAN("0");

}
#endif

#if defined(TCSUPPORT_CT_UBUS)
int send_wifistatus2ubus()
{
	wifistatus_msg_t msgdata;
	char wifiswitch[4]		= {0};
	char wpsswitch[4]		= {0};
	char elinksync[4]		= {0};

	memset(&msgdata, 0, sizeof(wifistatus_msg_t));
	memset(wifiswitch, 0, sizeof(wifiswitch));
	memset(wpsswitch, 0, sizeof(wpsswitch));
	memset(elinksync, 0, sizeof(elinksync));
		
	switch(type)
	{
		case WIFI_2_4G:
			cfg_get_object_attr(WLAN_COMMON_NODE, "wifiswitch", wifiswitch, sizeof(wifiswitch));
			cfg_get_object_attr(WLAN_COMMON_NODE, "wpsswitch", wpsswitch, sizeof(wpsswitch));
			cfg_get_object_attr(WLAN_COMMON_NODE, "elinksync", elinksync, sizeof(elinksync));
			strncpy(msgdata.wifiswitch, wifiswitch, sizeof(msgdata.wifiswitch) - 1);
			strncpy(msgdata.wpsswitch, wpsswitch, sizeof(msgdata.wpsswitch) - 1);
			strncpy(msgdata.elinksync,elinksync , sizeof(msgdata.elinksync) - 1);			
			ctc_notify2ctcapd(NOTIFY_WIFI_STATUS_CHANGE, &msgdata, sizeof(wifistatus_msg_t));
			break;
		case WIFI_5G:
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "wifiswitch", wifiswitch, sizeof(wifiswitch));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "wpsswitch", wpsswitch, sizeof(wpsswitch));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "elinksync", elinksync, sizeof(elinksync));
			strncpy(msgdata.wifiswitch, wifiswitch, sizeof(msgdata.wifiswitch) - 1);
			strncpy(msgdata.wpsswitch, wpsswitch, sizeof(msgdata.wpsswitch) - 1);
			strncpy(msgdata.elinksync,elinksync , sizeof(msgdata.elinksync) - 1);			
			ctc_notify2ctcapd(NOTIFY_WIFI_STATUS_CHANGE, &msgdata, sizeof(wifistatus_msg_t));
			break;
		default:
			break;
	}
	
	return 0;
}

int send_wifistamsgMAC2ubus(char * stamac)
{

	wifipswerr_msg_t msgdata;

	strncpy(msgdata.stamac ,stamac , sizeof(msgdata.stamac) - 1);			
	ctc_notify2ctcapd(NOTIFY_WIFI_PSWERR_STATUS, &msgdata, sizeof(wifipswerr_msg_t));
	
	return 0;
}
#endif

#if defined(TCSUPPORT_SHCT_SDN)
wifi_name_value_attr	WLanSetValList[] =
{
	{"radio",		"on", 			WLAN_COMMON_NODE,	"APon",				"1",	NULL,					NULL},
	{"radio",		"off", 			WLAN_COMMON_NODE,	"APon",				"0",	NULL,					NULL},
	{"channel"	,	NULL,			WLAN_COMMON_NODE, 	"Channel",			NULL,	NULL,					NULL},
	{"radiomode",	"802.11b/g/n",	WLAN_COMMON_NODE, 	"WirelessMode",		"9",	NULL,					NULL},	
	{"radiomode",	"802.11b/g",	WLAN_COMMON_NODE, 	"WirelessMode",		"0",	NULL,					NULL},
	{"radiomode",	"802.11g/n",	WLAN_COMMON_NODE, 	"WirelessMode",		"7",	NULL,					NULL},	
	{"radiomode",	"802.11n",		WLAN_COMMON_NODE, 	"WirelessMode",		"6",	NULL,					NULL},	
	{"radiomode",	"802.11g",		WLAN_COMMON_NODE, 	"WirelessMode",		"4",	NULL,					NULL},	
	{"radiomode",	"802.11b",		WLAN_COMMON_NODE, 	"WirelessMode",		"1",	NULL,					NULL},	
	{"bandwidth",	"20",			WLAN_COMMON_NODE,	"HT_BW", 			"0",	"HT_BSSCoexistence",	"1"},
	{"bandwidth",	"40",			WLAN_COMMON_NODE,	"HT_BW", 			"1",	"HT_BSSCoexistence",	"0"},
	{"bandwidth",	"20/40",		WLAN_COMMON_NODE,	"HT_BW", 			"1",	"HT_BSSCoexistence",	"1"},
	{"power",		"100", 			WLAN_COMMON_NODE,	"TxPowerLevel",		"1",	NULL,					NULL},
	{"power",		"80", 			WLAN_COMMON_NODE,	"TxPowerLevel",		"2",	NULL,					NULL},
	{"power",		"60", 			WLAN_COMMON_NODE,	"TxPowerLevel",		"3",	NULL,					NULL},
	{"power",		"40", 			WLAN_COMMON_NODE,	"TxPowerLevel",		"4",	NULL,					NULL},
	{"power",		"20", 			WLAN_COMMON_NODE,	"TxPowerLevel",		"5",	NULL,					NULL},
	{"wmm",			NULL,			WLAN_ENTRY_N_NODE, 	"WMM",				NULL,	NULL,					NULL},
	{"wmm_aspd",	NULL,			NULL, 				NULL,				NULL,	NULL,					NULL},
	{"ssid0",		NULL,			WLAN_ENTRY_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid1",		NULL,			WLAN_ENTRY1_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid2",		NULL,			WLAN_ENTRY2_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid3",		NULL,			WLAN_ENTRY3_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"enable",		NULL,			WLAN_ENTRY_N_NODE, 	"EnableSSID",		NULL,	NULL,					NULL},
	{"hide",		NULL,			WLAN_ENTRY_N_NODE, 	"HideSSID",			NULL,	NULL,					NULL},
	{"maxassoc",	NULL,			WLAN_ENTRY_N_NODE, 	NULL,				NULL,	NULL,					NULL},
	{"apisolate",	NULL,			WLAN_ENTRY_N_NODE, 	NULL,				NULL,	NULL,					NULL},
	{"auth",		NULL,			WLAN_ENTRY_N_NODE, 	"AuthMode",			NULL,	NULL,					NULL},
	{"encrypt",		NULL,			WLAN_ENTRY_N_NODE, 	"EncrypType",		NULL,	NULL,					NULL},
	{"wepauth",		NULL,			WLAN_ENTRY_N_NODE, 	"WepAuth",			NULL,	NULL,					NULL},
	{"weplength",	NULL,			WLAN_ENTRY_N_NODE, 	"WepLength",		NULL,	NULL,					NULL},
	{"wepindex",	NULL,			WLAN_ENTRY_N_NODE, 	"WepIndex",			NULL,	NULL,					NULL},
	{"wep1pwd",		NULL,			WLAN_ENTRY_N_NODE, 	"Wep1pwd",			NULL,	NULL,					NULL},
	{"wep2pwd",		NULL,			WLAN_ENTRY_N_NODE, 	"Wep2pwd",			NULL,	NULL,					NULL},
	{"wep3pwd",		NULL,			WLAN_ENTRY_N_NODE, 	"Wep3pwd",			NULL,	NULL,					NULL},
	{"wep4pwd",		NULL,			WLAN_ENTRY_N_NODE, 	"Wep4pwd",			NULL,	NULL,					NULL},
	
	{NULL,			NULL,			NULL,				NULL,				NULL,	NULL,					NULL}
};
wifi_name_value_attr	WLan11acSetValList[] =
{
	{"radio",		"on", 			WLAN_COMMON_NODE,		"APOn",				"1",	NULL,					NULL},
	{"radio",		"off", 			WLAN_COMMON_NODE,		"APOn",				"0",	NULL,					NULL},
	{"channel",		NULL,			WLAN_COMMON_NODE, 		"Channel",			NULL,	NULL,					NULL},
	{"radiomode",	"802.11b/g/n",	WLAN_COMMON_NODE, 		"WirelessMode",		"9",	NULL,					NULL},	
	{"radiomode",	"802.11b/g",	WLAN_COMMON_NODE, 		"WirelessMode",		"0",	NULL,					NULL},
	{"radiomode",	"802.11g/n",	WLAN_COMMON_NODE, 		"WirelessMode",		"7",	NULL,					NULL},	
	{"radiomode",	"802.11n",		WLAN_COMMON_NODE, 		"WirelessMode",		"6",	NULL,					NULL},	
	{"radiomode",	"802.11g",		WLAN_COMMON_NODE, 		"WirelessMode",		"4",	NULL,					NULL},	
	{"radiomode",	"802.11b",		WLAN_COMMON_NODE, 		"WirelessMode",		"1",	NULL,					NULL},	
	{"bandwidth",	"20",			WLAN_COMMON_NODE,		"HT_BW", 			"0",	"HT_BSSCoexistence",	"1"},
	{"bandwidth",	"40",			WLAN_COMMON_NODE,		"HT_BW", 			"1",	"HT_BSSCoexistence",	"0"},
	{"bandwidth",	"20/40",		WLAN_COMMON_NODE,		"HT_BW", 			"1",	"HT_BSSCoexistence",	"1"},
	{"power",		"100", 			WLAN_COMMON_NODE,		"TxPowerLevel",		"1",	NULL,					NULL},
	{"power",		"80", 			WLAN_COMMON_NODE,		"TxPowerLevel",		"2",	NULL,					NULL},
	{"power",		"60", 			WLAN_COMMON_NODE,		"TxPowerLevel",		"3",	NULL,					NULL},
	{"power",		"40", 			WLAN_COMMON_NODE,		"TxPowerLevel",		"4",	NULL,					NULL},
	{"power",		"20", 			WLAN_COMMON_NODE,		"TxPowerLevel",		"5",	NULL,					NULL},
	{"wmm",			NULL,			WLAN_ENTRY_N_NODE, 		"WMM",				NULL,	NULL,					NULL},
	{"wmm_aspd",	NULL,			NULL, 					NULL,				NULL,	NULL,					NULL},
	{"ssid0",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid1",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid2",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"ssid3",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"SSID",				NULL,	NULL,					NULL},
	{"enable",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"EnableSSID",		NULL,	NULL,					NULL},
	{"hide",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"HideSSID",			NULL,	NULL,					NULL},
	{"maxassoc",	NULL,			WLAN11AC_ENTRY_N_NODE, 	NULL,				NULL,	NULL,					NULL},
	{"apisolate",	NULL,			WLAN11AC_ENTRY_N_NODE, 	NULL,				NULL,	NULL,					NULL},
	{"auth",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"AuthMode",			NULL,	NULL,					NULL},
	{"encrypt",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"EncrypType",		NULL,	NULL,					NULL},
	{"wepauth",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"WepAuth",			NULL,	NULL,					NULL},
	{"weplength",	NULL,			WLAN11AC_ENTRY_N_NODE, 	"WepLength",		NULL,	NULL,					NULL},
	{"wepindex",	NULL,			WLAN11AC_ENTRY_N_NODE, 	"WepIndex",			NULL,	NULL,					NULL},
	{"wep1pwd",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"Wep1pwd",			NULL,	NULL,					NULL},
	{"wep2pwd",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"Wep2pwd",			NULL,	NULL,					NULL},
	{"wep3pwd",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"Wep3pwd",			NULL,	NULL,					NULL},
	{"wep4pwd",		NULL,			WLAN11AC_ENTRY_N_NODE, 	"Wep4pwd",			NULL,	NULL,					NULL},
	
	{NULL,			NULL,			NULL,					NULL,				NULL,	NULL,					NULL}
};


static int sdn_conf_update_to_node(int wifi_type)
{
	FILE *fp = NULL;
	char value[64] = {0};
	char buf[64] = {0};
	int index = 0, i = 0;
	char nodeName[64] = {0};
	char attrName[32] = {0};
	char attrVal[64] = {0};
	char ssid[32] = {0};
	int ret = 0;
	wifi_name_value_attr *setValList = NULL;
		
	if(WIFI_2_4G == type)
	{
		ret = cfg_obj_get_object_attr(WLAN_COMMON_NODE, "sdn_cfg_24g", 0, value, sizeof(value));
	}
	else if(WIFI_5G == type)
	{
		ret = cfg_obj_get_object_attr(WLAN11AC_COMMON_NODE, "sdn_cfg_5g", 0, value, sizeof(value));
	}
	
	if (ret)
	{
		fp = fopen(value, "r");
		if(fp == NULL)
		{
			return -1;
		}
		while(fgets(buf,sizeof(buf),fp) )
		{
			if(WIFI_2_4G == type)
			{
				setValList = &WLanSetValList[0];
			}
			else if(WIFI_5G == type)
			{
				setValList = &WLan11acSetValList[0];
			}

			if(buf[0]=='\0' || buf[0]=='\n')
			{
				continue;
			}
			if(strstr(buf, "_") == NULL)
			{
				sscanf(buf, "%[^=]=%s", attrName, attrVal);
				sscanf(attrName,"ssid%d",&index);
			}
			else
			{
				sscanf(buf, "%[^_]_%[^=]=%s", ssid, attrName, attrVal);
				sscanf(ssid,"ssid%d",&index);
			}

			while(NULL != setValList->sdn_attr)
			{
				if(strcmp(attrName, setValList->sdn_attr) == 0)
				{	
					if(strcmp( attrName, "radio") == 0 || strcmp( attrName, "radiomode") == 0 || strcmp(attrName, "power") == 0)
					{
						if(strcmp(attrVal, setValList->sdn_value) == 0)
						{
							cfg_set_object_attr(setValList->node_name, setValList->attr_name,setValList->attr_val);
						}
					}
					else if(strcmp(setValList->sdn_attr, "bandwidth") == 0)
					{
						if(strcmp(attrVal, setValList->sdn_value) == 0)
						{
							cfg_set_object_attr(setValList->node_name, setValList->attr_name, setValList->attr_val);
							cfg_set_object_attr(setValList->node_name, setValList->attr1_name, setValList->attr1_val);
	
						}
					}						
					else
					{
						if(setValList->node_name == NULL)
						{
							break;
						}
						else
						{	
							
							if(strcmp(setValList->node_name,WLAN_COMMON_NODE) == 0)
							{
								cfg_set_object_attr(setValList->node_name, setValList->attr_name, attrVal);
							}
							else
							{
								snprintf(nodeName, sizeof(nodeName), setValList->node_name, index);
								cfg_set_object_attr(nodeName, setValList->attr_name, attrVal);
							}						
						}
					}
					break;
				}
				setValList++;
			}
		}
		fclose(fp);
	}
}
#endif
static int svc_wlan_update_object(char* path){
	if(svc_wlan_update_opt_cfg(path) != 0){
		tcdbg_printf("svc_wlan_update_opt_cfg() fail\n");
		return CFG2_RET_FAIL;
	}	

	if(WIFI_2_4G == type){
		svc_wlan_execute(path);	
#if defined(TCSUPPORT_CT_UBUS)
		send_wifistatus2ubus();
#endif		
	}
#if defined(TCSUPPORT_WLAN_AC)	
	else if(WIFI_5G == type){
		svc_wlan11ac_execute(path);	
#if defined(TCSUPPORT_CT_UBUS)
		send_wifistatus2ubus();
#endif 
	}
#endif
	else{
		tcdbg_printf("The wifi type is invalid.\n");
		return CFG2_RET_FAIL;
	}
#if defined(TCSUPPORT_SHCT_SDN)
	cfg_set_object_attr(WLAN_COMMON_NODE, "sdn_cfg_24g", " ");
	cfg_set_object_attr(WLAN11AC_COMMON_NODE, "sdn_cfg_5g", " ");
#endif
	return CFG2_RET_SUCCESS;
}
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
static int svc_wlan_roam11k_update_object(char* path){
	if(svc_wlan_update_opt_cfg(path) != 0){
		tcdbg_printf("svc_wlan_update_opt_cfg() fail\n");
		return CFG2_RET_FAIL;
	}	
	
	if(WIFI_2_4G == type){
		svc_wlan_roam11k_execute(path);	
	}else if(WIFI_5G == type){
		svc_wlan11ac_roam11k_execute(path);	
	}else{
		tcdbg_printf("The wifi type is invalid.\n");
		return CFG2_RET_FAIL;
	}
	return CFG2_RET_SUCCESS;
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
static int svc_wlan_roam11v_update_object(char* path){
	if(svc_wlan_update_opt_cfg(path) != 0){
		tcdbg_printf("svc_wlan_update_opt_cfg() fail\n");
		return CFG2_RET_FAIL;
	}	
	
	if(WIFI_2_4G == type){
		svc_wlan_roam11v_execute(path);	
	}else if(WIFI_5G == type){
		svc_wlan11ac_roam11v_execute(path);	
	}else{
		tcdbg_printf("The wifi type is invalid.\n");
		return CFG2_RET_FAIL;
	}
	return CFG2_RET_SUCCESS;
}
#endif
#ifdef TCSUPPORT_WLAN_VENDIE
static int svc_wlan_probevsie_update_object(char* path)
{	
	svc_wlan_probevsie_execute(path);	
	
	return CFG2_RET_SUCCESS;
}

static int svc_wlan_proberesp_update_object(char* path)
{	
	svc_wlan_proberesp_execute(path);	
	
	return CFG2_RET_SUCCESS;
}
static int svc_wlan_proberesp_delete_object(char* path)
{	
	svc_wlan_proberesp_delete(path);
	
	return CFG2_RET_SUCCESS;
}

#endif
#ifdef TCSUPPORT_WLAN_ACS
static int svc_wlan_acs_action(char* path)
{
	char node_name[128] = {0};
	char value[10];
	int  acs_2_4g_action=0;
	int  acs_5g_action=0;
	if(strstr(path,"wlan.acs")!=0){
		if (cfg_obj_get_object_attr(WLAN_ACS_NODE, "Start", 0, value, sizeof(value)) > 0){
			acs_2_4g_action = atoi(value);
			if(acs_2_4g_action == 3 || acs_2_4g_action == 7){
				if(wifimgr_lib_set_autoChannelSel(0, -1, value)!=0)
					return CFG2_RET_FAIL;
			}
			else
				tcdbg_printf("Please set wlan.acs Start=7 or 3\n");
		}
	}
	else if(strstr(path,"wlan11ac.acs")!=0){
		if (cfg_obj_get_object_attr(WLAN11AC_ACS_NODE, "Start", 0, value, sizeof(value)) > 0){
			acs_5g_action = atoi(value);
			if(acs_5g_action == 3 || acs_5g_action == 7){
				if(wifimgr_lib_set_autoChannelSel(1, -1, value)!=0)
					return CFG2_RET_FAIL;
			}
			else
				tcdbg_printf("Please set wlan11ac.acs Start=7 or 3\n");
		}
		
	}
	return CFG2_RET_SUCCESS;
}
#endif

void wifi_reboot_val(int wifi_type)
{
	char node_name[128] = {0};
	int i = 0;
	char ssid[32] = {0}, pwd[32] = {0};
	
	cfg_obj_set_object_attr("root.wlan.common", "syn_num", 0, "0");
	for (i = 1; i <= 8; i++)
	{
		if (0 == wifi_type)
			snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, i);
		else if (1 == wifi_type)
			snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, i);

		cfg_obj_get_object_attr(node_name, "SSID", 0, ssid, sizeof(ssid));
		if ('\0' != ssid[0])
			cfg_obj_set_object_attr(node_name, "SSID_reboot", 0, ssid);
		
		cfg_obj_get_object_attr(node_name, "WPAPSK", 0, ssid, sizeof(ssid));
		if ('\0' != ssid[0])
			cfg_obj_set_object_attr(node_name, "WPAPSK_reboot", 0, ssid);
	}
	
}

int svc_wlan_boot(void)
{        
	char tmp[1024]		= {0};
	FILE *fp			= NULL;
	char node_name[128] = {0};
	char* rt_driver_p 	= NULL;
	char rt_driver[5] 	= {0};
	int flag 			= 0;
	char version[16]	= {0};
	int ret				= 0;
#ifdef TCSUPPORT_WLAN_WDS
	char nodeName[128] 	= {0};
	int wds_enable			= 0;
	int i					= 0;
	char wds_tmp[128]		= {0};
	char tmp2[16]			= {0};
#endif 

#if defined(TCSUPPORT_CT_JOYME2)
	char active[8] = {0};
#endif
#if defined(TCSUPPORT_CT_JOYME)
	char sleepon[8] 	= {0};
#endif
#if defined(TCSUPPORT_NP_CMCC)
	char APOn[8] = {0};
#endif
	char cmd[64] = {0}, cmd_value[64] = {0};

	wifi_reboot_val(0);
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd_value), "e2p 0", cmd_value, 1);
	snprintf(cmd, sizeof(cmd), "%s > /tmp/rt_device", cmd_value);
	system(cmd);
#if defined(TCSUPPORT_CT_JOYME2)
	cfg_obj_get_object_attr(WLAN_RATEPRIORITY_NODE, "Active", 0, active, sizeof(active));
	if ( strcmp(active, "1") == 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "LowRateCtrl=1", cmd, 0);
		system(cmd);
	}
	
	cfg_obj_get_object_attr(WLAN11AC_RATEPRIORITY_NODE, "Active", 0, active, sizeof(active));
	if ( strcmp(active, "1") == 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "LowRateCtrl=1", cmd, 0);
		system(cmd);
	}
#endif
	fp = fopen("/tmp/rt_device", "r");
	if(fp){
		while(fgets(tmp, 1024, fp)){
			if((rt_driver_p = strstr(tmp,"7592")) || (rt_driver_p= strstr(tmp,"7615"))){
				snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
				strncpy(rt_driver, rt_driver_p, 4);
				cfg_set_object_attr(node_name, "rt_device", rt_driver);
				flag = 1;
				printf("read WLAN driver from rt_device success!\n");
				break;
			}
		}
		fclose(fp);	
	}
	if(0 == flag){
		printf("read WLAN driver from rt_device failed,set with default value!\n");
		snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
		ret = wifimgr_lib_get_version(WIFI_2_4G, version, sizeof(version));
		if (0 == ret)
		{	
			if(!strncmp(version, "7915D",5))
			{
				memset(version,0,16);
				strcpy(version,"7915");
			}	
			cfg_set_object_attr(node_name, "rt_device", version);
	}
	}

	svc_wlan_update_opt_cfg(WLAN_NODE);
	/* init wds pre state */
#ifdef TCSUPPORT_WLAN_WDS
	if(cfg_get_object_attr(node_name, "APOn", tmp, sizeof(tmp)) > 0){
		if(!strcmp(tmp, WLAN_APON)){
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WLAN_WDS_NODE);
			memset(wds_tmp,0,sizeof(wds_tmp));	
			/* wds enable/disable */
			if (cfg_get_object_attr(nodeName, "WdsEnable", wds_tmp, sizeof(wds_tmp)) > 0)
			{
				wds_enable = atoi(wds_tmp);
			} 
			pre_sys_state.WdsEnable = wds_enable;
	
			/* WdsEncrypType */
			memset(wds_tmp,0,sizeof(wds_tmp));	
			if(cfg_get_object_attr(nodeName, "WdsEncrypType", wds_tmp, sizeof(wds_tmp)) > 0)
			{
				strcpy(pre_sys_state.WdsEncrypType, wds_tmp);
			}

			/* wds key */
			memset(wds_tmp,0,sizeof(wds_tmp));	
			if(cfg_get_object_attr(nodeName, "WdsKey", wds_tmp, sizeof(wds_tmp)) > 0)
			{
				strcpy(pre_sys_state.WdsKey, wds_tmp);
			}

			/* wds mac */
			for(i = 0; i < MAX_WDS_ENTRY; i++)
			{
				snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0)
				{
					strcpy(pre_sys_state.Wds_MAC[i], tmp); 
				}
			}

			/* WepKeyStr */
			if (wds_enable != 0)
			{/* wds enable */
				memset(nodeName, 0, sizeof(node_name));
				snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
				if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0)
				{
					if (strstr(tmp, "WEP"))
					{
						for(i = 0; i < MAX_WDS_ENTRY; i++) {
							snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
							if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) <= 0)
							{
								continue;
							}
							strcpy(pre_sys_state.WepKeyStr[i], tmp);	
						}
					}
				}
			}
		}
	}
#endif	

#if defined(TCSUPPORT_CT_JOYME)
	memset(sleepon, 0, sizeof(sleepon));
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
	if((cfg_get_object_attr(node_name, "SleepFlag", sleepon, sizeof(sleepon)) > 0) 
									&& (strcmp(sleepon,"1") == 0)){
		cfg_set_object_attr(node_name, "SleepFlag", "0");
		cfg_set_object_attr(node_name, "APOn", "1");

		svc_wlan_execute(WLAN_NODE);
	}
#endif
	dowscd();
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wlanBoot", "1");

#if defined(TCSUPPORT_NP_CMCC)
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", APOn, sizeof(APOn)) > 0)
	{
		wlan_last_apon = atoi(APOn);
	}
#endif
	return CFG2_RET_SUCCESS;
}

#if defined(TCSUPPORT_WLAN_AC)	
int svc_wlan11ac_boot(void)
{

#ifdef TCSUPPORT_WLAN_WDS
	char node_name[128]		= {0};
	int wds_enable 			= 0;
	int i 					= 0;
	char wds_tmp[128] 		= {0};
	char tmp2[16]			= {0};
#endif     
	char tmp[1024]			= {0};
	FILE *fp 				= NULL;
	char nodeName[128]		= {0};

	char *rt_driver 		= NULL;
	int flag 				= 0;
#if defined(TCSUPPORT_NP_CMCC)
	char APOn[8] = {0};
	char mac[20] = {0};
	char FixedAPMode[32] = {0};
	char currentRadio[4] = {0};
	char cmd[256] = {0};
	char IFName[12] = {0};
#endif
	int ret					= 0;
	char version[16]		= {0};
	char cmdSys[64] = {0}, cmd_value[64] = {0};

	wifi_reboot_val(1);
	memset(cmdSys, 0, sizeof(cmdSys));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd_value), "e2p 0", cmd_value, 1);
	snprintf(cmdSys, sizeof(cmdSys), "%s > /tmp/rt_device", cmd_value);
	system(cmdSys);
	fp=fopen("/tmp/rt_device","r");
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
	if(fp){
		while(fgets(tmp,1024,fp)){
			if((rt_driver = strstr(tmp,"7612")) || (rt_driver = strstr(tmp,"7663")) || (rt_driver = strstr(tmp,"7615"))){
				rt_driver[4] = '\0';
				cfg_set_object_attr(nodeName, "rt_device", rt_driver);
				flag = 1;
				printf("read WLAN driver from rt_device success!\n");
				break;
			}
		}
		fclose(fp); 
	}
	if(0 == flag){
		tcdbg_printf("read WLAN driver from rt_device failed"
			",set with default value!\n");
		ret = wifimgr_lib_get_version(WIFI_5G, version, sizeof(version));
		if (0 == ret)
		{
			if(!strncmp(version, "7915D",5))
			{
				memset(version,0,16);
				strcpy(version,"7915");
			}
			cfg_set_object_attr(nodeName, "rt_device", version);
	}
	}

#if defined(TCSUPPORT_NP_CMCC)
	if(cfg_get_object_attr(INFO_ETHER_NODE, "mac", mac, sizeof(mac)) >= 0)
	{
		if((cfg_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", FixedAPMode, sizeof(FixedAPMode)) >= 0)
			&& (strcmp(FixedAPMode, "APClient") == 0))
		{
			if((cfg_get_object_attr(APCLI_COMMON_NODE, "currentRadio", currentRadio, sizeof(currentRadio)) >= 0)
				&& (strcmp(currentRadio, "0") == 0))
			{
				cfg_obj_set_object_attr(WLAN_ENTRY1_NODE, "ApCliMac_Manual_Flag", 0, "1");
				cfg_obj_set_object_attr(WLAN_ENTRY1_NODE, "ApCliMac_Manual", 0, mac);
				cfg_obj_set_object_attr(WLAN11AC_ENTRY1_NODE, "ApCliMac_Manual_Flag", 0, "0");
				cfg_obj_set_object_attr(WLAN11AC_ENTRY1_NODE, "ApCliMac_Manual", 0, "");
				
			}
			else
			{
				cfg_obj_set_object_attr(WLAN_ENTRY1_NODE, "ApCliMac_Manual_Flag", 0, "0");
				cfg_obj_set_object_attr(WLAN_ENTRY1_NODE, "ApCliMac_Manual", 0, "");
				cfg_obj_set_object_attr(WLAN11AC_ENTRY1_NODE, "ApCliMac_Manual_Flag", 0, "1");
				cfg_obj_set_object_attr(WLAN11AC_ENTRY1_NODE, "ApCliMac_Manual", 0, mac);
			}
			
			svc_wlan_update_opt_cfg(WLAN_NODE);
		}
	}
#endif
	svc_wlan_update_opt_cfg(WLAN11AC_NODE);
	dowscd();
	
	/* init wds pre state */
#ifdef TCSUPPORT_WLAN_WDS
	if(cfg_get_object_attr(nodeName, "APOn", tmp, sizeof(tmp)) > 0){
		if(!strcmp(tmp, WLAN_APON)){
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), WLAN11AC_WDS_NODE);
			memset(wds_tmp,0,sizeof(wds_tmp));	
			/* wds enable/disable */
			if (cfg_get_object_attr(node_name, "WdsEnable", wds_tmp, sizeof(wds_tmp)) <= 0){
				return -1;
			} 
			else {
				wds_enable = atoi(wds_tmp);
			}
			pre_sys_state.WdsEnable_11ac = wds_enable;
	
			/* WdsEncrypType */
			if(cfg_get_object_attr(node_name, "WdsEncrypType", wds_tmp, sizeof(wds_tmp)) < 0){
				return -1;
			}
			strcpy(pre_sys_state.WdsEncrypType_11ac, wds_tmp);

			/* wds key */
			if(cfg_get_object_attr(node_name, "WdsKey", wds_tmp, sizeof(wds_tmp)) <= 0){
				return -1;
			}
			strcpy(pre_sys_state.WdsKey_11ac, wds_tmp);

			/* wds mac */
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
				if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) <= 0){
					return -1;
				}
				strcpy(pre_sys_state.Wds_MAC_11ac[i], tmp); 
			}

			/* WepKeyStr */
			if (wds_enable != 0) {/* wds enable */
				memset(nodeName, 0, sizeof(node_name));
				snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
				if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) < 0){
					return -1;
				}
				if (strstr(tmp, "WEP")) {
					for(i = 0; i < MAX_WDS_ENTRY; i++) {
						snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
						if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) <= 0){
							continue;
						}
						strcpy(pre_sys_state.WepKeyStr_11ac[i], tmp);	
					}
				}
			}
		}
	}
#endif	

	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wlanBoot_ac", "1");
#if defined(TCSUPPORT_NP_CMCC)
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", APOn, sizeof(APOn)) > 0){
		wlan11ac_last_apon = atoi(APOn);
	}
#endif

	return 0;
}
#endif

#ifdef TCSUPPORT_WLAN_VENDIE
int beacon_send_msg_2_dbus(int entryidx ,int status)
{
	char  change2finished[128]={0};
	char  change2busy[128]={0};
	char  change2stop[128]={0};
	int ret = 0;
	memset(change2finished,0,sizeof(change2finished));
	memset(change2busy,0,sizeof(change2busy));
	memset(change2stop,    0,sizeof(change2stop));
	snprintf(change2finished, sizeof(change2finished),BEACONVSIE_STATUS_FINISHED_MESSAGE,entryidx+1); 
	snprintf(change2busy, sizeof(change2busy),BEACONVSIE_STATUS_BUSY_MESSAGE,entryidx+1); 
	snprintf(change2stop,     sizeof(change2stop),    BEACONVSIE_STATUS_STOP_MESSAGE,    entryidx+1); 
	
	if(status == FINISHED)
	{
	    ret = ecnt_event_send(ECNT_EVENT_DBUS,
						ECNT_EVENT_DBUS_INFORM,
						change2finished,
						strlen(change2finished)+1);
	}
	else if(status == BUSY)
	{
		ret = ecnt_event_send(ECNT_EVENT_DBUS,
						ECNT_EVENT_DBUS_INFORM,
						change2busy,
						strlen(change2busy)+1);
	}
	else if(status == STOP)
	{
		ret = ecnt_event_send(ECNT_EVENT_DBUS,
						ECNT_EVENT_DBUS_INFORM,
						change2stop,
						strlen(change2stop)+1);
	}
	
	return 0;
}
#endif

#ifdef TCSUPPORT_WLAN_VENDIE
int beaconvsie_entry_action(pt_wlan_evt param,int type)
{
	int entryidx,interfaceidx,ssididx=0;
	int ret;
	char value[1000];
	char SSIDAlias[20];
	int currentEntry=0;
	char interfacebuf[10] = {0};
	unsigned char vend_ie_driver_argu [3072]= {0};
	int len=0;
	int vend_ie_driver_arge_index = 0;
	char nodePath[64] = {0};
	char beaconvsieEntryPath[64] = {0};
	char wlanmappingEntryPath[64] = {0};
	unsigned char *p = NULL;
	int MaxEntryNum = MAX_ENTRY_NUM;
	int base=0;
	int offset=0;
	int needsend=0;
	int entry_needsend[MAX_ENTRY_NUM]={0};
	int dbglevel=0;
	int Stat;
	int num_of_entry_need_to_send=0;
	int currentEntryNum=0;
	int currentEntry_interface=-1;
	int VSIE_Need_Delete[MAX_INDEX_NUM]={0};
	char  change2busy[128]={0};
	char  change2finished[128]={0};
	
	/*dbg switch*/
	/*dbg on: echo 1>/tmp/ecnt_beaconvsie_dbg_enable*/
	if(access(ECNT_BEACONVSIE_DBG_ENABLE, F_OK) == 0)
	{
		dbglevel=1;
	}
	if(dbglevel)
	{
		if(type == BEACONVSIE_COMMIT)
			tcdbg_printf("excute BEACONVSIE_COMMIT\n");
		else if(type == BEACONVSIE_UPDATE)
			tcdbg_printf("excute BEACONVSIE_UPDATE\n");
		}
		
	char* path = param->buf;
	if (cfg_obj_query_object(path,NULL,NULL) <= 0){
		tcdbg_printf(" %s doesn't exsit!\n", path);
		return CFG2_RET_SUCCESS;
	}
	
	/*get currentEntry*/
	sscanf(path,"root.beaconvsie.entry.%d",&currentEntry);
	
	if(dbglevel)
		tcdbg_printf("currentEntry:%d\n",currentEntry);
	
	memset(nodePath,0,sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath),BEACONVSIE_COMMON_NODE);
	if (cfg_obj_get_object_attr(nodePath, "MaxEntryNum", 0, value, sizeof(value)) > 0)
	{
		MaxEntryNum = atoi(value);
		if (MaxEntryNum < 0 || MaxEntryNum > MAX_ENTRY_NUM)
		{
			MaxEntryNum = MAX_ENTRY_NUM;
		}
		memset(value,0,sizeof(value));
	}
	
	/*mapping from wlanmapping node*/
	if(type == BEACONVSIE_COMMIT)
	{ 
		for(entryidx=0; entryidx < MAX_ENTRY_NUM; entryidx++)
		{
			memset(beaconvsieEntryPath,0,sizeof(beaconvsieEntryPath));
			snprintf(beaconvsieEntryPath, sizeof(beaconvsieEntryPath),BEACONVSIE_ENTRY_N_NODE,entryidx+1);
			if (cfg_obj_query_object(beaconvsieEntryPath,NULL,NULL) > 0){
				currentEntryNum++;
			}
			if (cfg_obj_get_object_attr(beaconvsieEntryPath, "SSIDIndex", 0, value, sizeof(value)) > 0)
			{
				ssididx = atoi(value);
				if(ssididx > 0)
				{
					memset(wlanmappingEntryPath,0,sizeof(wlanmappingEntryPath));
					snprintf(wlanmappingEntryPath, sizeof(wlanmappingEntryPath), "root.wlanmapping.entry.%d", ssididx);
					memset(value,0,sizeof(value));
					if (cfg_obj_get_object_attr(wlanmappingEntryPath, "entryidx", 0, value, sizeof(value)) > 0)
					{
						cfg_obj_set_object_attr(beaconvsieEntryPath,"InterfaceIndex",0,value);	
					}	
					if (cfg_obj_get_object_attr(wlanmappingEntryPath, "SSIDAlias", 0, value, sizeof(value)) > 0)
					{
						cfg_obj_set_object_attr(beaconvsieEntryPath,"SSIDAlias",0,value);	
					}	
					
				}
			}
		}
		cfg_obj_set_object_attr(nodePath,"CurrentEntryNum",0,itoa(currentEntryNum));	
	}
	

	if (cfg_obj_get_object_attr(nodePath, "Enable", 0, value, sizeof(value)) > 0)
	{
		if (!strcmp(value, "1"))
		{
			if (cfg_obj_get_object_attr(nodePath, "Action", 0, value, sizeof(value)) > 0)
			{
				if (!strcmp(value, "Start"))
				{
					/*get currentEntry's interface*/
					if (cfg_obj_get_object_attr(path, "InterfaceIndex", 0, value, sizeof(value)) > 0)
					{
						currentEntry_interface = atoi(value);
					}
					if(dbglevel)
						tcdbg_printf("currentEntry_interface:%d\n",currentEntry_interface);

			
					if(currentEntry_interface< MAX_INDEX_NUM/2)
						sprintf(interfacebuf, "ra%d", currentEntry_interface);
					else
						sprintf(interfacebuf, "rai%d", currentEntry_interface-MAX_INDEX_NUM/2);
							
					/*check if there is any other entry has the same interfaceidx with the currentEntry, 
					   if so,then group them together and send by one interface*/
					for(entryidx=0; entryidx < MaxEntryNum; entryidx++)
					{					
						memset(nodePath,0,sizeof(nodePath));
						snprintf(nodePath, sizeof(nodePath),BEACONVSIE_ENTRY_N_NODE,entryidx+1); 
						/*if this entry doesn't exsit, skip */
						if (cfg_obj_query_object(nodePath,NULL,NULL) <= 0)
						{
							if(dbglevel)
								tcdbg_printf("Entry.%d doesn't exsit, Skip...\r\n", entryidx+1);
							continue;
						}
						memset(value,0,sizeof(value));

						if (cfg_obj_get_object_attr(nodePath, "ServiceName", 0, value, sizeof(value)) > 0)
						{
							if(!strcmp(value,"NULL"))
							{
								continue;
							}
						}
		
						memset(SSIDAlias,0,sizeof(SSIDAlias));
						memset(value,0,sizeof(value));
						if (cfg_obj_get_object_attr(nodePath, "InterfaceIndex", 0, value, sizeof(value)) > 0)
						{
							interfaceidx = atoi(value);
							if(strlen(value)>0 && interfaceidx >= 0)/*InterfaceIndex is not empty*/
							{
								 
								if(interfaceidx==currentEntry_interface)/*handle the same interface */
								{

									if(type == BEACONVSIE_COMMIT)
									{  
										if(entryidx == currentEntry-1)/*handle currentEntry*/
										{
										cfg_obj_set_object_attr(nodePath,"Status",0,_BUSY_);
										if(dbglevel)
											if (cfg_obj_get_object_attr(nodePath, "SSIDAlias", 0, SSIDAlias, sizeof(SSIDAlias)) > 0)
											{
												tcdbg_printf("ECNT_BeaconVSIE_DBG: Entry.%d(%s) become busy\r\n", entryidx+1,SSIDAlias);
											}
											/*send busy message to dbus */
										beacon_send_msg_2_dbus(entryidx,BEACONVSIE_ENTRY_BUSY);
											entry_needsend[entryidx]=1;
											num_of_entry_need_to_send++;
									}
										else/*handle other Entry that has the same interface with currentEntry*/
										{
											memset(value,0,sizeof(value));
											if (cfg_obj_get_object_attr(nodePath, "Status", 0, value, sizeof(value)) > 0)
											{  
												Stat = atoi(value);
												if(Stat == BUSY)/*if this Entry(who shares the same interface with current Entry) is running, then resend it.*/
												{
													if(dbglevel)
														tcdbg_printf("Entry.%d is running, ready to resend \n",entryidx+1);
									entry_needsend[entryidx]=1;
									num_of_entry_need_to_send++;
									
												}
											}
										}
									}
										
									 
									if(type == BEACONVSIE_UPDATE)
									{ 
										memset(value,0,sizeof(value));
										if (cfg_obj_get_object_attr(nodePath, "Status", 0, value, sizeof(value)) > 0)
										{  
											Stat = atoi(value);
											if(Stat == BUSY)/*if this Entry(who shares the same interface with current Entry) is running, then resend it.*/
											{
												if(dbglevel)
														tcdbg_printf("[Update]Entry.%d need to be reserved\n",entryidx+1);
													entry_needsend[entryidx]=1;
													num_of_entry_need_to_send++;

											}
										}
										memset(value,0,sizeof(value));
									}
								
								}
							}
							memset(value,0,sizeof(value));
						}
					}
					if(dbglevel)
						tcdbg_printf("num_of_entry_need_to_send:%d\n",num_of_entry_need_to_send);
					if(!num_of_entry_need_to_send)
						VSIE_Need_Delete[currentEntry_interface]=1;

					
					/* send by Interface*/
						needsend=0;
						offset = 0;
					    for(entryidx=0; entryidx < MAX_ENTRY_NUM; entryidx++)
					    {
					    	if(entry_needsend[entryidx] == 0)
					    	{
								continue;
					    	}
							needsend = 1;
							
							
							memset(nodePath,0,sizeof(nodePath));
							snprintf(nodePath, sizeof(nodePath),BEACONVSIE_ENTRY_N_NODE,entryidx+1); 
						/********************build VSIE data************************/
						/*2.data*/
						/*2.1 ElementID: 221 (Addr:base+offset )*/
							vend_ie_driver_argu[base+offset]= 0xdd; 
					
						/*2.2 Len of IEData (Addr:base+offset+1 )*/
							memset(value,0,sizeof(value));
							if (cfg_obj_get_object_attr(nodePath, "LenofIEData", 0, value, sizeof(value)) > 0)
							{
								len = atoi(value);
								if(len < 0 || len > 255)
								{
									tcdbg_printf("LenofIEData set error!(0~255)\r\n");
									return FAIL;
								}
							 	vend_ie_driver_argu[base+offset+1]= len;
							}
							
						/*2.3 IEData (Addr:base+offset+2 )*/
							memset(value,0,sizeof(value));
						
							if (cfg_obj_get_object_attr(nodePath, "IEData", 0, value, sizeof(value)) > 0)
							{	
								p = vend_ie_driver_argu + offset + 2;
								vend_ie_driver_arge_index = vendie_aton(value,p);
								if(vend_ie_driver_arge_index == -1) 
								{
									tcdbg_printf("doVendorIESet() IEData error!\r\n");
								/*         IEData            */
								/*	xx,xx,xx,xx...xx,xx  */
									
									return FAIL;
								}
								if(vend_ie_driver_arge_index != len) 
								{
									tcdbg_printf("Warning:LenofIEData dismatch with IEData!\r\n");
								}
							}
			                offset += len+2;
							
						/*********************build VSIE data*************************/
					    }	
						
					if(dbglevel){
						tcdbg_printf("entry_needsend:\n");
						for(entryidx=0; entryidx < MAX_ENTRY_NUM; entryidx++){
							tcdbg_printf("%d ",*(entry_needsend+entryidx));
							}
						tcdbg_printf("\n");
						tcdbg_printf("needsend: %d\n",needsend);
						}
					
					if(offset>sizeof(vend_ie_driver_argu))
						return FAIL;

						
					/*3.if no need,then skip. if UPDATE, need to delete the last VSIE entry*/
					if(!needsend)
					{   
						if(type == BEACONVSIE_UPDATE && VSIE_Need_Delete[currentEntry_interface])
						{
							del_VSIE_by_interface(currentEntry_interface);
							if(dbglevel)
								tcdbg_printf("ECNT_BeaconVSIE_DBG: delete the last Entry of Interface = %d\r\n", currentEntry_interface);
							
						}
						/*continue;*/
					}
					else/*4.send to wifi driver*/
					{
						insert_VSIE_to_Beacon_by_interface(interfacebuf, vend_ie_driver_argu, offset);
						
					}
				   return SUCCESS;
				 }
				else if(!strcmp(value, "Stop"))
				{
					/*what does Stop do?*/
					/*Case 1.stop current Entry(remove its VSIE content from Beacon frame) ,Only current Entry is busy*/
					/*  or */
					/*Case 2.resend any other Entry tha is busy and has the same InterfaceIndex with current Entry.*/
							
					/*get which wifi interface that current Entry belong to	*/	
					if (cfg_obj_get_object_attr(path, "InterfaceIndex", 0, value, sizeof(value)) > 0)
					{	
						currentEntry_interface = atoi(value);
					}
					if(dbglevel)
						tcdbg_printf("ECNT_BeaconVSIE_DBG: currentEntry_interface:%d\r\n", currentEntry_interface);
												


					if(currentEntry_interface < MAX_INDEX_NUM/2)
						sprintf(interfacebuf, "ra%d", currentEntry_interface);
						else
						sprintf(interfacebuf, "rai%d", currentEntry_interface-MAX_INDEX_NUM/2);
						
					
					/*search for all the other entry who is running and has the same interface with current entry */
					for(entryidx=0; entryidx < MaxEntryNum; entryidx++)
					{					
						memset(nodePath,0,sizeof(nodePath));
						snprintf(nodePath, sizeof(nodePath),BEACONVSIE_ENTRY_N_NODE,entryidx+1); 
						if (cfg_obj_query_object(nodePath,NULL,NULL) <= 0)
						{
							continue;
						}
						
						memset(value,0,sizeof(value));
						if (cfg_obj_get_object_attr(nodePath, "InterfaceIndex", 0, value, sizeof(value)) > 0)
						{
							interfaceidx = atoi(value);
							if(strlen(value)>0 && interfaceidx >= 0)/*InterfaceIndex is not empty*/
							{
								 /*handle the same interface */
								if(interfaceidx==currentEntry_interface && entryidx != currentEntry-1)
								{
									memset(value,0,sizeof(value));
									if (cfg_obj_get_object_attr(nodePath, "Status", 0, value, sizeof(value)) > 0)
									{  
										Stat = atoi(value);
										if(Stat == BUSY)/*if this Entry(who shares the same interface with current Entry) is running, then resend it.*/
										{
											if(dbglevel)
												tcdbg_printf("Entry.%d is running, ready to resend \n",entryidx+1);
											entry_needsend[entryidx]=1;
											num_of_entry_need_to_send++;
											needsend=1;

										}
									}
								}
								else/*delete currentEntry by interface directly*/
								{
									needsend=0;
								}
							}
						}
					}
					
					memset(nodePath,0,sizeof(nodePath));
					snprintf(nodePath, sizeof(nodePath),BEACONVSIE_ENTRY_N_NODE,currentEntry); 
					cfg_obj_set_object_attr(nodePath,"Status",0,_STOP_);
					beacon_send_msg_2_dbus(currentEntry-1,BEACONVSIE_ENTRY_STOP);
					
					if(dbglevel)
						tcdbg_printf("Entry.%d has been stopped\r\n", currentEntry);

					if(!needsend)
					{
						/*Case 1:*/	
						/*	if no other Entry has the same interface with currentEntry,*/
						/*then delete by interface to stop current Entry.*/
						/*PS:this case is simple*/
						
						/*send delete cmd to wifi driver by interface*/
						if (wifimgr_lib_OidQueryInformation(RT_OID_AP_VENDOR_IE_DEL, interfacebuf, NULL, 0) < 0)
						{
							tcdbg_printf("doVendorIESet() ERROR:RT_OID_AP_VENDOR_IE_DEL\n");
							return FAIL;			
						}
						else
						{
							printf("vendor IE del OK\n");		
						}
					}
					else
					{
						/*Case 2:*/
						/*	if any other Entry has the same interface with currentEntry,*/
						/*then resend other Entry (but not including currentEntry) by interface */
						/*PS:this case is complicated*/

						if(dbglevel)
							tcdbg_printf("Other Entrys has been resended\r\n");
					
						/* send by Interface*/
						offset = 0;
					    for(entryidx=0; entryidx < MAX_ENTRY_NUM; entryidx++)
					    {
					    	if(entry_needsend[entryidx] == 0)
					{					
								continue;
					    	}
				
						memset(nodePath,0,sizeof(nodePath));
						snprintf(nodePath, sizeof(nodePath),BEACONVSIE_ENTRY_N_NODE,entryidx+1); 
							/*********************build VSIE data*************************/
							/*2.data*/
							/*2.1 ElementID: 221 (Addr:base+offset )*/
							vend_ie_driver_argu[base+offset]= 0xdd; 
					
							/*2.2 Len of IEData (Addr:base+offset+1 )*/
							memset(value,0,sizeof(value));
							if (cfg_obj_get_object_attr(nodePath, "LenofIEData", 0, value, sizeof(value)) > 0)
							{
								len = atoi(value);
								if(len < 0 || len > 255)
						{
									tcdbg_printf("LenofIEData set error!(0~255)\r\n");
									return FAIL;
								}
							 	vend_ie_driver_argu[base+offset+1]= len;
						}
							
							/*2.3 IEData (Addr:base+offset+2 )*/
						memset(value,0,sizeof(value));
						
							if (cfg_obj_get_object_attr(nodePath, "IEData", 0, value, sizeof(value)) > 0)
						{
								p = vend_ie_driver_argu + offset + 2;
								vend_ie_driver_arge_index = vendie_aton(value,p);
								if(vend_ie_driver_arge_index == -1) 
							{
									tcdbg_printf("doVendorIESet() IEData error!\r\n");
									/*         IEData      */
									/*	xx,xx,xx,xx...xx,xx*/
								 
									return FAIL;
								}
								if(vend_ie_driver_arge_index != len) 
								{
									tcdbg_printf("Warning:LenofIEData dismatch with IEData!\r\n");
								}
							}
			                offset += len+2;
							
							/*********************build VSIE data*************************/
						}

						insert_VSIE_to_Beacon_by_interface(interfacebuf, vend_ie_driver_argu, offset);
					}
					
					
					return SUCCESS;
				}
				else
					tcdbg_printf("Unrecongnized Action! Please set Action='Start' or 'Stop'\n");
		
			}
			else
				tcdbg_printf("No Action! Please set Action='Start' or 'Stop'\n");
		}
		else
			tcdbg_printf("BeaconVSIE is disabled,please set Enable=1\n");
	}
	else
		tcdbg_printf("BeaconVSIE is disabled,please set Enable=1\n");

	return SUCCESS;

}
#endif


int svc_wlan_handle_event_update(pt_wlan_evt param)
{
	char mssid[32] = {0};
	char* path = param->buf;
#if defined(TCSUPPORT_CT_DBUS)
	char dbus_flag[8] = {0};
	
	memset(dbus_flag, 0, sizeof(dbus_flag));
	cfg_obj_get_object_attr(SYS_ENTRY_NODE, "ctcDbusFlag", 0, dbus_flag, sizeof(dbus_flag));
	if(!strcmp(dbus_flag, "5"))
		return CFG2_RET_SUCCESS;
#endif
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}

	if (svc_wlan_update_object(path) < 0){
		printf("svc_wlan_update_object: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}

	if(cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_wifi", mssid, sizeof(mssid)) > 0 && 0 == strcmp(mssid, "Yes"))
    {
        cfg_obj_set_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id", 0, "0");
    }

	return CFG2_RET_SUCCESS;
}
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
int svc_wlan_roam11k_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	printf("func:svc_wlan_roam11k_handle_event_update\n");
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_roam11k_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_roam11k_update_object(path) < 0){
		printf("svc_wlan_roam11k_handle_event_update: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}
	
	return CFG2_RET_SUCCESS;
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
int svc_wlan_roam11v_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_roam11v_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_roam11v_update_object(path) < 0){
		printf("svc_wlan_roam11v_handle_event_update: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}
	
	return CFG2_RET_SUCCESS;
}
#endif
int isChannel(char *channel)
{	
	int len = strlen(channel);
	int i = 0;
	int j = 0;
	
	for(i = 0; i < len; i ++)
	{
		if(channel[i] >= 48 && channel[i] <= 57)
			j++;
	}
	if(j == len)
		return 1;
	
	return 0;
}


int add_wlan24g_Interferenceinfo(char *info_data, FILE *fp_dst)
{	
	FILE *fp = NULL;
	char cmd[128] = {0};
	int i = 1;
	char *buffer = NULL;
	size_t size = 0;
	ssize_t ret;
	char No[6] = {0};
	char Channel[4] = {0};
	char SSID[64] = {0};
	char BSSID[32] = {0};
	char RSSI[6] = {0};
	char Security[32] = {0};
	char buf[128] = {0};
	int flag = 0;
	char data[128] =
		"{\n"
					"\"SSID\":\"%s\",\n"
					"\"Channel\":\"%s\",\n"
					"\"RSSI\":\"%s\",\n"
					"\"BSSID\":\"%s\""
		"},\n"
		;
	char tmp[128] = {0};
	
	fp = fopen(WLAN24G_SITESURVEY_CMD, "r");
	if (NULL == fp)
	{
		return -1;
	}
	while((ret = getline(&buffer, &size, fp)) != -1)
	{	
		i ++;
		if(i >= 4)
		{		
			memset(No, 0, sizeof(No));
			memset(Channel, 0, sizeof(Channel));
			memset(SSID, 0, sizeof(SSID));
			memset(BSSID, 0, sizeof(BSSID));
			memset(RSSI, 0, sizeof(RSSI));
			snprintf(No, 4, "%s", buffer);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 4, "%s", buffer+4);
			sscanf(tmp, "%[^ ]", Channel);
			snprintf(SSID, 33, "%s", buffer+8);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 19, "%s", buffer+41);
			sscanf(tmp, "%[^ ]", BSSID);
			snprintf(Security, 23, "%s", buffer+60);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 5, "%s", buffer+83);
			sscanf(tmp, "%[^ ]", RSSI);
			
			if(strlen(BSSID) >= 17 && 1 == isChannel(Channel) && atoi(RSSI) < 0)
			{
				if(strlen(info_data) > 0){
					fputs(info_data, fp_dst); 
					memset(info_data, 0, 512);
				}
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), data, SSID, Channel, RSSI, BSSID);
				strcat(info_data, buf);
				flag = 1;
			}
		}
	}
	
	if(buffer)
		free(buffer);
	if(fp)
		fclose(fp);

	return flag;
	
}
int add_wlan5g_Interferenceinfo(char *info_data, FILE *fp_dst)
{
	FILE *fp = NULL;
	char cmd[128] = {0};
	int i = 0;
	char *buffer = NULL;
	size_t size = 0;
	ssize_t ret;
	char No[6] = {0};
	char Channel[4] = {0};
	char SSID[64] = {0};
	char BSSID[32] = {0};
	char RSSI[6] = {0};
	char Security[32] = {0};
	char buf[128] = {0};
	int flag = 0;
	char data[128] =
	"{\n"
					"\"SSID\":\"%s\",\n"
					"\"Channel\":\"%s\",\n"
					"\"RSSI\":\"%s\",\n"
					"\"BSSID\":\"%s\""
	"},\n"
	;
	char tmp[128] = {0};

	fp = fopen(WLAN5G_SITESURVEY_CMD, "r");
	if (NULL == fp)
	{
		return -1;
	}
	while((ret = getline(&buffer, &size, fp)) != -1)
	{	
		i ++;
		if(i >= 4)
		{	
			memset(Channel, 0, sizeof(Channel));
			memset(SSID, 0, sizeof(SSID));
			memset(BSSID, 0, sizeof(BSSID));
			snprintf(No, 4, "%s", buffer);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 4, "%s", buffer+4);
			sscanf(tmp, "%[^ ]", Channel);
			snprintf(SSID, 33, "%s", buffer+8);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 19, "%s", buffer+41);
			sscanf(tmp, "%[^ ]", BSSID);
			snprintf(Security, 23, "%s", buffer+60);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, 5, "%s", buffer+83);
			sscanf(tmp, "%[^ ]", RSSI);
		
			if(strlen(BSSID) >= 17 && 1 == isChannel(Channel) && atoi(RSSI) < 0)
			{
				if(strlen(info_data) > 0){
					fputs(info_data, fp_dst); 
					memset(info_data, 0, 512);
				}
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), data, SSID, Channel, RSSI, BSSID);
				strcat(info_data, buf);
				flag=1;				
			}
		}
	}
	if(buffer)
		free(buffer);
	if(fp)
		fclose(fp);

	return flag;
	
}
void find_wlan(int i, char *BER, char *RSSI)
{
	FILE *fp = NULL;
	char *p = NULL;
	char *r = NULL;
	char cmd[64] = {0};
	char get_wlan_info[64] = {0};
	char wlaninfo_path[64] = {0};
	char s[1024] = {0}, tmp[10] = {0};
	char *buffer = NULL;
	size_t size = 0;
	ssize_t ret;
	
	if (i >= 1 && i <= WLAN_2_4_G_NODE_LEN)
	{	
		wifimgr_lib_set_WIFI_CMD("ra0", -1, sizeof(get_wlan_info), "stat", get_wlan_info, 1);
		snprintf(wlaninfo_path, sizeof(wlaninfo_path), "%s", WLAN24GINFO_PATH);
		snprintf(cmd, sizeof(cmd), "%s > %s", get_wlan_info, wlaninfo_path);
		system(cmd);
		fp = fopen(wlaninfo_path, "r");
	}
		
	else if (i > WLAN_2_4_G_NODE_LEN && i <= WLAN_MAPPING_NODE_LEN)
	{	
		wifimgr_lib_set_WIFI_CMD("rai0", -1, sizeof(get_wlan_info), "stat", get_wlan_info, 1);
		snprintf(wlaninfo_path, sizeof(wlaninfo_path), "%s", WLAN5GINFO_PATH);
		snprintf(cmd, sizeof(cmd), "%s > %s", get_wlan_info, wlaninfo_path);
		system(cmd);
		fp = fopen(wlaninfo_path, "r");
	}
		
	if (NULL == fp)
	{
		return -1;
	}
	while ((ret = getline(&buffer, &size, fp)) != -1)
	{	
		p = strstr(buffer, "Average RSSI");	
		if(p){
			continue;
			}
		p = strstr(buffer, "Rx with CRC");
		if(p)
		{	
			p = strstr(buffer, "PER=");
				if(p)
				{
					p += 4;
					sscanf(p, "%[^%]", BER);
				}
		}
		r = strstr(buffer, "RSSI");
		if(r)
		{	
			r = strstr(buffer, "=");
			if(r)
			{
				r += 1;
				strcat(RSSI, r);

				/* skip \n twice */
				if (strlen(RSSI) - 1 > 0 && RSSI[strlen(RSSI) - 1] == '\n')
					RSSI[strlen(RSSI) - 1] = '\0';

				if (strlen(RSSI) - 1 > 0 && RSSI[strlen(RSSI) - 1] == '\n')
					RSSI[strlen(RSSI) - 1] = '\0';
			}
		}
	}
	if(p)
		free(p);
	if(r)
		free(r);
	if(buffer)
		free(buffer);
	if(fp)
		fclose(fp);

	return;
}
int find_stanum(int i)
{	
	char buf[64] = {0};
	char nodeName[64] = {0};
	int j = 0;
	int stanum = 0;
	
	for (j = 0; j < 32; j ++)
	{
		snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, i, j + 1);
		memset(buf, 0, sizeof(buf));
		if(0 < cfg_get_object_attr(nodeName, "MAC", buf, sizeof(buf)))
		{
			stanum++;
		}
	}

	return stanum;
}

void find_rate(int i,char* rate)
{
	char buf[64] = {0};
	char rateBuf[64] = {0};
	char nodeName[64] = {0};
	snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, i, 1);
	cfg_get_object_attr(nodeName,"DataRate",buf,sizeof(buf));
	snprintf(rateBuf,sizeof(rateBuf),"%s000",buf);
	strcpy(rate,rateBuf);

	return;
}
void avg_rssi(char* rssi,char *avgRSSI,int i )
{
	char rssi1[16] = {0};
	char rssi2[16] = {0};
	int avg = -127;

	if (i >= 1 && i <= WLAN_2_4_G_NODE_LEN)
	{	
		avg = -65;
	}		
	else if (i > WLAN_2_4_G_NODE_LEN && i <= WLAN_MAPPING_NODE_LEN)
	{	
		avg = -75;
	}
	
	sscanf(rssi, "%s %s",rssi1,rssi2);
	
	if(atoi(rssi1)!=0 && atoi(rssi1)!=0)
	avg = (atoi(rssi1) + atoi(rssi2))/2;
	
	snprintf(avgRSSI,32,"%d",avg);
	return ;
}

void addwlanentryinfo(int i, char *info_data)
{
	char data[384] = {0};
	char buf[64] = {0};
	char nodeName[64] = {0};
	char SSID[64] = {0};
	char SSIDIndex[6] = {0};
	char SSIDAlias[16] = {0};
	char EnableSSID[4] = {0};
	char Rate[16] = {0};
	char HT_BW[4] = {0};
	char HT_BSSCoexistence[4] = {0};
	char Bandwidth[32] = {0};
	char wlanChannel[6] = {0};
	char BER[12] = {0};
	char RSSI[32] = {0};
	int stanum = 0;
	int wifi_type = 0;
	char RFband[6] = {0};
	char standard_value[64] = {0};
	char beacon_type_value[64] = {0};
	int j = 0;
	char wlanmapping_nodename[64] = {0};
	int i_entryidx = 0;
	char entryidx[4] = {0};
	char VHT_BW[4] = {0};
	char avgRSSI[32] = {0};
	char APOn[4] = {0}; 
	char CommonNodeName[64] = {0};
	char standard_type[16] = {0}; 

	Wireless_CODE_TO_NAME standard_WirelessMode2str[]=
	{
		{"0", "b,g", 		""	            	},//b,g mode
		{"1", "b", 			""               	},
		{"4", "g",			""               	},
		{"6", "n", 			""               	},
		{"7", "g,n", 		""	           		},//g,n node
		{"9", "b,g,n", 		""          		},
		{"2", "a" , 		""          		},//11a only
		{"8", "a,n", 		""   				},//11a/n mixed mode
		{"11", "n", 		""   				},
		{"14", "ac", 		""      			},//11vht AC/AN/A
		{"15", "an,ac", 	"ac"    	    	},//11vht AC/AN
#if defined(TCSUPPORT_WLAN_AX)
		{"16", "g,n,ax", 	"n,ax" 			  	},//ax
		{"17", "n,ac,ax", 	"ac,ax"	   			},//ax
#endif
	};
	Authmode_TO_Beacontype standard_Authmode2Bracontype[]=
	{
		{"OPEN",			"None"	   },
		{"WEP-64Bits",		"Basic"    },
		{"WEP-128Bits",		"Basic"    },
		{"WPAPSK",			"WPA"      },
		{"WPA2PSK",			"WPA2"     },
		{"WPAPSKWPA2PSK",	"WPA/WPA2" },
	};
	int max_wireless_mode = sizeof(standard_WirelessMode2str)/sizeof(Wireless_CODE_TO_NAME);
	int max_wireless_authmode = sizeof(standard_Authmode2Bracontype)/sizeof(Authmode_TO_Beacontype);
	
	for (j = 0; j < WLAN_MAPPING_NODE_LEN; j ++)
	{
		snprintf(wlanmapping_nodename, sizeof(wlanmapping_nodename), WLANMAPPING_ENTRY_N_NODE, j + 1);
		cfg_get_object_attr(wlanmapping_nodename, "entryidx", entryidx, sizeof(entryidx));
		i_entryidx = atoi(entryidx);
		if (i_entryidx == i - 1)
		{	
			snprintf(SSIDIndex, sizeof(SSIDIndex), "%d", j+1);
			if (i >= 1 && i <= WLAN_2_4_G_NODE_LEN)
			{
				snprintf(CommonNodeName,sizeof(CommonNodeName),WLAN_COMMON_NODE);
				snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, i);
				cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BW", HT_BW, sizeof(HT_BW));
				cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BSSCoexistence", HT_BSSCoexistence, sizeof(HT_BSSCoexistence));
				cfg_get_object_attr(INFO_WLAN_NODE, "CurrentChannel", wlanChannel, sizeof(wlanChannel));
				strcpy(RFband, "2.4G");
				cfg_get_object_attr(WLAN_COMMON_NODE, "WirelessMode", standard_value, sizeof(standard_value));

				if (0 == strcmp(HT_BW, "0"))
					strcpy(Bandwidth, "20MHz");
				else if (0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoexistence, "1"))
					strcpy(Bandwidth, "20/40MHz");
				else if (0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoexistence, "0"))
					strcpy(Bandwidth, "40MHz");
				else
					strcpy(Bandwidth, "20MHz");
				snprintf(SSIDAlias, sizeof(SSIDAlias), "2.4G-%d", i);
			}
			else if (i > WLAN_2_4_G_NODE_LEN && i <= WLAN_MAPPING_NODE_LEN)
			{	
				snprintf(CommonNodeName,sizeof(CommonNodeName),WLAN11AC_COMMON_NODE);
				snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, i - WLAN_2_4_G_NODE_LEN);
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_BW", HT_BW, sizeof(HT_BW));
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_BSSCoexistence", HT_BSSCoexistence, sizeof(HT_BSSCoexistence));
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "VHT_BW", VHT_BW, sizeof(VHT_BW));
				cfg_get_object_attr(INFO_WLAN11AC_NODE, "CurrentChannel", wlanChannel, sizeof(wlanChannel));
				strcpy(RFband, "5G");
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "WirelessMode", standard_value, sizeof(standard_value));
		
				if (0 == strcmp(HT_BW, "0"))
					strcpy(Bandwidth, "20MHz");
				else if (0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoexistence, "1") && 0 == strcmp(VHT_BW, "0"))
					strcpy(Bandwidth, "40MHz");
				else if (0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoexistence, "0") && 0 == strcmp(VHT_BW, "0"))
					strcpy(Bandwidth, "40MHz");
				else if (0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoexistence, "1") && 0 == strcmp(VHT_BW, "1"))
					strcpy(Bandwidth, "80MHz");
				else if (0 == strcmp(VHT_BW, "1"))
					strcpy(Bandwidth, "80MHz");
				else
					strcpy(Bandwidth, "80MHz");
				snprintf(SSIDAlias, sizeof(SSIDAlias), "5G-%d", i - WLAN_2_4_G_NODE_LEN);
			}
			break;
		}
	}
	
	for(j = 0; j < max_wireless_mode; j++){	
		if(strcmp(standard_WirelessMode2str[j].pCode,standard_value)==0){
			memset(standard_value, 0, sizeof(standard_value));
			if(!strcmp(standard_WirelessMode2str[j].pCode, "15"))
			{
				memset(standard_type, 0, sizeof(standard_type));
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "isACOnly", standard_type, sizeof(standard_type));
				if(strlen(standard_type) > 0 && !strcmp(standard_type, "1"))
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName1);
				else
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName);
			}
#if defined(TCSUPPORT_WLAN_AX)
			else if(!strcmp(standard_WirelessMode2str[j].pCode, "16"))
			{
				memset(standard_type, 0, sizeof(standard_type));
				cfg_get_object_attr(WLAN_COMMON_NODE, "is80211nax", standard_type, sizeof(standard_type));
				if(strlen(standard_type) > 0 && !strcmp(standard_type, "1"))
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName1);
				else
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName);
			}
			else if(!strcmp(standard_WirelessMode2str[j].pCode, "17"))
			{
				memset(standard_type, 0, sizeof(standard_type));
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "is80211acax", standard_type, sizeof(standard_type));
				if(strlen(standard_type) > 0 && !strcmp(standard_type, "1"))
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName1);
				else
					snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName);
			}
#endif
			else
			snprintf(standard_value, sizeof(standard_value), standard_WirelessMode2str[j].pCodeName);
				
			break;		
		}	
	}
	

	
	memset(buf, 0, sizeof(buf));
	if (0 >= cfg_get_object_attr(nodeName, "SSID", buf, sizeof(buf)))
		return;
	
	strcpy(SSID, buf);
	cfg_get_object_attr(CommonNodeName, "APOn", APOn, sizeof(APOn));
	if(1 == atoi(APOn))		
		cfg_get_object_attr(nodeName, "EnableSSID", EnableSSID, sizeof(EnableSSID));
	else
		snprintf(EnableSSID,sizeof(EnableSSID),"0");
			
	memset(beacon_type_value, 0, sizeof(beacon_type_value));
	cfg_get_object_attr(nodeName, "AuthMode", beacon_type_value, sizeof(beacon_type_value));

	for(j = 0; j < max_wireless_authmode; j++){	
		if(strcmp(standard_Authmode2Bracontype[j].pAuthmode,beacon_type_value)==0){
			memset(beacon_type_value, 0, sizeof(standard_value));
			snprintf(beacon_type_value, sizeof(beacon_type_value), standard_Authmode2Bracontype[j].pBeacontype);
			break;
		}	
	}

	memset(BER, 0, sizeof(BER));
	memset(RSSI, 0, sizeof(RSSI));
	find_wlan(i, BER, RSSI);
	
	if( 1 == atoi(APOn) && 1 == atoi(EnableSSID))
		avg_rssi(RSSI,avgRSSI,i);
	else
		snprintf(avgRSSI,sizeof(avgRSSI),"-100");
	
	stanum = find_stanum(i);
	if(stanum == 0)
		strcpy(Rate,"0");
	else
		find_rate(i,Rate);
	snprintf(data, sizeof(data), 
			"{\n"
			"\"SSID\":\"%s\",\n"
			"\"SSIDIndex\":\"%s\",\n"
			"\"SSIDAlias\":\"%s\",\n"
			"\"Enable\":\"%s\",\n"
			"\"Band\":\"%s\",\n"
			"\"Rate\":\"%s\",\n"
			"\"Bandwidth\":\"%s\",\n"
			"\"Mode\":\"%s\",\n"
			"\"Security\":\"%s\",\n"
			"\"Channel\":\"%s\",\n"
			"\"ClientCount\":%d,\n"
			"\"RSSI\":\"%s\",\n"
			"\"BER\":\"%s\"\n"	
			"},\n", SSID, SSIDIndex, SSIDAlias, EnableSSID, RFband, Rate, Bandwidth, 
			standard_value, beacon_type_value, wlanChannel, stanum, avgRSSI, BER);
	strcat(info_data, data);
	
	return;
}
int svc_wlan_collect_info_handle(void)
{
	FILE *fp = NULL;
	char wifi24g_apon[4] = {0};
	int i_24gwifi_apon = 0;
	char wifi5g_apon[4] = {0};
	int i_5gwifi_apon = 0;
	char buf[512] = {0};
	char filename[64] = "/tmp/diaginfo/";
	char filename2[64] = {0};
	char tmpbuff[64] = {0};
	time_t tmpcal_ptr;
	char now_time[64] = {0};
	int wifi_enable = 0;
	char nodeName[64] = {0};
	int flag24g = 0;
	int flag5g = 0;
	int dualband = 0;
	char EnableSSID[4] = {0};
	char wifi24g_on[4] = {0};
	int i_24gwifi_on = 0;	
	char wifi5g_on[4] = {0};
	int i_5gwifi_on = 0;
	
#if defined(TCSUPPORT_WLAN_AC)
	dualband = 1;
#endif
	char cmd[128] = {0};
	char cmdVal[64] = {0};
	int ret = 0;
	char wlan_ac_collect_sleep_time[16] = {0};
	int sleep_time_5g = 15;

	memset(wlan_ac_collect_sleep_time, 0, sizeof(wlan_ac_collect_sleep_time));
	if (cfg_get_object_attr(WLAN11AC_COMMON_NODE, "collect_sleep_tm", wlan_ac_collect_sleep_time, sizeof(wlan_ac_collect_sleep_time)) > 0)
	{
		sleep_time_5g = atoi(wlan_ac_collect_sleep_time);
	}

	cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", wifi24g_apon, sizeof(wifi24g_apon));
	i_24gwifi_apon = atoi(wifi24g_apon);
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", wifi5g_apon, sizeof(wifi5g_apon));
	i_5gwifi_apon = atoi(wifi5g_apon);


	if (cfg_get_object_attr(WLAN_ENTRY1_NODE, "EnableSSID", wifi24g_on, sizeof(wifi24g_on)) > 0)
	{
		i_24gwifi_on = atoi(wifi24g_on);
	}

	if (cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "EnableSSID", wifi5g_on, sizeof(wifi5g_on)) > 0)
	{
		i_5gwifi_on = atoi(wifi5g_on);
	}

	if (i_24gwifi_apon == 1&& i_24gwifi_on == 1)
	{
	wifimgr_lib_set_WIFI_CMD("ra0", -1, sizeof(cmd), "PartialScan=1", cmd, 0);
	system(cmd);
	}
	if (i_5gwifi_apon == 1 && i_5gwifi_on == 1)
	{
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai0", -1, sizeof(cmd), "PartialScan=1", cmd, 0);
	system(cmd);
	}	
	memset(cmd, 0, sizeof(cmd));
	sleep(sleep_time_5g);
	
	if (i_24gwifi_apon == 1 && i_24gwifi_on == 1)
	{
	memset(cmd, 0, sizeof(cmd));
	memset(cmdVal, 0, sizeof(cmdVal));
	wifimgr_lib_set_WIFI_CMD("ra0", -1, sizeof(cmdVal), "get_site_survey", cmdVal, 1);
	snprintf(cmd, sizeof(cmd), "%s > %s", cmdVal, WLAN24G_SITESURVEY_CMD);
	system(cmd);
	}
	if (i_5gwifi_apon == 1 && i_5gwifi_on == 1)
	{
	memset(cmdVal, 0, sizeof(cmdVal));
	wifimgr_lib_set_WIFI_CMD("rai0", -1, sizeof(cmdVal), "get_site_survey", cmdVal, 1);
	snprintf(cmd, sizeof(cmd), "%s > %s", cmdVal, WLAN5G_SITESURVEY_CMD);
	system(cmd);
	}	

	int i = 0;	
	
	fp = fopen("/tmp/diaginfo/wifi_info", "w");
	if ( NULL == fp )
	{
		return 1;
	}
	
	char data[128] = 
	"{\n"
	"\"Time\":\"%s\",\n"
	"\"Enable\":%d,\n"
	"\"24GWiFiEnable\":%d,\n"
		"\"5GWiFiEnable\":%d,\n"
"\"dualband\":%d,\n"
	"\"Info\":["
	;
	/*get real time */
	struct timespec curtime;
	struct tm nowTime;
	clock_gettime(CLOCK_REALTIME, &curtime);
	localtime_r(&curtime.tv_sec, &nowTime);

	if(i_24gwifi_apon == 1 || i_5gwifi_apon == 1)
		wifi_enable = 1;
	else
		wifi_enable = 0;
	
	snprintf(now_time, sizeof(now_time), "%04d-%02d-%02d %02d:%02d:%02d", (nowTime.tm_year + 1900), 
		(nowTime.tm_mon + 1), nowTime.tm_mday, nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	snprintf(buf, sizeof(data), data, now_time, wifi_enable, i_24gwifi_apon, i_5gwifi_apon, dualband);
	
	if(access(filename, 0))
	{
		snprintf(filename2, sizeof(filename2), "mkdir %s", filename);
		system(filename2);
	}
	for(i = 0; i < 8; i++)
	{	
		snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, i + 1);
		memset(tmpbuff, 0, sizeof(tmpbuff));
		if(0 >= cfg_get_object_attr(nodeName, "SSID", tmpbuff, sizeof(tmpbuff)))
			continue;
		if(strlen(buf) > 0){
			fputs(buf, fp); 
			memset(buf, 0, sizeof(buf));
		}
		addwlanentryinfo(i + 1, buf);
	}
	for(i = 0; i < 8; i++)
	{	
		snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, i + 1);
		memset(tmpbuff, 0, sizeof(tmpbuff));
		if(0 >= cfg_get_object_attr(nodeName, "SSID", tmpbuff, sizeof(tmpbuff)))
			continue;
		if(strlen(buf) > 0){
			fputs(buf, fp); 
			memset(buf, 0, sizeof(buf));
		}
		addwlanentryinfo(i + 8 + 1, buf);
	}

	buf[strlen(buf) - 2] = '\0';
	
	char buf2[32] =
	"]\n"
			",\"Interference_2G\":[\n";
	strcat(buf, buf2);
	
	fputs(buf, fp); 
	memset(buf, 0, sizeof(buf));
	
	flag24g = add_wlan24g_Interferenceinfo(buf, fp);
	if ( 1 == flag24g)
	{
		buf[strlen(buf) - 2] = '\0';
	}	
	strcat(buf, "]\n");
	char buf3[32] =",\"Interference_5G\":[\n";
	strcat(buf, buf3);
	flag5g = add_wlan5g_Interferenceinfo(buf, fp);
	if ( 1 == flag5g)
	{
		buf[strlen(buf) - 2] = '\0';
	}
	strcat(buf, "]}\n");
	fputs(buf, fp);
	if(fp)
		fclose(fp);

	char rdata[64] = "collect wifi info success";
	ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_COLLECTWIFIINFOSTATUS, rdata, strlen(rdata) + 1);
	
	return 0;
}

#ifdef TCSUPPORT_WLAN_VENDIE
int svc_wlan_probevsie_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_probevsie_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_probevsie_update_object(path) < 0){
		printf("svc_wlan_probevsie_handle_event_update: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}
	
	return CFG2_RET_SUCCESS;
}

int svc_wlan_proberesp_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_probevsie_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_proberesp_update_object(path) < 0){
		printf("svc_wlan_probevsie_handle_event_update: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}
	
	return CFG2_RET_SUCCESS;
}

int svc_wlan_proberesp_handle_event_delete(pt_wlan_evt param)
{
	char* path = param->buf;
	if (svc_wlan_proberesp_delete_object(path) < 0){
		tcdbg_printf("svc_wlan_proberesp_handle_event_delete: [%s] delete fail \n",path);
		return CFG2_RET_FAIL;
	}
	return CFG2_RET_SUCCESS;

}

#endif

#ifdef TCSUPPORT_WLAN_ACS
int svc_wlan_acs_handle_event_update(pt_wlan_evt param)
{
	tcdbg_printf("enter svc_wlan_acs_handle_event_update\n");
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_acs_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_acs_action(path) < 0){
		printf("svc_wlan_acs_handle_event_update: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}
	
	return CFG2_RET_SUCCESS;
}
#endif
/* 
	type = 0, 2.4G
	type = 1, 5G
*/
	
int svc_wlan_roam_handle_event_update(int type)
{
	char nodeName[32] = {0};
	char enable[8] = {0};
	char cmd[128] = {0};
        int ch;
#if defined(TCSUPPORT_WLAN_DOT11K_RRM)||defined(TCSUPPORT_WLAN_DOT11V_WNM)
	kv_daemon_check();
	if(type == 0)
	{
		cfg_obj_get_object_attr(WLAN_ROAM_NODE,"Enable",0,enable,sizeof(enable));
		if(strcmp(enable,"1") == 0)
		{
			system("/userfs/bin/kvctrl ra0 rrm 1");
		}
		else
		{
			system("/userfs/bin/kvctrl ra0 rrm 0");
		}
	}
	else if(type == 1)
	{
		cfg_obj_get_object_attr(WLAN_ROAM11AC_NODE,"Enable",0,enable,sizeof(enable));
		if(strcmp(enable,"1") == 0)
		{
			system("/userfs/bin/kvctrl rai0 rrm 1");
		}
		else
		{
			system("/userfs/bin/kvctrl rai0 rrm 0");
		}
	}
#endif	
	return CFG2_RET_SUCCESS;	
}


#if defined(TCSUPPORT_CT_JOYME2)
int svc_wlan_ratepriority_handle_event_update(int type)
{
	char nodeName[32] = {0};
	char active[8] = {0};
	char buf[64] = {0};
	char cmd[128] = {0};
	char cmd_value[64] = {0};
	if ( type == 0 )
	{
		cfg_obj_get_object_attr(WLAN_RATEPRIORITY_NODE, "Active", 0, active, sizeof(active));
		if ( strcmp(active, "1") == 0 ){
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "HT_BW=1", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "HT_BSSCoexistence=1", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "WirelessMode=6", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "LowRateCtrl=1", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			cfg_obj_get_object_attr(WLAN_ENTRY1_NODE,"SSID",0,buf,sizeof(buf));
			snprintf(cmd_value, sizeof(cmd_value), "SSID=%s", buf);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
		}
		else {
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "HT_BW=0", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "WirelessMode=9", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "LowRateCtrl=0", cmd, 0);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			cfg_obj_get_object_attr(WLAN_ENTRY1_NODE,"SSID",0,buf,sizeof(buf));
			snprintf(cmd_value, sizeof(cmd_value), "SSID=%s", buf);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
		}	
	}
	else if ( type == 1 )
	{
		cfg_obj_get_object_attr(WLAN11AC_RATEPRIORITY_NODE, "Active", 0, active, sizeof(active));
		if ( strcmp(active, "1") == 0 )
		{
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "LowRateCtrl=1", cmd, 0);
			system(cmd);
		}
		else
		{
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "LowRateCtrl=0", cmd, 0);
			system(cmd);
		}
	}
	else
		return CFG2_RET_FAIL;
	
	return CFG2_RET_SUCCESS;
}
#endif
int set_BlackWhitePolicyInfo(int maxNum, int mode, char *poliy)
{
	int i = 0, j = 0, len = 0;
	char cmd[1400] = {0};
	char MacAddr[20] = {0};
	char MacList[1300] = {0}, strResult[1300] = {0}; /* append for lanhost2 */
	char nodeName[32] = {0}, nodehead[32] = {0};
	char wname[8] = {0};
	char AccessPolicy[4] = {0}, MacAccessMode[4] = {0};
	char wlan_entry_name[32] = {0}, tmpbuf[20] = {0}, iface_name[32] = {0};
	int excute_black = 0;
	char cmd_value[64] = {0};

	
	if (WLAN_MODE == mode) {
		snprintf(wlan_entry_name, sizeof(wlan_entry_name), WLAN_ENTRY_NODE);
		snprintf(iface_name, sizeof(iface_name), "ra");
	}
	else if (WLAN11AC_MODE == mode) {
		snprintf(wlan_entry_name, sizeof(wlan_entry_name), WLAN11AC_ENTRY_NODE);
		snprintf(iface_name, sizeof(iface_name), "rai");
	}
	else {
		return -1;
	}

	if(!strcmp(poliy, "1"))
	{
		strcpy(AccessPolicy, "1");
		snprintf(nodehead, sizeof(nodehead), "%s", MACWHITELIST_NODE);
	}
	else
	{
		strcpy(AccessPolicy, "2");
		snprintf(nodehead, sizeof(nodehead), "%s", MACBLACKLIST_NODE);
	}
	
	/* loop for each wifi interface */
	for(i = 0; i < maxNum; i++) {
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.%d", wlan_entry_name, i + 1);
		snprintf(wname, sizeof(wname), "%s%d", iface_name, i);
		
#if 0
		/* get wifi interface AccessPolicy */
		if (0 > cfg_obj_get_object_attr(MACWHITELIST_COMMON_NODE, "MACPolicy", 0, AccessPolicy, sizeof(AccessPolicy) )
			|| '\0' == AccessPolicy[0])
			continue;
		
		/* if macfilter policy is not equal to wifi policy, do nothing for this ssid */
		if (strcmp(AccessPolicy, poliy))
			continue;
#endif	
		wifimgr_lib_set_WIFI_CMD(tmpbuf, i, sizeof(cmd), "ACLClearAll=1", cmd, 0);
		system(cmd);

		bzero(MacList, sizeof(MacList));
		len = 0;

#if !defined(TCSUPPORT_CT_UBUS)
		memset(MacAccessMode, 0, sizeof(MacAccessMode));
		if( cfg_obj_get_object_attr(nodeName, "AccessPolicy", 0, MacAccessMode, sizeof(MacAccessMode)) > 0 && !strcmp(poliy, MacAccessMode) )
#endif
		{
			/* get mac list from macfilterlsit */
			for (j = 0; j < WLAN_MAC_NUM; j++) {
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "%s.%d", nodehead, j + 1);
				if (0 > cfg_obj_get_object_attr(nodeName, "MAC", 0, MacAddr, sizeof(MacAddr)) || '\0' == MacAddr[0]
					|| !strcmp(MacAddr, "00:00:00:00:00:00"))
					continue;
				
				len += snprintf(MacList + len, sizeof(MacList) - len, "%s;", MacAddr);
			}
		}

		/* update MacList according to LANHost2Black Node */
		for ( j = 0; j < MAX_LANHOST2_ENTRY_NUM; j++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), LANHOST2BLACK_ENTRY_NODE, j + 1);
			memset(MacAddr, 0, sizeof(MacAddr));
			memset(tmpbuf, 0, sizeof(tmpbuf));
			/* no device mac, find next */
			if ( cfg_obj_get_object_attr(nodeName, "MAC", 0, tmpbuf, sizeof(tmpbuf)) <= 0 )
				continue;

			mac_add_dot(tmpbuf, MacAddr);
			strcat(MacAddr, ";");

			/* delete mac address in below cases
			** macfilter policy is white(policy = 1), and this mac is on blacklist of lanhost2
			*/
			if ( 0 == strcmp(AccessPolicy, "1") &&  0 != MacList[0] )
			{
				/* mac is on mac filter white list, delete it */
				if ( NULL != strcasestr(MacList, MacAddr) )
				{
					memset(strResult, 0, sizeof(strResult));
					if ( 0 == cutString(MacList, sizeof(MacList), strResult, sizeof(strResult), MacAddr, 0) )
					{
						if ( 0 != strResult[0] )
							strncpy(MacList, strResult, sizeof(MacList) - 1);
						else
						{
							/* all mac in  macwhitelist is pulled to blacklist by lanhost2 */
							excute_black = 1;
							break;
						}
					}
				}
			}
			/* add mac address in below cases
			** macfilter policy is not white, and this mac is on blacklist of lanhost2
			*/
			else /*if ( 0 == strcmp(AccessPolicy, "2") 
				|| ( 0 == strcmp(AccessPolicy, "1") &&  0 == MacList[0] ) )*/
			{
				strcpy(AccessPolicy, "2");
				/* mac is not on mac filter black list, add it */
				if ( NULL == strcasestr(MacList, MacAddr) )
				{
					memset(strResult, 0, sizeof(strResult));
					if ( 0 == addString(MacList, sizeof(MacList), strResult, sizeof(strResult), MacAddr, 0) )
						strncpy(MacList, strResult, sizeof(MacList) - 1);
				}
			}
		}

		/* all mac in  macwhitelist is pulled to blacklist by lanhost2, excute black policy for mac in lanhost2black */
		if ( excute_black )
		{
			memset(MacList, 0, sizeof(MacList));
			len = 0;
			strcpy(AccessPolicy, "2");
			for ( j = 0; j < MAX_LANHOST2_ENTRY_NUM; j++ )
			{
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), LANHOST2BLACK_ENTRY_NODE, j + 1);
				memset(MacAddr, 0, sizeof(MacAddr));
				memset(tmpbuf, 0, sizeof(tmpbuf));
				/* no device mac, find next */
				if ( cfg_obj_get_object_attr(nodeName, "MAC", 0, tmpbuf, sizeof(tmpbuf)) <= 0 )
					continue;
				
				mac_add_dot(tmpbuf, MacAddr);
				len += snprintf(MacList + len, sizeof(MacList) - len, "%s;", MacAddr);
			}
		}
		
		if ( 0 != MacList[0] ) /* when mac list is not empty. */
		{
			MacList[strlen(MacList) - 1] = '\0';

			bzero(cmd, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "AccessPolicy=%s", AccessPolicy);
			wifimgr_lib_set_WIFI_CMD(tmpbuf, i, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "ACLAddEntry=\"%s\"", MacList);
			wifimgr_lib_set_WIFI_CMD(tmpbuf, i, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
		}
		else
		{
			bzero(cmd, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "AccessPolicy=%s", "0");
			wifimgr_lib_set_WIFI_CMD(tmpbuf, i, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
		}
	}
	return 0;
}

int cfg_type_maclist_update(int policy)
{
	char nodeName[32] = {0};
	int i = 0, j = 0, maxNum = 0;
	char BSSIDNum[4] = {0};
	char AccessPolicy[4] = {0};
	char cmd[64] = {0};
	char wname[8] = {0};
	char MacAddr[20] = {0};
	char MacList[150] = {0};
	int len = 0;
	char tmp[4] = {0};

	snprintf(tmp, sizeof(tmp), "%d", policy);
	/* 2.4G */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
	cfg_obj_get_object_attr(nodeName, "BssidNum", 0, BSSIDNum, sizeof(BSSIDNum));
	maxNum = atoi(BSSIDNum);
	set_BlackWhitePolicyInfo(maxNum, WLAN_MODE, tmp);
	
#if defined(TCSUPPORT_WLAN_AC)
	/* 5G */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
	cfg_obj_get_object_attr(nodeName, "BssidNum", 0, BSSIDNum, sizeof(BSSIDNum));
	maxNum = atoi(BSSIDNum);
	set_BlackWhitePolicyInfo(maxNum, WLAN11AC_MODE, tmp);
#endif	

	return  0;
}

int svc_maclist_excute(int policy)
{
	cfg_type_maclist_update(policy);
	return 0;
}

#if defined(TCSUPPORT_WLAN_APCLIENT)
#define APCLI_EXEC_SH	"/tmp/apcli_exec.sh"

int syncWiFiConfig(int type)
{
	char tmpbuf[64] = {0};
	char path[64] = {0}, apPath[64] = {0};
	char defKeyID[4] = {0}, AuthMode[16] = {0}, EncrypType[16] = {0}, Passwd[20] = {0};
	char KeyBit[8] = {0}, attrname[32] = {0}, SSID[64] = {0};
	
	if( type )
	{
		snprintf(path, sizeof(path), WLAN11AC_ENTRY1_NODE);
		snprintf(apPath, sizeof(path), APCLI_ENTRY1_NODE);
	}
	else
	{
		snprintf(path, sizeof(path), WLAN_ENTRY1_NODE);
		snprintf(apPath, sizeof(path), APCLI_ENTRY0_NODE);
	}

	cfg_obj_get_object_attr(apPath, "SSID", 0, SSID, sizeof(SSID));
	cfg_obj_set_object_attr(path, "SSID", 0, SSID);
	
	cfg_obj_get_object_attr(apPath, "AuthMode", 0, AuthMode, sizeof(AuthMode));
	if( !strcmp(AuthMode, "OPEN") )
	{
		cfg_obj_set_object_attr(path, "AuthMode", 0, AuthMode);
	}
	else if( strstr(AuthMode, "WEP") )
	{
		cfg_obj_get_object_attr(apPath, "KeyBit", 0, KeyBit, sizeof(KeyBit));
		snprintf(tmpbuf, sizeof(tmpbuf), "WEP-%sBits", KeyBit);
		cfg_obj_set_object_attr(path, "AuthMode", 0, tmpbuf);
		
		if( strstr(AuthMode, "OPEN") )
			cfg_obj_set_object_attr(path, "WEPAuthType", 0, "OpenSystem");
		else
			cfg_obj_set_object_attr(path, "WEPAuthType", 0, "SharedKey");
		
		cfg_obj_get_object_attr(apPath, "DefaultKeyID", 0, defKeyID, sizeof(defKeyID)) ;
		cfg_obj_set_object_attr(path, "DefaultKeyID", 0, defKeyID);
		
		cfg_obj_get_object_attr(apPath, "Key", 0, Passwd, sizeof(Passwd));
		snprintf(attrname, sizeof(attrname), "Key%dStr", atoi(defKeyID));
		cfg_obj_set_object_attr(path, attrname, 0, Passwd);
			
	}
	else if( !strcmp(AuthMode, "WPAPSK") || !strcmp(AuthMode, "WPA2PSK") || !strcmp(AuthMode, "WPAPSKWPA2PSK"))
	{
		cfg_obj_set_object_attr(path, "AuthMode", 0, AuthMode);
		
		cfg_obj_get_object_attr(apPath, "EncrypType", 0, EncrypType, sizeof(EncrypType));
		cfg_obj_set_object_attr(path, "EncrypType", 0, EncrypType);
		
		cfg_obj_get_object_attr(apPath, "WPAPSK", 0, Passwd, sizeof(Passwd));
		cfg_obj_set_object_attr(path, "WPAPSK", 0, Passwd);
	}

	return 0;
}

int svc_wlan_wps_connect_event()
{
	char cmd[256] = {0}, attrname[10] = {0}, SSID[32] = {0}, param[8] = {0};
	char ifName[16] = {0}, tmpBuf[10] = {0}, currRadio[4] = {0};
	
	memset(currRadio, 0, sizeof(currRadio));
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currRadio, sizeof(currRadio));
	if( 0 != currRadio[0] && 0 == strcmp(currRadio, "1") )
	{
		
		if(cfg_obj_get_object_attr(APCLI_ENTRY1_NODE, "MACRepeaterEn", 0, tmpBuf, sizeof(tmpBuf)) > 0 && !strcmp(tmpBuf, "1"))
		{
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "MACRepeaterEn=1", cmd, 0);
			system(cmd);
		}
			
		snprintf(ifName, sizeof(ifName), "apclii0");
	}
	else
	{
		if(cfg_obj_get_object_attr(APCLI_ENTRY0_NODE, "MACRepeaterEn", 0, tmpBuf, sizeof(tmpBuf)) > 0 && !strcmp(tmpBuf, "1"))
		{
			memset(cmd, 0, sizeof(cmd));
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "MACRepeaterEn=1", cmd, 0);
			system(cmd);
		}
		
		snprintf(ifName, sizeof(ifName), "apcli0");
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/script/apclient_wps.sh %s", ifName);
	system(cmd);
	
	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "StartConn", 0, "0");
	return 0;
}
int svc_wlan_apcli_connect_event()
{
	char cmd[256] = {0}, attrname[10] = {0}, SSID[32] = {0},  path[64] = {0}, cmd_value[64] = {0};
	char EnableValue[4] = {0}, AuthMode[16] = {0}, EncrypType[16] = {0}, Passwd[20] = {0};
	char defKeyID[4] = {0}, BssidNum[4] = {0}, ifName[16] = {0}, tmpBuf[10] = {0}, currRadio[4] = {0};
	char channel[8] = {0}, RSSI[20] = {0}, param[8] = {0}, needScan[8] = {0}, Signal[20];
	char ssid_ifname[16] = {0};
	int res = 0, type = 0, need_commit = 0, op_type = 0;
	FILE *fp = NULL;

	memset(currRadio, 0, sizeof(currRadio));
	memset(ssid_ifname, 0, sizeof(ssid_ifname));
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currRadio, sizeof(currRadio));
	if( 0 != currRadio[0] && 0 == strcmp(currRadio, "1") )
	{
		type = 1;
		snprintf(path, sizeof(path), APCLI_ENTRY1_NODE);
		snprintf(ifName, sizeof(ifName), "apclii0");
		snprintf(ssid_ifname, sizeof(ssid_ifname), "rai0");
	}
	else
	{
		type = 0;
		strncpy(currRadio, "0", sizeof(currRadio) - 1);
		snprintf(path, sizeof(path), APCLI_ENTRY0_NODE);
		snprintf(ifName, sizeof(ifName), "apcli0");
		snprintf(ssid_ifname, sizeof(ssid_ifname), "ra0");
	}
	
	if( cfg_obj_get_object_attr(path, "Enable", 0, EnableValue, sizeof(EnableValue)) > 0 && !strcmp(EnableValue, "1"))
	{			
		cfg_obj_get_object_attr(path, "SSID", 0, SSID, sizeof(SSID));
		cfg_obj_get_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, needScan, sizeof(needScan));
		if( 0 == atoi(needScan) )
		{
			res = np_wlan_apcli_get_sitesurvey(type, SSID, channel, RSSI, Signal);
			if( 0 > res )
			{
				/* return when AP is fixed mode, otherwise change radio to research ssid */
				memset(tmpBuf, 0, sizeof(tmpBuf));
				cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", 0, tmpBuf, sizeof(tmpBuf));
				if ( 0 != tmpBuf[0] )
					return -1;
				
				if( 0 == type )
				{
					op_type = 1;
					snprintf(path, sizeof(path), APCLI_ENTRY1_NODE);
					snprintf(ifName, sizeof(ifName), "apclii0");
				}
				else
				{
					op_type = 0;
					snprintf(path, sizeof(path), APCLI_ENTRY0_NODE);
					snprintf(ifName, sizeof(ifName), "apcli0");
				}
				cfg_obj_get_object_attr(path, "SSID", 0, SSID, sizeof(SSID));
				snprintf(currRadio, sizeof(currRadio), "%d", op_type);
				res = np_wlan_apcli_get_sitesurvey(op_type, SSID, channel, RSSI, Signal);
				
				if( 0 > res )
					return -1;

				type = op_type;
			}
			else
			{
				if ( 0 == channel[0] )
					return -1;
			}
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currRadio);
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "1");
			cfg_obj_set_object_attr(path, "RSSI", 0, RSSI);
		}
		if( channel[0] == '\0' )
				return -1;
		
		if( cfg_obj_get_object_attr(path, "needSync", 0, tmpBuf, sizeof(tmpBuf)) > 0 && !strcmp(tmpBuf, "1"))
		{
			syncWiFiConfig(type);
			need_commit = 1;
		}
		
		if(type)
		{				
			cfg_obj_set_object_attr(WLAN11AC_COMMON_NODE, "Channel", 0, channel);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "Channel=%s", channel);
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);

			if( need_commit )
				cfg_commit_object(WLAN11AC_ENTRY1_NODE);
		}
		else
		{
			cfg_obj_set_object_attr(WLAN_COMMON_NODE, "Channel", 0, channel);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "Channel=%s", channel);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
			pre_sys_state.wlan11ac_chann = atoi(channel);

			if( need_commit )
				cfg_commit_object(WLAN_ENTRY1_NODE);
		}
	
		fp = fopen(APCLI_EXEC_SH, "w");
		if( !fp )
			return -1;
		
		if(cfg_obj_get_object_attr(path, "MACRepeaterEn", 0, tmpBuf, sizeof(tmpBuf)) > 0 && !strcmp(tmpBuf, "1"))
		{
			wifimgr_lib_set_WIFI_CMD(ssid_ifname, 0, sizeof(cmd), "MACRepeaterEn=1\n", cmd, 0);
			fputs(cmd,fp);
		}
		
		wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), "ApCliEnable=0\n", cmd, 0);
		fputs(cmd, fp);
		
		cfg_obj_get_object_attr(path, "AuthMode", 0, AuthMode, sizeof(AuthMode));
		
		if( !strcmp(AuthMode, "OPEN") )
		{
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "ApCliAuthMode=%s\n", AuthMode);
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
			fputs(cmd, fp);
			
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), "ApCliEncrypType=NONE\n", cmd, 0);
			fputs(cmd, fp);				
		}
		else if( strstr(AuthMode, "WEP") )
		{
			if( strstr(AuthMode, "OPEN") )
				wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), "ApCliAuthMode=OPEN\n", cmd, 0);
			else
				wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), "ApCliAuthMode=SHARED\n", cmd, 0);

			fputs(cmd, fp);
			
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), "ApCliEncrypType=WEP\n", cmd, 0);
			fputs(cmd, fp);
			
			if( cfg_obj_get_object_attr(path, "DefaultKeyID", 0, defKeyID, sizeof(defKeyID)) > 0 && atoi(defKeyID) > 0)
			{
				cfg_obj_get_object_attr(path, "Key", 0, Passwd, sizeof(Passwd));
				memset(cmd_value, 0, sizeof(cmd_value));
				snprintf(cmd_value, sizeof(cmd_value), "ApCliDefaultKeyID=%s\n", Passwd);
				wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
				fputs(cmd, fp);
				
				memset(cmd_value, 0, sizeof(cmd_value));
				snprintf(cmd_value, sizeof(cmd_value), "ApCliKey%s=%s\n", defKeyID, Passwd);
				wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
				fputs(cmd, fp);
			}
		}
		else if( !strcmp(AuthMode, "WPAPSK") || !strcmp(AuthMode, "WPA2PSK") )
		{
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "ApCliAuthMode=%s\n", AuthMode);
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
			fputs(cmd, fp);
			
			cfg_obj_get_object_attr(path, "EncrypType", 0, EncrypType, sizeof(EncrypType));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "ApCliEncrypType=%s", EncrypType);
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
			fputs(cmd, fp);
			
			cfg_obj_get_object_attr(path, "WPAPSK", 0, Passwd, sizeof(Passwd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "ApCliWPAPSK=%s\n", Passwd);
			wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
			fputs(cmd, fp);
		}
		
		memset(cmd_value, 0, sizeof(cmd_value));
		snprintf(cmd_value, sizeof(cmd_value), "ApCliSsid=%s\n", SSID);
		wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
		fputs(cmd, fp);
		
		memset(cmd_value, 0, sizeof(cmd_value));
		snprintf(cmd_value, sizeof(cmd_value), "ApCliEnable=%s\n", EnableValue);
		wifimgr_lib_set_WIFI_CMD(ifName, -1, sizeof(cmd), cmd_value, cmd, 0);
		fputs(cmd, fp);

		fclose(fp);
		chmod(APCLI_EXEC_SH, 777);
		system(APCLI_EXEC_SH);
		unlink(APCLI_EXEC_SH);
			
		if( cfg_obj_get_object_attr(APCLI_COMMON_NODE, "syncFlag", 0, tmpBuf, sizeof(tmpBuf)) > 0
			&& (1 == atoi(tmpBuf) ) )
		{
			snprintf(param, sizeof(param), "%s", tmpBuf);
			cfg_send_event(EVT_WAN_INTERNAL, EVT_WAN_AP_LINK_UPDATE, param, sizeof(param));
		}
		cfg_obj_set_object_attr(APCLI_COMMON_NODE, "StartConn", 0, "0");
	}

	return 0;
}

int svc_wlan_apcli_stop() 
{
	char cmd[256] = {0};

	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "StartConn", 0, "0");
	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "1");
	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "0");
#if defined(TCSUPPORT_CT_UBUS)
	cfg_obj_set_object_attr(WANINFO_COMMON_NODE, "APClientState", 0, "down");
#endif

	wifimgr_lib_set_WIFI_CMD("apcli0", -1, sizeof(cmd), "WscStop=1", cmd, 0);
	system(cmd);
	wifimgr_lib_set_WIFI_CMD("apcli0", -1, sizeof(cmd), "ApCliEnable=0", cmd, 0);
	system(cmd);
	wifimgr_lib_set_WIFI_CMD("apclii0", -1, sizeof(cmd), "WscStop=1", cmd, 0);
	system(cmd);
	wifimgr_lib_set_WIFI_CMD("apclii0", -1, sizeof(cmd), "ApCliEnable=0", cmd, 0);
	system(cmd);
	return 0;
}

int svc_wlan_apcli_start() 
{
#if defined(TCSUPPORT_CT_UBUS)
	char enable[4] = {0}, radio[4] = {0};
	char nodeName[32] = {0};

	/* firstly, stop apcli uplink */
	svc_wlan_apcli_stop();

	/* check radio and enable */
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, radio, sizeof(radio));
	if ( 0 == strcmp(radio, "1") )
		strcpy(nodeName, APCLI_ENTRY1_NODE); /* 5g */
	else
		strcpy(nodeName, APCLI_ENTRY0_NODE); /* 2.4g */
	
	cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable));
	if ( 0 == enable[0] || 0 != strcmp(enable, "1") )
		return -1;
#endif

	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "StartConn", 0, "1");
	cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "0");
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", "3");

	return 0;
}
#endif
int svc_wlan_mgr_internal_handle_event(int event, pt_wlan_evt param)
{
	int ret = 0;
		
	switch(event){
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
	case EVT_WLAN_ROAM11K_UPDATE:
		ret = svc_wlan_roam11k_handle_event_update(param);
		break;
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
	case EVT_WLAN_ROAM11V_UPDATE:
		ret = svc_wlan_roam11v_handle_event_update(param);
		break;
#endif
		case EVT_WLAN_UPDATE:
			ret = svc_wlan_handle_event_update(param);
			break;

		case EVT_WLAN_BOOT:
			ret = svc_wlan_boot();
			break;
#if defined(TCSUPPORT_WLAN_AC)	
		case EVT_WLAN11AC_BOOT:
			ret = svc_wlan11ac_boot();
			break;
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
		case EVT_WLAN_AP_START_CLIENT:
			ret = svc_wlan_apcli_start();
			break;
		case EVT_WLAN_AP_STOP_CLIENT:
			svc_wlan_apcli_stop();
			break;
#endif
#ifdef TCSUPPORT_WLAN_VENDIE
		case EVT_CFG_BEACONVSIE_START:
			ret = beaconvsie_entry_action(param,BEACONVSIE_COMMIT);
			break;

		case EVT_CFG_BEACONVSIE_UPDATE:
			ret = beaconvsie_entry_action(param,BEACONVSIE_UPDATE);
			break;
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
		case EVT_WLAN_BNDSTRG_UPDATE:
			ret = svc_wlan_Bndstrg_handle_event_update(param);
			break;
			
#endif	
		case EVT_WLAN_MONITOR_UPDATE:
			ret = svc_wlan_Monitor_handle_event_update(param);
			break;
		case EVT_WLAN_MACBLACKLIST_UPDATE:
			ret = svc_maclist_excute(2);
			break;
		case EVT_WLAN_MACWHITELIST_UPDATE:
			ret = svc_maclist_excute(1);
			break;
#ifdef TCSUPPORT_WLAN_VENDIE
		case EVT_WLAN_PROBEVSIE_UPDATE:
			ret = svc_wlan_probevsie_handle_event_update(param);
			break;
		case EVT_WLAN_PROBERESP_UPDATE:
			ret = svc_wlan_proberesp_handle_event_update(param);
			break;
		case EVT_WLAN_PROBERESP_DELETE:
			ret = svc_wlan_proberesp_handle_event_delete(param);
			break;
#endif
		case EVT_WLAN_COLLECT_INFO:
			ret = svc_wlan_collect_info_handle();
			break;
#ifdef TCSUPPORT_WLAN_ACS
		case EVT_WLAN_ACS_UPDATE:
			ret = svc_wlan_acs_handle_event_update(param); 
			break;
		case EVT_WLAN11AC_ACS_UPDATE:
			ret = svc_wlan_acs_handle_event_update(param); 
			break;
#endif
#if defined(TCSUPPORT_CT_JOYME2)
		case EVT_WLAN_RATEPRIORITY_UPDATE:
			ret = svc_wlan_ratepriority_handle_event_update(0); 
			break;
		case EVT_WLAN11AC_RATEPRIORITY_UPDATE:
			ret = svc_wlan_ratepriority_handle_event_update(1); 
			break;
		case EVT_WLAN_ROAM:
			ret = svc_wlan_roam_handle_event_update(0);
			break;
		case EVT_WLAN11AC_ROAM:
			ret = svc_wlan_roam_handle_event_update(1);
			break;
#endif
		default:
			tcdbg_printf("svc_wlan_mgr_internal_handle_event: path[%s]  event [%d]  \n", param->buf, event);	
	}

	return ret;
}

#if 0
int svc_wlan_mgr_external_handle_event(int event,pt_wlan_evt param)
{
	int ret = 0;
	
	printf("svc_wlan_mgr_external_handle_event: event [%d] [%s]  \n", event, param->buf);	
	switch(event){		
		default:
			printf("svc_wlan_mgr_external_handle_event: path[%s]  event [%d]  \n", param->buf, event);	
	}

	return ret;
}
#endif
static int svc_wlan_Monitor_update_object(char* path){
	svc_wlan_Monitor_execute(path);
	return CFG2_RET_SUCCESS;
}

int svc_wlan_Monitor_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_Bndstrg_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_Monitor_update_object(path) < 0){
		printf("svc_wlan_update_object: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}

	return CFG2_RET_SUCCESS;
}
int svc_wlan_Monitor_execute(char* path)
{
	sem_post(&SEM_CCRSSI);
	return CFG2_RET_SUCCESS;		
}

#if defined(TCSUPPORT_NP_CMCC)
int WLanLEDSwitch(int type, int APOn)
{
	int last_apon = 0;
	char cmd[128] = {0}, radio_buf[16] = {0};
	
	if( type )
	{
		last_apon = wlan11ac_last_apon;
		wlan11ac_last_apon = APOn;
		snprintf(radio_buf, sizeof(radio_buf), "wlan11acled");
	}
	else
	{
		last_apon = wlan_last_apon;
		wlan_last_apon = APOn;
		snprintf(radio_buf, sizeof(radio_buf), "wlanled");
	}
	
	if( 0 == last_apon && 1 == APOn )
	{
		snprintf(cmd, sizeof(cmd), "sys %s recover", radio_buf);
	}
	else if( 1 == last_apon && 0 == APOn )
	{
		snprintf(cmd, sizeof(cmd), "sys %s off", radio_buf);
	}
	system(cmd);
	
	return 0;
}
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
static int svc_wlan_Bndstrg_update_object(char* path){	
	svc_wlan_Bndstrg_execute(path);
	return CFG2_RET_SUCCESS;
}

int svc_wlan_Bndstrg_handle_event_update(pt_wlan_evt param)
{
	char* path = param->buf;
	if (cfg_query_object(path,NULL,NULL) <= 0){
		printf("svc_wlan_Bndstrg_handle_event_update: cfg query [%s] fail  \n", path);
		return CFG2_RET_SUCCESS;
	}
	
	if (svc_wlan_Bndstrg_update_object(path) < 0){
		printf("svc_wlan_update_object: [%s] update fail \n",path);
		return CFG2_RET_FAIL;
	}

	return CFG2_RET_SUCCESS;
}
int svc_wlan_Bndstrg_execute(char* path)
{
	char attrtmp[160] = {0};
	char cmd_value[64] = {0};
	int BndTrue = BndstrgParaCheck();
	tcdbg_printf("svc_wlan_Bndstrg_execute BndTrue=%d\n",BndTrue);
	
	if(BndTrue){
#if defined(TCSUPPORT_CT_JOYME4)
		if(cfg_get_object_attr(MESH_COMMON_NODE, "SteerEnable", attrtmp, sizeof(attrtmp)) > 0){
#else
		if(cfg_get_object_attr(WLAN_BNDSTRG_NODE, "BndStrgEnable", attrtmp, sizeof(attrtmp)) > 0){
#endif
			if(1 == atoi(attrtmp)){
				BndStrgSetParameter();
				}
			else if(0 == atoi(attrtmp)){
	#if defined(TCSUPPORT_CT_JOYME4)
				system("/userfs/bin/mapd_cli /tmp/mapd_ctrl setsteer 0");
	#else
				system("killall -15 bndstrg2");
				memset(cmd_value, 0, sizeof(cmd_value));
				wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd_value), "BndStrgEnable=0", cmd_value, 0);
				system(cmd_value);
				memset(cmd_value, 0, sizeof(cmd_value));
				wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd_value), "BndStrgEnable=0", cmd_value, 0);
				system(cmd_value);
	#endif
				}
			else
				tcdbg_printf("invalid bndstrg2 argument,should be true or false\n");
		}	
	}
	return CFG2_RET_SUCCESS;
		
}

#endif
int svc_wlan_execute(char* path)
{
	FILE *fp 			= NULL;
	char APOn[4] 		= {0}; 
	char Bssid_Num[4] 	= {0};
	char channel[4] 	= {0};
	int WpsStatus 		= 0;
	int bssidNum 		= 0;
	int iChannel		= 0;
	WLan_info wlan_info;
	WLan_info wlan_info_other;
	char flag[4]={0};
	char BssidNum_11ac[4]	= {0};
	int ret = 0;
	int WiFi2gResetFlag = 0;/*need to clear*/
	
	char buf[64] = {0};
	unsigned char map_enable = 0;

#if defined (TCSUPPORT_CT_JOYME4)	
	char WLanJoymeSet[8] = {0};
#endif
#ifdef TCSUPPORT_WLAN_WDS	
	int isWDSReloadWIFI 	= 0;
	int WdsActive			= 0;
	int isWDSReloadWIFI_other 	= 0;
	int WdsActive_other			= 0;
#endif
	char whnat[4]		= {0}; 
	int iWhnat			= 1;
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	Wifi_common_info wifi_common_info;
	char BeaconPeriod[8]	= {0};
	char  DtimPeriod[8] = {0};
	char  TxPower[8]	= {0};
	char HtBssCoex[8]	= {0};
	char WirelessMode[8]	= {0};
	char HtExchar[8]	= {0};
	char HTBw[8]	= {0};
	char HtGi[8]	= {0};
	char HtTxstream[8]	= {0};
	char HtRxstream[8]	= {0};
	char CountryRegion[8]	= {0};
#if defined(TCSUPPORT_WLAN_AX)
	char MuOfdmaDlEnable[8] = {0};
	char MuOfdmaUlEnable[8] = {0};
	char MuMimoDlEnable[8] = {0};
	char MuMimoUlEnable[8] = {0};
	char TWTSupport[8] = {0};
	char SREnable[8] = {0};
	char SRMode[8] = {0};
	char SRSDEnable[8] = {0};
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	char Ant_dis[8] = {0};
	char Ant_convert[8] = {0};
	char Ant_sel[8] = {0};
#endif
#endif

	int nodeid;
	char id_tmp[4] = {0};
	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id", id_tmp, sizeof(id_tmp)) > 0){
				nodeid = atoi(id_tmp);
	}else{
				nodeid = 0;
	}
#endif

#ifdef TCSUPPORT_CT_WLAN_JOYME3
	char samessidAction[30] = {0};
#endif
#if 0
#ifdef TCSUPPORT_SYSLOG
	check_wlan_exist_for_syslog(top);
#endif
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
	KillBndStrgProcess();
#endif
	if(strstr(path, "common") != NULL){
#ifdef TCSUPPORT_MTD_ENCHANCEMENT	
		checkWriteBinToFlash();
#endif	
		return CFG2_RET_SUCCESS;
	}
	if((cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", APOn, sizeof(APOn)) < 0) || 
	(cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", Bssid_Num, sizeof(Bssid_Num)) < 0)){		
		return CFG2_RET_FAIL;
	}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "Channel", channel, sizeof(channel)) > 0){
		wifi_common_info.wlan_chann = atoi(channel);
	}else
		wifi_common_info.wlan_chann = pre_sys_state.wlan_chann;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "BeaconPeriod", BeaconPeriod, sizeof(BeaconPeriod)) > 0){
		wifi_common_info.wlan_BeaconPeriod =	atoi(BeaconPeriod);
	}else
		wifi_common_info.wlan_BeaconPeriod = pre_sys_state.wlan_BeaconPeriod;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "DtimPeriod", DtimPeriod, sizeof(DtimPeriod)) > 0){
		wifi_common_info.wlan_DtimPeriod =	atoi(DtimPeriod);
	}else
		wifi_common_info.wlan_DtimPeriod = pre_sys_state.wlan_DtimPeriod;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_TxStream", HtTxstream, sizeof(HtTxstream)) > 0){
		wifi_common_info.wlan_HtTxstream=	atoi(HtTxstream);
	}else
		wifi_common_info.wlan_HtTxstream = pre_sys_state.wlan_HtTxstream;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_RxStream", HtRxstream, sizeof(HtRxstream)) > 0){
		wifi_common_info.wlan_HtRxstream =	atoi(HtRxstream);
	}else
		wifi_common_info.wlan_HtRxstream = pre_sys_state.wlan_HtRxstream;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BSSCoexistence", HtBssCoex, sizeof(HtBssCoex)) > 0){
		wifi_common_info.wlan_HtBssCoex=	atoi(HtBssCoex);
	}else
		wifi_common_info.wlan_HtBssCoex = pre_sys_state.wlan_HtBssCoex;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "WirelessMode", WirelessMode, sizeof(WirelessMode)) > 0){
		wifi_common_info.wlan_WirelessMode= atoi(WirelessMode);
	}else
		wifi_common_info.wlan_WirelessMode = pre_sys_state.wlan_WirelessMode;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_EXTCHA", HtExchar, sizeof(HtExchar)) > 0){
		wifi_common_info.wlan_HtExchar= atoi(HtExchar);
	}else
		wifi_common_info.wlan_HtExchar = pre_sys_state.wlan_HtExchar;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BW", HTBw, sizeof(HTBw)) > 0){
		wifi_common_info.wlan_HTBw= atoi(HTBw);
	}else
		wifi_common_info.wlan_HTBw = pre_sys_state.wlan_HTBw;

	if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_GI", HtGi, sizeof(HtGi)) > 0){
		wifi_common_info.wlan_HtGi= atoi(HtGi);
	}else
		wifi_common_info.wlan_HtGi = pre_sys_state.wlan_HtGi;

	if(cfg_get_object_attr(WLAN_COMMON_NODE, "CountryRegion", CountryRegion, sizeof(CountryRegion)) > 0){
		wifi_common_info.wlan_CountryRegion= atoi(CountryRegion);
	}else
		wifi_common_info.wlan_CountryRegion = pre_sys_state.wlan_CountryRegion;
	
#if defined(TCSUPPORT_WLAN_AX)
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "MuOfdmaDlEnable", MuOfdmaDlEnable, sizeof(MuOfdmaDlEnable)) > 0
		&& strcmp(MuOfdmaDlEnable, "N/A")){
		wifi_common_info.wlan_MuOfdmaDlEnable= atoi(MuOfdmaDlEnable);
	}else
		wifi_common_info.wlan_MuOfdmaDlEnable = pre_sys_state.wlan_MuOfdmaDlEnable;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "MuOfdmaUlEnable", MuOfdmaUlEnable, sizeof(MuOfdmaUlEnable)) > 0
		&& strcmp(MuOfdmaUlEnable, "N/A")){
		wifi_common_info.wlan_MuOfdmaUlEnable= atoi(MuOfdmaUlEnable);
	}else
		wifi_common_info.wlan_MuOfdmaUlEnable = pre_sys_state.wlan_MuOfdmaUlEnable;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "MuMimoDlEnable", MuMimoDlEnable, sizeof(MuMimoDlEnable)) > 0
		&& strcmp(MuMimoDlEnable, "N/A")){
		wifi_common_info.wlan_MuMimoDlEnable= atoi(MuMimoDlEnable);
	}else
		wifi_common_info.wlan_MuMimoDlEnable = pre_sys_state.wlan_MuMimoDlEnable;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "MuMimoUlEnable", MuMimoUlEnable, sizeof(MuMimoUlEnable)) > 0
		&& strcmp(MuMimoUlEnable, "N/A")){
		wifi_common_info.wlan_MuMimoUlEnable= atoi(MuMimoUlEnable);
	}else
		wifi_common_info.wlan_MuMimoUlEnable = pre_sys_state.wlan_MuMimoUlEnable;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "TWTSupport", TWTSupport, sizeof(TWTSupport)) > 0
		&& strcmp(TWTSupport, "N/A")){
		wifi_common_info.wlan_TWTSupport= atoi(TWTSupport);
	}else
		wifi_common_info.wlan_TWTSupport = pre_sys_state.wlan_TWTSupport;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "SREnable", SREnable, sizeof(SREnable)) > 0
		&& strcmp(SREnable, "N/A")){
		wifi_common_info.wlan_SREnable= atoi(SREnable);
	}else
		wifi_common_info.wlan_SREnable = pre_sys_state.wlan_SREnable;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "SRMode", SRMode, sizeof(SRMode)) > 0
		&& strcmp(SRMode, "N/A")){
		wifi_common_info.wlan_SRMode= atoi(SRMode);
	}else
		wifi_common_info.wlan_SRMode = pre_sys_state.wlan_SRMode;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "SRSDEnable", SRSDEnable, sizeof(SRSDEnable)) > 0
		&& strcmp(SRSDEnable, "N/A")){
		wifi_common_info.wlan_SRSDEnable= atoi(SRSDEnable);
	}else
		wifi_common_info.wlan_SRSDEnable = pre_sys_state.wlan_SRSDEnable;
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)		
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_dis", Ant_dis, sizeof(Ant_dis)) > 0
		&& strcmp(Ant_dis, "N/A")){
		wifi_common_info.wlan_ant_dis = atoi(Ant_dis);
	}else
		wifi_common_info.wlan_ant_dis = pre_sys_state.wlan_ant_dis;
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_convert", Ant_convert, sizeof(Ant_convert)) > 0
		&& strcmp(Ant_dis, "N/A")){
		wifi_common_info.wlan_ant_convert = atoi(Ant_convert);
	}else
		wifi_common_info.wlan_ant_convert = pre_sys_state.wlan_ant_convert;

	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_sel", Ant_sel, sizeof(Ant_sel)) > 0
		&& strcmp(Ant_dis, "N/A")){
		wifi_common_info.wlan_ant_sel = atoi(Ant_sel);
	}else
		wifi_common_info.wlan_ant_sel = pre_sys_state.wlan_ant_sel;
#endif
#endif
	if (wifi_common_info.wlan_chann 	   !=	pre_sys_state.wlan_chann ||
		wifi_common_info.wlan_CountryRegion !=	pre_sys_state.wlan_CountryRegion ||
		wifi_common_info.wlan_BeaconPeriod !=	pre_sys_state.wlan_BeaconPeriod ||
		wifi_common_info.wlan_DtimPeriod   !=	pre_sys_state.wlan_DtimPeriod ||
		wifi_common_info.wlan_HtTxstream   !=	pre_sys_state.wlan_HtTxstream ||
		wifi_common_info.wlan_HtRxstream   !=	pre_sys_state.wlan_HtRxstream ||
		wifi_common_info.wlan_HtBssCoex    !=	pre_sys_state.wlan_HtBssCoex ||
		wifi_common_info.wlan_WirelessMode !=	pre_sys_state.wlan_WirelessMode ||
		(wifi_common_info.wlan_HtExchar 	!=	pre_sys_state.wlan_HtExchar && wifi_common_info.wlan_chann != 0) ||
		wifi_common_info.wlan_HTBw		   !=	pre_sys_state.wlan_HTBw ||
		wifi_common_info.wlan_HtGi		   !=	pre_sys_state.wlan_HtGi 
#if defined(TCSUPPORT_WLAN_AX)
		|| wifi_common_info.wlan_MuOfdmaDlEnable		!=	pre_sys_state.wlan_MuOfdmaDlEnable 
		|| wifi_common_info.wlan_MuOfdmaUlEnable		!=	pre_sys_state.wlan_MuOfdmaUlEnable 
		|| wifi_common_info.wlan_MuMimoDlEnable 		!=	pre_sys_state.wlan_MuMimoDlEnable 
		|| wifi_common_info.wlan_MuMimoUlEnable 		!=	pre_sys_state.wlan_MuMimoUlEnable 
		|| wifi_common_info.wlan_TWTSupport 			!=	pre_sys_state.wlan_TWTSupport 
		|| wifi_common_info.wlan_SREnable				!=	pre_sys_state.wlan_SREnable 
		|| wifi_common_info.wlan_SRMode 				!=	pre_sys_state.wlan_SRMode 
		|| wifi_common_info.wlan_SRSDEnable 			!=	pre_sys_state.wlan_SRSDEnable 
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
		|| wifi_common_info.wlan_ant_dis				!=	pre_sys_state.wlan_ant_dis 
		|| wifi_common_info.wlan_ant_convert			!=	pre_sys_state.wlan_ant_convert
		|| wifi_common_info.wlan_ant_sel				!=	pre_sys_state.wlan_ant_sel
#endif
#endif
		){
		if (wifimgr_lib_get_11N_CMDflag != 1)
		{
			WiFi2gResetFlag = 1 ;
		}
		tcdbg_printf("@@@@%s,%d,nodeid=%d,2g need to reset\r\n",__FUNCTION__,__LINE__,nodeid);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_chann=%d,pre_sys_state.wlan_chann=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_chann,pre_sys_state.wlan_chann);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_CountryRegion=%d,pre_sys_state.wlan_CountryRegion=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_CountryRegion,pre_sys_state.wlan_CountryRegion);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_BeaconPeriod=%d,pre_sys_state.wlan_BeaconPeriod=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_BeaconPeriod,pre_sys_state.wlan_BeaconPeriod);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_DtimPeriod=%d,pre_sys_state.wlan_DtimPeriod=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_DtimPeriod,pre_sys_state.wlan_DtimPeriod);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HtTxstream=%d,pre_sys_state.wlan_HtTxstream=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HtTxstream ,pre_sys_state.wlan_HtTxstream);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HtRxstream=%d,pre_sys_state.wlan_HtRxstream=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HtRxstream ,pre_sys_state.wlan_HtRxstream);

		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HtBssCoex=%d,pre_sys_state.wlan_HtBssCoex=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HtBssCoex  ,pre_sys_state.wlan_HtBssCoex);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_WirelessMode=%d,pre_sys_state.wlan_WirelessMode=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_WirelessMode,pre_sys_state.wlan_WirelessMode);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HtExchar=%d,pre_sys_state.wlan_HtExchar=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HtExchar,pre_sys_state.wlan_HtExchar);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HTBw=%d,pre_sys_state.wlan_HTBw=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HTBw,pre_sys_state.wlan_HTBw);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_HtGi=%d,pre_sys_state.wlan_HtGi=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_HtGi,pre_sys_state.wlan_HtGi);

#if defined(TCSUPPORT_WLAN_AX)
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_MuOfdmaDlEnable=%d,pre_sys_state.wlan_MuOfdmaDlEnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_MuOfdmaDlEnable,pre_sys_state.wlan_MuOfdmaDlEnable);

		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_MuOfdmaUlEnable=%d,pre_sys_state.wlan_MuOfdmaUlEnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_MuOfdmaUlEnable,pre_sys_state.wlan_MuOfdmaUlEnable);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_MuMimoDlEnable=%d,pre_sys_state.wlan_MuMimoDlEnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_MuMimoDlEnable,pre_sys_state.wlan_MuMimoDlEnable);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_MuMimoUlEnable=%d,pre_sys_state.wlan_MuMimoUlEnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_MuMimoUlEnable,pre_sys_state.wlan_MuMimoUlEnable);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_TWTSupport=%d,pre_sys_state.wlan_TWTSupport=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_TWTSupport,pre_sys_state.wlan_TWTSupport);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_SREnable=%d,pre_sys_state.wlan_SREnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_SREnable,pre_sys_state.wlan_SREnable);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_SRMode=%d,pre_sys_state.wlan_SRMode=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_SRMode,pre_sys_state.wlan_SRMode);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_SRSDEnable=%d,pre_sys_state.wlan_SRSDEnable=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_SRSDEnable,pre_sys_state.wlan_SRSDEnable);
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)			
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_nt_dis=%d,pre_sys_state.wlan_ant_dis=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_ant_dis,pre_sys_state.wlan_ant_dis);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_ant_convert=%d,pre_sys_state.wlan_ant_convert=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_ant_convert,pre_sys_state.wlan_ant_convert);
		
		tcdbg_printf("@@@@%s,%d,wifi_common_info.wlan_ant_sel=%d,pre_sys_state.wlan_ant_sel=%d\r\n",
					__FUNCTION__,__LINE__,wifi_common_info.wlan_ant_sel,pre_sys_state.wlan_ant_sel);
#endif
#endif
	}
#endif

	bssidNum = atoi(Bssid_Num);
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "Channel", channel, sizeof(channel)) > 0){
		iChannel =	atoi(channel);
	}

	memset(&wlan_info, 0, sizeof(WLan_info));
	memset(&wlan_info_other, 0, sizeof(WLan_info));
#ifdef TCSUPPORT_WLAN_WDS
	wlan_wds_execute(&isWDSReloadWIFI, &WdsActive);
	wlan_info.wdsActive = WdsActive;
        wlan_info.isWDSReloadWIFI = isWDSReloadWIFI;
#endif
	wlan_info.type 	= WIFI_2_4G;
	wlan_info.oldNum = pre_sys_state.wlan_Bssid_num;
	wlan_info.newNum = bssidNum;
	wlan_info.index = getWlanSSIDEnableNum(WIFI_2_4G, bssidNum);
#ifdef TCSUPPORT_CT_WLAN_JOYME3
#if defined(TCSUPPORT_WLAN_AC)	
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "Action", samessidAction, sizeof(samessidAction)) > 0 ) {
		if (!strcmp(samessidAction,"set"))
			SetSameSSIDandKey(samessidAction,"wlan11ac"); 
	}
#endif
#endif
#ifdef TCSUPPORT_WLAN_LED_BY_SW
	wlan_led_action();
#endif
#if defined(TCSUPPORT_WLAN_AC)	
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", BssidNum_11ac, sizeof(BssidNum_11ac)) < 0)
			return CFG2_RET_FAIL;
#endif
	wifimgr_lib_reboot(WIFI_2_4G, &(pre_sys_state.wlan_Bssid_num), bssidNum, MAX_WDS_ENTRY, BssidNum_11ac, "BssidNum");
	if ( cfg_get_object_attr(WLAN_COMMON_NODE, "WHNAT", whnat, sizeof(whnat)) > 0 )
	{
		iWhnat = atoi(whnat);
	}
	wifimgr_lib_reboot(WIFI_2_4G, &(pre_sys_state.wlan_whnat), iWhnat, MAX_WDS_ENTRY, BssidNum_11ac, "WHNAT");

	/*need reload module when Bssid_num change*/
#ifdef TCSUPPORT_WLAN_WDS
	if(wifimgr_lib_get_11N_CMDflag() == 0 ||
		(bssidNum != pre_sys_state.wlan_Bssid_num)|| (isWDSReloadWIFI == 1))
#else
	if(wifimgr_lib_get_11N_CMDflag() == 0 ||
		bssidNum != pre_sys_state.wlan_Bssid_num)
#endif
	{
#if defined(TCSUPPORT_WLAN_AC)	
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", flag, sizeof(flag)) <= 0){
			return CFG2_RET_FAIL;
		}
#endif
#ifdef TCSUPPORT_WLAN_WDS
		wlan11ac_wds_execute(&isWDSReloadWIFI_other, &WdsActive_other);
		wlan_info_other.wdsActive = WdsActive_other;
	    wlan_info_other.isWDSReloadWIFI = isWDSReloadWIFI_other;
#endif
		wlan_info_other.type 	= WIFI_5G;
		wlan_info_other.oldNum = pre_sys_state.wlan11ac_Bssid_num;
		wlan_info_other.newNum = atoi(BssidNum_11ac);
		wlan_info_other.index = getWlanSSIDEnableNum(WIFI_5G, atoi(BssidNum_11ac));
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_BSSIDNUM_CHANGE, nodeid, WiFi2gResetFlag);
#else
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_BSSIDNUM_CHANGE);
#endif
		pre_sys_state.wlan_Bssid_num = bssidNum;
	}

	if(!strcmp(APOn, WLAN_APOFF)){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_DOWN, nodeid, WiFi2gResetFlag);
#else
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_DOWN);
#endif
#ifdef WSC_AP_SUPPORT		
		wpsAction(path, APOn, WLANSTOP);
#endif	
		g_wlan_switch_cfg.wifi_switch &= ~(WLAN_24G_ON);
	}else if(!strcmp(APOn, WLAN_APON)){
		g_wlan_switch_cfg.wifi_switch |= (WLAN_24G_ON);
		if((iChannel == 0)&&(pre_sys_state.wlan_chann != 0)){
			snprintf(flag, sizeof(flag), "1");
		}
#if defined(TCSUPPORT_MAP_R2)
		memset(buf, 0, sizeof(buf));
		ret = cfg_get_object_attr("root.mesh.dat", "MapEnable", buf, sizeof(buf));
		map_enable = atoi(buf);
		
		if(map_enable && iChannel != pre_sys_state.wlan_chann)
		{
			snprintf(flag, sizeof(flag), "1");
		}
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		ret = wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_UP, nodeid, WiFi2gResetFlag);
#else
		ret = wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_UP);
#endif
		
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
		fp = fopen(SVC_WLAN_ANT_PATH, "r");
		if(fp != NULL){
			fclose(fp);
			system(SVC_WLAN_ANT_PATH);
		}
#endif

		if (ret == 1)
		{
			/*start all interface */
			startWifiInterface(path, bssidNum);
		}
			
		dowscd(path);
#ifdef WSC_AP_SUPPORT	
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
		if((wps_both_start == 0)&&(isWPSRunning() == 0)){
			WpsStatus = wpsAction(path, APOn, WPSSTART);
		}else{
			wps24g_done = 1;
		}
#else
		WpsStatus = wpsAction(path, APOn, WPSSTART);
#endif
#endif
		wifimgr_lib_set_WifiTxQueue(bssidNum, WIFI_DEV_TXQUEUE_LEN);

		if(!WpsStatus && (iChannel == 0)
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && defined(TCSUPPORT_CT_WLAN_JOYME3)
		 && (nodeid == 0 )
#elif defined(TCSUPPORT_WPS_BTN_DUALBAND)
		 && (wps_both_start == 0)
#endif
		){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
			wifimgr_lib_do_restartInterface(wlan_info, nodeid, WiFi2gResetFlag);
#else
			if(1 != wifimgr_lib_get_11N_CMDflag()){
				wifimgr_lib_do_restartInterface(wlan_info);
			}
			wifimgr_lib_set_11N_CMDflag(0);
#endif
		}
#ifdef TCSUPPORT_MTD_ENCHANCEMENT
		checkWriteBinToFlash();
#endif	
		executeWLANSetupScript();

#if defined(TCSUPPORT_WLAN_8021X)	
		rtdot1xd_stop();
		rtdot1xd_start();
#endif

#if defined (TCSUPPORT_CT_JOYME4)	
		if(cfg_get_object_attr(WLAN_COMMON_NODE, "WLanJoymeSet", WLanJoymeSet, sizeof(WLanJoymeSet)) > 0){
			if(!strcmp(WLanJoymeSet, "1")){
				tcdbg_printf("execute wlan_JoymeSet.sh\r\n");
				system("/usr/script/wlan_JoymeSet.sh &");
			}
		}

#endif
	}else{
		return CFG2_RET_FAIL;
	} 
	pre_sys_state.wlan_chann = iChannel;
#ifdef TCSUPPORT_WLAN_BNDSTRG
	svc_wlan_Bndstrg_execute(WLAN_BNDSTRG_NODE);
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
	svc_cfg_maclist_commit();
#endif

#if defined(TCSUPPORT_NP_CMCC)
	WLanLEDSwitch(0, atoi(APOn));
#endif
	return CFG2_RET_SUCCESS;
}

#ifdef TCSUPPORT_WLAN_DOT11K_RRM
int svc_wlan_roam11k_execute(char* path)
{
	char cmd[128] = {0};
	char RrmEnable[2]={0};
	char ApCount[4]={0};
	char Sta_mac[20]={0};
	int Regclass = 0;
	int TimeOut = 0;
	char Channel[7][4] = {0};
	char Bssid[7][20]= {0}; 
	int n = 0;
	int j;
	char temp[64] = {0};
	char temp1[64] = {0};
	
	if( cfg_get_object_attr(WLAN_ROAM11K_NODE, "Rrm", RrmEnable, sizeof(RrmEnable)) > 0){
		kv_daemon_check();
		if(0 == atoi(RrmEnable)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 rrm %d",atoi(RrmEnable));
			return CFG2_RET_SUCCESS;
		}		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 rrm %d",atoi(RrmEnable)); 
		system(cmd);
	}

	if(cfg_get_object_attr(WLAN_ROAM11K_NODE, "ApCount", ApCount, sizeof(ApCount)) < 0){		  
		tcdbg_printf("The node attr for ApCount does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	if(cfg_get_object_attr(WLAN_ROAM11K_NODE, "Sta_mac", Sta_mac, sizeof(Sta_mac)) < 0){
		tcdbg_printf("The node attr for Sta_mac does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	n = atoi(ApCount);

	for(j=0;j<n;j++){
		memset(temp,0,sizeof(temp));
		snprintf(temp,sizeof(temp),"Channel%d",j); 
		if( cfg_get_object_attr(WLAN_ROAM11K_NODE, temp, Channel[j], sizeof(Channel[j])) < 0){
			
			tcdbg_printf("The node attr for channel[%d] does not exist\n",j);
			return CFG2_RET_FAIL;
		}
		memset(temp1,0,sizeof(temp1));
		snprintf(temp1,sizeof(temp1),"Bssid%d",j); 
		if( cfg_get_object_attr(WLAN_ROAM11K_NODE, temp1, Bssid[j], sizeof(Bssid[j])) < 0){
			
			tcdbg_printf("The node attr for Bssid[%d] does not exist\n",j);
			return CFG2_RET_FAIL;
		}
		if((strlen(Sta_mac)!=0) && (strlen(Channel[j])!=0) && (strlen(Bssid[j])!=0)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 bcnreq %s %d %s %s %d",Sta_mac,Regclass,Channel[j],Bssid[j],TimeOut);
			system(cmd);
		}
		cnt = j;
	}
	return CFG2_RET_SUCCESS;
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
int svc_wlan_roam11v_execute(char* path)
{
	char cmd[128] = {0};
	char BtmEnable[2]={0};
	char Sta_mac[18]={0};
	int Preference = 250;
	int TimeOut = 0;
	char Channel[4] = {0};
	char Bssid[18]={0}; 
	int Disasso_timer = 200; 
	char StatusCode[4] = {0};
	
	if( cfg_get_object_attr(WLAN_ROAM11V_NODE, "Btm", BtmEnable, sizeof(BtmEnable)) > 0){
		kv_daemon_check();
		if(0 == atoi(BtmEnable)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 btm %d",atoi(BtmEnable));
			return CFG2_RET_SUCCESS;
		}		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 btm %d",atoi(BtmEnable)); 
		system(cmd);
	}
	
	if( cfg_get_object_attr(WLAN_ROAM11V_NODE, "Channel", Channel, sizeof(Channel)) < 0){
		tcdbg_printf("The node attr for Channel does not exist\n");
		return CFG2_RET_FAIL;
	}	
	if( cfg_get_object_attr(WLAN_ROAM11V_NODE, "Bssid", Bssid, sizeof(Bssid)) < 0){
		tcdbg_printf("The node attr for Channel does not exist\n");
		return CFG2_RET_FAIL;
	}	
	if( cfg_get_object_attr(WLAN_ROAM11V_NODE, "Sta_mac", Sta_mac, sizeof(Sta_mac)) < 0){
		tcdbg_printf("The node attr for Sta_mac does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	if((strlen(Sta_mac)!=0) && (strlen(Bssid)!=0) && (strlen(Channel)!=0)){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl ra0 btmreq %s %s %s %d %d %d",Sta_mac,Bssid,Channel,Preference,Disasso_timer,TimeOut);
		system(cmd);
	}
	
	return CFG2_RET_SUCCESS;
}
#endif

#ifdef TCSUPPORT_WLAN_DOT11K_RRM
int svc_wlan11ac_roam11k_execute(char* path)
{
	char cmd[128] = {0};
	char RrmEnable[2]={0};
	char ApCount[4]={0};
	char Sta_mac[18]={0};
	int Regclass = 0;
	int TimeOut = 0;
	char Channel[7][4] = {0};
	char Bssid[7][20]= {0}; 
	int n = 0;
	int j;
	char temp[64] = {0};
	char temp1[64] = {0};
	tcdbg_printf("func: svc_wlan_roam11k_execute\n");
	if( cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, "Rrm", RrmEnable, sizeof(RrmEnable)) > 0){
		kv_daemon_check();
		if(0 == atoi(RrmEnable)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 rrm %d",atoi(RrmEnable));
			return CFG2_RET_SUCCESS;
		}		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 rrm %d",atoi(RrmEnable)); 
		system(cmd);
	}
	
	if(cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, "ApCount", ApCount, sizeof(ApCount)) < 0){		  
		tcdbg_printf("The node attr for ApCount does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	if( cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, "Sta_mac", Sta_mac, sizeof(Sta_mac)) < 0){
		tcdbg_printf("The node attr for Sta_mac does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	n = atoi(ApCount);
	
	for(j=0;j<n;j++){
		memset(temp,0,sizeof(temp));
		snprintf(temp,sizeof(temp),"Channel%d",j); 
		if( cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, temp, Channel[j], sizeof(Channel[j])) < 0){
			
			tcdbg_printf("The node attr for channel[%d] does not exist\n",j);
			return CFG2_RET_FAIL;
		}
		memset(temp1,0,sizeof(temp1));
		snprintf(temp1,sizeof(temp1),"Bssid%d",j); 
		if( cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, temp1, Bssid[j], sizeof(Bssid[j])) < 0){
			
			tcdbg_printf("The node attr for Bssid[%d] does not exist\n",j);
			return CFG2_RET_FAIL;
		}
		
		if((strlen(Sta_mac)!=0) && (strlen(Channel[j])!=0) && (strlen(Bssid[j])!=0)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 bcnreq %s %d %s %s %d",Sta_mac,Regclass,Channel[j],Bssid[j],TimeOut);
			system(cmd);
		}
		cnt = j;
	}
	return CFG2_RET_SUCCESS;
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
int svc_wlan11ac_roam11v_execute(char* path)
{
	char cmd[128] = {0};
	char BtmEnable[2]={0};
	char Sta_mac[18]={0};
	int Preference = 250;
	int TimeOut = 0;
	char Channel[4] = {0};
	char Bssid[18]={0}; 
	int Disasso_timer = 200; 
	char StatusCode[4] = {0};
	
	if( cfg_get_object_attr(WLAN11AC_ROAM11V_NODE, "Btm", BtmEnable, sizeof(BtmEnable)) > 0){
		kv_daemon_check();
		if(0 == atoi(BtmEnable)){
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 btm %d",atoi(BtmEnable));
			return CFG2_RET_SUCCESS;
		}		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 btm %d",atoi(BtmEnable)); 
		system(cmd);
	}
	
	if( cfg_get_object_attr(WLAN11AC_ROAM11V_NODE, "Channel", Channel, sizeof(Channel)) < 0){
		tcdbg_printf("The node attr for Channel does not exist\n");
		return CFG2_RET_FAIL;
	}	
	if( cfg_get_object_attr(WLAN11AC_ROAM11V_NODE, "Bssid", Bssid, sizeof(Bssid)) < 0){
		tcdbg_printf("The node attr for Channel does not exist\n");
		return CFG2_RET_FAIL;
	}	
	if( cfg_get_object_attr(WLAN11AC_ROAM11V_NODE, "Sta_mac", Sta_mac, sizeof(Sta_mac)) < 0){
		tcdbg_printf("The node attr for Sta_mac does not exist\n");
		return CFG2_RET_FAIL;
	}
	
	if((strlen(Sta_mac)!=0) && (strlen(Bssid)!=0) && (strlen(Channel)!=0)){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/userfs/bin/kvctrl rai0 btmreq %s %s %s %d %d %d",Sta_mac,Bssid,Channel,Preference,Disasso_timer,TimeOut);
		system(cmd);
	}
	
	return CFG2_RET_SUCCESS;
}
#endif

#if defined(TCSUPPORT_WLAN_AC)	
int svc_wlan11ac_execute(char* path)
{
	FILE *fp 			= NULL;
#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
	char node_name[128]	= {0};
#endif
	char APOn[4]		= {0};
	char BssidNum[4]	= {0};
	char tmp[128]		= {0};
	char channel[4]		= {0};
	WLan_info wlan_info;
	WLan_info wlan_info_other;
	int i;
	char BssidNum_11n[4]	= {0};
	char flag[4]={0};
	int ret = 0;

#if defined (TCSUPPORT_CT_JOYME4)	
	char WLanJoymeSet5G[8] = {0};
#endif
	char whnat[4]		= {0}; 
	int iWhnat			= 1;
#ifdef TCSUPPORT_WLAN_WDS	
	int isWDSReloadWIFI 	= 0;
	int WdsActive 			= 0;
	int isWDSReloadWIFI_other 	= 0;
	int WdsActive_other 	= 0;
#endif
#ifdef TCSUPPORT_CT_WLAN_JOYME3
	char samessidAction[30] = {0};
#endif

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	Wifi_common_info wifi_common_info;
	char BeaconPeriod[8]	= {0};
	char  DtimPeriod[8]	= {0};
	char  TxPower[8]	= {0};
	char HtBssCoex[8]	= {0};
	char WirelessMode[8]	= {0};
	char HtExchar[8]	= {0};
	char HTBw[8]	= {0};
	char HtGi[8]	= {0};
	char VhtBw[8]	= {0};
	char VhtGi[8]	= {0};
	int nodeacid;
	char id_tmp[4] = {0};
	int WiFi5gResetFlag = 0;//need to clear
	char HtTxstream[8]	= {0};
	char HtRxstream[8]	= {0};
	char CountryRegionABand[8]	= {0};
#if defined(TCSUPPORT_WLAN_AX)
	char MuOfdmaDlEnable[8] = {0};
	char MuOfdmaUlEnable[8] = {0};
	char MuMimoDlEnable[8] = {0};
	char MuMimoUlEnable[8] = {0};
	char TWTSupport[8] = {0};
	char SREnable[8] = {0};
	char SRMode[8] = {0};
	char SRSDEnable[8] = {0};
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	char Ant_dis[8] = {0};
	char Ant_convert[8] = {0};
	char Ant_sel[8] = {0};
#endif
#endif

	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_ac_id", id_tmp, sizeof(id_tmp)) > 0){
    	nodeacid = atoi(id_tmp);
  	}else{
    	nodeacid = 0;
 	}
#endif

#if 0
#ifdef TCSUPPORT_SYSLOG_ENHANCE
	check_wlan_exist_for_syslog(top);
#endif
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
	KillBndStrgProcess();
#endif
	if(strstr(path, "common") != NULL){
#ifdef TCSUPPORT_MTD_ENCHANCEMENT	
		checkWriteBinToFlash();
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
	SetSameSSIDCommonExcute(path);
#endif	
		return CFG2_RET_SUCCESS;
	}

	if((cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", APOn, sizeof(APOn)) < 0) || 
		(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", BssidNum, sizeof(BssidNum)) < 0)){
		return CFG2_RET_FAIL;
	}
	
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) || defined(TCSUPPORT_WPS_5G_BTN)
	if(!strcmp(APOn, WLAN_APOFF)){
#ifdef WSC_AP_SUPPORT
#if defined(TCSUPPORT_WPS_5G_BTN)
		wpsAction(path, APOn, WLAN5GSTOP);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
		wpsAction(path, APOn, WLANSTOP);
#endif
#endif
#endif
	}else if(!strcmp(APOn, WLAN_APON)){
#ifdef WSC_AP_SUPPORT
#if defined(TCSUPPORT_WPS_5G_BTN)
		wpsAction(path, APOn, WPS5GSTART);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
		if((0 == wps_both_start )&&(0 == isWPSRunning())){
			wpsAction(path, APOn, WPS5GSTART);
		}else{
			wps5g_done = 1;
		}
#endif
#endif
#endif
	}	
#endif
	memset(&wlan_info, 0, sizeof(WLan_info));
	memset(&wlan_info_other, 0, sizeof(WLan_info));
#ifdef TCSUPPORT_WLAN_WDS
	wlan11ac_wds_execute(&isWDSReloadWIFI, &WdsActive);
	wlan_info.wdsActive = WdsActive;
#endif
	wlan_info.type 	= WIFI_5G;
	wlan_info.oldNum = pre_sys_state.wlan11ac_Bssid_num;
	wlan_info.newNum = atoi(BssidNum);
	wlan_info.index = getWlanSSIDEnableNum(WIFI_5G, atoi(BssidNum));
#ifdef TCSUPPORT_CT_WLAN_JOYME3
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "Action", samessidAction, sizeof(samessidAction)) > 0 ) {
		if (!strcmp(samessidAction,"set"))
			SetSameSSIDandKey(samessidAction,"wlan");  
	}
#endif
#ifdef TCSUPPORT_WLAN_LED_BY_SW
	wlan_led_action();
#endif
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", BssidNum_11n, sizeof(BssidNum_11n)) < 0)
		return CFG2_RET_FAIL;

#ifdef TCSUPPORT_WLAN_WDS
	wlan11ac_wds_execute(&isWDSReloadWIFI_other, &WdsActive_other);
	wlan_info.wdsActive = WdsActive_other;
#endif	
	wlan_info_other.type	= WIFI_2_4G;
	wlan_info_other.oldNum = pre_sys_state.wlan_Bssid_num;
	wlan_info_other.newNum = atoi(BssidNum_11n);
	wlan_info_other.index = getWlanSSIDEnableNum(WIFI_2_4G, atoi(BssidNum_11n));
	wifimgr_lib_reboot(WIFI_5G, &(pre_sys_state.wlan11ac_Bssid_num), atoi(BssidNum), MAX_WDS_ENTRY, BssidNum_11n, "BssidNum");
	
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "WHNAT", whnat, sizeof(whnat)) > 0 )
	{
		iWhnat = atoi(whnat);
	}
	wifimgr_lib_reboot(WIFI_5G, &(pre_sys_state.wlan11ac_whnat), iWhnat, MAX_WDS_ENTRY, BssidNum_11n, "WHNAT");

#ifdef TCSUPPORT_WLAN_WDS
	if(0 == wifimgr_lib_get_11AC_CMDflag() ||
		(atoi(BssidNum) != pre_sys_state.wlan11ac_Bssid_num) ||
		(isWDSReloadWIFI == 1))
#else
	if(0 == wifimgr_lib_get_11AC_CMDflag() ||
		atoi(BssidNum) != pre_sys_state.wlan11ac_Bssid_num)
#endif
	{
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_BSSIDNUM_CHANGE, nodeacid, WiFi5gResetFlag);
#else
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_BSSIDNUM_CHANGE);
#endif
		pre_sys_state.wlan11ac_Bssid_num = atoi(BssidNum);
	}
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "Channel", channel, sizeof(channel)) > 0){
		if((atoi(channel)==0)&&(pre_sys_state.wlan11ac_chann!=0) ){
			snprintf(flag, sizeof(flag), "1");
		}
		pre_sys_state.wlan11ac_chann = atoi(channel);
	}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_INIT, nodeacid, WiFi5gResetFlag);
#else
	wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_INIT);
#endif

	if(cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", flag, sizeof(flag)) <= 0){
		return CFG2_RET_FAIL;
	}

	if(!strcmp(APOn, WLAN_APOFF)){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_DOWN, nodeacid, WiFi5gResetFlag);
#else
		wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_DOWN);
#endif
#ifdef WSC_AP_SUPPORT
		if(cfg_get_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", tmp, sizeof(tmp)) > 0){
			if(1 == atoi(tmp)){
				cfg_set_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", "0");
			}
		}
#endif
	}
	else if(!strcmp(APOn, WLAN_APON)){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		ret = wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_UP, nodeacid, WiFi5gResetFlag);
#else
		ret = wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, WIFI_INTERFACE_UP);
#endif
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
			fp = fopen(SVC_WLAN_ANT_PATH, "r");
			if(fp != NULL){
				fclose(fp);
				system(SVC_WLAN_ANT_PATH);
			}
#endif
		/*start all interface */
		if (ret == 1)
		{
			/*start all interface */
			start11acWifiInterface(path, atoi(BssidNum));
		}
#ifdef TCSUPPORT_MTD_ENCHANCEMENT
		checkWriteBinToFlash();
#endif
		dowscd();
		/*execute WLAN setup script*/
		fp = fopen(SVC_WLAN11AC_SCRIPT_PATH, "r");
		if(fp != NULL){
 			fclose(fp);
			wifimgr_lib_execute_wlan_script(1, SVC_WLAN11AC_SCRIPT_PATH);
		}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		if (0 == WiFi5gResetFlag)
		{
			fp = fopen(SVC_WLAN11AC_ENTRY_ATTR_SCRIPT_PATH, "r");
			if(fp != NULL){
				fclose(fp);
				system(SVC_WLAN11AC_ENTRY_ATTR_SCRIPT_PATH);
			}
		}
#endif

#if defined(TCSUPPORT_WLAN_8021X) && defined(TCSUPPORT_WLAN_AC)	
		rtdot1xd_AC_stop();
        rtdot1xd_AC_start();
#endif
#if defined (TCSUPPORT_CT_JOYME4)	
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "WLanJoymeSet5G", WLanJoymeSet5G, sizeof(WLanJoymeSet5G)) > 0){
			if(!strcmp(WLanJoymeSet5G, "1")){
				tcdbg_printf("execute wlan_JoymeSet5G.sh\r\n");
				system("/usr/script/wlan_JoymeSet5G.sh &");
			}
		}
	
#endif
	}else{
		return CFG2_RET_FAIL;
	}
#ifdef TCSUPPORT_WLAN_BNDSTRG
	svc_wlan_Bndstrg_execute(WLAN_BNDSTRG_NODE);
#endif
//#if defined(TCSUPPORT_CT_WLAN_JOYME3)
//	svc_cfg_maclist_commit();
//#endif

#if defined(TCSUPPORT_NP_CMCC)
	WLanLEDSwitch(1, atoi(APOn));
#endif
	return CFG2_RET_SUCCESS;
}
#endif

int chtoi(char c)
{
	if('0' <= c && c <= '9')
		return c - '0';
	else if('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else
		return 0;
}
#ifdef TCSUPPORT_WLAN_VENDIE
int svc_cfg_boot_recorderxvsie(void)
{
	char nodeName1[48] = {0};
	char nodeName2[48] ={0};
	char buf1[64] = {0};
	char buf2[64] = {0};
	char buf3[64] = {0};
	char RecordNum[8] = {0};
	int RecordNumIdx = 10;
	int i,j;

	snprintf(nodeName2, sizeof(nodeName2), "root.proberxvsie.entry.1");
	for(i=0;i<3;i++)
	{
		RecordNumIdx = 10;
		snprintf(nodeName1, sizeof(nodeName1), "root.proberxvsie.entry.%d", i+1);
		cfg_get_object_attr(nodeName1, "RecordNum", RecordNum, sizeof(RecordNum));
		if (RecordNumIdx < atoi(RecordNum))
			RecordNumIdx = atoi(RecordNum);
		
		snprintf(nodeName2,sizeof(nodeName2),"root.wifiprobeinfo.entry.%d",i+1);
		for(j=0;j<RecordNumIdx;j++)
		{
			snprintf(buf1,sizeof(buf1),"BAND%d",j);
			snprintf(buf2,sizeof(buf2),"MAC%d",j);
			snprintf(buf3,sizeof(buf3),"VSIE%d",j);
			cfg_set_object_attr(nodeName2,buf1,"");
			cfg_set_object_attr(nodeName2,buf2,"");
			cfg_set_object_attr(nodeName2,buf3,"");
		}
	}
	return 0;
}

int svc_cfg_boot_recorderxvsie_new(void)
{
	char nodeName1[48] = {0};
	char nodeName2[48] ={0};
	char buf1[64] = {0};
	char buf2[64] = {0};
	char buf3[64] = {0};
	int i,j;
	char RecordNum[8] = {0};
	int RecordNumIdx = 10;

	cfg_obj_create_object("root.wifiprobeinfo.entry.1");
	cfg_obj_create_object("root.wifiprobeinfo.entry.2");
	cfg_obj_create_object("root.wifiprobeinfo.entry.3");
	
	for(i=0;i<3;i++)
	{
		RecordNumIdx = 10;
		snprintf(nodeName1, sizeof(nodeName1), "root.proberxvsie.entry.%d", i+1);
		cfg_get_object_attr(nodeName1, "RecordNum", RecordNum, sizeof(RecordNum));
		if (RecordNumIdx < atoi(RecordNum))
			RecordNumIdx = atoi(RecordNum);
		
		snprintf(nodeName2,sizeof(nodeName2),"root.wifiprobeinfo.entry.%d",i+1);
		for(j=0;j<RecordNumIdx;j++)
		{
			snprintf(buf1,sizeof(buf1),"BAND%d",j);
			snprintf(buf2,sizeof(buf2),"MAC%d",j);
			snprintf(buf3,sizeof(buf3),"VSIE%d",j);
			cfg_set_object_attr(nodeName2,buf1,"");
			cfg_set_object_attr(nodeName2,buf2,"");
			cfg_set_object_attr(nodeName2,buf3,"");
		}
	}
	return 0;
}

int svc_wlan_probevsie_execute(char* path)
{
	char OUI[3][10]= {0};
	char dstNode[30] = {0};
	char buf[3] = {0};
	char ProbeBand[2] = {0};
	int i,j;
	char data[20] 	= {0};
	char buftmp[4] = {0};
	int dataLen = 3;
	/*to get the total entry i*/
	for(i=0;i<3;i++){
		snprintf(dstNode,sizeof(dstNode),PROBERXVSIE_ENTRY_NODE,i+1);
		cfg_get_object_attr(dstNode, "OUI", OUI[i], sizeof(OUI[i]));
		// printf("dstNode[%d] = %s\n",i,dstNode);
		// printf("OUI[%d] = %s\n",i,OUI[i]);
		if(strlen(OUI[i])<=0){
			snprintf(buf,sizeof(buf),"%d",i);
			cfg_set_object_attr(PROBERXVSIE_COMMON_NODE,"CurrentEntryNum",buf);
			break;
		}	
	}
#ifndef TCSUPPORT_CT_UBUS
	/*for every entry */
	cfg_get_object_attr(PROBERXVSIE_COMMON_NODE, "Enable", buftmp, sizeof(buftmp));
	if(0 == atoi(buftmp))
		dataLen = 0;
#endif	
	// tcdbg_printf("sock open-------\n");
	for(j=0;j<i;j++){
		memset(dstNode,0,sizeof(dstNode));
		snprintf(dstNode,sizeof(dstNode),PROBERXVSIE_ENTRY_NODE,j+1);
		cfg_get_object_attr(dstNode, "OUI", OUI[j], sizeof(OUI[j]));
		memset(buf,0,sizeof(buf));
		buf[0] = chtoi(OUI[j][0]) * 16 + chtoi(OUI[j][1]);
		buf[1] = chtoi(OUI[j][2]) * 16 + chtoi(OUI[j][3]);
		buf[2] = chtoi(OUI[j][4]) * 16 + chtoi(OUI[j][5]);
		// tcdbg_printf("OUI = %s\n",OUI);
		snprintf(data,sizeof(data),"%c%c%c",buf[0],buf[1],buf[2]);

		cfg_get_object_attr(dstNode, "ProbeBand", ProbeBand, sizeof(ProbeBand));
		
		if(0 == atoi(ProbeBand) || 2 == atoi(ProbeBand)){
			wifimgr_lib_set_OUI_FILTER(0, data, dataLen);
		}
#if defined(TCSUPPORT_WLAN_AC)	
		if(1 == atoi(ProbeBand) || 2 == atoi(ProbeBand)){
			wifimgr_lib_set_OUI_FILTER(1, data, dataLen);
		}
#endif
	}
	svc_cfg_boot_recorderxvsie();
	return CFG2_RET_SUCCESS;
}

int getProbRespIndex(char * interface)
{
	char *p = NULL;
	int ret = -1;

	if(NULL == interface)
	{
		return -1;
	}

	if('\0' == interface[0])
	{
		return -1;
	}


	p = strrchr(interface, '.');
	if(p == NULL){
		return -1;
	}
	
	ret = atoi(p + 1);
	if(ret > 0)
	{
		return ret;
	}
	return 0;
}

int iedata_rm_dot(char *old_iedata, char *new_iedata)
{
	int i = 0, j = 0, iedata_len = 0;

	if (old_iedata == NULL || new_iedata == NULL)
		return -1;
	
	iedata_len = strlen(old_iedata);
	for (i = 0; i < iedata_len; i++) 
	{
		if (old_iedata[i] != ',')
			new_iedata[j++] = old_iedata[i];
	}
	new_iedata[iedata_len] = '\0';
	return 0;
}

int svc_wlan_proberesp_execute(char* path)
{
	int i,num,j,k = 0;
	char dstNode[30] = {0};
	char proverespvsieEntryPath[64] = {0};
	char proverxvsieEntryPath[64] = {0};
	char currentEntryNum[4] = {0};
	int entryidx = 0;
	char Band[2] = {0};
	char entry_iedata[252] = {0};
	char temp_entry_iedata[252] = {0};
	char old_iedata[1000] = {0};
	char new_iedata[1000] = {0};
	unsigned char tmp_stamac[24] = {0};
	unsigned char entry_stamac[6] = {0};
	vsie_probresp_set_cfg_t probesp_set;
	unsigned int iedataLength = 0;
	unsigned int length = 0;
	int ret = 0;
	char bssidNum[16] = {0};
	int ssidEnable = 0;
	int rfband = 0;
	
	memset(proverespvsieEntryPath,0,sizeof(proverespvsieEntryPath));
	memset(proverxvsieEntryPath,0,sizeof(proverxvsieEntryPath));
	
	/*add the entry,set the data to driver*/
	for(entryidx=0; entryidx < 3; entryidx++)
	{
		snprintf(proverespvsieEntryPath, sizeof(proverespvsieEntryPath),PROBERESPVSIE_ENTRY_NODE,entryidx+1);
		snprintf(proverxvsieEntryPath, sizeof(proverxvsieEntryPath),PROBERXVSIE_ENTRY_NODE,entryidx+1);

		/*if this entry doesn't exsit, skip */
		if (cfg_obj_query_object(proverespvsieEntryPath,NULL,NULL) <= 0)
		{
			tcdbg_printf("Entry.%d doesn't exsit, Skip...\r\n", entryidx+1);
			continue;
		}
		
		/*build the VSIE PROB RESPONSE SET DATA*/
		/*1. First get the iedata value*/
		/*a.get the old data which likes:11,11,11,11,11.........*/
		cfg_get_object_attr(proverespvsieEntryPath, "IEDATA", old_iedata, sizeof(old_iedata));
		/*b.delete the ','     11,11,11 ---->111111*/
		if(iedata_rm_dot(old_iedata, new_iedata) != 0)
		{
			tcdbg_printf("iedata rm dot fail\n");
		}
		length = strlen(new_iedata) / 2;
		for(k = 0;k < length; k++)
		{
			entry_iedata[k] = chtoi(new_iedata[k * 2]) * 16 + chtoi(new_iedata[k * 2 + 1]);
		}
		memcpy(probesp_set.IEDATA, entry_iedata, sizeof(entry_iedata));

		/*2.Second get the STA mac*/
		cfg_get_object_attr(proverespvsieEntryPath, "MAC", tmp_stamac, sizeof(tmp_stamac));
		/*change  the format from "00AABBCCDD:EE"to six bytes*/
		for(j = 0;j < 6; j++)
		{
			entry_stamac[j] = chtoi(tmp_stamac[j * 2]) * 16 + chtoi(tmp_stamac[j * 2 + 1]);
		}
		memcpy(probesp_set.stamac, entry_stamac, sizeof(entry_stamac));
		
		/*3.Third calcuate the iedata length*/
		iedataLength = length;
		probesp_set.length = iedataLength;

		/*4. fourth get the band from the "probe_rx_vsie"entry*/
		cfg_get_object_attr(proverxvsieEntryPath, "ProbeBand", Band, sizeof(Band));
		probesp_set.band  = atoi(Band);
		if (WIFI_2_4G == probesp_set.band)
		{
			cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", bssidNum, sizeof(bssidNum));
			rfband = 0;
		}
		else if (WIFI_5G == probesp_set.band || 2 == probesp_set.band)
		{
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", bssidNum, sizeof(bssidNum));
			rfband = 1;
		}
		
		ssidEnable = getWlanSSIDEnableNum(rfband, atoi(bssidNum));
		wifimgr_lib_proberesp_execute(probesp_set.band, &probesp_set, sizeof(probesp_set), ssidEnable);
	}
	return 0;
}

int svc_wlan_proberesp_delete(char *path)
{
	int j = 0;
	char proverespvsieEntryPath[64] = {0};
	char proverxvsieEntryPath[64] = {0};
	unsigned char tmp_stamac[24] = {0};
	unsigned char entry_stamac[6] = {0};
	vsie_probresp_del_cfg_t probesp_del;
	int idx = 0;
	int ret = 0;
	char Band[2] = {0};
	
	memset(proverespvsieEntryPath,0,sizeof(proverespvsieEntryPath));
	memset(proverxvsieEntryPath,0,sizeof(proverxvsieEntryPath));
	
	snprintf(proverespvsieEntryPath, sizeof(proverespvsieEntryPath),path);

	if(NULL == strstr(proverespvsieEntryPath,"root.proberespvsie.entry."))
	{
		tcdbg_printf("[%s][path:%s]Incorrect entry path,return\n",__FUNCTION__,proverespvsieEntryPath);
		return 0;
	}
	
	/*get the entry id*/	
	idx = getProbRespIndex(proverespvsieEntryPath);	
	snprintf(proverxvsieEntryPath,sizeof(proverxvsieEntryPath), PROBERESPVSIE_ENTRY_NODE, idx);
	
	/*build the VSIE PROB RESPONSE DELETE DATA*/
	/*1.First get the STA mac*/
	cfg_get_object_attr(proverespvsieEntryPath, "MAC", tmp_stamac, sizeof(tmp_stamac));
	/*change  the format from "00AABBCCDD:EE"to six bytes*/
	for(j = 0;j < 6; j++)
	{
		entry_stamac[j] = chtoi(tmp_stamac[j * 2]) * 16 + chtoi(tmp_stamac[j * 2 + 1]);
	}
	memcpy(probesp_del.stamac, entry_stamac, sizeof(entry_stamac));
	
	/*2. second get the band from the "probe_rx_vsie"entry*/
	cfg_get_object_attr(proverxvsieEntryPath, "ProbeBand", Band, sizeof(Band));
	probesp_del.band  = atoi(Band);
	wifimgr_lib_proberesp_delete(probesp_del.band, &probesp_del, sizeof(probesp_del));

	/*5.delete the data of romfile*/
	cfg_obj_delete_object(proverespvsieEntryPath);
	
	return 0;
	
}

#endif
#if defined(TCSUPPORT_NP)
static int cfg_apdebug_on(void)
{
	FILE* fp;
	if ((fp = fopen("/tmp/cfg_apdebug_on","r")) == NULL)
		return 0;
	fclose(fp);	
	return 1;
}
#endif
#if defined(TCSUPPORT_NP_CMCC)
#define WLAN_UPLINK_RECONNECT_TIMEOUT	60
int np_apcli_status_chk()
{
#if defined(TCSUPPORT_ANDLINK)
	static struct timeval lastTime;
	struct timeval curTime;
	static unsigned long oldRxBytes = 0;
	static unsigned long oldTxBytes = 0;
	static int count = 0;
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
	int currentStatus = 0, conn_timeout = 0, ret = 0, iswpsmode = 0, conn_cnt = 0;
	static int lastStatus = 0, needSwitch = 0, tryTimes = 0, wpsconnected = 0;
	int needSetSite = 0, reconnect_timeout = 0;
	int needrenew = 1;
	char tmpBuf[16] = {0}, param[16] = {0}, autoSelect[10] = {0};
	char SSID_24G[32] = {0}, SSID_5G[32] = {0};
	char connstatus[4] = {0};
	double rxRate_rt = 0,txRate_rt = 0;
	static struct timeval lastTime1;
	struct timeval curTime1;
	static int count1 = 0;
	static unsigned long long oldra0RxBytes = 0,oldra0TxBytes = 0;
	int rate_limit = 0;
#endif
	char cmd_value[64] = {0};

#if defined(TCSUPPORT_ANDLINK)
	if( 0 == count )
	{
		memset(&lastTime, 0, sizeof(struct timeval));
		count++;
	}

	gettimeofday(&curTime, NULL);
	getUplinkRate(lastTime, curTime, &oldRxBytes, &oldTxBytes);
	lastTime = curTime;
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, tmpBuf, sizeof(tmpBuf));
	iswpsmode = atoi(tmpBuf);
	memset(tmpBuf, 0, sizeof(tmpBuf));
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, tmpBuf, sizeof(tmpBuf));
	conn_cnt = atoi(tmpBuf);
	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	if( cfg_obj_get_object_attr(APCLI_COMMON_NODE, "StartConn", 0, tmpBuf, sizeof(tmpBuf)) > 0
		&& 1 == atoi(tmpBuf))
	{
		if( 0 == count1 )
		{
			memset(&lastTime1, 0, sizeof(struct timeval));
			count1++;
		}
		
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if(cfg_obj_get_object_attr(APCLI_COMMON_NODE, "Ra0RateLimit", 0, tmpBuf, sizeof(tmpBuf)) > 0)
			rate_limit = atoi(tmpBuf);
		else
			rate_limit = 512;
		
		gettimeofday(&curTime1, NULL);
		get_Info_Ra0_Rate(lastTime1,curTime1,&oldra0RxBytes, &oldra0TxBytes,&rxRate_rt,&txRate_rt);	
		lastTime1 = curTime1;

		if(rxRate_rt > rate_limit || txRate_rt > rate_limit)
		{	/*rxRate_rt > 512kbps  or txRate_rt > 512kbps ,stop scan*/
			return 0;
		}
		
		tryTimes = 0;
		lastStatus = 0;
		wpsconnected = 0;
		
		if( iswpsmode )
		{
			tcdbg_printf("\n%s():%d APClient(WPS) Mode Start!!!\n", __FUNCTION__, __LINE__);
			/* wps mode */
			svc_wlan_wps_connect_event();
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "1");
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, "0");
			cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", "3");
		}
		else
		{
			memset(tmpBuf, 0, sizeof(tmpBuf));
			cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", 0, tmpBuf, sizeof(tmpBuf));
			needSetSite = atoi(tmpBuf);
			/* apclient mode */
			if( needSetSite )
			{
				/* set site survey */
				if( 3 == needSetSite )
				{
#if defined(TCSUPPORT_NP)
					if (cfg_apdebug_on())
#endif						
					tcdbg_printf("\n%s():%d APClient set SiteSurvey!!!\n", __FUNCTION__, __LINE__);				
					cfg_obj_get_object_attr(APCLI_ENTRY0_NODE, "SSID", 0, SSID_24G, sizeof(SSID_24G));
					cfg_obj_get_object_attr(APCLI_ENTRY1_NODE, "SSID", 0, SSID_5G, sizeof(SSID_5G));
					wifimgr_lib_set_SiteSurvery(0, SSID_24G);
					wifimgr_lib_set_SiteSurvery(1, SSID_5G);
					cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "0");
				}
				memset(tmpBuf, 0, sizeof(tmpBuf));
				snprintf(tmpBuf, sizeof(tmpBuf), "%d", needSetSite - 1);
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", tmpBuf);
				return 0;
			}
			else
			{
				/* get site survey and set connect info if scan target SSID success */
				ret = svc_wlan_apcli_connect_event();
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", "3");

				if ( ret < 0 )
					return 0;
				tcdbg_printf("\n%s():%d APClient Mode Start!!!\n", __FUNCTION__, __LINE__);
				cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "1");
			}
		}
	}

	if(cfg_obj_get_object_attr(APCLI_COMMON_NODE, "ReconnectTimeout", 0, tmpBuf, sizeof(tmpBuf)) > 0
		&& 0 != tmpBuf[0])
		reconnect_timeout = atoi(tmpBuf);
	else
		reconnect_timeout = WLAN_UPLINK_RECONNECT_TIMEOUT;
	
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, connstatus, sizeof(connstatus));
	if( cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", 0, tmpBuf, sizeof(tmpBuf)) > 0
		&& !strcmp(tmpBuf, "APClient")
		&& 0 == strcmp(connstatus, "1") )
	{		
		if( iswpsmode )
		{
			/* check wps connect status */
			currentStatus = np_wlan_apcli_connstatus();

			if( lastStatus && 0 == currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli(WPS) connStatus turn DOWN!!!\n", __FUNCTION__, __LINE__);
				/* send message to release ip */
				strncpy(param, "APClient", sizeof(param) - 1);
				cfg_send_event(EVT_WAN_INTERNAL, EVT_XPON_DOWN, param, sizeof(param));
			}
			else if( 0 == lastStatus && currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli(WPS) connStatus turn UP!!!\n", __FUNCTION__, __LINE__);
				wpsconnected++;
				tryTimes = 0;
				
				/* send message to renew ip */
				strncpy(param, "APClient", sizeof(param) - 1);
				cfg_send_event(EVT_WAN_INTERNAL, EVT_XPON_UP, param, sizeof(param));
			}
			else if( 0 == lastStatus && 0 == currentStatus )
			{
				tryTimes++;
			}
			else /* 1 == lastStatus && 1 == currentStatus */
			{
				if ( 0 == conn_cnt )
				{
					if ( 0 == np_wlan_set_wps_profile() )
					{
						tcdbg_printf("%s():%d set WPS Profile OK!\n", __FUNCTION__, __LINE__);
						memset(tmpBuf, 0, sizeof(tmpBuf));
						snprintf(tmpBuf, sizeof(tmpBuf), "%d", conn_cnt + 1);
						cfg_set_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", tmpBuf);
						cfg_evt_write_romfile_to_flash();
					}
				}
			}

			if( 0 == wpsconnected )
			{
				if( tryTimes > 120 )
				{
					tcdbg_printf("\n%s():%d APCli(WPS) connect timeout!!!\n", __FUNCTION__, __LINE__);
					/* wps connect timeout, change to auto connect mode */
					cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, "0");
					cfg_commit_object(APWANINFO_COMMON_NODE);
					tryTimes = 0;
				}
			}
			else
			{
				if( tryTimes > reconnect_timeout )
				{
					cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, tmpBuf, sizeof(tmpBuf));
					if( !strcmp(tmpBuf, "1") )
					{
						/* wps disconnect and reconnect timeout, switch to wlan relay */
						cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, "0");
						cfg_commit_object(APWANINFO_COMMON_NODE);
						tryTimes = 0;
					}
				}
			}

			lastStatus = currentStatus;
		}
		else
		{
			/* check apclient connect status */
			cfg_obj_get_object_attr(APCLI_COMMON_NODE, "APClientUpTimeout", 0, tmpBuf, sizeof(tmpBuf));
			conn_timeout = atoi(tmpBuf);
			
			cfg_obj_get_object_attr(APCLI_COMMON_NODE, "AutoSelect", 0, autoSelect, sizeof(autoSelect));
			if( !strcmp(autoSelect, "1") )
			{
				ret = np_check_radio_signal();
				if( ret )
					needSwitch = ret;
			}
			currentStatus = np_wlan_apcli_connstatus();

			if( currentStatus )
			{
				snprintf(param, sizeof(param), "0");
				if( 1 == needSwitch )
				{
					wifimgr_lib_set_WIFI_CMD("apclii0", -1, sizeof(cmd_value), "ApCliEnable=0", cmd_value, 0);
					system(cmd_value); 
					cfg_send_event(EVT_WAN_INTERNAL, EVT_WAN_AP_LINK_UPDATE, param, sizeof(param));
					needSwitch = 0;
					needrenew = 0;
				}
				else if( 2 == needSwitch )
				{
					wifimgr_lib_set_WIFI_CMD("apcli0", -1, sizeof(cmd_value), "ApCliEnable=0", cmd_value, 0);
					system(cmd_value);
					cfg_send_event(EVT_WAN_INTERNAL, EVT_WAN_AP_LINK_UPDATE, param, sizeof(param));
					needSwitch = 0;
					needrenew = 0;
				}
				else
				{
					/* do nothing */
				}
			}

			if( lastStatus && 0 == currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli connStatus turn DOWN!!!\n", __FUNCTION__, __LINE__);
				/* send message to release ip */
				if( 0 == needSwitch )
				{
					strncpy(param, "APClient", sizeof(param) - 1);
					cfg_send_event(EVT_WAN_INTERNAL, EVT_XPON_DOWN, param, sizeof(param));
				}
			}
			else if( 0 == lastStatus && currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli connStatus turn UP!!!\n", __FUNCTION__, __LINE__);
				tryTimes = 0;
				cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "1");
				/* send message to renew ip */
				if( needrenew )
				{
					strncpy(param, "APClient", sizeof(param) - 1);
					cfg_send_event(EVT_WAN_INTERNAL, EVT_XPON_UP, param, sizeof(param));
				}
			}
			else if( 0 == lastStatus && 0 == currentStatus )
			{
				tryTimes++;
			}
			
			if( 0 != conn_timeout )
			{
				tcdbg_printf("\n%s():%d APCli connect timeout, Stop APCli!!!\n", __FUNCTION__, __LINE__);
				/* apclient connect timeout, stop connect */
				if( tryTimes > conn_timeout )
				{
					svc_wlan_apcli_stop();
					tryTimes = 0;
				}
			}
			else
			{
				if( tryTimes > reconnect_timeout )
				{
					/* retry wlan relay */
					cfg_commit_object(APWANINFO_COMMON_NODE);
					tryTimes = 0;
				}
			}

			lastStatus = currentStatus;
		}
	}		
#endif
	return 0;
}
#else
#if defined(TCSUPPORT_CT_UBUS)
#define WLAN_UPLINK_RECONNECT_TIMEOUT	60
int np_apcli_status_chk()
{
	static struct timeval lastTime;
	struct timeval curTime;
	static unsigned long oldRxBytes = 0;
	static unsigned long oldTxBytes = 0;
	static int count = 0;
	static struct timeval lastTime1;
	struct timeval curTime1;
	double rxRate_rt = 0,txRate_rt = 0;
	static unsigned long long oldra0RxBytes = 0,oldra0TxBytes = 0;
#if defined(TCSUPPORT_WLAN_APCLIENT)
	int currentStatus = 0, conn_timeout = 0, ret = 0, iswpsmode = 0, conn_cnt = 0;
	static int lastStatus = 0, needSwitch = 0, tryTimes = 0, wpsconnected = 0;
	int needSetSite = 0, reconnect_timeout = 0;
	int needrenew = 1;
	char tmpBuf[16] = {0}, param[16] = {0}, autoSelect[10] = {0};
	char SSID_24G[32] = {0}, SSID_5G[32] = {0};
	char connstatus[4] = {0};
	int rate_limit = 0;
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, tmpBuf, sizeof(tmpBuf));
	iswpsmode = atoi(tmpBuf);
	memset(tmpBuf, 0, sizeof(tmpBuf));
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, tmpBuf, sizeof(tmpBuf));
	conn_cnt = atoi(tmpBuf);
	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	if( cfg_obj_get_object_attr(APCLI_COMMON_NODE, "StartConn", 0, tmpBuf, sizeof(tmpBuf)) > 0
		&& 1 == atoi(tmpBuf))
	{		
		tryTimes = 0;
		lastStatus = 0;
		wpsconnected = 0;
		
		if( iswpsmode )
		{
			/* wps mode */
			svc_wlan_wps_connect_event();
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "1");
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, "0");
			cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", "3");
		}
		else
		{
			memset(tmpBuf, 0, sizeof(tmpBuf));
			cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", 0, tmpBuf, sizeof(tmpBuf));
			needSetSite = atoi(tmpBuf);
			/* apclient mode */
			if( needSetSite )
			{
				/* set site survey */
				if( 3 == needSetSite )
				{
					tcdbg_printf("\n%s():%d APClient set SiteSurvey!!!\n", __FUNCTION__, __LINE__);
					cfg_obj_get_object_attr(APCLI_ENTRY0_NODE, "SSID", 0, SSID_24G, sizeof(SSID_24G));
					cfg_obj_get_object_attr(APCLI_ENTRY1_NODE, "SSID", 0, SSID_5G, sizeof(SSID_5G));
					wifimgr_lib_set_SiteSurvery(0, SSID_24G);
					wifimgr_lib_set_SiteSurvery(1, SSID_5G);
					cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "0");
				}
				memset(tmpBuf, 0, sizeof(tmpBuf));
				snprintf(tmpBuf, sizeof(tmpBuf), "%d", needSetSite - 1);
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", tmpBuf);
				return 0;
			}
			else
			{
				/* get site survey and set connect info if scan target SSID success */
				ret = svc_wlan_apcli_connect_event();
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "needSetSite", "3");

				if ( ret < 0 )
					return 0;
				tcdbg_printf("\n%s():%d APClient Mode Start!!!\n", __FUNCTION__, __LINE__);
				cfg_obj_set_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, "1");
			}
		}
	}

	if(cfg_obj_get_object_attr(APCLI_COMMON_NODE, "ReconnectTimeout", 0, tmpBuf, sizeof(tmpBuf)) > 0
		&& 0 != tmpBuf[0])
		reconnect_timeout = atoi(tmpBuf);
	else
		reconnect_timeout = WLAN_UPLINK_RECONNECT_TIMEOUT;
	
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "ConnStatus", 0, connstatus, sizeof(connstatus));
	if(  0 == strcmp(connstatus, "1") )
	{		
		if( iswpsmode )
		{
			/* check wps connect status */
			currentStatus = np_wlan_apcli_connstatus();

			if( lastStatus && 0 == currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli(WPS) connStatus turn DOWN!!!\n", __FUNCTION__, __LINE__);
				/* send message to release ip */
				memset(param, 0, sizeof(param));
				strncpy(param, "down", sizeof(param) - 1);
				cfg_set_object_attr(WANINFO_COMMON_NODE, "APClientState", "down");
				cfg_send_event(EVT_PON_EXTERNAL, EVT_APCLI_WAN_STATUS, (void *)(param), strlen(param));
			}
			else if( 0 == lastStatus && currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli(WPS) connStatus turn UP!!!\n", __FUNCTION__, __LINE__);
				wpsconnected++;
				tryTimes = 0;
				
				/* send message to renew ip */
				memset(param, 0, sizeof(param));
				strncpy(param, "up", sizeof(param) - 1);
				cfg_set_object_attr(WANINFO_COMMON_NODE, "APClientState", "up");
				cfg_send_event(EVT_PON_EXTERNAL, EVT_APCLI_WAN_STATUS, (void *)(param), strlen(param));
			}
			else if( 0 == lastStatus && 0 == currentStatus )
			{
				tryTimes++;
			}
			else /* 1 == lastStatus && 1 == currentStatus */
			{
				if ( 0 == conn_cnt )
				{
					if ( 0 == np_wlan_set_wps_profile() )
					{
						tcdbg_printf("%s():%d set WPS Profile OK!\n", __FUNCTION__, __LINE__);
						memset(tmpBuf, 0, sizeof(tmpBuf));
						snprintf(tmpBuf, sizeof(tmpBuf), "%d", conn_cnt + 1);
						cfg_set_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", tmpBuf);
						cfg_evt_write_romfile_to_flash();
					}
				}
			}

			if( 0 == wpsconnected )
			{
				if( tryTimes > 120 )
				{
					tcdbg_printf("\n%s():%d APCli(WPS) connect timeout!!!\n", __FUNCTION__, __LINE__);
					/* wps connect timeout, change to auto connect mode */
					cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, "0");
					tryTimes = 0;
				}
			}
			else
			{
				if( tryTimes > reconnect_timeout )
				{
					cfg_obj_get_object_attr(APCLI_COMMON_NODE, "WPSConnCnt", 0, tmpBuf, sizeof(tmpBuf));
					if( !strcmp(tmpBuf, "1") )
					{
						/* wps disconnect and reconnect timeout, switch to wlan relay */
						cfg_obj_set_object_attr(APCLI_COMMON_NODE, "WPSMode", 0, "0");
						tryTimes = 0;
					}
				}
			}

			lastStatus = currentStatus;
		}
		else
		{
			/* check apclient connect status */
			cfg_obj_get_object_attr(APCLI_COMMON_NODE, "APClientUpTimeout", 0, tmpBuf, sizeof(tmpBuf));
			conn_timeout = atoi(tmpBuf);

			currentStatus = np_wlan_apcli_connstatus();

			if( lastStatus && 0 == currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli connStatus turn DOWN!!!\n", __FUNCTION__, __LINE__);
				memset(param, 0, sizeof(param));
				strncpy(param, "down", sizeof(param) - 1);
				cfg_set_object_attr(WANINFO_COMMON_NODE, "APClientState", "down");
				cfg_send_event(EVT_PON_EXTERNAL, EVT_APCLI_WAN_STATUS, (void *)(param), strlen(param));
			}
			else if( 0 == lastStatus && currentStatus )
			{
				tcdbg_printf("\n%s():%d APCli connStatus turn UP!!!\n", __FUNCTION__, __LINE__);
				tryTimes = 0;
				cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "1");
				memset(param, 0, sizeof(param));
				strncpy(param, "up", sizeof(param) - 1);
				cfg_set_object_attr(WANINFO_COMMON_NODE, "APClientState", "up");
				cfg_send_event(EVT_PON_EXTERNAL, EVT_APCLI_WAN_STATUS, (void *)(param), strlen(param));
			}
			else if( 0 == lastStatus && 0 == currentStatus )
			{
				tryTimes++;
			}
			
			if( 0 != conn_timeout )
			{
				tcdbg_printf("\n%s():%d APCli connect timeout, Stop APCli!!!\n", __FUNCTION__, __LINE__);
				/* apclient connect timeout, stop connect */
				if( tryTimes > conn_timeout )
				{
					svc_wlan_apcli_stop();
					tryTimes = 0;
				}
			}
			else
			{
				if( tryTimes > reconnect_timeout )
				{
					svc_wlan_apcli_stop();
					tryTimes = 0;
				}
			}

			lastStatus = currentStatus;
		}
	}		
#endif
	return 0;
}
#endif
#endif
#if defined(TCSUPPORT_MAP_R2)
int mapd_pid_check(void)
{
	int mapd_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};

	rv = find_pid_by_name( "mapd", pid_t, &pidnum);
	if( 0 == rv 
		&& 1 == pidnum )
		mapd_pid = pid_t[0];

	return mapd_pid;
}

int _1905_pid_check(void)
{
	int _1905_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};

	rv = find_pid_by_name( "p1905_managerd", pid_t, &pidnum);
	if( 0 == rv 
		&& 1 == pidnum )
		_1905_pid = pid_t[0];
	
	return _1905_pid;
}

int wappp_pid_check(void)
{
	int wappp_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};

	rv = find_pid_by_name( "wapp", pid_t, &pidnum);
	if( 0 == rv 
		&& 2 == pidnum )
		wappp_pid = pid_t[0] & pid_t[1];

	return wappp_pid;
}

int wapp_socket_file_check(void)
{

	if (access("/tmp/wapp_ctrl", F_OK) != 0)
	{
		tcdbg_printf("/tmp/wapp_ctrl does not exist.\n");
		return 0;
	}

	if (access("/tmp/wapp_server", F_OK) != 0)
	{
		tcdbg_printf("/tmp/wapp_server does not exist.\n");
		return 0;
	}

	return 1;
}

void map_daemon_check()
{
	char tmp[16] = {0};
	int map_datenable = 0;
	int map_restart = 0;
	int process_exist = 0;
	char param[16] = {0};

	cfg_get_object_attr(MESH_DAT_NODE, "MapEnable", tmp, sizeof(tmp));
	map_datenable = atoi(tmp);
	memset(tmp, 0, sizeof(tmp));
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_RESTART_FLAG, tmp, sizeof(tmp));
	map_restart = atoi(tmp);

	if(map_datenable == 0 || map_restart == 1)
	{
		return;
	}

    if (mapd_pid_check() == 0)
	{
		tcdbg_printf("\r\t====mesh mapd fail===\n");
		process_exist++;
	}
	if (_1905_pid_check() == 0)
	{
		tcdbg_printf("\r\t====mesh 1905 fail===\n");
		process_exist++;
	}

    if (wappp_pid_check() == 0 || wapp_socket_file_check() == 0)
	{
		tcdbg_printf("\r\t====mesh wapp fail===\n");
		process_exist++;
	}
    
	if (0 != process_exist)
	{
		tcdbg_printf("\r\t====mesh restart===\n");
		/* only process exist and exit,then can reopen the process again*/
		memset(param, 0, sizeof(param));
		strncpy(param, "root.mesh", sizeof(param) - 1);
		cfg_send_event(EVT_CENTRAL_CTRL_EXTERNAL, EVT_CC_TRIGGER_MESH_REINIT, (void *)(param), strlen(param));
	}
	return;
}

#endif

void wlan_cc(void* unused)
{
	char buffer[8] 		= {0};
#ifdef TCSUPPORT_WLAN_AC
	char buffer_ac[8] 	= {0};
#endif
	int count			= 0;
#ifdef TCSUPPORT_WLAN_BNDSTRG
	int bndonce = 0;
#endif
	char tmp[8]			= {0};
	struct timespec curtime;
	int cnt = -1;
#if defined (TCSUPPORT_CT_JOYME4)
	int loadWlanSetCount = 0;
	int loadWlanSetCount5G = 0;
	char WLanJoymeSet[8] = {0};
	char WLanJoymeSet5G[8] = {0};
#endif
	int ret = 0;
#if defined(TCSUPPORT_MAP_R2)
	int meshcount = 110 ;
#endif

	while(1){
		sleep(1);

		if(checkCFGload() == 0){
			continue;
		}

		if(0 == count){
			clock_gettime(CLOCK_MONOTONIC, &curtime);
			memset(buffer, 0, sizeof(buffer));
			cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "WanStatus", buffer, sizeof(buffer));
			if(curtime.tv_sec < 60 && atoi(buffer) != 1)					
			{
				continue;
			}


			if(curtime.tv_sec < 60 && cnt < 0)					
			{
				cnt = 5;   /*wan is up, delay 5s to load wifi*/
			}

			
			
			if(cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanBoot", buffer, sizeof(buffer)) > 0 
			&& strcmp(buffer, "1") == 0			
#ifdef TCSUPPORT_WLAN_AC
			&& cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanBoot_ac", buffer_ac, sizeof(buffer_ac)) > 0 
			&& strcmp(buffer_ac, "1") == 0				
#endif
			)
			{
				/*if(curtime.tv_sec < 40)					
				{
					continue;
				}*/

				if(cnt > 0){
					cnt--;
					continue;
				}
				system("taskset 4 /usr/script/wlan_load.sh &");
				count ++;					
				pre_sys_state_init();
			}
		}
#if defined (TCSUPPORT_CT_JOYME4)
		if (loadWlanSetCount == 0){
			clock_gettime(CLOCK_MONOTONIC, &curtime);
			if(curtime.tv_sec > 90){ 	
				if(cfg_get_object_attr(WLAN_COMMON_NODE, "WLanJoymeSet", WLanJoymeSet, sizeof(WLanJoymeSet)) > 0){
					if(!strcmp(WLanJoymeSet, "1")){
						tcdbg_printf("execute wlan_JoymeSet.sh\r\n");
						system("/usr/script/wlan_JoymeSet.sh &");
						loadWlanSetCount++;
					}
				}
			}
		}
		if (loadWlanSetCount5G == 0){
			clock_gettime(CLOCK_MONOTONIC, &curtime);
			if(curtime.tv_sec > 90){ 
				if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "WLanJoymeSet5G", WLanJoymeSet5G, sizeof(WLanJoymeSet5G)) > 0){
					if(!strcmp(WLanJoymeSet5G, "1")){
						tcdbg_printf("execute wlan_JoymeSet_5G.sh\r\n");
						system("/usr/script/wlan_JoymeSet5G.sh &");
						loadWlanSetCount5G++;
					}
				}
			}
		}


#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
		guest_cc();
#endif	
		if(cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", tmp, sizeof(tmp)) <= 0
			|| strcmp(tmp, "1") != 0){
			continue;
		}
#ifdef TCSUPPORT_WLAN
		check_wifi();
		checkWlanBtn();
#ifdef TCSUPPORT_WLAN_AC
		checkWlan11acBtn();
#endif
#endif

#ifdef WSC_AP_SUPPORT
		check_wpsbtn();
#if defined(TCSUPPORT_WPS_5G_BTN)	
		check_wps5gbtn();
#endif
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
		clr_wps_flag();
#endif
#endif
#if defined(TCSUPPORT_CT_BUTTONDETECT)
		check_wlan_wps_button();
#endif
#if defined(TCSUPPORT_CT_JOYME)
#if !defined(TCSUPPORT_CMCCV2)
		check_wpsstatus();
#endif
#endif
#if defined(TCSUPPORT_CT_PON)
#ifdef WSC_AP_SUPPORT	
		wlanNodeUpdate();	  
#endif
		if(0 == wifimgr_lib_get_ixia5gflag())
		{
#if !defined(TCSUPPORT_ANDLINK)
			wifiMacTab_update();
#endif
		}
		check_wifi_mac_entry();
#endif
		ret = wifimgr_lib_check_wifi_ixia_test();
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
		//beaconVSIE_duration_check();
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
		if(bndonce == 0){
			bndonce++;
			Bndstrg_check_wlanload();
		}
#endif
#if defined(TCSUPPORT_MAP_R2)
	/*only wifi up 100s to check,and 10s a loop*/
	meshcount--;
	if(meshcount <= 10)
	{
		meshcount = 20;
		map_daemon_check();
	}	
#endif

#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
		np_apcli_status_chk();
#endif
	}
	return ;
}
#ifdef TCSUPPORT_CT_WLAN_JOYME3
void wlan_MonitorTask(void* unused)
{
	
	int i = 0,j = 0;
	int stationNum = 0;
	char channel[4] = {0};
	char tmp[20] = {0};
	char mac_addr[20] = {0};
	char interfacebuf[10] = {0};
	int rssi = 0;
	char errdest[50];
	int mixmethod = MIXMODE;
	int ret = 0;
	int isconnecttoap = 0;
	
	tcdbg_printf("enter wlan_MonitorTask in.\n");
	
	ret =sem_init(&SEM_CCRSSI,0,0);	
	while(1){	
		ret = sem_wait(&SEM_CCRSSI);
		
		if((ret != 0) && (errno == EINTR))
		{
			continue;
		}
		tcdbg_printf("enter wlan_MonitorTask GO.\n");
		stationNum = GetStationNum();
		
		if (stationNum > MAX_NUM_OF_MONITOR_STA){	
			tcdbg_printf("wlan_MonitorTask mixmode only support 16 station GO.\n");
			continue;
			}
		
		for	(i=0;i<stationNum;i++){
				
			if	(GetMixmodeNodeValue(mac_addr,channel,i)){
				
				SetMimodeInterface(channel,interfacebuf);					
					
				/*if the station connect to own ap,not run airmonitor*/
				isconnecttoap = ChenckMacTableConnect(mac_addr,&rssi);
				if(isconnecttoap==1){
					tcdbg_printf("wlan_MonitorTask the station connec to own ap.\n");
					SendRssiValueMsg(rssi,mac_addr,interfacebuf,i,errdest);
					continue;
					}
				/********************* run airmonitor************/
					mixmethod = GetMixMethod(interfacebuf);
					
				for (j=0;j<4;j++){/*mix mode try only 4 times*/
						
					if (MIXMODE == mixmethod){
						MixmodeSetRssi(mac_addr,interfacebuf,i,channel);			
						sleep(1)/*waitting for get rssi*/;
						rssi = MixmodeGetRssi(mac_addr,interfacebuf);
						}
					else{
						AirMonitorSetRssi(mac_addr,interfacebuf,i,channel);
						sleep(1)/*waitting for get rssi*/;
						rssi = AirMonitorGetRssi(mac_addr,interfacebuf);
						}
				
					if	(rssi!=-127 && rssi!=0){
						SendRssiValueMsg(rssi,mac_addr,interfacebuf,i,errdest);	
						break;
						}
					else
						usleep(500000);	
				}						
			}	
			if	(j==4){
				snprintf(errdest,sizeof(errdest),"%s","get invalided RSSI");
				SendRssiValueMsg(rssi,mac_addr,interfacebuf,i,errdest);	
				memset(errdest, 0, sizeof(errdest));
				}					
		}
	}
	

	return;	
				
}
#endif
/*add----------------------------------*/
#ifdef TCSUPPORT_CT_WLAN_JOYME3
static void device_event_stop(struct device_context *dctx,
							  struct nlmsghdr *h)
{
	close(dctx->event_sock);	
}

void wlan_driver_event_user(void)
{
	sleep(20);
	struct sockaddr_nl local;	
    device_context.event_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (device_context.event_sock < 0) {
        perror("socket(PF_NETLINK,SOCK_RAW,NETLINK_ROUTE)");
        return 0;
    }
	
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups = RTMGRP_LINK;
    if (bind(device_context.event_sock, (struct sockaddr *) &local, sizeof(local)) < 0) {
        perror("bind(netlink)");
        close(device_context.event_sock);
        return 0;
    }
	
    do {
        char buf[8192];
        int left;
        struct sockaddr_nl from;
        socklen_t fromlen;
        struct nlmsghdr *h;
        int sock = device_context.event_sock;
        struct device_context *dctx = &device_context;
        int max_events = 10;
try_again:
        fromlen = sizeof(from);
        left = recvfrom(sock, (void *)buf, sizeof(buf), 0/* MSG_DONTWAIT */,
			(struct sockaddr *) &from, &fromlen);
        if (left < 0) {
            perror("recvfrom(netlink)");
            return 0;
        }
		
        h = (struct nlmsghdr *) buf;
		
		while (left >= (int) sizeof(*h)) {
            int len, plen;
			
            len = h->nlmsg_len;
            plen = len - sizeof(*h);
            if (len > left || plen < 0) {
                break;
            }
			
            switch (h->nlmsg_type) {
            case RTM_NEWLINK:
                device_event_hook(dctx, h, plen);
                break;
            }
			
            h = NLMSG_NEXT(h, left);
        }
		
        if (left > 0) {
            device_printf("%d extra bytes in the end of netlink message\n", left);
        }
		
        if (max_events > 1) {
            max_events = max_events - 1;
            goto try_again;
        }
        goto try_again;
    } while (0) ;
}
static void device_event_hook(struct device_context *dctx,
							  struct nlmsghdr *h, size_t len)
{
    struct ifinfomsg *ifi;
    int attrlen, nlmsg_len, rta_len;
    struct rtattr * attr;
	
    ifi = NLMSG_DATA(h);
	
    nlmsg_len = NLMSG_ALIGN(sizeof(struct ifinfomsg));
	
    attrlen = NLMSG_PAYLOAD(h, sizeof(struct ifinfomsg));
    
    if (attrlen < 0)
        return;
    attr = (struct rtattr *) ((unsigned char*)((NLMSG_DATA(h) + nlmsg_len)));
    rta_len = RTA_ALIGN(sizeof(struct rtattr));
    while (RTA_OK(attr, attrlen)) {
        if (attr->rta_type == IFLA_WIRELESS) {
            device_user_event_wireless(
                dctx, ((char *) attr) + rta_len,
                attr->rta_len - rta_len);
        } 
        attr = RTA_NEXT(attr, attrlen);
    }
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
void rsp_hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen,int* channel,int* rcpi,char bssid[])
{
	unsigned char *pt;
	int x;
	
	pt = pSrcBufVA;
	printf("%s: %p, len = %d\n",str,  pSrcBufVA, SrcBufLen);
	
	*rcpi = pt[13];
	*channel = pt[1];
	#if 0
	for (x=0; x<SrcBufLen; x++) {
		if (x % 16 == 0)
			printf("0x%04x : ", x);
		printf("%02x ", ((unsigned char)pt[x]));
		if (x%16 == 15) printf("\n");
	}
        #endif
	snprintf(bssid,18,"%02X%02X%02X%02X%02X%02X",pt[15],pt[16],pt[17],pt[18],pt[19],pt[20]);
	printf("\n");
}

int event_bcn_rep (struct rrm_event_s *rrm_event_data)
{
	p_bcn_rsp_data_t data = NULL;
	unsigned char* rcpi = NULL;
	char bssid[18]={0};
	char RSSI;
	char temp[20] = {0};
	char temp1[20] = {0};
	char temp2[20] = {0};
	data = (p_bcn_rsp_data_t)rrm_event_data->event_body;
	char rdata[128] = {0};
	char result[128] = {0};
	int rfband;
	char errdesc[128] = {0};
	char mac[20] = {0}, macnodot[20] = {0};
	int channel;
	
	snprintf(temp2,sizeof(temp2),"BSSIDandRSSI%d",cnt);
	snprintf(result,sizeof(result),"%d",1);
	if(data->bcn_rsp_len ==0) {
		tcdbg_printf("bcn rsp timeout\n");
		return -1;
	} else {
		tcdbg_printf("receive bcn rsp\n");
	}
	
	if(data->bcn_rsp_len > 3) {
		tcdbg_printf("receive valid bcn rep info\n");
	} else {
		tcdbg_printf("receive invalid bcn rep info\n");
		return -1;
	}

	rcpi = (unsigned char*)(malloc(sizeof(unsigned char)));
	if(!rcpi)
	{
		tcdbg_printf("alloc memory fail\n");
		return -1;
	}
	
	rsp_hex_dump("bcn rsp:", data->bcn_rsp, data->bcn_rsp_len,&channel,rcpi,bssid);
	tcdbg_printf("*rcpi = %d\n",*rcpi);
	if(*rcpi < 190)
		RSSI = RCPI_TO_RSSI(*rcpi);
	else
		RSSI = (char)(*rcpi);
	tcdbg_printf("RSSI = %d\n",RSSI);
	snprintf(temp,sizeof(temp),"RSSI%d",cnt);
	snprintf(temp1,sizeof(temp1),"%d",RSSI);
	
	if((channel<= 14))
	{
		cfg_set_object_attr(WLAN_ROAM11K_NODE,temp, temp1);
		cfg_set_object_attr(WLAN_ROAM11K_NODE,temp2, "0");

		if(cfg_get_object_attr(WLAN_ROAM11K_NODE, "Sta_mac", mac, sizeof(mac)) < 0){		  
			tcdbg_printf("The node attr for Sta_mac does not exist\n");
			if(rcpi)
			free(rcpi);
			return CFG2_RET_FAIL;
		}
		rfband = 0;
		snprintf(result,sizeof(result),"%d",0);
	}
	else
	{
		cfg_set_object_attr(WLAN11AC_ROAM11K_NODE,temp, temp1);
		cfg_set_object_attr(WLAN11AC_ROAM11K_NODE,temp2, "0");

		if(cfg_get_object_attr(WLAN11AC_ROAM11K_NODE, "Sta_mac", mac, sizeof(mac)) < 0){		  
			tcdbg_printf("The node attr for Sta_mac does not exist\n");
			if(rcpi)
			free(rcpi);
			return CFG2_RET_FAIL;
		}
		rfband = 1;
		snprintf(result,sizeof(result),"%d",0);
	}
	mac_rm_dot(mac, macnodot);
	snprintf(rdata,sizeof(rdata),"%d;%s;%s;%s;%d;%s",
		rfband,result,macnodot,bssid,RSSI,errdesc);
	ecnt_event_send(ECNT_EVENT_WIFI,ECNT_EVENT_WIFI_APRSSIQUERYRESULT,
		rdata,strlen(rdata) + 1);

	if(rcpi)
    free(rcpi);
	return 0;
}

int rrm_event_handle(char *data)
{
	rrm_event_t *rrm_event_data = (p_rrm_event_t)data;
	
	/*rrm event*/
	switch (rrm_event_data->event_id)
	{
	case OID_802_11_RRM_EVT_BEACON_REPORT:
		tcdbg_printf("EVT_RRM_BEACON_REPORT----\n");
		event_bcn_rep(rrm_event_data);
		break;
		
	default:
		tcdbg_printf("unknown rrm event\n");
		break;
	}
	
	return 0;
}
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
static void parse_btm_rsp_data(unsigned char *pSrcBufVA, unsigned int SrcBufLen, int* StatusCode, unsigned char bssid[])
{
	unsigned char *p;
	unsigned char code  = 0;
	p = pSrcBufVA;
	
	/*status code*/
	code = (unsigned char) *p;
	*StatusCode = code;
	p += 1;
	
	/*skip BTM delay*/
	p += 1;

	/*target bssid*/
	memcpy(bssid, p, ETH_ALEN);
	tcdbg_printf("BSSID = " ADDR_FMT_STR "\n", ADDR_FMT_ARS(bssid));
	
	return ;
}
#if defined(TCSUPPORT_ECNT_MAP)
int event_btm_rsp (struct btm_rsp_data *evt_dat)
#else
int event_btm_rsp (struct wnm_event *evt_dat)
#endif
{
	int  result = 1;
	int  rfband;
	int  StatusCode;
	char path[32];
	char value[64];
	char errdesc[32];
	char macnodot[32];
	char ifname[IFNAMSIZ];
	unsigned char bssid[ETH_ALEN];
	char rsp_buf[128];
	struct btm_rsp_data *data = NULL;
	int ret = 0;

	if(NULL == evt_dat)
	{
		return -1;
	}
#if defined(TCSUPPORT_ECNT_MAP)
	data = (struct btm_rsp_data *)evt_dat;
#else
	data = (struct btm_rsp_data *)evt_dat->event_body;
#endif
	memset(ifname, 0, sizeof(ifname));
	if(NULL == if_indextoname(data->ifindex, ifname))
	{
		return -1;
	}
	
	/*BTM time out*/
	if(1 >= data->btm_rsp_len) 
	{
		tcdbg_printf("%s timeout! didn't receive btm rsp!\n", __FUNCTION__);
		return -1;
	}
	else 
	{
		memset(bssid, 0, sizeof(bssid));
#if defined(TCSUPPORT_ECNT_MAP)
		//hex_dump("BTMRsp:", data->btm_rsp, data->btm_rsp_len);
		parse_btm_rsp_data(data->btm_rsp, data->btm_rsp_len, &StatusCode, bssid);
#else
		//hex_dump("BTMRsp:", (data->btm_rsp+1), (data->btm_rsp_len-1));
		parse_btm_rsp_data((data->btm_rsp+1), (data->btm_rsp_len-1), &StatusCode, bssid);
#endif		
	}

	memset(path, 0, sizeof(path));
	if(0 == strcmp(ifname, "ra0"))
	{
		rfband = 0;
		snprintf(path, sizeof(path), WLAN_ROAM11V_NODE);
	}
	else if(0 == strcmp(ifname, "rai0"))
	{
		rfband = 1;
		snprintf(path, sizeof(path), WLAN11AC_ROAM11V_NODE);
	}
	else
	{
		tcdbg_printf("cfg 2.0 didn't receive btm rsp, ifname = %s\n", ifname);
		return CFG2_RET_FAIL;
	}
	
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), "%d", StatusCode);
	cfg_set_object_attr(path, "StatusCode",   value);
	cfg_set_object_attr(path, "Result",       "0");
	memset(value, 0, sizeof(value));
	if(0 >= cfg_get_object_attr(path, "Sta_mac", value, sizeof(value)))
	{		  
		tcdbg_printf("path = %s, The node attr for Sta_mac does not exist\n", path);
		return CFG2_RET_FAIL;
	}
	mac_rm_dot(value, macnodot);
	result = 0;
	memset(errdesc, 0, sizeof(errdesc));
	memset(rsp_buf, 0, sizeof(rsp_buf));
	snprintf(rsp_buf,sizeof(rsp_buf),"%d;%d;%s;%d;%s", rfband, result, macnodot, StatusCode, errdesc);
	
	ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_STABSSTRANSITIONRESULT, \
		rsp_buf, strlen(rsp_buf) + 1);

	if(ret < 0)
		tcdbg_printf("send ECNT_EVENT_WIFI_STABSSTRANSITIONRESULT error\n");

	return 0;
}

int wnm_event_handle(char *data)
{
	struct wnm_event *wnm_event_data = (struct wnm_event *)data;
	
	/*wnm event*/
	switch (wnm_event_data->event_id)
	{
	case OID_802_11_WNM_EVT_BTM_RSP:
		printf("EVT_BTM_RSP\n");
		event_btm_rsp(wnm_event_data);			
		break;
		
	default:
		printf("unknown wnm event\n");
		break;
	}
	
	return 0;
}
#endif
int conn_fail_event_handle(char *data)
{
	struct CONN_FAIL_MSG *msg = (struct CONN_FAIL_MSG *)data;
	char rdata[128] = {0};
	int ret = 0;
	char stamac[64]={0};
	memset(stamac,0, sizeof(stamac));
	/*wifi conn fail event*/
	switch (msg->ReasonCode)
	{
	case REASON_MIC_FAILURE:
	case REASON_AKMP_NOT_VALID:
		snprintf(rdata,sizeof(rdata),"%s;%02X%02X%02X%02X%02X%02X;Authentication failed",msg->Ssid,
			msg->StaAddr[0],msg->StaAddr[1],msg->StaAddr[2],msg->StaAddr[3],msg->StaAddr[4],msg->StaAddr[5]);
		//tcdbg_printf("Sta %02x:%02x:%02x:%02x:%02x:%02x connect to SSID %s Authentication failed\n", msg->StaAddr[0], 
		//msg->StaAddr[1],msg->StaAddr[2],msg->StaAddr[3],msg->StaAddr[4],msg->StaAddr[5], msg->Ssid);	
		ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_WIFICONNFAILURE, rdata, strlen(rdata) + 1);
#if defined(TCSUPPORT_CT_UBUS)
		snprintf(stamac,sizeof(stamac),"%02X%02X%02X%02X%02X%02X",
			msg->StaAddr[0],msg->StaAddr[1],msg->StaAddr[2],msg->StaAddr[3],msg->StaAddr[4],msg->StaAddr[5]);
		send_wifistamsgMAC2ubus(stamac);
#endif 
		break;
	case REASON_DECLINE:
		snprintf(rdata,sizeof(rdata),"%s;%02X%02X%02X%02X%02X%02X;Rejected by blacklist",msg->Ssid,
			msg->StaAddr[0],msg->StaAddr[1],msg->StaAddr[2],msg->StaAddr[3],msg->StaAddr[4],msg->StaAddr[5]);
		ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_WIFICONNFAILURE, rdata, strlen(rdata) + 1);
#if defined(TCSUPPORT_CT_UBUS)
		snprintf(stamac,sizeof(stamac),"%02X%02X%02X%02X%02X%02X",
			msg->StaAddr[0],msg->StaAddr[1],msg->StaAddr[2],msg->StaAddr[3],msg->StaAddr[4],msg->StaAddr[5]);
		send_wifistamsgMAC2ubus(stamac);
#endif
		break;
	default:
		tcdbg_printf("other ReasonCode failed\n");
		break;
	}
	
	return 0;
}
#ifdef TCSUPPORT_WLAN_VENDIE
int probevsie_event_handle(char* data)
{
	char mac[20] = {0};
	char OUI[10] = {0};
	char ProbeBand[2] = {0};
	char ServiceName[64] = {0};
	char RecodeNum[4] = {0};
	char dstNode[30] = {0};
	char mac1[5] = {0};
	char band1[6] = {0};
	char addr_mac[20][20] = {0};
	char addr_band[20][2] ={0};
	char dstNode2[30] = {0};
	int i = 0,j = 0,k = 0; /*k for entry,total entry is 3*/
	char buf[20]={0};
	char vsieIdx[8] = {0};
	char vsiebuf[1024] = {0};
	char vsiebufcat[1024] = {0};
	char buf1[20] = {0};
	char temp1[20],temp2[20],temp3[20];
	char rdata[128] = {0};
	int ret = 0;
	struct vendor_ie vsie;
	unsigned char band;
	struct probe_req_report *probe_event_data = (struct probe_req_report*)data;
	int RecordNum = 10;
		
	vsie = probe_event_data->vendor_ie;

	cfg_get_object_attr(PROBERXVSIE_COMMON_NODE, "Enable", buf, sizeof(buf));
	if(atoi(buf) == 0)
		return -1;

	for(k=0;k<3;k++){
		snprintf(dstNode,sizeof(dstNode),PROBERXVSIE_ENTRY_NODE,k+1);
		memset(buf,0,sizeof(buf));
		cfg_get_object_attr(dstNode, "OUI", buf, sizeof(buf));
		if(strlen(buf)<=0)
			break;
	}
	/*for entry k*/
	snprintf(dstNode2,sizeof(dstNode2),WIFIPROBEINFO_ENTRY_NODE,k);

	memset(buf,0,sizeof(buf));
	if(cfg_get_object_attr(dstNode2, "MAC0", buf, sizeof(buf)) < 0)
	{
		svc_cfg_boot_recorderxvsie_new();
	}
	
	memset(buf,0,sizeof(buf));
	snprintf(buf,sizeof(buf),"%d",i+1);
	memset(dstNode,0,sizeof(dstNode));
	snprintf(dstNode,sizeof(dstNode),PROBERXVSIE_ENTRY_NODE,k);
	cfg_get_object_attr(dstNode, "RecordNum", buf, sizeof(buf));
	if (atoi(buf) > 20)
		RecordNum = 20;
	else if (RecordNum < atoi(buf))
		RecordNum = atoi(buf);

	/*i for the ready mac number*/
	for(i = 0; i < RecordNum - 1; i++){
		memset(mac1,0,sizeof(mac1));
		snprintf(mac1,sizeof(mac1),"MAC%d",i);
		memset(band1,0,sizeof(band1));
		snprintf(band1,sizeof(band1),"BAND%d",i);
		cfg_get_object_attr(dstNode2, mac1, addr_mac[i], sizeof(addr_mac[i]));
		cfg_get_object_attr(dstNode2,band1,addr_band[i],sizeof(addr_band[i]));
		if(strlen(addr_mac[i])<=0)
			break;
	}

	memset(buf,0,sizeof(buf));
	snprintf(buf,sizeof(buf),"%02X%02X%02X%02X%02X%02X",probe_event_data->sta_mac[0],probe_event_data->sta_mac[1],
		probe_event_data->sta_mac[2],probe_event_data->sta_mac[3],probe_event_data->sta_mac[4],probe_event_data->sta_mac[5]);
	snprintf(buf1,sizeof(buf1),"%d",probe_event_data->band);
	for(j=0;j<i;j++){
		if((!strcmp(buf,addr_mac[j])) && (!strcmp(buf1,addr_band[j])))
			return -1;		
	}
	
	/*set the record MAC BAND VSIE*/
	snprintf(temp1,sizeof(temp1),"MAC%d",i);
	snprintf(temp2,sizeof(temp2),"BAND%d",i);
	snprintf(temp3,sizeof(temp3),"VSIE%d",i);
	
	//tcdbg_printf("probevsie_event_handle--mac:%02x:%02x:%02x:%02x:%02x:%02x----\n",PRINT_MAC(probe_event_data->sta_mac));
	memset(buf,0,sizeof(buf));
	snprintf(buf,sizeof(buf),"%02X%02X%02X%02X%02X%02X",probe_event_data->sta_mac[0],probe_event_data->sta_mac[1],
		probe_event_data->sta_mac[2],probe_event_data->sta_mac[3],probe_event_data->sta_mac[4],probe_event_data->sta_mac[5]);
	cfg_set_object_attr(dstNode2,temp1,buf);
	band = probe_event_data->band;
	if(band == 0)
		cfg_set_object_attr(dstNode2,temp2,"0");
	else
		cfg_set_object_attr(dstNode2,temp2,"1");
	
	for(j=0;j<vsie.len;j++)
	{
		snprintf(vsieIdx,sizeof(vsieIdx),"%02x", vsie.custom_ie[j]);
		strcat(vsiebuf, vsieIdx);
		strcat(vsiebufcat, vsiebuf);
	}

	cfg_set_object_attr(dstNode2,temp3,vsiebufcat);
	snprintf(rdata,sizeof(rdata),"%d;%d",k,i);
 
 	ret = ecnt_event_send(ECNT_EVENT_WIFI,
					ECNT_EVENT_DBUS_PROBERX_RECORD,
					rdata,
					strlen(rdata) + 1);
	return 0;
}
#endif
#ifdef TCSUPPORT_CT_WLAN_JOYME3
static void device_user_event_wireless(struct device_context *dctx,
									   char *data, int len)
{               
    struct iw_event iwe_buf, *iwe = &iwe_buf;
    char *pos, *end, *custom, *buf;
    pos = data;
    end = data + len;   
	
    while (pos + IW_EV_LCP_LEN <= end) {

		memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
			if (iwe->len <= IW_EV_LCP_LEN)
				return;
			
			custom = pos + IW_EV_POINT_LEN;
			
            char *dpos = (char *) &iwe_buf.u.data.length;
            int dlen = dpos - (char *) &iwe_buf;
            memcpy(dpos, pos + IW_EV_LCP_LEN,
				sizeof(struct iw_event) - dlen);
			
			switch (iwe->cmd) {
			case IWEVCUSTOM:
				if (custom + iwe->u.data.length > end)
					return;
				buf = malloc(iwe->u.data.length + 1);
				if (buf == NULL)
					return;
				memcpy(buf, custom, iwe->u.data.length);
				buf[iwe->u.data.length] = '\0';
				
				switch (iwe->u.data.flags) {
#ifdef TCSUPPORT_WLAN_ACS
			        case IW_CHANNEL_CHANGE_EVENT_FLAG:
				        tcdbg_printf("IW_CHANNEL_CHANGE_EVENT_FLAG\n");
					channelchange_event_handle(0);
				        break;
#endif
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
				case OID_802_11_RRM_EVENT:
					tcdbg_printf("RRM_EVENT----\n");
					rrm_event_handle(buf);
					break;
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
#if defined(TCSUPPORT_ECNT_MAP)
				case OID_802_11_WNM_BTM_RSP:
				{
					event_btm_rsp(buf);
					break;
				}
#else
				case OID_802_11_WNM_EVENT:
					tcdbg_printf("WNM_EVENT----\n");
					wnm_event_handle(buf);
					break;
#endif
#endif					
			        case OID_802_11_CONN_FAIL_MSG:
					conn_fail_event_handle(buf);
					break;	
#ifdef TCSUPPORT_WLAN_VENDIE
	                        case RT_PROBE_REQ_REPORT_EVENT:
					tcdbg_printf("RT_PROBE_REQ_REPORT_EVENT\n");
					probevsie_event_handle(buf);
					break;
#endif
                                default:
					if (iwe->u.data.flags != 1)
						//tcdbg_printf("unkwnon event type(%d)\n", iwe->u.data.flags);
						break; 
				}
				
				free(buf);
				break;
			}
			
			pos += iwe->len;
    }
}
#endif
#ifdef TCSUPPORT_WLAN_ACS
int channelchange_event_handle(wifi_type type)
{
	tcdbg_printf("Enter channelchange_event_handle\n");
	int ret = 0;
	char save[MAX_KEYWORD_LEN] = {0};
	char value[10];
	char current_time[30] = {0};
	int acs_2_4g_action=0;
	int   acs_5g_action=0;
	char  data[128]={0};
	char  data5G[128]={0};
	char old_channel[8] = {0};
	
	strcpy(data,IGD_PATH"/Info/WiFi;"IGD_NAME".WiFiInfo;ChannelScanStatus:y:1;");
	strcpy(data5G,IGD_PATH"/Info/WiFi;"IGD_NAME".WiFiInfo;ChannelScanStatus5G:y:1;");
	
	get_current_only_time(current_time, sizeof(current_time) - 1);
	
	if (cfg_obj_get_object_attr(WLAN_ACS_NODE, "Start", 0, value, sizeof(value)) > 0)
	{
		acs_2_4g_action = atoi(value);
		if(acs_2_4g_action > 0)
		{   
			cfg_obj_set_object_attr(WLAN_ACS_NODE,"ChannelScanFinishTime",0,current_time);
			if(0 == wifimgr_lib_get_channelScanScore(0,-1,save,MAX_KEYWORD_LEN))
			{
				cfg_obj_set_object_attr(WLAN_ACS_NODE,"ChannelScanScore",0,save);
			}
			if(0 == wifimgr_lib_get_bestChannel(0,-1,save,MAX_KEYWORD_LEN))
			{
				memset(old_channel, 0, sizeof(old_channel));
				if (cfg_obj_get_object_attr(WLAN_ACS_NODE,"Channel",0,old_channel,sizeof(old_channel)) > 0)
				{
					cfg_obj_set_object_attr(WLAN_ACS_NODE,"oldChannel",0,old_channel);
				}
				cfg_obj_set_object_attr(WLAN_ACS_NODE,"Channel",0,save);
			}
			cfg_obj_set_object_attr(WLAN_ACS_NODE,"Start",0,"0");
			cfg_obj_set_object_attr(WLAN_ACS_NODE,"ChannelScanStatus",0,"1");	
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_INFORM,
					data,
					strlen(data)+1);
			return 0;
		}
		//return 0;
	}
	if (cfg_obj_get_object_attr(WLAN11AC_ACS_NODE, "Start", 0, value, sizeof(value)) > 0)
	{
		acs_5g_action = atoi(value);
		if(acs_5g_action > 0)
		{
			cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"ChannelScanFinishTime",0,current_time);	
			if(0 == wifimgr_lib_get_channelScanScore(1,-1,save,MAX_KEYWORD_LEN))
			{
				cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"ChannelScanScore",0,save);
			}
			if(0 == wifimgr_lib_get_bestChannel(1,-1,save,MAX_KEYWORD_LEN))
			{
				memset(old_channel, 0, sizeof(old_channel));
				if (cfg_obj_get_object_attr(WLAN11AC_ACS_NODE, "Channel",0,old_channel,sizeof(old_channel)) > 0)
				{
					cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"oldChannel",0,old_channel);
				}
				cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"Channel",0,save);
			}
			cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"Start",0,"0");
			cfg_obj_set_object_attr(WLAN11AC_ACS_NODE,"ChannelScanStatus",0,"1");		
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_INFORM,
					data5G,
					strlen(data5G)+1);
		}
	}
					
    return 0 ;
}
#endif

#ifdef TCSUPPORT_WLAN_VENDIE
void timer_tool(void* unused)
{
	while(1){
		usleep(Check_Period);//sleep 100ms

		beaconVSIE_duration_check();
	}
}
#endif

