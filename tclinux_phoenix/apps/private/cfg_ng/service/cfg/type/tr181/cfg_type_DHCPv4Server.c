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

#if 0
int findWIFISsidEntryByLanIdx(int lanIdx){
	char wifiSsidNode[MAXLEN_NODE_NAME]= {0};
	char tmp[8] = {0};
	int i = 0;
	
	for(i = 0; i < WIFI_SSID_NUM_MAX; i++){
		snprintf(wifiSsidNode, MAXLEN_NODE_NAME - 1,CFG_TR181_WIFI_SSID_ENTRY_NODE, i + 1);
		cfg_obj_get_object_attr(wifiSsidNode, "lanIndex",0, tmp,sizeof(tmp));
		if(atoi(tmp) == lanIdx){
			return i;
		}
	}

	return -1;
}


int syncDhcpdLanIf2Tr181DHCPv4s(){
	char buf[512] = {0};
	int i =0;
	char tmp[8] = {0};
	char val[8] = {0};
	char tmpValue[32] = {0};
	int len=0;
	int index = -1;

	for(i = 1; i < BRIDGE_PORT_NUM ; i++){
		memset(val,0,sizeof(val));
		memset(tmp,0,sizeof(tmp));
		memset(tmpValue,0,sizeof(tmpValue));
		if(i < LAN_INDEX_WLAN_START)
		{
			snprintf(tmp,sizeof(tmp),"eth0.%d",i);
			if(cfg_obj_get_object_attr(DHCPD_COMMON_NODE, tmp, 0,val,sizeof(val)) >= 0 && !strcmp(val,"Yes"))
			{
				snprintf(tmpValue,sizeof(tmpValue),"Device.Ethernet.Interface.%d,",i);	
			}
		}
		else if(i < LAN_INDEX_WLAN_5G_START)
		{
			snprintf(tmp,sizeof(tmp),"ra%d",i - LAN_INDEX_WLAN_START);
			if(cfg_obj_get_object_attr(DHCPD_COMMON_NODE, tmp, 0,val,sizeof(val)) >= 0 && !strcmp(val,"Yes"))
			{	
				index = findWIFISsidEntryByLanIdx(i);
				snprintf(tmpValue,sizeof(tmpValue),"Device.WiFi.SSID.%d,",index + 1);	
			}	
		}
		else
		{
			snprintf(tmp,sizeof(tmp),"rai%d",i - LAN_INDEX_WLAN_5G_START);
			if(cfg_obj_get_object_attr(DHCPD_COMMON_NODE, tmp, 0,val,sizeof(val)) >= 0 && !strcmp(val,"Yes"))
			{	
				index = findWIFISsidEntryByLanIdx(i);
				snprintf(tmpValue,sizeof(tmpValue),"Device.WiFi.SSID.%d,",index + 1);	
			}	
		}
		strncat(buf,tmpValue,sizeof(tmpValue));		
	}
	len=strlen(buf);
	buf[len-1]='\0';	
	cfg_obj_set_object_attr(CFG_TR181_DHCPV4_SERVER_COMMON_NODE, "MTK_LanInterface", 0 ,buf);
	
	return 0;
}


int syncDHCPv4SLanIF2Dhcpd(char* lanIF){
	char wifinode[MAXLEN_NODE_NAME]= {0};
	int i =0,idx = -1;
	char *tmp = NULL;
	char attrname[16]={0};
	char lanidx[8]={0};


	/*clear all lan info before set*/
	for(i = 1; i < BRIDGE_PORT_NUM; i++)
	{
		if(i < LAN_INDEX_WLAN_START)
		{
			snprintf(attrname,sizeof(attrname),"eth0.%d", i);
			cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "No");
		}else if(i < LAN_INDEX_WLAN_5G_START){
			snprintf(attrname,sizeof(attrname),"ra%d", i - LAN_INDEX_WLAN_START);
			cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "No");
		}
		else{
			snprintf(attrname,sizeof(attrname),"rai%d", i - LAN_INDEX_WLAN_5G_START);
			cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "No");
		}
	}

	/*parse lanIF */
	tmp=strtok(lanIF,",");
	while(tmp)
	{	
		memset(attrname,0,sizeof(attrname));	
		memset(lanidx,0,sizeof(lanidx));
		if(strstr(tmp,TR181_ETHER_INTERFACE))
		{
			idx = getSuffixIntByLowerLayers(tmp);
			snprintf(attrname,sizeof(attrname),"eth0.%d",idx);
			cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "Yes");
		}
		else if(strstr(tmp,TR181_WIFI_SSID_IF))
		{
			idx = getSuffixIntByLowerLayers(tmp);
			memset(wifinode,0,sizeof(wifinode));
			snprintf(wifinode,sizeof(wifinode),CFG_TR181_WIFI_SSID_ENTRY_NODE,idx);
			cfg_obj_get_object_attr(wifinode, "lanIndex", 0 ,lanidx ,sizeof(lanidx));
			if(atoi(lanidx) >= LAN_INDEX_WLAN_START && atoi(lanidx) < LAN_INDEX_WLAN_5G_START)
			{
				snprintf(attrname,sizeof(attrname),"ra%d",atoi(lanidx) - LAN_INDEX_WLAN_START);
				cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "Yes");
			}
			else if(atoi(lanidx)>= LAN_INDEX_WLAN_5G_START)
			{
				snprintf(attrname,sizeof(attrname),"rai%d",atoi(lanidx) - LAN_INDEX_WLAN_5G_START);
				cfg_obj_set_object_attr(DHCPD_COMMON_NODE, attrname,0 , "Yes");
			}
				
		}
		else
		{
			return -1;
		}
		
		tmp = strtok(NULL,",");
	}
	
	return 0;
}
#endif


int updateDHCPv4SFromLan(){
	char buf[8] = {0};
	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(LAN_DHCP_NODE, "type" ,0 , buf ,sizeof(buf)) >= 0 ){
		if(!strcmp(buf, "0")){
			/*static*/
			cfg_obj_set_object_attr(TR181_DHCPV4_SERVER_COMMON_NODE, "Enable", 0, "0");	
			
			updateDHCPv4SvrPool("0");
			
			update_tr181_DHCPv4Relay_FromLan("0");
		}else if(!strcmp(buf, "1")){
			/*DHCP*/
			cfg_obj_set_object_attr(TR181_DHCPV4_SERVER_COMMON_NODE, "Enable", 0, "1");
			
			updateDHCPv4SvrPool("1");
			
			update_tr181_DHCPv4Relay_FromLan("0");
		}else if(!strcmp(buf, "2")){
			/*DHCP relay*/			
			cfg_obj_set_object_attr(TR181_DHCPV4_SERVER_COMMON_NODE, "Enable", 0, "0"); 

			updateDHCPv4SvrPool("0");
			
			update_tr181_DHCPv4Relay_FromLan("1");				
			update_tr181_DHCPv4Relay_FromDhcpRelay();
		}
	}
		
	return 0;

}


int cfg_type_dhcpv4server_func_commit()
{
	char buf[8] = {0};
	if(cfg_obj_get_object_attr(TR181_DHCPV4_SERVER_COMMON_NODE, "Enable", 0 ,buf, sizeof(buf)) >= 0) {
		cfg_obj_set_object_attr(LAN_DHCP_NODE, "type", 0, !strcmp(buf, "0") ? "0" : "1"); 
		cfg_tr181_commit_object(LAN_DHCP_NODE);
	}
	return 0;
}


static cfg_node_ops_t cfg_type_dhcpv4server_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4server_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4server_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_dhcpv4server, 
	 .ops = &cfg_type_dhcpv4server_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv4server_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4server_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4server_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv4server, 
	 .ops = &cfg_type_dhcpv4server_common_ops, 
};


static cfg_node_ops_t cfg_type_dhcpv4server_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4server_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4server_child[] = { 
	 &cfg_type_dhcpv4server_entry, 
	 &cfg_type_dhcpv4server_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4server= { 
	 .name = "DHCPv4Server", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4server_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4server_child, 
	 .ops = &cfg_type_dhcpv4server_ops, 
}; 
