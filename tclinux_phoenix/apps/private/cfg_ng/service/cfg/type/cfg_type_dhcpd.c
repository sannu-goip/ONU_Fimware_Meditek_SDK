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
#include <svchost_evt.h>
#include "utility.h" 

#define MAX_STATIC_NUM 8

static char* cfg_type_dhcpd_index[] = { 
	 "dhcpd_id", 
	 NULL 
}; 


static int cfg_type_dhcpd_func_read(char* path,char* attr, char* val,int len) 
{ 
	char subNode[64]={0}, count[3]={0};	
	int i;
	int total = 0,empty_entry = -1;
	char groupPath[32] = {0};

	strncpy(groupPath, DHCPD_NODE, sizeof(groupPath)-1);

	if(cfg_query_object(groupPath, NULL, NULL) <= 0){
		return -1;
	}
	
	/*Delete the Entry node if Common node is not existed when attr is IP*/
	if(!strcmp(attr,"IP")){		
		if(cfg_query_object(path, NULL, NULL) <= 0){
			/*Entry is not exited,need not to check*/
			return 0;
		}		
		
		if(cfg_query_object(DHCPD_COMMON_NODE, NULL, NULL) <= 0){
			cfg_delete_object(path);
			return 0;		
		}					
	}
	

		for(i=1; i<=MAX_STATIC_NUM; i++){
			/*set query data, indicate entry*/		
			snprintf(subNode,sizeof(subNode),DHCPD_ENTRY_NODE, i);			
			if(cfg_query_object(subNode, NULL, NULL) > 0){				
				total++;				
			}
			else{
				if(empty_entry < 0)
					empty_entry = i;
			}			
		}
		


	if(empty_entry < 0){     /*static pool is full*/
		cfg_set_object_attr(groupPath, "Empty_Entry", "N/A");
	}
	else{
		snprintf(count, sizeof(count), "%d", empty_entry);
		cfg_set_object_attr(groupPath, "Empty_Entry", count);
	}
	snprintf(count,sizeof(count),"%d",total);
	cfg_set_object_attr(groupPath, "Static_Num", count);
	
	 return 0; 
} 

int svc_cfg_boot_dhcpd(void)
{
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_DHCPD_BOOT, NULL, 0); 
	return 0;
}

static int cfg_type_dhcpd_func_commit(char *path)
{
#if defined(TCSUPPORT_CWMP_TR181)
		char tmpValue[8]={0};
		cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
		if(strcmp(tmpValue,"1"))
		{	
			/*not commit from tr181*/
			updateDHCPv4SvrPool("1");
		}
#endif

	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_DHCPD_UPDATE, (void*)&param, sizeof(param));
	return 0;
}

static int cfg_type_dhcpd_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcpd_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcpd_entry_ops  = { 
	 .get = cfg_type_dhcpd_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpd_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_dhcpd, 
	 .index = cfg_type_dhcpd_index, 
	 .ops = &cfg_type_dhcpd_entry_ops, 
}; 


static int cfg_type_dhcpd_common_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcpd_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcpd_common_ops  = { 
	 .get = cfg_type_dhcpd_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpd_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_dhcpd, 
	 .ops = &cfg_type_dhcpd_common_ops, 
}; 


static int cfg_type_dhcpd_option60_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcpd_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcpd_option60_ops  = { 
	 .get = cfg_type_dhcpd_option60_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpd_option60 = { 
	 .name = "Option60", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_dhcpd, 
	 .ops = &cfg_type_dhcpd_option60_ops, 
}; 


static int cfg_type_dhcpd_option240_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcpd_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcpd_option240_ops  = { 
	 .get = cfg_type_dhcpd_option240_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpd_option240 = { 
	 .name = "Option240", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_dhcpd, 
	 .ops = &cfg_type_dhcpd_option240_ops, 
}; 


static int cfg_type_dhcpd_option125_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcpd_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcpd_option125_ops  = { 
	 .get = cfg_type_dhcpd_option125_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpd_option125 = { 
	 .name = "Option125", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_dhcpd, 
	 .ops = &cfg_type_dhcpd_option125_ops, 
}; 


static cfg_node_ops_t cfg_type_dhcpd_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpd_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpd_child[] = { 
	 &cfg_type_dhcpd_entry, 
	 &cfg_type_dhcpd_common, 
	 &cfg_type_dhcpd_option60, 
	 &cfg_type_dhcpd_option240, 
	 &cfg_type_dhcpd_option125, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpd = { 
	 .name = "Dhcpd", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpd_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpd_child, 
	 .ops = &cfg_type_dhcpd_ops, 
}; 
