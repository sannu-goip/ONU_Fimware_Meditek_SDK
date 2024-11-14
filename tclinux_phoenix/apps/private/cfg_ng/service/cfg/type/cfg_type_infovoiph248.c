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
#include <stdlib.h>
#include <cfg_cli.h> 
#include <svchost_evt.h>

#include "cfg_types.h" 
#include "cfg_type_voip.h"
#include "utility.h"

int cfg_type_infovoiph248_entry_get(char* path,char* attr,char* val,int len)
{
	char VoIPH248_node[32] = {0};
	int lineIdx = 0;

	if(strstr(path, "entry"))
		sscanf(path, INFOVOIPH248_ENTRY_NODE, &lineIdx);
	else if(strstr(path, "Entry"))
		sscanf(path, "root.InfoVoIPH248.Entry.%d", &lineIdx);

	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
		cfg_obj_create_object(path);

    /* update voip line status */    
    if (!strcmp(attr, "RegFailReason") && isVoIPAppDown(1))
    {
        memset(VoIPH248_node, 0, sizeof(VoIPH248_node));
        
        /* 1. update InfoVoIPH248_Entry%d Status to be Error 
           2. update InfoVoIPH248_Entry%d RegFailReason to be 1(IAD error)*/
        snprintf(VoIPH248_node, sizeof(VoIPH248_node), VOIPH248_ENTRY_NODE, lineIdx);
        cfg_set_object_attr(VoIPH248_node, "UserServiceState", "5");/*5-Register fail*/
        cfg_set_object_attr(path, "RegFailReason", "1");
    }

	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_infovoiph248_common_set(char* path,char* attr,char* val)
{
	/*printf("func: %s, line: %d\n", __func__, __LINE__);
	if (cfg_obj_query_object("root.infovoiph248", NULL, NULL) <= 0)
		cfg_obj_create_object("root.infovoiph248");
	
	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
		cfg_obj_create_object(path);

	printf("func: %s, line: %d\n", __func__, __LINE__);*/
	return cfg_obj_set_object_attr(path,attr,0,val);
}


static cfg_node_ops_t cfg_type_infovoiph248_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit, 
}; 

static cfg_node_ops_t cfg_type_infovoiph248_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit, 
}; 

static cfg_node_ops_t cfg_type_infovoiph248_entry_ops  = { 
	 .get = cfg_type_infovoiph248_entry_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit,
}; 

cfg_node_type_t cfg_type_infovoiph248_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE,
	 .parent = &cfg_type_infovoiph248, 
	 .ops = &cfg_type_infovoiph248_common_ops, 
}; 

cfg_node_type_t cfg_type_infovoiph248_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MULTIPLE|CFG_TYPE_MAX_VOIP_LINE_NUM,
	 .parent = &cfg_type_infovoiph248, 
	 .ops = &cfg_type_infovoiph248_entry_ops, 
}; 


static cfg_node_type_t* cfg_type_infovoiph248_child[] = { 
	 &cfg_type_infovoiph248_common, 
	 &cfg_type_infovoiph248_entry, 
	 NULL 
};


cfg_node_type_t cfg_type_infovoiph248 = { 
	 .name = "InfoVoIPH248", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE|1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_infovoiph248_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_infovoiph248_child, 
	 .ops = &cfg_type_infovoiph248_ops, 
}; 

