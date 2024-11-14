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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "svchost_evt.h"
#include "utility.h"

#define VPNDESTLIST							"VPNDestList"
#define MAX_FLOWNUM_RULE 					255
#define RULE_REMOTE_DOMAIN_MODE 			"domainMode"

#define BuffSet_Format(x, y, z) snprintf(x, sizeof(x), y, z)
#define BuffSet_Str(x, y) snprintf(x, sizeof(x), "%s", y)
static int vpndest_list_boot = 0;
extern cfg_node_type_t cfg_type_vpndest_list_pvc;


static int vpndestlist_pvcrule_clear(int pvc_index)
{
	char cmdbuf[256] = {0};
	
	/* flush and zero it */
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "ebtables -t broute -F vpn_dest_entry%d", pvc_index);
	system(cmdbuf);
	
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "ebtables -t broute -Z vpn_dest_entry%d", pvc_index);
	system(cmdbuf);

	return 0;
}

/*
add resolve IPv4 address to VPN LAN Entry.
*/
static int addvpndest(struct in_addr *addr, int mask, int pvc_index)
{
	char ipaddr[64] = {0};
	char cmdbuf[256] = {0};
	const char *ebtfrmt =
		"ebtables -t broute -%s vpn_dest_entry%d -p IPv4 "
		"--ip-dst %s/%d "
		"-j mark --mark-or 0x%x --mark-target CONTINUE";
#define VPN_PPP_FWMAKR_START 0x6e0000

	if ( !addr || pvc_index < 0 || pvc_index >= PVC_NUM )
		return -1;

	bzero(ipaddr, sizeof(ipaddr));
	inet_ntop(AF_INET, addr
			, ipaddr, sizeof(ipaddr));

	if ( 0 == ipaddr[0] )
		return -2;

	/* delete old rule and add it */
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, ebtfrmt
		, "D"
		, pvc_index
		, ipaddr, mask
		, VPN_PPP_FWMAKR_START);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, ebtfrmt
		, "A"
		, pvc_index
		, ipaddr, mask
		, VPN_PPP_FWMAKR_START);
	system(cmdbuf);

	return 0;
}

static int parsedone_vpn[PVC_NUM] = {0};
static int vpndestlist_domain_resolve(void)
{
	int flow_idx = 0, flow_num = 0, rule_idx = 0, ip_mask = 0;
	char nodeName[32] = {0}, s_rule_count[32];
	char remoteAddress[256] = {0}, domain_mode[24] = {0}, domain_addr[40] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL, *p_res = NULL;
	struct sockaddr_in *cvtaddr2 = NULL;

	if ( !vpndest_list_boot )
	{
		return 0;
	}

	for ( flow_idx = 0; flow_idx < 1; flow_idx ++ )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, VPNDESTLIST_PVC_NODE, flow_idx);	
		if (0 >= cfg_get_object_attr(nodeName, "RuleCount", s_rule_count, sizeof(s_rule_count)))
		{
			BuffSet_Str(s_rule_count, "0");
		}
		
		flow_num = atoi(s_rule_count);
		if ( flow_num <= 0 )
			continue;

		if ( parsedone_vpn[flow_idx] == flow_num )
			continue;
		else
			parsedone_vpn[flow_idx] = 0;

		/* IP class */
		for ( rule_idx = 0; rule_idx < MAX_FLOWNUM_RULE; rule_idx++ )
		{
			bzero(domain_mode, sizeof(domain_mode));
			bzero(remoteAddress, sizeof(remoteAddress));
			bzero(domain_addr, sizeof(domain_addr));
			bzero(nodeName, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VPNDESTLIST_PVC_ENTRY_NODE, (flow_idx + 1), (rule_idx + 1));
			
			if ( 0 >= cfg_get_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, domain_mode, sizeof(domain_mode)) )
				continue;

			if( 0 != strcmp(domain_mode, "Yes") )
			{
				parsedone_vpn[flow_idx]++;
				continue;
			}

			if ( 0 >= cfg_get_object_attr(nodeName, "destAddr", remoteAddress, sizeof(remoteAddress))
				|| 0 == remoteAddress[0] )
				continue;

			bzero(&hints, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;

			/* parse domain */
			if(0 != getaddrinfo(remoteAddress, NULL, &hints, &res))
				continue;

			for ( p_res = res; p_res; p_res = p_res->ai_next )
			{
				if ( p_res && AF_INET == p_res->ai_family )
				{
					cvtaddr2 = (struct sockaddr_in *) p_res->ai_addr;
					ip_mask = 32;
					addvpndest(&cvtaddr2->sin_addr, ip_mask, flow_idx);
				}
			}

			cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, "");
			if ( res )
				freeaddrinfo(res);
			parsedone_vpn[flow_idx]++;
		}
	}

	return 0;
}

static int vpndestlist_execute(void)
{
	char nodeName[32] = {0};
	int flow_idx = 0, rule_idx = 0, flow_num = 0;
	int ip_mask = 0, is_exist = 0;
	char remoteAddress[256] ={0}, start_addr[80]= {0};
	char s_flow_class_num[32] = {0};
	struct in_addr in_s_v4addr = {0};

	for ( flow_idx = 0; flow_idx < 1; flow_idx ++ )
	{
		vpndestlist_pvcrule_clear(flow_idx);
		/* IP class */
		flow_num = 0;
		for ( rule_idx = 0; rule_idx < MAX_FLOWNUM_RULE; rule_idx++ )
		{
			bzero(nodeName, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VPNDESTLIST_PVC_ENTRY_NODE, (flow_idx + 1), (rule_idx + 1));
			is_exist = 0;
			if (0 >= cfg_get_object_attr(nodeName, "destAddr", remoteAddress, sizeof(remoteAddress)))
			{
				remoteAddress[0] = 0;
			}
			else
			{
				is_exist = 1;
			}
			
			if ( is_exist )
			{
				flow_num ++;
			}

			if ( 0 != remoteAddress[0] )
			{
				cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, "");
				if ( NULL == strstr(remoteAddress, "/") ) /* single IP */
				{
					if ( 1 != inet_pton(AF_INET, remoteAddress, &in_s_v4addr) )
					{
						/* domain */
						cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, "Yes");
						continue;
					}

					ip_mask = 32;
				}
				else
				{
					/* IP mask range */
					sscanf(remoteAddress, "%[^/]/%d", start_addr, &ip_mask);

					if ( 1 != inet_pton(AF_INET, start_addr, &in_s_v4addr) )
						continue;
				}

				if ( 0 == ip_mask )
					continue;

				addvpndest(&in_s_v4addr, ip_mask, flow_idx);
			}
		}		

		/* update rule num */
		BuffSet_Format(s_flow_class_num, "%d", flow_num);
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VPNDESTLIST_PVC_NODE, flow_idx);
		cfg_set_object_attr(nodeName, "RuleCount", s_flow_class_num);
	}

	bzero(parsedone_vpn, sizeof(parsedone_vpn));

	return SUCCESS;
}


int svc_cfg_vpndestlist_boot(void)
{
	int flow_idx = 0;
	char cmdbuf[256] = {0};

	for ( flow_idx = 0; flow_idx < 1; flow_idx ++ )
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -N vpn_dest_entry%d -P RETURN", flow_idx);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -I BROUTING -j vpn_dest_entry%d", flow_idx);
		system(cmdbuf);

		/* flush and zero it */
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -F vpn_dest_entry%d", flow_idx);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -Z vpn_dest_entry%d", flow_idx);
		system(cmdbuf);
	}

	vpndest_list_boot = 1;
	vpndestlist_execute();

	return 0;
}

static int vpn_dest_list_commit(void)
{
	vpndestlist_execute();
	return 0;
}


static cfg_node_ops_t cfg_type_vpndest_list_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .commit = vpn_dest_list_commit,
	 .query = cfg_type_default_func_query 
}; 


static cfg_node_type_t cfg_type_vpndest_list_pvc_entry = { 
	 .name = "Entry", 
	 .flag =  CFG_TYPE_FLAG_MULTIPLE | MAX_FLOWNUM_RULE , 
	 .parent = &cfg_type_vpndest_list_pvc, 
	 .ops = &cfg_type_vpndest_list_pvc_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_vpndest_list_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .commit = vpn_dest_list_commit,
	 .query = cfg_type_default_func_query 
}; 

static cfg_node_type_t* cfg_type_vpndest_list_pvc_child[] = { 
	 &cfg_type_vpndest_list_pvc_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_vpndest_list_pvc = { 
	 .name = "PVC", 
	 .flag =  CFG_TYPE_FLAG_MULTIPLE | 1 , 
	 .parent = &cfg_type_vpndest_list, 
	 .nsubtype = sizeof(cfg_type_vpndest_list_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_vpndest_list_pvc_child,  
	 .ops = &cfg_type_vpndest_list_pvc_ops, 
}; 


static cfg_node_ops_t cfg_type_vpndest_list_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = vpn_dest_list_commit 
};


static cfg_node_type_t* cfg_type_vpndest_list_child[] = { 
	 &cfg_type_vpndest_list_pvc, 
	 NULL 
}; 

cfg_node_type_t cfg_type_vpndest_list = { 
	 .name = "VPNDestList", 
	 .flag =  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_vpndest_list_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_vpndest_list_child, 
	 .ops = &cfg_type_vpndest_list_ops, 
}; 

