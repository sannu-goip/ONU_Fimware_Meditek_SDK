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

int update_tr181_DHCPv4Relay_FromLan(char *value)
{
	char buf[4] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int etyIdx = -1;
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName),TR181_DHCPV4_RELAY_ENTRY_NODE,1);

	cfg_obj_set_object_attr(TR181_DHCPV4_RELAY_COMMON_NODE, "Enable", 0, value); 
	if(cfg_query_object(nodeName,NULL,NULL) <= 0)
	{		
		etyIdx = cfg_obj_create_object(nodeName);
		if(etyIdx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
			return -1;
		}
	}
	cfg_obj_set_object_attr(nodeName, "Enable", 0, value); 	
	
	return 0;
}


int update_tr181_DHCPv4Relay_FromDhcpRelay()
{
	char buf[64] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int etyIdx = -1;

	snprintf(nodeName,sizeof(nodeName),TR181_DHCPV4_RELAY_ENTRY_NODE,1);
	if(cfg_obj_get_object_attr(DHCPRELAY_ENTRY_NODE, "Server", 0 ,buf ,sizeof(buf)) >= 0){
		if(cfg_query_object(nodeName,NULL,NULL) <= 0)
		{		
			etyIdx = cfg_obj_create_object(nodeName);
			if(etyIdx < 0) 
			{
				tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
				return -1;
			}
		}
		cfg_obj_set_object_attr(nodeName, "DHCPServerIPAddress" ,0 , buf);
	}
	
	return 0;
}


int cfg_type_dhcpv4rly_func_commit(char *path)
{
	char buf[32] = {0};
	char tmpbuf[8] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmpNode[MAXLEN_NODE_NAME] = {0};
	char type[4] = {0};

	memset(nodeName, 0 , sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), TR181_DHCPV4_RELAY_ENTRY_NODE , 1);

	if(cfg_obj_get_object_attr(TR181_DHCPV4_RELAY_COMMON_NODE, "Enable", 0, buf ,sizeof(buf)) >= 0 && cfg_obj_get_object_attr(nodeName, "Enable", 0 , tmpbuf ,sizeof(tmpbuf)) >= 0)
	{
		cfg_obj_get_object_attr(LAN_DHCP_NODE, "type", 0, type ,sizeof(type));
		if(!strcmp(buf,"1") && !strcmp(tmpbuf,"1"))
		{	
			if(strcmp(type, "2"))	
			{
				cfg_obj_set_object_attr(LAN_DHCP_NODE, "type", 0 , "2");
				cfg_tr181_commit_object("root.lan");
			}
			memset(buf, 0, sizeof(buf));
			if(cfg_obj_get_object_attr(nodeName, "DHCPServerIPAddress", 0 , buf, sizeof(buf)) >= 0)
			{
				cfg_obj_set_object_attr(DHCPRELAY_ENTRY_NODE, "Server", 0 , buf);
				cfg_tr181_commit_object(DHCPRELAY_ENTRY_NODE);
			}
		}
		else
		{
			if(!strcmp(type, "2"))
			{
				cfg_obj_set_object_attr(LAN_DHCP_NODE, "type", 0 ,"0");
				cfg_tr181_commit_object("root.lan");
				cfg_tr181_commit_object(DHCPRELAY_ENTRY_NODE);
			}
		}
	}
	return 0;
}


static cfg_node_ops_t cfg_type_dhcpv4rly_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4rly_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4rly_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV4_RELAY_NUM_MAX, 
	 .parent = &cfg_type_dhcpv4rly, 
	 .ops = &cfg_type_dhcpv4rly_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv4rly_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4rly_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4rly_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv4rly, 
	 .ops = &cfg_type_dhcpv4rly_common_ops, 
};


static cfg_node_ops_t cfg_type_dhcpv4rly_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4rly_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4rly_child[] = { 
	 &cfg_type_dhcpv4rly_entry, 
	 &cfg_type_dhcpv4rly_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4rly= { 
	 .name = "DHCPv4Rly", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4rly_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4rly_child, 
	 .ops = &cfg_type_dhcpv4rly_ops, 
}; 
