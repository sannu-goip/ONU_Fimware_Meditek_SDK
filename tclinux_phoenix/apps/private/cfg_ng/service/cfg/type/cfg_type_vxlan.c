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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"

static int cfg_type_vxlan_statistics_instance_num()
{
	char tmp[16] = {0};
	char nodeName[64] = {0};
	int i = 0, entryNum = 0, maxEntryNum = 0;
	
	cfg_obj_get_object_attr(VXLAN_COMMON_NODE, "MaxVxLANNumber", 0, tmp, sizeof(tmp));
	if ( 0 != tmp[0] )
	{
		maxEntryNum = atoi(tmp);
		for ( i = 0; i < maxEntryNum; i++ ) 
		{
			memset(nodeName,0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VXLAN_ENTRY_NODE, i + 1);
			memset(tmp, 0, sizeof(tmp));			
			cfg_obj_get_object_attr(nodeName, "Enable", 0, tmp, sizeof(tmp));
			if ( 0 != tmp[0] )
			{
				entryNum++;
			}
		}

		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", entryNum);
		cfg_set_object_attr(VXLAN_COMMON_NODE, "VxLANNumber", tmp);
	}

	return 0;
}

int svc_cfg_boot_vxlan(void)
{
	/* Statistics the instance num */
	cfg_type_vxlan_statistics_instance_num();

	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VXLAN_BOOT, NULL, 0); 
	return 0;
} 

static int cfg_type_vxlan_func_commit(char* path)
{
	char nodeName[64] = {0};
	char tmp[16] = {0};
	wan_related_evt_t param;

	/* Statistics the instance num */
	cfg_type_vxlan_statistics_instance_num();

	/* vxlan action by vxlan_id */
	memset(tmp, 0, sizeof(tmp));
	cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "vxlan_id", tmp, sizeof(tmp));
	if ( 0 == tmp[0] )
		return 0;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VXLAN_ENTRY_NODE, atoi(tmp) + 1);

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, nodeName, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VXLAN_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

static int cfg_type_vxlan_entry_func_commit(char* path)
{
	wan_related_evt_t param;

	/* Statistics the instance num */
	cfg_type_vxlan_statistics_instance_num();

	/* vxlan action */
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VXLAN_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

int cfg_type_vxlan_entry_func_delete(char* path)
{
	wan_related_evt_t param;

	cfg_obj_delete_object(path);

	/* Statistics the instance num */
	cfg_type_vxlan_statistics_instance_num();

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VXLAN_DELETE, (void *)&param, sizeof(param)); 

	return 0;
}


static cfg_node_ops_t cfg_type_vxlan_ipentry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_vxlan_ipentry = { 
	 .name = "IPEntry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_vxlan, 
	 .ops = &cfg_type_vxlan_ipentry_ops, 
};


static char* cfg_type_vxlan_index[] = { 
	 "vxlan_id", 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_vxlan_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_vxlan_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_vxlan_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_vxlan_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_vxlan, 
	 .index = cfg_type_vxlan_index, 
	 .ops = &cfg_type_vxlan_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_vxlan_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_vxlan_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_vxlan, 
	 .ops = &cfg_type_vxlan_common_ops, 
};


static cfg_node_ops_t cfg_type_vxlan_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_vxlan_func_commit 
}; 


static cfg_node_type_t* cfg_type_vxlan_child[] = { 
	 &cfg_type_vxlan_common, 
	 &cfg_type_vxlan_entry, 
	 &cfg_type_vxlan_ipentry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_vxlan = { 
	 .name = "VxLAN", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_vxlan_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_vxlan_child, 
	 .ops = &cfg_type_vxlan_ops, 
}; 
