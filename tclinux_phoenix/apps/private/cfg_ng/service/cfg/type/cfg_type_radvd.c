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


int svc_cfg_boot_radvd(void)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	strncpy(param.buf, RADVD_COMMON_NODE, EVT_BUF_LENGTH - 1);
#else
	strncpy(param.buf, RADVD_ENTRY_NODE, EVT_BUF_LENGTH - 1);
#endif
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_RADVD_BOOT, (void*)&param, sizeof(param)); 

	return 0;
}


static int cfg_type_radvd_func_commit(char* path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	if( NULL == strstr(path, "entry") )
		strncpy(param.buf, RADVD_COMMON_NODE, EVT_BUF_LENGTH - 1);
	else
		strncpy(param.buf, path, EVT_BUF_LENGTH - 1);
#else
	strncpy(param.buf, path, EVT_BUF_LENGTH - 1);
#endif

#if defined(TCSUPPORT_CWMP_TR181)
		char tmpValue[8]={0};
		int  ipIfIdx = -1;
		cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
		if(strcmp(tmpValue,"1"))
		{	
			/*not commit from tr181*/
			ipIfIdx = findLanIPInterfaceEntry();
			updateIPv6PreFix(ipIfIdx);
		}
#endif
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_RADVD_UPDATE, (void*)&param, sizeof(param));
	return 0;
}

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
static cfg_node_ops_t cfg_type_radvd_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_radvd_func_commit 
}; 


static cfg_node_type_t cfg_type_radvd_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_RADVD_NUM, 
	 .parent = &cfg_type_radvd, 
	 .ops = &cfg_type_radvd_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_radvd_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_radvd_func_commit
}; 

static cfg_node_type_t cfg_type_radvd_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_radvd, 
	 .ops = &cfg_type_radvd_common_ops, 
};
#else
static cfg_node_ops_t cfg_type_radvd_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_radvd_func_commit
}; 

static cfg_node_type_t cfg_type_radvd_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_radvd, 
	 .ops = &cfg_type_radvd_entry_ops, 
};
#endif

static cfg_node_ops_t cfg_type_radvd_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_radvd_func_commit 
}; 


static cfg_node_type_t* cfg_type_radvd_child[] = { 
	 &cfg_type_radvd_entry,
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	 &cfg_type_radvd_common,
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_radvd = { 
	 .name = "Radvd", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_radvd_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_radvd_child, 
	 .ops = &cfg_type_radvd_ops, 
}; 
