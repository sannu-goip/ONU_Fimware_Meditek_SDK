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
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <sys/socket.h>    /* for connect and socket*/
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/if.h>
#include <fcntl.h>
#include <linux/wireless.h>
#include <linux/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mxml.h>
#ifdef TCSUPPORT_WLAN
#include <libapi_lib_wifimgr.h>
#endif
#include <cfg_cli.h> 
#include <cfg_api.h>
#include "cfg_types.h"
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
#include <pon_api.h>
#include "xpon_public_const.h"
#endif
#endif
#include "cfg_type_info.h"
#include "../event_api.h"
#include <utility.h>
#include "blapi_traffic.h"
#include "blapi_xdsl.h"

#define MAX_BUF_SIZE 	2048
#define MAX_NODE_NAME 	32
#define MAXGET_PROFILE_SIZE 128
#ifndef PURE_BRIDGE
#define ETHERINFO_PATH	"/proc/tc3162/eth_stats"
#endif


#if defined(TCSUPPORT_CT_E8GUI)
#define IFINFO_ETHCMD1_PATH	"/tmp/ifinfo_ethcmd_stats1"
#define DVID_PATH			"/etc/dvid.conf"
#define HDVER_PATH			"/etc/hdver.conf"
#define SDVER_PATH			"/etc/sdver.conf"
#define BATCODE_PATH		"/etc/batcode.conf"
#if defined(TCSUPPORT_CT_JOYME)
#define GATEWAY_PATH		"/usr/osgi/config/gateway.conf"
#else
#define GATEWAY_PATH		"/etc/gateway.conf"
#endif
#define DEVDEF_PATH			"/etc/devInf.conf"
#define MODEL_PATH			"/etc/model.conf"
#endif
#define BR_STB_LIST_PATH	"/proc/br_fdb_host/stb_list"
#define DHCPLEASE_PATH 		"/etc/udhcp_lease"
#define IFINFO_PATH			"/tmp/ifinfo_stats"
#define SEC_PER_DAY 		86400 /*60*60*24  for ppp link-up time*/
#define SEC_PER_HOUR 		3600 /*60*60         shnwind 2008.4.14*/
#define SEC_PER_MIN 		60

#define MACRO_CMD_STRUCT(name)			struct PONMGR_##name##_S

#define WLANINFO_PATH		"/tmp/wlan_stats"
#if defined(TCSUPPORT_WLAN_AC)
#define WLAN11ACINFO_PATH		"/tmp/wlan11ac_stats"
#endif

#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
#define WIFI_AC_IS_EXIST_CMD	"/sbin/ifconfig rai%d > /tmp/wireless_ac_stat.tmp"
#endif

#if defined(TCSUPPORT_CT_DSL_EX)
#define DSL_ADSL						0
#define DSL_VDSL						1
#define MODULATIONTYPE_T1_413 			"T1.413"
#define MODULATIONTYPE_G_DMT 			"G.DMT"
#define MODULATIONTYPE_G_LITE 			"G.lite"
#define MODULATIONTYPE_ADSL2 			"ADSL2"
#define MODULATIONTYPE_ADSL2PLUS 		"ADSL2+"
#define MTENSTANDARD_ADSL2 				"G.dmt.bis"
#define MTENSTANDARD_ADSL2PLUS 			"G.dmt.bisplus"
#define ANNEXTYPEA_L					"Annex L"
#define ANNEXTYPEA_M					"Annex M"
#define INIT							"0"
#define MODULATIONTYPE_AUTOSYNCUP 		"Auto Sync-Up"
#define OPMODE_ANSI_T1_413				"ANSI T1.413"
#define OPMODE_ITU_G_992_1_GDMT			"ITU G.992.1(G.DMT)"
#define OPMODE_ITU_G_992_2_GLITE		"ITU G.992.2(G.Lite)"
#define OPMODE_ITU_G_992_3_ADSL2		"ITU G.992.3(ADSL2)"
#define OPMODE_ITU_G_992_5_ADSL2PLUS	"ITU G.992.5(ADSL2PLUS)"
#define OPMODE_ITU_G_993_2_VDSL2		"ITU G.993.2(VDSL2)"
#define VDSL_INTERFACE_CFG_PATH 		"/proc/tc3162/vdsl_interface_config"
#endif

#if defined(TCSUPPORT_WLAN_AC)

#define PHY_MODE_11A_ONLY 2
#define PHY_MODE_11AN_MIXED 8
#define PHY_11VHT_N_A_MIXED 14 
#define PHY_11VHT_N_MIXED 15

#if defined(TCSUPPORT_WLAN_AX)
#define PHY_MODE_N_AC_AX_MIX 17
#endif
/* check if wireless mode is 11n 5G band */
int Is11nWirelessMode5G(){
	int is11nmode=0;
  	char node_name[128] = {0};
	char tmp[160] = {0};

	sprintf(node_name, WLAN11AC_COMMON_NODE);
	if(cfg_obj_get_object_attr(node_name, "WirelessMode", 0, tmp, sizeof(tmp)) > 0){
		if((atoi(tmp)==PHY_MODE_11AN_MIXED) || (atoi(tmp)==PHY_11VHT_N_A_MIXED) || (atoi(tmp)==PHY_11VHT_N_MIXED)
#if defined(TCSUPPORT_WLAN_AX)
					|| (atoi(tmp)==PHY_MODE_N_AC_AX_MIX)
#endif
		){
			is11nmode=1;
		}
	}
	
	return is11nmode;
}
	
int doCheck11acHtExtcha(char* path, int isAuto)
{
	char node_name[128] = {0};
	char tmpBW[8] = {0}, tmpChannel[8] = {0}, tmpHtExtcha[8] = {0}, curChannel[8] = {0};
	int tempChannel;
	char HT5GExtCh[22][2] = 
	{
		{"1"}, /* 36's extension channel -> 40 */
		{"0"}, /* 40's extension channel -> 36 */
		{"1"}, /* 44's extension channel -> 48 */
		{"0"}, /* 48's extension channel -> 44 */
		{"1"}, /* 52's extension channel -> 56 */
		{"0"}, /* 56's extension channel -> 52 */
		{"1"}, /* 60's extension channel -> 64 */
		{"0"}, /* 64's extension channel -> 60 */
		{"1"}, /* 100's extension channel -> 104 */
		{"0"}, /* 104's extension channel -> 100 */
		{"1"}, /* 108's extension channel -> 112 */
		{"0"}, /* 112's extension channel -> 108 */
		{"1"}, /* 116's extension channel -> 120 */
		{"0"}, /* 120's extension channel -> 116 */
		{"1"}, /* 124's extension channel -> 128 */
		{"0"}, /* 128's extension channel -> 124 */
		{"1"}, /* 132's extension channel -> 136 */
		{"0"}, /* 136's extension channel -> 132 */
		{"1"}, /* 149's extension channel -> 153 */
		{"0"}, /* 153's extension channel -> 149 */
		{"1"}, /* 157's extension channel -> 161 */
		{"0"}, /* 161's extension channel -> 157 */
	};
	
	if(Is11nWirelessMode5G()){
		sprintf(node_name, WLAN11AC_COMMON_NODE);
		if((cfg_obj_get_object_attr(node_name, "HT_BW", 0, tmpBW, sizeof(tmpBW)) > 0)
		&& (cfg_obj_get_object_attr(node_name, "Channel", 0, tmpChannel, sizeof(tmpChannel)) > 0)
		&& (cfg_obj_get_object_attr(node_name, "HT_EXTCHA", 0, tmpHtExtcha, sizeof(tmpHtExtcha)) > 0))
		{
			if((atoi(tmpBW)==1)){	
				if(atoi(tmpChannel) == 0 && isAuto == 1){ /*auto channel*/
					sprintf(node_name, INFO_WLAN11AC_NODE);
					if(cfg_obj_get_object_attr(node_name, "CurrentChannel", 0, curChannel, sizeof(curChannel))>0)
					{
						strcpy(tmpChannel,curChannel);
					}
				}					

				sprintf(node_name, WLAN11AC_COMMON_NODE);
				tempChannel = atoi(tmpChannel);
				if ((tempChannel >= 36) && (tempChannel <= 64)) {
					tempChannel /= 4;
					tempChannel -= 9;
					cfg_set_object_attr(node_name, "HT_EXTCHA", HT5GExtCh[tempChannel]);
				}
				else if ((tempChannel >= 100) && (tempChannel <= 136)) {
					tempChannel /= 4;
					tempChannel -= 17;
					cfg_set_object_attr(node_name, "HT_EXTCHA", HT5GExtCh[tempChannel]);
				}
				else if ((tempChannel >= 149) && (tempChannel <= 161)) {
					tempChannel -= 1;
					tempChannel /= 4;
					tempChannel -= 19;
					cfg_set_object_attr(node_name, "HT_EXTCHA", HT5GExtCh[tempChannel]);
				}
				else {
					cfg_set_object_attr(node_name, "HT_EXTCHA", "0");
				}	
			}
		}
	}
	return 0;
}
#endif

int isInfoReading(char *attr)
{
	char node_name[128] = {0};
	char isRead[12] = "0";

	if (NULL == attr)
		return 0;

	sprintf(node_name, WEBCURSET_ENTRY_NODE);

	if ( cfg_obj_get_object_attr(node_name, attr, 0, isRead, sizeof(isRead)) < 0
													|| 0 == isRead[0] )
		snprintf(isRead, sizeof(isRead), "%s", "0");
	
	return atoi(isRead);
}

static unsigned long int get_key_ifvlaue(char *str, char *keyword, int base)
{
	char *pch_type = NULL;
	char *pch_itemname = NULL;
	char *pch_temp = NULL;
	char value[11]={0};
	char type[4] = {0};
	char itemname[16] = {0};

	pch_itemname = strstr(str, keyword);
	if (pch_itemname != NULL){
		pch_temp = strstr(pch_itemname,":");
		if (pch_temp != NULL){
			strncpy(value,pch_temp+1, 10);
			return strtoul(value,NULL, base);
		}
	}
	else {
		memset(type, 0, sizeof(type));
		strncpy(type, keyword, sizeof(type) - 1);
		pch_type = strstr(str, type);
		if ( NULL != pch_type ){
			memset(itemname, 0, sizeof(itemname));
			strncpy(itemname, keyword + 3, sizeof(itemname) - 1);
			pch_itemname = strstr(pch_type, itemname);
			if (NULL != pch_itemname){
				pch_temp = strstr(pch_itemname, ":");
				if (NULL != pch_temp){
					strncpy(value, pch_temp + 1, 10);
					return strtoul(value, NULL, base);
				}
			}
		}
	}
	
	return 0;
}

static unsigned long int get_key_vlaue(char *str, char *keyword, int base){
	char *pch_itemname 	= NULL;
	char *pch_temp 		= NULL;
	char value[11]		= {0};

	pch_itemname = strstr(str, keyword);
	if (pch_itemname != NULL){
		pch_temp = strstr(pch_itemname,"= ");
		if (pch_temp != NULL){
			strncpy(value,pch_temp+2, 10);
			return strtoul(value,NULL, base);
		}
	}
	return 0;
}


#if defined(TCSUPPORT_CUC_CWMP_PARAMETER) || defined(TCSUPPORT_CMCCV2)
struct stats {
        unsigned int    user;
        unsigned int    nice;
        unsigned int    system;
        unsigned int    idle;
        unsigned int    iowait;
        unsigned int    irq;
        unsigned int    softirq;
        unsigned int    total;
};
#endif


static int memory_read(char *memoryUsage, char *memoryTotal)
{
    FILE *fp 				= NULL;
    char buf[40] 			= {0};
    unsigned long temp = 0;
    unsigned long memTotal 	= 0;
    unsigned long memFree 	= 0;
    unsigned long buffers 	= 0;
    unsigned long cached 	= 0;
	int ram;
    int i = 0;
    
    fp = fopen("/proc/meminfo","r");
    if(NULL == fp)
    {       
    	printf("\r\nopen file error!");
        return -1;
    }

    while( !(feof(fp)) && (i<5) )
	{
        memset(buf,0, sizeof(buf));
        fgets(buf , sizeof(buf) , fp);
        sscanf(buf,"%*s %lu %*s",&temp);
        if(strncmp(buf,"MemTotal:",strlen("MemTotal:")) == 0){
            memTotal = temp;
        } else if(strncmp(buf,"MemFree:",strlen("MemFree:")) == 0){
            memFree = temp;
        } else if(strncmp(buf,"Buffers:",strlen("Buffers:")) == 0){
            buffers = temp;
        } else if(strncmp(buf,"Cached:",strlen("Cached:")) == 0){
            cached = temp;
        }
		
        i++;
    }
	
    if(memTotal != 0)
    {
    	sprintf(memoryUsage,"%lu",((memTotal-memFree-buffers-cached)*100)/memTotal);
    }

	ram = memTotal/1024;
	
	if (ram > 512)
		ram = 1024;
	else if (ram > 256)
		ram = 512;
	else if (ram > 128)
		ram = 256;
	else if (ram > 64)
		ram = 128;
	else if (ram > 0)
		ram = 64;
	else 
		ram = -1;
	sprintf(memoryTotal, "%d", ram);
    fclose(fp);
    return 0;
}

int getPonLanStatus(char* buf)
{
	int ret = -1;
	int portStatus[4] = {0}; 

	ret = blapi_traffic_check_eth_port_status(buf, portStatus);
	if(ret == -1)
	{
		tcdbg_printf("failed to read eth_port_status\n");
			return -1;
		}

	return 0;
}


int getLanPortStatusByMac(char * macstr)
{
	FILE *fp = NULL;
	char tempbuf[512] = {0},port[4] = {0};
	char mac[17] = {0}; 
	int lanStatus[4] = {0};
	int pNum = 0;

	if(macstr == NULL){		
		printf("[%s:%d]: macstr == NULL || buf == NULL\n", __FUNCTION__,__LINE__);
		return -1;
	}

	fp = fopen(BR_STB_LIST_PATH,"r");
	if(fp == NULL){
		printf("[%s:%d]: open %s fail\n", __FUNCTION__,__LINE__,BR_STB_LIST_PATH);
		return -1;
	}
	while(feof(fp) == 0)
	{
		fgets(tempbuf,sizeof(tempbuf),fp);
		sscanf(tempbuf, "%[^=]",port);
		sscanf(tempbuf, "%*[^=]=%s",mac);
		if(strcmp(macstr,mac) == 0) {/*mac find*/
			pNum = atoi(port);
			if (pNum >= 1 && pNum <= 4){
				getPonLanStatus2(lanStatus);
				if (lanStatus[pNum-1] == 1) { /* this lan connect */
					fclose(fp);
					return 1;
	             	}
				else{/* this lan disconnect */	
		            fclose(fp);
					return 0;
				}
           	}
        }
	}
	fclose(fp);
	return -2;
}


static void trimIP(char* str)
{
	char* c = str + strlen(str) - 1;
	while(isspace(*c) && c>str)
	{
		*c ='\0';
		c--;
	}
}

int get_dhcpLease_status(char *strIP, char *pTime, 
									char *phostName, int hostNameLen)
{
	char buf[160] 		= {0};
	char mac[17]  		= {0}; 
	char ip[16]   		= {0};
	char expire[10]		= {0};
	char hostname[128]	= {0};
	//time_t curTime = time(0);
	time_t curTime;
	struct timespec curtime;
	int timeLeft;
	int day,hour,min,sec;
	FILE *fp = NULL;

#if defined(TCSUPPORT_CMCCV2)
	char devicetype[20] = {0};
	char optionvalue[128] = {0};
#endif
	fp=fopen(DHCPLEASE_PATH, "r");
	
	if(fp == NULL){
		return 0;
	}

	clock_gettime(CLOCK_MONOTONIC,&curtime);
	curTime = curtime.tv_sec;

	while (fgets(buf, 160, fp)){
#if defined(TCSUPPORT_CMCCV2)
		sscanf(buf, "%s %s %s %s %s,%s",mac, ip, expire, hostname, devicetype, optionvalue);
#else
		sscanf(buf, "%s %s %s %s", mac, ip, expire, hostname);
#endif
		timeLeft = atoi(expire) - curTime;
		if(timeLeft > 0){
			trimIP(strIP);
			trimIP(ip);
			if(!strcmp(strIP,ip)){
				day = timeLeft/SEC_PER_DAY;
                hour = (timeLeft - SEC_PER_DAY * day) / SEC_PER_HOUR;
                min = (timeLeft - SEC_PER_DAY * day - SEC_PER_HOUR * hour) / SEC_PER_MIN;
                sec = timeLeft-SEC_PER_DAY * day - SEC_PER_HOUR * hour - SEC_PER_MIN * min;
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%d:%d:%d:%d", day, hour, min, sec);
				strcpy(pTime, buf);
				fclose(fp);				
				snprintf(phostName, hostNameLen, "%s", hostname);
				return 1;
			}
		}	
		memset(buf, 0, sizeof(buf));
	}
	fclose(fp);

	return 0;	
}/*end get_dhcpLease_status*/

static int HostNameSwitch(char *mac,char *hostname, int size)
{
	int i;
	char node_name[40] = {0};
	char Num[5] = {0};
	char Mac[32] = {0};
	char Name[32] = {0};

	sprintf(node_name, HOSTNAME_COMMON_NODE);
	cfg_obj_get_object_attr(node_name, "Num", 0, Num, sizeof(Num));
		
	
	for(i = 0; i < atoi(Num); i++)
	{
		memset(node_name,0,sizeof(node_name));
		snprintf(node_name, sizeof(node_name)-1, HOSTNAME_ENTRY_NODE,i+1);
		cfg_obj_get_object_attr(node_name, "Mac", 0, Mac, sizeof(Mac));
		cfg_obj_get_object_attr(node_name, "Name", 0, Name, sizeof(Name));
		
		if(strcmp(mac,Mac) == 0)
		{
			strncpy(hostname,Name,size-1);
			return 1;
		}
	}

	return 0;
}


int ifInfo_ethcmd_read(char* path)
{
	char ifInfo_ethcmd_attr[][16]=
	{
		{"Port1Status"},{"Port2Status"},
		{"Port3Status"},{"Port4Status"},
		{""}
	};
	
	char ifInfo_PortInfo_attr[][16] = 
	{
		{"PortMac"},{"PortIP"},
		{"PortDHCP"},{"PortExpire"},
		{"PortMac2"},{"HostName"},
		{""}
	};

	char ifInfo_PortRate_attr[][20] =
	{
		{"port1CurrentBitRate"},{"port2CurrentBitRate"},
		{"port3CurrentBitRate"},{"port4CurrentBitRate"},
		{""}
	}; 

	char *pch_itemname = NULL;
  	char *pch_temp = NULL;
	char PortName[20] = {0};
	char value[20] = {0};
	int temp_value = 0;

	char buf[512]={0};
	char ip[64]={0},lladdr[15]={0},mac[20]={0},stale[15]={0};
	char portstatus[4][4] = {0};
	int currline=0;
	int i=0;
	char cmd[64]={0};
	char hostname[128] = {0};
	
	char *pBuf[6];
	char buftemp[64] = {0};
	char expireTime[32] = {0};
	char temp[80] = {0};
	int	ret = 0, idx_buf = 0;

    char nodeName[64] = {0};
	char Active[4] = {0};
	char PortMacTmp[20] = {0};
	char PortIPTmp[20] = {0};
	char PortIP6Tmp[20] = {0};
	char PortMacTmpDot[20] = {0};
	char ConnectionType[4] = {0};
	char Port[4] = {0};
#if defined(TCSUPPORT_ANDLINK)
	char CurAPMpde[8] = {0};
#endif
	for(i=0; i<6; i++){
		pBuf[i] = NULL;
		pBuf[i] = (char *)malloc(512*2);
		if(NULL == pBuf[i])
			goto FreeBUF;
		memset(pBuf[i], 0x00, 512*2);
	}
    for(i = 0; i < 64; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), LANHOST2_ENTRY_NODE, i);
		if((cfg_obj_get_object_attr(nodeName, "Active", 0, Active, sizeof(Active)) >= 0) 
			&& (strcmp(Active, "1") == 0))
		{
			memset(PortMacTmp, 0, sizeof(PortMacTmp));
			memset(PortIPTmp, 0, sizeof(PortIPTmp));
			if((cfg_obj_get_object_attr(nodeName, "MAC", 0, PortMacTmp, sizeof(PortMacTmp)) >= 0) 
				&& (cfg_obj_get_object_attr(nodeName, "IP", 0, PortIPTmp, sizeof(PortIPTmp)) >= 0))
			{
				if(((strlen(pBuf[0]) + strlen(PortMacTmp)) > 512*2) 
					|| ((strlen(pBuf[1]) + strlen(PortIPTmp)) > 512*2))
				{
						goto SetAttrVal;
				}
                else
				{
					/*PortMac*/
					memset(buftemp, 0x00, sizeof(buftemp));
					mac_add_dot(PortMacTmp, PortMacTmpDot);
					snprintf(buftemp,sizeof(buftemp), "%s,", PortMacTmpDot);
					strncat(pBuf[0], buftemp, sizeof(buftemp) - 1);				
					/*PortIP*/
					memset(buftemp, 0x00, sizeof(buftemp));
					snprintf(buftemp,sizeof(buftemp), "%s,", PortIPTmp);
					strncat(pBuf[1], buftemp, sizeof(buftemp) - 1);	
#if defined(TCSUPPORT_ANDLINK)
					cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "CurAPMode", 0, CurAPMpde, sizeof(CurAPMpde));
					if(strcmp(CurAPMpde, "Bridge") != 0)
					{
#endif					
						memset(buftemp, 0x00, sizeof(buftemp));
						memset(expireTime, 0x00, sizeof(expireTime));					
						if (!get_dhcpLease_status(PortIPTmp, expireTime, hostname, sizeof(hostname)))
						{
							strncpy(buftemp, "0,",sizeof(buftemp)-1);
						}
						else
						{
							strncpy(buftemp, "1,",sizeof(buftemp)-1);
						}

						/*PortDHCP: static/dhcp*/
						strncat(pBuf[2], buftemp,sizeof(buftemp)-1);
						/*HostName*/
						memset(hostname, 0, sizeof(hostname));
						cfg_obj_get_object_attr(nodeName, "HostName", 0, hostname, sizeof(hostname));					
						idx_buf += snprintf(pBuf[5] + idx_buf, 512*2 - idx_buf, "%s,", hostname);					
						/*PortExpire*/
						memset(buftemp, 0x00, sizeof(buftemp));
						if(strlen(expireTime) != 0)
							snprintf(buftemp,sizeof(buftemp), "%s,", expireTime);
						else
							strncpy(buftemp, "0,",sizeof(buftemp)-1);
						strncat(pBuf[3], buftemp,sizeof(buftemp)-1);
#if defined(TCSUPPORT_ANDLINK)						
					}
#endif					
				}	
			}
#if !defined(TCSUPPORT_ANDLINK)
			if((cfg_obj_get_object_attr(nodeName, "MAC", 0, PortMacTmp, sizeof(PortMacTmp)) >= 0) 
				&& (cfg_obj_get_object_attr(nodeName, "l_IPv6", 0, PortIP6Tmp, sizeof(PortIP6Tmp)) >= 0))
			{
				if(((strlen(pBuf[0]) + strlen(PortMacTmp)) > 512*2) 
					|| ((strlen(pBuf[1]) + strlen(PortIP6Tmp)) > 512*2))
				{
					goto SetAttrVal;
				}
				else
				{				
					/*PortMac*/
					memset(buftemp, 0x00, sizeof(buftemp));
					mac_add_dot(PortMacTmp, PortMacTmpDot);
					snprintf(buftemp,sizeof(buftemp), "%s,", PortMacTmpDot);
					strncat(pBuf[0], buftemp, sizeof(buftemp) - 1);
					/*PortIP*/
					memset(buftemp, 0x00, sizeof(buftemp));
					snprintf(buftemp,sizeof(buftemp), "%s,", PortIP6Tmp);
					strncat(pBuf[1], buftemp, sizeof(buftemp) - 1);
					/*PortDHCP: static; IPv6 is local ip, eg:fe80:xx*/
					memset(buftemp, 0x00, sizeof(buftemp));					
					strncpy(buftemp, "0,",sizeof(buftemp)-1);
					strncat(pBuf[2], buftemp,sizeof(buftemp)-1);				
					/*HostName*/
					memset(hostname, 0, sizeof(hostname));
					cfg_obj_get_object_attr(nodeName, "HostName", 0, hostname, sizeof(hostname));				
					idx_buf += snprintf(pBuf[5] + idx_buf, 512*2 - idx_buf, "%s,", hostname);
					/*PortExpire*/
					memset(buftemp, 0x00, sizeof(buftemp));
					strncpy(buftemp, "0,",sizeof(buftemp)-1);
					strncat(pBuf[3], buftemp,sizeof(buftemp)-1);	
					
				}
			}
#endif
		}
		
	}
	
	getPonLanStatus(temp);
	sscanf(temp, "%s %s %s %s",portstatus[0],portstatus[1],portstatus[2],portstatus[3]);	
	cfg_set_object_attr(path, ifInfo_ethcmd_attr[0], portstatus[0]);
	cfg_set_object_attr(path, ifInfo_ethcmd_attr[1], portstatus[1]);
	cfg_set_object_attr(path, ifInfo_ethcmd_attr[2], portstatus[2]);
	cfg_set_object_attr(path, ifInfo_ethcmd_attr[3], portstatus[3]);

	memset(buf, 0, sizeof(buf));
	ret = blapi_traffic_get_portrate_info(buf, 512);
	for(i = 0; strlen(ifInfo_PortRate_attr[i]) != 0; i++){
		snprintf(PortName, sizeof(PortName), "Port%d", i + 1);
		pch_itemname = strstr(buf, PortName);
		if(pch_itemname != NULL){
			pch_temp = strstr(pch_itemname,":");
			if(pch_temp != NULL){
				pch_temp += 2;
				temp_value = atoi(pch_temp);
				snprintf(value, sizeof(value), "%d", temp_value);
			}
		}
		cfg_set_object_attr(path, ifInfo_PortRate_attr[i], value);
	}

	getLanPortInfo(path);

SetAttrVal:
		for(i=0; strlen(ifInfo_PortInfo_attr[i])!=0; i++)
			cfg_set_object_attr(path, ifInfo_PortInfo_attr[i], pBuf[i]);
FreeBUF:
	for(i=0; i<6; i++){
		if(NULL != pBuf[i]){
			free(pBuf[i]);
			pBuf[i] = NULL;
		}
	}	
	return 0;
}

int cfg_type_etherInfo_read(char* path)
{
	char etherInfo_keywd[][17]=
	{
		{"inOctets"},       {"inUnicastPkts"},
		{"inMulticastPkts"},{"outOctets"},
		{"outUnicastPkts"}, {"outMulticastPkts"},
		{"outErrors"},      {"txCollisionCnt"},
		{"txUnderRunCnt"},  {"rxCrcErr"},
		{""}
	};
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_JOYME2) 
	char f_buf[64] 	= {0};
	FILE *fp 		= NULL;
	int idx_buf 	= 0;
	int buf_len 	= 0;
#endif
	enum ethInfo_en{
		InOctets=0,
		InUnicastPkts,
		InMulticastPkts,
		OutOctets,
		OutUnicastPkts,
		OutMulticastPkts,
		OutErrors,
		TxCollisionCnt,
		TxUnderRunCnt,
		RxCrcErr,
	};
	int rt = -1;
	int flag = 0;
	char value[32]	= {0};
	char *buf 		= NULL;
	int i			= 0;
	unsigned long int ethStatus[10];
#if defined(TCSUPPORT_ANDLINK)
	char ssid[32] = {0};
#endif

	/* get port connection status*/
	ifInfo_ethcmd_read(path);

	if(path){
		buf = (char *)malloc(MAX_BUF_SIZE);
		if(buf == NULL)
		{
			perror("malloc fail");
			return -1;
		}
		memset(buf,0, MAX_BUF_SIZE);
		//get status information
		rt = blapi_traffic_check_eth_link_status(&flag);
		if(flag == 1)
		{
			if(cfg_set_object_attr(path, "status", "Up") < 0)
			{
					printf("cfg_set_object_attr: set status Up fail.\n");
				}
			}
		if(flag == 2)
		{
			if(cfg_set_object_attr(path, "status", "NoLink") < 0)
			{
					printf("cfg_set_object_attr: set status NoLink fail.\n");
				}
			}

		memset(buf,0, MAX_BUF_SIZE);
		fileRead(ETHERINFO_PATH, buf, MAX_BUF_SIZE);
		if(strlen(buf)!=0){
			for(i = 0; strlen(etherInfo_keywd[i]) != 0; i++){
				ethStatus[i] = get_key_vlaue(buf, etherInfo_keywd[i], 16);
				sprintf(value, "%lu", ethStatus[i]);
				if(cfg_set_object_attr(path, etherInfo_keywd[i], value) < 0){
					printf("cfg_set_object_attr: set %s %s fail.\n", etherInfo_keywd[i], value);
				}
			}
			/*rx frame=InUnicastPkts+InMulticastPkts */
			sprintf(value, "%lu", ethStatus[InUnicastPkts] + ethStatus[InMulticastPkts]);
			if(cfg_set_object_attr(path, "rxFrames", value) < 0){
				printf("cfg_set_object_attr: set rxFrames %s fail.\n", value);
			}
			/*tx frame=OutUnicastPkts+OutMulticastPkts */
			sprintf(value, "%lu", ethStatus[OutUnicastPkts] + ethStatus[OutMulticastPkts]);
			if(cfg_set_object_attr(path, "txFrames", value) < 0){
				printf("cfg_set_object_attr: set txFrames %s fail.\n", value);
			}
		}

		memset(value, 0, sizeof(value));
		if(cfg_set_object_attr(path, "ip", getWanInfo(Ip_type, "br0", buf)) < 0){
			printf("cfg_set_object_attr: set ip %s fail.\n", 
								getWanInfo(Ip_type, "br0", buf));
		}
		if(cfg_set_object_attr(path, "mac", getWanInfo(Mac_type, "br0", buf)) < 0){
			printf("cfg_set_object_attr: set ip %s fail.\n", 
								getWanInfo(Mac_type, "br0", buf));
		}
#ifdef IPV6
		if(cfg_set_object_attr(path, "isIPv6Supported", "Yes") < 0){
			printf("cfg_set_object_attr: set isIPv6Supported Yes fail.\n");
		}
#endif
#if defined(TCSUPPORT_VIR_SERVER)
		if(cfg_set_object_attr(path, "isVirServerSupported", "Yes") < 0){
			printf("cfg_set_object_attr: set isVirServerSupported Yes fail.\n");
		}
#endif

		
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_JOYME2)
		memset(buf, 0, MAX_BUF_SIZE);
		fp = fopen("/proc/br_fdb_host/stb_list", "r");
		if(fp){
			memset(f_buf, 0, sizeof(f_buf));
			while (fgets(f_buf, sizeof(f_buf), fp) )
			{
				buf_len = strlen(f_buf);
				if ( '\n' == f_buf[buf_len - 1] )
					f_buf[buf_len - 1] = 0;
				/* update mac host name*/
				idx_buf += snprintf(buf + idx_buf, MAX_BUF_SIZE - idx_buf, "%s,", f_buf);
				memset(f_buf, 0, sizeof(f_buf));
				if ( idx_buf >= MAX_BUF_SIZE )
				{
					break;
				}
			}
			if(cfg_set_object_attr(path, "brHost", buf) < 0){
				printf("cfg_set_object_attr: set brHost %s fail.\n", buf);
			}
			fclose(fp);
		}
#endif
#if defined(TCSUPPORT_ANDLINK)
		if(cfg_obj_get_object_attr(DEVICEINFO_DEVPARADYNAMIC_NODE, "ssid", 0, ssid, sizeof(ssid)) >= 0)
		{
			cfg_set_object_attr(path, "wanhostname", ssid);
		}
#endif
		free(buf);
	}
	else{
		return -1;
	}
	return 0;
}

#if !defined(TCSUPPORT_NP)
int cfg_type_xponInfo_read(char* path)
{
	return 0;
}

#if defined(TCSUPPORT_PONMGR) && (defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON))
static int cfg_type_ponPhyInfo_read(char* path)
{
	char buf[20] = {0};
	int fecMode  = 0;
	enum phy_info{
		Temperature = 0,
		SupplyVoltage,
		TxBiasCurrent,
		RxPower,
		TxPower,
		PhyMaxAttr
	};
	char phy_attr[][32] = {
		{"Temperature"},
		{"SupplyVoltage"},
		{"TxBiasCurrent"},
		{"RxPower"},
		{"TxPower"},
		{""}
	};
	PHY_PARAMS_t PhyTransParams = {0};
	WAN_LINKCFG_t sysLinkCfg = {0};
	PHY_FECCONFIG_t phyFecConfig = {0};
	
	/************* PON PHY Trans Param Info *************/
	if(cfg_pon_get_info(METHD_PHY_TRANS_PARAS, (void *)&PhyTransParams)){
		printf("%s: get method PHY_TRANS_PARAS\n",__FUNCTION__);
		return -1;
	}

#if defined(TCSUPPORT_CT_JOYME)
	char node_name[128]	= {0};
	char attrVal[40] 	= {0};

	//snprintf(nodeName[0], MAX_NODE_NAME - 1, "%s", "XPON");
	//snprintf(nodeName[1], MAX_NODE_NAME - 1, "%s", "Common");
	sprintf(node_name, XPON_COMMON_NODE);
	if (path)
	{
		memset(attrVal, 0, sizeof(attrVal));
		if(cfg_obj_get_object_attr(node_name, "phyStatus", 0, attrVal, sizeof(attrVal)) < 0
													|| NULL == strstr(attrVal, "up")){
			/* reset TxCurrent to 0 when pon down */
			PhyTransParams.txCurrent = 0;
		}
	}
#endif
	
	sprintf(buf, "%u", PhyTransParams.temperature);
	if(cfg_set_object_attr(path, phy_attr[Temperature], buf) < 0){
		printf("cfg_set_object_attr: set %s %s fail.\n", phy_attr[Temperature], buf);
	}

	memset(buf, 0 , 20);
	sprintf(buf, "%u", PhyTransParams.voltage);
	//mxmlElementSetAttr(curNode, phy_attr[SupplyVoltage], buf);
	if(cfg_set_object_attr(path, phy_attr[SupplyVoltage], buf) < 0){
		printf("cfg_set_object_attr: set %s %s fail.\n", phy_attr[SupplyVoltage], buf);
	}
	
	memset(buf, 0 , 20);
	sprintf(buf, "%u", PhyTransParams.txCurrent);
	//mxmlElementSetAttr(curNode, phy_attr[TxBiasCurrent], buf);
	if(cfg_set_object_attr(path, phy_attr[TxBiasCurrent], buf) < 0){
		printf("cfg_set_object_attr: set %s %s fail.\n", phy_attr[TxBiasCurrent], buf);
	}

	memset(buf, 0 , 20);
	if (PhyTransParams.txPower == 0)
		sprintf(buf, "%u", 1);
	else
		sprintf(buf, "%u", PhyTransParams.txPower);
	//mxmlElementSetAttr(curNode, phy_attr[TxPower], buf);
	if(cfg_set_object_attr(path, phy_attr[TxPower], buf) < 0){
		printf("cfg_set_object_attr: set %s %s fail.\n", phy_attr[TxPower], buf);
	}

	memset(buf, 0 , 20);
	if (PhyTransParams.rxPower == 0)
		sprintf(buf, "%u", 1);
	else
		sprintf(buf, "%u", PhyTransParams.rxPower);
	//mxmlElementSetAttr(curNode, phy_attr[RxPower], buf);
	if(cfg_set_object_attr(path, phy_attr[RxPower], buf) < 0){
		printf("cfg_set_object_attr: set %s %s fail.\n", phy_attr[RxPower], buf);
	}

	if (cfg_pon_get_info(METHD_SYS_LINK_CFG, (void *)&sysLinkCfg)){
		printf("%s: get method SYS_LINK_CFG\n",__FUNCTION__);
		return -1;
	}

	if (sysLinkCfg.linkStatus == XMCS_IF_WAN_LINK_EPON){
		if (cfg_pon_get_info(METHD_EPON_LLID_FEC, (void *)&fecMode)){
			printf("%s: get method EPON_LLID_FEC\n",__FUNCTION__);
			return -1;
		}
		if (fecMode == -1) 
			return -1;
		
		memset(buf, 0, 20);
		sprintf(buf, "%u", fecMode); // 0 is disable, 1 is enable
		//mxmlElementSetAttr(curNode, "FecStatus", buf);
		if(cfg_set_object_attr(path, "FecStatus", buf) < 0){
			printf("cfg_set_object_attr: set FecStatus %s fail.\n", buf);
		}
		
	}else{ // default is gpon
		if (cfg_pon_get_info(METHD_PHY_FEC_CFG, (void *)&phyFecConfig)){
			printf("%s: get method PHY_FEC_CFG\n",__FUNCTION__);
			return -1;
		}

		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%u", phyFecConfig.fecStatus);
		//mxmlElementSetAttr(curNode, "FecStatus", buf);
		if(cfg_set_object_attr(path, "FecStatus", buf) < 0){
			printf("cfg_set_object_attr: set FecStatus %s fail.\n", buf);
		}
	}
	return 0;
}

static int cfg_type_ponWanStatsInfo_read(char* path)
{
	char buf[64] = {0};
	enum stats_info{
		TxFrameCnt = 0,
		TxFrameLen,
 		TxDropCnt,
 		TxBroadcastCnt,
 		TxMulticastCnt,
 		TxLess64Cnt,
 		TxMore1518Cnt,
 		Tx64Cnt,
 		Tx65To127Cnt,
 		Tx128To255Cnt,
 		Tx256To511Cnt,
 		Tx512To1023Cnt,
 		Tx1024To1518Cnt,
 		RxFrameCnt,
 		RxFrameLen,
 		RxDropCnt,
 		RxBroadcastCnt,
 		RxMulticastCnt,
 		RxCrcCnt,
 		RxFragFameCnt,
 		RxJabberFameCnt,
 		RxLess64Cnt,
 		RxMore1518Cnt,
 		Rx64Cnt,
 		Rx65To127Cnt,
 		Rx128To255Cnt,
 		Rx256To511Cnt,
 		Rx512To1023Cnt,
 		Rx1024To1518Cnt,
		RxHecErrorCnt,
		RxFecErrorCnt,
		WanStatsMaxAttr,
	};
	char stats_attr[][32] = {
		{"TxFrameCnt"},
		{"TxFrameLen"},
 		{"TxDropCnt"},
 		{"TxBroadcastCnt"},
 		{"TxMulticastCnt"},
 		{"TxLess64Cnt"},
 		{"TxMore1518Cnt"},
 		{"Tx64Cnt"},
 		{"Tx65To127Cnt"},
 		{"Tx128To255Cnt"},
 		{"Tx256To511Cnt"},
 		{"Tx512To1023Cnt"},
 		{"Tx1024To1518Cnt"},
 		{"RxFrameCnt"},
 		{"RxFrameLen"},
 		{"RxDropCnt"},
 		{"RxBroadcastCnt"},
 		{"RxMulticastCnt"},
 		{"RxCrcCnt"},
 		{"RxFragFameCnt"},
 		{"RxJabberFameCnt"},
 		{"RxLess64Cnt"},
 		{"RxMore1518Cnt"},
 		{"Rx64Cnt"},
 		{"Rx65To127Cnt"},
 		{"Rx128To255Cnt"},
 		{"Rx256To511Cnt"},
 		{"Rx512To1023Cnt"},
 		{"Rx1024To1518Cnt"},
		{"RxHecErrorCnt"},
		{"RxFecErrorCnt"},
		{""},
	};
	
	WAN_STATISTIC_t WanCntStats = {0};
	
	/************* PON WAN Counter Stats Info *************/
	if (cfg_pon_get_info(METHD_PWAN_CNT_STATS, (void *)&WanCntStats)){
		printf("%s: get method PWAN_CNT_STATS\n",__FUNCTION__);
		return -1;
	}
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.txFrameCnt);
	cfg_set_object_attr(path, stats_attr[TxFrameCnt], buf);

	snprintf(buf, sizeof(buf), "%llu", WanCntStats.txFrameLen);
	cfg_set_object_attr(path, stats_attr[TxFrameLen], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.txDropCnt);
	cfg_set_object_attr(path, stats_attr[TxDropCnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.txBroadcastCnt);
	cfg_set_object_attr(path, stats_attr[TxBroadcastCnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.txMulticastCnt);
	cfg_set_object_attr(path, stats_attr[TxMulticastCnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.txLess64Cnt);
	cfg_set_object_attr(path, stats_attr[TxLess64Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.txMore1518Cnt);
	cfg_set_object_attr(path, stats_attr[TxMore1518Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx64Cnt);
	cfg_set_object_attr(path, stats_attr[Tx64Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx65To127Cnt);
	cfg_set_object_attr(path, stats_attr[Tx65To127Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx128To255Cnt);
	cfg_set_object_attr(path, stats_attr[Tx128To255Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx256To511Cnt);
	cfg_set_object_attr(path, stats_attr[Tx256To511Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx512To1023Cnt);
	cfg_set_object_attr(path, stats_attr[Tx512To1023Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.tx1024To1518Cnt);
	cfg_set_object_attr(path, stats_attr[Tx1024To1518Cnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxFrameCnt);
	cfg_set_object_attr(path, stats_attr[RxFrameCnt], buf);
	
	snprintf(buf, sizeof(buf), "%llu", WanCntStats.rxFrameLen);
	cfg_set_object_attr(path, stats_attr[RxFrameLen], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxDropCnt);
	cfg_set_object_attr(path, stats_attr[RxDropCnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxBroadcastCnt);
	cfg_set_object_attr(path, stats_attr[RxBroadcastCnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxMulticastCnt);
	cfg_set_object_attr(path, stats_attr[RxMulticastCnt], buf);
	
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxCrcCnt);
	cfg_set_object_attr(path, stats_attr[RxCrcCnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxFragFameCnt);
	cfg_set_object_attr(path, stats_attr[RxFragFameCnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxJabberFameCnt);
	cfg_set_object_attr(path, stats_attr[RxJabberFameCnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxLess64Cnt);
	cfg_set_object_attr(path, stats_attr[RxLess64Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxMore1518Cnt);
	cfg_set_object_attr(path, stats_attr[RxMore1518Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx64Cnt);
	cfg_set_object_attr(path, stats_attr[Rx64Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx65To127Cnt);
	cfg_set_object_attr(path, stats_attr[Rx65To127Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx128To255Cnt);
	cfg_set_object_attr(path, stats_attr[Rx128To255Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx256To511Cnt);
	cfg_set_object_attr(path, stats_attr[Rx256To511Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx512To1023Cnt);
	cfg_set_object_attr(path, stats_attr[Rx512To1023Cnt], buf);

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rx1024To1518Cnt);
	cfg_set_object_attr(path, stats_attr[Rx1024To1518Cnt], buf);

#ifdef TCSUPPORT_WAN_GPON
	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxHecErrorCnt);
	cfg_set_object_attr(path, stats_attr[RxHecErrorCnt], buf);
#endif

	snprintf(buf, sizeof(buf), "%u", WanCntStats.rxFecErrorCnt);
	cfg_set_object_attr(path, stats_attr[RxFecErrorCnt], buf);

	return 0;
}

#endif
#endif
int cfg_type_autoPVCInfo_read(char* path){
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
static const char *ss_fmt[] = {
	"%n%Lu%lu%lu%lu%lu%n%n%n%Lu%lu%lu%lu%lu%lu",
	"%Lu%Lu%lu%lu%lu%lu%n%n%Lu%Lu%lu%lu%lu%lu%lu",
	"%Lu%Lu%lu%lu%lu%lu%lu%lu%Lu%Lu%lu%lu%lu%lu%lu%lu"
};


static void get_dev_info(char *buf, struct wan_info_stats *stats, int procnetdev_vsn)
{
	memset(stats, 0, sizeof(struct wan_info_stats));
	
	sscanf(buf, ss_fmt[procnetdev_vsn],
		   &stats->rx_bytes, /* missing for 0 */
		   &stats->rx_packets,
		   &stats->rx_errors,
		   &stats->rx_dropped,
		   &stats->rx_fifo_errors,
		   &stats->rx_frame_errors,
		   &stats->rx_compressed, /* missing for <= 1 */
		   &stats->rx_multicast, /* missing for <= 1 */
		   &stats->tx_bytes, /* missing for 0 */
		   &stats->tx_packets,
		   &stats->tx_errors,
		   &stats->tx_dropped,
		   &stats->tx_fifo_errors,
		   &stats->collisions,
		   &stats->tx_carrier_errors,
		   &stats->tx_compressed /* missing for <= 1 */
		   );

	if ( procnetdev_vsn <= 1 ) {
		if ( procnetdev_vsn == 0 ) 
		{
			stats->rx_bytes = 0;
			stats->tx_bytes = 0;
		}
		stats->rx_multicast = 0;
		stats->rx_compressed = 0;
		stats->tx_compressed = 0;
	}
}

static inline int procnetdev_version(char *buf)
{
	if (strstr(buf, "compressed"))
		return 2;
	
	if (strstr(buf, "bytes"))
		return 1;
	
	return 0;
}

char *get_ifname(char *name, char *p)
{
	int namestart=0, nameend=0, aliasend;
	
	while ( isspace(p[namestart]) )
		namestart++;
	
	nameend=namestart;
	while ( p[nameend] && p[nameend]!=':' && !isspace(p[nameend]) )
		nameend++;
	
	if ( p[nameend]==':' ) {
		aliasend=nameend+1;
		while ( p[aliasend] && isdigit(p[aliasend]) )
			aliasend++;
		if ( p[aliasend]==':' ) {
			nameend=aliasend;
		}
		if ( (nameend-namestart) < IFNAMSIZE ) {
			memcpy(name,&p[namestart],nameend-namestart);
			name[nameend-namestart]='\0';
			p=&p[nameend];
		} 
		else {
			/* Interface name too large */
			name[0]='\0';
		}
	} 
	else {
		/* first ':' not found - return empty */
		name[0]='\0';
	}
	return p + 1;
}

static int get_itf_stat(char* path, char *ifname)
{
	FILE *fp = NULL;
	char *buf = NULL, value[32] = {0};
	char name[128] = {0}, *s = NULL, *ptmp = NULL;
	struct wan_info_stats stats;
	int procnetdev_vsn = 0, i = 0;
	size_t len = 0;
	unsigned long long ifStats[9] = {0};
	char ifInfo_attr[][16] = {
		{"rxpackets"},		{"txpackets"},
		{"rxbytes"},		{"txbytes"},
		{"rxerrpkts"},		{"txerrpkts"},
		{"rxdroppkts"},		{"txdroppkts"},
		{""}
	};

	enum ifInfo_en{
		RXpackets=0,
		TXpackets,
		RXbytes,
		TXbytes,
		RXerrpkts,
		TXerrpkts,
		RXdroppkts,
		TXdroppkts,
	};
	
	fp = fopen(_PATH_PROCNET_DEV, "r");
	if ( !fp ) 
		return -1;
	
	if(-1 != getline(&buf, &len, fp) && -1 != getline(&buf, &len, fp))
	{
	    procnetdev_vsn = procnetdev_version(buf);
	}
	while ( -1 != getline(&buf, &len, fp) ) {
		memset(name, 0, sizeof(name));
		s = get_ifname(name, buf);
		
		if ( NULL == strstr(name, "nas") && NULL == strstr(name, "eth") 
			&& NULL == strstr(name, "ra") && NULL == strstr(name, "rai")
#if defined(TCSUPPORT_WLAN_APCLIENT)
			&& NULL == strstr(name, "apcli0") && NULL == strstr(name, "apclii0")
#endif
			)
			continue;
#if !defined(TCSUPPORT_NP_CMCC)
		if ( 'n' == name[0] ) {
			ptmp = strstr(name, "_");
			if ( ptmp )
				*ptmp = '.';
		}
#endif
		if ( 0 == strcmp(ifname, name) ) {
			get_dev_info(s, &stats, procnetdev_vsn);

			ifStats[RXpackets] = stats.rx_packets;
			ifStats[TXpackets] = stats.tx_packets;
			ifStats[RXbytes] = stats.rx_bytes;
			ifStats[TXbytes] = stats.tx_bytes;
			ifStats[RXerrpkts] = stats.rx_errors;
			ifStats[TXerrpkts] = stats.tx_errors;
			ifStats[RXdroppkts] = stats.rx_dropped;
			ifStats[TXdroppkts] = stats.tx_dropped;

			for ( i = 0; i <= TXdroppkts; i++ ) {
				snprintf(value, sizeof(value), "%llu", ifStats[i]);
				cfg_set_object_attr(path, ifInfo_attr[i], value);
			}
			break;
		}
	}
	
	fclose(fp);
		
	if(buf)
		free(buf);
	
	return 0;
}

int get_itf_hwaddr(char *path, char *ifname)
{
#if defined(TCSUPPORT_WLAN)
	char hwaddrbuf[20] = {0};
	int ret = 0;
	
	ret = wifimgr_lib_get_itf_hwaddr(ifname, hwaddrbuf, sizeof(hwaddrbuf));
	cfg_set_object_attr(path, "hwaddr", hwaddrbuf);
#endif
	return 0;
}

int loopdetect(char *path, char *ifname)
{
	int ret = -1;
	char Status[4] = {0};
	char detectionStatus[20] = {0};
		
	ret = blapi_traffic_check_loop_detect(ifname, Status, detectionStatus);
	
	cfg_set_object_attr(path, "Status", Status);
	cfg_set_object_attr(path, "detectionStatus", detectionStatus);

    return 0;
}
#endif

int ifInfo_read(char* path, char* type)
{
	char ifInfo_keywd[][16] = {
		{"RX packets"}, 	{"TX packets"},
		{"RX bytes"},  		{"TX bytes"},
		{"RX errors"},  	{"TX errors"},
		{"RX dropped"},  	{"TX dropped"},
		{"HWaddr"},			{""}
	};
	/*set attr name same as keywd name*/
	char ifInfo_attr[][16] = {
		{"rxpackets"},		{"txpackets"},
		{"rxbytes"},		{"txbytes"},
		{"rxerrpkts"},		{"txerrpkts"},
		{"rxdroppkts"},		{"txdroppkts"},
		{"hwaddr"},			{""}
	};
	
	enum ifInfo_en{
		RXpackets=0,
		TXpackets,
		RXbytes,
		TXbytes,
		RXerrpkts,
		TXerrpkts,
		RXdroppkts,
		TXdroppkts,
		HWAddr,
	};

	int res = 0;
	char value[32];
	char *buf = NULL;
	int i=0;
	unsigned long long ifStatus[9] = {0};
	char hwAddr[20] = {0};
	char *hwAddrPtr = NULL;
	char cmd[64]={0};
#if defined(TCSUPPORT_CT_LOOPDETECT)
	char flag = 0;
#endif

	char *info_path = "/tmp/ether_status";
	char tempvalue[2];
	char *tmp = NULL;
	int index,iStatus;	
	FILE *fp = NULL;
	char *pt = NULL;
	char nasnode[12] = {0};
	int ret = -1;
	char Status[4] = {0};
	char detectionStatus[20] = {0};
	
	memset(value, 0, sizeof(value));
	memset(hwAddr, 0, sizeof(hwAddr));
	if(path){
		buf = (char *)malloc(MAX_BUF_SIZE);
		if(NULL == buf){
			perror("malloc fail");
			return -1;
		}
		memset(buf, 0, MAX_BUF_SIZE);

		if (NULL != strstr(type, "nas") || NULL != strstr(type, "br0")){
			strncpy(nasnode, type, sizeof(nasnode) - 1);
			if (NULL != (pt = strstr(nasnode, ".")))
				pt[0] = '_';
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s > %s", nasnode, IFINFO_PATH);
		}	
		else{
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s > %s", type, IFINFO_PATH);
		}
			
		system(cmd);

		fileRead(IFINFO_PATH, buf, MAX_BUF_SIZE);
		if(strstr(buf, "Device not found") != NULL){
			free(buf);
			return -1;
		}
		
		if(strlen(buf) != 0){
			for(i = 0; strlen(ifInfo_keywd[i]) != 0; i++){
				if (!strcmp(ifInfo_keywd[i], "HWaddr")) {
					hwAddrPtr = strstr(buf, "HWaddr");
					if(hwAddrPtr){
						strncpy(hwAddr, hwAddrPtr + strlen("HWaddr") + 1, 17); /*the length of mac addr is 17.*/
					}
				}
				else{
					ifStatus[i] = get_key_ifvlaue(buf, ifInfo_keywd[i], 10);
				}
			}
			for(i=0; strlen(ifInfo_attr[i]) != 0; i++){
				switch(i){
					case RXpackets:/*RXpackets*/
						snprintf(value, sizeof(value), "%llu", ifStatus[RXpackets]);
						break;
					case TXpackets:/*TXpackets*/
						snprintf(value, sizeof(value), "%llu", ifStatus[TXpackets]);
						break;
					case RXbytes:/*RXbytes*/
						snprintf(value, sizeof(value), "%llu", ifStatus[RXbytes]);
						break;
					case TXbytes:/*TXbytes*/
						snprintf(value, sizeof(value), "%llu", ifStatus[TXbytes]);
						break;
					case RXerrpkts:/*RX errors*/
						snprintf(value, sizeof(value), "%llu", ifStatus[RXerrpkts]);
						break;
					case TXerrpkts:/*TX errors*/
						snprintf(value, sizeof(value), "%llu", ifStatus[TXerrpkts]);
						break;
					case RXdroppkts:/*RX dropped*/
						snprintf(value, sizeof(value), "%llu", ifStatus[RXdroppkts]);
						break;
					case TXdroppkts:/*TX dropped*/
						snprintf(value, sizeof(value), "%llu", ifStatus[TXdroppkts]);
						break;
					case HWAddr:/*HWaddr*/
						strcpy(value, hwAddr);
						break;
					default:
						memset(value, 0, sizeof(value));
						break;
				}
				cfg_set_object_attr(path, ifInfo_attr[i], value);
			}
		}
		unlink(IFINFO_PATH);
			
		ret = blapi_traffic_check_loop_detect(type, Status, detectionStatus);
		
		cfg_set_object_attr(path, "Status", Status);
		cfg_set_object_attr(path, "detectionStatus", detectionStatus);

		free(buf);
	}
	else{
		return -1;
	}
	return 0;
}


#ifdef TCSUPPORT_CMCCV2
static void info_UpdateRadioStatics(RADIO_STATISTICS* pRadioStatics,int bandidx)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	RADIO_STATISTICS radioStatics;
	unsigned long BcnFail=0;

	if(pRadioStatics == NULL){
		tcdbg_printf("==>info_UpdateBW: pRadioStatics is NULL\n");
		return;
	}
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>info_UpdateBW: Create socket fail\n");
		return;
	}

	if(bandidx == 0)
		strcpy(wrq.ifr_name, "ra0");
	else if(bandidx == 1)
		strcpy(wrq.ifr_name, "rai0");
	else{
		printf("wrong bandidx=%d\n",bandidx);
		close(socket_id);
		return;
	}
	wrq.u.data.length = sizeof(RADIO_STATISTICS);
	wrq.u.data.pointer = pRadioStatics;
	wrq.u.data.flags = OID_802_11_GET_RADIO_STATISTICS;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>info_UpdateBW: ioctl open fail\n");
    		close(socket_id);
		return;
	}

	close(socket_id);
	return;
}

static void info_UpdateBW(RADIO_STATISTICS radioStatics,int bandidx,char* outBw)
{
	char HT_BW = 0;
	char VHT_BW = 0;
	char tmp[4]			= {0};
	char node_name[128]	= {0};

	
	if(bandidx == 0){
		snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
		if(cfg_get_object_attr(node_name, "HT_BW", tmp, sizeof(tmp)) > 0){
			HT_BW = atoi(tmp);
		}
	}else if(bandidx == 1){
		snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
		if(cfg_get_object_attr(node_name, "HT_BW", tmp, sizeof(tmp)) > 0){
			HT_BW = atoi(tmp);
		}
		if(cfg_get_object_attr(node_name, "VHT_BW", tmp, sizeof(tmp)) > 0){
			VHT_BW = atoi(tmp);
		}
	}else{
		printf("error bandidx!\n");
		return;
	}

	snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);

	switch(radioStatics.CurrentChannelWidth){
		case 0:
			sprintf(outBw,"%d",20);
			break;
		case 1:
			sprintf(outBw,"%d",40);
			break;
		case 2:
			sprintf(outBw,"%d",80);
			break;
		case 3:
			sprintf(outBw,"%d",160);
			break;
		case -1:
		default:
			if(HT_BW == 0)
				sprintf(outBw,"%d",20);
			else if(HT_BW == 1 && VHT_BW == 0)
				sprintf(outBw,"%d",40);
			else if(HT_BW == 1 && VHT_BW == 1)
				sprintf(outBw,"%d",80);
			else
				sprintf(outBw,"%d",20);
			break;
	}
		return;
}

static void info_UpdateBcnFails(RADIO_STATISTICS radioStatics,char* outBcnFails)
{
	sprintf(outBcnFails,"%d",radioStatics.BcnFail);
}

static void info_UpdateNoise(char* Noise,int bandidx)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int noise_floor = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateChannel: Create socket fail\n");
		sprintf(Noise,"%d",noise_floor);
		return;
	}
	if(bandidx == 0)
			strcpy(wrq.ifr_name, "ra0");
		else if(bandidx == 1)
			strcpy(wrq.ifr_name, "rai0");
		else{
			printf("wrong bandidx=%d\n",bandidx);
			sprintf(Noise,"%d",noise_floor);
			close(socket_id);
			return;
		}

       wrq.u.data.length = sizeof(noise_floor);
       wrq.u.data.pointer = &noise_floor;
       wrq.u.data.flags = OID_802_11_GET_NF;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>info_UpdateNoise: ioctl open fail\n");
    	close(socket_id);
		sprintf(Noise,"%d",noise_floor);
		return;
	}
	sprintf(Noise,"%d",noise_floor);
	close(socket_id);
	return;

}

static void info_UpdateChanLoad(char* ChanLoad,int bandidx)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int channel_load = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>info_UpdateChanLoad: Create socket fail\n");
		sprintf(ChanLoad,"%d",channel_load);
		return;
	}
	if(bandidx == 0)
			strcpy(wrq.ifr_name, "ra0");
		else if(bandidx == 1)
			strcpy(wrq.ifr_name, "rai0");
		else{
			printf("wrong bandidx=%d\n",bandidx);
			sprintf(ChanLoad,"%d",channel_load);
			close(socket_id);
			return;
		}

       wrq.u.data.length = sizeof(channel_load);
       wrq.u.data.pointer = &channel_load;
       wrq.u.data.flags = RT_OID_GET_CHANLOAD;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>info_UpdateNoise: ioctl open fail\n");
    	close(socket_id);
		sprintf(ChanLoad,"%d",channel_load);
		return;
	}
	channel_load /=10000;
	sprintf(ChanLoad,"%d",channel_load);
	close(socket_id);
	return;

}

static void info_UpdateChanIdle(char* ChanIdle,int bandidx)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int channel_load = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>info_UpdateChanIdle: Create socket fail\n");
		sprintf(ChanIdle,"%d",channel_load);
		return;
	}
	if(bandidx == 0)
			strcpy(wrq.ifr_name, "ra0");
		else if(bandidx == 1)
			strcpy(wrq.ifr_name, "rai0");
		else{
			printf("wrong bandidx=%d\n",bandidx);
			sprintf(ChanIdle,"%d",channel_load);
			close(socket_id);
			return;
		}

       wrq.u.data.length = sizeof(channel_load);
       wrq.u.data.pointer = &channel_load;
       wrq.u.data.flags = RT_OID_GET_CHANLOAD;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>info_UpdateChanIdle: ioctl open fail\n");
    	close(socket_id);
		sprintf(ChanIdle,"%d",channel_load);
		return;
	}
	channel_load /=10000;
	sprintf(ChanIdle,"%d",100-channel_load);
	close(socket_id);
	return;

}

#endif


#ifdef WSC_AP_SUPPORT
static void info_UpdatePinCode(char* outPinCode)
{
	char tmp[4]			= {0};
	char node_name[128]	= {0};
	int wlan_id=0;
	
	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(cfg_get_object_attr(node_name, "wlan_id", tmp, sizeof(tmp)) > 0){
		wlan_id = atoi(tmp);
	}

	wifimgr_lib_get_WPS_PIN_CODE(0, wlan_id, outPinCode);

	return;
	
}
#endif


#if defined(TCSUPPORT_WLAN_AC)
#ifdef WSC_AP_SUPPORT
static void info_UpdatePinCodeAC(char * outPinCode)
{
	char tmp[4]			= {0};
	char node_name[128]	= {0};
	int wlan_id=0;

	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(cfg_get_object_attr(node_name, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
		wlan_id = atoi(tmp);
	}
	wifimgr_lib_get_WPS_PIN_CODE(1, wlan_id, outPinCode);

	return;
	
}

#endif /* WSC_AP_SUPPORT */
#endif /* TCSUPPORT_WLAN_AC */

#if defined(TCSUPPORT_WLAN)

static void check_wireless_card(char *retval)
{
	int fd,len;
	char stream[64] = {0};
	int res = 0;
#if 0
	system("/sbin/ifconfig ra0 > /tmp/wireless_stat.tmp");
	memset(stream,0,sizeof(stream));
	fd = open("/tmp/wireless_stat.tmp",O_RDONLY);
	if (fd == -1){
		strcpy(retval, "Off");
		return;
	}

	len=read(fd,stream,sizeof(stream) - 1);
	stream[sizeof(stream) - 1] = '\0';
	close(fd);
	res = remove("/tmp/wireless_stat.tmp");
	if((len == 0) || (len == -1)){
		strcpy(retval, "Off");
		return;
	}

	if((strstr(stream,"HWaddr FF:FF:FF:FF:FF:FF") != 0)
	||(strstr(stream,"HWaddr 00:00:00:00:00:00") != 0)){
		fprintf(stderr,"There are some error when CPE getting data from EEPROM");
		strcpy(retval, "Error");
		return;
	}else{
		strcpy(retval, "On");
	}
#else
	res = wifimgr_lib_get_interface_mac0("ra0", stream, sizeof(stream));
	if( res < 0 ){
		strcpy(retval, "Off");
	}
	else{
		if((strstr(stream,"FF:FF:FF:FF:FF:FF") != 0)
			||(strstr(stream,"00:00:00:00:00:00") != 0)){
			tcdbg_printf("==>%s: error when get ra0 EEPROM data\n", __FUNCTION__);
			strcpy(retval, "Error");
		}
		else{
			strcpy(retval, "On");
		}
	}
	return;
#endif
}

int cfg_type_wlanInfo_read(char* path)
{
#if defined(TCSUPPORT_WLAN)
	char wlanInfo_keywd[][32] = {
		{"Tx success  "},		{"Tx fail"},
		{"Rx success"},			{"Rx with CRC"},
		{"Rx drop"},			{"Channel"},
#ifdef WSC_AP_SUPPORT
		{"Enrollee PinCode(ra0)"},
#ifdef TCSUPPORT_WLAN_MULTI_WPS	
		{"WPS Query Status(ra0)"},
		{"WPS Query Status(ra1)"},	
		{"WPS Query Status(ra2)"},
		{"WPS Query Status(ra3)"},		
		{"WPS Wsc2MinsTimerRunning(ra0)"},
		{"WPS Wsc2MinsTimerRunning(ra1)"},
		{"WPS Wsc2MinsTimerRunning(ra2)"},
		{"WPS Wsc2MinsTimerRunning(ra3)"},
#else
		{"WPS Query Status(ra0)"},
		{"WPS Wsc2MinsTimerRunning(ra0)"},
#endif
#endif
		{""}
	};

	char wlanInfo_attr[][32]=
	{
		{"wlanTxFrames"},		{"wlanTxErrFrames"},
		{"wlanTxDropFrames"},	{"wlanRxFrames"},
		{"wlanRxErrFrames"},	{"wlanRxDropFrames"},
		{"CurrentChannel"},
#ifdef WSC_AP_SUPPORT
		{"wlanSelfPinCode"},
#ifdef TCSUPPORT_WLAN_MULTI_WPS		
		{"wlanWPSStatus_0"},	{"wlanWPSStatus_1"},
		{"wlanWPSStatus_2"},	{"wlanWPSStatus_3"},
		{"wlanWPStimerRunning_0"},{"wlanWPStimerRunning_1"},
		{"wlanWPStimerRunning_2"},{"wlanWPStimerRunning_3"},
#else	
		{"wlanWPSStatus"},		{"wlanWPStimerRunning"},
#endif	
#endif
		{""}
	};

	enum wlanInfo_en{
		TxSuccess=0,
		TxFail,
		RxSuccess,
		RxCRC,
		RxDrop,
		Channel,
#ifdef WSC_AP_SUPPORT
		PinCode,
#ifdef TCSUPPORT_WLAN_MULTI_WPS		
		WPSStatus_0,
		WPSStatus_1,
		WPSStatus_2,	
		WPSStatus_3,
		WPSTimerRunning_0,
		WPSTimerRunning_1,
		WPSTimerRunning_2,
		WPSTimerRunning_3,
#else	
		WPSStatus,
		WPSTimerRunning,
#endif
#endif
	};

#ifdef TCSUPPORT_CMCCV2
	char wlanInfo_radioStatic[][32]=
	{
		{"bandWidth"},		{"bcnFails"},
		{"noise"},			{"chanLoad"},
		{"chanIdle" },
	};
	enum wlanInfo_radioStatic{
		BandWidth,
		BcnFails,
		Noise,
		ChanLoad,
		ChanIdle
	};
#endif

	char value[32] 	= {0};
	char macvalue[20] = {0};
	char *buf 		= NULL;
	int i			= 0;
#ifdef TCSUPPORT_WLAN_MULTI_WPS		
	unsigned long int wlanStatus[15];
#else
	unsigned long int wlanStatus[9];
#endif
	char cmd[64]={0};
#ifdef WSC_AP_SUPPORT
	int wpsStatus	= 0;
#endif
	static struct timespec pre_sec = {0};
	struct timespec cur_sec = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	RT_AP_COUNT apcnt_24;
#endif
	bzero(&cur_sec, sizeof(cur_sec));
	clock_gettime(CLOCK_MONOTONIC, &cur_sec);
	if ( (cur_sec.tv_sec - pre_sec.tv_sec) <= 2 )
		return 0;
	
	if(path){
#ifdef TCSUPPORT_WLAN_WDS
	cfg_set_object_attr(path, "isWDSSupported", "Yes"); 
#endif

		buf = (char *)malloc(MAX_BUF_SIZE);
		if(buf==NULL){
			perror("malloc fail");
			return -1;
		}
		memset(buf,0, MAX_BUF_SIZE);
#if defined(TCSUPPORT_C1_CUC)
		cfg_set_object_attr(path, "isCUCSupport", "Yes");
#endif
#if defined(TCSUPPORT_TRUE_LANDING_PAGE)
		cfg_set_object_attr(path, "isLandingPageSupport","Yes");
#endif
#if defined(TCSUPPORT_WLAN_8021X)
		cfg_set_object_attr(path, "isDot1XSupported", "Yes"); 
#endif

		/*	Create a tmp file to get current wlan traffic statistics information.
		*/
#if defined(TCSUPPORT_CT_JOYME2)
		memset(&apcnt_24, 0, sizeof(RT_AP_COUNT));
		getAPCount(0, &apcnt_24);
#endif

		wifimgr_lib_get_WIFI_STATS("ra0", buf, MAX_BUF_SIZE);
#if defined(TCSUPPORT_CT_E8GUI)
#if defined(TCSUPPORT_CT_JOYME2)
		get_itf_hwaddr(path, "ra0");
		get_itf_stat(path, "ra0");
#else
		ifInfo_read(path, "ra0");
#endif
#endif
		if(strlen(buf) != 0){
			for(i = 0; strlen(wlanInfo_keywd[i]) != 0; i++){
				wlanStatus[i] = get_key_vlaue(buf, wlanInfo_keywd[i], 10);
			}
#if defined(TCSUPPORT_CT_JOYME2)
			snprintf(value, sizeof(value), "%llu", apcnt_24.txBytes);
			cfg_set_object_attr(path, "txbytes", value);
			snprintf(value, sizeof(value), "%llu", apcnt_24.rxBytes);
			cfg_set_object_attr(path, "rxbytes", value);
#endif

			for(i = 0; strlen(wlanInfo_attr[i]) != 0; i++){
				switch(i){
#if defined(TCSUPPORT_CT_JOYME2)
					case 0:/*wlanTxFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.txPackets);
						break;
					case 1:/*wlanTxErrFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.txErrPkts);
						break;
					case 2:/*wlanTxDropFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.txDropPkts);
						break;
					case 3:/*wlanRxFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.rxPackets);
						break;
					case 4:/*wlanRxErrFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.rxErrPkts);
						break;
					case 5:/*wlanRxDropFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_24.rxDropPkts);
						break;
#else
					case 0:/*wlanTxFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxSuccess]);
						break;
					case 1:/*wlanTxErrFrames*/
					case 2:/*wlanTxDropFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxFail]);
						break;
					case 3:/*wlanRxFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxSuccess]);
						break;
					case 4:/*wlanRxErrFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxCRC]);
						break;
					case 5:/*wlanRxDropFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxCRC]+wlanStatus[RxDrop]);
						break;
#endif
					case 6:/*CurrentChannel */
						wifimgr_lib_get_CHANNEL(0, value);
						break;
#ifdef WSC_AP_SUPPORT
					case 7:/*PinCode */
						info_UpdatePinCode(value);
						break;

#ifdef TCSUPPORT_WLAN_MULTI_WPS
					case 8:/*wpsstatus,ssid0*/
					case 9:/*wpsstatus,ssid1*/
					case 10:/*wpsstatus,ssid2*/
					case 11:/*wpsstatus,ssid3*/
						wifimgr_lib_get_WpsStatus(0, &wpsStatus, macvalue, i-8);
						if(wpsStatus == 0 || wpsStatus == 1){
							snprintf(value, sizeof(value), "%s", "Idle");					
						}else if(wpsStatus == 2){
							snprintf(value, sizeof(value), "%s", "WPS Process Fail");
						}else if((wpsStatus > 2) && (wpsStatus < 34)){
							snprintf(value, sizeof(value), "%s", "In progress");
						}else{
							snprintf(value, sizeof(value), "%s", "Configured");
						}
						if(macvalue[0]){
							cfg_set_object_attr(path, "wpsmac", macvalue);
						}
						break;
					case 12:/*wps timer running-ra0*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_0]);
						break;
					case 13:/*wps timer running-ra1*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_1]);
						break;
					case 14:/*wps timer running-ra2*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_2]);
						break;
					case 15:/*wps timer running-ra3*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_3]);
						break;
#else
					case 8:/*wpsstatus*/
						wifimgr_lib_get_WpsStatus(0, &wpsStatus);
						if(wpsStatus == 0 || wpsStatus == 1){
							snprintf(value, sizeof(value), "%s", "Idle");					
						}else if(wpsStatus == 2 || wpsStatus == 31 || 
								wpsStatus == 32  || wpsStatus == 33){
							snprintf(value, sizeof(value), "%s", "WPS Process Fail");
						}else if((wpsStatus > 2) && (wpsStatus < 31)){
							snprintf(value, sizeof(value), "%s", "In progress");
						}else{
							snprintf(value, sizeof(value), "%s", "Configured");
						}
						break;
					case 9:/*wps timer running*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning]);
						break;
#endif
#endif
					default:
						memset(value, 0, sizeof(value));
						break;
				}
				cfg_set_object_attr(path, wlanInfo_attr[i], value);
			}
#ifdef TCSUPPORT_CMCCV2
				RADIO_STATISTICS radioStatics;
				info_UpdateRadioStatics(&radioStatics,0);
				memset(value, 0, sizeof(value));
				for(i = 0; strlen(wlanInfo_radioStatic[i]) != 0; i++){
					switch(i){
						case BandWidth:
							info_UpdateBW(radioStatics,0,value);
							break;
						case BcnFails:
							info_UpdateBcnFails(radioStatics,value);
							break;
						case Noise:
							info_UpdateNoise(value,0);
							break;
						case ChanLoad:
							info_UpdateChanLoad(value,0);
							break;
						case ChanIdle:
							info_UpdateChanIdle(value,0);
							break;
						default:
							memset(value, 0, sizeof(value));
							break;	
					}
					cfg_set_object_attr(path, wlanInfo_radioStatic[i], value);
				}
#endif	

#ifdef WSC_AP_SUPPORT
			cfg_set_object_attr(path, "isWPSSupported", "YES");
#endif
			wifimgr_lib_is_Tx_Rx_Stream_support(value, sizeof(value));
			cfg_set_object_attr(path, "isTxRxStreamSupported", value);
		}
		unlink(WLANINFO_PATH);
		free(buf);
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
		wifimgr_lib_get_max_sta_num_ui(value, sizeof(value));
		cfg_set_object_attr(path, "maxStaNumSupported", value);
#endif
		wifimgr_lib_is_WHNAT_support(WIFI_2_4G, value, sizeof(value));
		cfg_set_object_attr(path, "isWHNATSupported", value);
	}
	else{
		return -1;
	}
	bzero(&pre_sec, sizeof(pre_sec));
	clock_gettime(CLOCK_MONOTONIC, &pre_sec);
#endif
	return 0;
}

#if defined(TCSUPPORT_WLAN_AC)
static void check_wireless_ac_card(char *retval)
{
	int fd,len;
	char stream[64];
#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
	int i;
	char isexist = 0;
	char BssidNum[4] = {0};
	char tmp[16]={0};
	char checkwifiisexist[128] = {0};
#endif
	char node_name[128] = {0};
	char apswitch[4] = {0};
	int res = 0;
	char if_name[16] = {0};
	int sock = -1;
	struct ifreq ifbuf;
	uint8_t *tmphwaddr = NULL;
	char hwaddrbuf[20] = {0};

	snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
	/*if wifi function is off,return disable directly*/
	if(cfg_obj_get_object_attr(node_name, "APOn", 0, apswitch, sizeof(apswitch)) > 0){
		if(atoi(apswitch) == 0){
			strcpy(retval, "Disable");
			return;
		}
	}
	else{
		strcpy(retval, "Off");
		return;
	}
	
	#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
	if(cfg_obj_get_object_attr(node_name, "BssidNum", 0, BssidNum, sizeof(BssidNum)) > 0)
	{
		for(i = 0; i < atoi(BssidNum); i++){
			snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, i+1);
			cfg_obj_get_object_attr(node_name,"EnableSSID", 0, tmp, sizeof(tmp));
			if(atoi(tmp) == 1){
				snprintf(if_name, sizeof(if_name), "rai%d", i);
				isexist = 1;
				break;
			}
		}
	}
	else{
		strcpy(retval, "Off");
		return;
	}
	
	if(!isexist)	{
		/*wifi load but all ssid is disable,so return on let webpage know it*/
		strcpy(retval, "On");
		return;
	}
#endif
#if 0
#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
	system(checkwifiisexist);
#else
	system("/sbin/ifconfig rai0 > /tmp/wireless_ac_stat.tmp");
#endif
	memset(stream,0,sizeof(stream));
	fd = open("/tmp/wireless_ac_stat.tmp",O_RDONLY);
	if (fd == -1){
		strcpy(retval, "Off");
		return;
	}

	len=read(fd,stream,sizeof(stream) - 1);
	stream[sizeof(stream) - 1] = '\0';
	close(fd);
	res = remove("/tmp/wireless_ac_stat.tmp");
	if((len == 0) || (len == -1)){
		strcpy(retval, "Off");
		return;
	}
#endif
	res = wifimgr_lib_get_interface_mac0("rai0", stream, sizeof(stream));
	if( res < 0 ){
		strcpy(retval, "Off");
	}
	else{
		if((strstr(stream,"FF:FF:FF:FF:FF:FF") != 0)
			||(strstr(stream,"00:00:00:00:00:00") != 0)){
			tcdbg_printf("==>%s: error when get ra0 EEPROM data\n", __FUNCTION__);
			strcpy(retval, "Error");
		}
		else{
			strcpy(retval, "On");
		}
	}
	return;
}/*end check_wireless_ac_card*/

int cfg_type_wlan11acInfo_read(char* path)
{
	enum wlanInfo_en{
		TxSuccess=0,
		TxFail,
		RxSuccess,
		RxCRC,
		RxDrop,
		Channel,
		RetryCount,
		RTSSuccess,
		RTSFail,
		SNR0,
		SNR1,
		SNR2,
#ifdef WSC_AP_SUPPORT
		PinCode,
#ifdef TCSUPPORT_WLAN_MULTI_WPS		
		WPSStatus_0,
		WPSStatus_1,
		WPSStatus_2,	
		WPSStatus_3,
		WPSTimerRunning_0,
		WPSTimerRunning_1,
		WPSTimerRunning_2,
		WPSTimerRunning_3,
#else	
		WPSStatus,
		WPSTimerRunning,
#endif
#endif
		RxPackets,
		TxPackets,
		RxBytes,
		TxBytes,
		
		/* used to define the size of wlanStatus[],so don't enumerate any variables behind it */
		LastOne
	};
	char wlan11acInfo_keywd[][32]=
	{
			{"Tx success  "},		{"Tx fail"},
			{"Rx success"},			{"Rx with CRC"},
			{"Rx drop"},			{"Channel"},
			{"Tx retry count"},		{"RTS Success Rcv CTS"},
			{"RTS Fail Rcv CTS"},	{"RSSI -A"},
			{"RSSI -B"},			{"RSSI -C"},
#ifdef WSC_AP_SUPPORT
			{"Enrollee PinCode(rai0)"},
#ifdef TCSUPPORT_WLAN_MULTI_WPS	
			{"WPS Query Status(rai0)"},
			{"WPS Query Status(rai1)"}, 
			{"WPS Query Status(rai2)"},
			{"WPS Query Status(rai3)"}, 	
			{"WPS Wsc2MinsTimerRunning(rai0)"},
			{"WPS Wsc2MinsTimerRunning(rai1)"},
			{"WPS Wsc2MinsTimerRunning(rai2)"},
			{"WPS Wsc2MinsTimerRunning(rai3)"},
#else
			{"WPS Query Status(rai0)"},
			{"WPS Wsc2MinsTimerRunning(rai0)"},
#endif
#endif
			{"Packets Received"},
			{"Packets Sent"},
			{"Bytes Received"},
			{"Bytes Sent"}, 		 
			{""}
	};
	char wlanInfo_attr_ac[][32]=
	{
			{"wlanTxFrames"},		{"wlanTxErrFrames"},
			{"wlanTxDropFrames"},	{"wlanRxFrames"},
			{"wlanRxErrFrames"},	{"wlanRxDropFrames"},
			{"CurrentChannel"},		{"wlanTxRetryCount"},
			{"wlanRTSSuccessRcvCTS"},{"wlanRTSFailRcvCTS"},
			{"wlanSNR0"},			{"wlanSNR1"},
			{"wlanSNR2"},
#ifdef WSC_AP_SUPPORT
			{"wlanSelfPinCode"},
#ifdef TCSUPPORT_WLAN_MULTI_WPS		
			{"wlanWPSStatus_0"},	{"wlanWPSStatus_1"},
			{"wlanWPSStatus_2"},	{"wlanWPSStatus_3"},
			{"wlanWPStimerRunning_0"},{"wlanWPStimerRunning_1"},
			{"wlanWPStimerRunning_2"},{"wlanWPStimerRunning_3"},
#else
			{"wlanWPSStatus"},{"wlanWPStimerRunning"},
#endif
#endif
#ifdef WSC_AP_SUPPORT
#endif
			{"wlanRxPackets"},
			{"wlanTxPackets"},
			{"wlanRxBytes"},
			{"wlanTxBytes"},
			{""}
	};
#ifdef TCSUPPORT_CMCCV2
		char wlanInfo_radioStatic_ac[][32]=
		{
			{"bandWidth"},		{"bcnFails"},
			{"noise"},			{"chanLoad"},
			{"chanIdle" },
		};
	enum wlanInfo_radioStatic{
		BandWidth,
		BcnFails,
		Noise,
		ChanLoad,
		ChanIdle
	};
#endif

	char value[32];
	char macvalue[20]	= {0};
	char *buf = NULL;
	int i = 0;
	unsigned long int wlanStatus[LastOne];
	char node_name[128]	= {0};
	char rt_device[16]	= {0};
	static struct timespec pre_time = {0, 0};
	struct timespec cur_time = {0, 0};
	char cmd[64] 		= {0};
#ifdef WSC_AP_SUPPORT
	int wpsStatus		= 0;
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	RT_AP_COUNT apcnt_58;
#endif

	if(path){
		memset(&cur_time, 0, sizeof(cur_time));
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		if ( (cur_time.tv_sec - pre_time.tv_sec) <= 2 ) {
			return 0;
		}

		memset(value, 0, sizeof(value));
		check_wireless_ac_card(value);
		cfg_set_object_attr(path, "isExist", value);
		
#ifdef TCSUPPORT_WLAN_WDS
		cfg_set_object_attr(path, "isWDSSupported", "Yes"); 
#endif
#if defined(TCSUPPORT_WLAN_8021X)
		cfg_set_object_attr(path, "isDot1XSupported", "Yes"); 
#endif

		/* get current wlan traffic statistics information. */
		buf = (char *)malloc(MAX_BUF_SIZE);
		if ( NULL == buf ) {
			perror("malloc fail");
			return -1;
		}
		memset(buf, 0, MAX_BUF_SIZE);
		
		wifimgr_lib_get_WIFI_STATS("rai0", buf, MAX_BUF_SIZE);
#if defined(TCSUPPORT_CT_JOYME2)
		memset(&apcnt_58, 0, sizeof(RT_AP_COUNT));
		getAPCount(1, &apcnt_58);
#endif
		
		if(strlen(buf)!=0){
			memset(&pre_time, 0, sizeof(pre_time));
			clock_gettime(CLOCK_MONOTONIC, &pre_time);
			for(i=0; strlen(wlan11acInfo_keywd[i])!=0; i++){
				wlanStatus[i]=get_key_vlaue(buf, wlan11acInfo_keywd[i], 10);
			}
			for(i=0; strlen(wlanInfo_attr_ac[i])!=0; i++){
				switch(i){
#if defined(TCSUPPORT_CT_JOYME2)
					case TxSuccess:/*wlanTxFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.txPackets);
						break;
					case TxFail:/*wlanTxErrFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.txErrPkts);
						break;
					case (TxFail+1):/*wlanTxDropFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.txDropPkts);
						break;
					case (RxSuccess+1):/*wlanRxFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.rxPackets);
						break;
					case (RxCRC+1):/*wlanRxErrFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.rxErrPkts);
						break;
					case (RxDrop+1):/*wlanRxDropFrames*/
						snprintf(value, sizeof(value), "%lu", apcnt_58.rxDropPkts);
						break;
					case (RxBytes+1):/*Bytes Received*/
						snprintf(value, sizeof(value), "%llu", apcnt_58.rxBytes);
						break;
					case (TxBytes+1):/*Bytes Sent*/
						snprintf(value, sizeof(value), "%llu", apcnt_58.txBytes);
						break;
#else
					case TxSuccess:/*wlanTxFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxSuccess]);
						break;
					case TxFail:/*wlanTxErrFrames*/
					case (TxFail+1):/*wlanTxDropFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxFail]);
						break;
					case (RxSuccess+1):/*wlanRxFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxSuccess]);
						break;
					case (RxCRC+1):/*wlanRxErrFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxCRC]);
						break;
					case (RxDrop+1):/*wlanRxDropFrames*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxCRC]+wlanStatus[RxDrop]);
						break;
					case (RxBytes+1):/*Bytes Received*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxBytes]);
						break;
					case (TxBytes+1):/*Bytes Sent*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxBytes]);
						break;
#endif
					case (Channel+1):/*CurrentChannel */
						wifimgr_lib_get_CHANNEL(1, value);
						break;
					case (RetryCount+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[RetryCount]);
						break;
					case (RTSSuccess+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[RTSSuccess]);
						break;
					case (RTSFail+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[RTSFail]);
						break;
					case (SNR0+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[SNR0]);
						break;
					case (SNR1+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[SNR1]);
						break;
					case (SNR2+1):
						snprintf(value, sizeof(value), "%lu", wlanStatus[SNR2]);
						break;
					
#ifdef WSC_AP_SUPPORT
					case (PinCode+1):
						info_UpdatePinCodeAC(value);
						break;
#ifdef TCSUPPORT_WLAN_MULTI_WPS
					case (WPSStatus_0+1):/*wpsstatus,ssid0*/
					case (WPSStatus_1+1):/*wpsstatus,ssid1*/
					case (WPSStatus_2+1):/*wpsstatus,ssid2*/
					case (WPSStatus_3+1):/*wpsstatus,ssid3*/
						wifimgr_lib_get_WpsStatus(1, &wpsStatus, macvalue, i-WPSStatus_0-1);
						if( wpsStatus == 0 || wpsStatus == 1){
							snprintf(value, sizeof(value), "%s", "Idle");					
						}else if(wpsStatus == 2){
							snprintf(value, sizeof(value), "%s", "WPS Process Fail");
						}else if((wpsStatus > 2) && (wpsStatus < 34)){
							snprintf(value, sizeof(value), "%s", "In progress");
						}else{
							snprintf(value, sizeof(value), "%s", "Configured");
						}

						if( macvalue[0]){
							cfg_set_object_attr(path, "wpsmac", macvalue);
						}
						break;
					case (WPSTimerRunning_0+1):/*wps timer running-rai0*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_0]);
						break;
					case (WPSTimerRunning_1+1):/*wps timer running-rai1*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_1]);
						break;
					case (WPSTimerRunning_2+1):/*wps timer running-rai2*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_2]);
						break;
					case (WPSTimerRunning_3+1):/*wps timer running-rai3*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning_3]);
						break;
#else	
					case (WPSStatus+1):/*wpsstatus*/
						wifimgr_lib_get_WpsStatus(1, &wpsStatus);
						if(wpsStatus==0||wpsStatus==1){
							snprintf(value, sizeof(value), "%s", "Idle");					
						}else if(wpsStatus==2 || wpsStatus==31 || wpsStatus==32 
															   || wpsStatus==33){
							snprintf(value, sizeof(value), "%s", "WPS Process Fail");
						}else if((wpsStatus>2) && (wpsStatus<31)){
							snprintf(value, sizeof(value), "%s", "In progress");
						}else{
							snprintf(value, sizeof(value), "%s", "Configured");
						}
						break;
					case (WPSTimerRunning+1):/*wps timer running*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[WPSTimerRunning]);
						break;

#endif						
#endif
					case (RxPackets+1):/*Packets Received*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[RxPackets]);
						break;
					case (TxPackets+1):/*Packets Sent*/
						snprintf(value, sizeof(value), "%lu", wlanStatus[TxPackets]);
						break;
					default:
						memset(value, 0, sizeof(value));
						break;
				}
				cfg_set_object_attr(path, wlanInfo_attr_ac[i], value);
			}
#ifdef TCSUPPORT_CMCCV2
			RADIO_STATISTICS radioStatics;
			info_UpdateRadioStatics(&radioStatics,1);
			memset(value, 0, sizeof(value));
			for(i=0; strlen(wlanInfo_radioStatic_ac[i])!=0; i++){
				switch(i){
					case BandWidth:
						info_UpdateBW(radioStatics,1,value);
						break;
					case BcnFails:
						info_UpdateBcnFails(radioStatics,value);
						break;
					case Noise:
						info_UpdateNoise(value,1);
						break;
					case ChanLoad:
						info_UpdateChanLoad(value,1);
						break;
					case ChanIdle:
						info_UpdateChanIdle(value,1);
						break;
					default:
						memset(value, 0, sizeof(value));
						break;	
				}
				cfg_set_object_attr(path, wlanInfo_radioStatic_ac[i], value);
			}
#endif

#ifdef WSC_AP_SUPPORT
			cfg_set_object_attr(path, "isWPSSupported", "YES");
#endif
			wifimgr_lib_is_Tx_Rx_Stream_support(value, sizeof(value));
			cfg_set_object_attr(path, "isTxRxStreamSupported", value);
		}
		unlink(WLAN11ACINFO_PATH);
		free(buf);
#ifdef IGMP_SNOOP_SUPPORT
		cfg_set_object_attr(path, "M2U", "1");
#else
		cfg_set_object_attr(path, "M2U", "0");
#endif
#ifdef DFS_SUPPORT
		cfg_set_object_attr(path, "DFS", "1");
#else
		cfg_set_object_attr(path, "DFS", "0");
#endif
#ifdef CARRIER_DETECTION_SUPPORT
		cfg_set_object_attr(path, "Carrier", "1");
#else
		cfg_set_object_attr(path, "Carrier", "0");
#endif
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
		wifimgr_lib_get_max_sta_num_ui(value, sizeof(value));
		cfg_set_object_attr(path, "maxStaNumSupported", value);
#endif
		wifimgr_lib_is_WHNAT_support(WIFI_5G, value, sizeof(value));
		cfg_set_object_attr(path, "isWHNATSupported", value);
		snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
		cfg_obj_get_object_attr(node_name, "rt_device", 0, rt_device, sizeof(rt_device));
		if((strlen(rt_device)>0)){
			if(rt_device[strlen(rt_device)-1]=='2'){
				cfg_set_object_attr(path, "TXPath", "2");
				cfg_set_object_attr(path, "RXPath", "2");
			}
			else{
				cfg_set_object_attr(path, "TXPath", "1");
				cfg_set_object_attr(path, "RXPath", "1");
			}
		}

	}
	else{
		return -1;
	}
	return 0;
}

#endif
#endif

int cfg_type_devDefInf_read(char* path)
{
	char attrName[32];
	char attrVal[128];
	int i, ret;
	
	char devDefInfo_keywd[][32]=
	{
		{"Manufacturer"},			{"ModelName"},
		{"ProductClass"},			{"CustomerSoftwareVersion"},
		{"CustomerSwVersionTime"},	{""}
	};

	memset( attrName, 0, sizeof(attrName) );
	memset( attrVal, 0, sizeof(attrVal) );

	for ( i = 0; strlen(devDefInfo_keywd[i]) != 0; i++ ) {
		sprintf( attrName, "%s=", devDefInfo_keywd[i] );
		ret = get_profile_str( attrName, attrVal, sizeof(attrVal), NO_QMARKS, DEVDEF_PATH);
		if ( ret != -1 ) {
			cfg_set_object_attr(path, devDefInfo_keywd[i], attrVal);
		}
		
	}
	
	return 0;	
}

int cfg_type_sysDeviceInfo_read(char* path, char* attr)
{
	char value[80] 		= {0}, memTotal[128] = {0};
	char buf[16]		= {0};

    memset(value,0, sizeof(value));
    if(path){
		if(strcmp(attr, "CpuUsage") == 0){
			if(cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CpuUsage", 0, buf, sizeof(buf)) > 0){
				cfg_set_object_attr(path, "CpuUsage", buf);
			}	 
		} 
		
        if(memory_read(value, memTotal) != 0){
             return -1;
        }
        cfg_set_object_attr(path, "MemoryUsage", value);
		cfg_set_object_attr(path, "MemoryTotal", memTotal);
    }
    else
    {
        return -1;
    }
    return 0;
}


int cfg_type_info_wlan11ac_get(char* path,char* attr,char* val,int len)
{
	char value[32] = {0};

#if defined(TCSUPPORT_WLAN)
#if defined(TCSUPPORT_WLAN_AC)

#ifdef TCSUPPORT_WLAN_BNDSTRG
	if ( !strcmp(attr, "ChanLoad") ) {
		memset(value, 0, sizeof(value));
		wifimgr_lib_get_BndStrgChanload(value,sizeof(value));
		cfg_set_object_attr(path, "ChanLoad", value);
		goto end;
	}
#endif
	if ( 1 == isInfoReading("info_wlan11ac_read"))
		goto end;
	
	if ( cfg_type_wlan11acInfo_read(path) < 0 ) {
		printf("cfg_type_wlan11acInfo_read fail.\n");
		goto end;
	}
	doCheck11acHtExtcha(path, 1);
#endif
#endif

end:
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_info_wlan_get(char* path,char* attr,char* val,int len)
{
	char value[32] = {0};
#if defined(TCSUPPORT_WLAN)
	if ( 1 == isInfoReading("info_wlan_read"))
		goto end;
	
	if(path)
	{
		if ( strcmp(attr, "isExist") == 0)
		{
			check_wireless_card(value);
			cfg_set_object_attr(path, "isExist", value);
			goto end;
		}
	}
	
	if(cfg_type_wlanInfo_read(path) < 0){
		printf("cfg_type_wlanInfo_read fail.\n");
		goto end;
	}
#endif
end:
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_info_rai_get(char* path,char* attr,char* val,int len)
{
	char sub_name[8] = {0};
	int idx = 0;
#if defined(TCSUPPORT_CT_DBUS)
	char buf[4] = {0};
#endif
	static struct timespec pre_sec[8] = {0};
	struct timespec cur_sec = {0};

	if (cfg_check_create_object(path) < 0){
		printf("create %s Fail.\n", path);
		return -1;
	}	
	
	/* root.info.rai.x */
	sscanf(path, "root.info.rai.%d", &idx);
	if ( idx < 1 || idx > 8)
		idx = 1;
	
	idx--;
	
	snprintf(sub_name, sizeof(sub_name), "rai%d", idx);
#if defined(TCSUPPORT_CT_DBUS) 
	cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", buf, sizeof(buf));
	if(strcmp(buf, "5") == 0){
		return cfg_obj_get_object_attr(path, attr, 0, val, len);
	}
#endif

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
	if ( !strcmp(attr, "hwaddr") ) {
		get_itf_hwaddr(path, sub_name);
	}else 
	{
		bzero(&cur_sec, sizeof(cur_sec));
		clock_gettime(CLOCK_MONOTONIC, &cur_sec);
		if ( (cur_sec.tv_sec - pre_sec[idx].tv_sec) <= 2 )
			return cfg_obj_get_object_attr(path, attr, 0, val, len);
		
		get_itf_stat(path, sub_name);
		bzero(&pre_sec[idx], sizeof(pre_sec[idx]));
		clock_gettime(CLOCK_MONOTONIC, &pre_sec[idx]);
	}
#else
		ifInfo_read(path, sub_name);
#endif

	return cfg_obj_get_object_attr(path, attr, 0, val, len);
}

int cfg_type_info_ra_get(char* path,char* attr,char* val,int len)
{
	char sub_name[8] = {0};
	int idx = 0;
#if defined(TCSUPPORT_CT_DBUS)
	char buf[4] = {0};
#endif
	static struct timespec pre_sec[8] = {0};
	struct timespec cur_sec = {0};
	
	if (cfg_check_create_object(path) < 0){
		printf("create %s Fail.\n", path);
		return -1;
	}
	/* root.info.ra.x */
	sscanf(path, "root.info.ra.%d", &idx);
	if ( idx < 1 || idx > 8)
		idx = 1;
	
	idx--;
	
	snprintf(sub_name, sizeof(sub_name), "ra%d", idx);
#if defined(TCSUPPORT_CT_DBUS) 
		cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", buf, sizeof(buf));
		if(strcmp(buf, "5") == 0){
			return cfg_obj_get_object_attr(path, attr, 0, val, len);
		}
#endif
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
	if ( !strcmp(attr, "hwaddr") ) {
		get_itf_hwaddr(path, sub_name);
	}else if ( (strstr(sub_name, "ra") != NULL && !strcmp(attr, "detectionStatus")) )
		loopdetect(path, sub_name);
	else 
	{
		bzero(&cur_sec, sizeof(cur_sec));
		clock_gettime(CLOCK_MONOTONIC, &cur_sec);
#if defined(TCSUPPORT_WLAN_APCLIENT)
		if ((strcmp(attr, "rxbytes") && strcmp(attr, "txbytes")) || idx )
#endif
		if ( (cur_sec.tv_sec - pre_sec[idx].tv_sec) <= 2 )
			return cfg_obj_get_object_attr(path, attr, 0, val, len);
		get_itf_stat(path, sub_name);
		bzero(&pre_sec[idx], sizeof(pre_sec[idx]));
		clock_gettime(CLOCK_MONOTONIC, &pre_sec[idx]);
	}
#else
		ifInfo_read(path, sub_name);
#endif

	return cfg_obj_get_object_attr(path, attr, 0, val, len); 

}

#if defined(TCSUPPORT_CT_DSL_EX)
int cfg_type_adslInfo_read(char *node)
{
	char adslInfo_keywd[][16]=
	{
		{"outPkts"},{"inPkts"},
		{"outDiscards"},{"inDiscards"},
		{"outBytes"},{"inBytes"},
		{""}
	};
	/*
	 * It is used to insert into Infol_Adsl Node
	 * */
	char adslStat_attr[][20]=
	{
		{"lineState"},{"Opmode"},
		{"SNRMarginDown"},{"AttenDown"},
		{"SNRMarginUp"},{"AttenUp"},
		{"DataRateDown"},{"DataRateUp"},
		{"WanListMode"},
		{"FECDown"},{"FECUp"},
		{"CRCDown"},{"CRCUp"},
		{"HECDown"},{"HECUp"},
		{"ADSLUpTime"},{"ADSLActiveTime"},
		{"PowerDown"},{"PowerUp"},
		{"ATURID"},{"ATUCID"},
		{"AttainUp"},{"AttainDown"},
#ifdef CWMP
		{"ShowtimeStart"}, {"TotalStart"},
		{"ATURANSIRev"},{"ATUCANSIRev"},
		{"ATURANSIStd"},{"ATUCANSIStd"},
		{"InterleaveDepth"},
#if defined(TCSUPPORT_CT_ADSLSETTING)
		{"AdslStandard"},
		{"AdslType"},
#endif
#endif
		{"InterleaveDepthDown"},
		{""}
	};
	/*
	 * It is used to get attributes from ADSL's /proc
	 * */
	char adslState_keywd[][40]=
	{
		{"ADSL link status: "},{"Opmode: "},
		{"noise margin downstream: "},{"attenuation downstream: "},
		{"noise margin upstream: "},{"attenuation upstream: "},
		{"near-end interleaved channel bit rate: "},{"near-end fast channel bit rate: "},
		{"far-end interleaved channel bit rate: "},{"far-end fast channel bit rate: "},
		{"near-end FEC error fast: "},{"near-end FEC error interleaved: "},
		{"far-end FEC error fast: "},{"far-end FEC error interleaved: "},
		{"near-end CRC error fast: "},{"near-end CRC error interleaved: "},
		{"far-end CRC error fast: "},{"far-end CRC error interleaved: "},
		{"near-end HEC error fast: "},{"near-end HEC error interleaved: "},
		{"far-end HEC error fast: "},{"far-end HEC error interleaved: "},
		{"ADSL uptime :"},{"ADSL activetime :"},
		{"output power downstream: "},{"output power upstream: "},
		{"near end itu identification: "},{"far end itu identification: "},
		{"attain upstream: "},{"attain downstream: "},
#ifdef CWMP 
		{"ADSL activetime second: "},{"ADSL total ativetime second: "},
		{"ATURANSIRev: "},{"ATUCANSIRev: "},
		{"ATURANSIStd: "},{"ATUCANSIStd: "},
		{"Interleave Depth: "},
#endif
#if defined(TCSUPPORT_CT_ADSLSETTING)
		{"Adsl Standard: "},
		{"Adsl Type: "},
#endif
		{"Interleave Depth Down:"},
		{""}
	};

#if defined(TCSUPPORT_CT_WAN_PTM)
	char attrName[24] = {0};
	char vdslInterfaceCfg_keywd[][20]=
	{
		{"CurrentProfiles:"},{"UPBOKLE:"},
		{"TRELLISds:"},{"TRELLISus:"},
		{"ACTSNRMODEds:"}, {"ACTSNRMODEus:"},
		{"ACTUALCE:"},{"SNRMpbds:"},
		{"SNRMpbus:"},
		{""}
	};

	char vdslInterfaceCfg_attr[][20]=
	{
		{"CurrentProfiles"},{"UPBOKLE"},
		{"TRELLISds"},{"TRELLISus"},
		{"ACTSNRMODEds"}, {"ACTSNRMODEus"},
		{"ACTUALCE"},{"SNRMpbds"},
		{"SNRMpbus"},
		{""}
	};

#endif
	char opModeStr[128] = {0};

	enum adslStat_en{
		Adsl_stat=0,
		Opmode,
		Noise_m_down,
		Atten_down,
		Noise_m_up,
		Atten_up,
		Near_end_inter,
		Near_end_fast,
		Far_end_inter,
		Far_end_fast,
		Near_end_FEC_fast,
		Near_end_FEC_inter,
		Far_end_FEC_fast,
		Far_end_FEC_inter,
		Near_end_CRC_fast,
		Near_end_CRC_inter,
		Far_end_CRC_fast,
		Far_end_CRC_inter,
		Near_end_HEC_fast,
		Near_end_HEC_inter,
		Far_end_HEC_fast,
		Far_end_HEC_inter,
		Adsl_uptime,
		Adsl_activetime,
		Power_down,
		Power_up,
		ATUR_ID,
		ATUC_ID,
		Attain_up,
		Attain_down,
#ifdef CWMP
		Show_time_start,
		Total_start,
		ATURANSIRev,
		ATUCANSIRev,
		ATURANSIStd,
		ATUCANSIStd,
		InterleaveDepth,
#endif
	};

	char value[32];
	char *buf = NULL;
	int stat_attr_index=0;
	int i=0;
	char fwVer[50]={0};
	char adslRateInfo[2][16];

	int modeRecord=2;
	int data_loss_rate[2]={0};
	int loss_rate_index=0;
	char tmpLossRateStr[5] = {0};
	char modulationType[40] = {0};
	char annexType[40] = {0};
	char tmpStandardStr[40] = {0};
	int upStat = 1;
	char adslState_keyword_attr[sizeof(adslState_keywd)/(sizeof(char)*40)][64];
	char filePath[64] = {'\0'};
	

	if(node)
	{
		buf = (char *)malloc(MAX_BUF_SIZE);
		if(buf==NULL)
		{
			tcdbg_printf("malloc fail");
			return FAIL;
		}
		memset(buf,0, MAX_BUF_SIZE);
		blapi_xdsl_get_tsarm(buf, MAX_BUF_SIZE);
		if(strlen(buf)!=0)
		{
			for(i=0; strlen(adslInfo_keywd[i])!=0; i++)
			{
				snprintf(value, sizeof(value), "%lu", get_key_vlaue(buf, adslInfo_keywd[i], 16));
				cfg_set_object_attr(node, adslInfo_keywd[i], value);
			}
		}
		blapi_xdsl_get_fwver(fwVer, sizeof(fwVer));
		
		if (cfg_check_create_object(node) < 0)
		{
			tcdbg_printf("create %s Fail.\n", node);
			return -1;
		}
		
		cfg_set_object_attr(node, "fwVer", fwVer);
		memset(adslState_keyword_attr,0,sizeof(adslState_keyword_attr));
		/*get all attribute value via passing keyword to get profile string*/
		for(i=0; strlen(adslState_keywd[i])!=0; i++)
		{
			blapi_xdsl_set_key_val_to_attr(adslState_keywd[i], adslState_keyword_attr[i] , sizeof(adslState_keyword_attr[i]));
			if((i==0)&&(strcmp(adslState_keyword_attr[i],"up")!=0))
			{
				upStat = 0;
				break;
			}
		}
		/*After catching all of values corresponding to ADSL's proc keyword,
		 *	Use there attributes to get the attributes' value in Info_Adsl node.
		 *	stat_attr_index is indicated to the adsl_state array.
		 * */
		for(i=0; strlen(adslState_keywd[i])!=0; i++)
		{
			if(upStat == 0)
			{
				/*If the ADSL link isn't up, there is no other information*/
				cfg_set_object_attr(node, adslStat_attr[stat_attr_index], adslState_keyword_attr[i]);
				break;
			}
			if(i < Near_end_inter)
			{
				cfg_set_object_attr(node, adslStat_attr[stat_attr_index], adslState_keyword_attr[i]);
 				stat_attr_index++;
			}
			else if((i >= Near_end_inter) && (i <= Far_end_fast))
			{
				/*
					To set DataRateDown &&	DataRateUp attribute-value.
					We need to check the CPE at interleaved/fast channel mode.
					So we use j variable to stroe current attribute index value.
				*/
				memset(adslRateInfo, 0, sizeof(adslRateInfo));
				/*ext: 7645 kbps*/
				sscanf(adslState_keyword_attr[i],"%s %s", adslRateInfo[0], adslRateInfo[1]);
				if(atoi(adslRateInfo[0]) > 0)
				{
					if((i == Near_end_inter) || (i == Far_end_inter))
					{
						modeRecord --;
					}
					cfg_set_object_attr(node, adslStat_attr[stat_attr_index], adslState_keyword_attr[i]);
 					stat_attr_index++;
				}
				if(i == Far_end_fast)
				{
					if(modeRecord)
					{
						cfg_set_object_attr(node, adslStat_attr[stat_attr_index], "1");
 						stat_attr_index++;
					}
					else
					{
						cfg_set_object_attr(node, adslStat_attr[stat_attr_index], "0");
 						stat_attr_index++;
					}
				}
			}
			else if((i >= Near_end_FEC_fast) && (i <= Far_end_HEC_inter))
			{
				data_loss_rate[loss_rate_index] = atoi(adslState_keyword_attr[i]);
				if(loss_rate_index == 1)
				{
					memset(tmpLossRateStr, 0, sizeof(tmpLossRateStr));
					snprintf(tmpLossRateStr, sizeof(tmpLossRateStr), "%d", data_loss_rate[0]+data_loss_rate[1]);
					cfg_set_object_attr(node, adslStat_attr[stat_attr_index], tmpLossRateStr);
					stat_attr_index++;
				}
				loss_rate_index = (loss_rate_index+1)%2;
			}
			else
			{
				cfg_set_object_attr(node, adslStat_attr[stat_attr_index], adslState_keyword_attr[i]);
				stat_attr_index++;
			}
		}

#if defined(TCSUPPORT_CT_WAN_PTM)
		for(i=0; strlen(vdslInterfaceCfg_keywd[i])!=0; i++)
		{
			blapi_xdsl_get_interface_config_file(filePath, sizeof(filePath));
			get_profile_str(vdslInterfaceCfg_keywd[i],	value ,sizeof(value), NO_QMARKS, filePath);
			cfg_set_object_attr(node, vdslInterfaceCfg_attr[i], value);
		}
#endif

		if(upStat == 1)
		{
			if(cfg_obj_query_object(ADSL_ENTRY_NODE, NULL, NULL) > 0)
			{
				
				if((cfg_obj_get_object_attr(ADSL_ENTRY_NODE, "MODULATIONTYPE", 0, modulationType, sizeof(modulationType)) >= 0) 
					&& (cfg_obj_get_object_attr(ADSL_ENTRY_NODE, "ANNEXTYPEA", 0, annexType, sizeof(annexType)) >= 0))
				{
					if(!strcmp(modulationType,MODULATIONTYPE_T1_413))
					{
						strncpy(tmpStandardStr, MODULATIONTYPE_T1_413, sizeof(tmpStandardStr) - 1);
					}
					else if(!strcmp(modulationType,MODULATIONTYPE_G_DMT))
					{
						strncpy(tmpStandardStr, MODULATIONTYPE_G_DMT, sizeof(tmpStandardStr) - 1);
					}
					else if(!strcmp(modulationType,MODULATIONTYPE_G_LITE))
					{
						strncpy(tmpStandardStr, MODULATIONTYPE_G_LITE, sizeof(tmpStandardStr) - 1);
					}
					else if(!strcmp(modulationType,MODULATIONTYPE_ADSL2))
					{
						if((!strcmp(annexType,"ANNEX A/L")) || (!strcmp(annexType,"ANNEX A/I/J/L/M")))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2,ANNEXTYPEA_L);
						}
						else if(!strcmp(annexType,"ANNEX M"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2, ANNEXTYPEA_M);
						}
						else
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MTENSTANDARD_ADSL2);
						}
					}
					else if(!strcmp(modulationType,MODULATIONTYPE_AUTOSYNCUP))
					{
						cfg_obj_get_object_attr(node, "Opmode", 0, opModeStr, sizeof(opModeStr));
						if(!strcmp(opModeStr, "ITU G.993.2(VDSL2)"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.993.2(VDSL2)");
						}
						else if(!strcmp(opModeStr, "ITU G.993.5(G.Vectoring)"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.993.5(G.Vectoring)");
						}
						else if(!strcmp(opModeStr, "ITU G.993.5(G.Vectoring),G.998.4(G.INP)")) {
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.993.5(G.Vectoring),G.998.4(G.INP)");
						}
						else if(!strcmp(opModeStr, "ITU G.993.2(VDSL2), G.998.4(G.INP)")) {
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.993.2(VDSL2), G.998.4(G.INP)");
						}
						else if(!strcmp(opModeStr, "ITU G.992.3(ADSL2) ,G.998.4(G.INP)")) {
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.992.3(ADSL2) ,G.998.4(G.INP)");
						}
						else if(!strcmp(opModeStr, "ITU G.992.5(ADSL2PLUS) ,G.998.4(G.INP)")) {
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", "ITU G.992.5(ADSL2PLUS) ,G.998.4(G.INP)");
						}
						else if(!strcmp(opModeStr, "ANSI T1.413"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MODULATIONTYPE_T1_413);
						}
						else if(!strcmp(opModeStr, "ITU G.992.1(G.DMT)"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MODULATIONTYPE_G_DMT);
						}	
						else if(!strcmp(opModeStr, "ITU G.992.2(G.Lite)"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MODULATIONTYPE_G_LITE);
						}	
						else if(!strcmp(opModeStr, "ITU G.992.3(ADSL2)"))
						{
							if((!strcmp(annexType,"ANNEX A/L")) || (!strcmp(annexType,"ANNEX A/I/J/L/M")))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2,ANNEXTYPEA_L);
							}
							else if(!strcmp(annexType,"ANNEX M"))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2,ANNEXTYPEA_M);
							}
							else
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MTENSTANDARD_ADSL2);
							}
						}	
						else if(!strcmp(opModeStr, "ITU G.992.5(ADSL2PLUS)"))
						{
							if((!strcmp(annexType,"ANNEX A/L")) || (!strcmp(annexType,"ANNEX A/I/J/L/M")))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_L);
							}
							else if(!strcmp(annexType,"ANNEX M"))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_M);
							}
							else
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MTENSTANDARD_ADSL2PLUS);
							}
						}	
						else
						{
							if((!strcmp(annexType,"ANNEX A/L")) || (!strcmp(annexType,"ANNEX A/I/J/L/M")))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_L);
							}
							else if(!strcmp(annexType,"ANNEX M"))
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)", MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_M);
							}
							else
							{
								snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s", MTENSTANDARD_ADSL2PLUS);
							}
						}
					}
					else{
						if((!strcmp(annexType,"ANNEX A/L")) || (!strcmp(annexType,"ANNEX A/I/J/L/M")))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)",MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_L);
						}
						else if(!strcmp(annexType,"ANNEX M"))
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s (%s)",MTENSTANDARD_ADSL2PLUS,ANNEXTYPEA_M);
						}
						else
						{
							snprintf(tmpStandardStr, sizeof(tmpStandardStr), "%s",MTENSTANDARD_ADSL2PLUS);
						}
					}
				}
				else
				{
					free(buf);
					return FAIL;
				}
			}
			else
			{
				free(buf);
				return FAIL;
			}
			cfg_set_object_attr(node, "mtenStandard", tmpStandardStr);
		}
		free(buf);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}/*end adslInfo_read*/
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
int cfg_type_info_apcli_get(char* path,char* attr,char* val,int len)
{
	char sub_name[8] = {0};
	int idx = 0;
	
	if (cfg_check_create_object(path) < 0){
		printf("create %s Fail.\n", path);
		return -1;
	}

#if SMUX_ENABLE
	snprintf(sub_name, sizeof(sub_name), "nas_br");
#else
	sscanf(path, "root.info.%s", sub_name); 
#endif
	if ( !strcmp(attr, "hwaddr") ) {
		get_itf_hwaddr(path, sub_name);
	}
	else 
		get_itf_stat(path, sub_name);

	return cfg_obj_get_object_attr(path, attr, 0, val, len); 

}
#endif

int cfg_type_RomfileCheck_read(char* path, char* attr)
{
	char operatename[32] = {0};
	int ret = -1;

    memset(operatename,0, sizeof(operatename));
    if(path)
	{
		if(strcmp(attr, "Result") == 0)
		{
			cfg_set_object_attr(path, "Result", "0");
			ret = checkCwmpUploadedRomfile();
			if(ret<0)
			{
				cfg_set_object_attr(path, "Result", "0");
			}
			else
			{
				cfg_set_object_attr(path, "Result", "1");
			}
		}
    }
    else
    {
        return -1;
    }
	return 0;	
}

int cfg_type_info_get(char* path,char* attr,char* val,int len)
{
	char* ptmp = NULL;
	char* ptmp2 = NULL;
	char sub_name[8] = {0};
	char value[32];
#if defined(TCSUPPORT_ANDLINK)
	int idx = 0;
	static struct timespec pre_sec[4] = {0};
	struct timespec cur_sec = {0};
#endif

	if(strstr(path,"ether") != NULL ){
		if ( 1 == isInfoReading("info_ether_read"))
			return 0;
		if(cfg_type_etherInfo_read(path) < 0){
			printf("cfg_type_etherInfo_read fail.\n");
			return -1;
		}
	}

#if !defined(TCSUPPORT_NP)
	else if(strstr(path,"xpon") != NULL){
		if(cfg_type_xponInfo_read(path) < 0){
			printf("cfg_type_xponInfo_read fail.\n");
			return -1;
		}
	}
#if defined(TCSUPPORT_PONMGR) && (defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON))	
	else if(strstr(path, "ponphy") != NULL){
		if(cfg_type_ponPhyInfo_read(path) < 0){
			printf("cfg_type_ponPhyInfo_read fail.\n");
			return -1;
		}
	}
#if 0
	else if (strstr(path, "ponwanstats") != NULL){
		if(cfg_type_ponWanStatsInfo_read(path) < 0){
			printf("cfg_type_ponWanStatsInfo_read fail.\n");
			return -1;
		}
	}
#endif
#endif
#if defined(TCSUPPORT_WAN_EPON) && defined(TCSUPPORT_EPON_OAM)
	else if(strstr(path, "oam") != NULL){
		return 0;
	}
#endif
#endif
#if 0
#if defined(TCSUPPORT_WLAN)
#if defined(TCSUPPORT_WLAN_AC)

#ifdef TCSUPPORT_WLAN_BNDSTRG
	else if(strstr(path, "wlan11ac") != NULL && !strcmp(attr, "ChanLoad")){
		memset(value, 0, sizeof(value));
		wifimgr_lib_get_BndStrgChanload(value);
		cfg_set_object_attr(path, "ChanLoad", value);
		return 0;
		}
#endif

	else if(strstr(path, "wlan11ac") != NULL){
		if ( 1 == isInfoReading("info_wlan11ac_read"))
			return 0;
		if(cfg_type_wlan11acInfo_read(path) < 0){
			printf("cfg_type_wlan11acInfo_read fail.\n");
			return -1;
		}
		doCheck11acHtExtcha(path, 1);
	}
#endif	
	else if(strstr(path, "wlan") != NULL){
		if ( 1 == isInfoReading("info_wlan_read"))
			return 0;
		if(cfg_type_wlanInfo_read(path) < 0){
			printf("cfg_type_wlanInfo_read fail.\n");
			return -1;
		}
	}
#endif
#endif

	/*Get default device inforamtion of the cpe from config file /etc/devInf.conf. */
	else if(strstr(path, "devdefinf") != NULL) {
		if(cfg_type_devDefInf_read(path) < 0){
			printf("cfg_type_wlanInfo_read fail.\n");
			return -1;
		}
	}
	else if(strstr(path, "sysdevice") != NULL){
		if(cfg_type_sysDeviceInfo_read(path, attr) < 0){
			printf("cfg_type_sysDeviceInfo_read fail.\n");
			return -1;
		}
	 }
	else if((ptmp = strstr(path,"eth")) != NULL){
		if (cfg_check_create_object(path) < 0){
			printf("create %s Fail.\n", path);
			return -1;
		}
#if defined(TCSUPPORT_ANDLINK)
		sscanf(path, "root.info.eth0.%d", &idx);
		if ( idx < 1 || idx > 4)
			idx = 1;
		
		idx--;

		bzero(&cur_sec, sizeof(cur_sec));
		clock_gettime(CLOCK_MONOTONIC, &cur_sec);
		if ( (cur_sec.tv_sec - pre_sec[idx].tv_sec) <= 2 )
			return cfg_obj_get_object_attr(path, attr, 0, val, len);

		bzero(&pre_sec[idx], sizeof(pre_sec[idx]));
		clock_gettime(CLOCK_MONOTONIC, &pre_sec[idx]);
#endif
		
		if(NULL != (ptmp2 = strchr(ptmp, '.'))){
			strncpy(sub_name, ptmp, ptmp2 - ptmp + 2);
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
			if ( !strcmp(attr, "hwaddr") ) {
				if ( 'n' == sub_name[0] ) {
					if ( (ptmp = strstr(sub_name, ".")) != NULL ) {
						*ptmp = '_';
					}
					else
						return -1;
				}
				get_itf_hwaddr(path, sub_name);
			}
			else if ((strstr(sub_name, "eth0.") != NULL && 
				(!strcmp(attr, "detectionStatus") || !strcmp(attr, "Status"))))
			{
				loopdetect(path, sub_name);
			}
			else 
				get_itf_stat(path, sub_name);
#else
			ifInfo_read(path, sub_name);
#endif
		}else{
			return -1;
		}	
	}
	else if((ptmp = strstr(path,"br0")) != NULL)
	{
		if (cfg_check_create_object(path) < 0){
			printf("create %s Fail.\n", path);
			return -1;
		}
		ifInfo_read(path,ptmp);
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else if(strstr(path, "adsl") != NULL) 
	{
		if(cfg_type_adslInfo_read(path) < 0)
		{
			tcdbg_printf("cfg_type_adslInfo_read fail.\n");
			return -1;
		}
	}
#endif
	else if(strstr(path, "romfilecheck") != NULL)
	{
		if(cfg_type_RomfileCheck_read(path, attr) < 0){
			printf("cfg_type_RomfileCheck_read fail.\n");
			return -1;
		}
	}
	
	return 0;
}

int cfg_type_info_ponWanStats_get(char* path,char* attr,char* val,int len)
{
	static struct timespec pre_sec;
	struct timespec cur_sec = {0};

	bzero(&cur_sec, sizeof(cur_sec));
	clock_gettime(CLOCK_MONOTONIC, &cur_sec);
	if ( (cur_sec.tv_sec - pre_sec.tv_sec) <= 2 )
		return cfg_obj_get_object_attr(path, attr, 0, val, len);
#ifndef TCSUPPORT_NP	
#if defined(TCSUPPORT_PONMGR) && (defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON))	
	if(cfg_type_ponWanStatsInfo_read(path) < 0){
		printf("cfg_type_ponWanStatsInfo_read fail.\n");
		return -1;
	}
#endif
#endif
	bzero(&pre_sec, sizeof(pre_sec));
	clock_gettime(CLOCK_MONOTONIC, &pre_sec);

	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_info_nas_get(char* path,char* attr,char* val,int len)
{
	int idx1 = 0, idx2 = 0;
	char sub_name[8] = {0};
	char time[32] = {0};
	long pre_sec = 0;
	struct timespec cur_sec = {0};
#if defined(TCSUPPORT_CT_DBUS)
		char buf[4] = {0};
#endif

	if (cfg_check_create_object(path) < 0){
		printf("create %s Fail.\n", path);
		return -1;
	}

#if defined(TCSUPPORT_NP_CMCC)
	snprintf(sub_name, sizeof(sub_name), "nas0_0");
	get_itf_stat(path, sub_name);
#else
	sscanf(path, "root.info.nas%d.%d", &idx1, &idx2);
	if ( idx1 < 0 || idx1 > 12 )
		idx1 = 0;
	
	if ( idx2 < 0 || idx2 > 8 )
		idx2 = 1;
	
	idx2--;
	if ( cfg_obj_get_object_attr(path, "time", 0, time, sizeof(time)) > 0 )
		sscanf(time, "%ld", &pre_sec);
	
	bzero(&cur_sec, sizeof(cur_sec));
	clock_gettime(CLOCK_MONOTONIC, &cur_sec);
#if defined(TCSUPPORT_CT_DBUS) 
	cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", buf, sizeof(buf));
	if(strcmp(buf, "5") == 0)
	{
		if ( (cur_sec.tv_sec - pre_sec) <= 2 )
			return cfg_obj_get_object_attr(path, attr, 0, val, len);
	}
#endif

	snprintf(sub_name, sizeof(sub_name), "nas%d.%d", idx1, idx2);
	
#if defined(TCSUPPORT_CT_JOYME2)
		get_itf_stat(path, sub_name);
#else
		ifInfo_read(path, sub_name);
#endif
	bzero(&cur_sec, sizeof(cur_sec));
	clock_gettime(CLOCK_MONOTONIC, &cur_sec);
	memset(time, 0, sizeof(time));
	snprintf(time, sizeof(time), "%ld", cur_sec.tv_sec);
	cfg_obj_set_object_attr(path, "time", 0, time);
#endif
	return cfg_obj_get_object_attr(path, attr, 0, val, len);
}

int cfg_type_info_func_get(char* path,char* attr,char* val,int len)
{
	cfg_type_info_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len); 
}

static cfg_node_ops_t cfg_type_info_RomfileCheck_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_RomfileCheck = { 
	 .name = "RomfileCheck", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_RomfileCheck_ops, 
}; 

static cfg_node_ops_t cfg_type_info_ether_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
}; 

static cfg_node_type_t cfg_type_info_ether = { 
	 .name = "Ether", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_ether_ops, 
}; 

static cfg_node_ops_t cfg_type_info_usb_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_usb = { 
	 .name = "Usb", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_usb_ops, 
}; 

static cfg_node_ops_t cfg_type_info_xpon_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_xpon = { 
	 .name = "Xpon", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_xpon_ops, 
}; 

static cfg_node_ops_t cfg_type_info_ponPhy_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_ponPhy = { 
	 .name = "PonPhy", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_ponPhy_ops, 
}; 

static cfg_node_ops_t cfg_type_info_ponWanStats_ops  = { 
	 .get = cfg_type_info_ponWanStats_get,
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_ponWanStats = { 
	 .name = "PonWanStats", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_ponWanStats_ops, 
}; 

static cfg_node_ops_t cfg_type_info_oam_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_oam = { 
	 .name = "Oam", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_oam_ops, 
}; 

static cfg_node_ops_t cfg_type_info_simWebAuth_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_simWebAuth = { 
	 .name = "SIMWebAuth", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_simWebAuth_ops, 
}; 

#ifdef TCSUPPORT_WLAN
static cfg_node_ops_t cfg_type_info_wlan_ops  = { 
	 .get = cfg_type_info_wlan_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_wlan = { 
	 .name = "WLan", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_wlan_ops, 
}; 

#ifdef TCSUPPORT_WLAN_AC
static cfg_node_ops_t cfg_type_info_wlan11ac_ops  = { 
	 .get = cfg_type_info_wlan11ac_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_wlan11ac = { 
	 .name = "WLan11ac", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_wlan11ac_ops, 
}; 
#endif
#endif

static cfg_node_ops_t cfg_type_info_devDefInf_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_devDefInf = { 
	 .name = "devDefInf", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_devDefInf_ops, 
}; 

static cfg_node_ops_t cfg_type_info_sysDevice_ops  = { 
	 .get = cfg_type_info_func_get,
	 .set = cfg_type_default_func_set,
};

static cfg_node_type_t cfg_type_info_sysDevice = { 
	 .name = "SysDevice", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_sysDevice_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas0_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas0 = { 
	 .name = "nas0", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas0_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas1_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas1 = { 
	 .name = "nas1", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas1_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas2_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas2 = { 
	 .name = "nas2", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas2_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas3_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas3 = { 
	 .name = "nas3", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas3_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas4_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas4 = { 
	 .name = "nas4", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas4_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas5_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas5 = { 
	 .name = "nas5", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas5_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas6_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas6 = { 
	 .name = "nas6", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas6_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas7_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas7 = { 
	 .name = "nas7", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas7_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas8_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas8 = { 
	 .name = "nas8", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas8_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas9_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas9 = { 
	 .name = "nas9", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas9_ops, 
}; 

static cfg_node_ops_t cfg_type_info_nas10_ops  = { 
	 .get = cfg_type_info_nas_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_nas10 = { 
	 .name = "nas10", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_nas10_ops, 
}; 

#ifdef TCSUPPORT_WLAN
static cfg_node_ops_t cfg_type_info_ra_ops  = { 
	 .get = cfg_type_info_ra_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_ra = { 
	 .name = "ra", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_ra_ops, 
}; 

#ifdef TCSUPPORT_WLAN_AC
static cfg_node_ops_t cfg_type_info_rai_ops  = { 
	 .get = cfg_type_info_rai_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_rai = { 
	 .name = "rai", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_rai_ops, 
}; 
#endif
#endif

static cfg_node_ops_t cfg_type_info_eth_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_ops_t cfg_type_info_br0_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
}; 


static cfg_node_type_t cfg_type_info_eth = { 
	 .name = "eth0", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 5, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_eth_ops, 
}; 

#if defined(TCSUPPORT_CT_DSL_EX)
static cfg_node_ops_t cfg_type_info_adsl_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_adsl = { 
	 .name = "Adsl", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_adsl_ops, 
}; 
#endif

int cfg_type_info_romfilechk_get(char* path,char* attr,char* val,int len)
{
	mxml_node_t *tree = NULL;
	FILE *fp = NULL;
	int flag = -1;
#define UPLOAD_ROMFILE_PATH		"/var/tmp/up_romfile.cfg"
#define CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME "/tmp/cwmp_ctcheckromfile.cfg"

	if ( cfg_check_create_object(path) < 0) {
		printf("create %s Fail.\n", path);
		return -1;
	}

	cfg_obj_set_object_attr(path, "Result", 0, "0");

#if defined(TCSUPPORT_CT_CWMP_ZIPROMFILE)
	flag = check_checksum(UPLOAD_ROMFILE_PATH, CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME);
	if( flag < 0 )
	{
		goto romfile_chk_end;
	}

	fp = fopen(CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME, "r");
	if( NULL == fp )
	{
		goto romfile_chk_end;
	}
#else
	fp = fopen(UPLOAD_ROMFILE_PATH, "r");
	if( fp == NULL )
	{
		goto romfile_chk_end;
	}
#endif
	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	fclose(fp);
#if defined(TCSUPPORT_CT_CWMP_ZIPROMFILE)
	unlink(CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME);
#endif

	if( tree == NULL )
	{
		unlink(UPLOAD_ROMFILE_PATH);
		goto romfile_chk_end;
	}
	else
	{
		/*Delete cfg node tree that is used to verfiy_upload_romfile*/
		mxmlDelete(tree);

		/*set Result = 1 to inform tr069 checking file succeed, use orig file to upgrade*/
		cfg_obj_set_object_attr(path, "Result", 0, "1");	
	}
romfile_chk_end:

	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

#if defined(TCSUPPORT_WLAN_APCLIENT)
static cfg_node_ops_t cfg_type_info_apcli_ops  = { 
	 .get = cfg_type_info_apcli_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_apcli = { 
	 .name = "apcli0", 
	 .flag = 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_apcli_ops, 
};

static cfg_node_ops_t cfg_type_info_apclii_ops  = { 
	 .get = cfg_type_info_apcli_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_apclii = { 
	 .name = "apclii0", 
	 .flag = 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_apclii_ops, 
};

#endif

static cfg_node_ops_t cfg_type_info_romfilechk_ops  = { 
	 .get = cfg_type_info_romfilechk_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_info_romfilechk = { 
	 .name = "RomfileCheck", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_info, 
	 .ops = &cfg_type_info_romfilechk_ops, 
};

static cfg_node_type_t cfg_type_info_br0 = {
	.name = "br0", 
	.flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 1, 
	.parent = &cfg_type_info, 
	.ops = &cfg_type_info_br0_ops, 
		
};


static cfg_node_ops_t cfg_type_info_ops  = { 
	 .get = cfg_type_info_func_get, 
	 .set = cfg_type_default_func_set,
}; 

static cfg_node_type_t* cfg_type_info_child[] = { 
	 &cfg_type_info_ether,
#ifndef TCSUPPORT_NP_CMCC
	 &cfg_type_info_usb,
#endif
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
	 &cfg_type_info_xpon,
	 &cfg_type_info_ponPhy,
	 &cfg_type_info_ponWanStats,
	 &cfg_type_info_oam,
	 &cfg_type_info_simWebAuth,
#endif
#endif
#ifdef TCSUPPORT_WLAN
	 &cfg_type_info_wlan,
#ifdef TCSUPPORT_WLAN_AC
	 &cfg_type_info_wlan11ac,
#endif
#endif
	 &cfg_type_info_devDefInf,
	 &cfg_type_info_sysDevice,
	 &cfg_type_info_nas0, 
	 &cfg_type_info_nas1, 
	 &cfg_type_info_nas2, 
	 &cfg_type_info_nas3, 
	 &cfg_type_info_nas4, 
	 &cfg_type_info_nas5, 
	 &cfg_type_info_nas6, 
	 &cfg_type_info_nas7, 
	 &cfg_type_info_nas8, 
	 &cfg_type_info_nas9, 
	 &cfg_type_info_nas10, 
	 &cfg_type_info_eth,
#ifdef TCSUPPORT_WLAN
	 &cfg_type_info_ra,	
#ifdef TCSUPPORT_WLAN_AC 
	 &cfg_type_info_rai,
#endif
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
	 &cfg_type_info_adsl,
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
	 &cfg_type_info_apcli,
	 &cfg_type_info_apclii,
#endif
	 &cfg_type_info_RomfileCheck,
	 &cfg_type_info_romfilechk,
	 &cfg_type_info_br0,
	 NULL 
}; 


cfg_node_type_t cfg_type_info = { 
	 .name = "Info", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_info_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_info_child, 
	 .ops = &cfg_type_info_ops, 
}; 
