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


int checkDSLineEnable(int entryIdx)
{
	char nodeName[32] = {0};
	char enable[4] = {0};

	if( entryIdx < 0 )
	{
		tcdbg_printf("[%s:%d] entryIdx=%d, error!\n",__FUNCTION__,__LINE__,entryIdx);
		return -1;
	}
	
	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_CHANNEL_ENTRY_NODE, entryIdx);

	if( (cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable)) >= 0)
		&& strcmp(enable, "1") == 0)
	{
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}
	else
	{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
	
}

int cfg_type_tr181dsline_func_commit(char *path)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char dslLineEnable[4] = {0};
	int dslLineIdx = -1;
	char tmp[32] = {0};
	char node[MAXLEN_NODE_NAME] = {0};

	if ( get_entry_number_cfg2(path, "entry.", &dslLineIdx) != SUCCESS ) 
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "dslLine_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			dslLineIdx = atoi(tmp) + 1;
		}
		else
		{
			return FAIL;
		}
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_LINE_ENTRY_NODE, dslLineIdx);

	cfg_obj_get_object_attr(nodeName, "Enable", 0, dslLineEnable, sizeof(dslLineEnable));

	setWanConnectionActive(dslLineEnable);
	
	commitWanByWanIf(-1);

	return 0;
}

static cfg_node_ops_t cfg_type_tr181dsline_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dsline_func_commit
}; 


static cfg_node_type_t cfg_type_tr181dsline_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DSL_LINE_NUM, 
	 .parent = &cfg_type_tr181dslline, 
	 .ops = &cfg_type_tr181dsline_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181dslline_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dsline_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181dslline_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181dslline, 
	 .ops = &cfg_type_tr181dslline_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181dslline_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dsline_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181dslline_child[] = { 
	 &cfg_type_tr181dslline_common, 
	 &cfg_type_tr181dsline_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181dslline = { 
	 .name = "DSLLine", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181dslline_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181dslline_child, 
	 .ops = &cfg_type_tr181dslline_ops, 
}; 
