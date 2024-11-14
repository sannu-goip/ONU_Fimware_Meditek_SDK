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
#include <semaphore.h>
#ifdef TCSUPPORT_CWMP_TR181
#include "tr181/cfg_tr181_global.h" 
#endif
#include "utility.h"
static char* cfg_type_wlan_index[] = { 
	 "wlan_id", 
	 NULL 
}; 

int svc_cfg_boot_wlan(void)
{
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_BOOT, NULL, 0);	
	return 0;
}
static int cfg_type_wlanroam_func_commit(char* path) 
{
	printf("enter wlanroam_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ROAM, (void*)&param, sizeof(param));	
	return 0;

}

static int cfg_type_wlan_func_commit(char* path) 
{ 
	char tmpValue[8]={0};
	printf("enter wlan_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
#ifdef TCSUPPORT_CWMP_TR181
	cfg_obj_get_object_attr("root.globalstate.common", "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	if(strcmp(tmpValue,"1"))
	{
		updateTR181AllWLanInterface();	
	}
#endif
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 

#if defined(TCSUPPORT_CT_JOYME2)
int cfg_type_wlan_entry_func_set(char* path,char* attr,char* val)
{
	int entryidx = 0;
	int mask = 0;
	char WifiEnableMask[8] = {0};

	if ( NULL == path || NULL == attr || NULL == val )
		return -1;
	
	if ( 0 == strcmp(attr, "EnableSSID") )
	{
		if( 0 != get_entry_number_cfg2(path, "entry.", &entryidx) )
		{
			return -1;
		}

		cfg_get_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", WifiEnableMask, sizeof(WifiEnableMask));
		mask = atoi(WifiEnableMask);

		if ( 0 == strcmp(val, "1") )
			SET_BIT(mask, entryidx - 1);
		else
			CLR_BIT(mask, entryidx - 1);

		bzero(WifiEnableMask, sizeof(WifiEnableMask));
		snprintf(WifiEnableMask, sizeof(WifiEnableMask), "%d", mask);
		cfg_set_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", WifiEnableMask);
	}
	return cfg_obj_set_object_attr(path,attr,0,val);
}
#endif

static cfg_node_ops_t cfg_type_wlan_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
#if defined(TCSUPPORT_CT_JOYME2)
	 .set = cfg_type_wlan_entry_func_set, 
#else
	 .set = cfg_type_default_func_set, 
#endif
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_wlan, 
	 .index = cfg_type_wlan_index, 
	 .ops = &cfg_type_wlan_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_wlan_wds_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_commit 
}; 

static cfg_node_type_t cfg_type_wlan_wds = { 
	 .name = "WDS", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_wds_ops, 
}; 


static cfg_node_ops_t cfg_type_wlan_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan_common = { 
	 .name = "Common", 
	 .flag =  1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_common_ops, 
}; 
static int cfg_type_wlan_func_monitorSTA_commit(char* path) 
{ 

	wlan_evt_t param;
	tcdbg_printf("enter cfg_type_wlan_func_monitorSTA_commit.\n");
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_MONITOR_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 
#ifdef TCSUPPORT_WLAN_ACS
int cfg_type_wlan_func_acs_commit(char* path)
{
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ACS_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}
#endif
int cfg_type_wlan_func_ratepriority_commit(char* path)
{
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_RATEPRIORITY_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}


static cfg_node_ops_t cfg_type_wlan_monitorSTA_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_monitorSTA_commit 
}; 

static cfg_node_type_t cfg_type_wlan_monitorSTA = { 
	 .name = "MonitorSTA", 
	 .flag =  1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_monitorSTA_ops, 
};
static cfg_node_ops_t cfg_type_wlan_roaming_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlanroam_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan_roaming = { 
	 .name = "Roam", 
	 .flag =  1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_roaming_ops, 
}; 
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
static int cfg_type_roam11k_func_commit(char* path) 
{ 
	printf("enter roam11k_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ROAM11K_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 

static cfg_node_ops_t cfg_type_wlan_roam11k_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_roam11k_func_commit 
}; 

static cfg_node_type_t cfg_type_wlan_roam11k = { 
	 .name = "Roam11k", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_roam11k_ops, 
}; 
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
static int cfg_type_roam11v_func_commit(char* path) 
{ 
	printf("enter roam11v_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ROAM11V_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 

static cfg_node_ops_t cfg_type_wlan_roam11v_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_roam11v_func_commit 
}; 

static cfg_node_type_t cfg_type_wlan_roam11v = { 
	 .name = "Roam11v", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_roam11v_ops, 
};
#endif
#ifdef TCSUPPORT_WLAN_BNDSTRG
static int cfg_type_wlan_func_bndstrg_commit(char* path) 
{ 

	wlan_evt_t param;
	//printf("enter wlan_commit.\n");
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_BNDSTRG_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 
static cfg_node_ops_t cfg_type_wlan_bndstrg_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_bndstrg_commit 
}; 
static cfg_node_type_t cfg_type_wlan_bndstrg = { 
	 .name = "Bndstrg", 
	 .flag =  1, 
	 .parent = &cfg_type_wlan, 
	 .ops = &cfg_type_wlan_bndstrg_ops, 
};
#endif

#ifdef TCSUPPORT_WLAN_ACS
static cfg_node_ops_t cfg_type_wlan_acs_ops = {
	.get = cfg_type_default_func_get,
	.set = cfg_type_default_func_set,
	.query = cfg_type_default_func_query,
	.commit = cfg_type_wlan_func_acs_commit
};

static cfg_node_type_t cfg_type_wlan_acs= {
	.name = "ACS",
	.flag = 1,
	.parent = &cfg_type_wlan,
	.ops = &cfg_type_wlan_acs_ops,

};
#endif

static cfg_node_ops_t cfg_type_wlan_ratepriority_ops = {
	.get = cfg_type_default_func_get,
	.set = cfg_type_default_func_set,
	.query = cfg_type_default_func_query,
	.commit = cfg_type_wlan_func_ratepriority_commit
};

static cfg_node_type_t cfg_type_wlan_ratepriority= {
	.name = "RatePriority",
	.flag = 1,
	.parent = &cfg_type_wlan,
	.ops = &cfg_type_wlan_ratepriority_ops,

};


static cfg_node_ops_t cfg_type_wlan_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan_func_commit 
}; 


static cfg_node_type_t* cfg_type_wlan_child[] = { 
	 &cfg_type_wlan_entry, 
	 &cfg_type_wlan_wds,	
	 &cfg_type_wlan_common, 
 #ifdef TCSUPPORT_WLAN_ACS
	 &cfg_type_wlan_acs,
 #endif
#if defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_wlan_ratepriority,
#endif
 #ifdef TCSUPPORT_WLAN_BNDSTRG
	 &cfg_type_wlan_bndstrg, 
 #endif
 	 &cfg_type_wlan_monitorSTA, 	 
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
         &cfg_type_wlan_roam11k,
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
	 &cfg_type_wlan_roam11v,
#endif	
	 &cfg_type_wlan_roaming,
	 NULL 
}; 


cfg_node_type_t cfg_type_wlan = { 
	 .name = "WLan", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_wlan_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wlan_child, 
	 .ops = &cfg_type_wlan_ops, 
}; 
