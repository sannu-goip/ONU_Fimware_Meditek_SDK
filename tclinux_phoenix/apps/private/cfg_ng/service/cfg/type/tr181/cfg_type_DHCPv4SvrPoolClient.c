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
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h" 


int cfg_type_DHCPv4Spc_get(char* path,char* attr,char* val,int len)
{
	char ip[64]={0};
	char hostname[128] = {0};
	char expireTime[32] = {0};
	char tmp[80] = {0};

	if(attr == NULL ){
		return -1;
	}

	if(strcmp(attr, "LeaseTime") != 0 ){
		return 0;
	}
	
	if(cfg_obj_get_object_attr(path, "IP", 0,ip,sizeof(ip)) >= 0 )
	{
		if (get_dhcpLease_status(ip, expireTime, hostname, sizeof(hostname)) != 0)
		{
			memset(tmp, 0x00, sizeof(tmp));
			if(strlen(expireTime) != 0)
				snprintf(tmp,sizeof(tmp), "%s", expireTime);
			else
				strncpy(tmp, "0", sizeof(tmp)-1);
			
			cfg_obj_set_object_attr(path, "LeaseTime" , 0, tmp);
		}
	}
	
	return 0;
}

int cfg_type_DHCPv4Spc_func_get(char* path,char* attr,char* val,int len) {
	
	cfg_type_DHCPv4Spc_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len);

}


int updateDHCPv4Spc()
{	
	char nodeName[MAXLEN_NODE_NAME]={0};
	char lanHostNode[MAXLEN_NODE_NAME]={0};
	int i= 0,j = 0;
	char hostname[128] = {0};
	char expireTime[32] = {0};
	char tmp[64] = {0};
	char mac[64]={0};
	char ipaddr[64] = {0};
	char enable[8] = {0};

	for(i = 0;i < DHCPV4_SPC_NUM_MAX;i++)
	{
		memset(nodeName,0,sizeof(nodeName));		
		snprintf(nodeName,sizeof(nodeName),TR181_DHCPV4_SVRPOLC_ENTRY_NODE, i+1);
		cfg_obj_delete_object(nodeName);
	}

	for(i = 0;i < 64;i++)
	{
		memset(lanHostNode,0,sizeof(lanHostNode));
		snprintf(lanHostNode,sizeof(lanHostNode),LANHOST2_ENTRY_NODE, i+1);
		memset(ipaddr,0,sizeof(ipaddr));
		if(cfg_obj_get_object_attr(lanHostNode, "IP", 0,ipaddr,sizeof(ipaddr)) >= 0 )
		{
			if (get_dhcpLease_status(ipaddr, expireTime, hostname, sizeof(hostname)) != 0)
			{
				memset(tmp, 0x00, sizeof(tmp));
				if(strlen(expireTime) != 0)
					snprintf(tmp,sizeof(tmp), "%s", expireTime);
				else
					strncpy(tmp, "0", sizeof(tmp)-1);
			}

			memset(nodeName,0,sizeof(nodeName));		
			snprintf(nodeName,sizeof(nodeName),TR181_DHCPV4_SVRPOLC_ENTRY_NODE, j+1);
			if(cfg_obj_query_object(nodeName, NULL, NULL) <= 0)
			{
				cfg_obj_create_object(nodeName);				
			}			
			if(cfg_obj_get_object_attr(lanHostNode, "Active", 0, enable,sizeof(enable)) > 0)
				cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0,enable);
			
			cfg_obj_set_object_attr(nodeName, "IP" , 0,ipaddr);
			cfg_obj_set_object_attr(nodeName, "LeaseTime" , 0, tmp);
			
			memset(tmp, 0,sizeof(tmp));
			memset(mac, 0, sizeof(mac));
			memset(hostname, 0, sizeof(hostname));
			if(cfg_obj_get_object_attr(lanHostNode,"MAC" , 0, mac,sizeof(mac)) > 0)
			{	
				mac_add_dot(mac,tmp);
				cfg_obj_set_object_attr(nodeName, "Mac" , 0,tmp);
			}

			if(cfg_obj_get_object_attr(lanHostNode,"HostName" , 0, hostname, sizeof(hostname)) > 0)
			{	
				cfg_obj_set_object_attr(nodeName, "HostName" , 0, hostname);
			}
			j++;
		}
	}


	return 0;
}


static cfg_node_ops_t cfg_type_dhcpv4svrpoolclient_entry_ops  = { 
	 .get = cfg_type_DHCPv4Spc_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4svrpoolclient_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV4_SPC_NUM_MAX, 
	 .parent = &cfg_type_dhcpv4svrpoolclient, 
	 .ops = &cfg_type_dhcpv4svrpoolclient_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv4svrpoolclient_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4svrpoolclient_child[] = { 
	 &cfg_type_dhcpv4svrpoolclient_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4svrpoolclient= { 
	 .name = "DHCPv4SPC", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4svrpoolclient_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4svrpoolclient_child, 
	 .ops = &cfg_type_dhcpv4svrpoolclient_ops, 
}; 
