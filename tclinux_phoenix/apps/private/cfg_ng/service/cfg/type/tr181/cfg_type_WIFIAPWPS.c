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

int
wlanNodeSync2WifiAPWpsByIdx( int lanPortIdx, int apIdx){
	char tmp[256] = {0};	
	char wlanNode[16] = {0};
	int wlanIdx = -1;
	char wlanEntryNode[64] = {0};
	char tr181nodePath[64] = {0};

	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]wlanIdx=%d, error\n",__FUNCTION__,__LINE__, wlanIdx);
		return FAIL;
	}

	if(apIdx < 0){
		tcdbg_printf("[%s:%d]ssidIndex=%d, error\n",__FUNCTION__,__LINE__, apIdx);
		return FAIL;
	}

	setWlanNodeByLanPortIdx(wlanEntryNode, lanPortIdx);


	memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
	snprintf(tr181nodePath,sizeof(tr181nodePath), TR181_WIFIAPWPS_ENTRY_NODE, apIdx);
	if(SUCCESS != checkNodePath(tr181nodePath)){
		return FAIL;
	}
	
	/* Enable*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryNode, "WPSConfMode",0, tmp,sizeof(tmp)) > 0){
		if (strcmp(tmp, "0")) {
			cfg_obj_set_object_attr(tr181nodePath, "Enable",0, "1");
		}
		else{
			cfg_obj_set_object_attr(tr181nodePath, "Enable",0, "0");
		}
	}

	/* ConfigMethodsEnabled*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wlanEntryNode, "WPSMode", 0, tmp,sizeof(tmp)) > 0){
		if (!strcmp(tmp, "0")) {
			/*pin code*/
			cfg_obj_set_object_attr(tr181nodePath, "CfgMthEn",0, "PIN" );
		}
		else if(!strcmp(tmp, "1")) {
			/*push button*/
			cfg_obj_set_object_attr(tr181nodePath, "CfgMthEn",0, "PushButton" );
		}
	}

    return SUCCESS;
}

int createWifiAPWPSEntry( int lanPort){	
	struct tr181_interface_s interface;
	int index = -1;
		
	if(lanPort < LAN_INDEX_WLAN_START || lanPort > LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM){
		tcdbg_printf("[%s]lanPort=%d not wlan index\n",__FUNCTION__,lanPort);
		return -1;
	}

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName,TR181_WIFIAPWPS_NODE, sizeof(interface.nodeName)-1);
	interface.num = WIFI_AP_NUM_MAX;
	interface.pvcIndex = -1;
	interface.lanIndex = lanPort;
	interface.active = 1;
	index = createTR181InterfaceEntry( interface);
	if(index < 0){
		return -1;
	}

	wlanNodeSync2WifiAPWpsByIdx(lanPort, index + 1);
	return index;
}


int cfg_type_WIFIAPWPS_func_commit(char *path)
{
	int etyIdx = -1;
	char wifiapWPSPath[MAXLEN_NODE_NAME] = {0};
	char nodePath[MAXLEN_NODE_NAME] = {0};	
	char infoPath[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	char wlanEntryPath[MAXLEN_NODE_NAME] = {0};
	int lanPortIdx = -1;
	int wlanIdx = 0;
	int commitType = 0;
	char node[MAXLEN_NODE_NAME] = {0};
	int wlanMode = -1;

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}
	
	lanPortIdx = findLanPortIdxbyApIdx( etyIdx);
	if(-1 == lanPortIdx){
		return 0;
	}
	
	memset(wlanEntryPath, 0x00, sizeof(wlanEntryPath));
	wlanMode = setWlanNodeByLanPortIdx(wlanEntryPath ,lanPortIdx);
	
	memset(nodePath, 0, sizeof(nodePath));
	if(IS_WLAN_MODE == wlanMode){
		snprintf(nodePath, sizeof(nodePath), WLAN_NODE);
		snprintf(infoPath, sizeof(infoPath), INFO_WLAN_NODE);
	}else if(IS_WLAN11AC_MODE == wlanMode){
		snprintf(nodePath, sizeof(nodePath), WLAN11AC_NODE);
		snprintf(infoPath, sizeof(infoPath), INFO_WLAN11AC_NODE);
	}

	memset(wifiapWPSPath, 0x00, sizeof(wifiapWPSPath));
	snprintf(wifiapWPSPath, MAX_NODE_NAME, TR181_WIFIAPWPS_ENTRY_NODE, etyIdx);

	/* Enable*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapWPSPath, "Enable",0, tmp,sizeof(tmp)) >0){
		if (!strcmp(tmp, "0")) {
			cfg_obj_set_object_attr( wlanEntryPath, "WPSConfMode",0, "0" );
			commitType = 0;
		}
		else if (!strcmp(tmp, "1")) {
			cfg_obj_set_object_attr( wlanEntryPath, "WPSConfMode",0, "7" );
			cfg_obj_set_object_attr( wlanEntryPath, "WPSMode",0, "1" );
			cfg_obj_set_object_attr( infoPath, "WPSActiveStatus",0, "1" );
			commitType = 0;
		}
	}

	/* ConfigMethodsEnabled*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapWPSPath, "CfgMthEn",0, tmp,sizeof(tmp)) >0){
		if (!strcmp(tmp, "PIN")) {
			/*pin code*/
			cfg_obj_set_object_attr( wlanEntryPath, "WPSMode",0, "0" );
			commitType = 1;
		}
		else if(!strcmp(tmp, "PushButton")) {
			/*push button*/
			cfg_obj_set_object_attr( wlanEntryPath, "WPSMode",0, "1" );
			commitType = 1;
		}
	}

	
	/*set LanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, "%d", lanPortIdx);
	cfg_obj_set_object_attr(wlanEntryPath, LAN_INDEX_ATTR,0, tmp);

	if(1 == commitType){
	   cfg_tr181_commit_object(nodePath);
	}else if(0 == commitType){
		cfg_tr181_commit_object(wlanEntryPath);
	}
	
	return 0;
}


static char* cfg_type_WIFIAPWPS_index[] = { 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_WIFIAPWPS_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAPWPS_func_commit 
}; 

static cfg_node_ops_t cfg_type_WIFIAPWPS_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_ops_t cfg_type_WIFIAPWPS_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAPWPS_func_commit 
}; 

static cfg_node_type_t cfg_type_WIFIAPWPS_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | WIFI_APWPS_NUM_MAX, 
	 .parent = &cfg_type_WIFIAPWPS, 
	 .index = cfg_type_WIFIAPWPS_index, 
	 .ops = &cfg_type_WIFIAPWPS_entry_ops, 
}; 

static cfg_node_type_t cfg_type_WIFIAPWPS_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_WIFIAPWPS, 
	 .index = cfg_type_WIFIAPWPS_index, 
	 .ops = &cfg_type_WIFIAPWPS_common_ops, 
}; 

static cfg_node_type_t* cfg_type_WIFIAPWPS_child[] = { 
	 &cfg_type_WIFIAPWPS_entry,
	 &cfg_type_WIFIAPWPS_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_WIFIAPWPS = { 
	 .name = "WIFIAPWPS", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_WIFIAPWPS_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_WIFIAPWPS_child, 
	 .ops = &cfg_type_WIFIAPWPS_ops, 
}; 
