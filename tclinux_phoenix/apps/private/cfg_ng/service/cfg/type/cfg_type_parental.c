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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"
#include "../other/parental_common.h"

#define MAX_PARENTAL_NUM	8
static char* cfg_type_parental_index[] = { 
	 "parental_id", 
	 NULL 
}; 

int svc_cfg_boot_parental(void)
{
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_PARENTAL_BOOT, NULL, 0); 
	return 0;
}

static int cfg_type_parental_func_commit(char *path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	tcdbg_printf("EVT_CFG_OTHER_PARENTAL_UPDATE. line = %d\n", __LINE__);
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_PARENTAL_UPDATE, (void*)&param, sizeof(param));
	return 0;
}

static int parental_del(char* path,char* attr,char* val){
	char *p = NULL;
	int idx = -1;	
	char tmp[32] = {0};
	char duraionAttr[64] = {0};
	int i = 0;
	char macPath[64] = {0};
	char macVal[32] = {0};
	char macAttr[32] ={0};
	
	p = strtok(val, ",");
	while(p != NULL)
	{
		idx = atoi(p);
		if(idx < 0)
			return FAIL;

		if(0 == strcmp(attr, "delMAC"))
		{
			memset(macAttr,0,sizeof(macAttr));
			snprintf(macAttr,sizeof(macAttr),"MAC%d",idx);
			
			cfg_obj_get_object_attr(path,macAttr,0,macVal,sizeof(macVal));
			for(i = 0; i < MAX_PARENTAL_MAC_NUM; i++)
			{
				memset(macPath,0,sizeof(macPath));
				snprintf(macPath,sizeof(macPath),"%s.%d", PARENTAL_MAC_ENTRY_NODE, i + 1);
				memset(tmp,0,sizeof(tmp));
				if(cfg_obj_get_object_attr(macPath,"Mac",0,tmp,sizeof(tmp)) > 0 && 0 == strcmp(macVal,tmp)){
					cfg_obj_delete_object(macPath);
					break;
				}
			}
			cfg_obj_set_object_attr(path,macAttr,0,"");	
		}
		else if( 0 == strcmp(attr, "delURL"))
		{
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),"URL%d",idx);
			cfg_obj_set_object_attr(path,tmp,0,"");		
		}
		else if(0 == strcmp(attr, "delTime"))
		{
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),"StartTime%d", idx);
			cfg_obj_set_object_attr(path,tmp,0,"");
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),"EndTime%d", idx);
			cfg_obj_set_object_attr(path,tmp,0,"");
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),"RepeatDay%d", idx);
			cfg_obj_set_object_attr(path,tmp,0,"");	
		}else
		{
			return FAIL;
		}
		p = strtok(NULL, ",");
	}

	return SUCCESS;
}

static int cfg_type_parental_func_set(char* path,char* attr,char* val){
	char tmp[32] = {0};
	char macPath[64] = {0};
	char ParentalID[4] = {0};
	char macAttr[64] = {0};
	char urlAttr[64] = {0};
	char duraionAttr[64] = {0};
	char addIdx[16] = {0};
	int i = 0;
	int idx = -1;
	int delIdx[32] = {0};
	int sec = -1;
	char bit[4] = {0};
	char setValue[32] = {0};
	int flag = 0;
	
	memset(addIdx,0,sizeof(addIdx));
	cfg_obj_get_object_attr(PARENTAL_COMMON_NODE,"AddIndx",0,addIdx,sizeof(addIdx));
	if(0 == strcmp(attr, "MAC"))
	{
		if(atoi(addIdx) > MAX_MAC_PER_ENTRY)
			return FAIL;
		memset(macAttr,0,sizeof(macAttr));
		snprintf(macAttr,sizeof(macAttr),"MAC%s",addIdx);
		cfg_obj_set_object_attr(path,macAttr,0,val);
											
		for(i = 0; i < MAX_PARENTAL_MAC_NUM; i++)
		{
			memset(macPath,0,sizeof(macPath));
			snprintf(macPath,sizeof(macPath),"%s.%d", PARENTAL_MAC_ENTRY_NODE, i + 1);
			memset(tmp,0,sizeof(tmp));
			if(cfg_obj_get_object_attr(macPath,"Mac",0,tmp,sizeof(tmp)) <= 0 ){
				if(cfg_query_object(macPath,NULL,NULL) <= 0)
				{		
					idx = cfg_obj_create_object(macPath);
					if(idx < 0) 
					{
						return FAIL;
					}
				}
				memset(tmp,0,sizeof(tmp));
				cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE,"parental_id",0,tmp,sizeof(tmp));
				cfg_obj_set_object_attr(macPath,"Mac",0,val);
				memset(ParentalID,0,sizeof(ParentalID));
				snprintf(ParentalID,sizeof(ParentalID),"%d",atoi(tmp));
				if(0 == atoi(tmp))
				{
					cfg_obj_set_object_attr(path,PARENTAL_DURATION_ACTIVE,0,"Yes");
					cfg_obj_set_object_attr(path,PARENTAL_DURATION_POLICY,0,"White");
				}
				return cfg_obj_set_object_attr(macPath,"ParentalID",0,ParentalID);	
			}
		}
	}

	if(0 == strcmp(attr, "URL"))
	{
		memset(urlAttr,0,sizeof(urlAttr));
		snprintf(urlAttr,sizeof(urlAttr),"URL%s", addIdx);
		return cfg_obj_set_object_attr(path,urlAttr,0,val);
		
	}
	if(0 == strcmp(attr, "StartTime"))
	{
		memset(duraionAttr,0,sizeof(duraionAttr));
		snprintf(duraionAttr,sizeof(duraionAttr),"StartTime%s", addIdx);
		snprintf(tmp,sizeof(tmp),"%d",sec);

		return cfg_obj_set_object_attr(path,duraionAttr,0,val);
	}

	if(0 == strcmp(attr, "EndTime"))
	{
		memset(duraionAttr,0,sizeof(duraionAttr));
		snprintf(duraionAttr,sizeof(duraionAttr),"EndTime%s", addIdx);
		return cfg_obj_set_object_attr(path,duraionAttr,0,val);
	}

	if(0 == strcmp(attr, "RepeatDay"))
	{
		memset(duraionAttr,0,sizeof(duraionAttr));
		snprintf(duraionAttr,sizeof(duraionAttr),"RepeatDay%s",addIdx);
		i = 0;
		while( val[i] != NULL && i < 7)
		{
			memset(bit,0,sizeof(bit));
			snprintf(bit,sizeof(bit),"%c",val[i]);
			
			if(atoi(bit) <= 0 || atoi(bit) > 7)
				return -1;
			else
			{
				if(atoi(bit) == 7)
				{
					flag = flag | 1;
				}
				else
				{
					flag = flag | (1 << atoi(bit));
				}
			}			
			i++;
		}
		memset(setValue,0,sizeof(setValue));
		snprintf(setValue,sizeof(setValue),"%d",flag);
		return cfg_obj_set_object_attr(path,duraionAttr,0,setValue);
	}			
	cfg_obj_get_object_attr(PARENTAL_COMMON_NODE,"DelIdx",0,delIdx,sizeof(delIdx));

	if(0 == strcmp(attr, "DelFlag"))
	{
		return parental_del(path,val,delIdx);
	}
	
	return cfg_obj_set_object_attr(path,attr,0,val);
}
static cfg_node_ops_t cfg_type_parental_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_parental_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_parental_func_commit 
}; 


static cfg_node_type_t cfg_type_parental_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_PARENTAL_NUM, 
	 .parent = &cfg_type_parental, 
	 .index = cfg_type_parental_index, 
	 .ops = &cfg_type_parental_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_parental_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_parental_func_commit 
}; 


static cfg_node_type_t cfg_type_parental_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_parental, 
	 .ops = &cfg_type_parental_common_ops, 
}; 

static cfg_node_ops_t cfg_type_parental_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_parental_func_commit 
}; 

static cfg_node_type_t* cfg_type_parental_child[] = { 
	 &cfg_type_parental_entry, 
	 &cfg_type_parental_common, 
	 NULL 
}; 

cfg_node_type_t cfg_type_parental = { 
	 .name = "Parental", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_parental_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_parental_child, 
	 .ops = &cfg_type_parental_ops, 
}; 
