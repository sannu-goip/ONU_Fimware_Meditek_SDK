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

int clearQosQue()
{
	int i = 0;
	char tr181node[MAXLEN_NODE_NAME] = {0};	
	
	for(i = 0; i < QOS_QUE_NUM_MAX ; i++)
	{
		memset(tr181node, 0, sizeof(tr181node) );
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSQUE_ENTRY_NODE , i+1);	
		if(cfg_query_object(tr181node,NULL,NULL) >= 0)
		{
			cfg_tr181_delete_object(tr181node);
		}
	}
	
	return 0;
}

int initQosQue()
{
	int i = 0;
	int ret = -1;
	char buf[64] = {0};
	char Discipline[32] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char* weightAttr[] = {"QueueBW1", "QueueBW2", "QueueBW3", "QueueBW4", "QueueBW5", "QueueBW6"};
	char* enableAttr[] = {"QueueSW1", "QueueSW2", "QueueSW3", "QueueSW4", "QueueSW5", "QueueSW6"};
	
	memset(nodeName, 0x00, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), QOS_COMMON_NODE);
	
	memset(Discipline, 0x00, sizeof(Discipline));
	cfg_obj_get_object_attr(nodeName, "Discipline", 0, Discipline, sizeof(Discipline));
	if( !strcmp(Discipline, "PQ") ||'\0' == Discipline[0] )
	{
		memset(Discipline, 0x00, sizeof(Discipline));
		strncpy(Discipline, "SP", sizeof(Discipline)-1);
	}
	
	for(i = 0; i < QOS_QUE_NUM_MAX ; i++)
	{
		memset(tr181node, 0x00, sizeof(tr181node));
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSQUE_ENTRY_NODE, i+1);

		if(cfg_query_object(tr181node,NULL,NULL) < 0)
		{
			ret = cfg_create_object(tr181node);
			if(ret < 0)
			{
				return -1;
			}
		}

		/*Enable */
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, enableAttr[i], 0, buf, sizeof(buf)) >= 0 && '\0' != buf[0])
		{
			if(0 == strcmp(buf, "Yes"))
			{
				strncpy(buf, "1", sizeof(buf) -1);	
			}
			else
			{
				strncpy(buf, "0", sizeof(buf) -1);
			}
			cfg_obj_set_object_attr(tr181node, "Enable", 0, buf);
		}
		
		/*TraCls */
		memset(buf, 0x00, sizeof(buf));
		snprintf(buf,sizeof(buf), "%d", i);
		cfg_obj_set_object_attr(tr181node, "TraCls", 0, buf);

		/* Weight */ 
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, weightAttr[i], 0, buf, sizeof(buf)) >= 0 && '\0' != buf[0])
		{
			cfg_obj_set_object_attr(tr181node, "Weight", 0, buf);
		}
		else
		{
			cfg_obj_set_object_attr(tr181node, "Weight", 0, "0");
		}

		/*SchAlg */
		cfg_obj_set_object_attr(tr181node, "SchAlg", 0, Discipline);
	}

	return SUCCESS;
}

static int cfg_type_tqosque_func_commit(char* path)
{
	int etyIdx = -1;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char* weightAttr[] = {"QueueBW1", "QueueBW2", "QueueBW3", "QueueBW4", "QueueBW5", "QueueBW6"};
	char* enableAttr[] = {"QueueSW1", "QueueSW2", "QueueSW3", "QueueSW4", "QueueSW5", "QueueSW6"};
	char tmp[64] = {0};
	char node[MAXLEN_NODE_NAME] = {0};

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0 &&  isNumber(tmp))
		{
			etyIdx = atoi(tmp) + 1;	
		}
		else
		{
			return -1;
		}
	}
	
	memset(tr181node, 0x00, sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_TQOSQUE_ENTRY_NODE, etyIdx);
	memset(nodeName, 0x00, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), QOS_COMMON_NODE);

	/*Enable */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Enable", 0, tmp, sizeof(tmp)) >=0 && tmp[0] != '\0')
	{	
		if(0 == strcmp(tmp, "1"))
		{
			strncpy(tmp, "Yes", sizeof(tmp)-1);
		}
		else
		{
			strncpy(tmp, "No", sizeof(tmp)-1);
		}
		cfg_obj_set_object_attr(nodeName, enableAttr[etyIdx-1], 0, tmp);
	}
	
	/* Weight */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Weight", 0, tmp, sizeof(tmp)) > 0 && tmp[0] != '\0')
	{
		cfg_obj_set_object_attr(nodeName, weightAttr[etyIdx-1], 0, tmp);
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, weightAttr[etyIdx-1], 0, "0");
	}

	/* SchedulerAlgorithm */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SchAlg", 0, tmp, sizeof(tmp)) > 0)
	{
		if(!strcmp(tmp, "SP"))
		{
			memset(tmp, 0x00, sizeof(tmp));
			strncpy(tmp, "PQ", sizeof(tmp)-1);
		}
		cfg_obj_set_object_attr(nodeName, "Discipline", 0, tmp);
	}

	/*Commit */
	cfg_tr181_commit_object(nodeName);
	
	return SUCCESS;
}


static cfg_node_ops_t cfg_type_tqosque_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosque_func_commit 
}; 


static cfg_node_ops_t cfg_type_tqosque_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosque_func_commit 
}; 

static cfg_node_type_t cfg_type_tqosque_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tqosque, 
	 .ops = &cfg_type_tqosque_common_ops, 
};


static cfg_node_type_t cfg_type_tqosque_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | QOS_QUE_NUM_MAX |1, 
	 .parent = &cfg_type_tqosque, 
	 .ops = &cfg_type_tqosque_entry_ops, 
};


static cfg_node_ops_t cfg_type_tqosque_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosque_func_commit 
}; 


static cfg_node_type_t* cfg_type_tqosque_child[] = { 
	 &cfg_type_tqosque_entry,
	 &cfg_type_tqosque_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_tqosque= { 
	 .name = "TQosQue", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tqosque) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tqosque_child, 
	 .ops = &cfg_type_tqosque_ops, 
}; 

