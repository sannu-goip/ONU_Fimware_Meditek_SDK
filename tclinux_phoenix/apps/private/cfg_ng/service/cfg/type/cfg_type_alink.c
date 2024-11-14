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
#include "cfg_msg.h"


int svc_cfg_boot_alinkmgr(void)
{
	char active[4] = {0};

	system("/usr/bin/killall -9 alink-mgr");
	
	cfg_obj_set_object_attr(ALINKMGR_ENTRY_NODE, "step", 0, "0");
	cfg_obj_set_object_attr(ALINKMGR_ENTRY_NODE, "heartBeatTime", 0, "");
	cfg_obj_get_object_attr(ALINKMGR_ENTRY_NODE, "Active", 0, active, sizeof(active));
	if ( 0 == strcmp(active, "Yes") )
		system("/userfs/bin/alink-mgr &");

	return SUCCESS;
}

int cfg_type_alinkmgr_entry_func_commit(char* path)
{
	svc_cfg_boot_alinkmgr();
	
	return SUCCESS;
}

static cfg_node_ops_t cfg_type_alinkmgr_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_alinkmgr_entry_func_commit 
}; 

static cfg_node_type_t cfg_type_alinkmgr_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_alinkmgr, 
	 .ops = &cfg_type_alinkmgr_entry_ops, 
}; 

static cfg_node_type_t* cfg_type_alinkmgr_child[] = { 
	 &cfg_type_alinkmgr_entry, 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_alinkmgr_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_alinkmgr_entry_func_commit 
}; 

cfg_node_type_t cfg_type_alinkmgr = { 
	 .name = "alinkmgr", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_alinkmgr_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_alinkmgr_child, 
	 .ops = &cfg_type_alinkmgr_ops, 
}; 


int cfg_type_if6regioninfo_common_func_commit(char* path)
{
	int i = 0;
	char num[10] = {0};
	char Region[20] = {0};
	char nodeName[40] = {0};
	char area[20] = {0};
	char IF6server[50] = {0};
	char IF6Port[10] = {0};
	
	cfg_obj_get_object_attr(IF6REGIONINFO_COMMON_NODE, "area", 0, area, sizeof(area));
	cfg_obj_get_object_attr(IF6REGIONINFO_COMMON_NODE, "NUM", 0, num, sizeof(num));
	if(strlen(area) == 0 || strlen(num) == 0)
		return 0;
	
	for(i = 0;i < atoi(num);i++)
	{
		snprintf(nodeName, sizeof(nodeName), IF6REGIONINFO_ENTRY_NODE, i+1);
		cfg_obj_get_object_attr(nodeName, "Region", 0, Region, sizeof(Region));
		cfg_obj_get_object_attr(nodeName, "IF6server", 0, IF6server, sizeof(IF6server));
		cfg_obj_get_object_attr(nodeName, "IF6Port", 0, IF6Port, sizeof(IF6Port));
		if(strcmp(Region,area) == 0 && strlen(IF6server) != 0 && strlen(IF6Port) != 0)
		{
			cfg_obj_set_object_attr(ALINKMGR_ENTRY_NODE, "ServerAddr", 0, IF6server);
			cfg_obj_set_object_attr(ALINKMGR_ENTRY_NODE, "ServerPort", 0, IF6Port);
			svc_cfg_boot_alinkmgr();
			return 0;
		}
	}
	return SUCCESS;
}

static cfg_node_ops_t cfg_type_if6regioninfo_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query
}; 

static cfg_node_ops_t cfg_type_if6regioninfo_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_if6regioninfo_common_func_commit 
}; 

static char* cfg_type_if6regioninfo_index[] = { 
	 NULL 
}; 


static cfg_node_type_t cfg_type_if6regioninfo_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 34, 
	 .index = cfg_type_if6regioninfo_index,
	 .parent = &cfg_type_if6regioninfo, 
	 .ops = &cfg_type_if6regioninfo_entry_ops, 
}; 

static cfg_node_type_t cfg_type_if6regioninfo_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_if6regioninfo, 
	 .ops = &cfg_type_if6regioninfo_common_ops, 
}; 


static cfg_node_type_t* cfg_type_if6regioninfo_child[] = { 
	 &cfg_type_if6regioninfo_entry, 
	 &cfg_type_if6regioninfo_common,
	 NULL 
}; 


static cfg_node_ops_t cfg_type_if6regioninfo_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query
}; 

cfg_node_type_t cfg_type_if6regioninfo = { 
	 .name = "if6regioninfo", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_if6regioninfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_if6regioninfo_child, 
	 .ops = &cfg_type_if6regioninfo_ops, 
}; 

