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

int wlanNodeSync2WifiAPSecByIdx(int lanPortIdx,int index){
	char tmp[256] = {0};	
	char wlanNode[16] = {0};
	int wlanIdx = -1;
	char wlanNodePath[64] = {0};
	char nodeNamePath[64] = {0};
	char tr181nodePath[64] = {0};
	char wepkey[64] = {0};

	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]wlanIdx=%d, error\n",__FUNCTION__,__LINE__, lanPortIdx);
		return FAIL;
	}

	if(index < 0){
		tcdbg_printf("[%s:%d]ssidIndex=%d, error\n",__FUNCTION__,__LINE__, index);
		return FAIL;
	}

	setWlanNodeByLanPortIdx(nodeNamePath, lanPortIdx);

	memset(tr181nodePath, 0x00, sizeof(tr181nodePath));
	snprintf(tr181nodePath,sizeof(tr181nodePath), TR181_WIFIAPSEC_ENTRY_NODE, index);
	
	if(SUCCESS != checkNodePath(tr181nodePath)){
		return FAIL;
	}
	/* ModeEnabled*/

	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "AuthMode",0, tmp,sizeof(tmp)) > 0){		
		if(0 == strcmp(tmp,"OPEN"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "None");
		else if(0 == strcmp(tmp,"WEP-64Bits"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WEP-64");
		else if(0 == strcmp(tmp,"WEP-128Bits"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WEP-128");
		else if(0 == strcmp(tmp,"WPAPSK"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA-Personal");
		else if(0 == strcmp(tmp,"WPA2PSK"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA2-Personal");
		else if(0 == strcmp(tmp,"WPAPSKWPA2PSK"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA-WPA2-Personal");
		else if(0 == strcmp(tmp,"WPA"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA-Enterprise");
		else if(0 == strcmp(tmp,"WPA2"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA2-Enterprise");
		else if(0 == strcmp(tmp,"WPA1WPA2"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "WPA-WPA2-Enterprise");
		else if(0 == strcmp(tmp,"Radius-WEP64"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "Radius-WEP64");
		else if(0 == strcmp(tmp,"Radius-WEP128"))
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "Radius-WEP128");
		else
			cfg_obj_set_object_attr(tr181nodePath, "ModeEnabled",0, "");

		if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK")){
			memset(tmp, 0x00, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeNamePath, "EncrypType",0, tmp, sizeof(tmp)) < 0 || !strlen(tmp)){
				cfg_obj_set_object_attr(tr181nodePath, "EncrypType",0, "AES");
			}else{
				cfg_obj_set_object_attr(tr181nodePath, "EncrypType",0, tmp);
			}
		}
	}

	/* WEPKey*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "DefaultKeyID",0, tmp,sizeof(tmp)) >0){
		memset(wepkey, 0x00, sizeof(wepkey));
		snprintf(wepkey,sizeof(wepkey),"Key%sStr",tmp);
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeNamePath, wepkey,0, tmp,sizeof(tmp)) >0){				
			cfg_obj_set_object_attr(tr181nodePath, "WEPKey",0, tmp);
		}
	}
	

	/* PreSharedKey*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath,  "WPAPSK",0, tmp,sizeof(tmp)) >0){ 			
		cfg_obj_set_object_attr(tr181nodePath, "PreSharedKey",0, tmp);
	}

	/* RekeyingInterval*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath,  "RekeyInterval", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "RekeyingInterval",0, tmp);
	}

	/* RadiusServerIP*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath,  "RADIUS_Server", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "RadiusSvrIP",0, tmp);
	}

	/* SecondaryRadiusServerIP*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "BAK_RADIUS_Server", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "2ndRadiusSvrIP",0, tmp);
	}

	/* RadiusServerPort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "RADIUS_Port", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "RadiusSvrPort",0, tmp);
	}

	/* SecondaryRadiusServerPort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "BAK_RADIUS_Port", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "2ndRadiusSvrPort",0, tmp);
	}
	
	/* RadiusSecret*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "RADIUS_Key", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "RadiusSecret",0, tmp);
	}

	/* SecondaryRadiusSecret*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeNamePath, "BAK_RADIUS_Key", 0, tmp,sizeof(tmp)) >0){
		cfg_obj_set_object_attr(tr181nodePath, "2ndRadiusSecret",0, tmp);
	}


    return SUCCESS;
}
int createWifiAPSecEntry(int lanPort){	
	struct tr181_interface_s interface;
	int index = -1;
		
	if(lanPort < LAN_INDEX_WLAN_START || lanPort > LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM){
		tcdbg_printf("[%s]lanPort=%d not wlan index\n",__FUNCTION__,lanPort);
		return -1;
	}

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName,TR181_WIFIAPSEC_NODE, sizeof(interface.nodeName)-1);
	interface.num = WIFI_AP_NUM_MAX;
	interface.pvcIndex = -1;
	interface.lanIndex = lanPort;
	interface.active = 1;
	index = createTR181InterfaceEntry(interface);
	if(index < 0){
		return -1;
	}

	wlanNodeSync2WifiAPSecByIdx(lanPort, index + 1);
	return index;
}


static int cfg_type_WIFIAPSec_func_commit(char* path)
{
	int etyIdx = -1;
	char wifiapsecPath[64] = {0};
	char wlanEntryPath[64] = {0};
	char tmp[256] = {0};
	int lanPortIdx = -1;
	char wepkey[32] = {0};
	
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

	memset(wlanEntryPath, 0, sizeof(wlanEntryPath));
	setWlanNodeByLanPortIdx(wlanEntryPath,lanPortIdx);

	snprintf(wifiapsecPath, sizeof(wifiapsecPath), TR181_WIFIAPSEC_ENTRY_NODE, etyIdx);

	/* ModeEnabled*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "ModeEnabled", 0,tmp, sizeof(tmp)) > 0){
		if(0 == strcmp(tmp,"None"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "OPEN");
		else if(0 == strcmp(tmp,"WEP-64"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WEP-64Bits");
		else if(0 == strcmp(tmp,"WEP-128"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WEP-128Bits");
		else if(0 == strcmp(tmp,"WPA-Personal"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WPAPSK");
		else if(0 == strcmp(tmp,"WPA2-Personal"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WPA2PSK");
		else if(0 == strcmp(tmp,"WPA-WPA2-Personal"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WPAPSKWPA2PSK");
		else if(0 == strcmp(tmp,"WPA-Enterprise"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0,"WPA");
		else if(0 == strcmp(tmp,"WPA2-Enterprise"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WPA2");
		else if(0 == strcmp(tmp,"WPA-WPA2-Enterprise"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "WPA1WPA2");
		else if(0 == strcmp(tmp,"Radius-WEP64"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "Radius-WEP64");
		else if(0 == strcmp(tmp,"Radius-WEP128"))
			cfg_obj_set_object_attr(wlanEntryPath, "AuthMode",0, "Radius-WEP128");


		if(!strcmp(tmp, "WPA-Personal") || !strcmp(tmp, "WPA2-Personal") || !strcmp(tmp, "WPA-WPA2-Personal")){
			memset(tmp, 0x00, sizeof(tmp));
			if(cfg_obj_get_object_attr(wifiapsecPath, "EncrypType",0, tmp, sizeof(tmp)) < 0 || !strlen(tmp)){
				cfg_obj_set_object_attr(wlanEntryPath, "EncrypType",0, "AES");
			}else{
				cfg_obj_set_object_attr(wlanEntryPath, "EncrypType",0, tmp);
			}
		}
	}

	/* WEPKey*/
	memset(tmp, 0x00, sizeof(tmp));
	cfg_obj_get_object_attr(wlanEntryPath, "DefaultKeyID",0, tmp,sizeof(tmp));
	if(tmp[0] == '\0')
	{
		snprintf(tmp,sizeof(tmp),"1");
	}
	memset(wepkey, 0x00, sizeof(wepkey));
	snprintf(wepkey,sizeof(wepkey),"Key%sStr",tmp);
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "WEPKey",0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, wepkey,0, tmp);
	}

	/* PreSharedKey*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "PreSharedKey",0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "WPAPSK",0, tmp);
	}

	
	/* RekeyingInterval*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "RekeyingInterval", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "RekeyInterval",0, tmp);
	}

	/* RadiusServerIP*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "RadiusSvrIP", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "RADIUS_Server",0, tmp);
	}

	/* SecondaryRadiusServerIP*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "2ndRadiusSvrIP", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "BAK_RADIUS_Server",0,tmp);
	}

	/* RadiusServerPort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "RadiusSvrPort", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "RADIUS_Port",0,tmp);
	}

	/* SecondaryRadiusServerPort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "2ndRadiusSvrPort", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "BAK_RADIUS_Port",0, tmp);
	}

	/* RadiusSecret*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "RadiusSecret", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "RADIUS_Key",0, tmp);

	}

	/* SecondaryRadiusSecret*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(wifiapsecPath, "2ndRadiusSecret", 0, tmp, sizeof(tmp)) > 0){
		cfg_obj_set_object_attr(wlanEntryPath, "BAK_RADIUS_Key",0, tmp);
	}

	/*set LanIndex*/
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, MAX_NODE_NAME, "%d", lanPortIdx);
	cfg_obj_set_object_attr(wlanEntryPath, LAN_INDEX_ATTR,0, tmp);
	cfg_obj_set_object_attr(wifiapsecPath, LAN_INDEX_ATTR,0, tmp);
	
	cfg_tr181_commit_object(wlanEntryPath);
	return 0;
}

static cfg_node_ops_t cfg_type_WIFIAPSec_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAPSec_func_commit 
}; 

static cfg_node_ops_t cfg_type_WIFIAPSec_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_ops_t cfg_type_WIFIAPSec_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_WIFIAPSec_func_commit 
}; 

static cfg_node_type_t cfg_type_WIFIAPSec_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_WIFIAPSec, 
	 .ops = &cfg_type_WIFIAPSec_entry_ops, 
}; 

static cfg_node_type_t cfg_type_WIFIAPSec_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_WIFIAPSec, 
	 .ops = &cfg_type_WIFIAPSec_common_ops, 
}; 

static cfg_node_type_t* cfg_type_WIFIAPSec_child[] = { 
	 &cfg_type_WIFIAPSec_entry,
	 &cfg_type_WIFIAPSec_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_WIFIAPSec = { 
	 .name = "WIFIAPSec", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_WIFIAPSec_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_WIFIAPSec_child, 
	 .ops = &cfg_type_WIFIAPSec_ops, 
}; 
