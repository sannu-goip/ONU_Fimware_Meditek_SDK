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

int natEntrySync2WanByWanPvc(int index, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char natEnable[20]={0};
	int ret = -1;
	int wanPvc = -1; 
	int wanEnt = -1;

	if(index < 1)
	{
		tcdbg_printf("[%s:%d]index=%d < 0, error!\n",__FUNCTION__,__LINE__,index);
		return -1;
	}

	if(wanIf < 0)
	{
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_NAT_INTERFACE_ENTRY_NODE, index);
	cfg_obj_get_object_attr(nodeName, "Enable", 0, natEnable, sizeof(natEnable));

	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*set NAT*/
	if(strcmp(natEnable, "1") == 0)
	{
		cfg_obj_set_object_attr(wanNode, "NATENABLE", 0, "Enable");
	}
	else
	{
		cfg_obj_set_object_attr(wanNode, "NATENABLE", 0, "Disabled");
	}
		
	return 0;
}

int natSync2WanByTR181IPIf(int ipIfIdx ,int wanIf)
{
	char tr181IPNode[MAXLEN_NODE_NAME] = {0};	
	char pvcBuf[20]={0};
	int wanPvc = -1;
	int wanEnt = -1;
	int index = -1;
	int ret = -1;

	/*Find NatInterface Entry index by TR181IP Index*/
	index = getTR181NodeEntryIdxByIPIf(TR181_NAT_INTERFACE_NODE, NAT_INTERFACE_NUM, ipIfIdx);
	if(index < 0)
	{
		/*not find NatInerface*/		
		tcdbg_printf("[%s:%d]index=%d < 0, error!\n",__FUNCTION__,__LINE__,index);
		return -1;
	}

	if(wanIf < 0){
		tcdbg_printf("[%s:%d]wanIf=%d < 0, error!\n",__FUNCTION__,__LINE__,wanIf);
		return -1;
	}
	
	ret = natEntrySync2WanByWanPvc(index, wanIf);
	return ret;
}

int findNATInterfaceEntry(int wanIf, int active)
{
	struct tr181_interface_s interface;
	int index = -1;

	memset(&interface,0,sizeof(interface));	
	strcpy(&interface.nodeName,TR181_NAT_INTERFACE_NODE);
	interface.num = NAT_INTERFACE_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	index = createTR181InterfaceEntry(interface);
	
	return index;
}

int cfg_type_natInterface_func_commit(char* path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	int natIdx = -1;	
	int ipIfIdx = -1;
	char tmp[256] = {0};
	int ret = -1;
	int newWanIf= -1;
	int oldWanIf = -1;
	char node[MAXLEN_NODE_NAME] = {0};
	char pvcIdx[8] = {0};
	int i = 0;
	
	if( get_entry_number_cfg2(path,"entry.",&natIdx) != SUCCESS )
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "nat_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			natIdx = atoi(tmp) + 1;
		}
		else
		{
			return FAIL;
		}
	}

	if(natIdx < 1)
	{
		tcdbg_printf("[%s:%d]natIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}
	
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_NAT_INTERFACE_NODE, natIdx);

	ipIfIdx = getIPIfByTR181Node(TR181_NAT_INTERFACE_NODE, natIdx);
	if(ipIfIdx < 0)
	{
		tcdbg_printf("[%s:%d]interface is not set or error, no need to sync\n",__FUNCTION__,__LINE__);			
		setPvcIdxByTR181NodeEntry(TR181_NAT_INTERFACE_NODE, natIdx, -1);
	}
	else
	{
		newWanIf =getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	}
	
	if(newWanIf != oldWanIf)
	{
		if(oldWanIf >= 0)
		{
			memset(wanNode, 0, sizeof(wanNode));
			ret = setWanNodeByWanIf(wanNode, oldWanIf);
			if(ret < 0)
			{		
				tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
				return -1;
			}

			cfg_obj_set_object_attr(wanNode, "NATENABLE", 0, "Disabled");
			commitWanByWanIf(oldWanIf);
		}
	
		/*new wan exist and change, need clear old wan first, clear pvcIndex*/
		clearOtherIneterfaceByWanIf(TR181_NAT_INTERFACE_NODE, NAT_INTERFACE_NUM, natIdx, newWanIf); 
	}
	
	if(newWanIf >= 0)
	{
		/*Sync new WanPvc*/	
		setPvcIdxByTR181NodeEntry(TR181_NAT_INTERFACE_NODE, natIdx, newWanIf);
		ret = natEntrySync2WanByWanPvc(natIdx, newWanIf);
		if(ret < 0)
		{
			tcdbg_printf("[%s:%d]natSync2Wan fail\n",__FUNCTION__,__LINE__);	
			return -1;
		}

		commitWanByWanIf(newWanIf);
	}
	
	return 0;
}

static cfg_node_ops_t cfg_type_natinterface_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natInterface_func_commit 
}; 


static cfg_node_type_t cfg_type_natinterface_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | NAT_INTERFACE_NUM, 
	 .parent = &cfg_type_natinterface, 
	 .ops = &cfg_type_natinterface_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_natinterface_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natInterface_func_commit 
}; 


static cfg_node_type_t cfg_type_natinterface_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_natinterface, 
	 .ops = &cfg_type_natinterface_common_ops, 
}; 


static cfg_node_ops_t cfg_type_natinterface_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natInterface_func_commit 
}; 


static cfg_node_type_t* cfg_type_natinterface_child[] = { 
	 &cfg_type_natinterface_common, 
	 &cfg_type_natinterface_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_natinterface = { 
	 .name = "NatInterface", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_natinterface_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_natinterface_child, 
	 .ops = &cfg_type_natinterface_ops, 
}; 
