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

static char* cfg_type_tr181ipv6addr_index[] = { 
	 "IPv6Addr_Idx", 
	 NULL 
}; 

int SnycIPIFv6AddressEntry(int wanIf, int ipIndex)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
 	char wanNode[MAXLEN_NODE_NAME] = {0};
 	char pvcNodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char strNum[8] = {0};
	int ipIfIdx_lan = -1;	

	if(ipIndex < 0){
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV6ADDR_PVC_ENTRY_NODE, ipIndex + 1, 1);
	
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);

		memset(pvcNodeName, 0, sizeof(pvcNodeName));	
		memset(tmp, 0, sizeof(tmp));	
		snprintf(pvcNodeName, sizeof(pvcNodeName), TR181_IP_IPV6ADDR_PVC_NODE, ipIndex + 1);
		cfg_obj_get_object_attr(pvcNodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp)) ;
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(pvcNodeName, TR181_NUM_ATTR, 0, strNum);
	}
	
	checkEntryExistByPvcLanIdx(&ipIfIdx_lan, TR181_IP_NODE, IP_INTERFACE_NUM, 0, 0);
	if(ipIndex == ipIfIdx_lan)
	{
		memset(tmp, 0, sizeof(tmp));	
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, "1");
		cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IP6", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(nodeName, "IPAddress", 0, tmp);
	}
	else
	{
		/*for Wan*/
		memset(wanNode, 0, sizeof(wanNode));	
		if(setWanNodeByWanIf(wanNode,wanIf) < 0){
			return -1;
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "Active", 0, tmp, sizeof(tmp)) > 0)
			cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, !strcmp(tmp, "Yes") ? "1" : "0");

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp)) > 0)
		{	
			if(!strcmp(tmp,"N/A")){
				cfg_obj_set_object_attr(nodeName, "Origin", 0, "Static");
				memset(tmp, 0, sizeof(tmp));
				if(cfg_obj_get_object_attr(wanNode, "IPADDR6", 0, tmp, sizeof(tmp)) >= 0)
				{	
					cfg_obj_set_object_attr(nodeName, "IPAddress", 0, tmp);
				}
			}else if(!strcmp(tmp,"No"))
				cfg_obj_set_object_attr(nodeName, "Origin", 0, "AutoConfigured");
			else
				cfg_obj_set_object_attr(nodeName, "Origin", 0, "DHCPv6");
		}
	}
	
	memset(tmp,0,sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"%s%d.IPv6Prefix.1",TR181_IP_INTERFACE,ipIndex + 1);
	cfg_obj_set_object_attr(nodeName, "Prefix", 0, tmp);

	return 0;
}


int cfg_type_tr181ipv6addr_func_commit(char *path){

	char nodeName[MAXLEN_NODE_NAME]= {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char ipNode[MAXLEN_NODE_NAME]= {0};
	int wanIf = -1;
	int pvcIdx = 0;
	int entryIdx = 0;
	char tmp[64] = {0};

	sscanf(path,TR181_TR181IPV6ADDR_PVC_ENTRY_NODE,&pvcIdx,&entryIdx);
	if(pvcIdx < 1 || entryIdx < 1)
	{
		return -1;
	}
	snprintf(nodeName, sizeof(nodeName),TR181_TR181IPV6ADDR_PVC_ENTRY_NODE, pvcIdx,entryIdx);
	
	memset(ipNode, 0, sizeof(ipNode));
	snprintf(ipNode, sizeof(ipNode),TR181_IP_ENTRY_NODE, pvcIdx);

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(ipNode, PVC_INDEX_ATTR, 0, tmp,sizeof(tmp)) < 0 ){
		return -1;
	}
	wanIf = atoi(tmp);
	if(wanIf < 0){
		moveIPInfoToLan(IPV6ADDR_EXECUTE_FLAG, pvcIdx, 1);
		return 0;
	}

	memset(wanNode, 0, sizeof(wanNode));
	if(setWanNodeByWanIf(wanNode,wanIf) <0){
		return -1;
	}

	/*get ipversion*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "IPVERSION",0,tmp,sizeof(tmp));
	if(strstr(tmp, "IPv6") != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) > 0)
			cfg_obj_set_object_attr(wanNode, "Active", 0, !strcmp(tmp, "1") ? "Yes" : "No");

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "IPAddress" ,0,tmp,sizeof(tmp)) > 0) {
			cfg_obj_set_object_attr(wanNode, "IPADDR6", 0,tmp);		 
		}	
		
		commitWanByWanIf(wanIf);
	}
	
	return 0;

}


static cfg_node_ops_t cfg_type_tr181ipv6addr_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6addr_func_commit 
}; 

static cfg_node_type_t cfg_type_tr181ipv6addr_pvc_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_IPV6ADDRESS_ENTRY_NUM, 
	 .parent = &cfg_type_tr181ipv6addr_pvc, 
	 .ops = &cfg_type_tr181ipv6addr_pvc_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv6addr_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6addr_func_commit 
}; 

static cfg_node_type_t* cfg_type_tr181ipv6addr_pvc_child[] = { 
	 &cfg_type_tr181ipv6addr_pvc_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_tr181ipv6addr_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_IP_INTERFACE_IPV6ADDR_NUM, 
	 .parent = &cfg_type_tr181ipv6addr, 
	 .nsubtype = sizeof(cfg_type_tr181ipv6addr_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv6addr_pvc_child, 
	 .ops = &cfg_type_tr181ipv6addr_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv6addr_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6addr_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipv6addr_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181ipv6addr, 
	 .ops = &cfg_type_tr181ipv6addr_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ipv6addr_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6addr_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ipv6addr_child[] = { 
	 &cfg_type_tr181ipv6addr_common, 
	 &cfg_type_tr181ipv6addr_pvc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ipv6addr = { 
	 .name = "TR181IPv6Addr", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ipv6addr_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv6addr_child, 
	 .ops = &cfg_type_tr181ipv6addr_ops, 
}; 
