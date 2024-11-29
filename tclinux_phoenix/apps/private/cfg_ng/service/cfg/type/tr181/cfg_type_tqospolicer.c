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
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h" 

int clearQosPolicer()
{
	char tr181node[MAXLEN_NODE_NAME] = {0};
	
	memset(tr181node, 0, sizeof(tr181node) );
	snprintf(tr181node, sizeof(tr181node), TR181_TQOSPOLICER_ENTRY_NODE, 1);
	if(cfg_query_object(tr181node,NULL,NULL) >= 0)
	{
		cfg_tr181_delete_object(tr181node);
	}
	
	return 0;
}


int initQosPolicer(void)
{
	int ret = -1;
	char tmp[64] = {0};
	char nodeName[MAXLEN_NODE_NAME] ={0};
	char tr181node[MAXLEN_NODE_NAME] = {0};

	memset(tr181node, 0x00, sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_TQOSPOLICER_ENTRY_NODE, 1);
	memset(nodeName, 0x00, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), QOS_COMMON_NODE);

	if(cfg_query_object(tr181node,NULL,NULL) < 0)
	{
		ret = cfg_create_object(tr181node);
		if(ret < 0)
		{
			return -1;
		}
	}
	
	cfg_obj_set_object_attr(tr181node, "Enable", 0, "0");
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "Active", 0, tmp, sizeof(tmp)) > 0)
	{
		if(!strcmp(tmp, "Yes"))
		{
			cfg_obj_set_object_attr(tr181node, "Enable", 0, "1");
		}
	}
	return 0;
}


static int cfg_type_tqospolicer_func_commit(char* path)
{
	int etyIdx = -1;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) > 0 &&  isNumber(tmp))
		{
			etyIdx = atoi(tmp) + 1;	
		}
		else
		{
			return -1;
		}
	}

	memset(tr181node, 0x00, sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_TQOSPOLICER_ENTRY_NODE, etyIdx);
	memset(nodeName, 0x00, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), QOS_COMMON_NODE);

	/* Enable */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Enable", 0, tmp, sizeof(tmp)) > 0)
	{
		if(!strcmp(tmp, "1"))
		{
			cfg_obj_set_object_attr(nodeName, "Active", 0, "Yes");
		}
		else if(!strcmp(tmp, "0"))
		{
			cfg_obj_set_object_attr(nodeName, "Active", 0, "No");
		}
	}
	
	cfg_tr181_commit_object(nodeName);
	
	return SUCCESS;
}

static cfg_node_ops_t cfg_type_tqospolicer_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqospolicer_func_commit 
}; 


static cfg_node_ops_t cfg_type_tqospolicer_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqospolicer_func_commit 
}; 

static cfg_node_type_t cfg_type_tqospolicer_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tqospolicer, 
	 .ops = &cfg_type_tqospolicer_common_ops, 
};


static cfg_node_type_t cfg_type_tqospolicer_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_tqospolicer, 
	 .ops = &cfg_type_tqospolicer_entry_ops, 
};


static cfg_node_ops_t cfg_type_tqospolicer_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqospolicer_func_commit 
}; 


static cfg_node_type_t* cfg_type_tqospolicer_child[] = { 
	 &cfg_type_tqospolicer_entry,
	 &cfg_type_tqospolicer_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_tqospolicer= { 
	 .name = "TQosPolicer", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tqospolicer) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tqospolicer_child, 
	 .ops = &cfg_type_tqospolicer_ops, 
}; 

