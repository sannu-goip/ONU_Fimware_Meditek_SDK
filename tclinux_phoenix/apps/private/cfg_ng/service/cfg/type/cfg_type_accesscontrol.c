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

int cfg_type_accesscontrol_func_commit(char *path)
{
#if defined(TCSUPPORT_CUC)
	int i = 0, j = 0, type = 0, port = 0;
	char MacAddr[20] = {0}, newMacAddr[20] = {0}, tmpValue[20] = {0};
	char cmd[256] = {0}, nodeName[128] = {0};
	char connType[4] = {0}, connPport[4] = {0}, kickCmd[256] = {0};
	char cmdValue[64] = {0};
	
	system("ebtables -t filter -F Station_Limit 2>/dev/null");	
	
	for(i = 0; i < 64; i++)
	{
		snprintf(nodeName, sizeof(nodeName), ACCESSCONTROL_ENTRY_NODE, i + 1);
		if (cfg_obj_get_object_attr(nodeName, "MAC", 0, MacAddr, sizeof(MacAddr)) <= 0 || !strcmp(MacAddr, "000000000000")) 
		{
			continue;
		}
		mac_add_dot(MacAddr, newMacAddr);
		snprintf(cmd, sizeof(cmd), "ebtables -A Station_Limit -s %s -j DROP 2>/dev/null", newMacAddr);
		system(cmd);
		for(j = 0; j < 64; j++)
		{
			snprintf(nodeName, sizeof(nodeName), LANHOST2_ENTRY_NODE, j + 1);
			/* kick out station */
			if( cfg_obj_get_object_attr(nodeName, "MAC", 0, tmpValue, sizeof(tmpValue)) > 0 && !strcmp(tmpValue, MacAddr))
			{
				if ( cfg_obj_get_object_attr(nodeName, "ConnectionType", 0, connType, sizeof(connType)) > 0 ) {
					type = atoi(connType);
					if ( type == 1 ) {
						if ( cfg_obj_get_object_attr(nodeName, "Port", 0, connPport, sizeof(connPport)) > 0 ) {
							port = atoi(connPport);
							if ( port > 0 && port <= 8)
							{
								snprintf(cmdValue, sizeof(cmdValue), "DisConnectSta=\"%s\"", newMacAddr);
								wifimgr_lib_set_WIFI_CMD("ra", port - 1, sizeof(kickCmd), cmdValue, kickCmd, 0);
							}
							else if ( port > 8 && port <= 16)
							{
								snprintf(cmdValue, sizeof(cmdValue), "DisConnectSta=\"%s\"", newMacAddr);
								wifimgr_lib_set_WIFI_CMD("rai", port - 9, sizeof(kickCmd), cmdValue, kickCmd, 0);
							}

							system(kickCmd);
						}
					}
				}
				cfg_obj_set_object_attr(nodeName, "Active", 0, "2");
				break;
			}
		}
	}	
#endif
	return 0;
}

#if defined(TCSUPPORT_CUC)
int svc_cfg_boot_accesscontrol()
{	
	system("ebtables -N Station_Limit 2>/dev/null");
	system("ebtables -A INPUT -j Station_Limit 2>/dev/null");
	cfg_type_accesscontrol_func_commit(NULL);
	return 0;
}
#endif

static char* cfg_type_accesscontrol_index[] = { 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_accesscontrol_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesscontrol_func_commit 
}; 


static cfg_node_type_t cfg_type_accesscontrol_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_accesscontrol, 
	 .index = cfg_type_accesscontrol_index,
	 .ops = &cfg_type_accesscontrol_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_accesscontrol_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesscontrol_func_commit 
}; 


static cfg_node_type_t cfg_type_accesscontrol_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_accesscontrol, 
	 .ops = &cfg_type_accesscontrol_common_ops, 
}; 


static cfg_node_ops_t cfg_type_accesscontrol_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesscontrol_func_commit 
}; 


static cfg_node_type_t* cfg_type_accesscontrol_child[] = { 
	 &cfg_type_accesscontrol_entry, 
	 &cfg_type_accesscontrol_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_accesscontrol = { 
	 .name = "AccessControl", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_accesscontrol_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_accesscontrol_child, 
	 .ops = &cfg_type_accesscontrol_ops, 
}; 
