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

int dhcp4cEntrySync2WanByWanPvc(int index, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char strISP[20]={0};
	char enable[20]={0};
	int ret = -1;
	

	if(index < 0)
	{
		tcdbg_printf("[%s:%d]index=%d < 0, error!\n",__FUNCTION__,__LINE__,index);
		return -1;
	}

	if(wanIf < 0){
		return -1;
	}
	
	memset(enable, 0, sizeof(enable));	
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DHCPV4CLIENT_ENTRY_NODE, index);
	cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable));

	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

    if(!strcmp(enable, "1"))
	{
		memset(strISP, 0, sizeof(strISP));
		cfg_obj_get_object_attr(wanNode, "ISP", 0, strISP, sizeof(strISP));
		if(!strcmp(strISP, "1"))
		{
			cfg_obj_set_object_attr(wanNode, "ISP", 0, "0");
			cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
			cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkIP");
		}
	}
	else
	{/*disable */
		memset(strISP, 0, sizeof(strISP));
		cfg_obj_get_object_attr(wanNode, "ISP", 0, strISP, sizeof(strISP));
		if(!strcmp(strISP, "0"))
		{
			cfg_obj_set_object_attr(wanNode, "ISP", 0, "1");
		}
	}	
	
	return 0;
}

int dhcp4cSync2WanByTR181IPIf(int ipIfIdx, int wanIf)
{
	char tr181IPNode[MAXLEN_NODE_NAME] = {0};	
	char pvcBuf[20]={0};
	int index = -1;
	int ret = -1;

	if(ipIfIdx < 0){
		return -1;
	}

	
	if(wanIf < 0){
		return -1;
	}

	/*Find NatInterface Entry index by TR181IP Index*/
	index = getTR181NodeEntryIdxByIPIf(TR181_DHCPV4_CLIENT_NODE, DHCPV4_CLIENT_NUM_MAX, ipIfIdx);
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

	
	ret = dhcp4cEntrySync2WanByWanPvc(index, wanIf);
	return ret;
}

void setDhcpv4ClientEnableState(int wanIf, int flag) 
{
	int i = 0;	
	char tmp[32] = {0};
	int tr181pvcIndex = -1;
	int tr181entIndex = -1;
	char dhcpv4node[MAXLEN_NODE_NAME] = {0};
	int pvcIndex = -1;
	int entIndex = -1;
	
	/*check  TR181_DHCPV4_CLIENT pvcIdx*/
	for( i =0;i <= DHCPV4_CLIENT_NUM_MAX; i++)
	{
		memset(dhcpv4node,0,sizeof(dhcpv4node));
		snprintf(dhcpv4node, sizeof(dhcpv4node), TR181_DHCPV4CLIENT_ENTRY_NODE, i + 1);
		cfg_obj_get_object_attr(dhcpv4node, "pvcIndex", 0, tmp, sizeof(tmp));
		if(isNumber(tmp) == wanIf)
			{
				if(flag  == 1 )
					cfg_obj_set_object_attr(dhcpv4node, "Enable", 0, "1");
				else
					cfg_obj_set_object_attr(dhcpv4node, "Enable", 0, "0");
			}	
		}
	}

int findDhcpv4ClientEntry(int wanIf, int active)
{
	struct tr181_interface_s interface;
	int ret = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName,TR181_DHCPV4_CLIENT_NODE, CFG_BUF_64_LEN - 1);
	interface.num = DHCPV4_CLIENT_NUM_MAX;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	ret = createTR181InterfaceEntry(interface);
	
	return ret;
}

int cfg_type_DHCPv4Client_func_commit(char *path)
{
	int etyIdx = -1;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int oldWanIf = -1;	
	int newWanIf = -1;
	int ipIfIdx = -1;
	int ret = -1;
	char strISP[20]={0};
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		return -1;
	}

	if(etyIdx < 0){
		tcdbg_printf("[%s:%d]etyIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}
	
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_DHCPV4_CLIENT_NODE, etyIdx);
	ipIfIdx = getIPIfByTR181Node(TR181_DHCPV4_CLIENT_NODE,  etyIdx);  
	
	newWanIf =getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	if(oldWanIf != newWanIf)
	{
		/*interface is modified,set old wan to static and clear old dhcpv4client entry*/
		ret = setWanNodeByWanIf(wanNode, oldWanIf);
		if(ret < 0)
		{		
			tcdbg_printf("[%s:%d]old wan is not exist, do nothing!\n",__FUNCTION__,__LINE__);
		}
		else
		{
			/*set wanNode to static*/
			cfg_obj_set_object_attr(wanNode, "ISP", 0, "1");
			commitWanByWanIf(oldWanIf);
		}
		clearOtherIneterfaceByWanIf(TR181_DHCPV4_CLIENT_NODE, DHCPV4_CLIENT_NUM_MAX, etyIdx, newWanIf);	
	}

	if(newWanIf < 0){
		tcdbg_printf("[%s:%d]newWanIf = %d, error\n",__FUNCTION__,__LINE__, newWanIf);	
		setPvcIdxByTR181NodeEntry(TR181_DHCPV4_CLIENT_NODE, etyIdx, -1);
		return -1;
	}
	
	/*Sync new WanPvc*/ 
	setPvcIdxByTR181NodeEntry(TR181_DHCPV4_CLIENT_NODE, etyIdx, newWanIf);
	ret = dhcp4cEntrySync2WanByWanPvc(etyIdx, newWanIf);  
	if(ret < 0){
		tcdbg_printf("[%s:%d]dhcp4cEntrySync2WanByWanPvc fail\n",__FUNCTION__,__LINE__);	
		return -1;
	}
	commitWanByWanIf(newWanIf);
	return 0;	
}

static cfg_node_ops_t cfg_type_dhcpv4client_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_DHCPv4Client_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4client_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV4_CLIENT_NUM_MAX, 
	 .parent = &cfg_type_dhcpv4client, 
	 .ops = &cfg_type_dhcpv4client_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_dhcpv4client_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_DHCPv4Client_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4client_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv4client, 
	 .ops = &cfg_type_dhcpv4client_common_ops, 
}; 


static cfg_node_ops_t cfg_type_dhcpv4client_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_DHCPv4Client_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4client_child[] = { 
	 &cfg_type_dhcpv4client_common, 
	 &cfg_type_dhcpv4client_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4client = { 
	 .name = "DHCPv4Client", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4client_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4client_child, 
	 .ops = &cfg_type_dhcpv4client_ops, 
}; 
