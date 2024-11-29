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
#include "blapi_traffic.h"

int svc_cfg_boot_bandwidth(void)
{
	char enable[8]={0};
	char cmd[64] = {0};
	char nodePath[64] = {0};
#if !defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
	snprintf(nodePath, sizeof(nodePath), BANDWIDTH_COMMON_NODE);
	if((cfg_obj_get_object_attr(nodePath, "Enable", 0, enable, sizeof(enable)) < 0) ||  strlen(enable) == 0 || strcmp(enable,"0") == 0)
	{
		blapi_traffic_set_dev_acnt_enable(0,0);
	}
	else
	{
		blapi_traffic_set_dev_acnt_enable(1,1);
	}
#endif

	return 0;
}

int cfg_type_bandwidth_execute( )
{	
	return svc_cfg_boot_bandwidth( );
}

int cfg_type_bandwidth_func_commit(char* path)
{	
	return cfg_type_bandwidth_execute( );
}

static char* cfg_type_bandwidth_index[] = { 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_bandwidth_lanport4_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_lanport4 = { 
	 .name = "LANPORT4", 
	 .flag = 1,
	 .parent = &cfg_type_bandwidth, 
	 .ops = &cfg_type_bandwidth_lanport4_ops, 
}; 

static cfg_node_ops_t cfg_type_bandwidth_lanport3_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_lanport3 = { 
	 .name = "LANPORT3", 
	 .flag = 1, 
	 .parent = &cfg_type_bandwidth, 
	 .ops = &cfg_type_bandwidth_lanport3_ops, 
}; 

static cfg_node_ops_t cfg_type_bandwidth_lanport2_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_lanport2 = { 
	 .name = "LANPORT2", 
	 .flag = 1, 
	 .parent = &cfg_type_bandwidth,  
	 .ops = &cfg_type_bandwidth_lanport2_ops, 
}; 

static cfg_node_ops_t cfg_type_bandwidth_lanport1_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_lanport1 = { 
	 .name = "LANPORT1", 
	 .flag = 1,
	 .parent = &cfg_type_bandwidth, 
	 .ops = &cfg_type_bandwidth_lanport1_ops, 
}; 

static cfg_node_ops_t cfg_type_bandwidth_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 30, 
	 .parent = &cfg_type_bandwidth, 
	 .index = cfg_type_bandwidth_index, 
	 .ops = &cfg_type_bandwidth_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_bandwidth_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_bandwidth, 
	 .ops = &cfg_type_bandwidth_common_ops, 
}; 


static cfg_node_ops_t cfg_type_bandwidth_devicespeed_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t cfg_type_bandwidth_devicespeed = { 
	 .name = "DeviceSpeed", 
	 .flag = 1, 
	 .parent = &cfg_type_bandwidth, 
	 .ops = &cfg_type_bandwidth_devicespeed_ops, 
}; 

static cfg_node_ops_t cfg_type_bandwidth_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_bandwidth_func_commit 
}; 


static cfg_node_type_t* cfg_type_bandwidth_child[] = { 
	 &cfg_type_bandwidth_entry,
	 &cfg_type_bandwidth_lanport1, 
	 &cfg_type_bandwidth_lanport2, 
	 &cfg_type_bandwidth_lanport3, 
	 &cfg_type_bandwidth_lanport4, 
	 &cfg_type_bandwidth_common, 
	 &cfg_type_bandwidth_devicespeed, 
	 NULL 
}; 


cfg_node_type_t cfg_type_bandwidth = { 
	 .name = "BandWidth", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_bandwidth_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_bandwidth_child, 
	 .ops = &cfg_type_bandwidth_ops, 
}; 
