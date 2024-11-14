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
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h"

static char* cfg_type_WIFIRdo_index[] = { 
	 "wlan_rdo_id", 
	 NULL 
}; 


int getWifiRoIdxByBnd( int bndflag){
	int i = 0;
	int flag = -1;

	for(i = 0; i < WIFI_RDO_NUM_MAX; i++){
		flag = getWifiBndByWifiRoIdx(i);
		if(flag == bndflag ){
			return i;
		}
	}
	
	return -1;
}

int wlanNodeSync2WIFIRdo(int bndFlag)
{
	char nodePath[64] = {0};
	char webCusTomNodePath[64] = {0};
	char tr181nodePath[64] = {0};
	char tmp[256] = {0};			
	char ht_bw[8] = {0};
	char ht_bsscoexist[8] = {0};
	char isHtBw40[8] = {0};
	char wlanEntryPath[64] = {0};

	if(bndFlag == 1){/*5G*/ 
		memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
		snprintf(tr181nodePath,sizeof(tr181nodePath), TR181_WIFIRDO_ENTRY_NODE, 2);
		if(SUCCESS != checkNodePath(tr181nodePath)){
			return FAIL;
		}
		memset(nodePath, 0x00, sizeof(nodePath));
		snprintf(nodePath,sizeof(nodePath), WLAN11AC_COMMON_NODE);

		memset(wlanEntryPath, 0x00, sizeof(wlanEntryPath));
		snprintf(wlanEntryPath,sizeof(wlanEntryPath), WLAN11AC_ENTRY_N_NODE,1);
		
		cfg_obj_set_object_attr(tr181nodePath, "SupFreqBnd",0, TR181_WIFIRDO_BND_5G);	
		cfg_obj_set_object_attr(tr181nodePath, "OptFreqBnd",0, TR181_WIFIRDO_BND_5G);
		cfg_obj_set_object_attr(tr181nodePath, "SupStandards",0, "a,n,ac");

		if(cfg_obj_get_object_attr(nodePath, "CountryRegionABand", 0,tmp,sizeof(tmp)) >0){
			if(strcmp(tmp, "0") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64,149,153,157,161,165");
			else if(strcmp(tmp, "1") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140");
			else if(strcmp(tmp, "2") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64");
			else if(strcmp(tmp, "3") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "52,56,60,64,149,153,157,161");
			else if(strcmp(tmp, "4") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "149,153,157,161,165");
			else if(strcmp(tmp, "5") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "149,153,157,161");
			else if(strcmp(tmp, "6") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48");
			else if(strcmp(tmp, "8") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "52,56,60,64");
			else if(strcmp(tmp, "9") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64,100,104,108,112,116,132,136,140,149,153,157,161,165");
			else if(strcmp(tmp, "10") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,149,153,157,161,165");
			else if(strcmp(tmp, "11") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64,100,104,108,112,116,120,149,153,157,161");
			else
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,149,153,157,161,165");
		}
	}else{/*2.4G*/
		memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
		snprintf(tr181nodePath,sizeof(tr181nodePath), TR181_WIFIRDO_ENTRY_NODE, 1);
		if(SUCCESS != checkNodePath(tr181nodePath)){
			return FAIL;
		}
		memset(nodePath, 0x00, sizeof(nodePath));
		snprintf(nodePath,sizeof(nodePath), WLAN_COMMON_NODE);

		memset(wlanEntryPath, 0x00, sizeof(wlanEntryPath));
		snprintf(wlanEntryPath,sizeof(wlanEntryPath), WLAN_ENTRY_N_NODE,1);
		
		cfg_obj_set_object_attr(tr181nodePath, "SupFreqBnd",0, TR181_WIFIRDO_BND_2_4G); 
		cfg_obj_set_object_attr(tr181nodePath, "OptFreqBnd",0, TR181_WIFIRDO_BND_2_4G);
		cfg_obj_set_object_attr(tr181nodePath, "SupStandards",0, "b,g,n");

		if(cfg_obj_get_object_attr(nodePath, "CountryRegion",0, tmp,sizeof(tmp)) > 0){
			if(strcmp(tmp, "0") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "1-11");
			else if(strcmp(tmp, "2") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "10-11");
			else if(strcmp(tmp, "3") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "10-13");
			else if(strcmp(tmp, "4") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "4-4");
			else if(strcmp(tmp, "5") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "1-14");
			else if(strcmp(tmp, "6") == 0)
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "3-9");
			else
				cfg_obj_set_object_attr(tr181nodePath, "PossChannels",0, "1-13");	
		}
	}

	/* OperatingStandards*/
	if(cfg_obj_get_object_attr(nodePath, "WirelessMode", 0,tmp,sizeof(tmp)) >0){
		if(strcmp(tmp, "1") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "b");
		else if(strcmp(tmp, "4") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "g");
		else if(strcmp(tmp, "0") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "b,g");
		else if(strcmp(tmp, "6") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "n");
		else if(strcmp(tmp, "7") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "g,n");
		else if(strcmp(tmp, "9") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "b,g,n");
		else if(strcmp(tmp, "2") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "a");
		else if(strcmp(tmp, "8") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "a,n");
		else if(strcmp(tmp, "14") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "ac,n,a");
		else if(strcmp(tmp, "15") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "ac,n");
		else if(bndFlag == 1)
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "ac,n");
		else
			cfg_obj_set_object_attr(tr181nodePath, "OptStandards",0, "b,g,n");
	}

	/*Channel,AutoChannelSupported,AutoChannelEnable*/
	memset(tmp, 0x00, sizeof(tmp));
	cfg_obj_set_object_attr(tr181nodePath, "AutoChannelSupport",0, "1");
	if(cfg_obj_get_object_attr(nodePath, "Channel", 0,tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "Channel",0, tmp);
		if(strcmp(tmp, "0") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "AutoChannel",0, "1");
		else
			cfg_obj_set_object_attr(tr181nodePath, "AutoChannel",0, "0");
	}

	/*OperatingChannelBandwidth*/
	memset(tmp, 0x00, sizeof(tmp)); 
	memset(webCusTomNodePath, 0x00, sizeof(webCusTomNodePath));
	snprintf(webCusTomNodePath,sizeof(webCusTomNodePath), WEBCUSTOM_ENTRY_NODE);

	cfg_obj_get_object_attr(nodePath, "11nMode",0, tmp ,sizeof(tmp));
	if(!strcmp(tmp,"1")){
		memset(isHtBw40, 0x00, sizeof(isHtBw40));
		memset(ht_bw, 0x00, sizeof(ht_bw));
		memset(ht_bsscoexist, 0x00, sizeof(ht_bsscoexist));
		cfg_obj_get_object_attr(webCusTomNodePath, "isHTBW40M",0, isHtBw40,sizeof(isHtBw40));
		
		cfg_obj_get_object_attr(nodePath, "HT_BW", 0,ht_bw,sizeof(ht_bw));
		
		cfg_obj_get_object_attr(nodePath, "HT_BSSCoexistence",0, ht_bsscoexist,sizeof(ht_bsscoexist));
		
		if(!strcmp(ht_bw, "1")){	/*support 40M*/
			if(!strcmp(isHtBw40, "Yes") && !strcmp(ht_bsscoexist, "0")){
				cfg_obj_set_object_attr(tr181nodePath, "OptChannelBw",0, "40MHz");
			}else{
				cfg_obj_set_object_attr(tr181nodePath, "OptChannelBw",0, "Auto");
			}	
		}else{/*only support 20M*/
			cfg_obj_set_object_attr(tr181nodePath, "OptChannelBw",0, "20MHz");
		}
	}
	else
		cfg_obj_set_object_attr(tr181nodePath, "OptChannelBw",0, "20MHz");

	/*ExtensionChannel*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, "HT_EXTCHA",0, tmp,sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0)
			cfg_obj_set_object_attr(tr181nodePath, "HT_EXTCHA",0, "BelowControlChannel");
		else
			cfg_obj_set_object_attr(tr181nodePath, "HT_EXTCHA",0, "AboveControlChannel");
	}

	/*GuardInterval*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, "HT_GI", 0,tmp,sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0)
		{
			cfg_obj_set_object_attr(tr181nodePath, "HT_GI",0, "800nsec");
		}	
		else
		{
				if(bndFlag == 1)
				{
			cfg_obj_set_object_attr(tr181nodePath, "HT_GI",0, "Auto");
	}
				else
				{
					cfg_obj_set_object_attr(tr181nodePath, "HT_GI",0, "400nsec");
				}	
		}
	}

	/*MCS*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryPath, "HT_MCS",0, tmp,sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(tr181nodePath, "HT_MCS",0, tmp);
	}

	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, "APOn",0, tmp,sizeof(tmp)) >=0)
	{
		if(strcmp(tmp, "1") == 0)
		{
			cfg_set_object_attr(tr181nodePath ,"Enable", "1");
		}
		else
		{
			cfg_set_object_attr(tr181nodePath ,"Enable", "0");
		}
	}

	return SUCCESS;
}



int ExtChOptCal(int is5g, int ChannelId){
	int ret = -1;	/*-1 error, 0 BelowControlChannel, 1 AboveControlChannel, 2 Config*/
	int channelvalue = ChannelId;
	int Ht5GExtChvalue[] = {1,0};
	if(is5g){
		if(ChannelId>=36 && ChannelId<=64){
			channelvalue /= 4;
			channelvalue -= 9;
			ret = Ht5GExtChvalue[channelvalue%2];
		}
		else if(ChannelId>=100 && ChannelId<=136){
			channelvalue /= 4;
			channelvalue -= 17;	
			ret = Ht5GExtChvalue[channelvalue%2];
		}
		else if(ChannelId>=149 && ChannelId<=161){
			channelvalue -= 1;
			channelvalue /= 4;
			channelvalue -= 19;	
			ret = Ht5GExtChvalue[channelvalue%2];
		}
		else
			ret = 0;
	}
	else{
		if(ChannelId>=0 && ChannelId<=4)
			ret = 1;
		else if(ChannelId>=8)
			ret = 0;
		else if(ChannelId>4 && ChannelId<8)
			ret = 2;
	}

	return ret;
}

static int wifiRdoSync2Wlan( int bndFlag, int index){
	char wifirdoPath[64] = {0};
	char nodeNamePath[64] = {0};	
	char wlanPath[64] = {0};
	char tmp[256] = {0}, tmpmode[256] = {0},tmpbw[256] = {0};
	int channelnum = 0, ret = 0;
	int wifiType = -1;
	
	if(index < 0){
		return FAIL;
	}

	/*check for WLan or WLan11ac*/
	memset(wlanPath, 0, sizeof(wlanPath));
	memset(nodeNamePath, 0, sizeof(nodeNamePath));
	if(bndFlag == 1){
		snprintf(wlanPath,sizeof(wlanPath), WLAN_ENTRY_N_NODE, 1);			/*set HT_MCS default in WLan_Entry0*/
		snprintf(nodeNamePath,sizeof(nodeNamePath), WLAN_COMMON_NODE);
		wifiType = 0;
	}else{
		snprintf(wlanPath,sizeof(wlanPath), WLAN11AC_ENTRY_N_NODE, 1);		/*set HT_MCS default in WLan11AC_Entry0*/
		snprintf(nodeNamePath,sizeof(nodeNamePath), WLAN11AC_COMMON_NODE);
		wifiType = 1;
	}	

	snprintf(wifirdoPath,sizeof(wifirdoPath), TR181_WIFIRDO_ENTRY_NODE, index);

	/* OperatingStandards*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifirdoPath, "OptStandards",0, tmp,sizeof(tmp)) >= 0){
		if(strcmp(tmp, "b") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "1");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "3");
		}
		else if(strcmp(tmp, "g") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "4");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "351");
		}
		else if(strcmp(tmp, "b,g") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "0");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "15");
		}
		else if(strcmp(tmp, "n") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "6");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "351");
		}
		else if(strcmp(tmp, "g,n") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "7");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "351");
		}
		else if(strcmp(tmp, "b,g,n") == 0){
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "9");
			cfg_obj_set_object_attr(nodeNamePath, "BasicRate",0, "351");
		}
		else if(strcmp(tmp, "a") == 0)
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "2");
		else if(strcmp(tmp, "a,n") == 0)
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "8");
		else if(strcmp(tmp, "ac,n,a") == 0)
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "14");
		else if(strcmp(tmp, "ac,n") == 0)
			cfg_obj_set_object_attr(nodeNamePath, "WirelessMode",0, "15");
	}

	/*Channel*/
	
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifirdoPath, "Channel",0, tmp,sizeof(tmp)) >=0){	
		cfg_obj_set_object_attr(nodeNamePath, "Channel",0, tmp);
	}
	
	/*OperatingChannelBandwidth, ExtensionChannel, GuardInterval, MCS*/
	memset(tmp, 0x00, sizeof(tmp));
	memset(tmpbw, 0x00, sizeof(tmpbw));
	memset(tmpmode, 0x00, sizeof(tmpmode));
	
	cfg_obj_get_object_attr(nodeNamePath, "11nMode",0, tmpmode,sizeof(tmpmode));
	cfg_obj_get_object_attr(wifirdoPath, "OptChannelBw",0, tmpbw,sizeof(tmpbw));

	if(!strcmp(tmpmode,"1")){
		if(!strcmp(tmpbw, "20MHz"))
			cfg_obj_set_object_attr(nodeNamePath, "HT_BW",0, "0");
		else{
			cfg_obj_set_object_attr(nodeNamePath, "HT_BW",0, "1");
			if(cfg_obj_get_object_attr(WEBCUSTOM_ENTRY_NODE, "isHTBW40M",0, tmp,sizeof(tmp)) >=0 && !strcmp(tmp, "Yes")){
				if(!strcmp(tmpbw, "40MHz"))
					cfg_obj_set_object_attr(nodeNamePath, "HT_BSSCoexistence",0, "0");
				else
					cfg_obj_set_object_attr(nodeNamePath, "HT_BSSCoexistence",0, "1");
			}
			
			/*HT_EXTCHA*/
			memset(tmp, 0x00, sizeof(tmp));
			if((cfg_obj_get_object_attr(nodeNamePath, "Channel",0, tmp,sizeof(tmp)) >=0) && (isNumber(tmp))){
				channelnum = atoi(tmp);
				if(0 == wifiType)
					ret = ExtChOptCal(0, channelnum);
				else
					ret = ExtChOptCal(1, channelnum);
				
				if(ret == 0)
					cfg_obj_set_object_attr(nodeNamePath, "HT_EXTCHA",0, "0");
				else if(ret == 1)
					cfg_obj_set_object_attr(nodeNamePath, "HT_EXTCHA",0, "1");
				else if((ret==2) && (cfg_obj_get_object_attr(wifirdoPath, "HT_EXTCHA",0, tmp,sizeof(tmp))>=0)){
					if(strcmp(tmp, "BelowControlChannel") == 0)
						cfg_obj_set_object_attr(nodeNamePath, "HT_EXTCHA",0, "0");
					else
						cfg_obj_set_object_attr(nodeNamePath, "HT_EXTCHA",0, "1");
				}
				
			}
		}
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(wifirdoPath, "HT_GI",0, tmp,sizeof(tmp)) >=0){
			if(!strcmp(tmp, "800nsec"))
				cfg_obj_set_object_attr(nodeNamePath, "HT_GI",0, "0");
			else
				cfg_obj_set_object_attr(nodeNamePath, "HT_GI",0, "1");
		}
		
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(wifirdoPath, "HT_MCS",0, tmp,sizeof(tmp)) >=0){
			cfg_obj_set_object_attr(wlanPath, "HT_MCS",0, tmp);
		}
	}
	else{
		cfg_obj_set_object_attr(nodeNamePath, "HT_BW",0, "0");
		cfg_obj_set_object_attr(wlanPath, "HT_MCS",0, "33");
	}

	/*sync Enable*/
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(wifirdoPath, "Enable",0, tmp,sizeof(tmp)) >=0)
		{
			if(strcmp(tmp, "1") == 0)
			{
				cfg_set_object_attr(nodeNamePath ,"APOn", "1");
			}
			else
			{
				cfg_set_object_attr(nodeNamePath ,"APOn", "0");
			}
		}

	return 0;
}
int getWifiBndByWifiRoIdx( int rdoIndex){
	char wifiRdoPath[64] = {0};	
	char tmp[32] ={0};
	int bndFlag = 0;

	if(rdoIndex < 0 || rdoIndex > WIFI_RDO_NUM_MAX){
		tcdbg_printf("[%s:%d]rdoIndex=%d, error\n",__FUNCTION__,__LINE__,rdoIndex);
		return FAIL;
	}

	snprintf(wifiRdoPath, sizeof(wifiRdoPath), TR181_WIFIRDO_ENTRY_NODE, rdoIndex + 1);

	memset(tmp, 0, sizeof(tmp));
	/*cfg_obj_get_object_attr(wifiRdoPath, "SupFreqBnd", 0, tmp, sizeof(tmp));*/
	cfg_obj_get_object_attr(wifiRdoPath, "OptFreqBnd", 0, tmp, sizeof(tmp));
	if(strcmp(tmp, TR181_WIFIRDO_BND_5G) == 0)
		bndFlag = 1;

	return bndFlag;
}

static int cfg_type_WIFIRdo_func_commit(char* path)
{
	int etyIdx = -1;
	char tmp[256] = {0};
	int bndFlag = 0;
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}

	/*check for WLan or WLan11ac*/
	bndFlag = getWifiBndByWifiRoIdx(etyIdx);
	wifiRdoSync2Wlan(bndFlag, etyIdx);

	if(bndFlag == 1){
		cfg_tr181_commit_object(WLAN_NODE);
	}else{
		cfg_tr181_commit_object(WLAN11AC_NODE);
	}

	return 0;
}

int cfg_type_WIFIRdo_func_get(char* path,char* attr,char* val,int len)
{
	int i = 0;
	int matchIndex = -1;
	int etyIdx = -1;
	char tmp[8] = {0};
	int bndFlag = 0;
	char nodeName[64] = {0};
	char ifData[64] = {0};
	unsigned long long ifData_total = {0};

	char ifInfo_attr[][16] = {
		{"rxpackets"},	
		{"txpackets"},
		{"rxbytes"},		
		{"txbytes"},
		{"rxerrors"},		
		{"txerrors"},
		{"rxdropped"},		
		{"txdropped"},		
		{""}
	};
		
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			etyIdx = atoi(tmp) + 1;
		}
		else
		{
			return -1;
		}
	}

	for(i=0; '\0' != ifInfo_attr[i][0]; i++ )
	{
		if(strcmp(ifInfo_attr[i], attr) == 0)
		{
			matchIndex=i;
			break;
		}
	}

	if(-1 != matchIndex)
	{
		/*check for WLan[1] or WLan11ac[0]*/
		bndFlag = getWifiBndByWifiRoIdx(etyIdx);
		for(i=0; i < MAX_BSSID_NUM; i++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			if(bndFlag == 1){
				snprintf(nodeName, sizeof(nodeName), INFO_RA_N_NODE, i+1);	
			}
			else{
				snprintf(nodeName, sizeof(nodeName), INFO_RAI_N_NODE, i+1);
			}
		
			memset(ifData, 0, sizeof(ifData));
			cfg_get_object_attr(nodeName, ifInfo_attr[matchIndex], ifData, sizeof(ifData));
			ifData_total += atoi(ifData);
		}
		
		memset(nodeName, 0, sizeof(nodeName));
		if(bndFlag == 1){
			snprintf(nodeName, sizeof(nodeName), TR181_WIFIRDO_ENTRY_NODE, 1);	
		}
		else{
			snprintf(nodeName, sizeof(nodeName), TR181_WIFIRDO_ENTRY_NODE, 2);	
		}
		
		memset(ifData, 0, sizeof(ifData));
		snprintf(ifData,sizeof(ifData), "%llu", ifData_total);
		cfg_obj_set_object_attr(nodeName, ifInfo_attr[matchIndex], 0, ifData);
	}
	
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

static cfg_node_ops_t cfg_type_WIFIRdo_entry_ops  = { 
	 .get = cfg_type_WIFIRdo_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIRdo_func_commit 
}; 
static cfg_node_ops_t cfg_type_WIFIRdo_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIRdo_func_commit 
}; 
static cfg_node_ops_t cfg_type_WIFIRdo_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_WIFIRdo_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | WIFI_RDO_NUM_MAX, 
	 .parent = &cfg_type_WIFIRdo, 
	 .index = cfg_type_WIFIRdo_index, 
	 .ops = &cfg_type_WIFIRdo_entry_ops, 
}; 
static cfg_node_type_t cfg_type_WIFIRdo_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_WIFIRdo, 
	 .index = cfg_type_WIFIRdo_index, 
	 .ops = &cfg_type_WIFIRdo_common_ops, 
}; 

static cfg_node_type_t* cfg_type_WIFIRdo_child[] = { 
	 &cfg_type_WIFIRdo_entry,
	 &cfg_type_WIFIRdo_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_WIFIRdo = { 
	 .name = "WIFIRdo", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_WIFIRdo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_WIFIRdo_child, 
	 .ops = &cfg_type_WIFIRdo_ops, 
}; 
