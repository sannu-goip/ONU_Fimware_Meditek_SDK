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
#include <stdlib.h> 
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 

int cfg_type_devmonitor_func_commit(char* path){
	char nodeName[64] = {0};
	char tmp[256] = {0};
	int i = 0, entryNum = 0, maxEntryNum = 0;

	memset(nodeName,0, sizeof(nodeName));
	strncpy(nodeName, DEVICEMONITOR_COMMON_NODE, sizeof(nodeName)-1);
	
	cfg_obj_get_object_attr(nodeName, "MaxInstanceNum", 0, tmp, sizeof(tmp));
	/* Statistics the instance num */
	if(tmp[0] != '\0') 
	{
		maxEntryNum = atoi(tmp);
		
		for(i=1; i<=maxEntryNum; i++) 
		{
			memset(nodeName,0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), DEVICEMONITOR_ENTRY_NODE, i);
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(nodeName, "ParaList", 0, tmp, sizeof(tmp));
			if(tmp[0] != '\0') 
			{
				if(strlen(tmp) != 0)
					entryNum++;
			}
		}

		memset(nodeName,0, sizeof(nodeName));
		strncpy(nodeName, DEVICEMONITOR_COMMON_NODE, sizeof(nodeName)-1);

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%d", entryNum);
		
		cfg_set_object_attr(nodeName, "InstanceNum", tmp);
	}
	return 0;
}/* end devMonitor_write */

static char* cfg_type_devicemonitor_index[] = { 
	 "monitor_id", 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_devicemonitor_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_devmonitor_func_commit 
}; 


static cfg_node_type_t cfg_type_devicemonitor_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 10, 
	 .parent = &cfg_type_devicemonitor, 
	 .index = cfg_type_devicemonitor_index, 
	 .ops = &cfg_type_devicemonitor_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_devicemonitor_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_devmonitor_func_commit 
}; 


static cfg_node_type_t cfg_type_devicemonitor_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_devicemonitor, 
	 .ops = &cfg_type_devicemonitor_common_ops, 
}; 


static cfg_node_ops_t cfg_type_devicemonitor_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_devmonitor_func_commit 
}; 


static cfg_node_type_t* cfg_type_devicemonitor_child[] = { 
	 &cfg_type_devicemonitor_entry, 
	 &cfg_type_devicemonitor_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_devicemonitor = { 
	 .name = "DeviceMonitor", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_devicemonitor_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_devicemonitor_child, 
	 .ops = &cfg_type_devicemonitor_ops, 
}; 
