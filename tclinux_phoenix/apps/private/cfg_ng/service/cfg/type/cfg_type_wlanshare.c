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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#else
#define MAX_BSSID_NUM 4
#endif
#include <libapi_lib_wifimgr.h>

#define ACTIVE_YES "Yes"

#define GSSIDINFO 		"GuestSSIDInfo"
#define GSSIDENABLE	"EnableSSID"
#define GSSIDDURATION	"Duration"
#define GSSIDPORTISOL	"PortIsolation"
#define GUESTSSIDINFO_MAX_TIME 365*24*60*60
#define WLAN_ENTRY_ATTR_NUM 4
#define WLAN_COMMON_ATTR_NUM 3
#define WLAN_ENTRY_TYPE 1
#define WLAN_COMMON_TYPE 2


int  setshareconfig(int actType, int index, char *enableUserId, char *userId, 
					char *staIsolation, char *portIsolation)
{
	char cmd[64] = {'\0'};
	char cmdValue[32] = {0};
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_get_WLANSHARE_CMD(cmd, actType, index - 1, enableUserId, userId);
	system_escape(cmd);

	if (index > MAX_BSSID_NUM) { /*5G*/		
		/*staIsolation*/
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmdValue, sizeof(cmdValue), "NoForwarding=%s", staIsolation);
		wifimgr_lib_set_WIFI_CMD("rai", index - 5, sizeof(cmdValue), staIsolation, cmd, 0);
		system_escape(cmd);

		/*portIsolation*/	
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd),
			"/usr/bin/portbindcmd portisolation %d %d %s", 
			actType, index-5, portIsolation);
		system_escape(cmd);
	}
	else {/*2.4G*/
		/*staIsolation*/
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmdValue, sizeof(cmdValue), "NoForwarding=%s", staIsolation);
		wifimgr_lib_set_WIFI_CMD("ra", index - 1, sizeof(cmdValue), staIsolation, cmd, 0);
		system_escape(cmd);
		/*portIsolation*/	
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd),
			"/usr/bin/portbindcmd portisolation %d %d %s", 
			actType, index-1, portIsolation);
		system_escape(cmd);
	}
	return 0;
}


int wlanshare_configure()
{
	char nodeName[64]={'\0'};
	char active[8] = {'\0'};
	char ssidIndex[8] = {'\0'};
	char portIsolation[8] = {'\0'};
	char staIsolation[8] = {'\0'};
	char enableUserId[8] = {'\0'};
	char userId[32] = {'\0'};
	int index = 0, i = 0, actType = 0, oldssid = -1;
	char buf[8] = {'\0'};
	
	for(i = 1; i <= WLANSHARE_INSTANCE_NUM; i++){
		snprintf(nodeName,sizeof(nodeName),WLANSHARE_ENTRY_NODE, i);
		
		/*SSIDIndex*/
		memset(ssidIndex, 0, sizeof(ssidIndex));
		if(cfg_obj_get_object_attr(nodeName, "SSIDIndex", 0, ssidIndex, sizeof(ssidIndex)) < 0){
			/*ssidindex is null*/
			continue;
		}
		index = atoi(ssidIndex);
		if (checkconflictssid(SHARE_TYPE, index) == -1) {
			printf("wlanshare_configure:ssid %d is guest ssid\n", index);
			continue;
		}
		if(index > MAX_BSSID_NUM*2 || index <= 0)
			continue;
		
		/*delete old portIsolation and staIsolation of this entry*/
		if (cfg_obj_get_object_attr(nodeName, "oldSSIDIndex", 0, buf, sizeof(buf)) > 0) {
			oldssid = atoi(buf);
			if (oldssid != -1) {
				setshareconfig(0, oldssid, "0", "0", "0", "0");
			}
		}
		cfg_set_object_attr(nodeName, "oldSSIDIndex", "-1");

		/*PortIsolation*/
		memset(portIsolation, 0, sizeof(portIsolation));
		if(cfg_obj_get_object_attr(nodeName, "PortIsolation", 0, portIsolation, sizeof(portIsolation)) < 0){
			/*ssidindex is null*/
			portIsolation[0] = '0';
		}

		/*STAIsolation*/
		memset(staIsolation, 0, sizeof(staIsolation));
		if(cfg_obj_get_object_attr(nodeName, "STAIsolation", 0, staIsolation, sizeof(staIsolation)) < 0){
			/*ssidindex is null*/
			staIsolation[0] = '0';
		}

		/*EnableUserId*/
		memset(enableUserId, 0, sizeof(enableUserId));
		if(cfg_obj_get_object_attr(nodeName, "EnableUserId", 0, enableUserId, sizeof(enableUserId)) < 0){
			/*ssidindex is null*/
			enableUserId[0] = '0';
		}
		
		/*UserId*/
		memset(userId, 0, sizeof(userId));
		if(cfg_obj_get_object_attr(nodeName, "UserId", 0, userId, sizeof(userId)) > 0){ /*if UserId error , do nothing*/
			memset(active, 0, sizeof(active));
			cfg_obj_get_object_attr(nodeName, "Active", 0, active, sizeof(active));
			if(strncmp(active, ACTIVE_YES, sizeof(ACTIVE_YES) - 1) == 0){
				actType = 1;
			}
			else {
				actType = 0;
			}
			setshareconfig(actType, index, enableUserId, userId, staIsolation, portIsolation);
		}
	}
	return 0;
}


int cfg_type_wlanshare_func_write()
{
	return 0;
}


int cfg_type_wlanshare_func_execute()
{
	wlanshare_configure();
	return 0;
}


int svc_cfg_boot_wlanshare()
{
	system("/usr/bin/ebtables -t filter -N WlanShare");
	system("/usr/bin/ebtables -t filter -P WlanShare RETURN");
	system("/usr/bin/ebtables -A INPUT -j WlanShare");
	
	wlanshare_configure();
	return 0;
}


static int cfg_type_wlanshare_func_commit(char* path)
{
	cfg_type_wlanshare_func_write();
	cfg_type_wlanshare_func_execute();
	return 0;
}



static cfg_node_ops_t cfg_type_wlanshare_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlanshare_func_commit 
}; 


static cfg_node_type_t cfg_type_wlanshare_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | WLANSHARE_INSTANCE_NUM, 
	 .parent = &cfg_type_wlanshare, 
	 .ops = &cfg_type_wlanshare_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_wlanshare_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlanshare_func_commit 
}; 


static cfg_node_type_t* cfg_type_wlanshare_child[] = { 
	 &cfg_type_wlanshare_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_wlanshare = { 
	 .name = "WlanShare", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_wlanshare_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wlanshare_child, 
	 .ops = &cfg_type_wlanshare_ops, 
}; 
