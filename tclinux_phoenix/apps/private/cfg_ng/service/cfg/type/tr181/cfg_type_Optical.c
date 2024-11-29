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

int checkOpticalEnable(void)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char enable[4] = {0};

	/*check enable*/
	memset(enable, 0, sizeof(enable));	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, TR181_OPTICAL_ENTRY_0_NODE, sizeof(nodeName) - 1);

	if((cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable)) >= 0)
		&& strcmp(enable, "1") == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
	
}

int cfg_type_optical_entry_func_get(char* path,char* attr,char* val,int len)
{
	int i = 0;
	char tmp[16] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};

	if(cfg_query_object(path,NULL,NULL) < 0)
	{
		cfg_create_object(path);      
	}
	
	if(0 == strcmp(attr, "Status"))
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(path, "Enable", 0, tmp, sizeof(tmp)) > 0 && '\0' != tmp[0])
		{
			if( 0 == strcmp(tmp, "1")){
				cfg_obj_set_object_attr(path, attr, 0, "Up");	
			}
			else{
				cfg_obj_set_object_attr(path, attr, 0, "Down");	
			}
		}
		else
		{
			cfg_obj_set_object_attr(path, attr, 0, "Down");
			for(i = 0; i < WAN_INTERFACE_NUM; i++)
			{
				setWanNodeByWanIf(nodeName, i);
				if(cfg_obj_get_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) >= 0){
					if(0 == strcmp(tmp, "Yes")){
						cfg_obj_set_object_attr(path, attr, 0, "Up");
						break;
					}
				}
			}
		}
	}

	return cfg_obj_get_object_attr(path, attr, 0, val, len);
}

int cfg_type_optical_func_commit(char* path)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char ipIFNode[MAXLEN_NODE_NAME] = {0};
	char enable[2] = {0};
	int i = 0, ret = -1;
	char Active[4] = {0};
	char tmp[4] = {0};
	char node[MAXLEN_NODE_NAME] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, TR181_OPTICAL_ENTRY_0_NODE, sizeof(nodeName) - 1);

	cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable));

	if(0 ==  strcmp(enable, "0"))
	{/*Disable*/
		for(i = 0; i < WAN_INTERFACE_NUM; i++)
		{
			setWanNodeByWanIf(nodeName, i);
			if(cfg_obj_get_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, Active, sizeof(Active)) >= 0)
			{
				if(0 == strcmp(Active, "Yes"))
				{
					cfg_obj_set_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, "No");/*set wan interface disable*/
				}
			}
		}
	}
	else
	{/*Enable*/
		for(i = 0; i < WAN_INTERFACE_NUM; i++)
		{
			/*check wan by lowerlayer*/	
			ret = checkIPIFEnableByIdx(i + 1);
			if(1 == ret)
			{
				memset(ipIFNode, 0, sizeof(ipIFNode));	
				snprintf(ipIFNode, sizeof(ipIFNode), TR181_IP_ENTRY_NODE, i + 1);
				cfg_obj_get_object_attr(ipIFNode, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp));
				setWanNodeByWanIf(nodeName, atoi(tmp));
				cfg_obj_set_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, "Yes");/*set wan interface enable*/
			}
		}		
	}
	
	commitWanByWanIf(-1);

	return 0;
}

static cfg_node_ops_t cfg_type_optical_entry_ops  = { 
	 .get = cfg_type_optical_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_optical_func_commit 
}; 


static cfg_node_type_t cfg_type_optical_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_OPTICAL_MAXENRTRYNUM, 
	 .parent = &cfg_type_optical, 
	 .ops = &cfg_type_optical_entry_ops, 
};

static cfg_node_ops_t cfg_type_optical_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_optical_func_commit 
}; 


static cfg_node_type_t cfg_type_optical_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_optical, 
	 .ops = &cfg_type_optical_common_ops, 
}; 


static cfg_node_ops_t cfg_type_optical_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_optical_func_commit 
}; 


static cfg_node_type_t* cfg_type_optical_child[] = { 
	 &cfg_type_optical_common, 
	 &cfg_type_optical_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_optical = { 
	 .name = "OpticalInter", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_optical_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_optical_child, 
	 .ops = &cfg_type_optical_ops, 
}; 
