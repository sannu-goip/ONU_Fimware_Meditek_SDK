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
#include "cfg_msg.h"
#include <svchost_evt.h>

int svc_cfg_boot_mobile(void)
{
#if !defined(TCSUPPORT_CMCCV2)	
	char buffer[4] = "\0"; 
	char LedStatus[4] = {0};

	if(cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "ledstatus", 0, LedStatus, sizeof(LedStatus)) > 0)
	{
		if(!strcmp(LedStatus, "off"))
		{
			doValPut("/proc/tc3162/led_switch_button", "2\n");
		}
		else if(!strcmp(LedStatus, "on"))
		{
			doValPut("/proc/tc3162/led_switch_button", "3\n");
		}
	}

	if(cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "test", 0, buffer, sizeof(buffer)) > 0)
	{
		if(atoi(buffer) == 1)
		{
			system("/usr/local/ct/mobile-manager &");
			return SUCCESS;
		}
	}
	system("/userfs/bin/mobile-manager &");
	
#endif

	return SUCCESS;
}

int cfg_type_mobile_entry_func_commit(char* path)
{
	int ret = -1;
	char attrValue[32] = {0};
	char oldValue[32] = {0};
	
	cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "MgtURL", 0, attrValue, sizeof(attrValue));
	cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "oldURL", 0, oldValue, sizeof(oldValue));

	if (ret = strncmp(attrValue, oldValue, sizeof(attrValue))) 
	{
		/*MgtURL has changed, save as oldURL for next change*/
		cfg_set_object_attr(MOBILE_ENTRY_NODE, "oldURL", attrValue );
	}
	
	memset(attrValue, 0, sizeof(attrValue));
	memset(oldValue, 0, sizeof(oldValue));
	cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "Port", 0, attrValue, sizeof(attrValue));
	cfg_obj_get_object_attr(MOBILE_ENTRY_NODE, "oldPort", 0, oldValue, sizeof(oldValue));
	
	if (ret += strncmp(attrValue, oldValue, sizeof(attrValue))) 
	{
		/*Port has changed, save as Port for next change*/
		cfg_set_object_attr(MOBILE_ENTRY_NODE, "oldPort", attrValue );
	}
	
	if (ret != 0 ) 
	{
		sendToMobileManagerFIFO(URL_TYPE);
	}

#if defined(TCSUPPORT_CT_JOYME4)
	cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", "1");
	cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", "0");
	cfg_set_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", "0");
#endif

	return SUCCESS;
}

int cfg_type_mobile_wifiex_func_commit(char* path)
{
	int i=0;
	int count = 0;
	char attrName[32] = {0};
	char attrValue[32] = {0};
	
	for(i=1; i<=COUNT_WIFITIMER_ITMS; i++)
	{
		memset(attrValue, 0, sizeof(attrValue));	
		memset(attrName, 0, sizeof(attrName));	
		snprintf(attrName, sizeof(attrName), "Active%d", i);		
		if(cfg_obj_get_object_attr(MOBILE_WIFIEX_NODE, attrName, 0, attrValue, sizeof(attrValue)) > 0)
		{
			count++;
		}
	}

	memset(attrValue, 0, sizeof(attrValue));	
	snprintf(attrValue, sizeof(attrValue), "%d", count);
	cfg_set_object_attr(MOBILE_WIFIEX_NODE, "Count", attrValue );
	sendToMobileManagerFIFO(WIFITIMER_TYPE);

	return SUCCESS;
}


static cfg_node_ops_t cfg_type_mobile_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_mobile_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_dnsredirect_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_dnsredirect = { 
	 .name = "DnsRedirect", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_dnsredirect_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_sleep_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_sleep = { 
	 .name = "Sleep", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_sleep_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_wifiex_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_mobile_wifiex_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_wifiex = { 
	 .name = "WifiEx", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_wifiex_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_osright_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_osright = { 
	 .name = "OSRight", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_osright_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_pluginmonitor_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_mobile_pluginmonitor = { 
	 .name = "PluginMonitor", 
	 .flag = 1, 
	 .parent = &cfg_type_mobile, 
	 .ops = &cfg_type_mobile_pluginmonitor_ops, 
}; 


static cfg_node_ops_t cfg_type_mobile_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_mobile_child[] = { 
	 &cfg_type_mobile_entry, 
	 &cfg_type_mobile_dnsredirect, 
	 &cfg_type_mobile_sleep, 
	 &cfg_type_mobile_wifiex, 
	 &cfg_type_mobile_osright, 
	 &cfg_type_mobile_pluginmonitor, 
	 NULL 
}; 


cfg_node_type_t cfg_type_mobile = { 
	 .name = "mobile", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_mobile_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_mobile_child, 
	 .ops = &cfg_type_mobile_ops, 
}; 
