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

static char* cfg_type_users_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int updateUsers(void)
{
	char buf[256] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tr181node[MAXLEN_NODE_NAME] = {0};
	int tr181UsersIdx = -1;
	int i = 0;

	for(i = 0; i < USERS_NUM_MAX; i++){
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, i + 1);

		memset(tr181node, 0, sizeof(tr181node));
		snprintf(tr181node,sizeof(tr181node),TR181_USERS_ENTRY_NODE, i + 1);

		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "username", 0, buf, sizeof(buf)) > 0){
			if(cfg_query_object(tr181node,NULL,NULL) <= 0)
			{		
				tr181UsersIdx = cfg_obj_create_object(tr181node);
				if(tr181UsersIdx < 0) 
				{
					tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,tr181node);
					return -1;
				}
			}
			cfg_obj_set_object_attr(tr181node, "Enable", 0, "1");
			cfg_obj_set_object_attr(tr181node, "Username", 0, buf);
			cfg_obj_set_object_attr(tr181node, "Alias", 0, buf);
		}
		else{
			continue;
		}

		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "web_passwd", 0, buf, sizeof(buf)) >= 0){
			cfg_obj_set_object_attr(tr181node, "Password", 0, buf);
		}
		
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "Active", 0, buf, sizeof(buf)) >= 0){
			if(!strcmp(buf,"Yes"))
				strncpy(buf,"1",sizeof(buf));
			else
				strncpy(buf,"0",sizeof(buf));
			cfg_obj_set_object_attr(tr181node, "Enable", 0, buf);
		}
		cfg_obj_set_object_attr(tr181node, "RmtAccCap", 0, "0");
	}
	return 0;
}


int cfg_type_users_func_commit(char *path)
{
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	int accountEntryIdx = -1;
	int etyIdx = -1;
	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}

	memset(tr181node, 0, sizeof(tr181node));
	snprintf(tr181node,sizeof(tr181node),TR181_USERS_ENTRY_NODE, etyIdx);

	/*username */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Username", 0, tmp, sizeof(tmp)) > 0)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, etyIdx);
		if(cfg_query_object(nodeName,NULL,NULL) <= 0)
		{		
			accountEntryIdx = cfg_obj_create_object(nodeName);
			if(accountEntryIdx < 0) 
			{
				tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
				return -1;
			}
		}
		cfg_obj_set_object_attr(nodeName, "username", 0, tmp);
	}

	/* password */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Password", 0, tmp, sizeof(tmp)) >= 0)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, etyIdx);
		if(cfg_query_object(nodeName,NULL,NULL) <= 0)
		{		
			accountEntryIdx = cfg_obj_create_object(nodeName);
			if(accountEntryIdx < 0) 
			{
				tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
				return -1;
			}
		}
		
		cfg_obj_set_object_attr(nodeName, "web_passwd", 0, tmp);
	}

	/* enable */
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Enable",0,tmp, sizeof(tmp)) >= 0)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, etyIdx);
		if(cfg_query_object(nodeName,NULL,NULL) <= 0)
		{		
			accountEntryIdx = cfg_obj_create_object(nodeName);
			if(accountEntryIdx < 0) 
			{
				tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
				return -1;
			}
		}
		if(!strcmp(tmp,"1"))
			strncpy(tmp,"Yes",sizeof(tmp));
		else
			strncpy(tmp,"No",sizeof(tmp));
		
		cfg_obj_set_object_attr(nodeName, "Active", 0, tmp);
	}

	cfg_tr181_commit_object(ACCOUNT_NODE);
	return 0;
}


static cfg_node_ops_t cfg_type_users_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_users_func_commit 
}; 


static cfg_node_type_t cfg_type_users_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | USERS_NUM_MAX, 
	 .parent = &cfg_type_users, 
	 .index = cfg_type_users_index, 
	 .ops = &cfg_type_users_entry_ops, 
};

static cfg_node_ops_t cfg_type_users_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_users_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_users, 
	 .ops = &cfg_type_users_common_ops, 
};


static cfg_node_ops_t cfg_type_users_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_users_func_commit 
}; 


static cfg_node_type_t* cfg_type_users_child[] = { 
	 &cfg_type_users_entry, 
	 &cfg_type_users_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_users= { 
	 .name = "Users", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_users_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_users_child, 
	 .ops = &cfg_type_users_ops, 
}; 
