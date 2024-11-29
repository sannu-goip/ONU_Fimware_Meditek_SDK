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

int tr181EtherVlanTSync2Wan(int entryIdx, int pvcIdx)
{
	char ethvlantIFNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};
	int ret = -1;

	
	if(entryIdx < 0 || pvcIdx < 0){
		tcdbg_printf("[%s:%d]entryIdx=%d,pvcIdx=%d error!\n",__FUNCTION__,__LINE__,entryIdx,pvcIdx );
		return 0;
	}
	
	memset(ethvlantIFNode, 0, sizeof(ethvlantIFNode));
	snprintf(ethvlantIFNode, sizeof(ethvlantIFNode), TR181_ETHERVLANT_ENTRY_NODE, entryIdx);
	
	/*Sync Value*/
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, pvcIdx);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*"Device.Ethernet.VLANTermination.  exist,  set wan vlan enable */	
	cfg_obj_set_object_attr(wanNode, "dot1q", 0, "Yes");
	cfg_obj_set_object_attr(wanNode, "VLANMode", 0, "TAG");

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(ethvlantIFNode, "VLAN", 0, tmp, sizeof(tmp)) < 0)
		|| strlen(tmp) == 0) 
	{
		tcdbg_printf("[%s:%d] vlanid is not exist\n",__FUNCTION__,__LINE__);
		cfg_obj_set_object_attr(wanNode, "VLANID", 0, "0");
	}
	else 
	{
		cfg_obj_set_object_attr(wanNode, "VLANID", 0, tmp);
	}

	memset(tmp,0, sizeof(tmp));
	if((cfg_obj_get_object_attr(ethvlantIFNode, "TPID", 0, tmp, sizeof(tmp)) < 0)
		|| strlen(tmp) == 0) 
	{
		tcdbg_printf("[%s:%d] tpid is not exist\n",__FUNCTION__,__LINE__);
		cfg_obj_set_object_attr(wanNode, "TPID", 0, "0");
	}
	else 
	{
		cfg_obj_set_object_attr(wanNode, "TPID", 0, tmp);
	}

	return 0;
}

int getLowerLayerStackByVlanT(struct lowerlayer_stack_s *lowerlayer_stack)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	int idx = 0;
	int ret = -1;

	/*check lowerlayer*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, lowerlayer_stack->vlanTIdx);
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		/*lowerlayer is not exist, wan connect info is not enough*/
		tcdbg_printf("[%s:%d]lowerlayer not exist\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		/*lowerlayer is exist, check next lowerlayer*/
		if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL)
		{
			sscanf(lowerlayer + strlen(TR181_ETHERNET_LINK), "%d", &idx);			
			lowerlayer_stack->EtherLinkIdx = idx ;
			ret = getLowerLayerStackByEtherLink(lowerlayer_stack);
		}
	}
	return ret;
}

void setEtherVlanTermUpLayer(int vlanTIndex, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, vlanTIndex + 1);
	cfg_obj_set_object_attr(nodeName, "uplayer", 0, uplayer);

	return;
}
void setEtherVlanTermLowerLayer(int vlanTIndex, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, vlanTIndex + 1);
	cfg_obj_set_object_attr(nodeName, "LowerLayers", 0, lowerlayer);

	return;
}

int wanSync2tr181EtherVlanT(int entryIdx, int wanIf)
{
	char ethvlantIFNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char dot1q[32] = {0};
	char vlanid[32] = {0};
	char tpid[32] = {0};
	char tmp[32] = {0};
	int ret = -1;

	if(wanIf < 0 || entryIdx < 0)
	{
		tcdbg_printf("[%s:%d]wanIf=%d error!\n",__FUNCTION__,__LINE__,wanIf );
		return 0;
	}

	/*Sync Value*/
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(ethvlantIFNode, 0, sizeof(ethvlantIFNode));
	snprintf(ethvlantIFNode, sizeof(ethvlantIFNode), TR181_ETHERVLANT_ENTRY_NODE, entryIdx + 1);

	memset(dot1q, 0, sizeof(dot1q));
	if ((cfg_obj_get_object_attr(wanNode, "dot1q", 0, dot1q, sizeof(dot1q)) >= 0)
		&& !strcmp(dot1q, "Yes")) 
	{		
		memset(vlanid, 0, sizeof(vlanid));
		if (cfg_obj_get_object_attr(wanNode, "VLANID", 0, vlanid, sizeof(vlanid)) >= 0) 
		{
			cfg_obj_set_object_attr(ethvlantIFNode, "VLAN", 0, vlanid);
		}
		else
		{
			cfg_obj_set_object_attr(ethvlantIFNode, "VLAN", 0, "0");
		}

		memset(tpid, 0, sizeof(tpid));
		if (cfg_obj_get_object_attr(wanNode, "TPID", 0, tpid, sizeof(tpid)) >= 0) 
		{
			cfg_obj_set_object_attr(ethvlantIFNode, "TPID", 0, tpid);
		}
		else
		{
			cfg_obj_set_object_attr(ethvlantIFNode, "TPID", 0, "0");
		}
	}

	return 0;
}

int findEtherVlanTermEntry(int wanIf, int active)
{
	struct tr181_interface_s interface;
	int ret = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_ETHERVLANT_NODE, 64);
	interface.num = ETHER_VLAN_T_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	ret = createTR181InterfaceEntry(interface);
	
	return ret;
}

int cfg_type_ethervlant_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char EtherVlanTIFNode[MAXLEN_NODE_NAME] = {0};
	char pppIFNode[MAXLEN_NODE_NAME] = {0};
	char IpIFNode[MAXLEN_NODE_NAME] = {0};
	char strInterface[256] = {0};
	char uplayer[256] = {0};
	char lowerlayer[256] = {0};
	int ethvlantIfIdx = -1;
	char tmp[32] = {0};
	int ret = -1;
	int enable = -1;
	int oldWanIf = -1;
	int newWanIf = -1;
	int lowerPvc = -1;
	int ipIfIdx = -1;
	int pppIfIdx = -1;
	int i = 0;
	char uplayerOut[256] = {0};
	
	if ( get_entry_number_cfg2(path, "entry.", &ethvlantIfIdx) != SUCCESS ) 
	{
		return -1;
	}
	
	if(ethvlantIfIdx < 1)
	{
		tcdbg_printf("[%s:%d]ethvlantIfIdx is error, no need to sync\n",__FUNCTION__,__LINE__); 
		return -1;
	}
	
	/*check wan by lowerlayer*/	
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_ETHERVLANT_NODE, ethvlantIfIdx);
	newWanIf = getPvcIdxOfLowerLayer(TR181_ETHERVLANT_NODE, ethvlantIfIdx);

	
	/*get ipIf Index by uplayer*/	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, ethvlantIfIdx);
	ipIfIdx = getIpIFIdxByUplayer(nodeName);

	cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer, sizeof(lowerlayer));

	if(newWanIf != oldWanIf)
	{	/*lowerlayer modify: set old Wan Active to Disable*/
		memset(wanNode, 0, sizeof(wanNode));
		ret = setWanNodeByWanIf(wanNode, oldWanIf);
		if(ret >= 0)
		{
			/*wan set to  disable*/
			cfg_obj_set_object_attr(wanNode, "Active", 0, "No");			
			commitWanByWanIf(oldWanIf);
		}

		/*clear all old pvcindex by uplayer*/
		setPVCIndexByUpLayer(lowerlayer, -1);
					
		/*clear loweryer' uplayer to NULL*/
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_ETHERNET_VLANTERMINATION, ethvlantIfIdx);
		ClearOldUpLayer(uplayer);
	}

	/*set new uplayer and set pvcIndex by uplayer*/
	memset(uplayer, 0, sizeof(uplayer));
	snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_ETHERNET_VLANTERMINATION, ethvlantIfIdx);
	SetNextUpLayerByLowerLayer(lowerlayer, uplayer);
	setPVCIndexByUpLayer(lowerlayer, newWanIf);
		
	/*sync new WanInfo*/
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, newWanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	
	if(newWanIf != oldWanIf)
	{
		/*Wan modify, need to sync all info by uplayer.  to new wan*/
		sync2WanByUpLayer(uplayer, newWanIf);
	}else{
		/*sync vlan info*/
		tr181EtherVlanTSync2Wan(ethvlantIfIdx, newWanIf);
	}
	
	enable = checkIPIFEnableByIdx(ipIfIdx); 
	if(enable == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}
		
	commitWanByWanIf(newWanIf);
	
	return 0;
}


static cfg_node_ops_t cfg_type_ethervlant_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ethervlant_func_commit 
}; 


static cfg_node_type_t cfg_type_ethervlant_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | ETHER_VLAN_T_NUM, 
	 .parent = &cfg_type_ethervlant, 
	 .ops = &cfg_type_ethervlant_entry_ops, 
};

static cfg_node_ops_t cfg_type_ethervlant_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ethervlant_func_commit 
}; 


static cfg_node_type_t cfg_type_ethervlant_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_ethervlant, 
	 .ops = &cfg_type_ethervlant_common_ops, 
}; 


static cfg_node_ops_t cfg_type_ethervlant_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ethervlant_func_commit 
}; 


static cfg_node_type_t* cfg_type_ethervlant_child[] = { 
	 &cfg_type_ethervlant_common, 
	 &cfg_type_ethervlant_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_ethervlant = { 
	 .name = "EtherVlanT", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_ethervlant_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_ethervlant_child, 
	 .ops = &cfg_type_ethervlant_ops, 
}; 
