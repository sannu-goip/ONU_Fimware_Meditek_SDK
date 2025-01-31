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
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include <time.h>
#include <flash_layout/tc_partition.h>
#include <cfg_romfile.h>
#include <svchost_evt.h>
#include <flash_layout/prolinecmd.h>
#include "blapi_xdsl.h"


#define UDHCPC_PID_PATH	"/var/run/%s/udhcpc.pid"
#define UDHCP6C_PID_PATH	"/var/run/%s/dhcp6c.pid"
#define WAN_STOP_SH "/usr/script/wan_stop.sh"
#define PPP_START_SH "/usr/script/ppp_start.sh"
#define CONNECT    1
#define DISCONNECT 2
#define RENEW_IP	1
#define RELEASE_IP	2
#define RENEW_IP6	3
#define RELEASE_IP6	4

#define INIT "0"
#define FWVER_PATH	"/etc/fwver.conf"
#define COMPILE_TIME_PATH	"/etc/compile_time"
#if defined(TCSUPPORT_CT_JOYME)
#define GATEWAY_PATH	"/usr/osgi/config/gateway.conf"
#else
#define GATEWAY_PATH	"/etc/gateway.conf"
#endif
#if defined(TCSUPPORT_GENERAL_MULTILANGUAGE)
#define GATEWAY_EN_PATH	"/etc/gateway_en.conf"
#endif
#define TZ_MANUAL_TYPE	2
#define TZ_NTP_TYPE	0

#define SYSTEMSTARTUPTIME "sysStartTime"
#define NODE_ACTIVE	"Active"
#define PPPoE_LLC "PPPoE LLC"
#define PPPoE_VC_Mux "PPPoE VC-Mux"
#define PPPoA_LLC "PPPoA LLC"
#define PPPoA_VC_Mux "PPPoA VC-Mux"
#define CONNECT_TYPE_MANUALLY "Connect_Manually"
#define PPP_START_SH "/usr/script/ppp_start.sh"
#define SEC_PER_DAY 86400 /*60*60*24  for ppp link-up time*/
#define SEC_PER_HOUR 3600 /*60*60     */
#define SEC_PER_MIN 60

#define ISPCONFIG_PATH "/etc/isp%d.conf"
#define NO_QMARKS 0
#define QMARKS 1
#define CONNECTED	"1"
#define CONNECT_DISP_BUTTON "1"
#define NAS_IF	"nas%d"
#define PPP_IF	"ppp%d"

enum dev_pvc_info_en{
	Status=0,
	PppConnTime,
	DispConnBtnType,
	DispPPPconn,
	DispBtnType,
	WanIP,
	WanSubMask,
	DNSIP,
	SECDNSIP, 
	WanDefGW,
	connType,
	Gateway, 
};


#define PRODUCTLINECWMPFILEPATH "/tmp/productlinecwmpfile"
#define PRODUCTLINEPARAMAGIC 0x12344321

#define PL_CP_SSID_FLAG (1<<PL_CP_SSID)
#define PL_CP_WPAKEY_FLAG (1<<PL_CP_WPAKEY)
#define PL_CP_WEBPWD_FLAG (1<<PL_CP_WEBPWD)

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
#define DEFAULT_SSID "CMCC-AP"
#else
#define DEFAULT_SSID "Bharat_Net-AP"
#endif

#define DEFAULT_WPAKEY "12345678"
#define DEFAULT_WEBPWD "1234"

#define DEVICEPARASTATIC_PATH	"/etc/deviceParaStatic.conf"
#define PL_CP_HW_VER_FLAG (1<<PL_CP_HW_VER)

#define SYSROUTE_CMD	"/sbin/route -n > /tmp/route"
#define SYSROUTE_PATH	"/tmp/route"
#define DEF_GW_FLAG	"UG"
#define NAS_GATEWAY "gateway="
#define NAS_GATEWAYFILE "/etc/nas%d_gateway.conf"
#define TMP_PPP_PATH "/tmp/checkppp"
#define MAX_BUF_SIZE	2048
#define ACTIVE		"Yes"

int parsedStr(char *str_input, char *str_ret)
{
	char *pf = NULL;
	char *ps = str_input;
	int  iLen = 0;
	
	pf = strrchr(ps, '\r');
	if(NULL != pf){
		iLen++;
	}
	
	pf = strrchr(ps, '\n');
	if(NULL != pf){
		iLen++;
	}
	strncpy(str_ret, str_input, strlen(str_input)-iLen);
	return SUCCESS;	
}

void GetEtherAddr(unsigned char *mac_addr)
{
	FILE *fp = NULL;
	char *macORG = NULL, *mac_p = NULL;
	int len = 0, idx = 0, cnt = 0, ret = 0, line = 0;
	int  macaddr[7]={0};

	fp = fopen("/etc/mac.conf","r");
	if (fp != NULL) {
		while ((ret = getline(&macORG, &len, fp)) != -1) {
			if (line == 0) {
				line++;
				continue;
			}

			mac_p = strstr(macORG, "=");
			if (mac_p == NULL)
				break;

			sscanf(mac_p + 1, "%02x:%02x:%02x:%02x:%02x:%02x",
					&macaddr[0], &macaddr[1], &macaddr[2], &macaddr[3], &macaddr[4], &macaddr[5]);
			mac_addr[0] = macaddr[0]; 
			mac_addr[1] = macaddr[1]; 
			mac_addr[2] = macaddr[2]; 
			mac_addr[3] = macaddr[3]; 
			mac_addr[4] = macaddr[4]; 
			mac_addr[5] = macaddr[5]; 
		}

		if (macORG) 
			free(macORG);
		if (fp)
			fclose(fp);
	}

	return;
}

char* getDefGW(char* gateway)
{
	enum routeInfo_en{
		DstIP=0,
		Gw,
		Mask,
		Flags,
		Metric,
		Ref,
		Use,
		Iface,
		Usr_def,
	};

	int i=0;
	char *buf=NULL;
	char routeInfo[8][16];
	FILE *fp=NULL;

	memset(routeInfo,0, sizeof(routeInfo));
	strcpy(gateway, "");
	system(SYSROUTE_CMD);

	fp=fopen(SYSROUTE_PATH, "r");
	if(fp==NULL){
		return gateway;
	}
	else{
		buf= malloc(512);
		if(buf==NULL){
			fclose(fp);
			unlink(SYSROUTE_PATH);
			return gateway;
		}
		/*
		Kernel IP routing table
		Destination	Gateway	Genmask			Flags	Metric	Ref	Use	Iface
		10.10.10.0	0.0.0.0	255.255.255.0		U		0		0	0	br0
		127.0.0.0		0.0.0.0	255.255.0.0		U		0		0	0	lo
		*/
		while (fgets (buf, 512, fp) != NULL)
		{
			if(i > 1)
			{
				/*Skip the comment information*/
				sscanf(buf, "%s %s %s %s %s %s %s %s",
				routeInfo[DstIP], routeInfo[Gw], routeInfo[Mask],
				routeInfo[Flags], routeInfo[Metric], routeInfo[Ref],
				routeInfo[Use], routeInfo[Iface]);
				if(strcmp(routeInfo[Flags],DEF_GW_FLAG) == 0){
					strcpy(gateway, routeInfo[Gw]);
					break;
				}
			}
			i++;
		}
	}
	free(buf);
	fclose(fp);
	unlink(SYSROUTE_PATH);
	return gateway;
}/*end getDefGW*/

 
char* getDNSInfo(char* type, char* isp, char* dev, char* buf)
{
	char fileName[50] = {0};
	char sPVC[3] = {0};
	char tmpBuf[512] = {0};
	char dnsInfo[2][16];

	int iPVC = 0;

	strcpy(buf,"");
	memset(dnsInfo,0, sizeof(dnsInfo));
	strncpy(sPVC, dev+3, sizeof(sPVC) - 1);
	iPVC = atoi(sPVC);
	
	if(strcmp(type, "Pridns") == 0)
	{
		if(strcmp(isp, "Static") == 0)
		{	
			strcpy(buf, "168.95.1.1");
		}
		else if(strcmp(isp, "Dynamic") == 0)
		{	
			/*get from /etc/resolve_nasi.conf file */
			sprintf(fileName, "/etc/resolv_nas%d.conf", iPVC);
			fileRead(fileName, tmpBuf, sizeof(tmpBuf));			
			if(strstr(tmpBuf, DNS_ATTR))
			{
				sscanf(strstr(tmpBuf, DNS_ATTR),"%s %s", dnsInfo[0], dnsInfo[1]);
				strcpy(buf, dnsInfo[1]);	
			}
		}
		else
		{	
			/*ppp: get from /etc/ppp/resolve_pppi.conf file */
			sprintf(fileName, "/etc/ppp/resolv_ppp%d.conf", iPVC);
			fileRead(fileName, tmpBuf, sizeof(tmpBuf));
			if(strstr(tmpBuf, DNS_ATTR)){
				sscanf(strstr(tmpBuf, DNS_ATTR),"%s %s", dnsInfo[0], dnsInfo[1]);
				strcpy(buf, dnsInfo[1]);	
			}
		}
	}
	else
	{
		if(strcmp(isp, "Static") == 0)
		{
			strcpy(buf, "168.95.1.2");
		}
		else if(strcmp(isp, "Dynamic") == 0)
		{
			sprintf(fileName, "/etc/resolv_nas%d.conf", iPVC);
			memset(tmpBuf, 0, sizeof(tmpBuf));
			fileRead(fileName, tmpBuf, sizeof(tmpBuf));
			if(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR))
			{
				sscanf(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR), "%s %s", dnsInfo[0], dnsInfo[1]);
				strcpy(buf, dnsInfo[1]);
			}
		}
		else
		{	/* ppp */
			sprintf(fileName, "/etc/ppp/resolv_ppp%d.conf", iPVC);
			fileRead(fileName, tmpBuf, sizeof(tmpBuf));
			if(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR))
			{
				sscanf(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR), "%s %s", dnsInfo[0], dnsInfo[1]);
				strcpy(buf, dnsInfo[1]);
			}
		}
	}
	
	return buf;
}

int checkPpp(int cur_pvc)
{
	char ifconfig_ppp_cmd[32]={0};
	snprintf(ifconfig_ppp_cmd, sizeof(ifconfig_ppp_cmd), "/sbin/ifconfig ppp%d > %s", cur_pvc, TMP_PPP_PATH);
	system(ifconfig_ppp_cmd);
	char* buf = (char *)malloc(MAX_BUF_SIZE);
	if(buf==NULL)
	{
		perror("malloc fail");
		unlink(TMP_PPP_PATH);
		return FAIL;
	}
	memset(buf,0, MAX_BUF_SIZE);
	fileRead(TMP_PPP_PATH, buf, MAX_BUF_SIZE);
	unlink(TMP_PPP_PATH);
	if(strstr(buf,"inet addr:")==0)
	{/*ppp is down*/
		free(buf);
		return 0;
	}
	else
	{/* ppp up*/
		free(buf);
		return 1;
	}
}/*end get_ifconfig_value*/


char *getPVCGateway(int pvcindex,char *pvcGateway, int size)  
{
	char pvc_stat[32]={0};
	char adsl_stat[32]={0};
	char buf[32]={0};
	char dev[16] = {0};
	char ispfile[32] = {0};
	char gateway[32] = {0};
	char gatewayfile[32] = {0};
	char pvc_info[16]={0};
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	char nodePath[64] = {0};
#endif	 
#endif
	int xdsl_type = -1;
	
	if(pvcindex<0 || pvcindex>7 || pvcGateway == NULL || size<=0)
	{
		printf("getPVCGateway: pvcindex error is [%d],  range should be 0-7\n", pvcindex);
		return "N/A";
	}

	memset(pvcGateway, 0, size);
	strncpy(pvcGateway, "N/A", size-1);
	strncpy(gateway, "N/A", sizeof(gateway)-1);
	
	sprintf(ispfile, ISPCONFIG_PATH, pvcindex);
	get_profile_str("Active=",pvc_stat, sizeof(pvc_stat), QMARKS, ispfile);
	get_profile_str("ISP=",buf, sizeof(buf), QMARKS, ispfile);
	
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	memset(adsl_stat, 0, sizeof(adsl_stat));
	snprintf(nodePath, sizeof(nodePath), XPON_COMMON_NODE);
	cfg_obj_get_object_attr(nodePath, "trafficStatus", 0, adsl_stat, sizeof(adsl_stat));		
#endif	
#else
	blapi_xdsl_get_xdsl_linkstatus(adsl_stat, &xdsl_type);
#endif

	if((strcmp(pvc_stat, ACTIVE)==0) && (strstr(adsl_stat,"up")) )
	{
		switch(atoi(buf))
		{
#ifndef PURE_BRIDGE
			case WAN_ENCAP_DYN_INT:/*Dynamic IP Address*/
			case WAN_ENCAP_STATIC_INT:/*Static IP Address*/
				sprintf(dev, NAS_IF, pvcindex);
				if(strlen(getWanInfo(Ip_type, dev, pvc_info))>0)
				{
					strcpy(gateway, "N/A");
					if(atoi(buf) == WAN_ENCAP_STATIC_INT) /*Static IP, we get gateway from isp$i.conf*/
					{
						sprintf(gatewayfile, ISPCONFIG_PATH, pvcindex);
						get_profile_str("GATEWAY=", gateway, sizeof(gateway), QMARKS, gatewayfile);
					}
					else/*Dynamic IP, we get from /etc/nas$i_gateway.conf file*/
					{
						sprintf(gatewayfile, NAS_GATEWAYFILE, pvcindex);
						get_profile_str(NAS_GATEWAY, gateway, sizeof(gateway), NO_QMARKS, gatewayfile);
					}
				}
				break;
			case WAN_ENCAP_PPP_INT:/*PPPoA && PPPoE*/
				if(checkPpp(pvcindex))
				{
					sprintf(dev, PPP_IF, pvcindex);
					getWanInfo(PTP_type, dev, gateway);
				}
				break;
#endif
			case WAN_ENCAP_BRIDGE_INT:/*bridge mode*/
				break;
			default:
				break;
		}
	}
	strncpy(pvcGateway, gateway, size -1);
	return pvcGateway;

}

int cfg_type_deviceInfo_deviceParaDynamic_read(char*path)
{
	static int notUsed = 0;
#ifdef TCSUPPORT_PRODUCTIONLINE
	static proline_Para para;
#endif
	unsigned char nm_MAC[7] ;
	char buf[64] = {0};
	int ret;

	char nodePath[64] = {0};
	char deviceParaDynamic_keywd[][32]=
	{
		{"ManufacturerOUI"},
		{"SerialNum"},
		{"ProductClass"},
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
		{"ssid"},
#if !defined(TCSUPPORT_NP_CMCC)
		{"wpakey"},
		{"webpwd"},
#else
		{"AREA"},
		{"CMEI"},
#endif
#endif
#if defined(TCSUPPORT_CT_UBUS)
		{"CTEI"},
		{"Province"},
#endif
#if defined(TCSUPPORT_CT_JOYME4_JC)
		{"telnetEnable"},
#endif
		{""}
	};

	enum deviceParaDynamic_eu{
		ManufacturerOUI = 0,
		SerialNum,	
		ProductClass,
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
		plssid,
#if !defined(TCSUPPORT_NP_CMCC)
		plwpakey,
		plwebpwd,
#else
		AREA,
		CMEI,
#endif
#endif
#if defined(TCSUPPORT_CT_UBUS)
		CTEI,
		Province,
#endif
#if defined(TCSUPPORT_CT_JOYME4_JC)
		telnetEnable,
#endif
	};

#ifdef TCSUPPORT_PRODUCTIONLINE
	if ( 0 == notUsed )
		tcgetProLinePara(&para,0);
	
	if(PRODUCTLINEPARAMAGIC == para.magic)
	{
		notUsed = 1;
	}

	snprintf(nodePath, sizeof(nodePath), CWMP_ENTRY_NODE);
	if(notUsed && (para.flag & PL_CP_MANUFACUREROUI_FLAG))
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[ManufacturerOUI], para.manufacturerOUI );
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		cfg_obj_get_object_attr(nodePath, "ManufacturerOUI", 0, buf, sizeof(buf));
		if(buf[0] == '\0')
		{
			GetEtherAddr(nm_MAC);
			snprintf(buf, sizeof(buf),"%02X%02X%02X",nm_MAC[0],nm_MAC[1],nm_MAC[2]);
		}
		cfg_set_object_attr(path, deviceParaDynamic_keywd[ManufacturerOUI], buf );
	}
#if defined(TCSUPPORT_CT_UBUS)
	if (notUsed) 
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[CTEI], (para.flag & PL_CP_CTEI_FLAG) ? para.ctei : "" );
		cfg_set_object_attr(path, deviceParaDynamic_keywd[Province], (para.flag & PL_CP_PROVINCE_FLAG) ? para.province : "" );
	}
#endif
#if defined(TCSUPPORT_CT_JOYME4_JC)
	if ( notUsed )
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[telnetEnable], (para.flag & PL_CP_TELNET_FLAG) ? para.telnet: "" );
	}
#endif

	if(notUsed &&(para.flag & PL_CP_SERIALNUM_FLAG))
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[SerialNum], para.serialnum );
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		ret = cfg_obj_get_object_attr(nodePath, "SerialNum", 0, buf, sizeof(buf));
		
		if(buf[0] == '\0')
		{
			GetEtherAddr(nm_MAC);
			snprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X",nm_MAC[0],nm_MAC[1],nm_MAC[2],nm_MAC[3],nm_MAC[4],nm_MAC[5]);
		}
		cfg_set_object_attr(path, deviceParaDynamic_keywd[SerialNum], buf );
	}

	if(notUsed &&(para.flag & PL_CP_PRODUCTCLASS_FLAG))
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[ProductClass], para.productclass );
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		ret = cfg_obj_get_object_attr(nodePath, "ProductClass", 0, buf, sizeof(buf));
		
		if(buf[0] == '\0')
		{
			GetEtherAddr(nm_MAC);
			sprintf(buf,"%02x%02x%02x",nm_MAC[3],nm_MAC[4],nm_MAC[5]);
		}
		cfg_set_object_attr(path, deviceParaDynamic_keywd[ProductClass], buf );
	}
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
	if (notUsed) 
	{
		cfg_set_object_attr(path, deviceParaDynamic_keywd[plssid], (para.flag & PL_CP_SSID_FLAG) ? para.ssid : DEFAULT_SSID );
#if !defined(TCSUPPORT_NP_CMCC)
		cfg_set_object_attr(path, deviceParaDynamic_keywd[plwpakey], (para.flag & PL_CP_WPAKEY_FLAG) ? para.wpakey : DEFAULT_WPAKEY);
		cfg_set_object_attr(path, deviceParaDynamic_keywd[plwebpwd], (para.flag & PL_CP_WEBPWD_FLAG) ? para.webpwd : DEFAULT_WEBPWD );
#else
#if !defined(TCSUPPORT_ECNT_MAP)
		cfg_set_object_attr(path, deviceParaDynamic_keywd[AREA], (para.flag & PL_CP_BARCODE_FLAG) ? para.barcode : "GV" );
#endif		
		cfg_set_object_attr(path, deviceParaDynamic_keywd[CMEI], (para.flag & PL_CP_XPONSN_FLAG) ? para.xponsn : ( (para.flag & PL_CP_SERIALNUM_FLAG) ? para.serialnum : "" ) );		
#endif
	}
#endif
#else
	
	GetEtherAddr(nm_MAC);
	snprintf(buf,sizeof(buf),"%02X%02X%02X",nm_MAC[0],nm_MAC[1],nm_MAC[2]);
	cfg_set_object_attr(path, deviceParaDynamic_keywd[ManufacturerOUI], buf );
	
	GetEtherAddr(nm_MAC);
	snprintf(buf, sizeof(buf),"%02X%02X%02X-%02X%02X%02X%02X%02X%02X",nm_MAC[0],nm_MAC[1],nm_MAC[2],nm_MAC[0],nm_MAC[1],nm_MAC[2],nm_MAC[3],nm_MAC[4],nm_MAC[5]);
	cfg_set_object_attr(path, deviceParaDynamic_keywd[SerialNum], buf );
	
	GetEtherAddr(nm_MAC);
	snprintf(buf, sizeof(buf),"%02x%02x%02x",nm_MAC[3],nm_MAC[4],nm_MAC[5]);
	cfg_set_object_attr(path, deviceParaDynamic_keywd[ProductClass], buf );
#endif

	return SUCCESS;
}


int cfg_type_deviceInfo_deviceParaStatic_read(char*path)
{
	char attrName[32] = {0};
	char attrVal[128] = {0};
	int i = 0;
	int ret = 0;
	#if defined(TCSUPPORT_CT_MIDWARE)
	int j = 0;
	#endif

	char deviceParaStatic_keywd[][32]=
	{
		{"Manufacturer"},{"ModelName"},
		{"CustomerSWVersion"},{"CustomerHWVersion"},
#if defined(TCSUPPORT_CT_E8GUI)
		/*{"DvID"},*/{"BatCode"},
#endif
#if defined(TCSUPPORT_CT_FJ)
		{"DevDescribtion"},
#endif
		{"Lanports"},
#if defined(TCSUPPORT_NP_CMCC)
		{"deviceVendor"},{"UpgradeVersion"},{"CustomerSwVersionTime"},
#endif
		{""}
	};

#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
	static int notUsed = 0;
	static proline_Para para;
	
	if ( 0 == notUsed )
		tcgetProLinePara(&para,0);
	
	if(PRODUCTLINEPARAMAGIC == para.magic)
	{
		notUsed = 1;
	}
#endif

	for ( i = 0; strlen(deviceParaStatic_keywd[i]) != 0; i++ ) 
	{	
		memset( attrName, 0, sizeof(attrName) );
		memset( attrVal, 0, sizeof(attrVal) );
		snprintf( attrName, sizeof(attrName), "%s=", deviceParaStatic_keywd[i] );
		ret = get_profile_str_new( attrName, attrVal, sizeof(attrVal), DEVICEPARASTATIC_PATH);
		if ( ret != -1 )
		{
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
			if (!strcmp(deviceParaStatic_keywd[i], "CustomerHWVersion")) {
				if(notUsed && (para.flag & PL_CP_HW_VER_FLAG)) {
					strncpy(attrVal, para.hwver, sizeof(attrVal)-1);
				}
			}
			else if (!strcmp(deviceParaStatic_keywd[i], "ModelName")) {
				if(notUsed && (para.flag & PL_CP_PRODUCTCLASS_FLAG)) {
					strncpy(attrVal, para.productclass, sizeof(attrVal)-1);
				}
			}
#endif
	#if defined(TCSUPPORT_CT_MIDWARE)
			if(i == 2 || i == 3){
				for ( j = 0; j < strlen(attrVal); j ++ )
				{
					if ( ' ' == attrVal[j] )
						attrVal[j] = '_';
				}
			}
	#endif				
			cfg_set_object_attr(path, deviceParaStatic_keywd[i], attrVal );
		}
	}

	return SUCCESS;
}


static int cfg_type_deviceinfo_set_common_info(char* path,char* attr) 
{
	char fwVer[32]={0};
#if defined(TCSUPPORT_CT_PROLINE_SUPPORT) || defined(TCSUPPORT_CT_PON_CZ_GD)
	char compileTime[64] = {0};
#endif
	#if defined(TCSUPPORT_CT_E8GUI)
	char gateWayName[32]={0};
	char DeviceInfoTemp[32]={0};
	#endif	
	char buf[32]={0};
	char tz_type[4]={0};  
	int i=0;
	time_t timep;
	time_t tmpSysStartTime;
	time_t passedTime;
	char tmpTimeStr[32] = {0};
	char tmpTimeFormatStr[20] = {0};
	char language[32]={0};
	int timeFormatIdx = 0;
	int now_time,connect_time;/*for caclated ppp link-up time*/
	struct sysinfo info;
	int hour,min,sec;
	struct tm *pTm;

#if defined(TCSUPPORT_CT_PON_CY_JX)
	char ponuptimetemp[32] = {0};
	char ponuptime[32] = {0};
#endif

	char nodePath[64] = {0};
	char sysinfoPath[64] = {0};
	char globalPath[64] = {0};
	char ntpServer[16] = {0};

	if (path== NULL || attr == NULL)
	{
		return -1;
	}
	snprintf(globalPath, sizeof(globalPath), GLOBALSTATE_NODE);

#if defined(TCSUPPORT_CT_E8GUI)
#if defined(TCSUPPORT_CT_GUIPSWSHOW)
	snprintf(nodePath, sizeof(nodePath), WEBCURSET_ENTRY_NODE );
	cfg_set_object_attr(nodePath, "IsSupportShowPSW", "1" );
#endif
#endif

#if defined(TCSUPPORT_CT_GUIPSWSHOW) || defined(TCSUPPORT_NP_CMCC)
	snprintf(nodePath, sizeof(nodePath), WEBCURSET_ENTRY_NODE );
	cfg_set_object_attr(nodePath, "IsSupportWIFIShowPSW", "1" );
#endif

	fileRead(FWVER_PATH, fwVer, sizeof(fwVer));
	cfg_set_object_attr(DEVICEINFO_NODE, "FwVer", fwVer);
	
#if defined(TCSUPPORT_CT_PON_CY_JX)
	fileRead("/tmp/pon_uptime.conf", ponuptimetemp, sizeof(ponuptimetemp));
	parsedStr(ponuptimetemp,ponuptime);
	cfg_set_object_attr(DEVICEINFO_NODE, "ponUpTime", ponuptime);
	cfg_set_object_attr(DEVICEINFO_NODE, "CurTime", (const char *)getUpDateTime());
#endif

#if defined(TCSUPPORT_CT_PROLINE_SUPPORT) || defined(TCSUPPORT_CT_PON_CZ_GD)
	fileRead(COMPILE_TIME_PATH, compileTime, sizeof(compileTime));
	cfg_set_object_attr(DEVICEINFO_NODE, "CompileTime", compileTime);
#endif

#if defined(TCSUPPORT_CT_E8GUI)
#if defined(TCSUPPORT_GENERAL_MULTILANGUAGE)
	cfg_obj_get_object_attr(LANGUAGESWITCH_ENTRY_NODE, "Language", 0, language, sizeof(language)) ;
	if(!strcmp(language,"Chinese")){
		fileRead(GATEWAY_PATH, DeviceInfoTemp, sizeof(DeviceInfoTemp));
	}else if(!strcmp(language,"English")){
		fileRead(GATEWAY_EN_PATH, DeviceInfoTemp, sizeof(DeviceInfoTemp));
	}
#else
	fileRead(GATEWAY_PATH, DeviceInfoTemp, sizeof(DeviceInfoTemp));		
#endif
	parsedStr(DeviceInfoTemp,gateWayName);
	cfg_set_object_attr(DEVICEINFO_NODE, "GateWay", gateWayName);
#endif
	
	now_time=time(&timep);
	if(attr != NULL)
	{ 
		/*get the time server address, then ping it to check it exist or not*/
		if(strcmp(attr,"cur_time") == 0)
		{
			memset(nodePath, 0, sizeof(nodePath));
			snprintf(nodePath, sizeof(nodePath), TIMEZONE_ENTRY_NODE);
			if(cfg_obj_get_object_attr(nodePath, "TYPE", 0, tz_type, sizeof(tz_type)) < 0)
			{
				return FAIL;
			}
			
			if(atoi(tz_type) == TZ_MANUAL_TYPE)
			{	
				cfg_set_object_attr(DEVICEINFO_NODE, "cur_time", ctime(&timep));
			}
			else if(atoi(tz_type) == TZ_NTP_TYPE)
			{
				cfg_obj_get_object_attr(globalPath, "ntp_server_ret", 0, ntpServer, sizeof(ntpServer));
				if(atoi(ntpServer) == 0)
				{
					printf("get right time\n");
					cfg_set_object_attr(DEVICEINFO_NODE, "cur_time", ctime(&timep));
				}
				else
				{
					printf("ntp failed\n");
					cfg_set_object_attr(DEVICEINFO_NODE, "cur_time", "N/A (Can't find NTP server)");
				}
			}
		}
		else if( 0 == strcmp(attr, "cur_time2") ) 
		{
			memset(buf, 0, sizeof(buf));
			pTm = localtime(&timep);
			if ( pTm )
			{
			   snprintf(buf, sizeof(buf), "%d/%d/%d/%d/%d", 1+pTm->tm_mon, pTm->tm_mday, 1900+pTm->tm_year, pTm->tm_hour, pTm->tm_min);
			   cfg_set_object_attr(DEVICEINFO_NODE, attr, buf);
			}
		}
	}

	sysinfo(&info);
	sec = info.uptime % 60;
	min = (int) info.uptime / 60;
	hour= (min / 60) % 24;
	min %= 60;
	
	snprintf(tmpTimeFormatStr, sizeof(tmpTimeFormatStr), "%02d:%02d:%02d", 
		hour, min, sec) ;
	cfg_set_object_attr(DEVICEINFO_NODE,"passedSystime",tmpTimeFormatStr);
#if 0
	/*get system startup time to diff with current time*/
		snprintf(sysinfoPath, sizeof(sysinfoPath), SYSINFO_ENTRY_NODE);
		if(cfg_obj_get_object_attr(sysinfoPath, SYSTEMSTARTUPTIME, 0, tmpTimeStr, sizeof(tmpTimeStr)) > 0)
		{
			tmpSysStartTime = (time_t)(atoi(tmpTimeStr));
			passedTime = now_time - tmpSysStartTime;
			min = (int) passedTime / 60;
			hour = (min / 60) % 24;
			min %= 60;
			if(hour){
			timeFormatIdx += sprintf(tmpTimeFormatStr+timeFormatIdx,"%2d:%02d",hour, min);
			}
			else{
				if(min){
					timeFormatIdx += sprintf(tmpTimeFormatStr+timeFormatIdx,"00:%02d",min);
				}
				else{
					timeFormatIdx += sprintf(tmpTimeFormatStr+timeFormatIdx,"00:00");
				}
			}
			timeFormatIdx += sprintf(tmpTimeFormatStr+timeFormatIdx,":%d", (int)passedTime % 60);
			/*after getting passed system time from startup, write into deviceInfo node*/
		 cfg_set_object_attr(DEVICEINFO_NODE, "passedSystime", tmpTimeFormatStr);
		}
		else
		{
			return FAIL;
		}
#endif

	return SUCCESS;
}


static int cfg_type_deviceinfo_set_pvc_info(char* path) 
{
	char dev_pvc_attr[][16]=
	{
		{"Status"},{"pppConnTime"},{"DispConnBtnType"},
		{"DispPPPconn"},{"DispBtnType"},
		{"WanIP"},{"WanSubMask"},
		{"DNSIP"},{"SECDNSIP"},
		{"WanDefGW"},{"connType"},
		{"gateway"},
		{""},
	};
	char adsl_stat[32]={0};
	char buf[32]={0};
	char pppUptimePath[32]={0};
	char dev[8]={0};
	char pvc_stat[4]={0};
	int cur_pvc=0;
	char pvc_info[16]={0};
	char isp_conf_path[20]={0};
	int i=0;
	int now_time = 0,start_time = 0,connect_time = 0;/*for caclated ppp link-up time*/
	int day = 0,hour = 0,min = 0, sec = 0;
	char nodePath[64] = {0};
	char strPath[6][32] = {0};
	char pvcGateway[32] = {0};
	char copyPath[64] = {0};
	int xdsl_type = -1;

	if (path== NULL )
	{
		return -1;
	}
	strncpy(copyPath, path, sizeof(copyPath)-1); 
	memset(strPath, 0, sizeof(strPath));
	cutPath(copyPath,strPath);	 /* will change copyPath ,so need copy path */
	
	cfg_delete_object(path);
	if(cfg_create_object(path) < 0)
	{
		return FAIL;
	}

	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), "root.wan.%s.%s", strPath[2],strPath[3]);    
	cfg_obj_get_object_attr(nodePath, NODE_ACTIVE, 0, pvc_stat, sizeof(pvc_stat)) ;
	
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), XPON_COMMON_NODE);
	cfg_obj_get_object_attr(nodePath, "trafficStatus", 0, adsl_stat, sizeof(adsl_stat)) ;
#endif	
#else
	blapi_xdsl_get_xdsl_linkstatus(adsl_stat, &xdsl_type); 
#endif


	/*Init device_info from Status to DispBtnType*/
	memset(pvc_info, 0, sizeof(pvc_info));
	for(i=Status; i <= DispBtnType; i++)
	{
		cfg_set_object_attr(path, dev_pvc_attr[i], INIT) ;
	}
	cur_pvc= atoi (strPath[3]);
	
	snprintf(isp_conf_path, sizeof(isp_conf_path), ISPCONFIG_PATH, cur_pvc);
	get_profile_str("ISP=",buf, sizeof(buf), QMARKS,isp_conf_path);

	if((strcmp(pvc_stat, ACTIVE)==0) && (strstr(adsl_stat,"up")) )
	{
		/*
			If the pvc is actived and aslo adsl link is up.
			We are start to filled those configure node information.
		*/
		/*krammer change:
		change the getWanInfo,getDefGW to return a char pointer, by this way, we can use only
		one dimension array and do not need to save the value of attribute twice.
		*/
		switch(atoi(buf))
		{
#ifndef PURE_BRIDGE
			case WAN_ENCAP_DYN_INT:/*Dynamic IP Address*/
				cfg_set_object_attr(path, dev_pvc_attr[DispBtnType], CONNECTED) ;
		#ifdef TCSUPPORT_DNSEACHPVC
				snprintf(dev, sizeof(dev), NAS_IF, cur_pvc);				
				if(strlen(getWanInfo(Ip_type,dev, pvc_info))>0)
				{
					cfg_set_object_attr(path, dev_pvc_attr[WanIP],getWanInfo(Ip_type,dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[Status], CONNECTED);
					cfg_set_object_attr(path, dev_pvc_attr[WanDefGW],getDefGW(pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[WanSubMask],getWanInfo(Netmask_type,dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[DNSIP], getDNSInfo("Pridns","Dynamic", dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[SECDNSIP], getDNSInfo("Secdns","Dynamic", dev, pvc_info));
					/*Add for set gateway info*/
					cfg_set_object_attr(path, dev_pvc_attr[Gateway], getPVCGateway(cur_pvc,pvcGateway,sizeof(pvcGateway))); 	
				}
				break;
		#endif
	
			case WAN_ENCAP_STATIC_INT:/*Static IP Address*/
				sprintf(dev,NAS_IF, cur_pvc);
				if(strlen(getWanInfo(Ip_type,dev, pvc_info))>0)
				{
					cfg_set_object_attr(path, dev_pvc_attr[WanIP],getWanInfo(Ip_type,dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[Status], CONNECTED);
					cfg_set_object_attr(path, dev_pvc_attr[WanDefGW],getDefGW(pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[WanSubMask],getWanInfo(Netmask_type,dev, pvc_info));
		#ifndef TCSUPPORT_DNSEACHPVC
					cfg_set_object_attr(path, dev_pvc_attr[DNSIP], getWanInfo(Pridns_type,dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[SECDNSIP], getWanInfo(Secdns_type,dev, pvc_info)); 
		#else
					cfg_set_object_attr(path, dev_pvc_attr[DNSIP], getDNSInfo("Pridns","Static", dev, pvc_info));
					cfg_set_object_attr(path, dev_pvc_attr[SECDNSIP], getDNSInfo("Secdns","Static", dev, pvc_info));
		#endif
					/*Add for set gateway info*/
					cfg_set_object_attr(path, dev_pvc_attr[Gateway], getPVCGateway(cur_pvc,pvcGateway,sizeof(pvcGateway))); 	
				}
				break;
			case WAN_ENCAP_PPP_INT:/*PPPoA && PPPoE*/
				if(checkPpp(cur_pvc))
				{
					sprintf(dev,PPP_IF, cur_pvc);
					sprintf(pppUptimePath,"/tmp/pppuptime-ppp%d",cur_pvc);
					fileRead(pppUptimePath, pvc_info, sizeof(pvc_info));
					/*If this file is exist, it's mean the cpe is dialup PPP Connection.*/
					if(strlen(pvc_info) > 0)
					{
						/*Caculate ppp link-up time*/
						start_time=atoi(pvc_info);
						connect_time=now_time-start_time;
						day=connect_time/SEC_PER_DAY;
						hour=(connect_time-SEC_PER_DAY*day)/SEC_PER_HOUR;
						min=(connect_time-SEC_PER_DAY*day-SEC_PER_HOUR*hour)/SEC_PER_MIN;
						sec=connect_time-SEC_PER_DAY*day-SEC_PER_HOUR*hour-SEC_PER_MIN*min;
						snprintf(pvc_info, sizeof(pvc_info), "%d:%2d:%2d:%2d",day,hour,min,sec);
						cfg_set_object_attr(path, dev_pvc_attr[PppConnTime],pvc_info);
						/*End caculate ppp link-up time*/

						/*If the pppuptime-ppp0~7 file is exist, it's mean the ppp is dial up.*/
						cfg_set_object_attr(path, dev_pvc_attr[WanDefGW],getDefGW(pvc_info));
						cfg_set_object_attr(path, dev_pvc_attr[Status],CONNECTED);
						cfg_set_object_attr(path, dev_pvc_attr[DispPPPconn],CONNECTED);
						cfg_set_object_attr(path, dev_pvc_attr[WanIP],getWanInfo(Ip_type,dev, pvc_info));
						cfg_set_object_attr(path, dev_pvc_attr[WanSubMask],getWanInfo(Netmask_type,dev, pvc_info));
						
			#ifndef TCSUPPORT_DNSEACHPVC
						cfg_set_object_attr(path, dev_pvc_attr[DNSIP],getWanInfo(Pridns_type,dev, pvc_info));
						cfg_set_object_attr(path, dev_pvc_attr[SECDNSIP], getWanInfo(Secdns_type,dev, pvc_info)); 
			#else
						cfg_set_object_attr(path, dev_pvc_attr[DNSIP], getDNSInfo("Pridns","PPP", dev, pvc_info));
						cfg_set_object_attr(path, dev_pvc_attr[SECDNSIP], getDNSInfo("Secdns","PPP", dev, pvc_info));
			#endif
						/*Add for set gateway info*/
						cfg_set_object_attr(path, dev_pvc_attr[Gateway], getPVCGateway(cur_pvc,pvcGateway,sizeof(pvcGateway))); 	
					}
				}
				break;
#endif
			case WAN_ENCAP_BRIDGE_INT:/*bridge mode*/
				cfg_set_object_attr(path, dev_pvc_attr[Status], CONNECTED);
				break;
			default:
				break;
		}
	}


	/*2008 10 03 krammer add
	the following is to get the ISP type by reading /etc/isp(cur_pvc).conf
	if the ISP type is PPPoA or PPPoE, get the ENCAP value to decide the
	type is PPPoA or PPPoE. This connection type will show on web page
	even if pvc dosen't be actived and adsl isn't up, so I use other side to
	deside connection type instead of putting it into upper switch.
	*/
	switch(atoi(buf))
	{
#ifndef PURE_BRIDGE
		case WAN_ENCAP_DYN_INT:/*Dynamic IP Address*/
			cfg_set_object_attr(path, dev_pvc_attr[connType],"Dynamic IP");
			break;
		case WAN_ENCAP_STATIC_INT:/*Static IP Address*/
			cfg_set_object_attr(path, dev_pvc_attr[connType],"Static IP");
			break;
		case WAN_ENCAP_PPP_INT:/*PPPoA && PPPoE*/
			memset(buf,0,sizeof(buf));
			sprintf(isp_conf_path, ISPCONFIG_PATH, cur_pvc);
			get_profile_str("ENCAP=",buf, sizeof(buf), QMARKS,isp_conf_path);
			if(!(strcmp(buf, PPPoE_LLC)&&strcmp(buf, PPPoE_VC_Mux)))
			{
				cfg_set_object_attr(path, dev_pvc_attr[connType],"PPPoE");
			}
			else if(!(strcmp(buf, PPPoA_LLC)&&strcmp(buf, PPPoA_VC_Mux)))
			{
				cfg_set_object_attr(path, dev_pvc_attr[connType],"PPPoA");
			}
			else
			{
				cfg_set_object_attr(path, dev_pvc_attr[connType],"Unknown type");
			}
			get_profile_str("CONNECTION=",buf, sizeof(buf), QMARKS,isp_conf_path);
			if(!strcmp(buf, CONNECT_TYPE_MANUALLY))
			{
				cfg_set_object_attr(path, dev_pvc_attr[DispConnBtnType], CONNECT_DISP_BUTTON);
			}
			break;
#endif
		case WAN_ENCAP_BRIDGE_INT:/*bridge mode*/
		default:
			cfg_set_object_attr(path, dev_pvc_attr[connType],"Bridge");
			break;
	}
	
	return SUCCESS;
}

static int cfg_type_deviceinfo_func_get(char* path,char* attr, char* val,int len) 
{	
	cfg_type_deviceinfo_set_common_info(path,attr);
	return cfg_type_default_func_get(path,attr,val,len);
}

static int cfg_type_deviceinfo_pvc_func_get(char* path,char* attr, char* val,int len) 
{
	if( 0 == cfg_type_deviceinfo_set_common_info(path,attr))
	{	
		cfg_type_deviceinfo_set_pvc_info(path);
	}
	return cfg_type_default_func_get(path,attr,val,len);
}

static int cfg_type_deviceinfo_deviceParaDynamic_func_get(char* path,char* attr, char* val,int len) 
{	
	char flag[4] = {0};

	cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "deviceInfoRead", flag, sizeof(flag));

	if(0 == strcmp(flag, "1"))
	{
		return cfg_type_default_func_get(path,attr,val,len);
	}
		
	cfg_type_deviceInfo_deviceParaDynamic_read(path);
	cfg_type_deviceinfo_set_common_info(path,attr);
	
	return cfg_type_default_func_get(path,attr,val,len);
}

static int cfg_type_deviceinfo_deviceParaStatic_func_get(char* path,char* attr, char* val,int len) 
{	
	cfg_type_deviceInfo_deviceParaStatic_read(path);
	cfg_type_deviceinfo_set_common_info(path,attr);
	return cfg_type_default_func_get(path,attr,val,len);
}

int cfg_type_deviceinfo_execute(char* path)
{
#ifndef PURE_BRIDGE
	char filepath[32]={0};
	char pid[8]={0};
	char nasname[8] = {0};
	char DispConnBtnType[8] = {0};

	cfg_obj_get_object_attr(path, "DipNasName", 0, nasname, sizeof(nasname));
	
	memset(DispConnBtnType, 0, sizeof(DispConnBtnType));
	cfg_obj_get_object_attr(path, "DispBtnType", 0, DispConnBtnType, sizeof(DispConnBtnType));
	
	if((atoi(DispConnBtnType)==RENEW_IP) || (atoi(DispConnBtnType)==RELEASE_IP))
	{
		/*Get udhcpc process-id to send signal to release ip or renew ip*/
		snprintf(filepath, sizeof(filepath), UDHCPC_PID_PATH, nasname);
		fileRead(filepath, pid, sizeof(pid));
		
		if(strlen(pid)==0)
		{
			fprintf(stderr,"None active udhcpc process\r\n");
			printf("\n [%s]:line = %d,None active udhcpc process,return -1 ===============>\n",__FUNCTION__, __LINE__);
			return FAIL;
		}

		if(atoi(DispConnBtnType)==RENEW_IP)
		{
			/*Renew WAN IP*/
			kill(atoi(pid), SIGUSR1);
		}
		else if(atoi(DispConnBtnType)==RELEASE_IP)
		{
			/*Release WAN IP*/
			kill(atoi(pid), SIGUSR2);
		}
	}
#ifdef IPV6
	else if(atoi(DispConnBtnType)==RENEW_IP6)
	{
		/*Renew WAN IP6*/
		cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_WAN_RENEW6, nasname);	
	}
	else if(atoi(DispConnBtnType)==RELEASE_IP6)
	{
		/*Release WAN IP6*/
		cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_WAN_RELEASE6, nasname);
	}
#endif
	else
	{
		return FAIL;
	}
#endif

	return SUCCESS;
}/*end deviceInfo_execute*/

int cfg_type_deviceinfo_pvc_func_commit(char* path)
{
	return cfg_type_deviceinfo_execute(path);
}

static char* cfg_type_deviceinfo_index[] = { 
	 "dev_pvc", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_deviceinfo_devparastatic_ops  = { 
	.get = cfg_type_deviceinfo_deviceParaStatic_func_get,
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	  .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_deviceinfo_devparastatic = { 
	 .name = "devParaStatic", 
	 .flag = CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_deviceinfo, 
	 .index = cfg_type_deviceinfo_index, 
	 .ops = &cfg_type_deviceinfo_devparastatic_ops, 
}; 

static cfg_node_ops_t cfg_type_deviceinfo_devparadynamic_ops  = { 
	.get = cfg_type_deviceinfo_deviceParaDynamic_func_get,
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	  .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_deviceinfo_devparadynamic = { 
	 .name = "devParaDynamic", 
	 .flag = CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_deviceinfo, 
	 .index = cfg_type_deviceinfo_index, 
	 .ops = &cfg_type_deviceinfo_devparadynamic_ops, 
}; 

static cfg_node_ops_t cfg_type_deviceinfo_pvc_ops  = { 
	.get = cfg_type_deviceinfo_pvc_func_get,
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_deviceinfo_pvc_func_commit 
}; 


static cfg_node_type_t cfg_type_deviceinfo_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  CFG_TYPE_FLAG_MULTIPLE |CFG_TYPE_FLAG_MULTIPLE | 1 , 
	 .parent = &cfg_type_deviceinfo, 
	 .index = cfg_type_deviceinfo_index, 
	 .ops = &cfg_type_deviceinfo_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_deviceinfo_ops  = { 
	 .get = cfg_type_deviceinfo_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_deviceinfo_child[] = { 
	&cfg_type_deviceinfo_devparastatic,
	&cfg_type_deviceinfo_devparadynamic,
	 &cfg_type_deviceinfo_pvc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_deviceinfo = { 
	 .name = "DeviceInfo", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY |  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_deviceinfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_deviceinfo_child, 
	 .ops = &cfg_type_deviceinfo_ops, 
}; 
