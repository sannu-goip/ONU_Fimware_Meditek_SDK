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


static char* cfg_type_dns_forwarding_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int updateDnsRelay()
{
	char tmp[128] = {0};
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char Interface[256] = {0};

	memset(tr181Node, 0, sizeof(tr181Node));
	snprintf(tr181Node, sizeof(tr181Node), TR181_DNS_RELAY_FORWARDING_ENTRY_NODE, 1);

	if(cfg_obj_query_object(tr181Node,NULL,NULL) < 0)
	{
		cfg_obj_create_object(tr181Node);
	}
		
	/*Enable*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Active", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"Yes"))
	{
		cfg_obj_set_object_attr(tr181Node, TR181_ENABLE_ATTR, 0, "1");
	}
	else if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Active", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"No"))
	{	
		cfg_obj_set_object_attr(tr181Node, TR181_ENABLE_ATTR, 0, "0");
	}

	/*Dns Mode*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "type", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"1"))/*manual*/
	{
		cfg_obj_set_object_attr(tr181Node, "Type", 0, tmp);
	
		/*sync DnsPrimaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Primary_DNS", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(tr181Node, TR181_DNSIP_ATTR, 0, tmp);

		/*sync DnsSecondaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Secondary_DNS", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(tr181Node, "SECONSIP", 0, tmp);	
	}
	else if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "type", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"0"))/*auto*/
	{
		cfg_obj_set_object_attr(tr181Node, "Type", 0, tmp);
		
		/*sync DnsPrimaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Primary_DNS", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(tr181Node, TR181_DNSIP_ATTR, 0, tmp);

		/*sync DnsSecondaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, "Secondary_DNS", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(tr181Node, "SECONSIP", 0, tmp); 
	}

	/*Interface*/
	memset(Interface, 0, sizeof(Interface));
	getBr0IPInterface(Interface, sizeof(Interface));
	cfg_obj_set_object_attr(tr181Node, TR181_INTERFACE_ATTR, 0, Interface);

	return 0;
}

	
	
int cfg_type_dns_forwarding_func_commit(char *path)
{
	char tmp[128] = {0};
	char tr181Node[MAXLEN_NODE_NAME] = {0};

	memset(tr181Node, 0, sizeof(tr181Node));
	snprintf(tr181Node, sizeof(tr181Node), TR181_DNS_RELAY_FORWARDING_ENTRY_NODE, 1);

	/*Enable*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181Node, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"1"))
	{
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "Active", 0, "Yes");
	}
	else if(cfg_obj_get_object_attr(tr181Node, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"0"))
	{	
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "Active", 0, "No");
	}

	/*Dns Mode*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(tr181Node, "Type", 0, tmp, sizeof(tmp));
	if(!strcmp(tmp,"1"))
	{
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "type", 0, tmp);
	
		/*sync DnsPrimaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(tr181Node, TR181_DNSIP_ATTR, 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "Primary_DNS", 0, tmp);

		/*sync DnsSecondaryAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(tr181Node, "SECONSIP", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "Secondary_DNS", 0, tmp);		
	}
	else if(!strcmp(tmp,"0"))
	{
		cfg_obj_set_object_attr(DPROXY_ENTRY_NODE, "type", 0, tmp);
	}

	cfg_obj_commit_object(DPROXY_ENTRY_NODE);
	cfg_obj_commit_object(DHCPD_COMMON_NODE);
	return 0;
}
	


static cfg_node_ops_t cfg_type_dns_forwarding_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dns_forwarding_func_commit 
}; 


static cfg_node_ops_t cfg_type_dns_forwarding_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dns_forwarding_func_commit 
}; 

static cfg_node_type_t cfg_type_dns_forwarding_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dns_forwarding, 
	 .ops = &cfg_type_dns_forwarding_common_ops, 
};


static cfg_node_type_t cfg_type_dns_forwarding_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_dns_forwarding, 
	 .index = cfg_type_dns_forwarding_index,
	 .ops = &cfg_type_dns_forwarding_entry_ops, 
};


static cfg_node_ops_t cfg_type_dns_forwarding_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dns_forwarding_func_commit 
}; 


static cfg_node_type_t* cfg_type_dns_forwarding_child[] = { 
	 &cfg_type_dns_forwarding_entry,
	 &cfg_type_dns_forwarding_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_dns_forwarding= { 
	 .name = "DnsForward", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dns_forwarding_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dns_forwarding_child, 
	 .ops = &cfg_type_dns_forwarding_ops, 
}; 
