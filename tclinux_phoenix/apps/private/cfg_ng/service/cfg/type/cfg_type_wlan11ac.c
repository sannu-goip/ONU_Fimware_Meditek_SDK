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
#include <utility.h>
#include "cfg_types.h" 
#include <common/ecnt_chip_id.h>

static char* cfg_type_wlan11ac_index[] = { 
	 "wlan_ac_id", 
	 NULL 
}; 

typedef struct _power_consumption_wifi_attr_{	
	char *read_nodeName;
	char *write_nodeName;
	char *attr_type;
	char *attr_type_bak;
}power_consumption_wifi_attr;

static power_consumption_wifi_attr	wifi_syn[] =
{
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,			"SSID",				"SSID_bak"},
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,			"AuthMode",			"AuthMode_bak"},
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,			"EncrypType",		"EncrypType_bak"},
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,			"WPAPSK",			"WPAPSK_bak",},
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,			"EnableSSID",		"EnableSSID_bak"},
	{WLAN11AC_ENTRY1_NODE,		WLAN_ENTRY2_NODE,		    "HideSSID",	        "HideSSID_bak"},	
	{NULL,                  	NULL,						NULL,				NULL}
};
/*
	is_syn is 1
	wlan11ac_entry0 syn to wlan_entry1
	
	is_syn is 2
	wlan_entry1 save to back
	wlan11ac_entry0 syn to wlan_entry1
	
	is_syn is 3
	back to wlan_entry1
*/
static void wlan_wlan11ac_sys(int is_syn, int is_boot)
{
	int i = 0;
	char value[32] = {0};
	wlan_evt_t param;

	if(0 == is_boot)
		return;
	
	if (1 == is_syn)
	{
		while(NULL != wifi_syn[i].read_nodeName)
		{
			memset(value, 0, sizeof(value));
			cfg_get_object_attr(wifi_syn[i].read_nodeName, wifi_syn[i].attr_type, value, sizeof(value));
			cfg_set_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type, value);
			i++;
		}
	}
	else if(2 == is_syn)
	{
		while(NULL != wifi_syn[i].read_nodeName)
		{
			memset(value, 0, sizeof(value));
			cfg_get_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type, value, sizeof(value));
			cfg_set_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type_bak, value);

			memset(value, 0, sizeof(value));
			cfg_get_object_attr(wifi_syn[i].read_nodeName, wifi_syn[i].attr_type, value, sizeof(value));
			cfg_set_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type, value);
			i++;
		}
	}
	else if(3 == is_syn)
	{
		while(NULL != wifi_syn[i].read_nodeName)
		{
			memset(value, 0, sizeof(value));
			cfg_get_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type_bak, value, sizeof(value));
			cfg_set_object_attr(wifi_syn[i].write_nodeName, wifi_syn[i].attr_type, value);
			i++;
		}
	}
	
	cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id", "1");
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, WLAN_ENTRY2_NODE, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_UPDATE, (void*)&param, sizeof(param));
}

static unsigned int getChipIdx(void)
{
	unsigned int chidIdx = END_PACKAGE_ID;

	wifimgr_lib_get_CHIP_ID(&chidIdx);
	
	return chidIdx;
}

static void switch_open_lan(int switch_idx)
{
	unsigned int chidIdx = END_PACKAGE_ID; 

	if(0 > switch_idx || (0 != switch_idx && 0 == (switch_idx & 0x2)))
		return;
	
	system("/userfs/bin/pwctlcmd ethpwr Enable");
	
}

static void switch_close_lan(int switch_idx)
{
	unsigned int chidIdx = END_PACKAGE_ID; 

	if(0 > switch_idx || (0 != switch_idx && 0 == (switch_idx & 0x2)))
		return;
	
	system("/userfs/bin/pwctlcmd ethpwr Disable");
	
}
/*
	close 5G wifi syn to wlan_entry1
*/
static void switch_open_wifi(int switch_idx)
{
	char rt_device[16];
	char inter[16];
	char cmd[64];
	char bssidNum[8];
	int i = 0;

	if(0 > switch_idx || (0 != switch_idx && 0 == (switch_idx & 0x1)))
		return;
	
	wlan_wlan11ac_sys(2, switch_idx);
	memset(bssidNum, 0, sizeof(bssidNum));
	cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", bssidNum, sizeof(bssidNum));
	for(i = 0; i < atoi(bssidNum); i++)
	{
		memset(inter, 0, sizeof(inter));
		memset(cmd, 0, sizeof(cmd));
		snprintf(inter, sizeof(inter), "rai%d", i);
		snprintf(cmd, sizeof(cmd), "ifconfig %s down", inter);
		system(cmd);
	}
	system("ifconfig apclii0 down");
	
	wifimgr_lib_set_5G_PCIE(0);
	
}

static void switch_close_wifi(int switch_idx)
{
	char cmd[64];
	char bssidNum_5G[8];
	char wifi_enable[8];
	char node[32];
	int i = 0;

	if(0 > switch_idx || (0 != switch_idx && 0 == (switch_idx & 0x1)))
		return;
	
	wlan_wlan11ac_sys(3, switch_idx);
	wifimgr_lib_set_5G_PCIE(1);

	if (0 != switch_idx)
	{
		memset(bssidNum_5G, 0, sizeof(bssidNum_5G));
		cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", bssidNum_5G, sizeof(bssidNum_5G));
		for(i = 0; i < atoi(bssidNum_5G); i++)
		{
			memset(node, 0, sizeof(node));
			memset(wifi_enable, 0, sizeof(wifi_enable));
			snprintf(node, sizeof(node), WLAN11AC_ENTRY_N_NODE, i + 1);
			if (cfg_get_object_attr(node, "EnableSSID", wifi_enable, sizeof(wifi_enable)) > 0 && 0 == strcmp(wifi_enable, "1"))
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ifconfig rai%d up", i);
				system(cmd);
				sleep(1);
			}
		}
	}
	
}

static void switch_1T1R(char *op, int switch_idx)
{
	char cmd[64] = {0};
	char is_boot[8] = {0};
	int i = 0, chidIdx = 0;
	char bssidNum_24G[8];
	char bssidNum_5G[8];
	char node[32];
	char wifi_enable[8];
	char op_5G[32] = {0};

	if(0 > switch_idx || (0 != switch_idx && 0 == (switch_idx & 0x4)))
		return;

	chidIdx = getChipIdx();
	switch(chidIdx)
	{

		case EN7528HU:
		case EN7528DU:
		case EN7580GT:
		case EN7580ST:
		case EN7580GAT:
			break;
		default:
			tcdbg_printf("lower_power only support 7580/7528\n");
			return;
	}
	
	if(0 != (switch_idx & 0x4))
	{
		if(0 == strcmp(op, "Yes"))
		{
			system("sys modifybit bfb00240 0 1");	
			system("sys modifybit bfb00240 1 0");
		}
		else
		{
			system("sys modifybit bfb00240 0 0");	
			system("sys modifybit bfb00240 1 0");		
		}
		
		memset(bssidNum_24G, 0, sizeof(bssidNum_24G));
		cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", bssidNum_24G, sizeof(bssidNum_24G));
		for(i = 0; i < atoi(bssidNum_24G); i++)
		{
			memset(node, 0, sizeof(node));
			memset(wifi_enable, 0, sizeof(wifi_enable));
			snprintf(node, sizeof(node), WLAN_ENTRY_N_NODE, i + 1);
			if (cfg_get_object_attr(node, "EnableSSID", wifi_enable, sizeof(wifi_enable)) > 0 && 0 == strcmp(wifi_enable, "1"))
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ifconfig ra%d down", i);
				system(cmd);
				sleep(1);
				
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ifconfig ra%d up", i);
				system(cmd);
				sleep(1);
			}
		}
		memset(op_5G, 0, sizeof(op_5G));
		cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_wifi", op_5G, sizeof(op_5G));
		if(0 == strcmp(op_5G, "Yes"))
			return;

		memset(bssidNum_5G, 0, sizeof(bssidNum_5G));
		cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", bssidNum_5G, sizeof(bssidNum_5G));
		for(i = 0; i < atoi(bssidNum_5G); i++)
		{
			memset(node, 0, sizeof(node));
			memset(wifi_enable, 0, sizeof(wifi_enable));
			snprintf(node, sizeof(node), WLAN11AC_ENTRY_N_NODE, i + 1);
			if (cfg_get_object_attr(node, "EnableSSID", wifi_enable, sizeof(wifi_enable)) > 0 && 0 == strcmp(wifi_enable, "1"))
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ifconfig rai%d down", i);
				system(cmd);
				sleep(1);
				
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ifconfig rai%d up", i);
				system(cmd);
				sleep(1);
			}
		}			
	}
	else if(0 == switch_idx)
	{
		if(0 == strcmp(op, "Yes"))
		{
			system("sys modifybit bfb00240 0 1");	
			system("sys modifybit bfb00240 1 0");
		}
		else
		{
			system("sys modifybit bfb00240 0 0");	
			system("sys modifybit bfb00240 1 0");		
		}
	}
}
/*
	switch_id & 0x1 switch_wifi
	switch_id & 0x2 switch_lan
	switch_id & 0x4 switch_1T1R
*/
static int new_wifi_function(int isboot)
{
	char op[32] = {0};
	char switch_id[8] = {0};
	int switch_idx = -1;
	
	memset(switch_id, 0, sizeof(switch_id));
	cfg_get_object_attr(WLAN_COMMON_NODE, "switch_id", switch_id, sizeof(switch_id));
	if (isboot)
		switch_idx = 0;
	else
		switch_idx = atoi(switch_id);

	cfg_set_object_attr(WLAN_COMMON_NODE, "switch_id", "-1");
	
	memset(op, 0, sizeof(op));
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_lan", op, sizeof(op)) > 0)
	{
		if(0 == strcmp(op, "Yes"))
	    {
			switch_open_lan(switch_idx);
	    }
		else if (0 == strcmp(op, "No"))
	    {
			switch_close_lan(switch_idx);
	    }
	}
	memset(op, 0, sizeof(op));
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_1T1R", op, sizeof(op)) > 0)
		switch_1T1R(op, switch_idx);
	
	memset(op, 0, sizeof(op));
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_wifi", op, sizeof(op)) > 0)
	{
		if(0 == strcmp(op, "Yes"))
		{
			cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wlanBoot_ac", "1");
			switch_open_wifi(switch_idx);
		    return 1;
		}
		else if (0 == strcmp(op, "No"))
		{
			switch_close_wifi(switch_idx);
		}
	}
	
	return 0;
}

int svc_cfg_boot_wlan11ac(void)
{
	int ret = 0;
	ret = new_wifi_function(1);	
	if (1 == ret)
		return 0;
	
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN11AC_BOOT, NULL, 0);	
	return 0;
}

static int cfg_type_wlan11ac_func_commit(char* path) 
{ 
	char tmpValue[8]={0};
	int ret = 0;
	ret = new_wifi_function(0);	
	if (1 == ret)
	{
		if (0 == strcmp(path, WLAN11AC_ENTRY1_NODE))
			wlan_wlan11ac_sys(1, 1);
		
		return 0;
	}

#ifdef TCSUPPORT_CWMP_TR181
	cfg_obj_get_object_attr("root.globalstate.common", "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	if(strcmp(tmpValue,"1"))
	{
		updateTR181AllWLanInterface();	
	}
#endif
	printf("enter wlan_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 
#ifdef TCSUPPORT_WLAN_ACS
int cfg_type_wlan11ac_func_acs_commit(char* path)
{
	char mssid[32] = {0};
	
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "power_consumption_wifi", mssid, sizeof(mssid)) > 0 && 0 == strcmp(mssid, "Yes"))
    {
    	
        return 0;
    }

	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN11AC_ACS_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}
#endif
int cfg_type_wlan11ac_func_ratepriority_commit(char* path)
{
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN11AC_RATEPRIORITY_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
int cfg_type_wlan11ac_entry_func_set(char* path,char* attr,char* val)
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
			SET_BIT(mask, entryidx + WLAN_2_4_G_NODE_LEN - 1);
		else
			CLR_BIT(mask, entryidx + WLAN_2_4_G_NODE_LEN - 1);

		bzero(WifiEnableMask, sizeof(WifiEnableMask));
		snprintf(WifiEnableMask, sizeof(WifiEnableMask), "%d", mask);
		cfg_set_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", WifiEnableMask);
	}
	return cfg_obj_set_object_attr(path,attr,0,val);
}
#endif

static cfg_node_ops_t cfg_type_wlan11ac_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
#if defined(TCSUPPORT_CT_JOYME2)
	 .set = cfg_type_wlan11ac_entry_func_set, 
#else
	 .set = cfg_type_default_func_set, 
#endif
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan11ac_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_wlan11ac, 
	 .index = cfg_type_wlan11ac_index, 
	 .ops = &cfg_type_wlan11ac_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_wlan11ac_wds_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan11ac_wds = { 
	 .name = "WDS", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan11ac, 
	 .ops = &cfg_type_wlan11ac_wds_ops, 
}; 

static int cfg_type_wlan11acroam_func_commit(char* path) 
{
	printf("enter wlanroam_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN11AC_ROAM, (void*)&param, sizeof(param));	
	return 0;

}


static cfg_node_ops_t cfg_type_wlan11ac_roaming_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11acroam_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan11ac_roaming = { 
	 .name = "Roam", 
	 .flag =  1, 
	 .parent = &cfg_type_wlan11ac, 
	 .ops = &cfg_type_wlan11ac_roaming_ops, 
}; 
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
static int cfg_type_wlan11ac_roam11k_func_commit(char* path) 
{ 
	printf("enter roam11k_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ROAM11K_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 

static cfg_node_ops_t cfg_type_wlan11ac_roam11k_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_roam11k_func_commit 
}; 

static cfg_node_type_t cfg_type_wlan11ac_roam11k = { 
	 .name = "Roam11k", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan11ac, 
	 .ops = &cfg_type_wlan11ac_roam11k_ops, 
}; 
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
static int cfg_type_wlan11ac_roam11v_func_commit(char* path) 
{ 
	printf("enter roam11k_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_ROAM11V_UPDATE, (void*)&param, sizeof(param));	
	return 0;
} 

static cfg_node_ops_t cfg_type_wlan11ac_roam11v_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_roam11v_func_commit 
}; 

static cfg_node_type_t cfg_type_wlan11ac_roam11v = { 
	 .name = "Roam11v", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan11ac, 
	 .ops = &cfg_type_wlan11ac_roam11v_ops, 
}; 
#endif
static cfg_node_ops_t cfg_type_wlan11ac_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_func_commit 
}; 


static cfg_node_type_t cfg_type_wlan11ac_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_wlan11ac, 
	 .ops = &cfg_type_wlan11ac_common_ops, 
}; 

#ifdef TCSUPPORT_WLAN_ACS
static cfg_node_ops_t cfg_type_wlan11ac_acs_ops = {
	.get = cfg_type_default_func_get,
	.set = cfg_type_default_func_set,
	.query = cfg_type_default_func_query,
	.commit = cfg_type_wlan11ac_func_acs_commit
};

static cfg_node_type_t cfg_type_wlan11ac_acs = {
	.name = "ACS",
	.flag = 1 ,
	.parent = &cfg_type_wlan11ac,
	.ops = &cfg_type_wlan11ac_acs_ops,
};
#endif

static cfg_node_ops_t cfg_type_wlan11ac_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlan11ac_func_commit 
}; 

static cfg_node_ops_t cfg_type_wlan11ac_ratepriority_ops = {
	.get = cfg_type_default_func_get,
	.set = cfg_type_default_func_set,
	.query = cfg_type_default_func_query,
	.commit = cfg_type_wlan11ac_func_ratepriority_commit
};

static cfg_node_type_t cfg_type_wlan11ac_ratepriority= {
	.name = "RatePriority",
	.flag = 1,
	.parent = &cfg_type_wlan11ac,
	.ops = &cfg_type_wlan11ac_ratepriority_ops,

};


static cfg_node_type_t* cfg_type_wlan11ac_child[] = { 
	 &cfg_type_wlan11ac_entry, 
	 &cfg_type_wlan11ac_wds,
	 &cfg_type_wlan11ac_common, 
#ifdef TCSUPPORT_WLAN_ACS
	 &cfg_type_wlan11ac_acs,
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_wlan11ac_ratepriority,
#endif
#ifdef TCSUPPORT_WLAN_DOT11K_RRM
         &cfg_type_wlan11ac_roam11k,
#endif
#ifdef TCSUPPORT_WLAN_DOT11V_WNM
	 &cfg_type_wlan11ac_roam11v,
#endif	 
	 &cfg_type_wlan11ac_roaming,
	 NULL 
}; 


cfg_node_type_t cfg_type_wlan11ac = { 
	 .name = "WLan11ac", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_wlan11ac_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wlan11ac_child, 
	 .ops = &cfg_type_wlan11ac_ops, 
}; 
