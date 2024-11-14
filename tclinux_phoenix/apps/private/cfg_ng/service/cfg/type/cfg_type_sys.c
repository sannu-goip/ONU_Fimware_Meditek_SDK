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
#include "utility.h" 
#include "blapi_traffic.h"
#if defined(TCSUPPORT_CWMP_TR181)
#include "cfg_tr181_utility.h" 
#endif

#if defined(TCSUPPORT_CT_JOYME2)
int init_lan_port_stat(void)
{
	int i =0, ret = -1;
	char node_name[32] = {0}, real_rate[64] = {0};
	lanport_info_t lanport_info;
	
	memset(&lanport_info, 0, sizeof(lanport_info));
	snprintf(node_name, sizeof(node_name), "root.sys.entry");
	ret = blapi_traffic_get_lanport_info(&lanport_info);
	for(i = 0;  i < 4; i++)
	{
		sscanf(lanport_info.link_rate[i], "%[^/]", real_rate);
		if ( lanport_info.lan_port[i][0] == '\0' || lanport_info.lan_status[i][0] == '\0' ) 
			continue;
			
			if (!strcmp(real_rate, "10M")) {
			cfg_set_object_attr(node_name, lanport_info.lan_port[i], "10000");		
			cfg_set_object_attr(node_name, lanport_info.lan_status[i], "1");
			}
			else if (!strcmp(real_rate, "100M"))  {
			cfg_set_object_attr(node_name, lanport_info.lan_port[i], "100000"); 	
			cfg_set_object_attr(node_name, lanport_info.lan_status[i], "1");
			}
			else if (!strcmp(real_rate, "1000M")) {
			cfg_set_object_attr(node_name, lanport_info.lan_port[i], "1000000");		
			cfg_set_object_attr(node_name, lanport_info.lan_status[i], "1");
			}
			else {
			cfg_set_object_attr(node_name, lanport_info.lan_port[i], "0");		
			cfg_set_object_attr(node_name, lanport_info.lan_status[i], "0");
			}
		}
		
	return 0;
}
#endif

#ifdef TCSUPPORT_IPV6
static void sys_setIpv6Radio(void)
{
	char nodeName[128] 	= {0};
	char ipVersion[5] 	= {0};
#if defined(TCSUPPORT_CT_IPV4_RADIO)			
	char string[64] = {0};
#endif
#if defined(TCSUPPORT_WAN_ETHER) 
	char transMode[16];
#endif

	snprintf(nodeName, sizeof(nodeName), SYS_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodeName, "IPProtocolVersion", 0, ipVersion, sizeof(ipVersion)) > 0){
	/*create system.conf*/
#if defined(TCSUPPORT_CT_IPV4_RADIO)		
		snprintf(string, sizeof(string), "/%s/%s", "var/run", "system.conf");
		if(cfg_save_attrs_to_file(nodeName, string, NO_QMARKS) < 0){
			return;
		}
#endif

		if(!strcmp(ipVersion, "3")){/*IPv6 is enable*/
			doValPut("/proc/sys/net/ipv6/conf/all/disable_ipv6", "0\n");
#if defined(TCSUPPORT_WAN_ETHER) 
			memset(transMode,0,sizeof(transMode));
			if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode))
				&& 0 == strcmp(transMode, "Ethernet"))/*Ethernet Mode*/
			{
				doValPut("/proc/sys/net/ipv6/conf/nas10/disable_ipv6", "1");
			}

#endif

#if defined(TCSUPPORT_CT_IPV4_RADIO)		
			doValPut("/proc/sys/net/ipv4/conf/all/disable_ipv4", "0\n");
#endif
		}
		else if(!strcmp(ipVersion, "2")){
			doValPut("/proc/sys/net/ipv6/conf/all/disable_ipv6", "0\n");
#if defined(TCSUPPORT_CT_IPV4_RADIO)		
			doValPut("/proc/sys/net/ipv4/conf/all/disable_ipv4", "1\n");
#endif
		}
		else if(!strcmp(ipVersion, "1")){/*IPv6 is disabled*/
			doValPut("/proc/sys/net/ipv6/conf/all/disable_ipv6", "1\n");
#if defined(TCSUPPORT_CT_IPV4_RADIO)
			doValPut("/proc/sys/net/ipv4/conf/all/disable_ipv4", "0\n");
#endif
		}	
	}
	else{
		printf("setIpv6Radio: get SYS_IP_VERSION fail! \r\n");
	}		
}
#endif

#if defined(TCSUPPORT_CT_PORT_BIND)
static void sys_checkLanBind(void)
{
	char nodeName[128] 		= {0};
	char lanbindCheck[8] 	= {0};

  	snprintf(nodeName, sizeof(nodeName), SYS_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodeName, "lanbindCheck", 0, lanbindCheck, sizeof(lanbindCheck)) > 0
														&& 0 == strcmp(lanbindCheck,"1")){
		system("/usr/bin/portbindcmd checkLanBind 1");
	}else{		
		system("/usr/bin/portbindcmd checkLanBind 0");
	}
}
#endif

#if defined(TCSUPPORT_CWMP_TR181)
void set_tr181_ipv4_ipv6_enable(void)
{
	char IPProtocolVersion[2] = {0};

	cfg_obj_get_object_attr(SYS_ENTRY_NODE, "IPProtocolVersion", 0, IPProtocolVersion, sizeof(IPProtocolVersion));
	if(atoi(IPProtocolVersion) == 1)
	{
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv4Enable", "1");
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv6Enable", "0");
	}
	else if(atoi(IPProtocolVersion) == 2)
	{
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv4Enable", "0");
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv6Enable", "1");
	}
	else if(atoi(IPProtocolVersion) == 3)
	{
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv4Enable", "1");
		cfg_set_object_attr(TR181_IP_COMMON_NODE, "IPv6Enable", "1");
	}

	return;
}
#endif

static int sys_execute()
{
#ifdef TCSUPPORT_IPV6
	sys_setIpv6Radio();
#endif
#if defined(TCSUPPORT_CT_PORT_BIND)
	sys_checkLanBind();
#endif

#if defined(TCSUPPORT_CWMP_TR181)
	set_tr181_ipv4_ipv6_enable();
#endif

	return 0;
}

int svc_cfg_boot_sys(void)
{
#ifdef TCSUPPORT_IPV6
	sys_setIpv6Radio();
#endif
#if defined(TCSUPPORT_CT_PORT_BIND)
	sys_checkLanBind();
#endif
	
#if defined(TCSUPPORT_CT_JOYME2)
	init_lan_port_stat();
#endif
#if defined(TCSUPPORT_CWMP_TR181)
	set_tr181_ipv4_ipv6_enable();
#endif

	return 0;
}

static int cfg_type_sys_entry_func_commit(char* path)
{	
	int ret = -1;
	ret = sys_execute();

	return ret;
}

#if defined(TCSUPPORT_CT_JOYME2)
static cfg_node_ops_t cfg_type_sys_dbus_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 
static cfg_node_type_t cfg_type_sys_dbus = { 
	 .name = "dbus", 
	 .flag = 1, 
	 .parent = &cfg_type_sys, 
	 .ops = &cfg_type_sys_dbus_ops, 
};
#endif

static cfg_node_ops_t cfg_type_sys_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sys_entry_func_commit 
}; 

static cfg_node_type_t cfg_type_sys_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_sys, 
	 .ops = &cfg_type_sys_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_sys_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sys_entry_func_commit 
}; 

static cfg_node_type_t* cfg_type_sys_child[] = { 
	 &cfg_type_sys_entry, 
#if defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_sys_dbus, 
#endif
	 NULL 
}; 

cfg_node_type_t cfg_type_sys = { 
	 .name = "Sys", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_sys_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_sys_child, 
	 .ops = &cfg_type_sys_ops, 
}; 
