

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
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h>
#include "utility.h"
#include "libapi_lib_switchmgr.h"
#include "blapi_traffic.h"


#define MAC_ADDR_LEN		12
#define	MAC_ADDR_DOT_LEN	17

int internet_0_flag = 0;
int internet_1_flag = 0;
int storage_flag = 0;

int mac_add_dot(char *old_mac, char *new_mac)
{
	int i = 0, j = 0;

	if (old_mac == NULL || new_mac == NULL)
		return -1;

	for (j = 0; j < MAC_ADDR_DOT_LEN; j += 3) {
		new_mac[j] = old_mac[i];
		new_mac[j + 1] = old_mac[i + 1];
		/* before the fifth round*/
		if (j < (MAC_ADDR_DOT_LEN-2)){
			if (old_mac[i+2] == ':'){
				i++;
			}
			new_mac[j + 2] = ':';
		}
		i += 2;
	}
	return 0;
}

int getACLRate(int rate)
{
	int aclRate = 0x3FFF;
	int r1 = 0;
	int r2 = 0;

	r1 = rate/64;
	r2 = rate%32; 
	if(rate != 0){							
		if(r1 == 0){
			aclRate = 1;			/*ACL rate is n*64kbps */
		}else{
			if(r2 < 32){
				aclRate = r1;			/*ACL rate is n*64kbps */
			}
		}
			
	}

	return aclRate;
}

int clean_access_rule(void)
{
	char cmd[128] = {0};

	/* clear internet access / storage access rule first */
	snprintf(cmd, sizeof(cmd), "iptables -t filter -F internet_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iptables -t filter -D FORWARD -j internet_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -F internet_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -D FORWARD -j internet_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iptables -t filter -F storage_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iptables -t filter -D INPUT -j storage_chain 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "ebtables -t filter -F Station_Limit 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "ebtables -t filter -D INPUT -j Station_Limit 2>/dev/null");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "ebtables -t filter -D FORWARD -j Station_Limit 2>/dev/null");
	system(cmd);
	/* clear flag */
	internet_0_flag = 0;
	internet_1_flag = 0;
	storage_flag = 0;

	return 0;
	
}

int set_internet_access_rule(char *path, char *mac)
{
	int internet = 2;
	char internet_access[32] = {0};
	char cmd[128] = {0};
	int type = 0, port = 0;
	char connType[4] = {0}, connPport[4] = {0};
	char aclCmd_switch[256] = {0}, aclCmd_list[256] = {0};
	char sta_limit_cmd[256] = {0};
	char cmdValue[32] = {0};
	
	if (NULL == path || NULL == mac)
		return internet;

	if (cfg_obj_get_object_attr(path, "InternetAccess", 0, internet_access, sizeof(internet_access)) > 0) {
		internet = atoi(internet_access);
		
		if ( 0 == internet )
		{
			/* set limit rule */
			if ( internet_0_flag == 0 ) {
				snprintf(sta_limit_cmd, sizeof(sta_limit_cmd), "ebtables -N Station_Limit 2>/dev/null");
				system(sta_limit_cmd);
				memset(sta_limit_cmd, 0, sizeof(sta_limit_cmd));
				snprintf(sta_limit_cmd, sizeof(sta_limit_cmd), "ebtables -A INPUT -j Station_Limit 2>/dev/null");
				system(sta_limit_cmd);
				memset(sta_limit_cmd, 0, sizeof(sta_limit_cmd));
                snprintf(sta_limit_cmd, sizeof(sta_limit_cmd), "ebtables -I FORWARD -j Station_Limit 2>/dev/null");
				system(sta_limit_cmd);

				internet_0_flag = 1;
			}
			memset(sta_limit_cmd, 0, sizeof(sta_limit_cmd));
			snprintf(sta_limit_cmd, sizeof(sta_limit_cmd), "ebtables -A Station_Limit -s %s -j DROP 2>/dev/null", mac);
			system(sta_limit_cmd);
			
#if 0
			/* kick out station */
			if ( cfg_obj_get_object_attr(path, "ConnectionType", 0, connType, sizeof(connType)) > 0 ) {
				type = atoi(connType);
				if ( type == 1 ) {
					if ( cfg_obj_get_object_attr(path, "Port", 0, connPport, sizeof(connPport)) > 0 ) {
						port = atoi(connPport);
						if ( port > 0 && port <= 8)
						{
							wifimgr_lib_set_WIFI_CMD("ra", port - 1, sizeof(aclCmd_switch), "AccessPolicy=2", aclCmd_switch, 0);
							snprintf(cmdValue, sizeof(cmdValue), "ACLAddEntry=\"%s\"", mac);
							wifimgr_lib_set_WIFI_CMD(0, port - 1, sizeof(aclCmd_list), mac, aclCmd_list, 0);
						}
						else if ( port > 8 && port <= 16)
						{
							wifimgr_lib_set_WIFI_CMD("rai", port - 9, sizeof(aclCmd_switch), "AccessPolicy=2", aclCmd_switch, 0);
							snprintf(cmdValue, sizeof(cmdValue), "ACLAddEntry=\"%s\"", mac);
							wifimgr_lib_set_WIFI_CMD("rai", port - 9, sizeof(aclCmd_list), mac, aclCmd_list, 0);
						}

						system(aclCmd_switch);
						system(aclCmd_list);
					}
				}
			}
#endif
		}
		
		if ( 1 == internet ) {
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A internet_chain -i br0 -m mac --mac-source %s -j DROP 2>/dev/null", mac);
			system(cmd);

			if (internet_1_flag == 0) {
				snprintf(cmd, sizeof(cmd), "iptables -t filter -A FORWARD -j internet_chain 2>/dev/null");
				system(cmd);
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ip6tables -t filter -I FORWARD -j internet_chain 2>/dev/null"); /* chain need before FIREWALL_CHAIN or LAN_DROP_ICMPV6 */
				system(cmd);
				internet_1_flag = 1;
			}
			memset(sta_limit_cmd, 0, sizeof(sta_limit_cmd));
			snprintf(sta_limit_cmd, sizeof(sta_limit_cmd), "ebtables -A F_Station_Limit -o nas+ -s %s -j DROP 2>/dev/null", mac);
			system(sta_limit_cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A internet_chain -i br0 -o nas+ -m mac --mac-source %s -j DROP 2>/dev/null", mac);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A internet_chain -i br0 -o ppp+ -m mac --mac-source %s -j DROP 2>/dev/null", mac);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "ip6tables -t filter -A internet_chain -i br0 -o nas+ -m mac --mac-source %s -j DROP 2>/dev/null", mac);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "ip6tables -t filter -A internet_chain -i br0 -o ppp+ -m mac --mac-source %s -j DROP 2>/dev/null", mac);
			system(cmd);
		}

#if 0
		if(2 == internet) {	
			/* restore station */
			if ( cfg_obj_get_object_attr(path, "ConnectionType", 0, connType, sizeof(connType)) > 0 ) {
				type = atoi(connType);
				if ( type == 1 ) {
					if ( cfg_obj_get_object_attr(path, "Port", 0, connPport, sizeof(connPport)) > 0 ) {
						port = atoi(connPport);
						if ( port > 0 && port <= 8)
						{
							wifimgr_lib_set_WIFI_CMD("ra", port - 1, sizeof(aclCmd_switch), "AccessPolicy=0", aclCmd_switch, 0);
							snprintf(cmdValue, sizeof(cmdValue), "ACLAddEntry=\"%s\"", mac);
							wifimgr_lib_set_WIFI_CMD("ra", port - 1, sizeof(aclCmd_list), mac, aclCmd_list, 0);
						
						}
						else if ( port > 8 && port <= 16)
						{
							wifimgr_lib_set_WIFI_CMD("rai", port - 9, sizeof(aclCmd_switch), "AccessPolicy=0", aclCmd_switch, 0);
							snprintf(cmdValue, sizeof(cmdValue), "ACLAddEntry=\"%s\"", mac);
							wifimgr_lib_set_WIFI_CMD("rai", port - 9, sizeof(aclCmd_list), mac, aclCmd_list, 0);
						}
						system(aclCmd_switch);
						system(aclCmd_list);
					}
				}
			}
			}
#endif
	}

	return internet;
}

int set_storage_access_rule(char *path, char *mac)
{
	int storage = 1;
	char cmd[512] = {0};
	char storage_access[32] = {0};

	if (NULL == path || NULL == mac)
		return storage;
	
	/* storage access */
	if (cfg_obj_get_object_attr(path, "StorageAccess", 0, storage_access, sizeof(storage_access)) > 0) {
		storage = atoi(storage_access);
		if (0 == storage) {
			/* ftp, http, samba */
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A storage_chain -i br0 -m mac --mac-source %s -p TCP -m multiport --dport 21,445,139,389,901 -j DROP 2>/dev/null", mac);
			system(cmd);
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A storage_chain -i br0 -m mac --mac-source %s -p UDP -m multiport --dport 137,138 -j DROP 2>/dev/null", mac);
			system(cmd);
			if (storage_flag == 0) {
			snprintf(cmd, sizeof(cmd), "iptables -t filter -A INPUT -j storage_chain 2>/dev/null");
			system(cmd);
				storage_flag = 1;
			}
		}
	}

	return storage;
}
void init_lanhost2black_node(void)
{
	int i = 0;
	char nodeName[32] = {0}, policy[4] = {0};
	char mac_addr[20] = {0}, new_mac[20] = {0};
	char blackNode[32] = {0};
	
	for (i = 0; i < MAX_LANHOST2_ENTRY_NUM; i++ )
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), LANHOST2_ENTRY_NODE, i + 1);

		memset(policy, 0,  sizeof(policy));
		cfg_obj_get_object_attr(nodeName, "InternetAccess", 0, policy, sizeof(policy));
		if ( 0 == policy[0] || 0 != strcmp(policy, "0") )
			continue;
		
		memset(mac_addr, 0, sizeof(mac_addr));
		cfg_obj_get_object_attr(nodeName, "MAC", 0, mac_addr, sizeof(mac_addr));
		if ( 0 == mac_addr[0] )
			continue;
		
		memset(blackNode, 0, sizeof(blackNode));
		snprintf(blackNode, sizeof(blackNode), LANHOST2BLACK_ENTRY_NODE, i + 1);
		if ( cfg_query_object(blackNode, NULL, NULL) <= 0 )
			cfg_create_object(blackNode);

		memset(new_mac, 0, sizeof(new_mac));
		mac_add_dot(mac_addr, new_mac);
		cfg_set_object_attr(blackNode, "MAC", new_mac);
	}
	return;
}
int svc_other_handle_event_lanhost2_config(void)
{
	char lanhost_path[32] = {0};
	char value[32] = {0}, mac_addr[32] = {0}, new_mac[32] = {0};
	char control_status[32] = {0}, control_num[32] = {0}, host_num_buf[32] = {0};
	int i = 0, ctrl_status = 0, ctrl_num = 0, host_num = 0, internet = 0, storage = 0, bandwidth = 0;
	int bandwidth_lan_cnt = 0, bandwidth_wifi_cnt = 0;
	char cmd[128] = {0};
	char s_max_lan_host[32] = {0}, s_max_ctrl_lan_host[32] = {0};
	int max_lan_host = 64, max_ctrl_lan_host = 32;
	int ret = -1;
	char us_bandwidth[32] = {0}, ds_bandwidth[32] = {0}, conn_type[32] = {0};
	int us_bandwidth_val = -1, ds_bandwidth_val = -1, conn_type_val = -1;
	dev_bandwidth_tmp_t lan_mac_acl;
	int bandwidth_cnt = 0;
		
	strncpy(lanhost_path, LANHOST2_COMMON_NODE, sizeof(lanhost_path) - 1);
	if (cfg_obj_get_object_attr(lanhost_path, "EnableStats", 0, value, sizeof(value)) > 0) 
	{
		if (atoi(value) == 1) 
		{
			blapi_traffic_set_dev_acnt_enable(1,1);
		}
		else if (atoi(value) == 0) 
		{
			blapi_traffic_set_dev_acnt_enable(0,0);
		}
	}
#if !defined(TCSUPPORT_CUC)
	clean_access_rule();
#endif

	bzero(&lan_mac_acl, sizeof(dev_bandwidth_tmp_t));
	ret = blapi_traffic_set_wifi_dev_ratelimit_enable(0);

	snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_COMMON_NODE);
	cfg_obj_get_object_attr(lanhost_path, "LANHostMaxNumber", 0, 
		s_max_lan_host, sizeof(s_max_lan_host));
	cfg_obj_get_object_attr(lanhost_path, "ControlListMaxNumber", 0, 
		s_max_ctrl_lan_host, sizeof(s_max_ctrl_lan_host));
	if (s_max_lan_host[0] != '\0')
	{
		max_lan_host = atoi(s_max_lan_host);
	}
	if (s_max_ctrl_lan_host[0] != '\0')
	{
		max_ctrl_lan_host = atoi(s_max_ctrl_lan_host);
	}
	
	for (i = 0; i < max_lan_host; i++) {
		snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_ENTRY_NODE, i + 1);

		if ((cfg_obj_get_object_attr(lanhost_path, "MAC", 0, mac_addr, sizeof(mac_addr)) <= 0) || strlen(mac_addr) <= 0)
			continue;

		host_num++;
		mac_add_dot(mac_addr, new_mac);

		if (ctrl_num < max_ctrl_lan_host)
		{
#if !defined(TCSUPPORT_CUC)
			/* internet&storage access */
			internet = set_internet_access_rule(lanhost_path, new_mac);
			storage = set_storage_access_rule(lanhost_path, new_mac);
#endif		

			/* get usbandwidth and dsbandwidth */
			if (cfg_obj_get_object_attr(lanhost_path, "MaxUSBandwidth", 0, us_bandwidth, sizeof(us_bandwidth)) > 0) 
			{
				us_bandwidth_val = atoi(us_bandwidth);
			}
			
			if (cfg_obj_get_object_attr(lanhost_path, "MaxDSBandwidth", 0, ds_bandwidth, sizeof(ds_bandwidth)) > 0) 
			{
				ds_bandwidth_val = atoi(ds_bandwidth);
			}

			if ( (0 > us_bandwidth_val) && (0 > ds_bandwidth_val) ) 
			{	
				/* if the value of MaxUSBandwidth and MaxDSBandwidth is not set return directly */		
				continue;
			}
			else	
			{		
				/* if just MaxUSBandwidth or MaxDSBandwidth not set, set the value to 0 */		
				if ( 0 > us_bandwidth_val ) 		
					us_bandwidth_val = 0;		
				if ( 0 > ds_bandwidth_val ) 		
					ds_bandwidth_val = 0;	
			}

			/*get type of connection*/
			if (cfg_obj_get_object_attr(lanhost_path, "ConnectionType", 0, conn_type, sizeof(conn_type)) > 0) 
			{
				conn_type_val = atoi(conn_type);
			}

			if (bandwidth_cnt < 10) 
			{
				lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].us_queue_speed = us_bandwidth_val;
				lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].ds_queue_speed = ds_bandwidth_val;
				lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].conn_type = conn_type_val;


				sscanf(new_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[0]),
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[1]), 
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[2]), 
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[3]), 
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[4]), 
								&(lan_mac_acl.trtcm_acl_mac_table[bandwidth_cnt].dev_mac[5]));

				bandwidth_cnt = bandwidth_cnt + 1;
			}

			if (0 == internet || 1 == internet || storage == 0 || bandwidth == 1) {
				ctrl_status = 1;
				ctrl_num++;
			}
			else {
				ctrl_status = 0;
			}

			snprintf(control_status, sizeof(control_status), "%d", ctrl_status);
			cfg_obj_set_object_attr(lanhost_path, "ControlStatus", 0, control_status);

		}
	}

	snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_COMMON_NODE);
	if ( cfg_query_object(lanhost_path, NULL, NULL) < 0 )
		cfg_create_object(lanhost_path);	
	snprintf(control_num, sizeof(control_num), "%d", ctrl_num);
	cfg_obj_set_object_attr(lanhost_path, "ControlListNumber", 0, control_num);
	snprintf(host_num_buf, sizeof(host_num_buf), "%d", host_num);
	cfg_obj_set_object_attr(lanhost_path, "LANHostNumber", 0, host_num_buf);

	/*set bandwidth rule*/
	ret = blapi_traffic_set_dev_ratelimit_by_mac(&lan_mac_acl, bandwidth_cnt);

	return 0;
}


