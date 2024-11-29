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
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h" 

static char* cfg_type_bridgeport_index[] = { 
	 "bridgePort_Idx", 
	 NULL 
}; 


int setBridgePortLowerLayer(int bridgePortIdx, char* lowerlayer){	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char attrNode[32] = {0};
	char tmp[512] = {0};
	int i = 0;
	int fisrtEmptyAttr = -1;
	
	if(bridgePortIdx == 0){
		/*br0, more than one lowerlayer*/
		for(i = 0; i< BRIDGE_PORT_NUM; i++){
			memset(attrNode,0,sizeof(attrNode));
			memset(tmp,0,sizeof(tmp));
			snprintf(attrNode,sizeof(attrNode),"LowerLayers%d",i);
			if(cfg_query_object(TR181_BRIDGEPORT_ENTRY_0_NODE,NULL,NULL) < 0){
				cfg_create_object(TR181_BRIDGEPORT_ENTRY_0_NODE);
			}
			cfg_obj_get_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, attrNode, 0,tmp,sizeof(tmp)); 
			memset(attrNode,0,sizeof(attrNode));
			if(strcmp(tmp,lowerlayer) == 0){
				tcdbg_printf("[%s:%d]lowerlayer already exist,i=%d\n",__FUNCTION__,__LINE__,i);
				return 0;
			}
			
			if(strlen(tmp) == 0 && fisrtEmptyAttr < 0){
				fisrtEmptyAttr = i;
			}			
		}

		if(updateInterfaceNodeDbg()){
			tcdbg_printf("[%s:%d]lowerlayer =%s,fisrtEmptyAttr=%d,i=%d\n",__FUNCTION__,__LINE__,lowerlayer,fisrtEmptyAttr,i);	
		}
		if(fisrtEmptyAttr < 0){
			return -1;
		}else{
			memset(attrNode,0,sizeof(attrNode));
			snprintf(attrNode,sizeof(attrNode),"LowerLayers%d",fisrtEmptyAttr);
			cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, attrNode, 0,lowerlayer); 
		}
		
		return 0;
	}
	
	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, bridgePortIdx + 1);
	cfg_obj_set_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0,lowerlayer);	
	
	return 0;
}

int setBridgePortUpperLayer(int bridgePortIdx, char* upperlayer){	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	
	if(bridgePortIdx == 0){
		/*br0 upperlayer is etherlink*/
		if(cfg_query_object(TR181_BRIDGEPORT_ENTRY_0_NODE,NULL,NULL) < 0){
			cfg_create_object(TR181_BRIDGEPORT_ENTRY_0_NODE);
		}
		
		cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, TR181_UPLAYER_ATTR, 0,upperlayer); 
		return 0;
	}

	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, bridgePortIdx + 1);
	cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0,upperlayer);	
	
	return 0;
}


int  createBridgePortEntry0(void)
{
	char tmp[20] = {0};
	int num = 0;

	if (cfg_obj_query_object(TR181_BRIDGEPORT_ENTRY_0_NODE,NULL,NULL) < 0)
	{
		cfg_obj_create_object(TR181_BRIDGEPORT_ENTRY_0_NODE);
	}

	/*default is Enable*/
	cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, TR181_ENABLE_ATTR, 0, "1");
	cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, "ManagementPort", 0, "1");
	cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, PVC_INDEX_ATTR, 0, "-1");
	cfg_obj_set_object_attr(TR181_BRIDGEPORT_ENTRY_0_NODE, LAN_INDEX_ATTR, 0, "0");

	/*set num*/ 	
	memset(tmp,0,sizeof(tmp));		
	cfg_obj_get_object_attr(TR181_BRIDGEPORT_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
	num = atoi(tmp);
	
	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp, sizeof(tmp), "%d", num+1);
	cfg_obj_set_object_attr(TR181_BRIDGEPORT_COMMON_NODE, TR181_NUM_ATTR, 0, tmp);
	
	return 0;
}


int setLanInterfaceDownFunc(int lanIndex, int wifiIndex)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char cmd[64] = {0};
	int num = 0;
	char node[MAXLEN_NODE_NAME] = {0};


	if(lanIndex <= 0) {
		return -1;
	}

	num = getLanPortNum();	
	
	if(lanIndex >= 1 && lanIndex <= num) 
	{
		snprintf(cmd,sizeof(cmd),"ifconfig eth0.%d down", lanIndex);				
		system(cmd);
	}
	else if(lanIndex > num && lanIndex <= num + MAX_BSSID_NUM){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),WLAN_ENTRY_N_NODE, lanIndex - num);
		cfg_set_object_attr(nodeName, "EnableSSID","0");
		cfg_tr181_commit_object(nodeName);
	}
	else if(lanIndex > num + MAX_BSSID_NUM  &&  lanIndex <= num + MAX_BSSID_NUM*2){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),WLAN11AC_ENTRY_N_NODE, lanIndex - num - MAX_BSSID_NUM);
		cfg_set_object_attr(nodeName, "EnableSSID","0");
		cfg_tr181_commit_object(nodeName);
	}

	if(wifiIndex > 0){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),TR181_WIFISSID_ENTRY_NODE, wifiIndex);
		cfg_set_object_attr(nodeName, "Enable","0");

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),TR181_WIFIAP_ENTRY_NODE, wifiIndex);
		cfg_set_object_attr(nodeName, "Enable","0");
	}
	

	return 0;
}


int setLanInterfaceUpFunc(int lanIndex, int wifiIndex)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char cmd[64] = {0};
	int num = 0;
	char node[MAXLEN_NODE_NAME] = {0};

	if(lanIndex <= 0) {
		return -1;
	}
	
	num = getLanPortNum();	

	if(lanIndex >= 1 && lanIndex <= num) {
		snprintf(cmd, sizeof(cmd),"ifconfig eth0.%d up", lanIndex);				
		system(cmd);
	}
	else if(lanIndex > num && lanIndex <= num + MAX_BSSID_NUM){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),WLAN_ENTRY_N_NODE, lanIndex - num);
		cfg_set_object_attr(nodeName, "EnableSSID","1");
		cfg_tr181_commit_object(nodeName);
	}
	else if(lanIndex > num + MAX_BSSID_NUM  &&  lanIndex <= num + MAX_BSSID_NUM*2){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),WLAN11AC_ENTRY_N_NODE, lanIndex - num - MAX_BSSID_NUM);
		cfg_set_object_attr(nodeName, "EnableSSID","1");
		cfg_tr181_commit_object(nodeName);
	}

	if(wifiIndex > 0){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),TR181_WIFISSID_ENTRY_NODE, wifiIndex);
		cfg_set_object_attr(nodeName, "Enable","1");

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),TR181_WIFIAP_ENTRY_NODE, wifiIndex);
		cfg_set_object_attr(nodeName, "Enable","1");
	}

	return 0;
}


int cfg_type_bridgeport_func_commit(char *path){
	int etyIdx = -1;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wifinode[MAXLEN_NODE_NAME] = {0};
	int tmp[256] = {0};
	char lanIndexValue[4]= {0};
	int ret = -1;
	int lanIdx = -1;
	int index = -1;
	char setLanIndex[4] = {0};
	char LowerLayersValue[64] = {0};
	char upperLayersValue[64] = {0};
	int wifiIndex = -1;
	int retv = -1;

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "bridgePort_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}
	if(etyIdx <= 1){
		/*br0 Port or etyIdx < 1: do nothing*/
		return 0;
	}else{/*other bridgePort*/	
		memset(tr181node, 0, sizeof(tr181node));
		snprintf(tr181node,sizeof(tr181node), TR181_BRIDGEPORT_ENTRY_NODE, etyIdx);					
		/*check lowerlayer*/
		memset(lanIndexValue, 0, sizeof(lanIndexValue));	
		retv = cfg_obj_get_object_attr(tr181node, LAN_INDEX_ATTR, 0, lanIndexValue, sizeof(lanIndexValue));
		ret = checkBridgePortEnable(etyIdx);

		if(ret < 0){
			/*error or lowerlayer not set, clear lanIndex, ifconfig down*/
			index = atoi(lanIndexValue);
			setLanInterfaceDownFunc(index, wifiIndex);
			
			clearLanIdxOfTR181Interface(index);
			return 0;
		}
		
		lanIdx = getLanIdxOfLowerLayer(TR181_BRIDGE_PORT_NODE, etyIdx);
		
		cfg_obj_get_object_attr(tr181node, TR181_LOWERLAYER_ATTR, 0,LowerLayersValue,sizeof(LowerLayersValue));
		if(strstr(LowerLayersValue, TR181_WIFI_SSID_IF) != NULL)
		{
			wifiIndex = atoi(LowerLayersValue + strlen(TR181_WIFI_SSID_IF));
		}
		
		if(retv < 0 || lanIndexValue[0] == '\0')
		{
			setBridgePortUpperLayer(etyIdx-1,TR181_BRIDGE_PORT_BR0);
			memset(upperLayersValue,0,sizeof(upperLayersValue));
			snprintf(upperLayersValue, sizeof(upperLayersValue),TR181_BRIDGE_PORT"%d", etyIdx);	
			cfg_obj_get_object_attr(tr181node, TR181_LOWERLAYER_ATTR, 0,LowerLayersValue,sizeof(LowerLayersValue));
			if(wifiIndex > 0)
			{
				setWifiSSIDUpperLayer(wifiIndex-1,upperLayersValue);			
			}
			
			if(lanIdx > 0)
			{
				/*all interface is exist, set lanIndx */
				snprintf(setLanIndex, sizeof(setLanIndex),"%d", lanIdx);
				cfg_obj_set_object_attr(tr181node, LAN_INDEX_ATTR, 0,setLanIndex);
				if(wifiIndex > 0 ){
					memset(wifinode, 0, sizeof(wifinode));
					snprintf(wifinode, sizeof(wifinode),TR181_WIFISSID_ENTRY_NODE, wifiIndex);			
					cfg_obj_set_object_attr(wifinode, LAN_INDEX_ATTR, 0,setLanIndex);
				}
			}
			else {
				return 0;
			}
		}

		if(ret == 0){
			/*disable*/
			setLanInterfaceDownFunc(lanIdx, wifiIndex);
		}else{
			/*enable*/
			setLanInterfaceUpFunc(lanIdx, wifiIndex);
		}
	}
	
	return 0;
}


static cfg_node_ops_t cfg_type_bridgeport_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bridgeport_func_commit 
}; 


static cfg_node_type_t cfg_type_bridgeport_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | BRIDGE_PORT_NUM, 
	 .parent = &cfg_type_bridgeport, 
	 .index = cfg_type_bridgeport_index, 
	 .ops = &cfg_type_bridgeport_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_bridgeport_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bridgeport_func_commit 
}; 


static cfg_node_type_t cfg_type_bridgeport_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_bridgeport, 
	 .ops = &cfg_type_bridgeport_common_ops, 
}; 


static cfg_node_ops_t cfg_type_bridgeport_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bridgeport_func_commit 
}; 


static cfg_node_type_t* cfg_type_bridgeport_child[] = { 
	 &cfg_type_bridgeport_common, 
	 &cfg_type_bridgeport_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_bridgeport = { 
	 .name = "BridgePort", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_bridgeport_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_bridgeport_child, 
	 .ops = &cfg_type_bridgeport_ops, 
}; 
