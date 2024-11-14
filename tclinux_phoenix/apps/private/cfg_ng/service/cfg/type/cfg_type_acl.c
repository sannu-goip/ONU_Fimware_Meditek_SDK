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
#include <svchost_evt.h>

#if defined(TCSUPPORT_CWMP_TR181)
#include "cfg_tr181_utility.h"
#include "cfg_tr181_global.h"
#endif

#define TMP_ACL_FILE "/tmp/aclvalid"
#define MAX_ACL_RULE 16

static char* cfg_type_acl_index[] = { 
	 "acl_id", 
	 NULL 
}; 

int svc_cfg_boot_acl(void)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_ACL_BOOT, NULL, 0); 
	return 0;
}

int cfg_type_acl_commit(char* path)
{
	wan_related_evt_t param;

#if defined(TCSUPPORT_CWMP_TR181)
	updateFireWallChByNodeFlag(ACLFILTER_FLAG);
#endif
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_ACL_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

static int cfg_type_acl_func_get(char* path,char* attr,char* val,int len)
{
	int i = 0;
	int j = 0;
	int attrNum = 0;
	int noUsed = 0;
	int aclEntryNum = 0;
	int ret = 0;
	char count[4] = {0};
	FILE* fp = NULL;
	char nodeName[64] = {0};
	char buf[64] ={0};
	char tmp[64] ={0};
	char acl_attribute[][CFG_ATTR_SIZE]=
	{
		{"ACLName"},
		{"ScrIPAddrBegin"},
		{"ScrIPAddrEnd"},
		{"Interface"},
		{"Application"},
		{"Activate"},
	};
	
	if(!strcmp(attr,"acl_num") || !strcmp(attr,"add_aclnum"))
	{
		fp= fopen(TMP_ACL_FILE, "w");	
		if(fp)
		{
		memset(nodeName, 0, sizeof(nodeName));
		for(i=0; i<MAX_ACL_RULE; i++)
		{
			snprintf(nodeName, sizeof(nodeName), ACL_ENTRY_N_NODE, i + 1);
			if (cfg_query_object(nodeName,NULL,NULL) <= 0)
			{
				/*1.find the entry index which can be used*/
				if(!noUsed)
				{
					memset(count,0x00, sizeof(count));
					snprintf(count, sizeof(count), "%d", i);
					cfg_set_object_attr(ACL_COMMON_NODE, "add_aclnum", count);
					noUsed++;
				}
			}
			else
			{
				/*2.get acl_entry value and save in file*/
				memset(count,0x00, sizeof(count));
				snprintf(count, sizeof(count), "%d\n", i);
				fputs(count, fp);
				attrNum = sizeof(acl_attribute)/CFG_ATTR_SIZE;
				for(j= 0; j < attrNum; j++)
				{
					memset(buf, 0, sizeof(buf));
					memset(tmp, 0, sizeof(tmp));
					cfg_get_object_attr(nodeName, acl_attribute[j], buf, sizeof(buf));
					if(buf[0] != '\0')
					{
						snprintf(tmp, sizeof(tmp), "%s\n",buf);
						fputs_escape(tmp, fp);	
					}
					else
					{
						fputs("\n", fp);
					}
				}
				aclEntryNum++;
			}
		}
		fclose(fp);
		memset(count,0, sizeof(count));
		snprintf(count, sizeof(count), "%d", aclEntryNum);
		cfg_set_object_attr(ACL_COMMON_NODE, "acl_num", count);
			ret = chmod(TMP_ACL_FILE, 777);
		}
	}
	
	return cfg_type_default_func_get(path,attr,val,len);
}

#if defined(TCSUPPORT_CWMP_TR181)
int deleteFirewallChNodeByACLNode(char*path)
{
	int entryIdx= -1;
	char nodeName[MAXLEN_NODE_NAME] ={0};
		
	if(path == NULL || get_entry_number_cfg2(path, "entry.", &entryIdx) != 0)
	{
		return -1;
	}
	
	snprintf(nodeName, sizeof(nodeName), TR181_FIREWALLCH_PVC_ENTRY_NODE, ACL_FILTER_RULE_FLAG, entryIdx);
	cfg_obj_delete_object(nodeName);	

	return 0;	
}
#endif


int cfg_type_acl_entry_func_delete(char* path)
{
#if defined(TCSUPPORT_CWMP_TR181)
	deleteFirewallChNodeByACLNode(path);
#endif
	return cfg_obj_delete_object(path);
}


static cfg_node_ops_t cfg_type_acl_entry_ops  = { 
	 .get = cfg_type_acl_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_acl_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_acl_commit 
}; 


static cfg_node_type_t cfg_type_acl_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MULTIPLE | MAX_ACL_RULE, 
	 .parent = &cfg_type_acl, 
	 .index = cfg_type_acl_index, 
	 .ops = &cfg_type_acl_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_acl_common_ops  = { 
	 .get = cfg_type_acl_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_acl_commit 
}; 


static cfg_node_type_t cfg_type_acl_common = { 
	 .name = "Common", 
	 .flag =CFG_TYPE_FLAG_UPDATE| 1, 
	 .parent = &cfg_type_acl, 
	 .ops = &cfg_type_acl_common_ops, 
}; 


static cfg_node_ops_t cfg_type_acl_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_acl_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_acl_commit 
}; 


static cfg_node_type_t* cfg_type_acl_child[] = { 
	 &cfg_type_acl_entry, 
	 &cfg_type_acl_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_acl = { 
	 .name = "ACL", 
	 .flag =  CFG_TYPE_FLAG_UPDATE |1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_acl_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_acl_child, 
	 .ops = &cfg_type_acl_ops, 
}; 
