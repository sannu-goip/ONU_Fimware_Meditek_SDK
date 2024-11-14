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

int SnycIPIFv4AddressEntry(int wanIf, int ipIfIdx){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char ipv4addrpvcNode[MAXLEN_NODE_NAME] = {0};
	char ipAddr[32] = {0}, netMask[32] = {0};
	char typeVal[16] = {0}, tmp[8] = {0};
	char isp[4] = {0}, num[4] = {0};
	int number = -1;

	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV4ADDR_PVC_ENTRY_NODE, ipIfIdx + 1, 1);
	
	/*create pvc_entry{i}:*/
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);		
		memset(ipv4addrpvcNode, 0, sizeof(ipv4addrpvcNode));	
		snprintf(ipv4addrpvcNode, sizeof(ipv4addrpvcNode), TR181_IP_INTERFACE_IPV4ADDR_PVC_NODE, ipIfIdx + 1);
		cfg_obj_get_object_attr(ipv4addrpvcNode, TR181_NUM_ATTR, 0, num, sizeof(num));
		number = atoi(num) + 1;
		memset(num, 0, sizeof(num));
		snprintf(num, sizeof(num), "%d", number);
		cfg_obj_set_object_attr(ipv4addrpvcNode, TR181_NUM_ATTR, 0, num);
	}


	if(ipIfIdx == 0)
	{
		/*for Lan*/
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, "1");
		cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IP", 0, ipAddr, sizeof(ipAddr));
		cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "netmask", 0, netMask, sizeof(netMask));
		strncpy(typeVal, "Static", sizeof(typeVal) - 1);
	}
	else
	{
		/*get IPADDR,NETMASK form Wan_PVC*/ 
		if(setWanNodeByWanIf(wanNode,wanIf) < 0){
			return -1;
		}
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "Active", 0, tmp, sizeof(tmp)) > 0)
			cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, !strcmp(tmp, "Yes") ? "1" : "0");
		
		cfg_obj_get_object_attr(wanNode, "IPADDR", 0, ipAddr, sizeof(ipAddr));
		cfg_obj_get_object_attr(wanNode, "NETMASK", 0, netMask, sizeof(netMask));
		cfg_obj_get_object_attr(wanNode, "ISP", 0, isp, sizeof(isp));
		
		if(strcmp("0", isp)==0)
		{
			strncpy(typeVal, "DHCP", sizeof(typeVal) - 1);
		}
		else if(strcmp("1", isp)==0)
		{
			strncpy(typeVal, "Static", sizeof(typeVal) - 1);
		}
		else if(strcmp("2", isp)==0)
		{	/*PPP*/
			strncpy(typeVal, "IPCP", sizeof(typeVal) - 1);
		}

	}
	
	cfg_obj_set_object_attr(nodeName, "IPAddress", 0, ipAddr);
	cfg_obj_set_object_attr(nodeName, "SubnetMask", 0, netMask);
	cfg_obj_set_object_attr(nodeName, "AddressingType", 0, typeVal);
	                                   
	return 0;
}

int cfg_type_ipv4addr_func_commit(char* path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char ipIFNode[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int pvcIdx = -1;
	int entryIdx = -1;
	int wanIf = -1;
	char tmp[64] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
	int ret = -1;
		
	sscanf(path,TR181_TR181IPV4ADDR_PVC_ENTRY_NODE,&pvcIdx,&entryIdx);
	if(pvcIdx < 1 || entryIdx < 1)
		{
		return -1;
	}
	snprintf(nodeName, sizeof(nodeName),TR181_TR181IPV4ADDR_PVC_ENTRY_NODE, pvcIdx,entryIdx);

	memset(ipIFNode, 0, sizeof(ipIFNode));
	snprintf(ipIFNode, sizeof(ipIFNode), TR181_IP_ENTRY_NODE, pvcIdx);
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(ipIFNode, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) < 0)
	{
		return 0;
	}
	
	wanIf = atoi(tmp);
	if(wanIf < 0)
	{
		moveIPInfoToLan(IPV4ADDR_EXECUTE_FLAG, pvcIdx, TR181_IPV4ADDR_ENTRY_INDEX);
		return 0;
	}
	
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	/*get ipversion*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, TR181_IPVERSION_ATTR, 0, tmp, sizeof(tmp));
	
	if(strstr(tmp, "IPv4") != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) > 0)
			cfg_obj_set_object_attr(wanNode, "Active", 0, !strcmp(tmp, "1") ? "Yes" : "No");
		
		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, TR181_IPADDRESS_ATTR, 0, tmp, sizeof(tmp)) >= 0)
			&& strlen(tmp) > 0)
		{			
			cfg_obj_set_object_attr(wanNode, "IPADDR", 0, tmp);
		}
		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, TR181_SUBNETMASK_ATTR, 0, tmp, sizeof(tmp)) >= 0)
			&& strlen(tmp) > 0) 
		{
			cfg_obj_set_object_attr(wanNode, "NETMASK", 0, tmp);
		}
		
		commitWanByWanIf(wanIf);
	}

	return 0;	
}

static cfg_node_ops_t cfg_type_tr181ipv4addr_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipv4addr_func_commit 
}; 

static cfg_node_type_t cfg_type_tr181ipv4addr_pvc_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_IPV4ADDRESS_ENTRY_NUM, 
	 .parent = &cfg_type_tr181ipv4addr_pvc, 
	 .ops = &cfg_type_tr181ipv4addr_pvc_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv4addr_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipv4addr_func_commit 
}; 

static cfg_node_type_t* cfg_type_tr181ipv4addr_pvc_child[] = { 
	 &cfg_type_tr181ipv4addr_pvc_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_tr181ipv4addr_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_IP_INTERFACE_IPV4ADDR_NUM, 
	 .parent = &cfg_type_tr181ipv4addr, 
	 .nsubtype = sizeof(cfg_type_tr181ipv4addr_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv4addr_pvc_child, 
	 .ops = &cfg_type_tr181ipv4addr_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv4addr_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipv4addr_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipv4addr_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181ipv4addr, 
	 .ops = &cfg_type_tr181ipv4addr_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ipv4addr_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipv4addr_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ipv4addr_child[] = { 
	 &cfg_type_tr181ipv4addr_common, 
	 &cfg_type_tr181ipv4addr_pvc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ipv4addr = { 
	 .name = "TR181IPv4Addr", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ipv4addr_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv4addr_child, 
	 .ops = &cfg_type_tr181ipv4addr_ops, 
}; 
