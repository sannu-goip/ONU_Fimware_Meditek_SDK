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


int checkDSLChannelEnable(int entryIdx)
{
	char nodeName[32] = {0};
	char lowerlayer[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;

	if( entryIdx < 0 )
	{
		tcdbg_printf("[%s:%d] entryIdx=%d, error!\n",__FUNCTION__,__LINE__,entryIdx);
		return -1;
	}
	
	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_CHANNEL_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		
		tcdbg_printf("[%s:%d]lowerlayer not pass.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	nextEntryIdx = getSuffixIntByInterface(lowerlayer);
	if( nextEntryIdx < 0){
		return -1;
	}

	if(strstr(lowerlayer, TR181_DSL_LINE_INTERFACE) != NULL)
	{
		
		/*the next layer is DSLLine: no need to check*/	
		ret = checkDSLineEnable(nextEntryIdx);
	}

	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_CHANNEL_ENTRY_NODE, entryIdx);

	if( (cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable)) >= 0)
		&& strcmp(enable, "1") == 0 && ret > 0)
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


void setDSLChannelLowerLayer(int idx, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_CHANNEL_ENTRY_NODE, idx + 1);

	cfg_obj_set_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer);

	return;
}

int cfg_type_tr181dslChannel_func_commit(char *path)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char dslChannelEnable[4] = {0};
	int dslChannelIdx = -1;
	char tmp[32] = {0};
	char node[MAXLEN_NODE_NAME] = {0};

	if ( get_entry_number_cfg2(path, "entry.", &dslChannelIdx) != SUCCESS ) 
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "dslChannel_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			dslChannelIdx = atoi(tmp) + 1;
		}
		else
		{
			return FAIL;
		}
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DSL_CHANNEL_ENTRY_NODE, dslChannelIdx);
	
	cfg_obj_get_object_attr(nodeName, "Enable", 0, dslChannelEnable, sizeof(dslChannelEnable));

	setWanConnectionActive(dslChannelEnable);
		
	commitWanByWanIf(-1);
	return 0;
}

static cfg_node_ops_t cfg_type_tr181dslChannel_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dslChannel_func_commit
}; 


static cfg_node_type_t cfg_type_tr181dslChannel_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DSL_CHANNEL_NUM, 
	 .parent = &cfg_type_tr181dslChannel, 
	 .ops = &cfg_type_tr181dslChannel_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181dslChannel_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dslChannel_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181dslChannel_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181dslChannel, 
	 .ops = &cfg_type_tr181dslChannel_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181dslChannel_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181dslChannel_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181dslChannel_child[] = { 
	 &cfg_type_tr181dslChannel_common, 
	 &cfg_type_tr181dslChannel_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181dslChannel = { 
	 .name = "DSLChannel", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181dslChannel_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181dslChannel_child, 
	 .ops = &cfg_type_tr181dslChannel_ops, 
}; 
