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

int checkAtmLinkEnable(int entryIdx)
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
	snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	nextEntryIdx = getSuffixIntByInterface(lowerlayer);
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
	snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, entryIdx);

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


void setATMLinkUpLayer(int atmLinkIdx, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, atmLinkIdx + 1);
	
	cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer);

	return;
}

void setATMLinkLowerLayer(int atmLinkIdx, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, atmLinkIdx + 1);
	
	cfg_obj_set_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer);

	return;
}

int createATMLinkEntry(int WanIf,int active)
{
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char wanPVCNode[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	struct tr181_interface_s interface;
	int index = -1;
	int ret = -1;
	char tmp[32] = {0};
	char vpi[8] = {0};
	char vci[8] = {0};

	memset(&interface, 0, sizeof(interface));	
	strncpy(&interface.nodeName, TR181_ATMLINK_NODE, 63);
	interface.num = ATM_LINK_NUM;
	interface.pvcIndex = WanIf;
	interface.lanIndex = -1;
	interface.active = active;
	index = createTR181InterfaceEntry(interface);
	
	/*set other IP.Interface info, eg. IPv4Enable,IPv6Enable*/	
	memset(wanPVCNode, 0, sizeof(wanPVCNode));
	snprintf(wanPVCNode, sizeof(wanPVCNode), WAN_PVC_NODE, (WanIf/PVC_NUM) + 1);

	memset(wanNode, 0, sizeof(wanNode));	
	ret = setWanNodeByWanIf(wanNode, WanIf);
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", interface.nodeName, index + 1);
	
	memset(vpi, 0, sizeof(vpi));	
	memset(vci, 0, sizeof(vci));	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "VPI", 0, vpi, sizeof(vpi));
	cfg_obj_get_object_attr(wanPVCNode, "VCI", 0, vci, sizeof(vci));
	cfg_obj_set_object_attr(nodeName, "VPI", 0, vpi);
	cfg_obj_set_object_attr(nodeName, "VCI", 0, vci);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "QOS", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "QOS", 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "PCR", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "PCR", 0, tmp);
	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "SCR", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "SCR", 0, tmp);
			
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "MBS", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "MBS", 0, tmp);
	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanPVCNode, "ENCAP", 0, tmp, sizeof(tmp));
	if(strcmp(tmp, "1483 Bridged IP LLC") == 0 
		|| strcmp(tmp, "PPPoE LLC") == 0 
		|| strcmp(tmp, "PPPoA LLC") == 0
		|| strcmp(tmp, "1483 Bridged Only LLC") == 0)
	{
		cfg_obj_set_object_attr(nodeName, "ENCAP", 0, "LLC");
	}
	else if(strcmp(tmp, "1483 Bridged IP VC-Mux") == 0 
		|| strcmp(tmp, "1483 Routed IP VC-Mux") == 0 
		|| strcmp(tmp, "PPPoE VC-Mux") == 0 
		|| strcmp(tmp, "PPPoA VC-Mux") == 0
		|| strcmp(tmp, "1483 Bridged Only VC-Mux") == 0)
	{
		cfg_obj_set_object_attr(nodeName, "ENCAP", 0, "VCMUX");
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, "ENCAP", 0, " ");
	}
	
	if(strcmp("1483 Bridged Only VC-Mux", tmp) == 0 
		|| strcmp("1483 Bridged Only LLC", tmp) == 0 
		|| strcmp("PPPoE VC-Mux", tmp) == 0 
		|| strcmp("PPPoE LLC", tmp) == 0 
		|| strcmp("1483 Bridged IP LLC", tmp) == 0 
		|| strcmp("1483 Bridged IP VC-Mux", tmp) == 0)
	{
		cfg_obj_set_object_attr(nodeName, "LinkType", 0, "EoA");
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, "LinkType", 0, " ");
	}

			
	return index;
}

int tr181AtmLinkSync2Wan(int entryIdx, int WanIf)
{
	char atmLinkNode[MAXLEN_NODE_NAME] = {0}; 
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char wanPVC[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};
	int isp = 0;
	char linkType[20] = {0};
	char encap[128] = {0};
	int ret = -1;
	int pvcIdx = -1;

	
	if(entryIdx < 0 || WanIf < 0)
	{
		return 0;
	}
	
	memset(atmLinkNode, 0, sizeof(atmLinkNode));
	snprintf(atmLinkNode, sizeof(atmLinkNode), TR181_ATMLINK_ENTRY_NODE, entryIdx);

	ret = setWanNodeByWanIf(wanNode, WanIf);

	pvcIdx = WanIf/PVC_NUM;
	
	memset(wanPVC, 0, sizeof(wanPVC));
	snprintf(wanPVC, sizeof(wanPVC), "root.wan.pvc.%d", pvcIdx + 1);

	/*VPI/VCI Set*/ 
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "VPI", 0, tmp, sizeof(tmp)) >= 0) 
	{
		cfg_obj_set_object_attr(wanPVC, "VPI", 0, tmp);
	}
	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "VCI", 0, tmp, sizeof(tmp)) >= 0)  
	{
		cfg_obj_set_object_attr(wanPVC, "VCI", 0, tmp);
	}

	/*ATM qos*/ 	
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "QOS", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(wanPVC, "QOS", 0, tmp);
	}

	/*PCR*/
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "PCR", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(wanPVC, "PCR", 0, tmp);
	}

	/*MBS*/
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "MBS", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(wanPVC, "MBS", 0, tmp);
	}
	
	/*SCR*/
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(atmLinkNode, "SCR", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(wanPVC, "SCR", 0, tmp);
	}

	/*LinkType, ENCAP*/
	memset(linkType, 0, sizeof(linkType));	
	if(cfg_obj_get_object_attr(atmLinkNode, "LinkType", 0, linkType, sizeof(linkType)) >= 0)
	{
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(wanNode, "ISP", 0, tmp, sizeof(tmp));
		isp = atoi(tmp);
		
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(atmLinkNode, "ENCAP", 0, encap, sizeof(encap));
		if(strcmp(linkType, "EoA") == 0)
		{/*EoA*/
			switch(isp)
			{
				case WAN_ENCAP_STATIC_INT:	
				case WAN_ENCAP_DYN_INT: 				
					if(strcmp(encap,"VCMUX") == 0)
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "1483 Bridged IP VC-Mux");
					}
					else
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "1483 Bridged IP LLC");
					}
					cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
					cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkIP");
					break;
				case WAN_ENCAP_PPP_INT: 				
					if(strcmp(encap,"VCMUX") == 0)
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "PPPoE VC-Mux");
					}
					else
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "PPPoE LLC");
					}
					cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
					cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkPPP");
					cfg_obj_set_object_attr(wanNode, "PPPGETIP", 0, "Dynamic");
					break;
				case WAN_ENCAP_BRIDGE_INT:	
					if(strcmp(encap,"VCMUX") == 0)
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "1483 Bridged Only VC-Mux");
					}
					else
					{
						cfg_obj_set_object_attr(wanPVC, "ENCAP", 0, "1483 Bridged Only LLC");
					}
					cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Bridge");
					cfg_obj_set_object_attr(wanNode, "BridgeMode", 0, "PPPoE_Bridged");
					break;
			}
		}
	}	
	
	return 0;
}

int cfg_type_tr181_atmlink_func_commit(char *path)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	int atmLinkIdx = -1;
	char tmp[32] = {0};
	int ret = -1;		
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char strInterface[256] = {0};
	char uplayer[64] = {0};
	char lowerlayers[64] = {0};
	int oldWanIf = -1;
	int newWanIf = -1;
	int lowerPvc = -1;
	int ipIfIdx = -1;
	int enable = -1;
	int flag = -1;
	
	if ( get_entry_number_cfg2(path, "entry.", &atmLinkIdx) != SUCCESS ) 
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "atmLink_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			atmLinkIdx = atoi(tmp) + 1;
		}
		else
		{
			return FAIL;
		}
	}
	
	if(atmLinkIdx < 0)
	{
		tcdbg_printf("[%s:%d]atmLinkIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}

	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_ATMLINK_NODE, atmLinkIdx);

	if(oldWanIf < 0)
	{
		/*wan is not exist, do nothing*/
		return 0;
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, atmLinkIdx);
	
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
	
	tr181AtmLinkSync2Wan(atmLinkIdx, oldWanIf);

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

static cfg_node_ops_t cfg_type_tr181_atmlink_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_atmlink_func_commit
}; 


static cfg_node_type_t cfg_type_tr181_atmlink_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | ATM_LINK_NUM, 
	 .parent = &cfg_type_tr181_atmlink, 
	 .ops = &cfg_type_tr181_atmlink_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ip_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_atmlink_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181_atmlink_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181_atmlink, 
	 .ops = &cfg_type_tr181ip_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181_atmlink_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_atmlink_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181_atmlink_child[] = { 
	 &cfg_type_tr181_atmlink_common, 
	 &cfg_type_tr181_atmlink_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181_atmlink = { 
	 .name = "AtmLink", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181_atmlink_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181_atmlink_child, 
	 .ops = &cfg_type_tr181_atmlink_ops, 
}; 
