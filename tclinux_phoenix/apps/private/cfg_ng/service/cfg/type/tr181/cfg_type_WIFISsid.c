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

static char* cfg_type_WIFISsid_index[] = { 
	 "ssid_id", 
	 NULL 
}; 

static int	getWlanBssidNum( int bndFlag){
	char wlanPath[64] = {0};	
	char tmp[256] = {0};

	memset(wlanPath, 0x00, sizeof(wlanPath));
	if(bndFlag){
		snprintf(wlanPath,sizeof(wlanPath),WLAN11AC_COMMON_NODE);
	}else{
		snprintf(wlanPath,sizeof(wlanPath),WLAN_COMMON_NODE);
	}
	
	if(cfg_get_object_attr(wlanPath, "BssidNum", tmp,sizeof(tmp)) > 0){
		/*Wlan_Entry not exist*/
		return atoi(tmp);
	}

	return -1;
}

static int	setWlanBssidNum( int bndFlag, int num){
	char wlanPath[64] = {0};	
	char tmp[32] = {0};

	memset(wlanPath, 0x00, sizeof(wlanPath));
	if(bndFlag){
		snprintf(wlanPath,sizeof(wlanPath),WLAN11AC_COMMON_NODE);
	}else{
		snprintf(wlanPath,sizeof(wlanPath),WLAN_COMMON_NODE);
	}
	
	snprintf(tmp, sizeof(tmp), "%d", num);
	cfg_set_object_attr( wlanPath, "BssidNum", tmp);

	return 0;
}

static int delWLanNodeByLanPortIdx( int lanPortIdx){
	char wlanPath[64] ={0};
	int wlanIdx = -1;
	
	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]lanPortIdx=%d, error\n",__FUNCTION__,__LINE__,lanPortIdx);
		return FAIL;
	}

	if( lanPortIdx >= LAN_INDEX_WLAN_5G_START && lanPortIdx < (LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM)){
		/*5G node*/
		wlanIdx = lanPortIdx - LAN_INDEX_WLAN_5G_START ;
		snprintf(wlanPath, sizeof(wlanPath), WLAN11AC_ENTRY_N_NODE, wlanIdx + 1);

		
	}else if( lanPortIdx >= LAN_INDEX_WLAN_START && lanPortIdx < LAN_INDEX_WLAN_5G_START ){
		wlanIdx = lanPortIdx - LAN_INDEX_WLAN_START ;		
		snprintf(wlanPath, sizeof(wlanPath), WLAN_ENTRY_N_NODE, wlanIdx + 1);

	}else{
		tcdbg_printf("[%s:%d]lanPortIdx=%d, error\n",__FUNCTION__,__LINE__,lanPortIdx);
		return FAIL;
	}
	
	return cfg_obj_delete_object(wlanPath);
}
static int getEmtryLanPortByWifiSsid( int bndFlag){
	char wifiSsidPath[64] = {0};	
	char tmp[256] = {0};
	int i = 0;
	int start = 0;
	int ssidIdx = 0;
	
	if(bndFlag == 1){
		start = LAN_INDEX_WLAN_5G_START;
	}else{
		start = LAN_INDEX_WLAN_START;
	}
	
	for(i = start; i < start + MAX_BSSID_NUM; i++){
		for(ssidIdx = 0; ssidIdx < WIFI_NUM_MAX; ssidIdx++){		
			memset(wifiSsidPath, 0x00, sizeof(wifiSsidPath));
			snprintf(wifiSsidPath, sizeof(wifiSsidPath), TR181_WIFISSID_ENTRY_NODE, ssidIdx + 1);
			if(cfg_obj_get_object_attr(wifiSsidPath, LAN_INDEX_ATTR,0, tmp,sizeof(tmp)) > 0){
				if(atoi(tmp) == i){
					break;
				}
			}
		}
		if(ssidIdx == WIFI_NUM_MAX){
			/*not find lanportindex in WIFISsid*/
			return i;
		}
	
}

	return -1;
}

int getWifiBndByLanPortIdx( int lanPortIdx){
	int bndFlag = -1;

	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]rdoIndex=%d, error\n",__FUNCTION__,__LINE__,lanPortIdx);
		return FAIL;
	}

	if( lanPortIdx >= LAN_INDEX_WLAN_5G_START && lanPortIdx < LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM){
		/*5G node*/
		bndFlag = 1;
	}else if( lanPortIdx >= LAN_INDEX_WLAN_START && lanPortIdx < LAN_INDEX_WLAN_5G_START ){
		bndFlag = 0;
	}

	return bndFlag;
}


int
wlanNodeSync2WIFISsidByIdx( int lanPortIdx, int ssidIdx){
	char nodeNamePath[64] = {0};
	char tr181nodePath[64] = {0};
	char tmp[256] = {0};	
	int bndFlag = -1;
	int rdoIdx = -1;

	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]lanPortIdx=%d, error\n",__FUNCTION__,__LINE__, lanPortIdx);
		return FAIL;
	}

	if(ssidIdx < 0){
		tcdbg_printf("[%s:%d]ssidIndex=%d, error\n",__FUNCTION__,__LINE__, ssidIdx);
		return FAIL;
	}

	bndFlag = getWifiBndByLanPortIdx( lanPortIdx);
	if(bndFlag < 0){
		tcdbg_printf("[%s:%d]bndFlag=%d, lanPortIdx=%d, error\n",__FUNCTION__,__LINE__,bndFlag, lanPortIdx);
		return FAIL;
	}

	setWlanNodeByLanPortIdx(nodeNamePath, lanPortIdx);

	memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
	snprintf(tr181nodePath, sizeof(tr181nodePath), TR181_WIFISSID_ENTRY_NODE, ssidIdx + 1);
	
	if(SUCCESS != checkNodePath(tr181nodePath)){
		return FAIL;
	}

	/* Enable*/	
	
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "EnableSSID",0, tmp,sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(tr181nodePath, "Enable",0, tmp);
	}
	
	/* SSID*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "SSID", 0, tmp,sizeof(tmp)) > 0){
		
		cfg_obj_set_object_attr(tr181nodePath, "SSID",0, tmp);
	}

	/*lanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, "%d", lanPortIdx);
	cfg_obj_set_object_attr(tr181nodePath, LAN_INDEX_ATTR,0, tmp);

	/*lowerlayer*/
	rdoIdx = getWifiRoIdxByBnd( bndFlag);		
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, TR181_WIFI_RDAIO"%d", rdoIdx + 1);
	setWifiSSIDLowerLayer(ssidIdx, tmp); 
    return SUCCESS;
}

int createWifiSSIDEntry( int lanPort){		
	struct tr181_interface_s interface;
	int index = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName,TR181_WIFISSID_NODE, sizeof(interface.nodeName)-1);
	interface.num = WIFI_SSID_NUM_MAX;
	interface.pvcIndex = -1;
	interface.lanIndex = lanPort;
	interface.active = 1;
	
	index = createTR181InterfaceEntry(interface);
	if(index < 0){
		return -1;
	}

	wlanNodeSync2WIFISsidByIdx(lanPort, index);

	return index;
}

void setWifiSSIDLowerLayer( int wifiSSIDIndex, char* lowerlayer){
	char nodeNamePath[64] = {0};

	memset(nodeNamePath,0,sizeof(nodeNamePath));		
	snprintf(nodeNamePath, sizeof(nodeNamePath), TR181_WIFISSID_ENTRY_NODE, wifiSSIDIndex + 1);
	cfg_obj_set_object_attr(nodeNamePath, TR181_LOWERLAYER_ATTR,0, lowerlayer);	

	return;
}

void setWifiSSIDUpperLayer( int wifiSSIDIndex, char* upperlayer){
	char nodeNamePath[64] = {0};

	memset(nodeNamePath,0,sizeof(nodeNamePath));		
	snprintf(nodeNamePath, sizeof(nodeNamePath), TR181_WIFISSID_ENTRY_NODE, wifiSSIDIndex + 1);
	cfg_obj_set_object_attr(nodeNamePath, TR181_UPLAYER_ATTR,0, upperlayer);	

	return;
}


static int wifiSsidSync2Wlan( int lanPortIdx, int ssidIndex){
	char wifissidPath[64] = {0};
	char wlanPath[64] = {0};	
	char tmp[256] = {0};
	int flag = 0;
	int enable = 0;
	
	if(ssidIndex < 0){
		tcdbg_printf("[%s:%d]ssidIndex=%d, error\n",__FUNCTION__,__LINE__, ssidIndex);
		return FAIL;
	}

	if(lanPortIdx < LAN_INDEX_WLAN_START || lanPortIdx > LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM){
		tcdbg_printf("[%s:%d]wlanIdx=%d, error\n",__FUNCTION__,__LINE__, lanPortIdx);
		return FAIL;
	}
	
	setWlanNodeByLanPortIdx(wlanPath, lanPortIdx);
	if(SUCCESS != checkNodePath(wlanPath)){
			return FAIL;
	}
	
	memset(wifissidPath, 0x00, sizeof(wifissidPath));
	snprintf(wifissidPath, sizeof(wifissidPath), TR181_WIFISSID_ENTRY_NODE, ssidIndex);

	/*SSID*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifissidPath, "SSID",0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanPath, "SSID",0, tmp); 
	}


	/*Enable*/
	enable = checkWifiSSIDEnable(ssidIndex);
	if(enable == 1){
		cfg_set_object_attr(wlanPath, "EnableSSID", "1");
	}else{
		cfg_set_object_attr(wlanPath, "EnableSSID", "0");
	}

	return 0;
}

static int cfg_type_WIFISsid_func_commit(char *path)
{
	char wifiSsidPath[MAXLEN_NODE_NAME] = {0};
	char wlanPath[MAXLEN_NODE_NAME] = {0};
	char bridgePortNode[MAXLEN_NODE_NAME] = {0};
	int etyIdx = -1;
	char tmp[256] = {0};
	char lowerlayer[256] = {0};
	char upperlayer[256] = {0};
	int wlanIdx = 0;
	int lanPortIdx = -1;
	int bndFlag = 0;
	int rdoIdx = -1;
	int bssidNum = -1;
	int bridgeIdx = -1;
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}

	if(etyIdx < 1){
		tcdbg_printf("[%s:%d]etyIdx=%d, error\n",__FUNCTION__,__LINE__, etyIdx);
		return FAIL;
	}
	memset(wifiSsidPath, 0x00, sizeof(wifiSsidPath));
	snprintf(wifiSsidPath, sizeof(wifiSsidPath), TR181_WIFISSID_ENTRY_NODE, etyIdx);

	/*get WIFISsid old lanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiSsidPath, LAN_INDEX_ATTR,0, tmp,sizeof(tmp)) >=0 && strlen(tmp) != 0){
		lanPortIdx = atoi(tmp);
	}

	/*get lowerlayer,TR181_WIFI_RDAIO:Device.WiFi.Radio.*/
	if(cfg_obj_get_object_attr(wifiSsidPath, TR181_LOWERLAYER_ATTR,0, lowerlayer,sizeof(lowerlayer)) >=0
		&& strstr(lowerlayer,TR181_WIFI_RDAIO) != NULL){							
		/*Device.WiFi.Radio. exist*/
		rdoIdx = getSuffixIntByInterface(lowerlayer) - 1;
		
		if(rdoIdx < 0){
			delWLanNodeByLanPortIdx(lanPortIdx);
			return 0;
		}
		
		bndFlag = getWifiBndByWifiRoIdx( rdoIdx);		
	}else{
		delWLanNodeByLanPortIdx(lanPortIdx);
		return 0;
	}
	
		/*check lanPortIdx for old ssid, check flag for new ssid*/
	if((bndFlag == 1 && (lanPortIdx < LAN_INDEX_WLAN_5G_START || lanPortIdx >= LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM))
		|| (bndFlag == 0 && (lanPortIdx >= LAN_INDEX_WLAN_5G_START || lanPortIdx < LAN_INDEX_WLAN_START))
		|| (lanPortIdx < 0))
	{
		/*flag ==0:5G, and	change to 2.4G, 
		flag ==1: 2.4G change to 5G, 
		lanPortIdx < 0: no Wlan or Wlan11ac Entry for wifiSSID
		need find a new Wlan or Wlan11ac Entry;
		*/

		/*del old wlan*/
		delWLanNodeByLanPortIdx(lanPortIdx);
				
		/*set new wlan*/
		lanPortIdx = getEmtryLanPortByWifiSsid(bndFlag);
		if((bndFlag == 0 && lanPortIdx >= LAN_INDEX_WLAN_5G_START && lanPortIdx < LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM)
		|| (bndFlag == 1 && lanPortIdx >= LAN_INDEX_WLAN_START && lanPortIdx < LAN_INDEX_WLAN_5G_START) 
		|| (lanPortIdx < 0))
		{
			
			tcdbg_printf("[%s:%d]lanPortIdx=%d, bndFlag =%d, error\n",__FUNCTION__,__LINE__,lanPortIdx, bndFlag);
			return -1;
		}

		bssidNum = getWlanBssidNum( bndFlag);

		if(bndFlag == 1){
			wlanIdx = lanPortIdx - LAN_INDEX_WLAN_5G_START + 1;
		}else{
			wlanIdx = lanPortIdx - LAN_INDEX_WLAN_START + 1;
		}
		if(wlanIdx >= bssidNum){
			bssidNum++;
		}
		setWlanBssidNum( bndFlag, bssidNum);

	
	}

	wifiSsidSync2Wlan(lanPortIdx,  etyIdx);
	
	/*set LanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, "%d", lanPortIdx);
	cfg_obj_set_object_attr(wifiSsidPath, LAN_INDEX_ATTR,0, tmp); 

	memset(upperlayer,0,sizeof(upperlayer));
	if(cfg_obj_get_object_attr(wifiSsidPath, TR181_UPLAYER_ATTR,0, upperlayer,sizeof(upperlayer)) > 0)
		bridgeIdx = getSuffixIntByInterface(upperlayer);
	
	if(bridgeIdx > 0)
	{	
		snprintf(bridgePortNode,sizeof(bridgePortNode), TR181_BRIDGEPORT_ENTRY_NODE, bridgeIdx);		
		cfg_obj_set_object_attr(bridgePortNode, LAN_INDEX_ATTR, 0,tmp);		
	}
	setWlanNodeByLanPortIdx(wlanPath,lanPortIdx);

	cfg_tr181_commit_object( wlanPath);
	return 0;
}


int cfg_type_WIFISsid_func_delete(char* path){
	int etyIdx = -1;
	char tmp[8] = {0};
	char wlanPath[128] = {0};
	int lanPortIdx = -1;
	int wlanMode = - 1;
	
	if(cfg_obj_get_object_attr(path, LAN_INDEX_ATTR,0, tmp,sizeof(tmp)) >=0 && strlen(tmp) != 0){
		lanPortIdx = atoi(tmp);
	}
	delWLanNodeByLanPortIdx(lanPortIdx);
	
	cfg_obj_delete_object(path);
	
	wlanMode = setWlanNodeByLanPortIdx(wlanPath,lanPortIdx);
	
	if(IS_WLAN11AC_MODE == wlanMode)
		cfg_tr181_commit_object(WLAN11AC_NODE);
	else if(IS_WLAN_MODE == wlanMode)
		cfg_tr181_commit_object( WLAN_NODE);
	
	return 0;
	
}


static cfg_node_ops_t cfg_type_WIFISsid_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_WIFISsid_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFISsid_func_commit 
}; 
static cfg_node_ops_t cfg_type_WIFISsid_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFISsid_func_commit 
}; 

static cfg_node_type_t cfg_type_WIFISsid_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | WIFI_SSID_NUM_MAX, 
	 .parent = &cfg_type_WIFISsid, 
	 .index = cfg_type_WIFISsid_index, 
	 .ops = &cfg_type_WIFISsid_entry_ops, 
}; 

static cfg_node_type_t* cfg_type_WIFISsid_child[] = { 
	 &cfg_type_WIFISsid_entry,
	 NULL 
}; 

cfg_node_type_t cfg_type_WIFISsid = { 
	 .name = "WIFISsid", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_WIFISsid_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_WIFISsid_child, 
	 .ops = &cfg_type_WIFISsid_ops, 
}; 
