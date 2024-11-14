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

int cfg_type_StorageAccess_func_commit(char *path)
{
#if defined(TCSUPPORT_CUC)
	int i = 0;
	char MacAddr[20] = {0}, newMacAddr[20] = {0};
	char cmd[256] = {0}, nodeName[128] = {0};

	system("iptables -F storage_chain 2>/dev/null");
	
	for(i = 0; i < 64; i++)
	{
		snprintf(nodeName, sizeof(nodeName), STORAGEACCESSRIGHT_ENTRY_NODE, i + 1);
		if (cfg_obj_get_object_attr(nodeName, "MAC", 0, MacAddr, sizeof(MacAddr)) <= 0 || !strcmp(MacAddr, "000000000000")) 
		{
			continue;
		}
		mac_add_dot(MacAddr, newMacAddr);

		snprintf(cmd, sizeof(cmd), "iptables -t filter -A storage_chain -i br0 -m mac --mac-source %s -p TCP -m multiport --dport 21,445,139,389,901 -j DROP 2>/dev/null", newMacAddr);
		system(cmd);
		snprintf(cmd, sizeof(cmd), "iptables -t filter -A storage_chain -i br0 -m mac --mac-source %s -p UDP -m multiport --dport 137,138 -j DROP 2>/dev/null", newMacAddr);
		system(cmd);
	}	
#endif
	return 0;
}

#if defined(TCSUPPORT_CUC)
int svc_cfg_boot_storageaccessright()
{	
	system("iptables -t filter -A INPUT -j storage_chain 2>/dev/null");
	cfg_type_StorageAccess_func_commit(NULL);
	return 0;
}
#endif

static char* cfg_type_storageaccessright_index[] = { 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_storageaccessright_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_StorageAccess_func_commit 
}; 


static cfg_node_type_t cfg_type_storageaccessright_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_storageaccessright, 
	 .index = cfg_type_storageaccessright_index, 
	 .ops = &cfg_type_storageaccessright_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_storageaccessright_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_StorageAccess_func_commit 
}; 


static cfg_node_type_t cfg_type_storageaccessright_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_storageaccessright, 
	 .ops = &cfg_type_storageaccessright_common_ops, 
}; 


static cfg_node_ops_t cfg_type_storageaccessright_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_StorageAccess_func_commit 
}; 


static cfg_node_type_t* cfg_type_storageaccessright_child[] = { 
	 &cfg_type_storageaccessright_entry, 
	 &cfg_type_storageaccessright_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_storageaccessright = { 
	 .name = "StorageAccessRight", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_storageaccessright_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_storageaccessright_child, 
	 .ops = &cfg_type_storageaccessright_ops, 
}; 
