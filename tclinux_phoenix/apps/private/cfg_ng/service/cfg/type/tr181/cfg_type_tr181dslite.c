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

static char* cfg_type_tr181dslite_index[] = { 
	 "dsLite_Idx", 
	 NULL 
}; 




int syncWan2Dslite(int wanIf,int ipIndex)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int dsliteIdx = -1;
	int flag = 0;
	char strNum[8] = {0};

	flag = checkEntryExistByPvcLanIdx(&dsliteIdx, TR181_DSLITE_NODE, TR181_DSLITE_NUM, wanIf, 1);
	if(dsliteIdx < 0){
		tcdbg_printf("[%s:%d]Entry is not exist, Error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DSLITE_ENTRY_NODE, dsliteIdx+1);

	if(flag == 0){
		/*Entry is not exist*/
		cfg_obj_create_object(nodeName);
	
		/*modify num, need +1*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(TR181_DSLITE_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));		
		snprintf(strNum, sizeof(strNum), "%d", atoi(tmp) + 1); 
		cfg_obj_set_object_attr(TR181_DSLITE_COMMON_NODE, TR181_NUM_ATTR, 0, strNum);
	}

	memset(wanNode, 0, sizeof(wanNode));
	if(setWanNodeByWanIf(wanNode,wanIf) < 0)
	{
		return -1;
	}
	
	/*Enable*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(wanNode, "DsliteEnable", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"Yes")){
		cfg_obj_set_object_attr(nodeName, "Enable", 0, "1");
	}
	else if(cfg_obj_get_object_attr(wanNode, "DsliteEnable", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"No")){
		cfg_obj_set_object_attr(nodeName, "Enable", 0, "0");
	}

	/*Interface*/
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp,sizeof(tmp), TR181_IP_INTERFACE"%d", ipIndex + 1);
	cfg_obj_set_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, tmp);
		
	/*Mode*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(wanNode, "DsliteMode", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"1")){/*manual*/
		cfg_obj_set_object_attr(nodeName, "EndAssPrec", 0, tmp);
		cfg_obj_set_object_attr(nodeName, "Origin", 0, tmp);
		
		/*sync dsliteAddr*/
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(wanNode, "DsliteAddr", 0, tmp, sizeof(tmp));
		cfg_obj_set_object_attr(nodeName, "EndpointAddr", 0, tmp);
	}
	else if(cfg_obj_get_object_attr(wanNode, "DsliteMode", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"0")){/*auto*/
		cfg_obj_set_object_attr(nodeName, "EndAssPrec", 0, tmp);
		cfg_obj_set_object_attr(nodeName, "Origin", 0, tmp);
	}
	
	/*PvcIndex*/
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"%d",wanIf);
	cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);	

	return 0;
}



int cfg_type_dslite_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};
	int etyIdx = -1;
	char strInterface[128] = {0};
	int ipifIdx = -1;
	int newwanIf = -1;
	int oldwanIf = -1;
	char wanNode[MAXLEN_NODE_NAME] = {0};
	
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		return -1;
	}
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_DSLITE_ENTRY_NODE, etyIdx);
		
	memset(strInterface, 0, sizeof(strInterface));
	cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, strInterface, sizeof(strInterface));
	ipifIdx = getSuffixIntByInterface(strInterface);
	newwanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipifIdx);
	oldwanIf = getPvcIdxByTR181NodeEntry(TR181_DSLITE_NODE, etyIdx);

	if(newwanIf!=oldwanIf)
	{
		if(oldwanIf >= 0)
		{
			setWanNodeByWanIf(wanNode,oldwanIf);	
		
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(wanNode, "DsliteEnable", 0, tmp, sizeof(tmp));
			if(strcmp(tmp, "Yes") == 0)
			{ 
				cfg_obj_set_object_attr(wanNode, "DsliteEnable", 0, "No");
			}
			clearOtherIneterfaceByWanIf(TR181_DSLITE_NODE,TR181_DSLITE_NUM,etyIdx,newwanIf);
			commitWanByWanIf(oldwanIf);
		}
	}

	if(newwanIf >= 0)
	{		
		memset(wanNode, 0, sizeof(wanNode));
		if(setWanNodeByWanIf(wanNode,newwanIf) < 0)
		{
			return -1;
		}
		
		/*Enable*/
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"1"))
		{
			cfg_obj_set_object_attr(wanNode, "DsliteEnable", 0, "Yes");
		}
		else if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"0"))
		{	
			cfg_obj_set_object_attr(wanNode, "DsliteEnable", 0, "No");
		}

		/*Mode*/
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "EndAssPrec", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"1"))
		{
			cfg_obj_set_object_attr(wanNode, "DsliteMode", 0, tmp);

			/*sync dsliteAddr*/
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(nodeName, "EndpointAddr", 0, tmp, sizeof(tmp));
			cfg_obj_set_object_attr(wanNode, "DsliteAddr", 0, tmp);
		}
		else if(cfg_obj_get_object_attr(nodeName, "EndAssPrec", 0, tmp, sizeof(tmp)) >= 0 && !strcmp(tmp,"0"))
		{
			cfg_obj_set_object_attr(wanNode, "DsliteMode", 0, tmp);
		}
		
		/*set pvcIndex*/
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"%d",newwanIf);
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);
		commitWanByWanIf(newwanIf);
	}

	return 0;
}



static cfg_node_ops_t cfg_type_tr181dslite_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dslite_func_commit 
}; 


static cfg_node_ops_t cfg_type_tr181dslite_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dslite_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181dslite_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_DSLITE_NUM, 
	 .parent = &cfg_type_tr181dslite, 
	 .index = cfg_type_tr181dslite_index,
	 .ops = &cfg_type_tr181dslite_entry_ops, 
};

static cfg_node_type_t cfg_type_tr181dslite_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181dslite, 
	 .ops = &cfg_type_tr181dslite_common_ops, 
};

static cfg_node_ops_t cfg_type_tr181dslite_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dslite_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181dslite_child[] = { 
	 &cfg_type_tr181dslite_entry, 
     &cfg_type_tr181dslite_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_tr181dslite= { 
	 .name = "TR181DSLite", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181dslite_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181dslite_child, 
	 .ops = &cfg_type_tr181dslite_ops, 
}; 
