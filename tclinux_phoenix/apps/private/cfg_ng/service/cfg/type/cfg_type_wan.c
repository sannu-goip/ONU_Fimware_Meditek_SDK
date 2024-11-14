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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_wan_common.h"
#include "cfg_msg.h"
#include "blapi_traffic.h"
#if defined(TCSUPPORT_CT_UBUS)
#include "cfg_romfile.h"
#endif

extern cfg_node_type_t cfg_type_wan_pvc;

static char* cfg_type_wan_index[] = { 
	 "wan_pvc", 
	 NULL 
}; 

void cfg_type_wan_send_evt(int type, int evt_id, char* buf)
{
	wan_evt_t wan_evt;
	memset(&wan_evt, 0, sizeof(wan_evt));
	char evt_type[EVT_TYPE_LENGTH];
	
	if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
	{
		printf("error path is NULL || path length is too long.\n");
		return ;
	}
	
	strcpy(wan_evt.buf, buf);
	memset(evt_type, 0, sizeof(evt_type));
	switch (type)
	{
		case EVT_WAN_INTERNAL_TYPE :
		{
			strcpy(evt_type, EVT_WAN_INTERNAL);
			break;
		}
		case EVT_WAN_EXTERNAL_TYPE:
		{
			strcpy(evt_type, EVT_WAN_EXTERNAL);
			break;
		}
		default:
		{
			return ;
		}
	}
	
	cfg_obj_send_event(evt_type, evt_id, (void*)&wan_evt, sizeof(wan_evt));
	
	return ;
}

int
wan_check_filter(void){
	int filter_on = 0, i;
	char active[4]={0}, isp[4]={0};

	for(i = 0; i < MAX_WAN_IF_INDEX; i++){
		if(get_waninfo_by_index(i, "Active", active, sizeof(active)) == SUCCESS &&
			get_waninfo_by_index(i, "ISP", isp, sizeof(isp)) == SUCCESS){
			/*interface is active and is not bridge mode*/
			if(!strcmp(active, "Yes") && strcmp(isp, BRIDGE_MODE)){
				filter_on = 1;
				break;
			}
		}
			
	}
#if defined(TCSUPPORT_CT_UBUS)
	filter_on = 1;
#endif
	return filter_on;
}

int set_wan_filter_state(void){
	unsigned int new_filter_state=0;
	char newFilterState[CFG_BUF_32_LEN] = {0};
	char nodePath[CFG_BUF_64_LEN] = {0};
		
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_obj_get_object_attr(nodePath, "FilterState", 0, newFilterState, sizeof(newFilterState)) ;
#if !defined(TCSUPPORT_ANDLINK)
	if(wan_check_filter()){/*filter on*/
		new_filter_state=atoi(newFilterState)|WAN_VECTOR;
	}
	else{/*filter down*/
		new_filter_state=atoi(newFilterState)&(~WAN_VECTOR);
	}
#else
	new_filter_state = 1;
#endif
	
	check_and_set_filter(new_filter_state);

	return 0;
}

int svc_cfg_boot_wan(void)
{	

	set_wan_filter_state();
#if defined(TCSUPPORT_NPTv6)
	cfg_set_object_attr(RADVD_ENTRY_NODE, "npt_wan_idx", "");
#endif

	blapi_traffic_set_wan_acnt_enable(1); 
#if defined(TCSUPPORT_CT_JOYME4)
	/* create ICMP chain for wan interfaces. */
	system("iptables -t filter -N ICMP_WAN_LIMIT");
	system("iptables -t filter -A INPUT -j ICMP_WAN_LIMIT");
	system("iptables -t filter -F ICMP_WAN_LIMIT");
	system("iptables -t filter -Z ICMP_WAN_LIMIT");

	system("ip6tables -t filter -N ICMP_WAN_LIMIT");
	system("ip6tables -t filter -A INPUT -j ICMP_WAN_LIMIT");
	system("ip6tables -t filter -F ICMP_WAN_LIMIT");
	system("ip6tables -t filter -Z ICMP_WAN_LIMIT");
#endif

	cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_BOOT, WAN_NODE);
	return 0;
} 

#if defined(TCSUPPORT_CWMP_TR181)	
int updateTR181IFByWan(char* path){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int pvcIdx = -1;
	int entIdx = -1;
	int wanIf = -1;
	char tmp[2] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, GLOBALSTATE_COMMON_NODE, sizeof(nodeName) - 1);
	if(cfg_obj_get_object_attr(nodeName, "CommitFromTR181", 0, tmp, sizeof(tmp)) < 0
		|| strcmp(tmp, "1") != 0)
	{
		sscanf(path, WAN_PVC_ENTRY_NODE, &pvcIdx, &entIdx);
		if(pvcIdx > 0 && entIdx > 0){
			wanIf = (pvcIdx - 1)*PVC_NUM + (entIdx - 1);
			updateTR181IFByPvcIdx(wanIf);
			updateInterfaceStack();
		}
	}


	return 0;

}

int ClearTR181IFByWan(char* path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int pvcIdx = -1;
	int entIdx = -1;
	int wanIf = -1;
	char tmp[2] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, GLOBALSTATE_COMMON_NODE, sizeof(nodeName) - 1);
	if(cfg_obj_get_object_attr(nodeName, "CommitFromTR181", 0, tmp, sizeof(tmp)) < 0
		|| strcmp(tmp, "1") != 0)
	{
		sscanf(path, WAN_PVC_ENTRY_NODE, &pvcIdx, &entIdx);
		if(pvcIdx > 0 && entIdx > 0)
		{
			wanIf = (pvcIdx - 1)*PVC_NUM + (entIdx - 1);
			clearAllTR181WanInterfaceNodeByWanIF(wanIf);
			updateInterfaceStack();
		}
	}


	return 0;

}

#endif

static int cfg_type_wan_pvc_entry_func_commit(char* path)
{	
#if defined(TCSUPPORT_CWMP_TR181)	
	updateTR181IFByWan(path);
#endif

	cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_UPDATE, path);
	return 0;
} 

static int cfg_type_wan_pvc_func_commit(char* path)
{	
	int 			i = 0;
	char 		 node[32];
    
	if(NULL == path)
	{
		return 0;
	}
	
	if (cfg_obj_query_object(path,NULL,NULL) <= 0)
	{
		return 0;
	}
	
	for(i=0; i<MAX_WAN_ENTRY_NUMBER; i++)
	{
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node),"%s.entry.%d", path, (i+1));
		if (cfg_obj_query_object(node,NULL,NULL) <= 0)
		{
			continue;
		}
		
#if defined(TCSUPPORT_CWMP_TR181)	
		updateTR181IFByWan(node);
#endif
	
        /*commit root.pvc.%d.entry.%d*/
        cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_UPDATE, node);
	}

	return 0;
} 

static int cfg_type_wan_pvc_entry_func_delete(char* path)
{
	if(NULL == path)
	{
		return 0;
	}
	
	if (cfg_obj_query_object(path,NULL,NULL) <= 0)
	{
		return 0;
	}
#if defined(TCSUPPORT_CWMP_TR181)	
	ClearTR181IFByWan(path);
#endif
	cfg_obj_delete_object(path);
	cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_DELETE, path);
	
	return 0;
}


static int cfg_type_wan_pvc_func_delete(char* path)
{
	int 			i = 0;
	char 		 node[32];

	if(NULL == path)
	{
		return 0;
	}

	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
	{
		return 0;
	}

	for(i=0; i<MAX_WAN_ENTRY_NUMBER; i++)
	{
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node),"%s.entry.%d", path, (i+1));
		cfg_type_wan_pvc_entry_func_delete(node);
	}

	cfg_obj_delete_object(path);
	
	return 0;
}

#if defined(TCSUPPORT_CT_UBUS)
int init_wan_attributes(char *workmode)
{
	char nodeName[32] = {0};

	if ( NULL == workmode )
		return -1;

	/* set wan_pvc0 attrs */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, 1);
	cfg_set_object_attr(nodeName, "VLANMode", "TRANSPARENT");
	cfg_set_object_attr(nodeName, "VLANID", "4097");
	cfg_set_object_attr(nodeName, "DOT1P", "0");
	cfg_set_object_attr(nodeName, "ATMEnable", "No");
	cfg_set_object_attr(nodeName, "PTMEnable", "No");
	cfg_set_object_attr(nodeName, "GPONEnable", "Yes");
	cfg_set_object_attr(nodeName, "EPONEnable", "Yes");
	cfg_set_object_attr(nodeName, "ENCAP", "1483 Bridged IP LLC");

	/* set wan_pvc0_entry0 attrs */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 1, 1);
	cfg_set_object_attr(nodeName, "Active", "Yes");
	cfg_set_object_attr(nodeName, "ServiceList", "INTERNET");
	cfg_set_object_attr(nodeName, "BandActive", "N/A");
	cfg_set_object_attr(nodeName, "LAN1", "No");
	cfg_set_object_attr(nodeName, "LAN2", "No");
	cfg_set_object_attr(nodeName, "LAN3", "No");
	cfg_set_object_attr(nodeName, "LAN4", "No");
	cfg_set_object_attr(nodeName, "SSID1", "No");
	cfg_set_object_attr(nodeName, "SSID2", "No");
	cfg_set_object_attr(nodeName, "SSID3", "No");
	cfg_set_object_attr(nodeName, "SSID4", "No");
	cfg_set_object_attr(nodeName, "IPVERSION", "IPv4");
	cfg_set_object_attr(nodeName, "VLANMode", "TRANSPARENT");
	cfg_set_object_attr(nodeName, "dot1q", "No");
	cfg_set_object_attr(nodeName, "dot1p", "No");
	cfg_set_object_attr(nodeName, "IGMPproxy", "No");
	cfg_set_object_attr(nodeName, "IFIdx", "1");
	cfg_set_object_attr(nodeName, "NASName", "nas0_0");
	cfg_set_object_attr(nodeName, "IFName", "nas0_0");
	cfg_set_object_attr(nodeName, "ConnectionError", "ERROR_NO_ANSWER");
	if ( 0 == strcmp(workmode, "router") || 0 == strcmp(workmode, "repeater") )
	{
		cfg_set_object_attr(nodeName, "WanMode", "Route");
		cfg_set_object_attr(nodeName, "LinkMode", "linkIP");
		cfg_set_object_attr(nodeName, "MTU", "1500");
		cfg_set_object_attr(nodeName, "ISP", "0");
		cfg_set_object_attr(nodeName, "NATENABLE", "Enable");
		cfg_set_object_attr(nodeName, "option60String", "");
		cfg_set_object_attr(nodeName, "Barrier", "0");
		cfg_set_object_attr(nodeName, "DOT1P", "0");
		cfg_set_object_attr(nodeName, "dot1pData", "0");
	}
	else if ( 0 == strcmp(workmode, "bridge") )
	{
		cfg_set_object_attr(nodeName, "WanMode", "Bridge");
		cfg_set_object_attr(nodeName, "LinkMode", "linkPPP");
		cfg_set_object_attr(nodeName, "BridgeMode", "IP_Bridged");
		cfg_set_object_attr(nodeName, "NATENABLE", "Disabled");
		cfg_set_object_attr(nodeName, "DHCPRelay", "No");
		cfg_set_object_attr(nodeName, "ISP", "3");
		cfg_set_object_attr(nodeName, "MulticastVID", "");
		cfg_set_object_attr(nodeName, "DOT1P", "");
	}
	cfg_evt_write_romfile_to_flash();

	return 0;
}
int load_wan_default()
{
	char workmode[16] = {0}, nodeName[32] = {0};
	cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, workmode, sizeof(workmode));
	if ( 0 == workmode[0] || 0 != strcmp(workmode, "router") )
		return -1;

	/* create wan_pvc0_entry0 */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, 1);
	if ( cfg_obj_query_object(nodeName, NULL, NULL) <= 0 )
	{
		if ( cfg_obj_create_object(nodeName) > 0 )
		{
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 1, 1);
			if ( cfg_obj_create_object(nodeName) > 0 )
			{
				/* set default attr */
				init_wan_attributes(workmode);
			}
			else
				return -1;
		}
		else
			return -1;
	}
	else
	{
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 1, 1);
		if ( cfg_obj_query_object(nodeName, NULL, NULL) <= 0 )
		{
			if ( cfg_obj_create_object(nodeName) > 0 )
			{
				/* set default attr */
				init_wan_attributes(workmode);
			}
			else
				return -1;
		}
	}
	
	return 0;
}

int switch_dhcpd_active()
{
	char workmode[16] = {0}, nodeName[32] = {0};
	cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, workmode, sizeof(workmode));
	if ( 0 == workmode[0] )
		return -1;

	/* dhcpd */
	if ( 0 == strcmp(workmode, "router") )
	{
		/* start dhcpd */
		cfg_set_object_attr(LAN_DHCP_NODE, "type", "1");
		cfg_delete_object(DHCPRELAY_ENTRY_NODE);
		cfg_commit_object(DHCPD_COMMON_NODE);
		cfg_commit_object(LAN_DHCP_NODE);
		
		/* start dhcp6s & radvd */
		cfg_set_object_attr(DHCP6S_ENTRY_NODE, "Enable", "1");
		cfg_commit_object(DHCP6S_ENTRY_NODE);
		cfg_set_object_attr(RADVD_ENTRY_NODE, "Enable", "1");
		cfg_commit_object(RADVD_ENTRY_NODE);
		cfg_evt_write_romfile_to_flash();
	}
	else if ( 0 == strcmp(workmode, "bridge") || 0 == strcmp(workmode, "repeater") )
	{
		/* stop dhcpd */
		cfg_set_object_attr(LAN_DHCP_NODE, "type", "0");
		cfg_delete_object(DHCPRELAY_ENTRY_NODE);
		cfg_commit_object(DHCPD_COMMON_NODE);
		cfg_commit_object(LAN_DHCP_NODE);
		
		/* stop dhcp6s & radvd */
		cfg_set_object_attr(DHCP6S_ENTRY_NODE, "Enable", "0");
		cfg_commit_object(DHCP6S_ENTRY_NODE);
		cfg_set_object_attr(RADVD_ENTRY_NODE, "Enable", "0");
		cfg_commit_object(RADVD_ENTRY_NODE);
		cfg_evt_write_romfile_to_flash();
	}
	return 0;
}
static int cfg_type_common_func_commit(char* path)
{
	char nodeName[32] = {0}, pvcnode[32] = {0};
	int i, j;

	/* AP WorkMode change, clear all wan setting */
	for ( i = 0; i < MAX_WAN_PVC_NUMBER; i++ )
	{
		memset(pvcnode, 0, sizeof(pvcnode));
		snprintf(pvcnode, sizeof(pvcnode), WAN_PVC_NODE, i + 1);
		for ( j = 0; j < MAX_WAN_ENTRY_NUMBER; j++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, i + 1, j + 1);
			if ( cfg_obj_query_object(nodeName, NULL, NULL) <= 0 )
				continue;
			cfg_obj_delete_object(nodeName);
			cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_DELETE, nodeName);
		}
		if ( cfg_obj_query_object(pvcnode, NULL, NULL) > 0 &&
			cfg_obj_query_object(pvcnode, "entry", NULL) <= 0 )
			cfg_obj_delete_object(pvcnode);
	}

	/* dhcpd */
	switch_dhcpd_active();
	
	/* set defaut wan config */
	load_wan_default();

	cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_COMMON_UPDATE, path);
	
	return 0;
}
#endif

static cfg_node_ops_t cfg_type_wan_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_wan_pvc_entry_func_delete, 
	 .commit = cfg_type_wan_pvc_entry_func_commit,
	 .query = cfg_type_default_func_query 
}; 


static cfg_node_type_t cfg_type_wan_pvc_entry = { 
	 .name = "Entry", 
	 .flag =  CFG_TYPE_FLAG_MULTIPLE | 8 , 
	 .parent = &cfg_type_wan_pvc, 
	 .index = cfg_type_wan_index, 
	 .ops = &cfg_type_wan_pvc_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_wan_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_wan_pvc_func_delete, 
	 .commit = cfg_type_wan_pvc_func_commit,
	 .query = cfg_type_default_func_query 
}; 

static cfg_node_type_t* cfg_type_wan_pvc_child[] = { 
	 &cfg_type_wan_pvc_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_wan_pvc = { 
	 .name = "PVC", 
	 .flag =  CFG_TYPE_FLAG_MULTIPLE | 8 , 
	 .parent = &cfg_type_wan, 
	 .index = cfg_type_wan_index, 
	 .nsubtype = sizeof(cfg_type_wan_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wan_pvc_child,  
	 .ops = &cfg_type_wan_pvc_ops, 
}; 




static cfg_node_ops_t cfg_type_wan_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
#if defined(TCSUPPORT_CT_UBUS)
	 .commit = cfg_type_common_func_commit 
#else
	 .commit = cfg_type_default_func_commit 
#endif
}; 


static cfg_node_type_t cfg_type_wan_common = { 
	 .name = "Common", 
	 .flag =  1, 
	 .parent = &cfg_type_wan, 
	 .ops = &cfg_type_wan_common_ops, 
}; 


static cfg_node_ops_t cfg_type_wan_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
};


static cfg_node_type_t* cfg_type_wan_child[] = { 
	 &cfg_type_wan_pvc, 
	 &cfg_type_wan_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_wan = { 
	 .name = "Wan", 
	 .flag =  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_wan_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wan_child, 
	 .ops = &cfg_type_wan_ops, 
}; 
