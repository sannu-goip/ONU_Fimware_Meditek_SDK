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

void tr181PPPNodeSync2Wan(int pppIdx, int pvcIdx)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char WanPVC[MAXLEN_NODE_NAME] = {0};	
	char usrname[64] = {0};
	char password[64] = {0};
	char WANCnt1_1CntTrigger[32] = {0};
	char mru[8] = {0};
	int ret= -1;
	char discontime[32] = {0};
	char isp[4] = {0};
	char encap[128] = {0};
	int pvcIndex = -1;

	/*get value*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, pppIdx);

	cfg_obj_get_object_attr(nodeName, "Username", 0, usrname, sizeof(usrname));
	cfg_obj_get_object_attr(nodeName, "Password", 0, password, sizeof(password));
	cfg_obj_get_object_attr(nodeName, "ConnTrigger", 0, WANCnt1_1CntTrigger, sizeof(WANCnt1_1CntTrigger));
	cfg_obj_get_object_attr(nodeName, "MRU", 0, mru, sizeof(mru));
	cfg_obj_get_object_attr(nodeName, "AutoDisconnTime", 0, discontime, sizeof(discontime));
	if(mru[0] == '\0')
		snprintf(mru, sizeof(mru), "%d", 0);

	/*Sync Value*/
	memset(nodeName, 0, sizeof(nodeName));
	ret = setWanNodeByWanIf(nodeName, pvcIdx);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	cfg_obj_set_object_attr(nodeName, "USERNAME", 0, usrname);
	cfg_obj_set_object_attr(nodeName, "PASSWORD", 0, password);
	cfg_obj_set_object_attr(nodeName, "BridgeType", 0, "PPP");
	cfg_obj_set_object_attr(nodeName, "PPPGETIP", 0, "Dynamic");
	cfg_obj_set_object_attr(nodeName, "MTU", 0, mru);
	
	cfg_obj_get_object_attr(nodeName, "ISP", 0, isp, sizeof(isp));
	if(atoi(isp) != WAN_ENCAP_BRIDGE_INT)
	{
		memset(WanPVC, 0, sizeof(WanPVC));
		pvcIndex = pvcIdx/PVC_NUM;
		snprintf(WanPVC, sizeof(WanPVC), "root.wan.pvc.%d", pvcIndex + 1);
		
		memset(encap, 0, sizeof(encap));
		cfg_obj_get_object_attr(WanPVC, "ENCAP", 0, encap, sizeof(encap));

		/*change bridge mode to route mode when default wan mode is bridge*/
		if(strcmp(encap, "1483 Bridged Only VC-Mux") == 0)
		{
			cfg_obj_set_object_attr(WanPVC, "ENCAP", 0, "PPPoE VC-Mux");
		}
		else if(strcmp(encap, "1483 Bridged Only LLC") == 0)
		{
			cfg_obj_set_object_attr(WanPVC, "ENCAP", 0, "PPPoE LLC");
		}
		else
		{
			/*do nothing*/
		}
		
		cfg_obj_set_object_attr(nodeName, "WanMode", 0, "Route");
		cfg_obj_set_object_attr(nodeName, "LinkMode", 0, "linkPPP");
		cfg_obj_set_object_attr(nodeName, "ISP", 0, "2");
	}
	
	/*set Connection trigger*/	
	if(!strcmp(WANCnt1_1CntTrigger, "AlwaysOn")) 
	{
		cfg_obj_set_object_attr(nodeName, "CONNECTION", 0, "Connect_Keep_Alive");
		cfg_obj_set_object_attr(nodeName, "CLOSEIFIDLE", 0, "0");
	}
	else if(!strcmp(WANCnt1_1CntTrigger, "OnDemand")) 
	{
		cfg_obj_set_object_attr(nodeName, "CONNECTION", 0, "Connect_on_Demand");
		if( !discontime[0] )
			snprintf(discontime, sizeof(discontime), "%d", 0);
		cfg_obj_set_object_attr(nodeName, "CLOSEIFIDLE", 0, discontime);
	}
	else if(!strcmp(WANCnt1_1CntTrigger, "Manual")) 
	{
		cfg_obj_set_object_attr(nodeName, "CONNECTION", 0, "Connect_Manually");
		cfg_obj_set_object_attr(nodeName, "CLOSEIFIDLE", 0, "0");
	}
	else 
	{
		/*wrong Connection trigger value!*/
	}

	return;
}

int checkPPPEnable(int entryIdx)
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
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	
	nextEntryIdx = getSuffixIntByInterface(lowerlayer) -1 ;
	if( nextEntryIdx < 0)
	{
		return 0;
	}
	
	if(strstr(lowerlayer, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		/*the next layer is VLANTermination: for wan with vlan*/	
		ret = checkEtherVTEnable(nextEntryIdx);
	}
	else if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL)
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
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, entryIdx);

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

int getLowerLayerStackByPPPIf(struct lowerlayer_stack_s *lowerlayer_stack)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	int idx = 0;
	int ret = -1;

	/*check lowerlayer*/	
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, lowerlayer_stack->pppIFIdx);
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
		if(strstr(lowerlayer, TR181_ETHERNET_VLANTERMINATION) != NULL){
			/*the next layer is VLANTermination: for wan with vlan*/				
			sscanf(lowerlayer + strlen(TR181_ETHERNET_VLANTERMINATION), "%d", &idx);	
			lowerlayer_stack->vlanTIdx = idx;
			tcdbg_printf("[%s:%d] idx=%d\n",__FUNCTION__,__LINE__,idx);
			ret = getLowerLayerStackByVlanT(lowerlayer_stack);

		}else if(strstr(lowerlayer,TR181_ETHERNET_LINK) != NULL){
			/*the next layer is Link: for wan without vlan*/		
			sscanf(lowerlayer + strlen(TR181_ETHERNET_LINK), "%d", &idx);
			lowerlayer_stack->EtherLinkIdx = idx;
			tcdbg_printf("[%s:%d] idx=%d\n",__FUNCTION__,__LINE__,idx);
			ret = getLowerLayerStackByEtherLink(lowerlayer_stack);
		}

	}
	return ret;
}

void setPPPInterfaceUpLayer(int pppIndex, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, pppIndex + 1);
	cfg_obj_set_object_attr(nodeName, "uplayer", 0, uplayer);
	
	return;
}

void setPPPInterfaceLowerLayer(int pppIndex, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, pppIndex + 1);
	cfg_obj_set_object_attr(nodeName, "LowerLayers", 0, lowerlayer);
	
	return;
}

int wanSync2TR181PPPIfNode(int index, int wanIf)
{	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[64] = {0};
	char discontime[32] = {0};
	int ret = -1;
	
	if(index < 0 || wanIf < 0)
	{
		tcdbg_printf("[%s:%d]index < 0 || pvcIndex < 0 error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	
	memset(nodeName, 0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, index + 1);
	
	memset(tmp, 0, sizeof(tmp));	
	cfg_obj_get_object_attr(wanNode, "USERNAME", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "Username", 0, tmp);

	memset(tmp, 0, sizeof(tmp));	
	cfg_obj_get_object_attr(wanNode, "PASSWORD", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "Password", 0, tmp);

	memset(tmp, 0, sizeof(tmp));	
	cfg_obj_get_object_attr(wanNode, "MTU", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "MRU", 0, tmp);

	memset(tmp, 0, sizeof(tmp));	
	cfg_obj_get_object_attr(wanNode, "CONNECTION", 0, tmp, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "CLOSEIFIDLE", 0, discontime, sizeof(discontime));
	if(!strcmp(tmp, "Connect_Keep_Alive")) 
	{
		cfg_obj_set_object_attr(nodeName, "ConnTrigger", 0, "AlwaysOn");
		cfg_obj_set_object_attr(nodeName, "AutoDisconnTime", 0, "0");
	}
	else if(!strcmp(tmp, "Connect_on_Demand")) 
	{
		cfg_obj_set_object_attr(nodeName, "ConnTrigger", 0, "OnDemand");
		cfg_obj_set_object_attr(nodeName, "AutoDisconnTime", 0, discontime);
	}
	else if(!strcmp(tmp, "Connect_Manually")) 
	{
		cfg_obj_set_object_attr(nodeName, "ConnTrigger", 0, "Manual");
		cfg_obj_set_object_attr(nodeName, "AutoDisconnTime", 0, "0");
	}

	return 0;
}

int findPPPInterfaceEntry(int wanIf, int active)
{	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[64] = {0};
	struct tr181_interface_s interface;
	int index = -1;	

	if(wanIf < 0 )
	{
		tcdbg_printf("[%s:%d]pvcIndex < 0 \n",__FUNCTION__,__LINE__);
		return 0;
	}

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_PPP_NODE, 64);
	interface.num = PPP_INTERFACE_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	if(active == 1)
	{
		interface.active = active;
	}
	
	index = createTR181InterfaceEntry(interface);
	if(index < 0)
	{
		tcdbg_printf("[%s:%d]index < 0!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	wanSync2TR181PPPIfNode(index, wanIf);	
	
	return index;
}

int cfg_type_tr181ppp_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char pppIFNode[MAXLEN_NODE_NAME] = {0};
	char IpIFNode[MAXLEN_NODE_NAME] = {0};
	int pppIfIdx = -1;
	char tmp[256] = {0};
	int ret = -1;
	int oldWanIf = -1;
	int newWanIf  = -1;
	int ipIfIdx = -1;
	int enable = -1;
	int i = 0;
	char lowerlayer[256] = {0};
	int nextEntryIdx = -1;
	char uplayer[256] = {0};
	
	if ( get_entry_number_cfg2(path, "entry.", &pppIfIdx) != SUCCESS ) 
	{
		return -1;
	}

	if(pppIfIdx < 1)
	{
		tcdbg_printf("[%s:%d]pppIfIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}

	/*check wan by lowerlayer*/	
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_PPP_NODE, pppIfIdx);
	newWanIf = getPvcIdxOfLowerLayer(TR181_PPP_NODE, pppIfIdx);

	/*get lowlayer*/
	memset(nodeName, 0, sizeof(nodeName));	
	memset(lowerlayer, 0, sizeof(lowerlayer));
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, pppIfIdx);
	cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer, sizeof(lowerlayer));

	ipIfIdx = getIpIFIdxByUplayer(nodeName);
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
		snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_PPP_INTERFACE, pppIfIdx);
		ClearOldUpLayer(uplayer);
	}

	/*set new uplayer and set pvcIndex by uplayer*/
	memset(uplayer, 0, sizeof(uplayer));
	snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_PPP_INTERFACE, pppIfIdx);
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
	}else
	{
		/*sync ppp info*/
		tr181PPPNodeSync2Wan(pppIfIdx, newWanIf);
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

static cfg_node_ops_t cfg_type_tr181ppp_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ppp_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ppp_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | PPP_INTERFACE_NUM, 
	 .parent = &cfg_type_tr181ppp, 
	 .ops = &cfg_type_tr181ppp_entry_ops, 
};

static cfg_node_ops_t cfg_type_tr181ppp_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ppp_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ppp_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181ppp, 
	 .ops = &cfg_type_tr181ppp_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ppp_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ppp_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ppp_child[] = { 
	 &cfg_type_tr181ppp_common, 
	 &cfg_type_tr181ppp_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ppp = { 
	 .name = "TR181PPP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ppp_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ppp_child, 
	 .ops = &cfg_type_tr181ppp_ops, 
}; 
