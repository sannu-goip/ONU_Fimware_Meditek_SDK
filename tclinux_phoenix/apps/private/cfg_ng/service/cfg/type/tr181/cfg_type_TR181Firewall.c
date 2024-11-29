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

int syncFirewallToTr181firewall(void);

int  updateTR181FireWall(void)
{
	char buf[8] = {0};
	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, buf, sizeof(buf));
	if(strcmp(buf,"1"))
	{	
		syncFirewallToTr181firewall();
	}
	
	return 0;
}

int syncFirewallToTr181firewall(void)
{
	int ret = -1;
	char tmp[64] = {0};

	if(cfg_query_object(TR181_FIREWALL_ENTRY_NODE,NULL,NULL) < 0)
	{
		ret = cfg_create_object(TR181_FIREWALL_ENTRY_NODE);
		if(ret < 0)
		{
			return -1;
		}
	}
	
	if(cfg_obj_get_object_attr(FIREWALL_ENTRY_NODE, "firewall_status", 0, tmp, sizeof(tmp)) > 0 &&  tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(TR181_FIREWALL_ENTRY_NODE, "Enable", 0, tmp);
	}
	else
	{
		cfg_obj_set_object_attr(TR181_FIREWALL_ENTRY_NODE, "Enable", 0, "0");
	}

	if(cfg_obj_get_object_attr(FIREWALL_ENTRY_NODE, "firewall_level", 0, tmp, sizeof(tmp)) > 0 &&  tmp[0] != '\0')
	{
		if(0 == strcmp(tmp, "medium"))
		{
			/*There is no "medium" level for firewall on TR181 specification , so if set "medium" on webpage, it will be displayed "Low" in TR181 node. */
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, "low", sizeof(tmp));
		}
		string_FirstCharToUp(tmp);
		cfg_obj_set_object_attr(TR181_FIREWALL_ENTRY_NODE, "Config", 0, tmp);
	}
	
	return 0;
}


int cfg_type_tr181firewall_func_commit(char* path)
{
	char tr181Node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};
	char node[MAXLEN_NODE_NAME] = {0};	

	/*enable*/
	if(cfg_obj_get_object_attr(TR181_FIREWALL_ENTRY_NODE, "Enable", 0, tmp, sizeof(tmp)) >= 0 &&  tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(FIREWALL_ENTRY_NODE, "firewall_status", 0, tmp);
	}	

	/*Level*/
	if(cfg_obj_get_object_attr(TR181_FIREWALL_ENTRY_NODE, "Config", 0, tmp, sizeof(tmp)) >= 0 &&  tmp[0] != '\0')
	{
		string_tolower(tmp);
		cfg_obj_set_object_attr(FIREWALL_ENTRY_NODE, "firewall_level", 0, tmp);
	}	

	return 0;
}



static cfg_node_ops_t cfg_type_tr181firewall_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181firewall_func_commit 
}; 

static cfg_node_type_t cfg_type_tr181firewall_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181firewall, 
	 .ops = &cfg_type_tr181firewall_entry_ops, 
};


static cfg_node_ops_t cfg_type_tr181firewall_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181firewall_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181firewall_child[] = { 
	 &cfg_type_tr181firewall_entry,
	 NULL 
}; 

cfg_node_type_t cfg_type_tr181firewall= { 
	 .name = "TR181Firewall", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181firewall) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181firewall_child, 
	 .ops = &cfg_type_tr181firewall_ops, 
}; 

