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

int checkEtherVTEnable(int entryIdx)
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
	memset(lowerlayer, 0, sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	nextEntryIdx = getSuffixIntByInterface(lowerlayer) -1 ;
	if( nextEntryIdx < 0)
	{
		return -1;
	}
	
	if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL)
	{
		/*the next layer is Link: for wan without vlan*/	
		ret = checkEtherLinkEnable(nextEntryIdx);
	}

	if(ret < 0)
	{
		return -1;
	}
	
	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	memset(lowerlayer, 0, sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, entryIdx);

	if( (cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, enable, sizeof(enable)) >= 0)
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

int getLowerLayerStackByEtherLink(struct lowerlayer_stack_s *lowerlayer_stack)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	int idx = 0;
	int ret = -1;

	/*check lowerlayer*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, lowerlayer_stack->EtherLinkIdx);
	if((cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		/*lowerlayer is not exist, wan connect info is not enough*/
		tcdbg_printf("[%s:%d]lowerlayer not exist\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		/*lowerlayer is exist, check next lowerlayer*/
		if(strstr(lowerlayer, TR181_BRIDGE_PORT) != NULL)
		{
			/*bridge port,for br0 or lan/wlan, no need to check*/		
		}
		else if(strstr(lowerlayer, TR181_OPTICAL_INTERFACE) != NULL)
		{
			/*OPTICAL,no next lowerlayer*/
			lowerlayer_stack->opticalIFIdx = 1;
			return 0;
		}
	}
	
	return ret;
}


void setEtherLinkUpLayer(int etherLinkIndex, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	
	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, etherLinkIndex + 1);
	cfg_obj_set_object_attr(nodeName, "uplayer", 0, uplayer);

	return;
}

void setEtherLinkLowerLayer(int etherLinkIndex, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, etherLinkIndex + 1);
	cfg_obj_set_object_attr(nodeName, "LowerLayers", 0, lowerlayer);

	return;
}

int findEtherLinkEntry(int wanIf, int lanIndex, int active)
{
	struct tr181_interface_s interface;
	int ret = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_ETHERLINK_NODE, 64);
	interface.num = ETHER_LINK_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = lanIndex;
	interface.active = active;

	ret = createTR181InterfaceEntry(interface);
	
	return ret;
}

int checkEtherLinkEnable(int entryIdx)
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
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	nextEntryIdx = getSuffixIntByInterface(lowerlayer);
	if( nextEntryIdx <= 0){
		return -1;
	}

	if(strstr(lowerlayer, TR181_OPTICAL_INTERFACE) != NULL)
	{
		
		/*the next layer is Optical: no need to check*/	
		ret = checkOpticalEnable();
	}
	else if(strstr(lowerlayer, TR181_BRIDGE_PORT) != NULL)
	{
	
		/*bridge port,for br0 or lan/wlan, no need to check*/	
		ret = 1;
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else if(strstr(lowerlayer, TR181_ATMLINK_INTERFACE) != NULL)
	{
		/*the next layer is ATMLink: for atmlink wan*/	
		ret = checkAtmLinkEnable(nextEntryIdx);
	}
	else if(strstr(lowerlayer, TR181_PTMLINK_INTERFACE) != NULL)
	{
		/*the next layer is PTMLink: for prmlink wan*/	
		ret = checkPtmLinkEnable(nextEntryIdx);
	}
#endif

	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, entryIdx);

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

void setDefaultBridgeWan(int wanIf)
{
	int pvcIdx = -1;
	int entIdx = -1;
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char wanPVCNode[MAXLEN_NODE_NAME] = {0};
	char attrValue[64] = {0};
	int ret = -1;

	if(wanIf < 0 || wanIf > WAN_INTERFACE_NUM)
	{
		tcdbg_printf("[%s:%d] wanPvc =%d, error!\n",__FUNCTION__,__LINE__,wanIf);
		return -1;
	}
	
	pvcIdx = wanIf/PVC_NUM;
	entIdx = wanIf%PVC_NUM;

	memset(wanPVCNode, 0, sizeof(wanPVCNode));
	snprintf(wanPVCNode, sizeof(wanPVCNode), WAN_PVC_NODE, pvcIdx + 1);

	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if (cfg_query_object(wanNode,NULL,NULL) < 0)
	{
		cfg_obj_create_object(wanNode);
	}
	
	if(cfg_obj_get_object_attr(wanPVCNode, "VLANMode", 0, attrValue, sizeof(attrValue)) < 0)
	{
		/*PVC is not exist, set default value*/	
		cfg_obj_set_object_attr(wanPVCNode, "Action", 0, "Add");
		cfg_obj_set_object_attr(wanPVCNode, "VLANID", 0, "4096");
		cfg_obj_set_object_attr(wanPVCNode, "DOT1P", 0, "0");
		cfg_obj_set_object_attr(wanPVCNode, "VLANMode", 0, "UNTAG");
		cfg_obj_set_object_attr(wanPVCNode, "ENCAP", 0, "1483 Bridged IP LLC");
	}


	setDefaultBridgeWanCfg(wanNode);

	return 0;
}

int EtherLinkCreateWan(int EtherLinkIdx)
{		
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char wanPVCNode[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int ret = -1;
	int natEnable = 0;
	int  wanIf = -1;
	char node[MAXLEN_NODE_NAME] = {0};
	int ipIFIdx = -1;
	int pvcIdx = -1;
	int entIdx = -1;

	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d] : EtherLinkIdx=%d!\n",__FUNCTION__,__LINE__,EtherLinkIdx);
	}
	/*check wan connection*/
	if(EtherLinkIdx <= 0)
	{
		tcdbg_printf("[%s:%d]EtherLinkIdx <= 0 \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*create route wan:get empty wan pvc index*/
	wanIf = findEmptyUntagWanPVCIdx();
	if(wanIf < 0)
	{
		tcdbg_printf("\r\nNo free wanIf for use");
		return -1;
	}	
	setDefaultBridgeWan(wanIf);
	
	/*set pvcIndex*/
	setPvcIdxByTR181NodeEntry(TR181_ETHERLINK_NODE, EtherLinkIdx, wanIf);
	
	/*check Enable*/
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, EtherLinkIdx);
	ipIFIdx = getIpIFIdxByUplayer(nodeName);
	ret = checkIPIFEnableByIdx(ipIFIdx);
	
	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d]checkIPIFEnableByIdx ret =%d, ipIFIdx=%d\n",
			__FUNCTION__,__LINE__,ret , EtherLinkIdx);
	}

	setWanNodeByWanIf(wanNode, wanIf);
	if(ret == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}

	return wanIf;
}

int cfg_type_etherlink_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char ipNode[MAXLEN_NODE_NAME] = {0};
	char xdslNode[MAXLEN_NODE_NAME] = {0};
	int ethlinkIfIdx = -1;
	char tmp[32] = {0};
	char *temp = {0};
	int ret = -1;	
	char lowerlayers[64] = {0};
	char uplayer[64] = {0};
	char iplowerlayers[64] = {0};
	char xdsluplayer[64] = {0};
	int oldWanIdx = -1;
	int newWanIdx = -1;
	int lowerPvc = -1;
	int lowerEnt = -1;
	int ipIfIdx = -1;
	int enable = -1;
	int flag = -1;
	int index = -1;

	if ( get_entry_number_cfg2(path,"entry.",&ethlinkIfIdx) != SUCCESS ) 
	{
		return -1;
	}

	if(ethlinkIfIdx < 1)
	{
		tcdbg_printf("[%s:%d]ethlinkIfIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}
		
	oldWanIdx = getPvcIdxByTR181NodeEntry(TR181_ETHERLINK_NODE, ethlinkIfIdx);

	/*flag : check lowerlayer is OPTICAL*/
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, ethlinkIfIdx);
	
#if defined(TCSUPPORT_CT_DSL_EX)
	/*set xdsl node uplayer by ethlink node lowerlayers*/
	cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayers, sizeof(lowerlayers));

	temp = lowerlayers + (strlen(lowerlayers) - 1);
	index = atoi(temp);

	if(strstr(lowerlayers, TR181_ATMLINK_INTERFACE) != NULL)
	{			
		memset(xdslNode, 0, sizeof(xdslNode));
		snprintf(xdslNode, sizeof(xdslNode), TR181_ATMLINK_ENTRY_NODE, index);
	}
	else if(strstr(lowerlayers, TR181_PTMLINK_INTERFACE) != NULL)
	{
		memset(xdslNode, 0, sizeof(xdslNode));
		snprintf(xdslNode, sizeof(xdslNode), TR181_PTMLINK_ENTRY_NODE, index);
	}
	else
	{	/* wan is not exist*/
		if(oldWanIdx >= 0)
		{	/*old wan exist,need to delete  */
			unsetWanByPvcEnt(oldWanIdx);
		}
		return 0;
	}
	
	memset(xdsluplayer, 0, sizeof(xdsluplayer));
	snprintf(xdsluplayer, sizeof(xdsluplayer), "%s%d", TR181_ETHERNET_LINK, ethlinkIfIdx);
#else
	if((cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayers, sizeof(lowerlayers)) < 0)
		|| (strlen(lowerlayers) == 0) || strcmp(lowerlayers, TR181_OPTICAL_INTERFACE) != 0)
	{
		flag = 0;	
	}
	
	if(flag == 0)
	{	/* wan is not exist*/
		if(oldWanIdx >= 0)
		{	/*old wan exist,need to delete  */
			unsetWanByPvcEnt(oldWanIdx);
		}
		return 0;
	}
#endif

	/* wan is  exist, need  to update wan*/
	if(oldWanIdx < 0)
	{
		tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
		/*old wan is not exist, create default wan*/
		newWanIdx = EtherLinkCreateWan(ethlinkIfIdx);
		cfg_obj_get_object_attr(nodeName, "uplayer", 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIdx);
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else
	{
		newWanIdx = getPvcIdxOfLowerLayer(TR181_ETHERLINK_NODE, ethlinkIfIdx);
		if(newWanIdx < 0)
		{
			newWanIdx = oldWanIdx;
		}
		else if(newWanIdx != oldWanIdx)
		{
			/*lowerlayer modify: set old Wan Active to Disable*/
			memset(wanNode, 0, sizeof(wanNode));
			ret = setWanNodeByWanIf(wanNode, oldWanIdx);
			if(ret >= 0)
			{
				/*wan set to  disable*/
				cfg_obj_set_object_attr(wanNode, "Active", 0, "No");			
				commitWanByWanIf(oldWanIdx);
			}
			
			/*clear all old pvcindex by uplayer*/
			setPVCIndexByUpLayer(lowerlayers, -1);
			
			if(strstr(lowerlayers, TR181_ATMLINK_INTERFACE) != NULL)
			{			
				/*clear loweryer's uplayer to NULL*/
				SetNextOldUpLayerByWanIf(TR181_ATMLINK_ENTRY_NODE, ATM_LINK_NUM, xdsluplayer);
			}
			else if(strstr(lowerlayers, TR181_PTMLINK_INTERFACE) != NULL)
			{
				/*clear loweryer's uplayer to NULL*/
				SetNextOldUpLayerByWanIf(TR181_PTMLINK_ENTRY_NODE, PTM_LINK_NUM, xdsluplayer);
			}
		}

	}

	/*set new uplayer and set pvcIndex by uplayer*/	
	cfg_obj_set_object_attr(xdslNode, TR181_UPLAYER_ATTR, 0, xdsluplayer);
	setPVCIndexByUpLayer(lowerlayers, newWanIdx);
	
#else
	else
	{
		/*old wan exist*/
		newWanIdx = oldWanIdx;
	}
#endif

	ipIfIdx = getIpIFIdxByUplayer(nodeName);
	if(ipIfIdx > 0)
	{
		SyncWanByTR181IPIf(ipIfIdx, newWanIdx);
		
		memset(ipNode, 0, sizeof(ipNode));
		memset(iplowerlayers, 0, sizeof(iplowerlayers));
		snprintf(ipNode, sizeof(ipNode), TR181_IP_ENTRY_NODE, ipIfIdx);
		if((cfg_obj_get_object_attr(ipNode, "LowerLayers", 0, iplowerlayers, sizeof(iplowerlayers)) >= 0)
			&& (strstr(iplowerlayers, TR181_PPP_INTERFACE) != NULL))
		{
			memset(wanNode, 0, sizeof(wanNode));
			setWanNodeByWanIf(wanNode, newWanIdx);
			cfg_obj_set_object_attr(wanNode, "ISP", 0, "2");
			cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
			cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkPPP");
		}
	}
	
	enable = checkIPIFEnableByIdx(ipIfIdx);

	memset(wanNode, 0, sizeof(wanNode));	
	ret = setWanNodeByWanIf(wanNode, newWanIdx);
	if(enable == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{		
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}

	commitWanByWanIf(newWanIdx);
	
	return 0;
}

static cfg_node_ops_t cfg_type_etherlink_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_etherlink_commit 
}; 


static cfg_node_type_t cfg_type_etherlink_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | ETHER_LINK_NUM, 
	 .parent = &cfg_type_etherlink, 
	 .ops = &cfg_type_etherlink_entry_ops, 
};

static cfg_node_ops_t cfg_type_etherlink_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_etherlink_commit
}; 


static cfg_node_type_t cfg_type_etherlink_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_etherlink, 
	 .ops = &cfg_type_etherlink_common_ops, 
}; 


static cfg_node_ops_t cfg_type_etherlink_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_etherlink_commit 
}; 


static cfg_node_type_t* cfg_type_etherlink_child[] = { 
	 &cfg_type_etherlink_common, 
	 &cfg_type_etherlink_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_etherlink = { 
	 .name = "EtherLink", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_etherlink_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_etherlink_child, 
	 .ops = &cfg_type_etherlink_ops, 
}; 
