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


int checkPtmLinkEnable(int entryIdx)
{
	char nodeName[32] = {0};
	char lowerlayer[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;

	if( entryIdx < 0 )
	{
		tcdbg_printf("[%s:%d] entryIdx=%d, error!\n",__FUNCTION__,__LINE__,entryIdx);
		return -1;
	}
	
	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	nextEntryIdx = getSuffixIntByInterface(lowerlayer) -1 ;
	if( nextEntryIdx < 0){
		return -1;
	}

	if(strstr(lowerlayer, TR181_DSL_CHANNEL_INTERFACE) != NULL)
	{
		
		/*the next layer is DSLChannel: no need to check*/	
		ret = checkDSLChannelEnable(nextEntryIdx);
	}

	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, entryIdx);

	if( (cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable)) >= 0)
		&& strcmp(enable, "1") == 0 && ret > 0)
	{
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}
	else
	{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
	
}

void setPTMLinkUpLayer(int ptmLinkIdx, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, ptmLinkIdx + 1);
	
	cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer);

	return;
}

void setPTMLinkLowerLayer(int ptmLinkIdx, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, ptmLinkIdx + 1);
	
	cfg_obj_set_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer);

	return;
}

int createPTMLinkEntry(int WanIF,int active){
	struct tr181_interface_s interface;
	int index = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_PTMLINK_NODE, 63);
	interface.num = PTM_LINK_NUM;
	interface.pvcIndex = WanIF;
	interface.lanIndex = -1;
	interface.active = active;
	index = createTR181InterfaceEntry(interface);
	
	return index;
}

int cfg_type_tr181_ptmlink_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int ptmLinkIdx = -1;
	int pvcIdx = -1;
	char tmp[32] = {0};
	int ret = -1;			
	char strInterface[256] = {0};
	char lowerlayers[64] = {0};
	int oldWanIf = -1;
	int newWanPvc = -1;
	int lowerPvc = -1;
	int ipIfIdx = -1;
	int enable = -1;
	int flag = -1;
	

	if ( get_entry_number_cfg2(path, "entry.", &ptmLinkIdx) != SUCCESS ) 
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "ptmlink_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			ptmLinkIdx = atoi(tmp) + 1;
		}
		else
		{
			return FAIL;
		}
	}

	if(ptmLinkIdx < 0)
	{
		tcdbg_printf("[%s:%d]atmLinkIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}
	
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_PTMLINK_NODE, ptmLinkIdx);

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, ptmLinkIdx);
	
	if((cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayers, sizeof(lowerlayers)) < 0)
		|| (strlen(lowerlayers) == 0) || strcmp(lowerlayers, TR181_DSL_CHANNEL_INTERFACE) != 0)
	{
		flag = 0;	
	}
	
	if(flag == 0)
	{	/* wan is not exist*/
		if(oldWanIf >= 0)
		{	/*old wan exist,need to delete  */
			unsetWanByPvcEnt(oldWanIf);
		}
		return 0;
	}

	ipIfIdx = getIpIFIdxByUplayer(nodeName);
	if(ipIfIdx < 0)
	{
		/*wan is not exist, do nothing*/
		return 0;
	}
	
	enable = checkIPIFEnableByIdx(ipIfIdx);
	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d]checkIPIFEnableByIdx : enable=%d!\n",__FUNCTION__,__LINE__,enable);
	}
	
	memset(wanNode, 0, sizeof(wanNode));	
	ret = setWanNodeByWanIf(wanNode, oldWanIf);
	if(enable == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{		
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}

	commitWanByWanIf(oldWanIf);

	return 0;
}

static cfg_node_ops_t cfg_type_tr181_ptmlink_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_ptmlink_func_commit
}; 


static cfg_node_type_t cfg_type_tr181_ptmlink_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | PTM_LINK_NUM, 
	 .parent = &cfg_type_tr181_ptmlink, 
	 .ops = &cfg_type_tr181_ptmlink_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181_ptmlink_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_ptmlink_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181_ptmlink_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181_ptmlink, 
	 .ops = &cfg_type_tr181_ptmlink_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181_ptmlink_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_ptmlink_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181_ptmlink_child[] = { 
	 &cfg_type_tr181_ptmlink_common, 
	 &cfg_type_tr181_ptmlink_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181_ptmlink = { 
	 .name = "PtmLink", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181_ptmlink_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181_ptmlink_child, 
	 .ops = &cfg_type_tr181_ptmlink_ops, 
}; 
