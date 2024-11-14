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



static char* cfg_type_interface_rate_index[] = { 
	 "interfacerate_id", 
	 NULL 
}; 

static void update_wan_tx_rx_rate(char *path)
{
    char tmp[32] = {0}, wan_path[32] = {0};
    int itf_index = 0;
    int wan_index = 0;
    unsigned char wanid = 0;
	wan_acnt_t wan_acnt;

    memset(tmp, 0, sizeof(tmp));

	sscanf(path, "root.interfacerate.entry.%d", &wan_index);	

	snprintf(wan_path, sizeof(wan_path), WANINFO_ENTRY_NODE, wan_index);
	if((cfg_obj_get_object_attr(wan_path, "Status", 0, tmp, sizeof(tmp)) <= 0) 
		&& 0 != strcmp(tmp, "up")) 
	{		
		tcdbg_printf("%s %d\n the wan status is down ", __FUNCTION__, __LINE__);
		return;
	}
    
	wanid = (unsigned char)(wan_index - 1); 
    
    memset(&wan_acnt, 0, sizeof(wan_acnt));
    blapi_traffic_get_wan_acnt(wanid, &wan_acnt);   
    /*Need sleep 1s to get the rate in the last 1s*/
    sleep(1);
    memset(&wan_acnt, 0, sizeof(wan_acnt));
    blapi_traffic_get_wan_acnt(wanid, &wan_acnt);   

	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
		cfg_obj_create_object(path);

    memset(tmp, 0, sizeof(tmp));    
    snprintf(tmp, sizeof(tmp), "%llu", wan_acnt.txRate);
	if ( -1 == cfg_set_object_attr(path, "txRate", tmp) )
	{
		tcdbg_printf("------------------------------set failed %d\n", __LINE__);
		return;
	}
   
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%llu", wan_acnt.rxRate);
	if ( -1 == cfg_set_object_attr(path, "rxRate", tmp) )
	{
		tcdbg_printf("------------------------------set failed %d\n", __LINE__);
		return;
	}	 
	
	
    
    return;
}

int cfg_type_interface_rate_entry_get(char* path,char* attr,char* val,int len)
{
	update_wan_tx_rx_rate(path);

	return cfg_type_default_func_get(path, attr, val, len);
}

static cfg_node_ops_t cfg_type_interfacerate_entry_ops  = { 
	 .get = cfg_type_interface_rate_entry_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_interfacerate_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_interfacerate, 
	 .index = cfg_type_interface_rate_index, 
	 .ops = &cfg_type_interfacerate_entry_ops, 
}; 

static cfg_node_type_t* cfg_type_interfacerate_child[] = { 
	 &cfg_type_interfacerate_entry, 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_interfacerate_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


cfg_node_type_t cfg_type_interfacerate = { 
	 .name = "InterfaceRate", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_interfacerate_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_interfacerate_child, 
	 .ops = &cfg_type_interfacerate_ops, 
}; 
