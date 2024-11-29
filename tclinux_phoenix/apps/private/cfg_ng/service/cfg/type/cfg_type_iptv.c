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
#include "cfg_msg.h"
#include "utility.h"

int cfg_type_iptv_func_execute(void )
{
	char ifname[16] = {0};
	char nostbenable[8] = {0};
	char state[8] = {0};
	char cmd[128] = {0};

	/* kill udpxy */
	system("killall udpxy");

	if( cfg_get_object_attr(IPTV_COMMON_NODE, "NoStbEnable", nostbenable, sizeof(nostbenable)) <= 0 || 
		strncmp(nostbenable, "Yes", sizeof(nostbenable) - 1) )
		return 0;
	
	cfg_get_object_attr(IPTV_ENTRY_NODE,"status",state,sizeof(state));
	cfg_get_object_attr(IPTV_ENTRY_NODE,"ifname",ifname,sizeof(ifname));

	if(!strncmp(state, "up", sizeof(state)-1))
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/udpxy -a br0 -p 9090 -m %s -c 6 -B 1M -R 16 -H 2 &", ifname);
		system(cmd);
	}
	
	return 0;	
}

int svc_cfg_boot_iptv(void)
{
	return cfg_type_iptv_func_execute();
}

int cfg_type_iptv_func_commit(char* path)
{
	return cfg_type_iptv_func_execute();
}

static cfg_node_ops_t cfg_type_iptv_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_iptv_func_commit 
}; 

static cfg_node_ops_t cfg_type_iptv_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_iptv_func_commit 
}; 

static cfg_node_type_t cfg_type_iptv_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_iptv, 
	 .ops = &cfg_type_iptv_entry_ops, 
}; 

static cfg_node_type_t cfg_type_iptv_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_iptv, 
	 .ops = &cfg_type_iptv_common_ops, 
}; 

static cfg_node_ops_t cfg_type_iptv_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_iptv_func_commit, 
}; 

static cfg_node_type_t* cfg_type_iptv_child[] = { 
	 &cfg_type_iptv_entry, 
	 &cfg_type_iptv_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_iptv = { 
	 .name = "IPTV", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_iptv_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_iptv_child, 
	 .ops = &cfg_type_iptv_ops, 
}; 

