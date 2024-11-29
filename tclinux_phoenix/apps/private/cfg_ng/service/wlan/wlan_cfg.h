
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


#ifndef __SVC_WLAN_CFG_H__
#define __SVC_WLAN_CFG_H__
#include <stdio.h>
#include <libapi_lib_wifimgr.h>
#include <wifi_comm.h>

#define	PHY_MODE_BG_MIXED	0
#define	PHY_MODE_B_ONLY		1
#define	PHY_MODE_G_ONLY		4
#define	PHY_MODE_N_ONLY		6
#define	PHY_MODE_GN_MIXED	7
#define	PHY_MODE_BGN_MIXED	9
#define PHY_MODE_GNAX_MIXED	16

#define ESCAPE_CMD_BUF 		1025
#define SVC_WLAN_PATH_SIZE 	64

#define WLANBOTHSTATR 		400
#define WLAN5GSTATR   		500


#ifdef WSC_AP_SUPPORT
#define WPSSTART 			100
#define WPSSTOP 			200
#define WLANSTOP 			300
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) || defined(TCSUPPORT_WPS_5G_BTN)
#define WLANBOTHSTATR 		400
#define WPS5GSTART   		500
#define WPS5GSTOP  			600
#define WLAN5GSTOP 			700
#endif
#endif

#define WLANIF_11N 			0
#define WLANIF_11AC 		1
#define WLANIF_DOWN 		0
#define WLANIF_UP 			1

#define BEACONVSIE_UPDATE         1
#define BEACONVSIE_ENTRY_BUSY     2
#define BEACONVSIE_ENTRY_STOP     3
#define BEACONVSIE_ENTRY_FINISHED 0

#define _BEACONVSIE_ENTRY_FINISHED_ "0"

#define BEACONVSIE_MAX_ENTRY_NUM  8
#define BEACONVSIE_INTERFACE_ENTRY_NUM  16
/* Encryption type*/
#define WSC_ENCRTYPE_NONE    0x0001
#define WSC_ENCRTYPE_WEP     0x0002
#define WSC_ENCRTYPE_TKIP    0x0004
#define WSC_ENCRTYPE_AES     0x0008
#if defined(TCSUPPORT_CT_WLAN_WAPI)
#define WSC_ENCRTYPE_SMS4     0x0010
#endif

/* Authentication types*/
#define WSC_AUTHTYPE_OPEN        0x0001
#define WSC_AUTHTYPE_WPAPSK      0x0002
#define WSC_AUTHTYPE_SHARED      0x0004
#define WSC_AUTHTYPE_WPA         0x0008
#define WSC_AUTHTYPE_WPA2        0x0010
#define WSC_AUTHTYPE_WPA2PSK     0x0020
#define WSC_AUTHTYPE_WPANONE     0x0080
#define WSC_AUTHTYPE_WPA1WPA2	 WSC_AUTHTYPE_WPA|WSC_AUTHTYPE_WPA2
#if defined(TCSUPPORT_CT_WLAN_WAPI)
#define WSC_AUTHTYPE_WAPI_CERT		0x0040
#define WSC_AUTHTYPE_WAPI_PSK		0x0100
#endif

#define WLAN_MAC_NUM 				8
#define WLAN_SSID_ON 				"1"
#define WLAN_SSID_OFF				"0"
#define WLAN_APON 					"1"
#define WLAN_APOFF 					"0"

#define WLAN_MCS_SCRIPT_PATH 		"/etc/Wireless/RT2860AP/WLAN_mcs_exec.sh"
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
#define WLAN11AC_ENTRY_ATTR_SCRIPT_PATH 		"/etc/Wireless/RT2860AP_AC/WLAN_entry_attr_exec.sh"
#endif

#define KEY_NUM						4
#define WPS_ADD_IP_PATH 			"usr/osgi/config/ip.conf"

#define WIFIMACTAB_REINIT 			23
#define ARP_TABLE_PATH				"/tmp/arp_tab"

#define FINISHED                    0
#define BUSY                        2
#define STOP                        3

#define Check_Period				100000

#define MAX_STA_NUM 	70

#ifdef TCSUPPORT_WPS_BTN_DUALBAND

typedef struct _WSC_UUID_T{

	unsigned int timeLow;

	unsigned short timeMid;

	unsigned short timeHi_Version;

	unsigned char  clockSeqHi_Var;

	unsigned char  clockSeqLow;

	unsigned char  node[13];

}WSC_UUID_T;

#endif

#if defined(TCSUPPORT_CT_WLAN_JOYME3) 
typedef struct {
	long	tv_sec;		/* s */
	long	tv_usec;	/* us */
}BeaconTimeVal;

typedef struct 
{
	long SetDuration;	/* duration, s*/
	BeaconTimeVal StartBeaconTime;	/* StartBeaconTime*/
	BeaconTimeVal CurrentBeaconTime;	/* CurrentBeaconTime */
}EntryDuration;
#endif

typedef struct 
{
    char*           cChannelRate;
    int             iRateCount;
}TcWlanChannelMode;

typedef struct Countrynametocode{
	unsigned char	IsoName[3];
	char*		pCountryName;
}COUNTRY_NAME_TO_CODE;

struct PRE_SYS_STATE{
    char wlan_Bssid_num;/*WLan bssid_num */
	int wlan_chann;
   	char wlan11ac_Bssid_num;/*WLan11ac bssid_num */
	unsigned char wlan11ac_chann;/* to support AutoChann*/
	char wlan_whnat;
	char wlan11ac_whnat;
	char wlan_wirelessmode;
	char wlan_txPowerLevel;
	char wlan_gi;
	char wlan_extcha;
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	int  wlan_BeaconPeriod;
	int  wlan_DtimPeriod;
	int  wlan_TxPower;
	char wlan_HtBssCoex;
	char wlan_WirelessMode;
	char wlan_HtExchar;
	char wlan_HTBw;
	char wlan_HtGi;
	char wlan_HtTxstream;
	char wlan_HtRxstream;
	char wlan_CountryRegion;
	int  wlan11ac_BeaconPeriod;
	int  wlan11ac_DtimPeriod;
	int  wlan11ac_TxPower;
	char wlan11ac_HtBssCoex;
	char wlan11ac_WirelessMode;
	char wlan11ac_HtExchar;
	char wlan11ac_HTBw;
	char wlan11ac_HtGi;
	char wlan11ac_VhtBw;
	char wlan11ac_VhtGi;
	char wlan11ac_HtTxstream;
	char wlan11ac_HtRxstream;
	char wlan11ac_CountryRegionABand;
#if defined(TCSUPPORT_WLAN_AX)
	char wlan_MuOfdmaDlEnable;
	char wlan_MuOfdmaUlEnable;
	char wlan_MuMimoDlEnable;
	char wlan_MuMimoUlEnable;
	char wlan_TWTSupport;
	char wlan_SREnable;
	char wlan_SRMode;
	char wlan_SRSDEnable;
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)	
	char wlan_ant_dis;
	char wlan_ant_convert;
	char wlan_ant_sel;
#endif
	char wlan11ac_MuOfdmaDlEnable;
	char wlan11ac_MuOfdmaUlEnable;
	char wlan11ac_MuMimoDlEnable;
	char wlan11ac_MuMimoUlEnable;
	char wlan11ac_TWTSupport;
	char wlan11ac_SREnable;
	char wlan11ac_SRMode;
	char wlan11ac_SRSDEnable;
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	char wlan11ac_ant_dis;
	char wlan11ac_ant_convert;
	char wlan11ac_ant_sel;
#endif
#endif
#endif
	char wlan_bw;
	char wlan_bssconexist;
	int WdsEnable;
	char WdsEncrypType[5];
	int PerWdsON[MAX_WDS_ENTRY];
	char PerWdsKey[MAX_WDS_ENTRY][65];
	char WdsKey[65];
	char Wds_MAC[MAX_WDS_ENTRY][18];
	char WepKeyStr[4][27];
	int WdsEnable_11ac;
	char WdsEncrypType_11ac[5];
	int PerWdsON_11ac[MAX_WDS_ENTRY];
	char PerWdsKey_11ac[MAX_WDS_ENTRY][65];
	char WdsKey_11ac[65];
	char Wds_MAC_11ac[MAX_WDS_ENTRY][18];
	char WepKeyStr_11ac[4][27];
}pre_sys_state;

typedef struct GNU_PACKED _mix_peer_parameter {
	unsigned char channel; /*listen channel*/
	unsigned char center_channel;
	unsigned char bw;
	unsigned char ch_offset;
	unsigned char mac_addr[6];
	unsigned short duration; /*listen time*/
}mix_peer_parameter, *pmix_peer_parameter;

typedef struct WirelessCodeToName{
	char*	pCode;
	char*	pCodeName;
	char*	pCodeName1;
}Wireless_CODE_TO_NAME;

typedef struct AuthModeToBeacontype{
	char*	pAuthmode;
	char*		pBeacontype;
}Authmode_TO_Beacontype;

#ifdef TCSUPPORT_CT_WLAN_JOYME3
typedef struct _RSSI_SAMPLE {
	char LastRssi[4]; /* last received RSSI for ant 0~2 */
	char AvgRssi[4];
	signed short AvgRssiX8[4];
	char LastSnr[4];
	char AvgSnr[4];
	signed short AvgSnrX8[4];
	/*CHAR LastNoiseLevel[3]; */
	#ifdef TCSUPPORT_WLAN_AX
	char AckRssi[4];
	#endif
} RSSI_SAMPLE;

typedef struct  _MNT_STA_ENTRY {
	unsigned char  bValid;
	unsigned char  Band;
	unsigned char  muar_idx;
	unsigned char  muar_group_idx;
	unsigned long  Count;
	unsigned long  data_cnt;
	unsigned long  mgmt_cnt;
	unsigned long  cntl_cnt;
	unsigned char  addr[MAC_ADDR_LENGTH];
	RSSI_SAMPLE RssiSample;
	void *pMacEntry;
}MNT_STA_ENTRY, *PMNT_STA_ENTRY;
#define OID_GET_AIR_MONITOR_RESULT	0x1802
#define MONITORON 1
#define MONITOROFF 0
#define AIRMONITOR 1
#define MIXMODE 0
#define MONITOR_STA_ADDR_LEN		20
#define MAX_NUM_OF_MONITOR_STA		16
#endif

#define OID_MH_802_1X_SUPPORTED         0xFFEDC100
#define OID_802_11_ACL_LIST				0x052A


#if defined(TCSUPPORT_CT_WLAN_NODE)
#define TX_POWER_LEVLE_1 1
#define TX_POWER_LEVLE_2 2
#define TX_POWER_LEVLE_3 3
#define TX_POWER_LEVLE_4 4
#define TX_POWER_LEVLE_5 5
#define TX_POWER_LEVLE_6 6
#endif

#if defined(TCSUPPORT_WLAN_AC)
#define PHY_MODE_11A_ONLY 2
#define PHY_MODE_11AN_MIXED 8
#define PHY_MODE_11N_MIXED 11
#define PHY_11VHT_N_A_MIXED 14 
#define PHY_11VHT_N_MIXED 15
#if defined(TCSUPPORT_WLAN_AX)
#define PHY_MODE_N_AC_AX_MIX 17
#endif
#endif

#define BuffSet_Str(x, y) snprintf(x, sizeof(x), "%s", y)

#define WIFI_DOWN_CMD_RAI   	"ifconfig rai%d down"
#define WIFI_UP_CMD_RAI     	"sleep 1;ifconfig rai%d up;"
#define WIFI_DOWN_CMD_RA  		"ifconfig ra%d down"
#define WIFI_UP_CMD_RA  		"sleep 1;ifconfig ra%d up;"
#define WIFI_DOWN_CMD_WDSI  	"ifconfig wdsi%d down;"
#define WIFI_UP_CMD_WDSI     	"sleep 1;ifconfig wdsi%d up;"
#define WIFI_DOWN_CMD_WDS  		"ifconfig wds%d down"
#define WIFI_UP_CMD_WDS  		"sleep 1;ifconfig wds%d up;"

#define WIFI_RESET_CMD 			"ifconfig ra0 down;sleep 5;ifconfig ra0 up;"

#if 0//def TCSUPPORT_WLAN_BNDSTRG
#define S_IRUSR     00400
#define S_IWUSR     00200
#define S_IXUSR     00100
#endif
#define EXTCHA_BELOW		0x3
#define OID_GET_SET_TOGGLE	0x8000
#define OID_MIX_MODE_BASE	0x1280
enum mix_mode_subcmd_oid {
	OID_SUBCMD_SET_MIXMODE,
	OID_SUBCMD_GET_RSSI,
	OID_SUBCMD_CANCEL_MIXMODE,
	NUM_OID_SUBCMD_MIXMODE,
	OID_SUBCMD_MIXMODE_MAX = NUM_OID_SUBCMD_MIXMODE - 1
};
#define OID_SET_MIXMODE		(OID_MIX_MODE_BASE | OID_SUBCMD_SET_MIXMODE)
#define OID_MIXMODE_GET_RSSI	(OID_MIX_MODE_BASE | OID_SUBCMD_GET_RSSI)

char* WscGetAuthTypeStr(unsigned short authFlag);
char*  WscGetEncryTypeStr(unsigned short encryFlag);
void fputs_escape(char *cmd, FILE *fp);
int run_wps();
int wps_oob();
#if defined(TCSUPPORT_WLAN_AC)
int reset_ac_counter();
int txBurst_or_not_ac(int BssidNum);
#endif
int wps_genpincode();
int write_wlan_config(char* path, int Bssid_Num);
#if defined(TCSUPPORT_WLAN_AC)
int write_wlan11ac_config(char* path, int Bssid_Num);
#endif
void dowscd();
int reloadWifiModule();
int reloadWifi(char* path, int preBssidNum, int newBssidNum);
int wpsAction(char* path, char* APOn, int action);
void startWifiInterface(char* path, int BssidNum);
void start11acWifiInterface(char* path, int BssidNum);

int getWlanSSIDEnableNum(int type, int BssidNum);
int checkWriteBinToFlash(void);
void executeWLANSetupScript();
#if defined(TCSUPPORT_WLAN_AC)	
void rtdot1xd_AC_start();
void rtdot1xd_AC_stop();
#endif
int svc_wlan_execute(char* path);
#if defined(TCSUPPORT_WLAN_AC)	
int svc_wlan11ac_execute(char* path);
#endif
#ifdef TCSUPPORT_WLAN_WDS
int write_wds_attr(int type, FILE* fp);
int wlan_wds_execute(int *p_isWDSReloadWIFI, int *p_WdsActive);
int wlan11ac_wds_execute(int *p_isWDSReloadWIFI, int *p_WdsActive);
#endif
void check_wifi(void);
#ifdef WSC_AP_SUPPORT
int isWPSRunning(void);
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
void clr_wps_flag(void);
#endif 
void check_wpsbtn(void);
#if defined(TCSUPPORT_WPS_5G_BTN)	
void check_wps5gbtn(void);
#endif
#endif
void checkWlanBtn(void);
#ifdef 	TCSUPPORT_WLAN_AC
void checkWlan11acBtn(void);
#endif
void check_wlan_wps_button(void);
#if defined(TCSUPPORT_CT_JOYME)
void check_wpsstatus(void);
#endif
#if defined(TCSUPPORT_CT_PON)
#ifdef WSC_AP_SUPPORT	
void wlanNodeUpdate(void);	  
#endif	
void wifiMacTab_update(void);
void check_wifi_mac_entry(void);
#endif
int doWlanRate(char *Rate, int nodeid);
int	doWlanMcs(char *Mcs, int nodeid);
int write_wlan_exe_sh(int BssidNum);
int write_wlan_ant_sh(void);
void pre_sys_state_init(void);
int setMacToDataFile(int type);
#ifdef TCSUPPORT_WLAN_WDS
void setWdsAttrToDataFile(int type);
#endif
#if defined(TCSUPPORT_WLAN_AC)
int write_wlan11ac_exe_sh(int BssidNum);
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
int write_wlan11ac_entry_attr_exe_sh(int BssidNum);
int write_wlan_entry_7915D_sh(int BssidNum);
int write_wlan11ac_entry_7915D_sh(int BssidNum);
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
int guest_cc(void);
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
int sameIndex( int *wlan_id);
void SetSameSSIDandKey(char * action,char * wifitype);
void SetSameSSIDCommonExcute(char* path);
int BndstrgParatrue();
int BndstrgProceess();
void BndStrgSetParameter();
#endif

#if defined(TCSUPPORT_ANDLINK)
int getUplinkRate(struct timeval lastTime, struct timeval curTime,unsigned long *oldRxBytes, unsigned long *oldTxBytes);
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
int np_check_radio_signal();
#endif

#endif

