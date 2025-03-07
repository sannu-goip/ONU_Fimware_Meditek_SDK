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
#include <svchost_evt.h>

static int cfg_type_ddns_func_get(char* path,char* attr,char* val,int len)
{
	char buf[51] = {0};
	char statbuf[100]= {0};
	FILE *fd1 = NULL, *fd2 = NULL;
	int res = 0;

	if (path == NULL || attr == NULL|| val == NULL || len <= 0)
	{
		return -1;
	}
	
	if (0 == strcmp("DDNSStatus", attr) || 0 == strcmp("DDNSErrMsg", attr))
	{
		fd1 = fopen("/tmp/ddnsstat","r");
		if (fd1 != NULL) 
		{
			res = fread(buf, 1, sizeof(buf)-1, fd1);
			buf[sizeof(buf) - 1] = '\0';
			fclose(fd1);
			if (0 == strcmp(buf, "9")||0 == strcmp(buf, "10"))
			{
				fd1 = fopen("/tmp/statval","r");
				if (fd1 != NULL)
				{
					res = fread(statbuf, 1, sizeof(statbuf)-1, fd1);
					statbuf[sizeof(statbuf) - 1] = '\0';
					cfg_set_object_attr(path, "DDNSErrMsg", statbuf);
					fclose(fd1);
				}
				else
				{
					cfg_set_object_attr(path, "DDNSErrMsg", "fail to open file statval.");
				}
			}
			
			cfg_set_object_attr(path, "DDNSStatus", buf);
		}
		else
		{
			cfg_set_object_attr(path, "DDNSStatus", "9");
		}
	}

	return cfg_type_default_func_get(path, attr, val, len);
}


static char* cfg_type_ddns_index[] = { 
	 "ddns_id", 
	 NULL 
}; 

int svc_cfg_boot_ddns(void)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DDNS_BOOT, NULL, 0); 
	return 0;
}

static int cfg_type_ddns_entry_func_commit(char* path)
{
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DDNS_UPDATE, (void *)&param, sizeof(param)); 
	return 0;	
}
static cfg_node_ops_t cfg_type_ddns_entry_ops  = { 
	 .get = cfg_type_ddns_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ddns_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_ddns_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_ddns, 
	 .index = cfg_type_ddns_index, 
	 .ops = &cfg_type_ddns_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_ddns_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ddns_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_ddns_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_ddns, 
	 .ops = &cfg_type_ddns_common_ops, 
}; 


static cfg_node_ops_t cfg_type_ddns_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_ddns_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ddns_entry_func_commit 
}; 


static cfg_node_type_t* cfg_type_ddns_child[] = { 
	 &cfg_type_ddns_entry, 
	 &cfg_type_ddns_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_ddns = { 
	 .name = "Ddns", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_ddns_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_ddns_child, 
	 .ops = &cfg_type_ddns_ops, 
}; 
