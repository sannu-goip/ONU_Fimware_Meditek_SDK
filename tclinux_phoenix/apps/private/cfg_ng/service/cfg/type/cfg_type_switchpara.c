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
#include <stdlib.h>
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <unistd.h>
#include "utility.h"
#include "blapi_traffic.h"

#ifdef TCSUPPORT_CT_2PWIFI
#define SWITCHPARA_MAX_ENTRY 2
#else
#define SWITCHPARA_MAX_ENTRY 4
#endif


static char* cfg_type_switchpara_index[] = { 
	 "switchpara_id", 
	 NULL 
}; 



int cfg_type_switchpara_func_execute(char* path)
{
	char cmdbuf[100] = {0}; 
	int spid = -1;
	char tmp[32] = {0};
	char switchflag[8] = {0},maxbitrate[10] = {0},duplexmode[10] = {0},enable[8] = {0};
	char nodeName[64];
	int ret = -1;
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SWITCHPARA_COMMON_NODE, sizeof(nodeName)-1);

	if(cfg_obj_get_object_attr(nodeName, "SwitchNeedOperation", 0, switchflag, sizeof(switchflag)) < 0)
	{
		return -1;
	}

	if(!strcmp(switchflag,"Yes"))
	{
		/*get current ipping index*/
		if(get_entry_number_cfg2(path,"entry.",&spid) < 0)
		{
			memset(nodeName,0,sizeof(nodeName));
			strncpy(nodeName,WEBCURSET_ENTRY_NODE, sizeof(nodeName)-1);
			if(cfg_obj_get_object_attr(nodeName, "switchpara_id", 0, tmp, sizeof(tmp)) > 0)
			{
				spid = atoi(tmp) + 1;
			}
			else
			{
				return -1;
			}
		}

		/*get parameters and execute the cmd*/
		snprintf(nodeName, sizeof(nodeName), SWITCHPARA_ENTRY_NODE,spid);
		
		if(cfg_obj_get_object_attr(nodeName, "enable", 0, enable, sizeof(enable)) > 0)	
		{
			cfg_obj_get_object_attr(nodeName, "maxBitRate", 0, maxbitrate, sizeof(maxbitrate));
			cfg_obj_get_object_attr(nodeName, "duplexMode", 0, duplexmode, sizeof(duplexmode));
			ret = blapi_traffic_set_switch_linkmode(enable, spid-1, maxbitrate, duplexmode);	
		}
	}

	return 0;
}


int svc_cfg_boot_switchpara(void)
{
	int i = 0;
	char nodeName[64]={0};
	for(i = 1; i <= SWITCHPARA_MAX_ENTRY; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
	  	snprintf(nodeName, sizeof(nodeName), SWITCHPARA_ENTRY_NODE, i);
		cfg_type_switchpara_func_execute(nodeName);
	}
	return 0;
}


static int cfg_type_switchpara_func_commit(char* path)
{
	return cfg_type_switchpara_func_execute(path);
}


static cfg_node_ops_t cfg_type_switchpara_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_switchpara_func_commit 
}; 


static cfg_node_type_t cfg_type_switchpara_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | SWITCHPARA_MAX_ENTRY, 
	 .parent = &cfg_type_switchpara, 
	 .index = cfg_type_switchpara_index, 
	 .ops = &cfg_type_switchpara_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_switchpara_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_switchpara_func_commit 
}; 


static cfg_node_type_t cfg_type_switchpara_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_switchpara, 
	 .ops = &cfg_type_switchpara_common_ops, 
}; 


static cfg_node_ops_t cfg_type_switchpara_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_switchpara_func_commit 
}; 


static cfg_node_type_t* cfg_type_switchpara_child[] = { 
	 &cfg_type_switchpara_entry, 
	 &cfg_type_switchpara_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_switchpara = { 
	 .name = "SwitchPara", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_switchpara_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_switchpara_child, 
	 .ops = &cfg_type_switchpara_ops, 
}; 
