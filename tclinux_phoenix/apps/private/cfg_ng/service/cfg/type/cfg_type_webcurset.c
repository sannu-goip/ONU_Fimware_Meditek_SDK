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


#define MAX_WAN_IF_INDEX  (PVC_NUM*MAX_SMUX_NUM)

char webCurSet_attr[][16]=
	{
		{"wan_pvc"},
#ifndef PURE_BRIDGE
		{"nat_pvc"},
#endif
		{"diag_pvc"},
		{"dev_pvc"},
#ifndef PURE_BRIDGE
		{"virServ_id"},
		{"route_id"},
#ifdef IPV6
		{"route6_id"},
#endif
#endif
		{"statis_type"},
		{"lan_id"},
#ifdef ALIAS_IP
		{"lanAlias_id"},
#endif
#ifndef PURE_BRIDGE
		{"wlan_id"},
#if defined(TCSUPPORT_WLAN_AC)
		{"wlan_ac_id"},
#endif
		#if QOS_REMARKING 
		{"qos_id"},
		#endif
		{"acl_id"},
		{"ipfilter_id"},
		{"url_filter_id"},
#endif
		#if VOIP
		{"SIPIndexBasic"},
		{"SIPIndexCall"},
		{"SIPIndexMedia"},
		{"SIPIndexSpeed"},
		#endif
		{"CurrentAccess"},
		{"autopvc_id"},
#if defined(TCSUPPORT_CT_NETWORKMANAGESERVICE)
		{"account_id"},
#endif
#if defined(TCSUPPORT_CT_E8GUI)
		{"ddns_id"},
#endif
#if defined(TCSUPPORT_RIPD)
		{"ripd_id"},
#endif
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
		{"ippdiag_id"},
#endif
#if defined(TCSUPPORT_CT_ALARMMONITOR)
		{"alarm_id"},
		{"monitor_id"},
#endif
#if defined(TCSUPPORT_CT_USB_BACKUPRESTORE)
		{"rstresult"},
		{"opt"},
#endif
#ifdef CWMP
		{"switchpara_id"},
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
		{"user_id"},
		{"macfilter_user"},	
		{"ipfilter_user"},
		{"whiteurl_user"},
		{"blackurl_user"},		
#endif
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
		{"parental_id"},
#endif
		{""}
	};

int cfg_type_webcurset_func_get(char* path, char* attr, char* val, int len){
	char nodeName[64] = {0};
	cfg_node_type_t *curNode=NULL;
	int i=0;
#if defined(TCSUPPORT_CT_PON_JS)	
	char attrValue[32] = {0};
	char str_wanmode[32] = {0};
	char str_servicelist[32] = {0};
	int valid_if[MAX_WAN_IF_INDEX] = {0}, valid_if_num = 0;
	int pvc_index = 0, entry_index = 0;
#endif
	memset(nodeName,0,sizeof(nodeName));

	strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName)-1);
	
	curNode=cfg_type_query(nodeName);

	if(curNode==NULL)
	{
		return FAIL;
	}
	/*Init WebCurSet cfg node attribute-value*/
	for(i=0; strlen(webCurSet_attr[i])!=0; i++){
		cfg_set_object_attr(nodeName, webCurSet_attr[i], "0");
	}
	cfg_set_object_attr(nodeName, "dmz_ifidx", "999");
#if defined(TCSUPPORT_CT_PON_JS)
	valid_if_num = get_all_wan_index(valid_if, MAX_WAN_IF_INDEX);
	for(i = 0; i < valid_if_num; i++){	
		pvc_index = valid_if[i] / MAX_SMUX_NUM + 1;
		entry_index = valid_if[i] % MAX_SMUX_NUM + 1;
		memset(nodeName, 0, sizeof(nodeName));		
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
		
		if(cfg_obj_get_object_attr(nodeName, "WanMode", 0, str_wanmode, sizeof(str_wanmode)) < 0)
		{
			continue;
		}			
		if(cfg_obj_get_object_attr(nodeName, "ServiceList", 0, str_servicelist, sizeof(str_servicelist)) < 0)
		{
			continue;
		}
		if((strstr(str_servicelist,"TR069")==NULL || strstr(str_servicelist,"INTERNET")!=NULL) && strstr(str_wanmode,"Route")!=NULL)
		{
			snprintf(attrValue, sizeof(attrValue), "%d", valid_if[i]);
			break;
		}
	}
	
	if(strlen(attrValue) > 0)
		cfg_set_object_attr(nodeName, "nat_ifidx", attrValue);
	else		
		cfg_set_object_attr(nodeName, "nat_ifidx", "999");
#else
	cfg_set_object_attr(nodeName, "nat_ifidx", "999");
#endif
	return cfg_type_default_func_get(path,attr,val,len);
}

static cfg_node_ops_t cfg_type_webcurset_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_webcurset_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_webcurset, 
	 .ops = &cfg_type_webcurset_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_webcurset_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .memory_type_read = cfg_type_webcurset_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_webcurset_child[] = { 
	 &cfg_type_webcurset_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_webcurset = { 
	 .name = "WebCurSet", 
	 .flag = CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_webcurset_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_webcurset_child, 
	 .ops = &cfg_type_webcurset_ops, 
}; 
