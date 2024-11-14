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

static char* cfg_type_routeadvifset_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int syncRadvd2routeAdvIfSet(int ipIndex){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[64] = {0};
	int ipIfIdx = -1;
	int wanIf = -1;
	char strNum[8] = {0};
	int routeAdvIfIdx = -1;
	int pvcIndex = -1;
	int flag = 0;
	
	flag = checkEntryExistByPvcLanIdx(&routeAdvIfIdx, TR181_ROUTEADVIFSET_NODE, RTADV_ITFSET_NUM_MAX, 0, 0);
	if(routeAdvIfIdx < 0){
		tcdbg_printf("[%s:%d]Entry is not exist, Error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, routeAdvIfIdx+1);

	if(flag == 0){
		/*not exist need to create a new Entry*/	
		cfg_obj_create_object(nodeName);

		/*modify num, need +1*/
		cfg_obj_get_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
	}


	/*Enable*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(RADVD_ENTRY_NODE, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp));
	if(1 == atoi(tmp))
	{
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, "1");/*RouteAdvIfSet_Entry%d  Enable*/
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, "0");
	}

	/*Interface*/
	snprintf(tmp,sizeof(tmp), TR181_IP_INTERFACE"%d", ipIndex + 1);
	cfg_obj_set_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, tmp);
	cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, "-1");
	cfg_obj_set_object_attr(nodeName, LAN_INDEX_ATTR, 0, "0");

	/*Mode & ManualPrefixes*/		
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "Mode", 0, tmp, sizeof(tmp));
	if(1 == atoi(tmp))
	{	/*manual*/
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp,sizeof(tmp), "Device.IP.Interface.%d.IPv6Prefix.1", ipIndex + 1);
		cfg_obj_set_object_attr(nodeName, "MauPrefixes", 0, tmp);
		updateIPv6PreFix(-1,ipIndex);
	}
	else
	{	/*Auto*/		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "DelegatedWanConnection", 0, tmp, sizeof(tmp)) > 0)
		{	
			pvcIndex = atoi(tmp);
			ipIfIdx = getTR181NodeEntryIdxByWanIf(TR181_IP_NODE,IP_INTERFACE_NUM, pvcIndex);
			if(ipIfIdx >= 0){
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp,sizeof(tmp), "Device.IP.Interface.%d.IPv6Prefix.1", ipIfIdx + 1);
			cfg_obj_set_object_attr(nodeName, "MauPrefixes", 0, tmp);
			}
		}
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "ManagedEnable", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(nodeName, "AdvMngFlg", 0, !strcmp(tmp,"1") ? "1" : "0");/*RouteAdvIfSet_Entry%d  AdvManagedFlag*/
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "OtherEnable", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(nodeName, "AdvOthrCfgFlg", 0, !strcmp(tmp,"1") ? "1" : "0");/*RouteAdvIfSet_Entry%d  AdvOtherConfigFlag*/
	}


	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "MinRAInterval", 0, tmp, sizeof(tmp)) >=0)
	{
		cfg_obj_set_object_attr(nodeName, "MinRtrAdvItv", 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(RADVD_ENTRY_NODE, "MaxRAInterval", 0, tmp, sizeof(tmp)) >=0)
	{
		cfg_obj_set_object_attr(nodeName, "MaxRtrAdvItv", 0, tmp);
	}
	return 0;
}


int syncWanSlaac2routeAdvIfSet(int wanIf,int ipIndex, int EnableFlag){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[64] = {0};
	char strNum[8] = {0};
	int routeAdvIfIdx = -1;
	int pvcIndex = -1;
	int flag = 0;

	flag = checkEntryExistByPvcLanIdx(&routeAdvIfIdx, TR181_ROUTEADVIFSET_NODE, RTADV_ITFSET_NUM_MAX, wanIf, 1);
	if(routeAdvIfIdx < 0){
		tcdbg_printf("[%s:%d]Entry is not exist, Error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, routeAdvIfIdx+1);

	/*Enable*/
	if(EnableFlag == 1){
		/*slaac is Enable*/
		if(flag == 0){
			/*Entry is not exist*/
			cfg_obj_create_object(nodeName);

			/*modify num, need +1*/
			cfg_obj_get_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
			snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
			cfg_obj_set_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
		}

		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, "1");/*RouteAdvIfSet_Entry%d  Enable*/
		/*Interface*/
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp,sizeof(tmp), TR181_IP_INTERFACE"%d", ipIndex + 1);
		cfg_obj_set_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, tmp);
		
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", wanIf); 
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);
		cfg_obj_set_object_attr(nodeName, LAN_INDEX_ATTR, 0, "-1");
	}else{
		/*slaac is not set*/
		if(flag == 1){
			/*Entry is exist, need to delete*/
			cfg_obj_delete_object(nodeName);
			/*modify num, need +1*/
			cfg_obj_get_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
			snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) - 1); 
			cfg_obj_set_object_attr(TR181_ROUTEADVIFSET_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
		}
	}

	return 0;
}


 
int routeadvifsetSync2Radvd(int etyIdx)
{
	char tmp[128] = {0};	
	char MauInterface[128] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int ipifIdx = -1;
	int wanIf = -1;
	int ret = -1;
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, etyIdx);

	/*Enable*/
	cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp));
	if( strcmp(tmp,"1") )
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, TR181_ENABLE_ATTR, 0, "1");
	else if( !strcmp(tmp,"0") )
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, TR181_ENABLE_ATTR, 0, "0");
	
	/*ManualPrefixes:eg. IP.Interface.{i}.IPv6PreFix   => RADVD_ENTRY_NODE Mode(0:auto, 1:manual)*/	
	memset(MauInterface, 0, sizeof(MauInterface));
	ret = cfg_obj_get_object_attr(nodeName, "MauPrefixes", 0, MauInterface, sizeof(MauInterface));
	
	if( ret > 0 )
	{	
		sscanf(MauInterface, "Device.IP.Interface.%d.IPv6Prefix.1", &ipifIdx);	
		wanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipifIdx);
		if( wanIf < 0 )
		{
			/*not wan interface, set as br0 manual*/		
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "Mode", 0, "1");
		}
		else 
		{
			/*is wan interface, set mode to auto*/		
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "Mode", 0, "0");
			/*set select wan interface*/
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", wanIf);
			cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "DelegatedWanConnection", 0, tmp);
		}	
	}	
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "AdvMngFlg", 0, tmp, sizeof(tmp)) >= 0){		
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "ManagedEnable", 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "AdvOthrCfgFlg", 0, tmp, sizeof(tmp)) >= 0){		
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "OtherEnable", 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "MinRtrAdvItv", 0, tmp, sizeof(tmp)) >=0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "MinRAInterval", 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "MaxRtrAdvItv", 0, tmp, sizeof(tmp)) >=0)
	{
		cfg_obj_set_object_attr(RADVD_ENTRY_NODE, "MaxRAInterval", 0, tmp);
	}

	cfg_tr181_commit_object(RADVD_ENTRY_NODE);
	
	return 0;
}


int routeadvifsetSync2WanSlaac(int etyIdx)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char dhcpv6Node[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int ipIfIdx = -1;
	int oldwanIf = -1;
	int newwanIf = -1;
	int dhcpv6Idx = -1;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, etyIdx);
		
	ipIfIdx = getIPIfByTR181Node(TR181_ROUTEADVIFSET_NODE, etyIdx);
	newwanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	oldwanIf = getPvcIdxByTR181NodeEntry(TR181_ROUTEADVIFSET_NODE, etyIdx);	
	if(newwanIf!=oldwanIf){
		if(oldwanIf >= 0){
			setWanNodeByWanIf(wanNode,oldwanIf);	
			
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp));
			if(strcmp(tmp, "No") == 0){ /*if old wan is slaac, need to modify to static*/
				cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "N/A");		/*set to static*/
				cfg_obj_set_object_attr(wanNode, "ISP", 0, "1");
			}

			cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, "-1");
			commitWanByWanIf(oldwanIf);
		}
	}

	if(newwanIf >= 0){			
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp));
		if(!strcmp(tmp,"0") || !strlen(tmp)){
			return 0;
		}		

		memset(wanNode, 0, sizeof(wanNode));
		if(setWanNodeByWanIf(wanNode,newwanIf) < 0){
			return -1;
		}
		
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp));
		if(strcmp(tmp, "No") != 0){ 
			/*if wan is  not slaac, need to modify to slaac*/
			cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "No");
			
			clearOtherIneterfaceByWanIf(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, -1, newwanIf); 
		}

		/*set pvcIndex*/
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"%d",newwanIf);
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);
		commitWanByWanIf(newwanIf);
	}
	
	return 0;
}


int cfg_type_routeadvifset_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int etyIdx = -1;
	char strInterface[128] = {0};
	int ipifIdx = -1;
	int wanIf = -1;

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, etyIdx);
	
	memset(strInterface, 0, sizeof(strInterface));
	if( cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, strInterface, sizeof(strInterface)) <= 0 )
		return 0;
	
	ipifIdx = getSuffixIntByInterface(strInterface);					
	wanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipifIdx);
	if(wanIf < 0){
		/*sync to lan*/
		routeadvifsetSync2Radvd(etyIdx);
	}else{
		/*sync to wan: SLAAC*/
		routeadvifsetSync2WanSlaac(etyIdx);
	}

	return 0;
}

static cfg_node_ops_t cfg_type_routeadvifset_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeadvifset_func_commit 
}; 


static cfg_node_type_t cfg_type_routeadvifset_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | RTADV_ITFSET_NUM_MAX, 
	 .parent = &cfg_type_routeadvifset, 
	 .index = cfg_type_routeadvifset_index,
	 .ops = &cfg_type_routeadvifset_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_routeadvifset_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeadvifset_func_commit 
}; 


static cfg_node_type_t cfg_type_routeadvifset_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_routeadvifset, 
	 .ops = &cfg_type_routeadvifset_common_ops, 
}; 


static cfg_node_ops_t cfg_type_routeadvifset_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeadvifset_func_commit 
}; 


static cfg_node_type_t* cfg_type_routeadvifset_child[] = { 
	 &cfg_type_routeadvifset_common, 
	 &cfg_type_routeadvifset_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_routeadvifset = { 
	 .name = "RouteAdvIfSet", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_routeadvifset_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_routeadvifset_child, 
	 .ops = &cfg_type_routeadvifset_ops, 
}; 
