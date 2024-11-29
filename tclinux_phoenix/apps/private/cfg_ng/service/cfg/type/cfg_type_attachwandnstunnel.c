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
#include <stdlib.h>
#include "cfg_types.h" 
#include <svchost_evt.h>

extern cfg_node_type_t cfg_type_attachwandnstunnel_pvc;

int cfg_type_attachwandnstunnel_execute(char* path)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_ATTACH_UPDATE, NULL, 0); 
	return 0;
}

int cfg_type_attachwandnstunnel_func_commit(char* path)
{
	 return cfg_type_attachwandnstunnel_execute(path);
}


static char* cfg_type_attachwandnstunnel_index[] = { 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_attachwandnstunnel_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attachwandnstunnel_func_commit 
}; 


static cfg_node_type_t cfg_type_attachwandnstunnel_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 16, 
	 .parent = &cfg_type_attachwandnstunnel_pvc, 
	 .index = cfg_type_attachwandnstunnel_index, 
	 .ops = &cfg_type_attachwandnstunnel_entry_ops, 
}; 

static cfg_node_type_t* cfg_type_attachwandnstunnel_pvc_child[] = { 
	 &cfg_type_attachwandnstunnel_entry, 
	 NULL 
};

static cfg_node_ops_t cfg_type_attachwandnstunnel_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attachwandnstunnel_func_commit 
};

cfg_node_type_t cfg_type_attachwandnstunnel_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_attachwandnstunnel, 
	 .index = cfg_type_attachwandnstunnel_index,
	 .nsubtype = sizeof(cfg_type_attachwandnstunnel_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_attachwandnstunnel_pvc_child, 
	 .ops = &cfg_type_attachwandnstunnel_pvc_ops, 
	 
};

static cfg_node_ops_t cfg_type_attachwandnstunnel_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attachwandnstunnel_func_commit 
}; 


static cfg_node_type_t cfg_type_attachwandnstunnel_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_attachwandnstunnel, 
	 .ops = &cfg_type_attachwandnstunnel_common_ops, 
}; 


static cfg_node_ops_t cfg_type_attachwandnstunnel_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attachwandnstunnel_func_commit 
}; 


static cfg_node_type_t* cfg_type_attachwandnstunnel_child[] = { 
	 &cfg_type_attachwandnstunnel_pvc, 
	 &cfg_type_attachwandnstunnel_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_attachwandnstunnel = { 
	 .name = "AttachWanDNSTunnel", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_attachwandnstunnel_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_attachwandnstunnel_child, 
	 .ops = &cfg_type_attachwandnstunnel_ops, 
}; 
