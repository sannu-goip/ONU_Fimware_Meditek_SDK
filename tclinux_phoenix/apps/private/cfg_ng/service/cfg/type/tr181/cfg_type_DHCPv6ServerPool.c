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

static char* cfg_type_dhcpv6serverpool_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 

void syncDhcp6s2DNSServer(char *type)
{	
	char dnsNode[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char Interface[256] = {0};
	char strNum[8] = {0};
	int index = -1;
	int ipIndex = -1;
	int flag = -1;
	
	flag = checkEntryExistByPvcLanIdx(&index, TR181_DNS_CLIENT_NODE, DNS_CLIENT_NUM, 0, 0);
	if(flag == 0)
	{
		snprintf(dnsNode,sizeof(dnsNode),TR181_DNSCLIENT_ENTRY_NODE,index + 1);
		cfg_obj_create_object(dnsNode);
		/*modify num, need +1*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(TR181_DNS_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp)); 	
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(TR181_DNS_CLIENT_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
	}
	
	/*set Enable*/
	cfg_obj_set_object_attr(dnsNode, TR181_ENABLE_ATTR, 0, "1");
	
	/*set DNSServer*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "DNSserver", 0, tmp, sizeof(tmp)) > 0)
	{
		cfg_obj_set_object_attr(dnsNode, TR181_DNSIP_ATTR, 0, tmp);
	}
	
	/*set PvcIndex and LanIndex*/
	cfg_obj_set_object_attr(dnsNode, PVC_INDEX_ATTR, 0, "-1");
	cfg_obj_set_object_attr(dnsNode, LAN_INDEX_ATTR, 0, "0");
	
	/*set typeval*/
	setDNSServerType(index, type);
	
	/*set Interface*/
	checkEntryExistByPvcLanIdx(&ipIndex, TR181_IP_NODE, IP_INTERFACE_NUM, 0, 0);	
	memset(Interface, 0, sizeof(Interface));
	snprintf(Interface, sizeof(Interface), TR181_IP_INTERFACE"%d", ipIndex + 1);	
	setInterfaceValue(TR181_DNS_CLIENT_NODE, index, Interface);
	
	return ;

}

int updateDHCPv6ServerPool()
{
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char Interface[256] = {0};
	char strpvcIndex[8] = {0};
	int etyIdx = -1;
	int wanIf = -1;
	
	memset(tr181node, 0, sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_DHCPV6_SVRPOOL_ENTRY_NODE, 1);	
	if(cfg_query_object(tr181node,NULL,NULL) < 0){
		cfg_create_object(tr181node);
	}

	/*Interface*/
	memset(tmp, 0, sizeof(tmp));
	memset(Interface, 0, sizeof(Interface));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "Mode", 0, tmp, sizeof(tmp)) > 0)
	{	
		if(1 == atoi(tmp))
		{
			/*Manual*/
			cfg_obj_set_object_attr(tr181node, TR181_ENABLE_ATTR, 0,"0");
			getBr0IPInterface(Interface, sizeof(Interface));
			cfg_obj_set_object_attr(tr181node, TR181_INTERFACE_ATTR, 0,Interface);
			syncDhcp6s2DNSServer("Static");
		}
		else
		{	/*Auto*/
			cfg_obj_set_object_attr(tr181node, TR181_ENABLE_ATTR, 0, "1");
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "DNSType", 0, tmp, sizeof(tmp));
			if(!strcmp(tmp,"0"))
			{
				cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "DNSWANConnection", 0, strpvcIndex, sizeof(strpvcIndex));
				wanIf = atoi(strpvcIndex);
				etyIdx = getTR181NodeEntryIdxByWanIf(TR181_IP_NODE,IP_INTERFACE_NUM,wanIf);
				if(etyIdx >= 0){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp,sizeof(tmp),TR181_IP_INTERFACE"%d",etyIdx + 1);
				cfg_obj_set_object_attr(tr181node, TR181_INTERFACE_ATTR, 0,tmp);
			}
			}
			else if(!strcmp(tmp,"1"))		
			{
				cfg_obj_set_object_attr(tr181node, TR181_INTERFACE_ATTR, 0,Interface);
				syncDhcp6s2DNSServer("DHCPv6");
			}
		}

	}
		
	/*SourceAddress*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "PrefixIPv6", 0, tmp,sizeof(tmp)) >= 0){
		cfg_obj_set_object_attr(tr181node, "SourceAddress", 0,tmp);
	}
	
	/*SourceAddressMask*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, "Prefixv6Len", 0, tmp,sizeof(tmp)) >= 0){
		cfg_obj_set_object_attr(tr181node, "SourceAddressMask", 0,tmp);
	}
	
	return 0;
}


int cfg_type_dhcpv6serverpool_func_commit(char *path)
{
	int etyIdx = -1;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int ipIfIdx = -1;
	int wanIf= -1;
	char strpvcIdx[8] = {0};
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}
	memset(tmp, 0, sizeof(tmp));
	memset(tr181node,0,sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_DHCPV6_SVRPOOL_ENTRY_NODE, etyIdx);
	
	if(cfg_obj_get_object_attr(DHCP6S_ENTRY_NODE, TR181_ENABLE_ATTR, 0,tmp,sizeof(tmp)) <= 0 || !strcmp(tmp,"0")){
		return -1;
	}

	if(cfg_obj_get_object_attr(tr181node, TR181_ENABLE_ATTR, 0,tmp,sizeof(tmp)) >= 0)
	{
		if(!strcmp(tmp, "0")){
			/* serverpool enable == Manual*/
			cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Mode", 0, "1");
			cfg_obj_get_object_attr(tr181node, TR181_ENABLE_ATTR, 0,tmp,sizeof(tmp));
		}
		else if(!strcmp(tmp, "1"))
		{
			/*serverpool disable == Auto*/		
			cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Mode", 0, "0");
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(tr181node, TR181_INTERFACE_ATTR, 0,tmp,sizeof(tmp)) >= 0)
			{
				ipIfIdx = getSuffixIntByInterface(tmp);
				if(ipIfIdx >= 0)
				{
				wanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
				if(wanIf >= 0)
				{
					snprintf(strpvcIdx,sizeof(strpvcIdx),"%d",wanIf);
					cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "DNSWANConnection", 0,strpvcIdx);
					cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "DNSType", 0,"0");
				}
				else
					cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "DNSType", 0,"1");
			}
		}
	}
	}

	/*SourceAddress */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SourceAddress", 0,tmp,sizeof(tmp)) >= 0){
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "PrefixIPv6", 0,tmp);
	}

	/*SourceAddressMask */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SourceAddressMask", 0,tmp,sizeof(tmp)) >= 0){
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "Prefixv6Len", 0,tmp);
	}

	cfg_tr181_commit_object("root.dhcp6s");
	
	return 0;
}


static cfg_node_ops_t cfg_type_dhcpv6serverpool_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6serverpool_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv6serverpool_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV6_SVRPOL_NUM_MAX, 
	 .parent = &cfg_type_dhcpv6serverpool, 
	 .index = cfg_type_dhcpv6serverpool_index,
	 .ops = &cfg_type_dhcpv6serverpool_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv6serverpool_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6serverpool_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv6serverpool_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv6serverpool, 
	 .ops = &cfg_type_dhcpv6serverpool_common_ops, 
};


static cfg_node_ops_t cfg_type_dhcpv6serverpool_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv6serverpool_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv6serverpool_child[] = { 
	 &cfg_type_dhcpv6serverpool_entry, 
	 &cfg_type_dhcpv6serverpool_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv6serverpool= { 
	 .name = "DHCPv6SP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv6serverpool_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv6serverpool_child, 
	 .ops = &cfg_type_dhcpv6serverpool_ops, 
}; 
