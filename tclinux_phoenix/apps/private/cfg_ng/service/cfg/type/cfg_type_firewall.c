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

int svc_cfg_boot_firewall(void)
{	
	char nodeName[64] = {0};
	wan_related_evt_t param;
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, FIREWALL_ENTRY_NODE, sizeof(nodeName)-1);
	
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, nodeName, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_FIREWALL_BOOT, (void *)&param, sizeof(param)); 
	return 0;
}

#if defined(TCSUPPORT_CMCCV2)
int cfg_type_attack_entry_commit(char* path){
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_ATTACK_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}
#endif

int cfg_type_firewall_entry_commit(char* path)
{
	wan_related_evt_t param;
#if defined(TCSUPPORT_CWMP_TR181)
	updateTR181FireWall();
#endif
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_FIREWALL_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

#if defined(TCSUPPORT_CMCCV2)
static cfg_node_ops_t cfg_type_ipv6session_entry_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attack_entry_commit 
}; 

static cfg_node_type_t cfg_type_ipv6session_entry = { 
	 .name = "IPv6SessionEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_firewall, 
	 .ops = &cfg_type_ipv6session_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_portscan_entry_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attack_entry_commit 
}; 

static cfg_node_type_t cfg_type_portscan_entry = { 
	 .name = "PortScanEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_firewall, 
	 .ops = &cfg_type_portscan_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_dos_entry_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_attack_entry_commit 
}; 

static cfg_node_type_t cfg_type_dos_entry = { 
	 .name = "DoSEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_firewall, 
	 .ops = &cfg_type_dos_entry_ops, 
}; 
#endif

static cfg_node_ops_t cfg_type_firewall_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_firewall_entry_commit 
}; 


static cfg_node_type_t cfg_type_firewall_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_firewall, 
	 .ops = &cfg_type_firewall_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_firewall_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_firewall_entry_commit 
}; 


static cfg_node_type_t* cfg_type_firewall_child[] = { 
	 &cfg_type_firewall_entry,
#if defined(TCSUPPORT_CMCCV2)
	 &cfg_type_dos_entry,
	 &cfg_type_portscan_entry,
	 &cfg_type_ipv6session_entry,
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_firewall = { 
	 .name = "Firewall", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_firewall_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_firewall_child, 
	 .ops = &cfg_type_firewall_ops, 
}; 
