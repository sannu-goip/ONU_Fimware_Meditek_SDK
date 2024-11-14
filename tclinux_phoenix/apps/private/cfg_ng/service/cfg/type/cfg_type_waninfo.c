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
#include <malloc.h>
#include <time.h>
#include <svchost_evt.h> 
#include <cfg_cli.h> 

#include "cfg_types.h" 
#include "cfg_wan_common.h"
#include "cfg_msg.h"
#include "utility.h"
#include "global_def.h"

#if defined(TCSUPPORT_CMCCV2)
#include <syslog.h>
#endif
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#include <lan_port/lan_port_info.h>
#include <lan_port/bind_list_map.h>
#include <lan_port/itf_policy_route_table.h>
#endif

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_NP_CMCC)
#define MAX_BUFF_SIZE_CNT 17
#else
#if defined(TCSUPPORT_CT_JOYME4)
#define MAX_BUFF_SIZE_CNT 16
#else
#define MAX_BUFF_SIZE_CNT 15
#endif 
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
#define SMALL_BUFF_CNT 14
#define CYCLEVALUE_MAXCNT 29
#else
#define SMALL_BUFF_CNT 12
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#define CYCLEVALUE_MAXCNT 30
#else
#define CYCLEVALUE_MAXCNT 27
#endif

#define MAX_BUFF_SIZE 			1024
#define SMALL_BUFF_SIZE 		512

#ifndef PURE_BRIDGE
	#define PVC_NUM 8
#else
	#define PVC_NUM 4
#endif
#define MAX_SMUX_NUM 8
#define MAX_WAN_IF_INDEX  (PVC_NUM*MAX_SMUX_NUM)

#define MAX_BIND_INIT_SIZE			(5)

static char* cfg_type_waninfo_index[] = { 
	 "waninfo_index", 
	 NULL 
}; 

static void cfg_type_waninfo_send_evt(int type, int evt_id, char* buf)
{
	wan_evt_t wan_evt;
	memset(&wan_evt, 0, sizeof(wan_evt));
	char evt_type[EVT_TYPE_LENGTH];
	
	if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
	{
		printf("error path is NULL || path length is too long.\n");
		return ;
	}
	
	strncpy(wan_evt.buf, buf, EVT_BUF_LENGTH - 1);
	memset(evt_type, 0, sizeof(evt_type));
	switch (type)
	{
		case EVT_WAN_INTERNAL_TYPE :
		{
			strncpy(evt_type, EVT_WAN_INTERNAL, EVT_TYPE_LENGTH - 1);
			break;
		}
		case EVT_WAN_EXTERNAL_TYPE:
		{
			strncpy(evt_type, EVT_WAN_EXTERNAL, EVT_TYPE_LENGTH - 1);
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

static void update_wan_connect_time(char* path)
{
	int itf_index = 0;
	int pvc_index = 0;
	int entry_index = 0;
	char tmp[32];
	char nodename[32];
	char strSec[32];
	struct timespec cur_time;	
	int iSec = 0;

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(path, "Status", 0, tmp, sizeof(tmp)) <= 0) \
		|| 0 != strcmp(tmp, "up")) 
	{
		/*not exist or down*/
		return;
	}
	sscanf(path, WANINFO_ENTRY_NODE, &itf_index);
	itf_index -= 1;
	pvc_index = itf_index / MAX_WAN_ENTRY_NUMBER + 1;
	entry_index = itf_index % MAX_WAN_ENTRY_NUMBER + 1;
    clock_gettime(CLOCK_MONOTONIC, &cur_time);
	memset(nodename, 0, sizeof(nodename));
	snprintf(nodename, sizeof(nodename), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
	memset(tmp, 0, sizeof(tmp));	
	if(cfg_obj_get_object_attr(nodename, "uptime_sec", 0, tmp, sizeof(tmp)) > 0)
	{
		if(0 == strcmp(tmp, "-1"))
		{
			/*not up*/
			cfg_obj_set_object_attr(path, "connect_sec", 0, "-1");
		}
		else
		{
			iSec = cur_time.tv_sec - atoi(tmp);	
			memset(strSec, 0, sizeof(strSec));
			snprintf(strSec, sizeof(strSec), "%d", iSec);
			cfg_obj_set_object_attr(path, "connect_sec", 0,  strSec);
		}
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodename, "uptime6_sec", 0, tmp, sizeof(tmp)) > 0)
	{
		if(0 == strcmp(tmp, "-1"))
		{
			/*not up*/
			cfg_obj_set_object_attr(path, "connect6_sec", 0, "-1");
		}
		else
		{
			iSec = cur_time.tv_sec - atoi(tmp);	
			memset(strSec, 0, sizeof(strSec));
			snprintf(strSec, sizeof(strSec), "%d",iSec);
			cfg_obj_set_object_attr(path, "connect6_sec", 0, strSec);
		}
	}

	return ;
}

static int cfg_type_waninfo_entry_func_get(char* path,char* attr, char* val,int len) 
{ 	

	if (path == NULL || attr == NULL || val == NULL || len <= 0)
	{
		return -1;
	}
	update_wan_connect_time(path);

	return cfg_type_default_func_get(path,attr,val,len); 
}; 


static cfg_node_ops_t cfg_type_waninfo_entry_ops  = { 
	 .get = cfg_type_waninfo_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_waninfo_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  CFG_TYPE_FLAG_MULTIPLE | 64 , 
	 .parent = &cfg_type_waninfo, 
	 .index = cfg_type_waninfo_index, 
	 .ops = &cfg_type_waninfo_entry_ops, 
}; 




static int waninfo_common_free_buff(char** str_cycle_value,char**str_cycle_small_value)
{
	int i;
	
	for(i = 0; i < MAX_BUFF_SIZE_CNT; i++)
	{
		 if(str_cycle_value[i])
			free(str_cycle_value[i]);		
	}
		 
	for(i = 0; i < SMALL_BUFF_CNT; i++)
	{
		if(str_cycle_small_value[i])
			free(str_cycle_small_value[i]);
	}
	return 0;
}

static int waninfo_common_alloc_buff(char** str_cycle_value,char**str_cycle_small_value)
{
	int i;
	
	for(i = 0; i < MAX_BUFF_SIZE_CNT; i++)
		str_cycle_value[i] = NULL;
	
	for(i = 0; i < SMALL_BUFF_CNT; i++)
		str_cycle_small_value[i] = NULL;
	
	for(i = 0; i < MAX_BUFF_SIZE_CNT; i++)
	{
		str_cycle_value[i] = (char *)malloc(MAX_BUFF_SIZE);
		if(str_cycle_value[i] == NULL)
			goto FreeBuff;
		memset(str_cycle_value[i], 0, MAX_BUFF_SIZE);
	}
	for(i = 0; i < SMALL_BUFF_CNT; i++)
	{
		str_cycle_small_value[i] = (char *)malloc(SMALL_BUFF_SIZE);
		if(str_cycle_small_value[i] == NULL)
			goto FreeBuff;
		memset(str_cycle_small_value[i], 0, SMALL_BUFF_SIZE);
	}

	return 0;
	
FreeBuff:
	return -1;
}

#define getAttrValue(a,b,c)		cfg_obj_get_object_attr(a,b,0,c,sizeof(c))
#define setAttrValue(a,b,c)		cfg_obj_set_object_attr(a,b,0,c)

int appendCycleValue(char *cycleValueString, char *cycleValueTmp, int size)
{
	int len = 0;
	len = snprintf(cycleValueString + strlen(cycleValueString), size - strlen(cycleValueString), "%s", cycleValueTmp);
	if(len >= size - 1)
	{
	    tcdbg_printf("len:%d\n", len);
	    tcdbg_printf("cycleValueTmp:%s\n", cycleValueTmp);
	    tcdbg_printf("The length of CycleValue reaches maximum %d!!!\n", size - 1);
	}
	return 0;
}
static int waninfo_common_get_attr_cyclevalue( char* val,int len)
{
	int valid_if[MAX_WAN_INTF_NUMBER] = {0}, valid_if_num = 0, i;
	char* str_cycle_value[MAX_BUFF_SIZE_CNT];
	char* str_cycle_small_value[SMALL_BUFF_CNT];
	int pvc_index,entry_index;
	char  nodeName[34];
	char str_valid_if[128] = {0}, tmp[12] = {0};
	char str_guiinterfacename[64] = {0};
	char str_servicelist[64] = {0}, str_wanmode[8] = {0};
	char str_ifname[16] = {0};
	char str_active[8] = {0};
	char str_natenable[12] = {0};
	char str_nasname[8] = {0};
	char str_ipversion[16] = {0};
	char str_status[8] = {0};
	char str_ip[32] = {0};
	char str_netmask[32] = {0};
	char str_gateway[32] = {0};
	char str_dns[32] = {0};
	char str_secdns[32] = {0};
	char str_status6[8] = {0};
	char str_ip6[64] = {0};
	char str_gateway6[64] = {0};
	char str_dns6[64] = {0};
	char str_secdns6[64] = {0};
	char str_pd6[64] = {0}, str_orgpd6[64] = {0};
	char str_connection[30] = {0};
	char str_obtipmode[20] = {0};
	char str_vlanEnable[6] = {0};
	char str_priEnable[6] = {0};
	char str_linkmode[16] = {0};
	char str_mac[20] = {0};
	char str_ispvalue[6] = {0};
	char str_vid[6] = {0};
	char str_pri[2] = {0};
	char str_ifIdx[4] = {0};
	char str_mcvid[6] = {0};
	char str_ppp_bi[12] = {0};
	char str_cyclejump[4] = {0};
	int cycle_jump = 0;
/*	int allroute = 1;	*/
	static int cycle_index = 0;
	char str_cycle_tmp[64] = {0};
	char str_cycleidx[32] = {0};
#if defined(TCSUPPORT_CMCCV2)
	char str_dslite[64] = {0};
	char str_dslitemode[4] = {0};
	char wanInfoName[64] = {0};
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
	char wanPVCNode[MAXLEN_NODE_NAME] = {0};
	char str_atmenable[4] = {0};
	char str_ptmenable[4] = {0};
	int transmode = 0; /* ATM:1, PTM:2*/
	char str_vpi[4] = {0};
	char str_vci[4] = {0};
	char str_encap[32] = {0};
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
	char str_dhcpv6[16] = {0};
	char tmp_dhcpv6[16] = {0};
#endif
#if defined(TCSUPPORT_ANDLINK)
	char  nodeName1[32] = {0};
	char  CurAPMode[12] = {0};
#endif
#if defined(TCSUPPORT_CUC)
	int other_num = 0;
	char str_cycle_ifnamealis[MAX_BUFF_SIZE] = {0};
	char str_ifnamealis[64] = {0};
	int first_other_idx = 0;
	char tmp_node[32] = {0};
	char first_other_alis[64] = {0};
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	char str_pdorign[32] = {0};
	char str_pdenable[8] = {0};
#endif

	if (waninfo_common_alloc_buff(str_cycle_value,str_cycle_small_value) < 0)
		goto freebuff;
	
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);
	
	for(i = 0; i < valid_if_num; i++){
		
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d,", valid_if[i]);
		strcat(str_valid_if, tmp);	
		pvc_index = valid_if[i] / MAX_WAN_ENTRY_NUMBER + 1;
		entry_index = valid_if[i] % MAX_WAN_ENTRY_NUMBER + 1;
		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index,entry_index);
#if defined(TCSUPPORT_CT_DSL_EX)
		memset(wanPVCNode, 0, sizeof(wanPVCNode));
		snprintf(wanPVCNode, sizeof(wanPVCNode), WAN_PVC_NODE, pvc_index);

		if ((getAttrValue(wanPVCNode, "ATMEnable", str_atmenable) >= 0 )
			&& 0 == strcmp("Yes", str_atmenable) )
				transmode = 1;

		if ((getAttrValue(wanPVCNode, "PTMEnable", str_ptmenable) >= 0 )
			&& 0 == strcmp("Yes", str_ptmenable) )
				transmode = 2;

		if ( 0 == transmode )
			transmode = 1; /* default for ATM mode.*/
		
		if(getAttrValue(wanPVCNode, "VPI", str_vpi) < 0 )
		{
			strncpy(str_vpi, "0", sizeof(str_vpi) - 1);
		}
		if(getAttrValue(wanPVCNode, "VCI", str_vci) < 0 )
		{
			strncpy(str_vci, "35", sizeof(str_vci) - 1);
		}
		if(getAttrValue(wanPVCNode, "ENCAP", str_encap) < 0 )
		{
			strncpy(str_encap, "N/A", sizeof(str_encap) - 1);
		}
#endif

		if(getAttrValue(nodeName, "WanMode", str_wanmode) <= 0 )
			strncpy(str_wanmode, "Route", sizeof(str_wanmode) - 1);
					
		if(getAttrValue(nodeName, "ServiceList", str_servicelist) <= 0 )
			strncpy(str_servicelist, "INTERNET", sizeof(str_servicelist) - 1);
		
#if defined(TCSUPPORT_CUC)
		if( !strcmp(str_servicelist, "OTHER") )
		{
			other_num++;
			if( 1 == other_num )
				first_other_idx = i;
		}
#endif

		if(getAttrValue(nodeName, "IFName", str_ifname) <= 0 )
			strncpy(str_ifname, "nasx_x", sizeof(str_ifname) - 1);
		
		if(getAttrValue(nodeName, "Active", str_active) <= 0)
			strncpy(str_active, "N/A", sizeof(str_active) - 1);
	
		if(getAttrValue(nodeName, "NATENABLE", str_natenable) <= 0 )
			strncpy(str_natenable, "N/A", sizeof(str_natenable) - 1);
		
		if(getAttrValue(nodeName, "NASName", str_nasname) <= 0 )
			strncpy(str_nasname, "nasx_x", sizeof(str_nasname) - 1);
			
		if(getAttrValue(nodeName, "IPVERSION", str_ipversion) <= 0 )
			strncpy(str_ipversion, "N/A", sizeof(str_ipversion) - 1);

#if defined(TCSUPPORT_CMCCV2)
		if(getAttrValue(nodeName, "DsliteMode", str_dslitemode) > 0){
			snprintf(wanInfoName, sizeof(wanInfoName), WANINFO_ENTRY_NODE, entry_index);
			if(strcmp(str_dslitemode, "0") == 0){
				if(getAttrValue(wanInfoName, "DsliteName", str_dslite) <= 0){
					strncpy(str_dslite, "N/A", sizeof(str_dslite) - 1);
				}
			}else if(strcmp(str_dslitemode, "1") == 0){
				if(getAttrValue(nodeName, "DsliteAddr", str_dslite) <= 0){
					strncpy(str_dslite, "N/A", sizeof(str_dslite) - 1);
				}
			}
		}
		else{
			strncpy(str_dslite, "N/A", sizeof(str_dslite) - 1);
		}
#endif

/*		if ( !strcmp(str_active, "Yes") && strcmp(str_wanmode, "Route"))
			allroute= 0;
*/
#if defined(TCSUPPORT_CT_PPPOEPROXY)
		if (getAttrValue(nodeName, "pppProxyBiMode", str_ppp_bi) <= 0	|| 0 == str_ppp_bi[0] )
			strncpy(str_ppp_bi, "0", sizeof(str_ppp_bi) - 1);
#else
		if (getAttrValue(nodeName, "BridgeInterface", str_ppp_bi) <= 0	|| 0 == str_ppp_bi[0] )
			strncpy(str_ppp_bi, "No", sizeof(str_ppp_bi) - 1);
#endif

		if( getAttrValue(nodeName, "IFIdx", str_ifIdx) <= 0 )
			snprintf(str_ifIdx, sizeof(str_ifIdx), "%d", i+1);

		if( getAttrValue(nodeName, "CONNECTION", str_connection) <= 0 )
			strncpy(str_connection, "N/A", sizeof(str_connection) - 1);

		if( getAttrValue(nodeName, "LinkMode", str_linkmode) <= 0 )
			strncpy(str_linkmode, "N/A", sizeof(str_linkmode) - 1);

		if (getAttrValue(nodeName, "ISP", str_ispvalue) <= 0 )
			strncpy(str_ispvalue, "0", sizeof(str_ispvalue) - 1);		

		if (getAttrValue(nodeName, "dot1q", str_vlanEnable) <= 0 )
			strncpy(str_vlanEnable, "N/A", sizeof(str_vlanEnable) - 1);

		if (getAttrValue(nodeName, "dot1p", str_priEnable) <=  0 )
			strncpy(str_priEnable, "N/A", sizeof(str_priEnable) - 1);
		
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
		getAttrValue(nodeName, "DHCPv6", tmp_dhcpv6);
#endif		
		strncpy(str_vid, "-", sizeof(str_vid) - 1);
		if( 0 == strcmp(str_vlanEnable, "Yes") )
		{
			if( getAttrValue(nodeName, "VLANID", str_vid) <= 0 )
				strncpy(str_vid, "-", sizeof(str_vid) - 1);
		}

		strncpy(str_pri, "-", sizeof(str_pri) - 1);
		if( 0 == strcmp(str_priEnable, "Yes") )
		{
			if( getAttrValue(nodeName, "dot1pData", str_pri) <= 0 )
				strncpy(str_pri, "-", sizeof(str_pri) - 1);
		}
			
		strncpy(str_mcvid, "-", sizeof(str_mcvid) - 1);
		if( getAttrValue(nodeName, "MulticastVID", str_mcvid) <= 0 )	
			strncpy(str_mcvid, "-", sizeof(str_mcvid) - 1);

#if defined(TCSUPPORT_CT_JOYME4)
		if(getAttrValue(nodeName, "PDOrigin", str_pdorign) <= 0 )
		{
			if (getAttrValue(nodeName, "DHCPv6PD", str_pdenable) > 0 
				&& 0 == strcmp(str_pdenable, "Yes"))
				strncpy(str_pdorign, "PrefixDelegation", sizeof(str_pdorign) - 1);
			else
				strncpy(str_pdorign, "N/A", sizeof(str_pdorign) - 1);
		}
#endif

		snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, valid_if[i] + 1);

		if (cfg_obj_query_object(nodeName,NULL,NULL) <= 0)
			cfg_obj_create_object(nodeName);
		
		if( 0 == strcmp(str_wanmode, "Route") )
		{
#if defined(TCSUPPORT_CT_DSL_EX)
			if ( 1 == transmode )
			{
				sprintf(str_guiinterfacename, "%s_%s_R_%s_%s", str_ifIdx, str_servicelist, str_vpi, str_vci);
			}
			else
			{
#endif
				if( 0 == strcmp(str_vlanEnable, "Yes") )
				{
#if defined(TCSUPPORT_CUC)
					if( !strcmp(str_servicelist, "OTHER") )
					{
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s%d_R_VID_%s", str_ifIdx, str_servicelist, other_num, str_vid);
						if( 1 == other_num )
							snprintf(first_other_alis, sizeof(first_other_alis), "%s_%s%d_R_VID_%s", str_ifIdx, str_servicelist, other_num, str_vid);
					}
					else
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s_R_VID_%s", str_ifIdx, str_servicelist, str_vid);
#endif
					snprintf(str_guiinterfacename, sizeof(str_guiinterfacename), "%s_%s_R_VID_%s", str_ifIdx, str_servicelist, str_vid);
				}
				else
				{
#if defined(TCSUPPORT_CUC)
					if( !strcmp(str_servicelist, "OTHER") )
					{
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s%d_R_VID_", str_ifIdx, str_servicelist, other_num);
						if( 1 == other_num )
							snprintf(first_other_alis, sizeof(first_other_alis), "%s_%s%d_R_VID_", str_ifIdx, str_servicelist, other_num);
					}
					else
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s_R_VID_", str_ifIdx, str_servicelist);
#endif
					snprintf(str_guiinterfacename, sizeof(str_guiinterfacename), "%s_%s_R_VID_", str_ifIdx, str_servicelist);
				}
#if defined(TCSUPPORT_CT_DSL_EX)
			}
#endif
			if( 0 == strcmp(str_linkmode, "linkIP") )
			{
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
				if( strstr(str_ipversion, "IPv6") )
				{
					if( !strcmp(tmp_dhcpv6, "No") )
						strcpy(str_dhcpv6, "SLAAC");
					else if( !strcmp(tmp_dhcpv6, "Yes") )
						strcpy(str_dhcpv6, "DHCP");
					else if( !strcmp(tmp_dhcpv6, "N/A") )
						strcpy(str_dhcpv6, "Static");
					else
						strcpy(str_dhcpv6, "N/A");
				}
				else
					strcpy(str_dhcpv6, "N/A");
#endif				
				strncpy(str_connection, "Connect_Keep_Alive", sizeof(str_connection) - 1);
				if( 0 == strcmp(str_ispvalue, "0") )
					strncpy(str_obtipmode, "DHCP", sizeof(str_obtipmode) - 1);
				else
					strncpy(str_obtipmode, "Static", sizeof(str_obtipmode) - 1);
			}
			else
			{
				strncpy(str_obtipmode, "PPPoE", sizeof(str_obtipmode) - 1);
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
				if( strstr(str_ipversion, "IPv6") )
					strcpy(str_dhcpv6, "PPPoE");
				else
					strcpy(str_dhcpv6, "N/A");
#endif
			}
		}
		else
		{
			strncpy(str_connection, "Connect_Keep_Alive", sizeof(str_connection) - 1);
#if defined(TCSUPPORT_CT_DSL_EX)
			if ( 1 == transmode )
			{
				sprintf(str_guiinterfacename, "%s_%s_B_%s_%s",str_ifIdx, str_servicelist, str_vpi, str_vci);
			}
			else
			{
#endif
				if( 0 == strcmp(str_vlanEnable, "Yes") )
				{
#if defined(TCSUPPORT_CUC)
					if( !strcmp(str_servicelist, "OTHER") )
					{
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s%d_B_VID_%s", str_ifIdx, str_servicelist, other_num, str_vid);
						if( 1 == other_num )
							snprintf(first_other_alis, sizeof(first_other_alis), "%s_%s%d_B_VID_%s", str_ifIdx, str_servicelist, other_num, str_vid);
					}
					else
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s_B_VID_%s", str_ifIdx, str_servicelist, str_vid);
#endif
					snprintf(str_guiinterfacename, sizeof(str_guiinterfacename), "%s_%s_B_VID_%s", str_ifIdx, str_servicelist, str_vid);
				}
				else
				{
#if defined(TCSUPPORT_CUC)
					if( !strcmp(str_servicelist, "OTHER") )
					{
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s%d_B_VID_", str_ifIdx, str_servicelist, other_num);
						if( 1 == other_num )
							snprintf(first_other_alis, sizeof(first_other_alis), "%s_%s%d_B_VID_", str_ifIdx, str_servicelist, other_num);
					}
					else
						snprintf(str_ifnamealis, sizeof(str_ifnamealis), "%s_%s_B_VID_", str_ifIdx, str_servicelist);
#endif
					snprintf(str_guiinterfacename, sizeof(str_guiinterfacename), "%s_%s_B_VID_", str_ifIdx, str_servicelist);
				}
#if defined(TCSUPPORT_CT_DSL_EX)
			}
#endif
#if defined(TCSUPPORT_CUC)
			if( 0 == strcmp(str_linkmode, "linkIP") )
			{
				if( 0 == strcmp(str_ispvalue, "0") )
					strcpy(str_obtipmode, "DHCP");
				else
					strcpy(str_obtipmode, "Static");
			}
			else
				strcpy(str_obtipmode, "PPPoE_Bridge");		
#else
			str_obtipmode[0] = '\0';
#endif
		}
#if defined(TCSUPPORT_CUC)		
		if( other_num > 1 && !strcmp(str_servicelist, "OTHER"))
		{
			setAttrValue(nodeName, "GUIInterfaceName", str_ifnamealis);
			if( 2 == other_num )
			{
				snprintf(tmp_node, sizeof(tmp_node), WANINFO_ENTRY_NODE, valid_if[first_other_idx] + 1);
				setAttrValue(tmp_node, "GUIInterfaceName", first_other_alis);
			}
		}
		else
#endif
		setAttrValue(nodeName, "GUIInterfaceName", str_guiinterfacename);

#if defined(TCSUPPORT_ANDLINK)
		memset(nodeName1, 0, sizeof(nodeName1));
		snprintf(nodeName1, sizeof(nodeName1), APWANINFO_ENTRY_NODE);
		
		getAttrValue("root.waninfo.common", "CurAPMode", CurAPMode);
		if((strcmp(CurAPMode, "Bridge") == 0) || (strcmp(CurAPMode, "APClient") == 0))
		{
			if(getAttrValue(nodeName1, "Status", str_status) <= 0 )
				strncpy(str_status, "N/A", sizeof(str_status) - 1);
			
			if(getAttrValue(nodeName1, "IP", str_ip) <= 0 )
				strncpy(str_ip, "N/A", sizeof(str_ip) - 1);
							
			if(getAttrValue(nodeName1, "NetMask", str_netmask) <= 0 )
				strncpy(str_netmask, "N/A", sizeof(str_netmask) - 1);
							
			if(getAttrValue(nodeName1, "GateWay", str_gateway) <= 0 )
				strncpy(str_gateway, "N/A", sizeof(str_gateway) - 1);
					
			if(getAttrValue(nodeName1, "DNS", str_dns) <= 0 )
				strncpy(str_dns, "N/A", sizeof(str_dns) - 1);
							
			if(getAttrValue(nodeName1, "SecDNS", str_secdns) <= 0 )
				strncpy(str_secdns, "N/A", sizeof(str_secdns) - 1);
		}
		else
		{
#endif
		if(getAttrValue(nodeName, "Status", str_status) <= 0 )
			strncpy(str_status, "N/A", sizeof(str_status) - 1);
		
		if(getAttrValue(nodeName, "IP", str_ip) <= 0 )
			strncpy(str_ip, "N/A", sizeof(str_ip) - 1);
						
		if(getAttrValue(nodeName, "NetMask", str_netmask) <= 0 )
			strncpy(str_netmask, "N/A", sizeof(str_netmask) - 1);
						
		if(getAttrValue(nodeName, "GateWay", str_gateway) <= 0 )
			strncpy(str_gateway, "N/A", sizeof(str_gateway) - 1);
				
		if(getAttrValue(nodeName, "DNS", str_dns) <= 0 )
			strncpy(str_dns, "N/A", sizeof(str_dns) - 1);
						
		if(getAttrValue(nodeName, "SecDNS", str_secdns) <= 0 )
			strncpy(str_secdns, "N/A", sizeof(str_secdns) - 1);
#if defined(TCSUPPORT_ANDLINK)
		}
#endif

					
		if(getAttrValue(nodeName, "Status6", str_status6) <= 0 )
			strncpy(str_status6, "N/A", sizeof(str_status6) - 1);
			
		if (strcmp(str_wanmode, "Route") != 0)
			strncpy(str_status6, str_status, sizeof(str_status6) - 1);

		if(getAttrValue(nodeName, "IP6", str_ip6) <= 0 )
			strncpy(str_ip6, "N/A", sizeof(str_ip6) - 1);
					
		if(getAttrValue(nodeName, "GateWay6", str_gateway6) <= 0 )
			strncpy(str_gateway6, "N/A", sizeof(str_gateway6) - 1);
				
		if(getAttrValue(nodeName, "DNS6", str_dns6) <= 0 )
			strncpy(str_dns6, "N/A", sizeof(str_dns6) - 1);
		
		if(getAttrValue(nodeName, "SecDNS6", str_secdns6) <= 0 )
			strncpy(str_secdns6, "N/A", sizeof(str_secdns6) - 1);
						
		if(getAttrValue(nodeName, "PD6", str_pd6) <= 0 )
			strncpy(str_pd6, "N/A", sizeof(str_pd6) - 1);

		if (getAttrValue(nodeName, "OrgPD6", str_orgpd6) > 0 && str_orgpd6[0])
			strncpy(str_pd6, str_orgpd6, sizeof(str_pd6) - 1);

		if(getAttrValue(nodeName, "hwaddr", str_mac) <= 0 )
			strncpy(str_mac, "N/A", sizeof(str_mac) - 1);

#if defined(TCSUPPORT_CUC)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ifnamealis);
		appendCycleValue(str_cycle_ifnamealis, str_cycle_tmp, MAX_BUFF_SIZE);
		if( other_num > 1 )
		{
			snprintf(str_cycle_value[13], MAX_BUFF_SIZE, "%s", str_cycle_ifnamealis);
		}
		else
#endif
		{
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_guiinterfacename);
		appendCycleValue(str_cycle_value[13], str_cycle_tmp, MAX_BUFF_SIZE);
		}	
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ifname);
		appendCycleValue(str_cycle_small_value[4], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_active);
		appendCycleValue(str_cycle_small_value[5], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_natenable);
		appendCycleValue(str_cycle_small_value[6], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_nasname);
		appendCycleValue(str_cycle_small_value[7], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ipversion);
		appendCycleValue(str_cycle_value[1], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_status);
		appendCycleValue(str_cycle_small_value[0], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ip);
		appendCycleValue(str_cycle_value[2], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_netmask);
		appendCycleValue(str_cycle_value[3], str_cycle_tmp, MAX_BUFF_SIZE);
#if defined(TCSUPPORT_CT_DSL_EX)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_encap);
		appendCycleValue(str_cycle_value[4], str_cycle_tmp, MAX_BUFF_SIZE);
#else
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_obtipmode);
		appendCycleValue(str_cycle_value[4], str_cycle_tmp, MAX_BUFF_SIZE);
#endif
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_gateway);
		appendCycleValue(str_cycle_value[5], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dns);
		appendCycleValue(str_cycle_value[6], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_secdns);
		appendCycleValue(str_cycle_value[7], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_status6);
		appendCycleValue(str_cycle_small_value[1], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ip6);
		appendCycleValue(str_cycle_value[8], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_gateway6);
		appendCycleValue(str_cycle_value[9], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dns6);
		appendCycleValue(str_cycle_value[10], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_secdns6);
		appendCycleValue(str_cycle_value[11], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_pd6);
		appendCycleValue(str_cycle_value[12], str_cycle_tmp, MAX_BUFF_SIZE);
#if defined(TCSUPPORT_CT_DSL_EX)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_vpi);
		appendCycleValue(str_cycle_small_value[2], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_vci);
		appendCycleValue(str_cycle_small_value[3], str_cycle_tmp, SMALL_BUFF_SIZE);
#else
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ifIdx);
		appendCycleValue(str_cycle_small_value[2], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", "");
		appendCycleValue(str_cycle_small_value[3], str_cycle_tmp, SMALL_BUFF_SIZE);
#endif
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_connection);
		appendCycleValue(str_cycle_value[14], str_cycle_tmp, MAX_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s/%s,", str_vid, str_pri);
		appendCycleValue(str_cycle_small_value[8], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_mac);
		appendCycleValue(str_cycle_small_value[9], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_mcvid);
		appendCycleValue(str_cycle_small_value[10], str_cycle_tmp, SMALL_BUFF_SIZE);
#if defined(TCSUPPORT_CT_DSL_EX)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_obtipmode);
		appendCycleValue(str_cycle_small_value[11], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ifIdx);
		appendCycleValue(str_cycle_small_value[12], str_cycle_tmp, SMALL_BUFF_SIZE);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%d,", transmode);
		appendCycleValue(str_cycle_small_value[13], str_cycle_tmp, SMALL_BUFF_SIZE);
#else
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_ppp_bi);
		appendCycleValue(str_cycle_small_value[11], str_cycle_tmp, SMALL_BUFF_SIZE);
#endif
#if defined(TCSUPPORT_CMCCV2)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dslite);
		appendCycleValue(str_cycle_value[15], str_cycle_tmp, MAX_BUFF_SIZE);
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dhcpv6);
		strcat(str_cycle_value[16], str_cycle_tmp);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_pdorign);
		appendCycleValue(str_cycle_value[15], str_cycle_tmp, MAX_BUFF_SIZE);
#endif	
	}

	snprintf(nodeName, sizeof(nodeName),WANINFO_COMMON_NODE);	
	if (getAttrValue(nodeName, "CycleIdx", str_cycleidx) > 0 )
		cycle_index = atoi(str_cycleidx);
	else
		cycle_index  = 0;
	
	if (getAttrValue(nodeName, "CycleJump", str_cyclejump) > 0 )
	{
		if ((cycle_jump = atoi(str_cyclejump)) > 0)
			cycle_index += cycle_jump;
		setAttrValue(nodeName, "CycleJump", "0");
	}

	if( cycle_index >= CYCLEVALUE_MAXCNT )
		cycle_index = 0;

	
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_NP_CMCC)
	/*setAttrValue(top, nodeName, "CycleValue_0", str_cycle_value[13]);*/
	setAttrValue(nodeName, "CycleValue_1", str_cycle_small_value[4]);
	setAttrValue(nodeName, "CycleValue_2", str_valid_if);
	setAttrValue(nodeName, "CycleValue_3", str_cycle_small_value[5]);
	setAttrValue(nodeName, "CycleValue_4", str_cycle_small_value[6]);
	setAttrValue(nodeName, "CycleValue_5", str_cycle_small_value[7]);
	setAttrValue(nodeName, "CycleValue_6", str_cycle_value[1]);
	setAttrValue(nodeName, "CycleValue_7", str_cycle_small_value[0]);
	setAttrValue(nodeName, "CycleValue_8", str_cycle_value[2]);
	setAttrValue(nodeName, "CycleValue_9", str_cycle_value[4]);
	setAttrValue(nodeName, "CycleValue_10", str_cycle_value[5]);
	setAttrValue(nodeName, "CycleValue_11", str_cycle_value[6]);
	setAttrValue(nodeName, "CycleValue_12", str_cycle_value[7]);
	setAttrValue(nodeName, "CycleValue_13", str_cycle_small_value[1]);
	setAttrValue(nodeName, "CycleValue_14", str_cycle_value[8]);
	setAttrValue(nodeName, "CycleValue_15", str_cycle_value[9]);
	setAttrValue(nodeName, "CycleValue_16", str_cycle_value[10]);
	setAttrValue(nodeName, "CycleValue_17", str_cycle_value[11]);
	setAttrValue(nodeName, "CycleValue_18", str_cycle_value[12]);
	setAttrValue(nodeName, "CycleValue_19", str_cycle_value[3]);
	setAttrValue(nodeName, "CycleValue_20", str_cycle_small_value[2]);
	setAttrValue(nodeName, "CycleValue_21", str_cycle_small_value[3]);
	setAttrValue(nodeName, "CycleValue_22", str_cycle_value[14]);
	setAttrValue(nodeName, "CycleValue_23", str_cycle_small_value[8]);
	setAttrValue(nodeName, "CycleValue_24", str_cycle_small_value[9]);
	setAttrValue(nodeName, "CycleValue_25", str_cycle_small_value[10]);
	setAttrValue(nodeName, "CycleValue_26", str_cycle_small_value[11]);
	setAttrValue(nodeName, "CycleValue_27", str_cycle_value[15]);
	setAttrValue(nodeName, "CycleValue_28", str_cycle_value[16]);
#endif

	if(cycle_index ==  0)
	{
		strncpy(str_cycle_value[0], str_cycle_value[13], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 1)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[4], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 2)
	{
		strncpy(str_cycle_value[0], str_valid_if, MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 3)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[5], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 4)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[6], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 5)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[7], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 6)
	{
		strncpy(str_cycle_value[0], str_cycle_value[1], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 7)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[0], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 8)
	{
		strncpy(str_cycle_value[0], str_cycle_value[2], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 9)
	{
		strncpy(str_cycle_value[0], str_cycle_value[4], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 10)
	{
		strncpy(str_cycle_value[0], str_cycle_value[5], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 11)
	{
		strncpy(str_cycle_value[0], str_cycle_value[6], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 12)
	{
		strncpy(str_cycle_value[0], str_cycle_value[7], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 13)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[1], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 14)
	{
		strncpy(str_cycle_value[0], str_cycle_value[8], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 15)
	{
		strncpy(str_cycle_value[0], str_cycle_value[9], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 16)
	{
		strncpy(str_cycle_value[0], str_cycle_value[10], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 17)
	{
		strncpy(str_cycle_value[0], str_cycle_value[11], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 18)
	{
		strncpy(str_cycle_value[0], str_cycle_value[12], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 19)
	{
		strncpy(str_cycle_value[0], str_cycle_value[3], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 20)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[2], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 21)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[3], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 22)
	{
		strncpy(str_cycle_value[0], str_cycle_value[14], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 23)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[8], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 24)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[9], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 25)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[10], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 26)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[11], MAX_BUFF_SIZE - 1);
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else if(cycle_index == 27)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[12], MAX_BUFF_SIZE - 1);
	}
	else if(cycle_index == 28)
	{
		strncpy(str_cycle_value[0], str_cycle_small_value[13], MAX_BUFF_SIZE - 1);
	}
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	else if(cycle_index == 29)
	{
		strncpy(str_cycle_value[0], str_cycle_value[15], MAX_BUFF_SIZE - 1);
	}
#endif

	else
	{
		strncpy(str_cycle_value[0],"", MAX_BUFF_SIZE - 1);
	}
	cycle_index++;

	snprintf(str_cycleidx, sizeof(str_cycleidx), "%d",cycle_index);

	setAttrValue(nodeName, "CycleIdx", str_cycleidx);
	
	strncpy(val,str_cycle_value[0],len);

	val[len-1] = 0;
	
freebuff:
	waninfo_common_free_buff(str_cycle_value,str_cycle_small_value);

	return strlen(val);	
}

static int waninfo_common_get_attr_validifindex(char* val,int len)
{
	int valid_if[MAX_WAN_INTF_NUMBER] = {0}, valid_if_num = 0, i;
	char tmp[8],str_valid_if[256];
	
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);

	str_valid_if[0] = 0;

	for(i = 0; i< valid_if_num; i++)
	{
		snprintf(tmp, sizeof(tmp), "%d,", valid_if[i]);
		strcat(str_valid_if, tmp);
	}

	strncpy(val,str_valid_if,len);
	
	val[len-1] = 0;
	
	return strlen(val);
}

static int waninfo_common_get_attr_validifnum(char* val,int len)
{
	int valid_if[MAX_WAN_INTF_NUMBER] = {0}, valid_if_num = 0;
	char tmp[8];
	
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);

	snprintf(tmp, sizeof(tmp), "%d",valid_if_num);

	strncpy(val,tmp,len);

	val[len-1] = 0;
	
	return strlen(val);
}

static int waninfo_common_get_attr_allroute(char* val,int len)
{
	char str_active[8],str_mode[16];
	int valid_if[MAX_WAN_INTF_NUMBER], valid_if_num = 0, i;
	char node[32];
	int pvc,entry;
	
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);

	val[0] = '1';
	
	for(i=0; i< valid_if_num; i++)
	{
		pvc = (valid_if[i] & 0xff) / MAX_WAN_ENTRY_NUMBER + 1;
		entry = (valid_if[i] & 0xff) % MAX_WAN_ENTRY_NUMBER + 1;	

		snprintf(node, sizeof(node), WAN_PVC_ENTRY_NODE, pvc,entry);
		
		if (cfg_obj_get_object_attr(node,"Active",0,str_active,sizeof(str_active)) <=0)
			continue;
		
		if (cfg_obj_get_object_attr(node,"WanMode",0,str_mode,sizeof(str_mode)) <=0)
			continue;
		
	
	if (strcmp(str_active, "Yes") == 0 && strcmp(str_mode, "Route"))
		{
			val[0] = '0';
			break;
		}
	}

	val[1] = 0;
	return 2;	
}

static int waninfo_common_get_attr_dmz_cyclevalue(char* val, int len)
{
	int valid_if_num = 0;
	int valid_if[MAX_WAN_INTF_NUMBER];
	int i = 0;
	char path[32];
	char str_dmz_active[8];
	char str_dmz_ip[32];
	int cycle_index = 0;
	char str_cycle_tmp[32];
	char str_cycleidx[32];
	char* str_cycle_small_value = NULL;
	char* str_cycle_value = NULL;
	
	str_cycle_small_value = (char *)malloc(SMALL_BUFF_SIZE);
	if(NULL == str_cycle_small_value)
	{
		goto free_buff;
	}
	else
	{
		memset(str_cycle_small_value, 0, SMALL_BUFF_SIZE);
	}
	
	str_cycle_value = (char *)malloc(MAX_BUFF_SIZE);
	if(NULL == str_cycle_value)
	{
		goto free_buff;
	}
	else
	{
		memset(str_cycle_value, 0, MAX_BUFF_SIZE);
	}

	
	memset(valid_if, 0, sizeof(valid_if));
	memset(str_cycle_tmp, 0, sizeof(str_cycle_tmp));
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);
	for(i = 0; i < valid_if_num; i++)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), DMZ_ENTRY_NODE, (valid_if[i] + 1) & 0xff);
		memset(str_dmz_active, 0, sizeof(str_dmz_active));
		if (cfg_obj_get_object_attr(path, "Active", 0, str_dmz_active, sizeof(str_dmz_active)) <=0)
		{
			snprintf(str_dmz_active, sizeof(str_dmz_active), "N/A");
		}
		
		memset(str_dmz_ip, 0, sizeof(str_dmz_ip));
		if (cfg_obj_get_object_attr(path, "DMZ_IP", 0, str_dmz_ip, sizeof(str_dmz_ip)) <=0)
		{
			snprintf(str_dmz_ip, sizeof(str_dmz_ip), "N/A");
		}
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dmz_active);
		strcat(str_cycle_small_value, str_cycle_tmp);
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_dmz_ip);
		strcat(str_cycle_value, str_cycle_tmp);
	}
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WANINFO_COMMON_NODE);
	if (cfg_obj_get_object_attr(path, "CycleIdx", 0, str_cycleidx, sizeof(str_cycleidx)) <= 0)
	{
		cycle_index  = 0;
	}
	else
	{
		cycle_index = atoi(str_cycleidx);
	}
	
	if(cycle_index >= 2)
	{
		cycle_index = 0;
	}

	
	if(cycle_index == 0)
	{
		strncpy(val, str_cycle_small_value, len);
	}
	else if(cycle_index == 1)
	{
		strncpy(val, str_cycle_value, len);
	}
	
	cycle_index++;	
	snprintf(str_cycleidx, sizeof(str_cycleidx), "%d", cycle_index);
	cfg_obj_set_object_attr(path, "CycleIdx", 0, str_cycleidx);

	
	val[len-1] = 0;
	
free_buff:
	if(NULL != str_cycle_small_value)
	{
		free(str_cycle_small_value);
		str_cycle_small_value = NULL;
	}
	
	if(NULL != str_cycle_value)
	{
		free(str_cycle_value);
		str_cycle_value = NULL;
	}	
		
	return strlen(val);
}

static int waninfo_common_get_attr_auto_dmz_flag(char* val, int len)
{
#if defined(TCSUPPORT_AUTO_DMZ)
	int valid_if_num = 0;
	int valid_if[MAX_WAN_INTF_NUMBER];
	int i = 0;
	char path[32];
	char* str_cycle_small_value = NULL;
	char dmz_flag[2];
	char str_cycle_tmp[32];
	
	str_cycle_small_value = (char *)malloc(SMALL_BUFF_SIZE);
	if(NULL == str_cycle_small_value)
	{
		return strlen(val);
	}
	else
	{
		memset(str_cycle_small_value, 0, SMALL_BUFF_SIZE);
	}
	
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);
	for(i = 0; i < valid_if_num; i++)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), DMZ_ENTRY_NODE, (valid_if[i]+1) & 0xff);
		memset(dmz_flag, 0, sizeof(dmz_flag));
		if (cfg_obj_get_object_attr(path, "AutoDMZ_Flag", 0, dmz_flag, sizeof(dmz_flag)) <=0)
		{
			snprintf(dmz_flag, sizeof(dmz_flag), "0");
		}
		
		memset(str_cycle_tmp, 0, sizeof(str_cycle_tmp));
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", dmz_flag);
		strcat(str_cycle_small_value, str_cycle_tmp);
	}
	
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WANINFO_COMMON_NODE);
	cfg_obj_set_object_attr(path, "AutoDMZ_Flag", 0, str_cycle_small_value);
	
	strncpy(val, str_cycle_small_value, len);
	val[len-1] = 0;

	if(NULL != str_cycle_small_value)
	{
		free(str_cycle_small_value);
		str_cycle_small_value = NULL;
	}
#endif

	return strlen(val);
}

#define  MAX_VIRSERVER_NUM  (10)
static int waninfo_common_get_attr_vir_srv_cyclevalue(char* val, int len)
{
	char path[64];
	int cycle_index = 0;
	char str_cycle_tmp[32];
	char str_cycleidx[32];
	int i = 0;
	int vs_entry_index = 0;
	char vs_attrName[32];
	char str_vs_unit[32];
	char* str_cycle_value = NULL;
	char str_del[32] = {0};
	
	str_cycle_value = (char *)malloc(MAX_BUFF_SIZE);
	if(NULL == str_cycle_value)
	{
		return strlen(val);
	}
	memset(str_cycle_value, 0, MAX_BUFF_SIZE);

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WANINFO_COMMON_NODE);
	if (cfg_obj_get_object_attr(path, "CycleIdx", 0, str_cycleidx, sizeof(str_cycleidx)) <=0)
	{
		cycle_index  = 0;
	}
	else
	{
		cycle_index = atoi(str_cycleidx);
	}
	
	if(cycle_index >= 7)
	{
		cycle_index  = 0;
	}
	
#if !defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CUC)
	memset(path, 0, sizeof(path));
	memset(str_cycle_tmp, 0, sizeof(str_cycle_tmp));
	snprintf(path, sizeof(path), WEBCURSET_ENTRY_NODE);
#if defined(TCSUPPORT_VIR_SERVER)
	if (cfg_obj_get_object_attr(path, "nat_ifidx", 0, str_cycle_tmp, sizeof(str_cycle_tmp)) <=0)
#else
	if (cfg_obj_get_object_attr(path, "nat_pvc", 0, str_cycle_tmp, sizeof(str_cycle_tmp)) <=0)
#endif
	{
		strncpy(str_cycle_tmp, "0", sizeof(str_cycle_tmp) - 1);
	}
	vs_entry_index = atoi(str_cycle_tmp);
#endif
	
	for(i = 0; i < MAX_VIRSERVER_NUM; i++)
	{
		memset(path, 0, sizeof(path));
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
		snprintf(path, sizeof(path), VIRSERVER_ENTRY_NODE, i + 1);
#else
		snprintf(path, sizeof(path), VIRSERVER_ENTRY_ENTRY_NODE, (vs_entry_index + 1), (i+1));
#endif
		memset(vs_attrName, 0, sizeof(vs_attrName));
		if(cycle_index == 0)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "Active");
		}
		else if(cycle_index == 1)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "Protocal");
		}
		else if(cycle_index == 2)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "SrcIP");
		}
		else if(cycle_index == 3)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "STARTPORT");
		}
		else if(cycle_index == 4)
		{
#if (defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)) || defined(TCSUPPORT_CMCCV2)
			snprintf(vs_attrName, sizeof(vs_attrName), "INTPORT");
#else
			snprintf(vs_attrName, sizeof(vs_attrName), "ENDPORT");
#endif
		}
		else if(cycle_index == 5)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "LOCALIP");
		}
		else if(cycle_index == 6)
		{
			snprintf(vs_attrName, sizeof(vs_attrName), "MapName");
		}
		
		memset(str_vs_unit, 0, sizeof(str_vs_unit));
		if (cfg_obj_get_object_attr(path, vs_attrName, 0, str_vs_unit, sizeof(str_vs_unit)) <=0)
		{
			snprintf(str_vs_unit, sizeof(str_vs_unit), "N/A");
		}
#if defined(TCSUPPORT_CT_JOYME2)
		memset(str_del, 0, sizeof(str_del));
		if (cfg_obj_get_object_attr(path, "vDelEntry", 0, str_del, sizeof(str_del)) > 0)
		{
			memset(str_vs_unit, 0, sizeof(str_vs_unit));
			snprintf(str_vs_unit, sizeof(str_vs_unit), "N/A");
		}
#endif
		snprintf(str_cycle_tmp, sizeof(str_cycle_tmp), "%s,", str_vs_unit);
		strcat(str_cycle_value, str_cycle_tmp);
	}

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WANINFO_COMMON_NODE);
	cycle_index++;	
	snprintf(str_cycleidx, sizeof(str_cycleidx), "%d", cycle_index);
	cfg_obj_set_object_attr(path, "CycleIdx", 0, str_cycleidx);
	cfg_obj_set_object_attr(path, "VirServerCycleValue", 0, str_cycle_value);
	
	strncpy(val, str_cycle_value, len);
	val[len-1] = 0;

	if(NULL != str_cycle_value)
	{
		free(str_cycle_value);
		str_cycle_value = NULL;
	}

	return strlen(val);
}

int get_dslite_stateviacfg(char *nodeName)
{
	char dslite_enable[8] = {0};
	char serviceList[64] = {0};
	char isp[5] = {0};

	if (cfg_obj_get_object_attr(nodeName, "DsliteEnable", 0, dslite_enable, sizeof(dslite_enable)) <=0)
		return 0;
	if (cfg_obj_get_object_attr(nodeName, "ServiceList", 0, serviceList, sizeof(serviceList)) <=0)
		return 0;
	if (cfg_obj_get_object_attr(nodeName, "ISP", 0, isp, sizeof(isp)) <=0)
		return 0;

	if( NULL != strstr(serviceList, "INTERNET")
		&& 0 != strcmp(isp, "3")  /* not BRIDGE_MODE*/
		&& 0 == strcmp("Yes", dslite_enable))
		return 1;
	else
		return 0;
}

void update_bind_status(void)
{
	int pvc_index = 0, entry_index = 0;
	char str_valid_if[128] = {0}, tmp[12] = {0};
	int valid_if[MAX_WAN_IF_INDEX] = {0}, valid_if_num = 0, i;
	char str_ipversion[16] = {0};
	int n = 0;
	int idx = 0;
	char node_service_name[64] = {0}, svc_list[32] = {0};

	char	strbinding[512] = {0};
	char	strbindtemp[256] = {0};
#if defined(TCSUPPORT_CT_DSLITE)
	int dslite = 0;
#endif
	
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
	char portbinding_status[MAX_LAN_PORT_NUM][MAX_BIND_INIT_SIZE];
	int iIfindexV4[MAX_LAN_PORT_NUM];
	int iIfindexV6[MAX_LAN_PORT_NUM];
    int iIPv4[MAX_LAN_PORT_NUM];
	int iIPv6[MAX_LAN_PORT_NUM];

	memset(iIPv4, 0, sizeof(iIPv4));
	memset(iIPv6, 0, sizeof(iIPv6));
	memset(portbinding_status, 0, sizeof(portbinding_status));
	for(i=0; i<MAX_LAN_PORT_NUM; i++)
	{
		snprintf(portbinding_status[i], MAX_BIND_INIT_SIZE, "No");
	}
#if defined(TCSUPPORT_CT_DSLITE)
	for(i=0; i<MAX_LAN_PORT_NUM; i++)
	{
		iIfindexV4[i] = -1;
		iIfindexV6[i] = -1;
	}
#endif
#else
	/*initial portbind status string.*/
	int iIPv4[14] = {0}, iIPv6[14] = {0};
	char	portbinding_status[][5] = 
	{
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
#ifdef TCSUPPORT_WLAN_AC
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
		{"No"},
#endif
	};
#if defined(TCSUPPORT_CT_DSLITE)
	int iIfindexV4[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	int iIfindexV6[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
#endif
#endif

	char path[64] = {0};
	/*Update valid interface index everytime*/
	valid_if_num = get_all_wan_index(valid_if, MAX_WAN_IF_INDEX);

	for(i = 0; i < valid_if_num; i++)
	{
		snprintf(tmp, sizeof(tmp), "%d,", valid_if[i]);
		strncat(str_valid_if, tmp, (sizeof(str_valid_if) - strlen(str_valid_if)));
		
		/*Init wan info entry:update GUI interface name*/		
		pvc_index = valid_if[i] / MAX_SMUX_NUM;
		entry_index = valid_if[i] % MAX_SMUX_NUM;

		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);
		
		if (cfg_obj_get_object_attr(path, "IPVERSION", 0, str_ipversion, sizeof(str_ipversion)) <=0)
		{
			strncpy(str_ipversion, "N/A", sizeof(str_ipversion) - 1);
		}
		
#if defined(TCSUPPORT_CT_DSLITE)
		dslite = get_dslite_stateviacfg(path);
#endif
		for (n = 0; n < sizeof(bl_map_data) / sizeof(bl_map_data[0]); n++) 
		{
			if (cfg_obj_get_object_attr(path, bl_map_data[n].bindif, 0, strbinding, sizeof(strbinding)) >0)
			{
				if (!strcmp(strbinding, "Yes")) 
				{
					/* 5G */
					if ( strstr(bl_map_data[n].realif, "rai") )
					{
						bzero(node_service_name, sizeof(node_service_name));
						bzero(svc_list, sizeof(svc_list));
						idx = bl_map_data[n].realif[3] - '0';
						snprintf(node_service_name, sizeof(node_service_name), "root.wlan11ac.entry.%d", idx + 1);
						cfg_obj_get_object_attr(path, "ServiceList", 0, svc_list, sizeof(svc_list));
						cfg_set_object_attr(node_service_name, "Service", svc_list);
					}
					/* 2.4G */
					else if ( strstr(bl_map_data[n].realif, "ra") )
					{
						bzero(node_service_name, sizeof(node_service_name));
						bzero(svc_list, sizeof(svc_list));
						idx = bl_map_data[n].realif[2] - '0';
						snprintf(node_service_name, sizeof(node_service_name), "root.wlan.entry.%d", idx + 1);
						cfg_obj_get_object_attr(path, "ServiceList", 0, svc_list, sizeof(svc_list));
						cfg_set_object_attr(node_service_name, "Service", svc_list);
					}
					
					if(!strcmp(str_ipversion,"IPv4"))
					{
						iIPv4[n]++;
#if defined(TCSUPPORT_CT_DSLITE)
						iIfindexV4[n] = valid_if[i];
#endif
					}
					else if(!strcmp(str_ipversion,"IPv6"))
					{
#if defined(TCSUPPORT_CT_DSLITE)
						if ( dslite )
						{
							iIPv4[n]++;
							iIfindexV4[n] = valid_if[i];
						}
						iIfindexV6[n] = valid_if[i];
#endif
						iIPv6[n]++;
					}
					else if(!strcmp(str_ipversion,"IPv4/IPv6"))
					{
						iIPv4[n]++;
						iIPv6[n]++;
#if defined(TCSUPPORT_CT_DSLITE)
						iIfindexV4[n] = valid_if[i];
						iIfindexV6[n] = valid_if[i];
#endif
					}
					strncpy(portbinding_status[n], "Yes", MAX_BIND_INIT_SIZE - 1);
				}
			}
		}
	}

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WANINFO_COMMON_NODE);

	memset(strbinding, 0, sizeof(strbinding));
	memset(strbindtemp, 0, sizeof(strbindtemp));
	for(n=0; n<(sizeof(portbinding_status)/(sizeof(portbinding_status[0]))); n++)
	{
		if('\0' == strbindtemp[0])
			strncpy(strbindtemp, portbinding_status[n], sizeof(strbindtemp) - 1);
		else{
			strncat(strbindtemp, ",", (sizeof(strbindtemp) - strlen(strbindtemp)));
			strncat(strbindtemp, portbinding_status[n], (sizeof(strbindtemp) - strlen(strbindtemp)));
		}
	}
	strncpy(strbinding, strbindtemp, sizeof(strbinding) - 1);
	memset(strbindtemp, 0, sizeof(strbindtemp));
	for(n=0; n<sizeof(bl_map_data) / sizeof(bl_map_data[0]); n++){
		snprintf(strbindtemp, sizeof(strbindtemp), ",%d,%d", iIPv4[n], iIPv6[n]);
		strncat(strbinding, strbindtemp, (sizeof(strbinding) - strlen(strbinding)));
	}
#if defined(TCSUPPORT_CT_DSLITE)
	for(n=0; n<sizeof(bl_map_data) / sizeof(bl_map_data[0]); n++)
	{
		snprintf(strbindtemp, sizeof(strbindtemp), ",%d,%d", iIfindexV4[n], iIfindexV6[n]);
		strncat(strbinding, strbindtemp, (sizeof(strbinding) - strlen(strbinding)));
	}
#endif
	cfg_obj_set_object_attr(path, "BindStatus", 0, strbinding);
	return;
}


static int cfg_type_waninfo_common_func_get(char* path,char* attr, char* val,int len) 
{
	if (path == NULL || attr == NULL || val == NULL || len <= 0)
			return -1;
	
	if ( 1 == isInfoReading("waninfo_cycle_read") )
		return cfg_type_default_func_get(path,attr,val,len);
	
	if (strcmp(attr,"CycleValue") == 0)
		return waninfo_common_get_attr_cyclevalue(val,len);
	 else if (strcmp(attr,"ValidIFIndex") == 0)
		return waninfo_common_get_attr_validifindex(val,len);
	 else if (strcmp(attr,"ValidIFNum") == 0 )
		return waninfo_common_get_attr_validifnum(val,len);
	 else if (strcmp(attr,"CycleNum") == 0 )
	{
		setAttrValue(WANINFO_COMMON_NODE, "CycleIdx", "0");
		return waninfo_common_get_attr_validifnum(val,len);
	}
	else if (strcmp(attr,"AllRoute") == 0 )
		return waninfo_common_get_attr_allroute(val,len);
	else if(strcmp(attr,"DmzCycleValue") == 0 )
	{
		return waninfo_common_get_attr_dmz_cyclevalue(val,len);
	}
	else if(strcmp(attr,"AutoDMZ_Flag") == 0 )
	{
		return waninfo_common_get_attr_auto_dmz_flag(val,len);
	}
	else if(strcmp(attr,"VirServerCycleValue") == 0 )
	{
		return waninfo_common_get_attr_vir_srv_cyclevalue(val,len);
	}
	else if(0 == strcmp(attr, "BindStatus"))
	{
		update_bind_status();		
	}

	 return cfg_type_default_func_get(path,attr,val,len); 
}; 


static cfg_node_ops_t cfg_type_waninfo_common_ops  = { 
	 .get = cfg_type_waninfo_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_waninfo_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_common_ops, 
}; 


static int cfg_type_waninfo_message_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
}; 


static cfg_node_ops_t cfg_type_waninfo_message_ops  = { 
	 .get = cfg_type_waninfo_message_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_waninfo_message = { 
	 .name = "Message", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_message_ops, 
}; 

static int waninfo_wanif_get_valid_index(void)
{
	int ifindex = -1, i;
	char tmp[8];
	int valid_if[MAX_WAN_INTF_NUMBER], valid_if_num = 0;

	memset(valid_if, 0, sizeof(valid_if));
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);

	if (cfg_obj_get_object_attr(WANINFO_COMMON_NODE,"CurIFIndex",0, tmp,sizeof(tmp)) >0 )
	{
		ifindex = atoi(tmp);	
		for(i = 0; i < valid_if_num; i++)
		{
			if(ifindex == valid_if[i])
				break;
		}
	
		if(i == valid_if_num)
		{
			/*if CurIFIndex value is invalid, set first valid index as default value*/
			sprintf(tmp, "%d", valid_if[0]);
			cfg_set_object_attr(WANINFO_COMMON_NODE, "CurIFIndex", tmp);
		}
	}
	else
	{	
#if defined(TCSUPPORT_CUC_SFU)
		sprintf(tmp, "%d", 1);
#else
		sprintf(tmp, "%d", valid_if[0]);
#endif
		cfg_set_object_attr(WANINFO_COMMON_NODE, "CurIFIndex", tmp);
	}

	if (ifindex >= 0)
	{
		for(i=0; i < valid_if_num; i++)
		{
			if ((valid_if[i] & 0xff) == ifindex)
				return ifindex;
		}
	}
	
	if (valid_if_num > 0)
		ifindex = valid_if[0] & 0xff;
	
	return ifindex;
}

static int waninfo_wanif_sync_from_wan_entry(int pvc,int entry)
{
	char name[32];
	
	if (pvc < 1 || entry < 1 || pvc > MAX_WAN_PVC_NUMBER || entry > MAX_WAN_ENTRY_NUMBER)
		return 0;

	snprintf(name, sizeof(name), WAN_PVC_ENTRY_NODE,pvc,entry);

	cfg_obj_copy_private(WANINFO_WANIF_NODE,name);
	
	return 0;
}

static int cfg_type_waninfo_wanif_func_get(char* path,char* attr, char* val,int len) 
{ 
	int ifindex;

	if ((ifindex = waninfo_wanif_get_valid_index()) >= 0)
		waninfo_wanif_sync_from_wan_entry(ifindex / MAX_WAN_ENTRY_NUMBER + 1,ifindex % MAX_WAN_ENTRY_NUMBER + 1);
		
	return cfg_type_default_func_get(path,attr,val,len); 
}; 


static int cfg_type_waninfo_wanif_func_set(char* path,char* attr,char* val)
{
	if(NULL == path)
	{
		return -1;
	}

	return cfg_type_default_func_set(path, attr, val);
}

#if defined(TCSUPPORT_CT_JOYME4)
static int cfg_type_wanif_func_commit(char* path) 
{ 
	char conflict_ports[200] = {0}, tmp_conflict_ports[200] = {0};
	char *p = NULL;
	int pvc = 0, entry = 0;
	int v4flag = 0, v6flag = 0, v4v6flag = 0;
	char wan_if_ifidx[4] = {0};
	char wan_pvc_entry[32] = {0}, wan_active[8] = {0},wan_servicelist[32] = {0}, wan_bindport[12] = {0}, wan_ifidx[4] = {0};
	int conflictflag = 0;
	cfg_obj_get_object_attr(WANINFO_WANIF_NODE,"IFIdx",0, wan_if_ifidx,sizeof(wan_if_ifidx));


	if (cfg_obj_get_object_attr(WANINFO_WANIF_NODE,"conflict_ports",0, conflict_ports,sizeof(conflict_ports)) >0 && conflict_ports[0] != '\0')
	{
			for(pvc = 0; pvc < 8; pvc ++)
			{
				for(entry = 0; entry < 8; entry ++)
				{
					memset(wan_pvc_entry, 0, sizeof(wan_pvc_entry));
					memset(tmp_conflict_ports, 0, sizeof(tmp_conflict_ports));
					strncpy(tmp_conflict_ports, conflict_ports, sizeof(tmp_conflict_ports)-1);
					p = NULL;
					snprintf(wan_pvc_entry, sizeof(wan_pvc_entry), WAN_PVC_ENTRY_NODE, pvc + 1, entry + 1);
					if (cfg_obj_get_object_attr(wan_pvc_entry,"Active",0, wan_active,sizeof(wan_active)) <= 0)
						continue;
					if (cfg_obj_get_object_attr(wan_pvc_entry,"IFIdx",0, wan_ifidx,sizeof(wan_ifidx)) >0 && !strcmp(wan_ifidx, wan_if_ifidx))
						continue;
					if (cfg_obj_get_object_attr(wan_pvc_entry,"ServiceList",0, wan_servicelist,sizeof(wan_servicelist)) >0 && 
						(!strcmp(wan_servicelist, "TR069") || !strcmp(wan_servicelist, "VOICE") || !strcmp(wan_servicelist, "TR069_VOICE")))
						continue;

					p = strtok(tmp_conflict_ports, ",");
					while(p)
					{
						memset(wan_bindport, 0, sizeof(wan_bindport));
						if (cfg_obj_get_object_attr(wan_pvc_entry, p, 0, wan_bindport,sizeof(wan_bindport)) >0 &&  0 == strcmp(wan_bindport, "Yes"))
						{
								cfg_set_object_attr(wan_pvc_entry, p, "No");
								conflictflag = 1;
						}
						p = strtok(NULL, ",");
					}
					if(conflictflag)
					{
						cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_UPDATE, wan_pvc_entry);
						conflictflag = 0;
					}
				}
			}
	}
	
	return cfg_type_default_func_commit(path); 
}
#else
static int cfg_type_waninfo_wanif_func_commit(char* path)
{
	int ifindex = -1;
	char tmp[8] = {0}, nodeName[64] = {0};
	char str_pppmanualstatus[16] = {0};

	if ( cfg_obj_get_object_attr(path, "PPPManualStatus", 0, str_pppmanualstatus, sizeof(str_pppmanualstatus)) <= 0 )
		return -1;

	if ( cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "CurIFIndex", 0, tmp, sizeof(tmp) ) > 0 )
	{
		ifindex = atoi(tmp);
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, ifindex / MAX_WAN_ENTRY_NUMBER + 1,ifindex % MAX_WAN_ENTRY_NUMBER + 1);
		cfg_set_object_attr(nodeName, "PPPManualStatus", str_pppmanualstatus);
		cfg_obj_commit_object(nodeName);
	}
	
	return 0;
}
#endif
static cfg_node_ops_t cfg_type_waninfo_wanif_ops  = { 
	 .get = cfg_type_waninfo_wanif_func_get, 
	 .set = cfg_type_waninfo_wanif_func_set, 
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query,
#if defined(TCSUPPORT_CT_JOYME4)
	 .commit = cfg_type_wanif_func_commit
#else	 
	 .commit = cfg_type_waninfo_wanif_func_commit 
#endif
}; 


static cfg_node_type_t cfg_type_waninfo_wanif = { 
	 .name = "wanIF", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_wanif_ops, 
}; 


static int waninfo_wanpvc_sync_from_wan_pvc(int pvc)
{
	char name[32];
	
	if (pvc < 1 || pvc > MAX_WAN_PVC_NUMBER)
		return 0;
	
	memset(name, 0, sizeof(name));
	snprintf(name, sizeof(name), WAN_PVC_NODE,pvc);
	
	cfg_obj_copy_private(WANINFO_WANPVC_NODE,name);

	return 0;
}

static int cfg_type_waninfo_wanpvc_func_get(char* path,char* attr, char* val,int len) 
{ 
	int ifindex;

	if ((ifindex = waninfo_wanif_get_valid_index()) >= 0)
		waninfo_wanpvc_sync_from_wan_pvc(ifindex / MAX_WAN_ENTRY_NUMBER + 1);
	
	 return cfg_type_default_func_get(path,attr,val,len); 
}; 


#if !defined(TCSUPPORT_CMCC)
static int waninfo_wanpvc_find_pvc(char* vlan,char* pbit)
{
	int i;
	char tmp[32],node[32];
#if defined(TCSUPPORT_CT_DSL_EX)
	char transMode[16] = {0};
#endif

	for(i=1; i<= MAX_WAN_PVC_NUMBER; i++)
	{
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node), WAN_PVC_NODE, i);

#if defined(TCSUPPORT_CT_DSL_EX)
		if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
			&& (0 == strcmp(transMode, "PTM")))
		{
			memset(tmp, 0, sizeof(tmp));
			if (cfg_obj_get_object_attr(node,"VLANID",0,tmp,sizeof(tmp)) <= 0)
				continue;
			
			if (strcmp(vlan,tmp) != 0)
				continue;
			
			memset(tmp, 0, sizeof(tmp));
			if (cfg_obj_get_object_attr(node,"DOT1P",0,tmp,sizeof(tmp)) <= 0)
				continue;
			
			if (strcmp(pbit,tmp) != 0)
				continue;
		}
		else
		{
			memset(tmp, 0, sizeof(tmp));
			if (cfg_obj_get_object_attr(node,"VCI",0,tmp,sizeof(tmp)) <= 0)
				continue;
			
			if (strcmp(vlan,tmp) != 0)
				continue;
			
			memset(tmp, 0, sizeof(tmp));
			if (cfg_obj_get_object_attr(node,"VPI",0,tmp,sizeof(tmp)) <= 0)
				continue;
			
			if (strcmp(pbit,tmp) != 0)
				continue;
		}
#else
		memset(tmp, 0, sizeof(tmp));
		if (cfg_obj_get_object_attr(node,"VLANID",0,tmp,sizeof(tmp)) <= 0)
			continue;

		if (strcmp(vlan,tmp) != 0)
			continue;
		
		memset(tmp, 0, sizeof(tmp));
		if (cfg_obj_get_object_attr(node,"DOT1P",0,tmp,sizeof(tmp)) <= 0)
			continue;

		if (strcmp(pbit,tmp) != 0)
			continue;
#endif

		return i;
	}
	
	return -1;
}
#endif

static int waninfo_wanpvc_get_empty_entry(int pvc)
{
	int i;
	char tmp[32];

	for(i=1; i<= MAX_WAN_ENTRY_NUMBER; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE,pvc,i);
		if (cfg_obj_query_object(tmp,NULL,NULL) <= 0)
			return i;
	}
	
	return -1;
}

static int waninfo_wanpvc_get_empty_pvc(void)
{
	int i;
	char tmp[32];

	for(i=1; i<= MAX_WAN_PVC_NUMBER; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, i);
		if (cfg_obj_query_object(tmp,NULL,NULL) <= 0)
			return i;
	}
	
	return -1;
}

static int waninfo_wanpvc_sync_to_wan_pvc(int pvc)
{
	char tmp[32];
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, pvc);
	
	return cfg_obj_copy_private(tmp,WANINFO_WANPVC_NODE);
}

static int waninfo_wanif_set_ipv6_br0(char *nodeName)
{
	char ipv6_br0[64] = {0};

	if ( (cfg_get_object_attr(nodeName, "Br0Addr", ipv6_br0, sizeof(ipv6_br0)) > 0) \
		&& (0 != strcmp(ipv6_br0, "N/A")) )
	{		
		cfg_set_object_attr(WANINFO_WANIF_NODE, "Br0Addr", ipv6_br0);
	}

	return 0;
}

static int waninfo_wanif_sync_to_wan_entry(int pvc,int entry)
{
	char tmp[32];
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE,pvc,entry);

#if defined(TCSUPPORT_NP)
	waninfo_wanif_set_ipv6_br0(tmp);
#endif
	
	return cfg_obj_copy_private_without_del(tmp,WANINFO_WANIF_NODE);
}

static int waninfo_wanpvc_create_empty_entry(int dstpvc)
{
	int dstentry;
	char tmp[32];
		
	if (dstpvc > 0){
		if ((dstentry = waninfo_wanpvc_get_empty_entry(dstpvc)) <= 0)
			return -1;
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, dstpvc, dstentry);
		if (cfg_obj_create_object(tmp) < 0)
			return -1;
	}else{
		if ((dstpvc = waninfo_wanpvc_get_empty_pvc()) <= 0)
			return -1;
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), WAN_PVC_NODE,dstpvc);
		if (cfg_obj_create_object(tmp) < 0)
			return -1;
		dstentry = 1;
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE,dstpvc,dstentry);
		if (cfg_obj_create_object(tmp) < 0)
			return -1;
	}	

	return (dstpvc - 1) * MAX_WAN_ENTRY_NUMBER + dstentry - 1;
}

static int waninfo_wanpvc_del_entry(int pvc, int entry)
{
	char tmp[32];
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE,pvc,entry);
#if defined(TCSUPPORT_CWMP_TR181)	
	clearAllTR181WanInterfaceNodeByWanIF(((pvc - 1)*PVC_NUM + (entry - 1)));
#endif
	cfg_obj_delete_object(tmp);

	cfg_type_waninfo_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_DELETE, tmp);
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_NODE,pvc);

	if (cfg_obj_query_object(tmp,"entry",NULL) <= 0)
		cfg_obj_delete_object(tmp);
	
	return 0;
}

void updateBridgeInterface(int curIndex)
{
	char bridgeInterface[16] = {0};
	char buf[8] = {0};
	char *idx = NULL;
	char nodeName[64] = {0};
	int conflictIdx = -1;

	memset(bridgeInterface, 0, sizeof(bridgeInterface));
	if( 0 <= cfg_get_object_attr(WANINFO_WANIF_NODE, "BridgeInterface", bridgeInterface,sizeof(bridgeInterface))
			&& (0 == strcmp(bridgeInterface, "Yes")) )
	{
		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(WANINFO_WANIF_NODE, "orgBridgeInterfaceIdx", buf, sizeof(buf)) >= 0 
			&& 0 != buf[0])
		{
			idx = strtok(buf, ",");
			while(NULL != idx)
			{
				conflictIdx = atoi(idx);
				if(curIndex != conflictIdx)
				{
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, conflictIdx / MAX_WAN_ENTRY_NUMBER + 1, conflictIdx % MAX_WAN_ENTRY_NUMBER + 1);
					cfg_set_object_attr(nodeName, "BridgeInterface", "No");
					cfg_obj_commit_object(nodeName);
				}
				idx = strtok(NULL, ",");
			}

		}
	
	}

}

static int waninfo_wanpvc_action_modify(int index)
{
	int srcpvc,srcentry,dstpvc,dstentry,ifindex;

#if !defined(TCSUPPORT_CMCC)
	char str_vid[8],str_pbit[8];
#endif
	char tmp[32];
#if defined(TCSUPPORT_CT_DSL_EX)
	char transMode[16] = {0};
#endif

#if defined(TCSUPPORT_CMCC)
	dstpvc = 1;
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, dstpvc);
	if(cfg_obj_query_object(tmp,NULL,NULL) <= 0)
	{
		dstpvc = -1;
	}
#else
#if defined(TCSUPPORT_CT_DSL_EX)
	if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
		&& (0 == strcmp(transMode, "PTM")))
	{
		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VLANID",0,str_vid,sizeof(str_vid)) <= 0)
			return -1;

		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"DOT1P",0,str_pbit,sizeof(str_pbit)) <= 0)
			return -1;
	}
	else
	{
		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VCI",0,str_vid,sizeof(str_vid)) <= 0)
			return -1;

		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VPI",0,str_pbit,sizeof(str_pbit)) <= 0)
			return -1;
	}
#else
	if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VLANID",0,str_vid,sizeof(str_vid)) <= 0)
		return -1;

	if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"DOT1P",0,str_pbit,sizeof(str_pbit)) <= 0)
		return -1;
#endif

	dstpvc = waninfo_wanpvc_find_pvc(str_vid,str_pbit);
#endif

	srcpvc = index / MAX_WAN_ENTRY_NUMBER + 1;
	srcentry = index % MAX_WAN_ENTRY_NUMBER + 1;
	if (srcpvc == dstpvc){
		dstentry = srcentry;
	}else{
		waninfo_wanpvc_del_entry(srcpvc,srcentry);
		if ((ifindex = waninfo_wanpvc_create_empty_entry(dstpvc)) < 0)
		{
			printf("waninfo_wanpvc_action_modify: create pvc or entry fail! \n");
			return -1;
		}
		dstpvc = ifindex / MAX_WAN_ENTRY_NUMBER + 1;
		dstentry = ifindex % MAX_WAN_ENTRY_NUMBER + 1;
	}
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d",(dstpvc - 1) * MAX_WAN_ENTRY_NUMBER + dstentry -1);
	cfg_obj_set_object_attr(WANINFO_COMMON_NODE,"CurIFIndex",0,tmp);	

	waninfo_wanpvc_sync_to_wan_pvc(dstpvc);
	waninfo_wanif_sync_to_wan_entry(dstpvc,dstentry);
	updateBridgeInterface(index);
	
#if defined(TCSUPPORT_NP_CMCC)
	cfg_obj_commit_object(APWANINFO_NODE);
#else
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, dstpvc, dstentry);
	cfg_obj_commit_object(tmp);
#endif
	
	return 0;
}

static int waninfo_wanpvc_action_add(void)
{
	int dstpvc,dstentry,ifindex;

#if !defined(TCSUPPORT_CMCC)
	char str_vid[8],str_pbit[8];
#endif

	char tmp[32];
#if defined(TCSUPPORT_CT_DSL_EX)
	char transMode[16] = {0};
#endif

#if defined(TCSUPPORT_CMCC)
	dstpvc = 1;
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, dstpvc);
	if(cfg_obj_query_object(tmp,NULL,NULL) <= 0)
	{
		dstpvc = -1;
	}
#else
#if defined(TCSUPPORT_CT_DSL_EX)
	if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
		&& (0 == strcmp(transMode, "PTM")))
	{
		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VLANID",0,str_vid,sizeof(str_vid)) <= 0)
			return -1;

		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"DOT1P",0,str_pbit,sizeof(str_pbit)) <= 0)
			return -1;
	}
	else
	{
		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VCI",0,str_vid,sizeof(str_vid)) <= 0)
			return -1;

		if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VPI",0,str_pbit,sizeof(str_pbit)) <= 0)
			return -1;
	}
#else
	if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"VLANID",0,str_vid,sizeof(str_vid)) <= 0)
		return -1;

	if (cfg_obj_get_object_attr(WANINFO_WANPVC_NODE,"DOT1P",0,str_pbit,sizeof(str_pbit)) <= 0)
		return -1;
#endif

	dstpvc = waninfo_wanpvc_find_pvc(str_vid,str_pbit);
#endif

	if ((ifindex = waninfo_wanpvc_create_empty_entry(dstpvc)) < 0)
	{
		printf("waninfo_wanpvc_action_add: create pvc or entry fail! \n");
		return -1;
	}
	
	dstpvc = ifindex / MAX_WAN_ENTRY_NUMBER + 1;
	dstentry = ifindex % MAX_WAN_ENTRY_NUMBER + 1;
    
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d",ifindex);
	cfg_obj_set_object_attr(WANINFO_COMMON_NODE,"CurIFIndex",0,tmp);
	
	waninfo_wanpvc_sync_to_wan_pvc(dstpvc);
	waninfo_wanif_sync_to_wan_entry(dstpvc,dstentry);
	updateBridgeInterface(ifindex);
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE,dstpvc,dstentry);

#if defined(TCSUPPORT_CMCCV2)
	char wantype[32] = {0};

	if (cfg_obj_get_object_attr(tmp,"ServiceList",0,wantype,sizeof(wantype)) <= 0)
		return -1;
	if(0 == strcmp(wantype, "INTERNET"))
	{
		openlog("TCSysLog WEB", 0, LOG_LOCAL1);
		syslog(LOG_INFO, "INTERNET WAN create\n");
		closelog();
	}
	else if(0 == strcmp(wantype, "VOICE"))
	{
		openlog("TCSysLog WEB", 0, LOG_LOCAL1);
		syslog(LOG_INFO, "VOIP WAN create\n");
		closelog();
	}
#endif
#if defined(TCSUPPORT_NP_CMCC)
	cfg_obj_commit_object(APWANINFO_NODE);
#else
	cfg_obj_commit_object(tmp);
#endif

	return 0;
}

static int waninfo_wanpvc_action_del(int index)
{
	int pvc,entry;

	pvc = index / MAX_WAN_ENTRY_NUMBER + 1;
	entry = index % MAX_WAN_ENTRY_NUMBER + 1;
	
	waninfo_wanpvc_del_entry(pvc,entry);
	
	return 0;
}

static int waninfo_clean_unused_pvc()
{
	char pvcnode[32] = {0}, wannode[32] = {0}, tmp[16] = {0};
	int i = 0, j = 0;
	int entrynum = 0, entryid[MAX_WAN_ENTRY_NUMBER] = {0};

	for ( i = 0; i < MAX_WAN_PVC_NUMBER; i++ )
	{
		memset(pvcnode, 0, sizeof(pvcnode));
		snprintf(pvcnode, sizeof(pvcnode), WAN_PVC_NODE, i + 1);
		/* wan_pvc i+1 is not exist, ignore */
		if ( cfg_obj_query_object(pvcnode, NULL, NULL) <= 0 )
			continue;

		/* wan_pvc has vlan ID, this pvc has been used */
		memset(tmp, 0, sizeof(tmp));
		cfg_get_object_attr(pvcnode, "VLANID", tmp, sizeof(tmp));
		if ( 0 != tmp[0] )
			continue;
		/* 
		** otherwise, we consider this pvc as an unused node. 
		** before we delete this pvc, check weather it has child 
		** entry nodes, and delete all childs and itself ?
		*/
		entrynum = 0;
		memset(entryid, 0, sizeof(entryid));
		if ( ( entrynum = cfg_obj_query_object(pvcnode, "entry", entryid) ) > 0 )
		{
			for ( j = 0; j < entrynum; j++ )
			{
				memset(wannode, 0, sizeof(wannode));
				snprintf(wannode, sizeof(wannode), WAN_PVC_ENTRY_NODE, i + 1, entryid[j]);
				cfg_obj_delete_object(pvcnode);
			}
		}
		cfg_obj_delete_object(pvcnode);
	}
	return 0;
}

static int cfg_type_waninfo_wanpvc_func_commit(char* path)
{
	char action[16];
	char ifindex[8];
	int index = -1;
	
	/* delete unused pvc node */
	waninfo_clean_unused_pvc();

	if (cfg_obj_get_object_attr(path,"Action",0,action,sizeof(action)) <= 0)
		return -1;
	
	if (cfg_obj_get_object_attr(WANINFO_COMMON_NODE,"CurIFIndex",0,ifindex,sizeof(ifindex)) > 0)
		index = atoi(ifindex);

	if (strcmp(action,"Modify") == 0 && index >= 0 && index < MAX_WAN_INTF_NUMBER)
		waninfo_wanpvc_action_modify(index);
	else if (strcmp(action,"Add") == 0)
		waninfo_wanpvc_action_add();
	else if (strcmp(action,"Del") == 0 && index >= 0 && index < MAX_WAN_INTF_NUMBER)
		waninfo_wanpvc_action_del(index);
	else
		printf("[%s]: unsupported action [%s] \n", __FUNCTION__,action);
	
	return 0;
}

static cfg_node_ops_t cfg_type_waninfo_wanpvc_ops  = { 
	 .get = cfg_type_waninfo_wanpvc_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_waninfo_wanpvc_func_commit 
}; 


static cfg_node_type_t cfg_type_waninfo_wanpvc = { 
	 .name = "wanPVC", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_wanpvc_ops, 
}; 


static int cfg_type_waninfo_mannualdns_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
}; 


static cfg_node_ops_t cfg_type_waninfo_mannualdns_ops  = { 
	 .get = cfg_type_waninfo_mannualdns_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_waninfo_mannualdns = { 
	 .name = "mannualdns", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_mannualdns_ops, 
}; 

static int cfg_type_waninfo_internet_func_get(char* path,char* attr, char* val,int len) 
{ 
#if defined(TCSUPPORT_CT_PMINFORM)
	int valid_if_num = 0;
	int valid_if[MAX_WAN_INTF_NUMBER];
	int i = 0;
	char nodeName[32];
	int pvc_index = 0;
	int entry_index = 0;
	char str_active[8];
	char str_servicelist[64];
	char str_ispvalue[6];
	char user_name[32];

	if (path == NULL || attr == NULL || val == NULL || len <= 0)
	{
		return -1;
	}
	
	memset(valid_if, 0, sizeof(valid_if));
	valid_if_num =  get_all_wan_index(valid_if, MAX_WAN_INTF_NUMBER);
	for(i = 0; i < valid_if_num; i++)
	{
		pvc_index = valid_if[i] / MAX_WAN_ENTRY_NUMBER + 1;
		entry_index = valid_if[i] % MAX_WAN_ENTRY_NUMBER + 1;
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index,entry_index);
		
		memset(str_active, 0, sizeof(str_active));
		if ((cfg_obj_get_object_attr(nodeName,"Active", 0, str_active, sizeof(str_active)) <= 0) \
			|| (0 != strcmp(str_active, "Yes")))
		{
			continue;
		}

		memset(str_servicelist, 0, sizeof(str_servicelist));
		if ((cfg_obj_get_object_attr(nodeName,"Active", 0, str_servicelist, sizeof(str_servicelist)) <= 0) \
			|| (0 != strstr(str_servicelist, "INTERNET")))
		{
			continue;
		}
		
		/*ppp mode : 2*/
		memset(str_ispvalue, 0, sizeof(str_ispvalue));
		if ((cfg_obj_get_object_attr(nodeName,"ISP", 0, str_ispvalue, sizeof(str_ispvalue)) <= 0) \
			|| (0 != strcmp(str_ispvalue, "2")))
		{
			continue;
		}

		memset(user_name, 0, sizeof(user_name));
		cfg_obj_get_object_attr(nodeName, "USERNAME", 0, user_name, sizeof(user_name));
		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WANINFO_INTERNET_NODE);
		
		cfg_obj_set_object_attr(nodeName, "UserID", 0, user_name);

		break;
	}
#endif

	 return cfg_type_default_func_get(path,attr,val,len); 
}; 

static cfg_node_ops_t cfg_type_waninfo_internet_ops  = { 
	 .get = cfg_type_waninfo_internet_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_waninfo_internet = { 
	 .name = "INTERNET", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_internet_ops, 
}; 

#if defined(TCSUPPORT_CT_UBUS)
static void update_br_wan_connect_time(char* path)
{
	char tmp[32];
	char nodename[32];
	char strSec[32];
	struct timespec cur_time;	
	int iSec = 0;
	
	memset(nodename, 0, sizeof(nodename));
	snprintf(nodename, sizeof(nodename), WANINFO_BRENTRY_NODE);

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(path, "Status", 0, tmp, sizeof(tmp)) <= 0) \
		|| 0 != strcmp(tmp, "up")) 
	{
		/*not exist or down*/
		cfg_obj_set_object_attr(path, "connect_sec", 0,  "0");
		return ;
	}
	else
	{
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		memset(tmp, 0, sizeof(tmp));	
		if(cfg_obj_get_object_attr(nodename, "uptime_sec", 0, tmp, sizeof(tmp)) > 0)
		{
			if(0 == strcmp(tmp, "-1"))
			{
				/*not up*/
				cfg_obj_set_object_attr(path, "connect_sec", 0, "0");
			}
			else
			{
				iSec = cur_time.tv_sec - atoi(tmp);	
				memset(strSec, 0, sizeof(strSec));
				snprintf(strSec, sizeof(strSec), "%d", iSec);
				cfg_obj_set_object_attr(path, "connect_sec", 0,  strSec);
			}
		}
	}
	
	return ;
}

static int cfg_type_waninfo_brentry_func_get(char* path,char* attr, char* val,int len) 
{ 
	 if (path == NULL || attr == NULL || val == NULL || len <= 0)
	 {
		 return -1;
	 }
	 update_br_wan_connect_time(path);
	 return cfg_type_default_func_get(path,attr,val,len); 
}; 


static cfg_node_ops_t cfg_type_waninfo_brentry_ops  = { 
	 .get = cfg_type_waninfo_brentry_func_get,  
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_waninfo_brentry = { 
	 .name = "BrEntry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_waninfo, 
	 .ops = &cfg_type_waninfo_brentry_ops, 
}; 
#endif

static cfg_node_ops_t cfg_type_waninfo_ops  = { 
	 .query = cfg_type_default_func_query 
}; 


static cfg_node_type_t* cfg_type_waninfo_child[] = { 
	 &cfg_type_waninfo_entry, 
	 &cfg_type_waninfo_common, 
	 &cfg_type_waninfo_message, 
	 &cfg_type_waninfo_wanif, 
	 &cfg_type_waninfo_wanpvc, 
	 &cfg_type_waninfo_mannualdns, 
	 &cfg_type_waninfo_internet, 
#if defined(TCSUPPORT_CT_UBUS)
	 &cfg_type_waninfo_brentry, 
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_waninfo = { 
	 .name = "WanInfo", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_waninfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_waninfo_child, 
	 .ops = &cfg_type_waninfo_ops, 
}; 

