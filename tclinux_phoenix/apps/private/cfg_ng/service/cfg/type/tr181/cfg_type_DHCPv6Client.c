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


static char* cfg_type_dhcpv6client_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int dhcp6cEntrySync2WanByWanIf(int index, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char strISP[20]={0};
	char enable[20]={0};
	char tmp[64] = {0};
	int pvcNodeIdx = -1;
	char ipv6preNode[MAXLEN_NODE_NAME] = {0};	
	int enableFlag = -1;

	if(index < 0 || wanIf < 0)
	{
		tcdbg_printf("[%s:%d]index=%d wanIf=%d  error!\n",__FUNCTION__,__LINE__,index,wanIf);
		return -1;
	}
		
	memset(enable, 0, sizeof(enable));	
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DHCPV6_CLIENT_ENTRY_NODE, index + 1);

	if(setWanNodeByWanIf(wanNode, wanIf) < 0){		
		return -1;
	}

	/* set Wan  info*/
	memset(enable, 0, sizeof(enable));
	if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, enable, sizeof(enable)) >= 0)
	{
		cfg_obj_get_object_attr(wanNode, TR181_ISP_ATTR, 0, strISP, sizeof(strISP));
			
		if(!strcmp(enable, "1"))
		{
			/*V6 dhcp enable*/					
			/*only 0: dynamic ; 2:ppp support v6 DHCP function*/
			cfg_obj_set_object_attr(wanNode, TR181_ISP_ATTR, 0, "0");  /*set WAN_ENCAP_DYN_INT*/
			cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
			cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkIP");

			/*set DHCPv6*/
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp));
			if(strcmp(tmp, "Yes") != 0)             
			{ 
				/*if  wan is  not DHCP, need to set to DHCP*/
				cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "Yes");
				clearOtherIneterfaceByWanIf(TR181_ROUTEADVIFSET_NODE, RTADV_ITFSET_NUM_MAX, -1, wanIf); 
			}
		}
		else
		{
			if(atoi(strISP) == WAN_ENCAP_DYN_INT)
			{
				cfg_obj_set_object_attr(wanNode, TR181_ISP_ATTR, 0, "1");  /*set WAN_ENCAP_STATIC_INT*/
				cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "N/A");
			}
		}

		/*set PD*/
		memset(tmp, 0, sizeof(tmp));
		pvcNodeIdx = getPvcNodeIdxByWanIf(TR181_IP_INTERFACE_IPV6PREF_NODE,TR181_IP_INTERFACE_IPV6PREF_NUM,wanIf);
		if(pvcNodeIdx >= 0){
			snprintf(ipv6preNode, sizeof(ipv6preNode), TR181_IPV6PRE_PVC_ENTRY_NODE, pvcNodeIdx + 1,1);
			cfg_obj_get_object_attr(ipv6preNode, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp));
			enableFlag = atoi(tmp);
		}
		else
		{
			enableFlag = 1;
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "RequestPrefixes", 0, tmp, sizeof(tmp)) >= 0 && enableFlag == 1)
		{	
			if(!strcmp(tmp,"1") && enableFlag == 1)
		{		
				cfg_obj_set_object_attr(wanNode, "PDEnable", 0, "Yes");
				cfg_obj_set_object_attr(wanNode, "DHCPv6PD", 0, "Yes" );
				cfg_obj_set_object_attr(wanNode, "PDOrigin", 0, "PrefixDelegation");
			}
			else
			{
				cfg_obj_set_object_attr(wanNode, "PDEnable", 0, "No");				
				cfg_obj_set_object_attr(wanNode, "DHCPv6PD", 0, "No" );
				cfg_obj_set_object_attr(wanNode, "PDOrigin", 0, "None");
			}
		}
	}

	return 0;
}

int dhcp6cSync2WanByTR181IPIf(int ipIfIdx, int wanIf)
{
	char tr181IPNode[MAXLEN_NODE_NAME] = {0};	
	char pvcBuf[20]={0};
	int index = -1;
	int ret = -1;

	if(ipIfIdx < 0 || wanIf < 0){
		return -1;
	}

	/*Find Dhcp6c Entry index by TR181IP Index*/
	index = getTR181NodeEntryIdxByIPIf(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, ipIfIdx);
	if(index < 0)
	{
		/*not find NatInerface*/		
		tcdbg_printf("[%s:%d]index=%d < 0, error!\n",__FUNCTION__,__LINE__,index);
		return -1;
	}

	/*find wanPvc index by TR181IP index*/
	memset(tr181IPNode, 0, sizeof(tr181IPNode));	
	snprintf(tr181IPNode, sizeof(tr181IPNode), TR181_IP_ENTRY_NODE, ipIfIdx + 1);
	if((cfg_obj_get_object_attr(tr181IPNode, "pvcIndex", 0, pvcBuf, sizeof(pvcBuf)) < 0)
		|| (pvcBuf[0] == '\0'))
	{
		/*pvcIdx not exist, wan not exist*/
		tcdbg_printf("[%s:%d]pvcIndex error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	
	ret = dhcp6cEntrySync2WanByWanIf(index, wanIf);
	return ret;
}

int findDhcpv6ClientEntry(int wanIf, int active)
{
	struct tr181_interface_s interface;
	int ret = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName,TR181_DHCPV6_CLIENT_NODE, CFG_BUF_64_LEN - 1);
	interface.num = DHCPV6_CLIENT_NUM_MAX;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	ret = createTR181InterfaceEntry(interface);
	
	return ret;
}

int syncWan2DHCPv6Client(int wanIf, int ipIndex, int EnableFlag){	
	char dhcpv6node[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int etyIdx = -1;	
	int dhcp6c_flag = 0;
	char tmp[128] = {0};
	char strNum[8] = {0};
	
	dhcp6c_flag = checkEntryExistByPvcLanIdx(&etyIdx, TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, wanIf, 1) ;
	snprintf(dhcpv6node,sizeof(dhcpv6node),TR181_DHCPV6_CLIENT_ENTRY_NODE,etyIdx + 1);

	if(EnableFlag == 1){
		/*DHCPv6 is set*/
		if(dhcp6c_flag == 0){
			/*Entry not exist*/
			cfg_obj_create_object(dhcpv6node);			
			cfg_obj_get_object_attr(TR181_DHCPV6_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
			snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
			cfg_obj_set_object_attr(TR181_DHCPV6_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
		}

		/*set interface*/
		memset(tmp,0,sizeof(tmp));
		snprintf(tmp, sizeof(tmp), TR181_IP_INTERFACE"%d", ipIndex+1);	
		setInterfaceValue(TR181_DHCPV6_CLIENT_NODE, etyIdx, tmp);

			
		/*set pvcIndex, lanindex*/
		memset(tmp,0,sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"%d",wanIf);
		cfg_obj_set_object_attr(dhcpv6node, LAN_INDEX_ATTR, 0, "-1");
		cfg_obj_set_object_attr(dhcpv6node, PVC_INDEX_ATTR, 0, tmp);

		/*PDEnable*/
		memset(tmp,0,sizeof(tmp));
		if(setWanNodeByWanIf(wanNode,wanIf) < 0){	
			return -1;
		}
		
		memset(tmp, 0, sizeof(tmp));
		if( cfg_obj_get_object_attr(wanNode, "Active", 0, tmp, sizeof(tmp)) )
			cfg_obj_set_object_attr(dhcpv6node, TR181_ENABLE_ATTR, 0, !strcmp(tmp, "Yes") ? "1" : "0");

		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(wanNode, "PDEnable", 0,tmp,sizeof(tmp)) >= 0){
			cfg_obj_set_object_attr(dhcpv6node, "RequestPrefixes", 0, !strcmp(tmp,"Yes") ? "1" : "0");
		}
	}else{
		/*wan is to DHCPv6*/
		if(dhcp6c_flag == 1){
			/*Entry not exist*/
			cfg_obj_delete_object(dhcpv6node);			
			cfg_obj_get_object_attr(TR181_DHCPV6_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
			snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) - 1); 
			cfg_obj_set_object_attr(TR181_DHCPV6_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
		}

	}

	return 0;
}


int getPvcNodeIdxByWanIf(char *node,int num,int wanIf){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char value[256] = {0};
	int i = 0;
	
	for( i = 0; i < num; i++)
	{		
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.pvc.%d.entry.1", node, i + 1);

		memset(value,0,sizeof(value));		
		if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, value, sizeof(value)) > 0)
		{								
			if(atoi(value) == wanIf)
				return i;
		}
	}
	
	return -1;
}



int cfg_type_dhcpv6client_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int ipIfIdx = -1;
	int oldwanIf = -1;
	int newwanIf = -1;
	int enableFlag = 0;
	int etyIdx = -1; 
	int ret = -1;
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DHCPV6_CLIENT_ENTRY_NODE, etyIdx);
		
	ipIfIdx = getIPIfByTR181Node(TR181_DHCPV6_CLIENT_NODE, etyIdx);
	newwanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	oldwanIf = getPvcIdxByTR181NodeEntry(TR181_DHCPV6_CLIENT_NODE, etyIdx);	
	if(newwanIf!=oldwanIf)
	{
		if(oldwanIf >= 0)
		{
			if(setWanNodeByWanIf(wanNode,oldwanIf) < 0){
				return -1;
			}	
			
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp));
			if(strcmp(tmp, "Yes") == 0)
			{ /*if old wan is dhcp, need to modify to static*/
				cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "N/A");		/*set to static*/
				cfg_obj_set_object_attr(wanNode, "ISP", 0, "1");
			}

			commitWanByWanIf(oldwanIf);
			clearOtherIneterfaceByWanIf(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, etyIdx, newwanIf); 
		}
	}
	
	if(newwanIf < 0)
	{				
		setPvcIdxByTR181NodeEntry(TR181_DHCPV6_CLIENT_NODE, etyIdx, -1);
		return -1;
	}
	else
		{	
		/*Sync to Wan*/ 
		setPvcIdxByTR181NodeEntry(TR181_DHCPV6_CLIENT_NODE, etyIdx, newwanIf);
		ret = dhcp6cEntrySync2WanByWanIf(etyIdx - 1, newwanIf);  
		if(ret < 0){
			tcdbg_printf("[%s:%d]DHCPv6Client sync to wan fail\n",__FUNCTION__,__LINE__);	
			return -1;
		}

		commitWanByWanIf(newwanIf);
	}	
	return 0;
}

static cfg_node_ops_t cfg_type_dhcpv6client_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6client_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv6client_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV6_CLIENT_NUM_MAX, 
	 .parent = &cfg_type_dhcpv6client, 
	 .index = cfg_type_dhcpv6client_index,
	 .ops = &cfg_type_dhcpv6client_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_dhcpv6client_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6client_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv6client_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv6client, 
	 .ops = &cfg_type_dhcpv6client_common_ops, 
}; 


static cfg_node_ops_t cfg_type_dhcpv6client_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6client_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv6client_child[] = { 
	 &cfg_type_dhcpv6client_common, 
	 &cfg_type_dhcpv6client_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv6client = { 
	 .name = "DHCPv6Client", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv6client_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv6client_child, 
	 .ops = &cfg_type_dhcpv6client_ops, 
}; 
