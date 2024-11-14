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
#include <stdlib.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 
#include "svchost_evt.h"


#define 		MAX_TYPE_RULE_NUM	10
#define 		MAX_APP_RULE_NUM		4
#define 		MAX_TYPE_NUM			10


static char* cfg_type_downlinkqos_index[] = { 
	 "downlinkqos_id", 
	 NULL 
}; 

int svc_cfg_boot_downlinkqos(void)
{
	wan_related_evt_t param;

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, DOWNLINKQOS_COMMON_NODE, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DOWNLINKQOS_BOOT, (void *)&param, sizeof(param)); 
	return 0;
}

static int cfg_type_downlinkqos_func_commit(char* path)
{
	char tag[8] = {0};
	wan_related_evt_t param;

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DOWNLINKQOS_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
} 

static cfg_node_ops_t cfg_type_downlinkqos_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_downlinkqos_func_commit 
}; 


static cfg_node_type_t cfg_type_downlinkqos_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 30, 
	 .parent = &cfg_type_downlinkqos, 
	 .index = cfg_type_downlinkqos_index, 
	 .ops = &cfg_type_downlinkqos_entry_ops, 
}; 


static void  cfg_type_set_downlinkqos_common_curTypeIdx(void)
{
	int i = 0;
	char nodePath[64] = {0};
	char buf[10] = {0}, idx[10] = {0};
	
	/* get curTypeIdx */
	for ( i = 1; i <= MAX_TYPE_RULE_NUM; i++ )
	{
		snprintf(nodePath, sizeof(nodePath), DOWNLINKQOS_ENTRY_NODE, i);
		if ( cfg_obj_get_object_attr(nodePath, "ActQueue", 0, buf, sizeof(buf)) <= 0 )
		{
			/* it means this Entry is empty */
			snprintf(idx, sizeof(idx), "%d", i - 1);
			cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curTypeIdx", idx);
			break;
		}
	}
	if ( i == MAX_TYPE_RULE_NUM + 1 )
	{
		cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curTypeIdx", "N/A");
	}

	return;
}

static void  cfg_type_set_downlinkqos_common_curAppIdx(void)
{
	int i = 0;
	char nodePath[64] = {0};
	char buf[10] = {0}, idx[10] = {0};
	
	/* get cufAppIdx */
	for ( i = MAX_TYPE_RULE_NUM + 1; i <= MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM; i++ )
	{
		snprintf(nodePath, sizeof(nodePath), DOWNLINKQOS_ENTRY_NODE, i);
		if ( cfg_obj_get_object_attr(nodePath, "ActQueue", 0, buf, sizeof(buf)) <= 0 )
		{
			/* it means this Entry is empty */
			snprintf( idx, sizeof(idx), "%d", i - 1);
			cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curAppIdx", idx);
			break;
		}
	}

	if ( i == MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM +1 ) 
	{
		cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curAppIdx", "N/A");
	}
	
	return;
}

static int  cfg_type_set_downlinkqos_common_curSubTypeIdx(void)
{
	int i = 0;
	int entry_index = 0;
	char nodePath[64] = {0};
	char idxBuf[64] = {0}, typeName[20] = {0}, typeVal[20] = {0}, idx[10] = {0};
	
	/* get curSubTypeIdx */
	if ( cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "downlinkqos_id", 0, idxBuf, sizeof(idxBuf)) > 0 )
	{
		entry_index = atoi(idxBuf);
	}
	else
	{
		return -1;
	}

	snprintf(nodePath, sizeof(nodePath), DOWNLINKQOS_ENTRY_NODE, entry_index);
	for ( i = 0; i < MAX_TYPE_NUM; i++ )
	{
		snprintf(typeName, sizeof(typeName), "Type%d", i + 1);
		if ( cfg_obj_get_object_attr(nodePath, typeName, 0, typeVal, sizeof(typeVal)) <= 0 || ( !strcmp(typeVal, "N/A") ) )
		{
			/* it means this Entry is empty */
			snprintf( idx, sizeof(idx), "%d", i + 1 );
			cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curSubTypeIdx", idx);
			break;
		}
	}

	if ( i == MAX_TYPE_NUM )
	{
		cfg_set_object_attr(DOWNLINKQOS_COMMON_NODE, "curSubTypeIdx", "N/A");
	}

	return 0;
}

static int cfg_type_downlinkqos_common_func_get(char* path,char* attr, char* val,int len) 
{ 	
	if ( 0 == strcmp(attr, "curTypeIdx") )
	{
		cfg_type_set_downlinkqos_common_curTypeIdx();	
	}
	else if ( 0 == strcmp(attr, "curAppIdx") ) 
	{
		cfg_type_set_downlinkqos_common_curAppIdx();	
	}
	else if ( 0 == strcmp(attr, "curSubTypeIdx") )
	{
		if ( -1 == cfg_type_set_downlinkqos_common_curSubTypeIdx() )
		{
			return 0;
		}
	}
	
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_downlinkqos_common_ops  = { 
	 .get = cfg_type_downlinkqos_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_downlinkqos_func_commit 
}; 


static cfg_node_type_t cfg_type_downlinkqos_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_downlinkqos, 
	 .ops = &cfg_type_downlinkqos_common_ops, 
}; 


static cfg_node_ops_t cfg_type_downlinkqos_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_downlinkqos_func_commit 
}; 


static cfg_node_type_t* cfg_type_downlinkqos_child[] = { 
	 &cfg_type_downlinkqos_entry, 
	 &cfg_type_downlinkqos_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_downlinkqos = { 
	 .name = "DownlinkQoS", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_downlinkqos_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_downlinkqos_child, 
	 .ops = &cfg_type_downlinkqos_ops, 
}; 
