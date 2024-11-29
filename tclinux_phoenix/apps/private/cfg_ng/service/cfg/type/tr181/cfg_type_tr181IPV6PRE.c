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


static char* cfg_type_tr181ipv6pre_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int syncDhcp6s2LanPreFix(int ipIndex)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char pvcnodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char preAddr[128] = {0};
	char preLen[8] = {0};
	char strNum[8] = {0};

	if(ipIndex < 0){
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),TR181_IPV6PRE_PVC_ENTRY_NODE,ipIndex + 1,2);		
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
		memset(pvcnodeName, 0, sizeof(pvcnodeName));
		snprintf(pvcnodeName, sizeof(pvcnodeName), TR181_TR181IPV6PRE_PVC_NODE, ipIndex + 1);
		cfg_obj_get_object_attr(pvcnodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp)) ;
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(pvcnodeName, TR181_NUM_ATTR, 0, strNum);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{	
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, !strcmp(tmp,"1") ? "1" : "0");

	}	
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "PrefixIPv6", 0, preAddr, sizeof(preAddr)) >= 0 
		&& cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "Prefixv6Len", 0, preLen, sizeof(preLen)) >= 0)
	{
		snprintf(tmp,sizeof(tmp),"%s/%s",preAddr,preLen);
		cfg_obj_set_object_attr(nodeName, "Prefix", 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "PreferredLifetime", 0, tmp, sizeof(tmp)) >= 0)
	{	
		cfg_obj_set_object_attr(nodeName, "PreLifetime", 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "ValidLifetime", 0, tmp, sizeof(tmp)) >= 0)
	{	
		cfg_obj_set_object_attr(nodeName, "ValidTime", 0, tmp);
	}
  
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "Mode", 0, tmp, sizeof(tmp)) >= 0)
	{	
		cfg_obj_set_object_attr(nodeName, "StaticType", 0, !strcmp(tmp,"1") ? "Static" : "Auto");

	}

	return 0;
}



int updateIPv6PreFix(int wanIf,int ipIndex){
	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char pvcnodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char preAddr[128] = {0};
	char preLen[8] = {0};
	char strNum[8] = {0};
	int ipIfIdx_lan = -1;
	char mode[8] = {0};
	
	if(ipIndex < 0 )
	{
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),TR181_IPV6PRE_PVC_ENTRY_NODE,ipIndex + 1,1);		
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
		memset(pvcnodeName, 0, sizeof(pvcnodeName));
		snprintf(pvcnodeName, sizeof(pvcnodeName), TR181_TR181IPV6PRE_PVC_NODE, ipIndex + 1);
		cfg_obj_get_object_attr(pvcnodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp)) ;
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(pvcnodeName, TR181_NUM_ATTR, 0, strNum);
	}

	checkEntryExistByPvcLanIdx(&ipIfIdx_lan, TR181_IP_NODE, IP_INTERFACE_NUM, 0, 0);	
	if(ipIndex == ipIfIdx_lan)
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0,!strcmp(tmp,"1") ? "1" : "0");
		}	
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "PrefixIPv6", 0, preAddr, sizeof(preAddr)) >= 0 
			&& cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "Prefixv6Len", 0, preLen, sizeof(preLen)) >= 0)
		{
			snprintf(tmp,sizeof(tmp),"%s/%s",preAddr,preLen);
			cfg_obj_set_object_attr(nodeName, "Prefix", 0, tmp);
		}
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "PreferredLifetime", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "PreLifetime", 0, tmp);
		}
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "ValidLifetime", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "ValidTime", 0, tmp);
		}
	  
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "Mode", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "StaticType", 0, !strcmp(tmp,"1") ? "Static" : "Auto");
		}
	}
	else
	{
	
		if(setWanNodeByWanIf(wanNode,wanIf) < 0)
		{
			return -1;
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "PDEnable", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, !strcmp(tmp,"Yes") ? "1" : "0");
		}

		memset(mode, 0, sizeof(mode));
		if(cfg_obj_get_object_attr(wanNode, "DHCPv6PD", 0, mode, sizeof(mode)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "StaticType", 0, !strcmp(mode,"Yes") ? "Auto" : "Static");
		}

		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"%d",wanIf);
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR,0,tmp);

		memset(tmp, 0, sizeof(tmp));
		if(!strcmp(mode,"No"))
		{	
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(wanNode, "PDPrefix", 0, tmp, sizeof(tmp)) >= 0 )
			{
				cfg_obj_set_object_attr(nodeName, "Prefix", 0, tmp);
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "PrefixPltime", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "PreLifetime", 0, tmp);
		}
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "PrefixVltime", 0, tmp, sizeof(tmp)) >= 0)
		{	
			cfg_obj_set_object_attr(nodeName, "ValidTime", 0, tmp);
		}
		}
	}

	return 0;
}


int syncIPv6Pre2Dhcp6s(char *nodeName)
{
	char tmp[128] = {0};
    char *pPrefix = NULL;

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "StaticType", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Mode", 0,!strcmp(tmp,"Static") ? "1" : "0");
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,tmp,sizeof(tmp)) >= 0){
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, TR181_ENABLE_ATTR, 0,tmp);
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "Prefix", 0,tmp,sizeof(tmp)) >= 0)
	{
		pPrefix = strchr(tmp,'/');
		if(NULL != pPrefix)
		{
			tmp[pPrefix-tmp] = '\0';
			cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "PrefixIPv6", 0,tmp);
			pPrefix++;
			if(pPrefix != NULL)
			{
				cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Prefixv6Len", 0,pPrefix);
			}
		}
		else
		{
			cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "PrefixIPv6", 0,tmp);
			cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Prefixv6Len", 0,"64");
		}
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "PreLifetime", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "PreferredLifetime", 0,tmp);
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "ValidTime", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "ValidLifetime", 0,tmp);
	}
	
	cfg_tr181_commit_object(DHCP6S_ENTRY_NODE);
	return 0;
}


int syncIPv6Pre2Radvd(char *nodeName){
	char tmp[128] = {0};
    char *pPrefix = NULL;

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "StaticType", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "Mode", 0,!strcmp(tmp,"Static") ? "1" : "0");
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, TR181_ENABLE_ATTR, 0,tmp);
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "Prefix", 0,tmp,sizeof(tmp)) >= 0)
	{
		pPrefix = strchr(tmp,'/');
		if(NULL != pPrefix)
		{
			tmp[pPrefix-tmp] = '\0';
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "PrefixIPv6", 0, tmp);
			pPrefix++;
			if(pPrefix != NULL)
			{
				cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "Prefixv6Len", 0,pPrefix);
			}
		}
		else
		{
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "PrefixIPv6", 0,tmp);
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "Prefixv6Len", 0,"64");
		}
	}

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "PreLifetime", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "PreferredLifetime",0, tmp);
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "ValidTime", 0,tmp,sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "ValidLifetime", 0,tmp);
	}
	
	cfg_tr181_commit_object(RADVD_ENTRY_NODE);
	return 0;
}



int syncIPv6Pre2Wan(char *nodeName,int wanIf){
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char DHCPNode[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int isp = -1;
	int etyIdx = -1;
	int enableFlag = -1;
	char flag = -1; 

	memset(wanNode, 0, sizeof(wanNode));
	if(setWanNodeByWanIf(wanNode,wanIf) <0)
	{
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, TR181_ISP_ATTR,0,tmp,sizeof(tmp));
	isp = atoi(tmp);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, TR181_IPVERSION_ATTR,0,tmp,sizeof(tmp));
	if(strstr(tmp, "IPv6") != NULL)
	{

		/*set PD*/
		memset(tmp,0,sizeof(tmp));
		etyIdx = getTR181NodeEntryIdxByWanIf(TR181_DHCPV6_CLIENT_NODE,DHCPV6_CLIENT_NUM_MAX,wanIf);
		if(etyIdx >= 0)
		{
			snprintf(DHCPNode, sizeof(DHCPNode), TR181_DHCPV6_CLIENT_ENTRY_NODE, etyIdx + 1);
			cfg_obj_get_object_attr(DHCPNode, "RequestPrefixes", 0, tmp, sizeof(tmp));
			enableFlag = atoi(tmp);
		}else
		{
			enableFlag = 1;
		}

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		{	
			if(!strcmp(tmp,"1") && enableFlag == 1)
			{
				flag = 0;
				cfg_obj_set_object_attr(wanNode, "PDEnable", 0, "Yes");
			}
			else
			{
				cfg_obj_set_object_attr(wanNode, "PDEnable", 0, "No");
				cfg_obj_set_object_attr(wanNode, "DHCPv6PD", 0, "No");
				cfg_obj_set_object_attr(wanNode, "PDOrigin", 0,"None");
			}
		}

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "StaticType", 0,tmp,sizeof(tmp)) >= 0 && flag == 0){
			if(isp != WAN_ENCAP_STATIC_INT)
			{
				cfg_obj_set_object_attr(wanNode, "DHCPv6PD", 0,!strcmp(tmp,"Auto") ? "Yes" : "No");
				cfg_obj_set_object_attr(wanNode, "PDOrigin", 0,!strcmp(tmp,"Auto") ? "PrefixDelegation" : "Static");
			}
			else
			{
				cfg_obj_set_object_attr(wanNode, "DHCPv6PD", 0, "No");
				cfg_obj_set_object_attr(wanNode, "PDOrigin", 0, "Static");		
			}
			
		}  

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "Prefix", 0,tmp,sizeof(tmp)) >= 0)
		{
			cfg_obj_set_object_attr(wanNode, "PDPrefix", 0,tmp);
		} 

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "ValidTime", 0,tmp,sizeof(tmp)) >= 0)
		{
			cfg_obj_set_object_attr(wanNode, "PrefixVltime", 0,tmp);
		}

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "PreLifetime", 0,tmp,sizeof(tmp)) >= 0)
		{
			cfg_obj_set_object_attr(wanNode, "PrefixPltime", 0,tmp);
		}  
	}
	
	commitWanByWanIf(wanIf);
	return 0;
}


int cfg_type_tr181ipv6pre_func_commit(char *path){

	char nodeName[MAXLEN_NODE_NAME] = {0};
	char ipNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int pvcIdx = -1;
	int entryIdx = 0;
	int wanIf = -1;

	sscanf(path,TR181_IPV6PRE_PVC_ENTRY_NODE,&pvcIdx,&entryIdx);
	if(pvcIdx < 1 || entryIdx < 1)
	{
		return -1;
	}
	snprintf(nodeName, sizeof(nodeName),TR181_IPV6PRE_PVC_ENTRY_NODE, pvcIdx,entryIdx);

	memset(ipNode, 0, sizeof(ipNode));
	snprintf(ipNode, sizeof(ipNode),TR181_IP_ENTRY_NODE, pvcIdx);

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(ipNode, PVC_INDEX_ATTR, 0, tmp,sizeof(tmp)) < 0 ){
		return -1;
	}
	wanIf = atoi(tmp);

	if(wanIf < 0 && entryIdx == 1){
		/*sync Radvd_Entry*/
		syncIPv6Pre2Radvd(nodeName);
	}else if(wanIf < 0 && entryIdx == 2){
		/*sync Dhcp6s_Entry*/
		syncIPv6Pre2Dhcp6s(nodeName);
	}else if(wanIf >= 0){  
		/*sync Wan*/
		syncIPv6Pre2Wan(nodeName,wanIf); 		
	}
	return 0;
}


static cfg_node_ops_t cfg_type_tr181ipv6pre_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6pre_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipv6pre_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_IPV6PREFIX_ENTRY_NUM, 
	 .parent = &cfg_type_tr181ipv6pre_pvc, 
	 .ops = &cfg_type_tr181ipv6pre_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv6pre_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6pre_func_commit 
}; 

static cfg_node_type_t* cfg_type_tr181ipv6pre_pvc_child[] = { 
	 &cfg_type_tr181ipv6pre_entry,
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ipv6pre_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_IP_INTERFACE_IPV6PREF_NUM, 
	 .parent = &cfg_type_tr181ipv6pre, 
	 .nsubtype = sizeof(cfg_type_tr181ipv6pre_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv6pre_pvc_child, 
	 .ops = &cfg_type_tr181ipv6pre_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ipv6pre_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6pre_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipv6pre_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181ipv6pre, 
	 .ops = &cfg_type_tr181ipv6pre_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ipv6pre_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ipv6pre_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ipv6pre_child[] = { 
	 &cfg_type_tr181ipv6pre_common, 
	 &cfg_type_tr181ipv6pre_pvc,
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ipv6pre = { 
	 .name = "TR181IPV6PRE", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ipv6pre_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipv6pre_child, 
	 .ops = &cfg_type_tr181ipv6pre_ops, 
}; 
