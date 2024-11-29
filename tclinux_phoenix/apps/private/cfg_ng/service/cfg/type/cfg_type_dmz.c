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
#include <unistd.h>
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "../utility.h"

static char* cfg_type_dmz_index[] = { 
	 "nat_pvc", 
	 NULL 
}; 

#define DMZ_PATH	"/var/run/%s/dmz"
#define DMZ_SH		"/usr/script/dmz.sh"

static int wan_related_dmz_execute(char* path)
{
	char nodeName[128] 	= {0};
	char cur_pvc[4]		= {0};
	char cmd[32]		= {0};
	char status[8]		= {0};
	char if_name[32]	= {0};
	char dmz_file[32]	= {0};
	int pvcIndex;

	if(get_entry_number_cfg2(path, "entry.", &pvcIndex) != 0){
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		cfg_obj_get_object_attr(nodeName, "nat_pvc", 0, cur_pvc, sizeof(cur_pvc));
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DMZ_ENTRY_NODE, atoi(cur_pvc) + 1);
	}else{
		memset(nodeName, 0, sizeof(nodeName));
		strncpy(nodeName, path, sizeof(nodeName) - 1);
	}
	
	if(cfg_obj_get_object_attr(nodeName, "IFName", 0, if_name, sizeof(if_name)) >0 &&
		cfg_obj_get_object_attr(nodeName, "Active", 0, status, sizeof(status)) > 0){
		snprintf(dmz_file, sizeof(dmz_file), DMZ_PATH, if_name);
		if(0 == strcmp(status, "No")){			
			unlink(dmz_file);
		}
		else{
			if (cfg_obj_query_object(path, NULL, NULL) > 0){
				if(cfg_save_attrs_to_file(nodeName, dmz_file, NO_QMARKS) < 0){
					return -1;
				}	
			}else {
				return -1;
			}
		}
		snprintf(cmd, sizeof(cmd), "%s %s", DMZ_SH, if_name);
		system_escape(cmd);

		return 0;
	}
	return -1;
}/* end dmz_execute*/

static int cfg_type_dmz_entry_func_commit(char* path)
{
	int ret = -1;
	ret = wan_related_dmz_execute(path);

	return ret;	
}

int svc_cfg_boot_dmz(void)
{
	int i;
	char nodeName[128]	= {0};
	
	for(i = 0; i < MAX_WAN_IF_INDEX; i++){		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DMZ_ENTRY_NODE, i+1);
		wan_related_dmz_execute(nodeName);		
	}
	return 0;
}

static cfg_node_ops_t cfg_type_dmz_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dmz_entry_func_commit 
}; 

static cfg_node_type_t cfg_type_dmz_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_dmz, 
	 .index = cfg_type_dmz_index, 
	 .ops = &cfg_type_dmz_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_dmz_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dmz_entry_func_commit 
}; 


static cfg_node_type_t* cfg_type_dmz_child[] = { 
	 &cfg_type_dmz_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_dmz = { 
	 .name = "Dmz", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dmz_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dmz_child, 
	 .ops = &cfg_type_dmz_ops, 
}; 
