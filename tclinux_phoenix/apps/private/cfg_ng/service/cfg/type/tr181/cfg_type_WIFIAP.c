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
#include "utility.h"
#include "cfg_tr181_global.h" 
#include "cfg_types.h" 
#include "cfg_tr181_utility.h" 

static char* cfg_type_WIFIAP_index[] = { 
	 "wlan_ap_id", 
	 NULL 
};
int createWifiAPEntry( int lanPort,char *wlanCommonPath){		
	struct tr181_interface_s interface;
	int index = -1;
	char nodeNamePath[64] = {0};
	char tmp[16] = {0};
	int activeFlag = 1;
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanCommonPath, "APOn",0, tmp,sizeof(tmp)) > 0){
		activeFlag = atoi(tmp);
	}

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_WIFIAP_NODE, sizeof(interface.nodeName)-1);
	interface.num = WIFI_AP_NUM_MAX;
	interface.pvcIndex = -1;
	interface.lanIndex = lanPort;
	interface.active = activeFlag;
	
	index = createTR181InterfaceEntry( interface);
	if(index < 0){
		return -1;
	}

	wlanNodeSync2WifiAPByIdx( lanPort, index + 1);
	return index;
}
int
wlanNodeSync2WifiAPByIdx( int lanPortIdx, int apIdx){
	char wlanEntryNodePath[64] = {0};
	char tr181nodePath[64] = {0};
	char tmp[256] = {0};	
	char wlanNodePath[64] = {0};
	int mode = -1;

	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]wlanIdx=%d, error\n",__FUNCTION__,__LINE__, lanPortIdx);
		return FAIL;
	}

	if(apIdx < 0){
		tcdbg_printf("[%s:%d]ssidIndex=%d, error\n",__FUNCTION__,__LINE__, apIdx);
		return FAIL;
	}

	mode = setWlanNodeByLanPortIdx(wlanEntryNodePath, lanPortIdx);
	
	memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
	snprintf(tr181nodePath, sizeof(tr181nodePath), TR181_WIFIAP_ENTRY_NODE, apIdx);
	
	if(SUCCESS != checkNodePath(tr181nodePath)){
		return FAIL;
	}

	/* SSIDAdvertisementEnabled*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryNodePath, "HideSSID",0, tmp,sizeof(tmp)) > 0){
		if(!strcmp(tmp, "1")){
			cfg_obj_set_object_attr(tr181nodePath, "SSIDBroadcast", 0,"0");
		}
		else if(!strcmp(tmp, "0")){
			cfg_obj_set_object_attr(tr181nodePath, "SSIDBroadcast", 0,"1");
		}
	}

	/* EnableSSID*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryNodePath, "EnableSSID",0, tmp,sizeof(tmp)) > 0)
	{
		if(!strcmp(tmp, "1"))
		{
			cfg_obj_set_object_attr(tr181nodePath, "Enable", 0,"1");
		}
		else if(!strcmp(tmp, "0"))
		{
			cfg_obj_set_object_attr(tr181nodePath, "Enable", 0,"0");
		}
	}

	/*WMMEnable*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryNodePath, "WMM",0, tmp,sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(tr181nodePath, "WMMEnable",0, tmp);
	}
	
	/*WMMCapability*/
	memset(wlanNodePath, 0x00, sizeof(wlanNodePath));
	if(IS_WLAN11AC_MODE == mode)
		snprintf(wlanNodePath, sizeof(wlanNodePath), WLAN11AC_COMMON_NODE);
	else if(IS_WLAN_MODE == mode)
		snprintf(wlanNodePath, sizeof(wlanNodePath), WLAN_COMMON_NODE);
		
	memset(tmp, 0x00, sizeof(tmp));
	if((cfg_obj_get_object_attr(wlanNodePath, "11nMode", 0,tmp,sizeof(tmp)) > 0) && !(strcmp(tmp, "1"))){
		cfg_obj_set_object_attr(tr181nodePath, "WMMCapability",0, "0");
	}
	else{
		cfg_obj_set_object_attr(tr181nodePath, "WMMCapability",0, "1");
	}

	/* MaxAssociatedDevices*/	
	memset(tmp, 0x00, sizeof(tmp));
	if((cfg_obj_get_object_attr(WEBCUSTOM_ENTRY_NODE, "isMaxStaNumSupported",0, tmp,sizeof(tmp)) > 0) && (!strcmp(tmp, "Yes"))){
		memset(tmp, 0x00, sizeof(tmp));		
		if(cfg_obj_get_object_attr(wlanEntryNodePath, "MaxStaNum",0, tmp,sizeof(tmp)) > 0){
			cfg_obj_set_object_attr(tr181nodePath, "MaxAssoNum",0, tmp);
		}
		else
			cfg_obj_set_object_attr(tr181nodePath, "MaxAssoNum",0, "0");
	}
	else
		cfg_obj_set_object_attr(tr181nodePath, "MaxAssoNum",0, "0");


    return SUCCESS;
}

static int cfg_type_WIFIAP_func_commit(char* path)
{
	int etyIdx = -1;
	char wifiapPath[64] = {0};
	char wlanPath[64] = {0};	
	char wlanCommonPath[64] = {0};	
	char wlanEntryPath[64] = {0};
	char tmp[256] = {0};
	int wlanMode = -1;
	int commitType = 0;
	int lanPortIdx = -1;
	struct cfg_node_type* node = NULL;
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}
	
	lanPortIdx = findLanPortIdxbyApIdx(etyIdx);
	if(-1 == lanPortIdx){
		return 0;
	}
	memset(wlanEntryPath, 0x00, sizeof(wlanEntryPath));
	wlanMode = setWlanNodeByLanPortIdx(wlanEntryPath, lanPortIdx);

	memset(wlanCommonPath, 0x00, sizeof(wlanCommonPath));
	if(IS_WLAN11AC_MODE == wlanMode){
		snprintf(wlanCommonPath, sizeof(wlanCommonPath), WLAN11AC_COMMON_NODE);
		snprintf(wlanPath, sizeof(wlanPath), WLAN11AC_NODE);
	}else if(IS_WLAN_MODE == wlanMode){
		snprintf(wlanCommonPath, sizeof(wlanCommonPath), WLAN_COMMON_NODE);
		snprintf(wlanPath, sizeof(wlanPath), WLAN_NODE);
	}

	snprintf(wifiapPath, sizeof(wifiapPath), TR181_WIFIAP_ENTRY_NODE, etyIdx);
	
	/* Enable*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapPath,ENABLE_ATTR, 0,tmp,sizeof(tmp)) > 0)
	{
	
		if(!strcmp(tmp, "1")){
			cfg_obj_set_object_attr(wlanEntryPath,"EnableSSID",0,"1");
		}
		else if(!strcmp(tmp, "0")){
			cfg_obj_set_object_attr(wlanEntryPath,"EnableSSID",0,"0");
		}
	}

	/* SSIDAdvertisementEnabled*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapPath, "SSIDBroadcast",0, tmp,sizeof(tmp)) >0){
		if(!strcmp(tmp, "0"))
			cfg_obj_set_object_attr(wlanEntryPath, "HideSSID",0, "1");
		else if(!strcmp(tmp, "1"))
			cfg_obj_set_object_attr(wlanEntryPath, "HideSSID",0, "0");
	}

	/* MaxAssociatedDevices*/
	memset(tmp, 0x00, sizeof(tmp));
	node = cfg_type_query(WEBCUSTOM_ENTRY_NODE);
	if(NULL != node)
	{		
		cfg_obj_get_object_attr(WEBCUSTOM_ENTRY_NODE, "isMaxStaNumSupported",0, tmp,sizeof(tmp));
		if(!strcmp(tmp, "Yes")){
			memset(tmp, 0x00, sizeof(tmp)); 
			if(cfg_obj_get_object_attr(wifiapPath, "MaxAssoNum",0, tmp,sizeof(tmp)) >0 && strlen(tmp)){
				cfg_obj_set_object_attr(wlanEntryPath, "MaxStaNum",0, tmp);
			}
			
		}
	}
	
	/*WMMEnable*/
	memset(tmp, 0x00, sizeof(tmp));
	if((cfg_obj_get_object_attr(wlanCommonPath, "11nMode",0, tmp,sizeof(tmp)) > 0) && !(strcmp(tmp, "0"))){
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(wifiapPath, "WMMEnable",0, tmp,sizeof(tmp)) > 0 && strlen(tmp))
			cfg_obj_set_object_attr(wlanEntryPath, "WMM",0, tmp);
	}

	/*set LanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, "%d", lanPortIdx);
	cfg_obj_set_object_attr(wlanEntryPath,LAN_INDEX_ATTR,0, tmp); 

	cfg_tr181_commit_object(wlanPath);

	return 0;
}
static int delNodebyIdx(char *path, int idx ,int lanport){
	char nodePath[64] = {0};
	char wifiapwpsPath[64] = {0};
	char tmp[128] = {0};
	
	memset(nodePath,0,sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), path, idx);
	if(cfg_obj_get_object_attr(path, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp)) > 0)
	{
		if(atoi(tmp) == lanport){
			return cfg_obj_delete_object(nodePath);
		}
	}
	return 0;
}
static int del_apsec_and_wps_Node(char* path){
	int etyIdx = -1;
	int lanport = -1;
	int i = 0;
	int del_wifiapsec_done = 0;
	int del_wifiapwps_done = 0;
		
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		return 0;
	}
	lanport = findLanPortIdxbyApIdx(etyIdx);
	if(lanport < 0)
	{
		return 0;
	}
	for(i = 0; i < WIFI_NUM_MAX; i++){
		if(!del_wifiapsec_done){
			delNodebyIdx(TR181_WIFIAPSEC_ENTRY_NODE, i , lanport);
			del_wifiapsec_done = 1;
		}
		if(!del_wifiapwps_done){
			delNodebyIdx(TR181_WIFIAPWPS_ENTRY_NODE, i , lanport);
			del_wifiapwps_done = 1;
		}
	}
	
	return 0;
}

static int cfg_type_WIFIAP_func_delete(char* path)
{
	del_apsec_and_wps_Node(path);
	return cfg_obj_delete_object(path);
}

static cfg_node_ops_t cfg_type_WIFIAP_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_WIFIAP_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAP_func_commit 
}; 
static cfg_node_ops_t cfg_type_WIFIAP_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_ops_t cfg_type_WIFIAP_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAP_func_commit 
}; 

static cfg_node_type_t cfg_type_WIFIAP_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_WIFIAP, 
	 .index = cfg_type_WIFIAP_index, 
	 .ops = &cfg_type_WIFIAP_entry_ops, 
}; 

static cfg_node_type_t cfg_type_WIFIAP_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_WIFIAP, 
	 .index = cfg_type_WIFIAP_index, 
	 .ops = &cfg_type_WIFIAP_common_ops, 
}; 


static cfg_node_type_t* cfg_type_WIFIAP_child[] = { 
	 &cfg_type_WIFIAP_entry,
	 &cfg_type_WIFIAP_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_WIFIAP = { 
	 .name = "WIFIAP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_WIFIAP_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_WIFIAP_child, 
	 .ops = &cfg_type_WIFIAP_ops, 
}; 
