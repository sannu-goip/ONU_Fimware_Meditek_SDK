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

static char* cfg_type_firewallch_pvc_index[] = { 
	 "pvc_Idx", 
	 NULL 
}; 


int updateFireWallChainRule(void)
	{
	updateFireWallChByIpmacfilter();
	updateFireWallChByAclfilter();
		return 0;
	}
	

int clearFirewallChRuleByNodeFlag(int nodeFlag)
{
	if(nodeFlag ==  IPMACFILTER_FLAG)	
{
		if(cfg_query_object(TR181_FIREWALLCH_PVC_0_NODE,NULL,NULL) >= 0)
	{
			cfg_obj_delete_object(TR181_FIREWALLCH_PVC_0_NODE);
	}

		if(cfg_query_object(TR181_FIREWALLCH_PVC_1_NODE,NULL,NULL) >= 0)
	{	
			cfg_obj_delete_object(TR181_FIREWALLCH_PVC_1_NODE);
	}

		if(cfg_query_object(TR181_FIREWALLCH_PVC_2_NODE,NULL,NULL) >= 0)
	{
			cfg_obj_delete_object(TR181_FIREWALLCH_PVC_2_NODE);
	}	
	}
	else if(nodeFlag ==  ACLFILTER_FLAG)
	{
		if(cfg_query_object(TR181_FIREWALLCH_PVC_3_NODE,NULL,NULL) >= 0)
	{
			cfg_obj_delete_object(TR181_FIREWALLCH_PVC_3_NODE);
		}
	}
		
	return 0;
}

int updateFireWallChByIpmacfilter(void)	
	{
	clearFirewallChRuleByNodeFlag(IPMACFILTER_FLAG);
	
	syncFirewallChEnableByNodeFlag(IPMACFILTER_FLAG);
	
	syncMacRuleToTr181Firewallch(MAC_FILTER_RULE_FLAG);
	syncIpRuleToTr181Firewallch(IP_UP_FILTER_RULE_FLAG);
	syncIpRuleToTr181Firewallch(IP_DOWN_FILTER_RULE_FLAG);

	return 0;
		}

int updateFireWallChByAclfilter(void)
		{
	clearFirewallChRuleByNodeFlag(ACLFILTER_FLAG);
	
	syncFirewallChEnableByNodeFlag(ACLFILTER_FLAG);
	
	syncAclRuleToTr181Firewallch(ACL_FILTER_RULE_FLAG);

	return 0;
		}

int  updateFireWallChByNodeFlag(int nodeFlag)
		{
	char buf[8] = {0};
	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, buf, sizeof(buf));
	if(strcmp(buf,"1"))
		{
		if(nodeFlag ==IPMACFILTER_FLAG)
		{
			updateFireWallChByIpmacfilter();
		}
		else if(nodeFlag ==ACLFILTER_FLAG)
		{
			updateFireWallChByAclfilter();
		}
	}

	return 0;
}


int syncIpMacFilterActiveByPvcIdx(int pvcIdx)
{
	char enable[32] = {0};
	
	if(pvcIdx == MAC_FILTER_RULE_FLAG)
	{
		if(cfg_obj_get_object_attr(TR181_FIREWALLCH_PVC_0_NODE, "Enable", 0, enable, sizeof(enable)) >= 0 && enable[0] != '\0')
		{
			cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ActiveMac", 0, enable);
		}	
		
		/* Current setting is Mac Filter*/
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "Mac");
		cfg_tr181_commit_object(IPMACFILTER_NODE);
	}
	else if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		if(cfg_obj_get_object_attr(TR181_FIREWALLCH_PVC_1_NODE, "Enable", 0, enable, sizeof(enable)) >=0 && enable[0] != '\0')
		{
			cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortOut", 0, enable);
		}
		/* Current setting is IpUp Filter*/
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "IpUp"); 
		cfg_tr181_commit_object(IPMACFILTER_NODE);
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		if(cfg_obj_get_object_attr(TR181_FIREWALLCH_PVC_2_NODE, "Enable", 0, enable, sizeof(enable)) >=0 && enable[0] != '\0')
		{
			cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortIn", 0, enable);
		}	
		/* Current setting is IpUp Filter*/
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "IpDown"); 
		cfg_tr181_commit_object(IPMACFILTER_NODE);
	}
	else if(pvcIdx == ACL_FILTER_RULE_FLAG)
	{
		if(cfg_obj_get_object_attr(TR181_FIREWALLCH_PVC_3_NODE, "Enable", 0, enable, sizeof(enable)) >=0 && enable[0] != '\0')
		{
			if(atoi(enable) == 1)
			{
				cfg_obj_set_object_attr(ACL_COMMON_NODE, "Activate", 0, "Yes");
			}
			else
			{
				cfg_obj_set_object_attr(ACL_COMMON_NODE, "Activate", 0, "No");
			}
		}
		cfg_tr181_commit_object(ACL_NODE);
	}

	return 0;

}

int syncFirewallChEnableByNodeFlag(int nodeFlag)
{
	char active[32] = {0};

	if(nodeFlag == IPMACFILTER_FLAG)
	{
	memset(active, 0, sizeof(active));
	if(cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActiveMac", 0, active, sizeof(active)) < 0 || active[0] == '\0')
	{
		strncpy(active, "0", sizeof(active));
	}
	if(cfg_query_object(TR181_FIREWALLCH_PVC_0_NODE,NULL,NULL) < 0){
		if(cfg_create_object(TR181_FIREWALLCH_PVC_0_NODE) < 0){
			return -1;
		}
	}
	cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_0_NODE, "Enable", 0, active);
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_0_NODE, "Alias", 0, "MacFilterChain");

	memset(active, 0, sizeof(active));
	if(cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortOut", 0, active, sizeof(active)) < 0 || active[0] == '\0')
	{
		strncpy(active, "0", sizeof(active));
	}
	if(cfg_query_object(TR181_FIREWALLCH_PVC_1_NODE,NULL,NULL) < 0){
		if(cfg_create_object(TR181_FIREWALLCH_PVC_1_NODE) < 0){
			return -1;
		}
	}
	cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_1_NODE, "Enable", 0, active);
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_1_NODE, "Alias", 0, "IpUpFilterChain");

	memset(active, 0, sizeof(active));
	if(cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortIn", 0, active, sizeof(active)) < 0 || active[0] == '\0')
	{
		strncpy(active, "0", sizeof(active));
	}
	
	if(cfg_query_object(TR181_FIREWALLCH_PVC_2_NODE,NULL,NULL) < 0){
		if(cfg_create_object(TR181_FIREWALLCH_PVC_2_NODE) < 0){
			return -1;
		}
	}
	cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_2_NODE, "Enable", 0, active);
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_2_NODE, "Alias", 0, "IpDownFilterChain");
	
	}
	else if (nodeFlag == ACLFILTER_FLAG)
	{
		memset(active, 0, sizeof(active));
		if(cfg_obj_get_object_attr(ACL_COMMON_NODE, "Activate", 0, active, sizeof(active)) < 0 || active[0] == '\0'  || strcmp(active, "Yes"))
		{
			strncpy(active, "0", sizeof(active));
		}
		else 
		{
			strncpy(active, "1", sizeof(active));
		}

		if(cfg_query_object(TR181_FIREWALLCH_PVC_3_NODE,NULL,NULL) < 0){
			if(cfg_create_object(TR181_FIREWALLCH_PVC_3_NODE) < 0){
				return -1;
			}
		}
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_3_NODE, "Enable", 0, active);
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_3_NODE, "Alias", 0, "AclFilterChain");
	}
	return 0;
}


int setFirewallChListTypeByPvcIdx(int pvcIdx, char*listType, int size)
{
	char tmp[32] = {0};

	memset(tmp, 0, sizeof(tmp));
	if(pvcIdx == MAC_FILTER_RULE_FLAG)
	{
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeMac", 0, tmp, sizeof(tmp));
	}
	else if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpUp", 0, tmp, sizeof(tmp)); 
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpDown", 0, tmp, sizeof(tmp));
	}

	if(!strcasecmp("white", tmp))
	{
		strncpy(listType, "Accept", size - 1);
	}
	else
	{
		strncpy(listType, "Drop", size - 1);	 
	}	

	return 0;		
}

int setIpMacFilterListTypeByPvcIdx(int pvcIdx, char*listType)
{
	char tmp[32] = {0};

	if(!strcasecmp(listType, "Accept"))
	{
		strncpy(tmp, "white" ,sizeof(tmp)-1);
	}
	else
	{
		strncpy(tmp, "black" ,sizeof(tmp)-1);
	}
	
	if(pvcIdx == MAC_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeMac", 0, tmp);
	}
	else if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpUp", 0, tmp);
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpDown", 0, tmp);
	}
	
	return 0;		
}


int getWanIfNamebyTr181IfLink(char*wanIfName, char*tr181IfLink,int size)
{
	int ipIndex = -1;
	int pvcNum = -1;
	int entryNum = -1;
	int pvcIndex = -1;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	
	if(wanIfName == NULL ||tr181IfLink == NULL || size <= 0)
	{
		return -1;
	}
	
	ipIndex = getSuffixIntByInterface(tr181IfLink);
	pvcIndex = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIndex);
	memset(nodeName, 0, sizeof(nodeName));
	setWanNodeByWanIf(nodeName,pvcIndex);

	if(cfg_obj_get_object_attr(nodeName, "IFName", 0, wanIfName, size) < 0)
	{
		return -1;
	}

	return 0;
}

int getTr181IfLinkByWanIfName(char*wanIfName, char*tr181IfLink,int size)
{
	int i = 0;
	int j = 0;
	int ipIndex = -1;
	int pvcNum = -1;
	int entryNum = -1;
	int pvcIndex = -1;
	char strPvcIndex[32]= {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char IFName[32] = {0};
	
	if(wanIfName == NULL || tr181IfLink == NULL || size <= 0)
	{
		return -1;
	}

	/*get pvcIndex by matching IFname */
	for(i = 1; i <= PVC_NUM; i++)
	{
		for(j= 1; j <= MAX_WAN_ENTRY_NUMBER; j++)
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, i, j);
			if(cfg_obj_get_object_attr(nodeName, "IFName", 0, IFName, sizeof(IFName)) >= 0 &&  !strcmp(wanIfName, IFName))
			{
				pvcNum = i - 1;
				entryNum = j -1;
				pvcIndex = pvcNum * PVC_NUM + entryNum;
				break;
			}
		}
	}

	/*get ipIndex by matching pvcIndex */
	for(i = 1; i <= IP_INTERFACE_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, i);
		if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, strPvcIndex, sizeof(strPvcIndex)) >= 0 &&  strPvcIndex[0] != '\0')
		{
			if(atoi(strPvcIndex) == pvcIndex)
			{
				ipIndex = i;
				break;
			}
		}
	}

	snprintf(tr181IfLink, size, TR181_IP_INTERFACE_ENTRY, ipIndex);
	
	return 0;
}


int syncMacRuleToTr181Firewallch(int pvcIdx)
{
	char ipMacFilterNode[MAXLEN_NODE_NAME] = {0};
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char listType[32] = {0};
	int i = 0;
	char tmp[256] = {0};
	int ret = -1;
	int entryNum = 0;
	time_t timep;

	if(pvcIdx != MAC_FILTER_RULE_FLAG)
	{
		return -1;
	}
	
	for(i = 1; i<=TR181_FIREWALL_CHAIN_ENTRY_NUM; i++)
	{
		memset(ipMacFilterNode, 0, sizeof(ipMacFilterNode));
		memset(tr181Node, 0, sizeof(tr181Node));
		snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, i);
		snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, i);
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(ipMacFilterNode, "Active", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
		{	
			if (cfg_obj_query_object(tr181Node, NULL, NULL) < 0){
				ret = cfg_obj_create_object(tr181Node);
				if(ret < 0){
					return -1;
				}
			}
			
			entryNum++;
			
			/*FirewallCh_pvc%d_entry%d Enable */
			if(strcmp("Yes", tmp) == 0)
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "1");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Enabled");
			}
			else
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "0");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Disabled");
			}
				
			/*MTK_MACAddress*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "MacAddr", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				cfg_obj_set_object_attr(tr181Node, "MTKMACAddr", 0, tmp);
			}					
		
			/*MTK_RuleType*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "RuleType", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				cfg_obj_set_object_attr(tr181Node, "MTKRuleType", 0, tmp);
			}

			/*Targer*/
			memset(tmp, 0, sizeof(tmp));
			setFirewallChListTypeByPvcIdx(pvcIdx, tmp, sizeof(tmp));
			cfg_obj_set_object_attr(tr181Node, "Target", 0, tmp);	 

			/*CreationDate ,needed??*/	
			memset(tmp, 0, sizeof(tmp));
			datetimeFormat(time(&timep), tmp, sizeof(tmp));
			cfg_obj_set_object_attr(tr181Node, "CreationDate", 0, tmp);
		}
	}

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", entryNum);
	cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_0_NODE, "Num", 0, tmp);
	
	return 0;
}


int syncIpRuleToTr181Firewallch(int pvcIdx)
{
	char ipMacFilterNode[MAXLEN_NODE_NAME] = {0};
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	int i = 0, entryNum = 0;
	char tmp[256] = {0};
	char targer[20] = {0};
	int ret = -1;
	time_t timep;
	char tr181IfLink[32] = {0};

	if(pvcIdx != IP_UP_FILTER_RULE_FLAG && pvcIdx != IP_DOWN_FILTER_RULE_FLAG)
	{
		return -1;	
	}
	
	for(i = 1; i<=TR181_FIREWALL_CHAIN_ENTRY_NUM; i++)
	{
		memset(tr181Node, 0, sizeof(tr181Node));
		memset(ipMacFilterNode, 0, sizeof(ipMacFilterNode));
		snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, i);
		if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
		{	
			snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, i + MAX_MAC_FILTER_RULE);
		}
		else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
		{
			snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, i + MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE );
		}
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(ipMacFilterNode, "Active", 0, tmp, sizeof(tmp)) >= 0)
		{	
			if (cfg_obj_query_object(tr181Node, NULL, NULL) < 0){
				ret = cfg_obj_create_object(tr181Node);
				if(ret < 0){
					return -1;
				}
			}

			entryNum++;
			
			/*FirewallCh_pvc%d_Entry%d Enable */
			if(strcmp("Yes", tmp) == 0)
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "1");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Enabled");
			}
			else
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "0");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Disabled");
			}
		
			/* SourceIP*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "SrcIPAddr", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "SourceIP", 0, tmp);
			}
		
			/* SourceMask*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "SrcIPMask", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "SourceMask", 0, tmp);
			}
		
			/* SourcePort*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "SrcPort", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "SourcePort", 0, tmp);
			}
		
			/* DestIP*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "DesIPAddr", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "DestIP", 0, tmp);
			}
		
			/*	DestMask*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "DesIPMask", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "DestMask", 0, tmp);
			}
		
			/*DestPort*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "DesPort", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "DestPort", 0, tmp);
			}
			
			/*MTK_Protocol*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "Protocol", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "MTKProtoc", 0, tmp);
			}
						
			/*MTK_Direction*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "Direction", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "MTKDirect", 0, tmp);
			}
		
			/*MTK_RuleType*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(ipMacFilterNode, "RuleType", 0, tmp, sizeof(tmp)) >= 0)
			{
				cfg_obj_set_object_attr(tr181Node, "MTKRuleType", 0, tmp);
			}
			
			/* MTK_Interface*/
			if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
			{
				memset(tmp, 0, sizeof(tmp));
				if(cfg_obj_get_object_attr(ipMacFilterNode, "Interface", 0, tmp, sizeof(tmp)) >= 0)
				{
					getTr181IfLinkByWanIfName(tmp, tr181IfLink, sizeof(tr181IfLink));
					cfg_obj_set_object_attr(tr181Node, "MTKInter", 0, tr181IfLink);
				}
			}

			/*Targer*/
			memset(tmp, 0, sizeof(tmp));
			setFirewallChListTypeByPvcIdx(pvcIdx, tmp, sizeof(tmp));
			cfg_obj_set_object_attr(tr181Node, "Target", 0, tmp);	 
			
			/*CreationDate, needed??*/	
			memset(tmp, 0, sizeof(tmp));
			datetimeFormat(time(&timep), tmp, sizeof(tmp));
			cfg_obj_set_object_attr(tr181Node, "CreationDate", 0, tmp);
		}
	}

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", entryNum);
	if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_1_NODE, "Num", 0, tmp);
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_2_NODE, "Num", 0, tmp);
	}
	
	return 0;
}


int syncAclRuleToTr181Firewallch(int pvcIdx)
{
	char aclNodeName[MAXLEN_NODE_NAME] = {0};
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[256] = {0};
	int ret = -1;
	int entryNum = 0;
	time_t timep;

	if(pvcIdx != ACL_FILTER_RULE_FLAG)
	{
		return -1;
}

	for(i = 1; i<=TR181_FIREWALL_CHAIN_ENTRY_NUM; i++)
{
		memset(aclNodeName, 0, sizeof(aclNodeName));
		memset(tr181Node, 0, sizeof(tr181Node));
		snprintf(aclNodeName, sizeof(aclNodeName), ACL_ENTRY_N_NODE, i);
		snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, i);

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(aclNodeName, "Activate", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
		{	
			if (cfg_obj_query_object(tr181Node, NULL, NULL) < 0){
				ret = cfg_obj_create_object(tr181Node);
				if(ret < 0){
		return -1;
	}
			}
	
			entryNum++;

			/*FirewallCh_pvc%d_entry%d Enable */
			if(strcmp("Yes", tmp) == 0)
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "1");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Enabled");
		}		
			else
			{
				cfg_obj_set_object_attr(tr181Node, "Enable", 0, "0");
				cfg_obj_set_object_attr(tr181Node, "Status", 0, "Disabled");
	}

			/*SourceIP*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(aclNodeName, "ScrIPAddrBegin", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				cfg_obj_set_object_attr(tr181Node, "SourceIP", 0, tmp);
		}		
		
			/*DestIP*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(aclNodeName, "ScrIPAddrEnd", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				cfg_obj_set_object_attr(tr181Node, "DestIP", 0, tmp);
	}

			/*MTKApp*/
	memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(aclNodeName, "Application", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				cfg_obj_set_object_attr(tr181Node, "MTKApp", 0, tmp);
	}

			/*MTKInter*/
	memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(aclNodeName, "Interface", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
				string_toUper(tmp);
				cfg_obj_set_object_attr(tr181Node, "MTKInter", 0, tmp);
			}					
	
			/*CreationDate*/	
			memset(tmp, 0, sizeof(tmp));
			datetimeFormat(time(&timep), tmp, sizeof(tmp));
			cfg_obj_set_object_attr(tr181Node, "CreationDate", 0, tmp);
	}
	}

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", entryNum);
	cfg_obj_set_object_attr(TR181_FIREWALLCH_PVC_3_NODE, "Num", 0, tmp);

	return 0;
}


int syncMacRuleToIpMacFilter(int pvcIdx, int entryIdx)
{
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char ipMacFilterNode[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};
	int ret = -1;

	if(pvcIdx != MAC_FILTER_RULE_FLAG || entryIdx < 0 || entryIdx > TR181_FIREWALL_CHAIN_ENTRY_NUM)
	{
		return -1;
	}
	
	snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, entryIdx);
	snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, entryIdx);

	/* Current setting is Mac Filter*/
	cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "Mac");
	
	if(cfg_query_object(ipMacFilterNode,NULL,NULL) < 0){
		ret = cfg_create_object(ipMacFilterNode);
		if(ret < 0){
			return -1;
		}
		else{
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "No");
			cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "Action", 0, "Add");
		}
	}
	else{
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "Action", 0, "Mod");	
	}
	
	/*FirewallCh_pvc%d_entry%d Enable */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "Enable", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		if(atoi(tmp) == 1){ 	
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "Yes");
		}else{
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "No");
		}
	}

	/*MacAddr */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "MTKMACAddr", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "MacAddr", 0, tmp);
	}

	/*Target */
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(tr181Node, "Target", 0, tmp, sizeof(tmp));
	setIpMacFilterListTypeByPvcIdx(pvcIdx, tmp);
	

	/* The following is default setting */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(ipMacFilterNode, "MacName", 0, tmp, sizeof(tmp)) < 0 || tmp[0]== '\0')
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "MacRule%d", entryIdx);
		cfg_obj_set_object_attr(ipMacFilterNode, "MacName", 0, tmp);	
	}
	cfg_obj_set_object_attr(ipMacFilterNode, "RuleType", 0, "MAC");
	cfg_obj_set_object_attr(ipMacFilterNode, "Direction", 0, "Incoming");
	cfg_obj_set_object_attr(ipMacFilterNode, "Interface", 0, "br0");
	
	cfg_tr181_commit_object(ipMacFilterNode);
	
	return 0;
}


int syncIPRuleToIpMacFilter(int pvcIdx, int entryIdx)
{
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char ipMacFilterNode[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};
	int ret = -1;
	char wanIfName[32] = {0};

	if((pvcIdx != IP_UP_FILTER_RULE_FLAG && pvcIdx != IP_DOWN_FILTER_RULE_FLAG) || entryIdx < 0 || entryIdx > TR181_FIREWALL_CHAIN_ENTRY_NUM)
	{
		return -1;
	}
	
	snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, entryIdx);
	if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, entryIdx + MAX_MAC_FILTER_RULE);
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "IpUp"); /* Current setting is IpUp Filter*/
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		snprintf(ipMacFilterNode, sizeof(ipMacFilterNode), IPMACFILTER_ENTRY_N_NODE, entryIdx + MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE);
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, "IpDown"); /* Current setting is IpDown Filter*/
	}
		
	if(cfg_query_object(ipMacFilterNode,NULL,NULL) < 0){
		ret = cfg_create_object(ipMacFilterNode);
		if(ret < 0){
			return -1;
		}
		else{
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "No");
			cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "Action", 0, "Add");
		}
	}
	else{
		cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "Action", 0, "Mod");	
	}

	/*FirewallCh_pvc%d_Entry%d  Enable */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "Enable", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		if(atoi(tmp) == 1){ 	
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "Yes");
		}else{
			cfg_obj_set_object_attr(ipMacFilterNode, "Active", 0, "No");
		}
	}

	/* Protocol*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "MTKProtoc", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "Protocol", 0, tmp);
	}

	/* SrcIPAddr*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "SourceIP", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "SrcIPAddr", 0, tmp);
	}

	/* SrcIPMask*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "SourceMask", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "SrcIPMask", 0, tmp);
	}

	/* SrcPort*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "SourcePort", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "SrcPort", 0, tmp);
	}

	/* DesIPAddr*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "DestIP", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "DesIPAddr", 0, tmp);
	}

	/* DesIPMask*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "DestMask", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "DesIPMask", 0, tmp);
	}

	/* DesPort*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "DestPort", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "DesPort", 0, tmp);
	}

	/* Target*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(tr181Node, "Target", 0, tmp, sizeof(tmp));
	setIpMacFilterListTypeByPvcIdx(pvcIdx, tmp);

	/* Interface*/
	if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "Interface", 0, "br0");
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(tr181Node, "MTKInter", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
		{
			getWanIfNamebyTr181IfLink(wanIfName, tmp, sizeof(wanIfName));
		}
		cfg_obj_set_object_attr(ipMacFilterNode, "Interface", 0, wanIfName);
	}

	/* default setting*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(ipMacFilterNode, "IPName", 0, tmp, sizeof(tmp)) < 0 || tmp[0]== '\0')
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "IPRule%d", entryIdx);
		cfg_obj_set_object_attr(ipMacFilterNode, "IPName", 0, tmp);	
	}
	
	cfg_obj_set_object_attr(ipMacFilterNode, "RuleType", 0, "IP");
	if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{	
		cfg_obj_set_object_attr(ipMacFilterNode, "Direction", 0, "Outgoing");
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		cfg_obj_set_object_attr(ipMacFilterNode, "Direction", 0, "Incoming");
	}
	
	cfg_tr181_commit_object(ipMacFilterNode);
	
	return 0;
}

int syncAclRuleToACLFilter(int pvcIdx, int entryIdx)
{
	int ret = -1;
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char aclNodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[32] = {0};

	if(pvcIdx != ACL_FILTER_RULE_FLAG || entryIdx < 0 || entryIdx > TR181_FIREWALL_CHAIN_ENTRY_NUM - 1){
		return -1;
	}

	snprintf(tr181Node, sizeof(tr181Node), TR181_FIREWALLCH_PVC_ENTRY_NODE, pvcIdx, entryIdx);
	snprintf(aclNodeName, sizeof(aclNodeName), ACL_ENTRY_N_NODE, entryIdx);

	if(cfg_query_object(aclNodeName,NULL,NULL) < 0){
		ret = cfg_create_object(aclNodeName);
		if(ret < 0){
			return -1;
		}
		else{
			cfg_obj_set_object_attr(aclNodeName, "Activate", 0, "No");
			cfg_obj_set_object_attr(ACL_COMMON_NODE, "Action", 0, "Add");
		}
	}
	else{
		cfg_obj_set_object_attr(ACL_COMMON_NODE, "Action", 0, "Mod");	
}


	/*FirewallCh_pvc%d_entry%d Enable */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "Enable", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
{
		if(atoi(tmp) == 1){ 	
			cfg_obj_set_object_attr(aclNodeName, "Activate", 0, "Yes");
		}else{
			cfg_obj_set_object_attr(aclNodeName, "Activate", 0, "No");
		}
	}

	/* ScrIPAddrBegin*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "SourceIP", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(aclNodeName, "ScrIPAddrBegin", 0, tmp);
	}
	else
		{
		cfg_obj_set_object_attr(aclNodeName, "ScrIPAddrBegin", 0, "0.0.0.0");
		}

	/* ScrIPAddrEnd*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "DestIP", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(aclNodeName, "ScrIPAddrEnd", 0, tmp);
	}
	else
	{
		cfg_obj_set_object_attr(aclNodeName, "ScrIPAddrEnd", 0, "0.0.0.0");
	}

	/* Application*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "MTKApp", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(aclNodeName, "Application", 0, tmp);
	}
	else
		{
		cfg_obj_set_object_attr(aclNodeName, "Application", 0, "WEB");
	}

	/* Interface*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, "MTKInter", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
			{
		string_FirstCharToUp(tmp);
		cfg_obj_set_object_attr(aclNodeName, "Interface", 0, tmp);
			}
			else
			{
		cfg_obj_set_object_attr(aclNodeName, "Interface", 0, "Wan");
			}
	
	/* The following is default setting */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(aclNodeName, "ACLName", 0, tmp, sizeof(tmp)) < 0 || tmp[0]== '\0')
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "ACLRule%d", entryIdx);
		cfg_obj_set_object_attr(aclNodeName, "ACLName", 0, tmp);	
			}

	cfg_tr181_commit_object(aclNodeName);
	return 0;
		}


int cfg_type_firewallch_pvc_entry_func_commit(char* path)
{
	int idx = -1, pvcIdx = -1, entryIdx = -1;

	if(get_entry_number_cfg2(path, "pvc.", &pvcIdx) != 0 || get_entry_number_cfg2(path, "entry.", &entryIdx) != 0)
	{
		return -1;
	}
	
	switch (pvcIdx)
	{
		case MAC_FILTER_RULE_FLAG:
			syncMacRuleToIpMacFilter(MAC_FILTER_RULE_FLAG, entryIdx);
			break;
		case IP_UP_FILTER_RULE_FLAG:
			syncIPRuleToIpMacFilter(IP_UP_FILTER_RULE_FLAG, entryIdx);
			break;
		case IP_DOWN_FILTER_RULE_FLAG:
			syncIPRuleToIpMacFilter(IP_DOWN_FILTER_RULE_FLAG, entryIdx);
			break;
		case ACL_FILTER_RULE_FLAG:
			syncAclRuleToACLFilter(ACL_FILTER_RULE_FLAG, entryIdx);
			break;
		default:
			break;
	}

	return 0;
}

int cfg_type_firewallch_pvc_func_commit(char* path)
{
	int idx = -1, pvcIdx = -1;
	char tmp[32] = {0};

		if(get_entry_number_cfg2(path, "pvc.", &pvcIdx) != 0)
		{
			memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "pvc_Idx", 0, tmp, sizeof(tmp)) >= 0)
			{
			pvcIdx = atoi(tmp) + 1;	
			}
			else
			{
				return -1;
			}
		}

	switch (pvcIdx)
	{
		case MAC_FILTER_RULE_FLAG:
			syncIpMacFilterActiveByPvcIdx(MAC_FILTER_RULE_FLAG);
			break;
		case IP_UP_FILTER_RULE_FLAG:
			syncIpMacFilterActiveByPvcIdx(IP_UP_FILTER_RULE_FLAG);
			break;
		case IP_DOWN_FILTER_RULE_FLAG:
			syncIpMacFilterActiveByPvcIdx(IP_DOWN_FILTER_RULE_FLAG);
			break;
		case ACL_FILTER_RULE_FLAG:
			syncIpMacFilterActiveByPvcIdx(ACL_FILTER_RULE_FLAG);		
			break;
		default:
			break;
	}

	return 0;
}


int deleteIpmacfilterNodeRangeByPvcIdx(int pvcIdx)
{
	int i = 0;
	char nodeName[MAXLEN_NODE_NAME] ={0};
	
	for(i = 1; i <= TR181_FIREWALL_CHAIN_ENTRY_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		if(pvcIdx == MAC_FILTER_RULE_FLAG)
		{
			snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i);
		}
		else if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
		{
			snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i + MAX_MAC_FILTER_RULE);
		}
		else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
		{
			snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i + MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE);
		}

		cfg_obj_delete_object(nodeName);	
	}	
	cfg_tr181_commit_object(IPMACFILTER_NODE);
	
	return 0;
}

int deleteAclNodeRangeByPvcIdx(int pvcIdx)
{
	int i = 0;
	char nodeName[MAXLEN_NODE_NAME] ={0};
	
	for(i = 1; i <= TR181_FIREWALL_CHAIN_ENTRY_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ACL_ENTRY_N_NODE, i);
		cfg_obj_delete_object(nodeName);	
	}	
	cfg_tr181_commit_object(ACL_NODE);
	
	return 0;
}

int deleteIpmacfilterNodeEntryByPvcIdx(int pvcIdx, int entryIdx)
{
	char nodeName[MAXLEN_NODE_NAME] ={0};
	
	if(pvcIdx == MAC_FILTER_RULE_FLAG)
	{
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, entryIdx);
	}
	else if(pvcIdx == IP_UP_FILTER_RULE_FLAG)
	{
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, entryIdx + MAX_MAC_FILTER_RULE);
	}
	else if(pvcIdx == IP_DOWN_FILTER_RULE_FLAG)
	{
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, entryIdx + MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE);
	}
	
	cfg_obj_delete_object(nodeName);	
	cfg_tr181_commit_object(IPMACFILTER_NODE);
	
	return 0;
}


int deleteAclNodeEntryByEntryIdx(int entryIdx)
{
	char nodeName[MAXLEN_NODE_NAME] ={0};
	
	snprintf(nodeName, sizeof(nodeName), ACL_ENTRY_N_NODE, entryIdx);
	
	cfg_obj_delete_object(nodeName);	
	cfg_tr181_commit_object(ACL_NODE);
	
	return 0;
}

int cfg_type_firewallch_pvc_func_delete(char* path)
{
	int pvcIdx= -1;
		
	if(path == NULL || get_entry_number_cfg2(path, "pvc.", &pvcIdx) != 0)
	{
		return -1;
	}

	switch (pvcIdx)
	{
		case MAC_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeRangeByPvcIdx(MAC_FILTER_RULE_FLAG);
			break;
		case IP_UP_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeRangeByPvcIdx(IP_UP_FILTER_RULE_FLAG);
			break;
		case IP_DOWN_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeRangeByPvcIdx(IP_DOWN_FILTER_RULE_FLAG);
			break;
		case ACL_FILTER_RULE_FLAG:
			deleteAclNodeRangeByPvcIdx(ACL_FILTER_RULE_FLAG);
			break;
		default:
			break;
	}
	
	return cfg_obj_delete_object(path);
}

int cfg_type_firewallch_pvc_entry_func_delete(char* path)
{
	int pvcIdx= -1;
	int entryIdx = -1;
		
	if(path == NULL || get_entry_number_cfg2(path, "pvc.", &pvcIdx) != 0 || get_entry_number_cfg2(path, "entry.", &entryIdx) != 0)
	{
		return -1;
	}

	switch (pvcIdx)
	{
		case MAC_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeEntryByPvcIdx(MAC_FILTER_RULE_FLAG, entryIdx);
			break;
		case IP_UP_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeEntryByPvcIdx(IP_UP_FILTER_RULE_FLAG, entryIdx);
			break;
		case IP_DOWN_FILTER_RULE_FLAG:
			deleteIpmacfilterNodeEntryByPvcIdx(IP_DOWN_FILTER_RULE_FLAG, entryIdx);
			break;
		case ACL_FILTER_RULE_FLAG:
			deleteAclNodeEntryByEntryIdx(entryIdx);
			break;
		default:
			break;
	}
	
	return cfg_obj_delete_object(path);
}


static cfg_node_ops_t cfg_type_firewallch_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_firewallch_pvc_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_firewallch_pvc_entry_func_commit 
}; 

static cfg_node_type_t cfg_type_firewallch_pvc_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_FIREWALL_CHAIN_ENTRY_NUM, 
	 .parent = &cfg_type_firewallch_pvc, 
	 .ops = &cfg_type_firewallch_pvc_entry_ops, 
}; 

static cfg_node_ops_t ccfg_type_firewallch_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_firewallch_pvc_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_firewallch_pvc_func_commit 
}; 

static cfg_node_type_t* cfg_type_firewallch_pvc_child[] = { 
	 &cfg_type_firewallch_pvc_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_firewallch_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_FIREWALL_CHAIN_ENTRY_NUM, 
	 .parent = &cfg_type_firewallch, 
	 .nsubtype = sizeof(cfg_type_firewallch_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_firewallch_pvc_child, 
	 .index = cfg_type_firewallch_pvc_index,
	 .ops = &ccfg_type_firewallch_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_firewallch_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_firewallch_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_firewallch, 
	 .ops = &cfg_type_firewallch_common_ops, 
}; 


static cfg_node_ops_t cfg_type_firewallch_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_firewallch_pvc_func_commit 
}; 
	
static cfg_node_type_t* cfg_type_firewallch_child[] = { 
	 &cfg_type_firewallch_common, 
	 &cfg_type_firewallch_pvc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_firewallch = { 
	 .name = "FirewallCh", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_firewallch_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_firewallch_child, 
	 .ops = &cfg_type_firewallch_ops, 
}; 
