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
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <libapi_lib_wifimgr.h>
#include "cfg_type_info.h"
#include "../../wlan/wlan_cfg.h"
#include "../../wlan/wlan_mgr.h"
#include "../cfg/utility.h"
#include "../cfg/cfg_msg.h"
#include <cfg_msg.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"



int cfg_type_macblacklist_execute(void)
{
	return 0;
}

int getBlockTime(char *MacAddr, int mode, int maxNum, unsigned long *blocktime)
{
	int  k = 0;
	char nodeName[32] = {0};
	char AccessPolicy[4] = {0};
	RT_802_11_ACL * AclInfor= NULL;
	char AclList[WLAN_MAC_NUM][18];
	int i = 0;
	char tmpName[32] = {0};
	
	if (WLAN_MODE == mode) {
		snprintf(tmpName, sizeof(tmpName), WLAN_ENTRY_NODE);
	}
	else if (WLAN11AC_MODE == mode) {
		snprintf(tmpName, sizeof(tmpName), WLAN11AC_ENTRY_NODE);
	}
	else {
		return -1;
	}

	AclInfor = (RT_802_11_ACL *)malloc(sizeof(RT_802_11_ACL));
	if (!AclInfor)
		return -1;
	
	for (k = 0; k < maxNum; k++) {
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.%d", tmpName, k + 1);
	
		if (0 > cfg_obj_get_object_attr(nodeName, "AccessPolicy", 0, AccessPolicy, sizeof(AccessPolicy))
			|| '\0' == AccessPolicy[0] || strcmp(AccessPolicy, "2")) {
			continue;
		}

		memset(AclInfor, 0, sizeof(RT_802_11_ACL));
		wifimgr_lib_get_WPS_ACL_INFO(mode, k, AclInfor);
		for (i = 0; i < WLAN_MAC_NUM; i++) {
			snprintf(AclList[i], sizeof(AclList[i]), "%02x:%02x:%02x:%02x:%02x:%02x",
					AclInfor->Entry[i].Addr[0],
					AclInfor->Entry[i].Addr[1],
					AclInfor->Entry[i].Addr[2],
					AclInfor->Entry[i].Addr[3],
					AclInfor->Entry[i].Addr[4],
					AclInfor->Entry[i].Addr[5]);
			
			if (strcmp(AclList[i], "00:00:00:00:00:00")) {
				if(!strcasecmp(MacAddr, AclList[i])){
					*blocktime +=  AclInfor->Entry[i].Reject_Count;					
					break;
				}
			}
		}
	}
	
	if (AclInfor)
	{
		free(AclInfor);
		AclInfor = NULL;
	}
	return 0;
}

static int cfg_type_macblack_entry_func_get(char* path,char* attr, char* val,int len)
{
	int macblacklist_id = 0, maxNum = 0;
	char tmp[4] = {0};
	char nodeName[32] = {0};
	char MacAddr[20] = {0};
	char BSSIDNum[4] = {0};
	unsigned long blocktime = 0;
	char tmpvalue[8] = {0};

	if (0 >= cfg_query_object(path, NULL, NULL)) {
		/*printf(" %s is not exist!!\n", path);*/
		return -1;
	}
	if (cfg_obj_get_object_attr(path, "MAC", 0, MacAddr, sizeof(MacAddr)) < 0) {
		return -1;
	}

	/* 2.4G */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
	cfg_obj_get_object_attr(nodeName, "BssidNum", 0, BSSIDNum, sizeof(BSSIDNum));
	maxNum = atoi(BSSIDNum);
	getBlockTime(MacAddr, WLAN_MODE, maxNum, &blocktime);
	if(0 == blocktime)
	{
#if defined(TCSUPPORT_WLAN_AC)
	/* 5G */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
	cfg_obj_get_object_attr(nodeName, "BssidNum", 0, BSSIDNum, sizeof(BSSIDNum));
	maxNum = atoi(BSSIDNum);
	getBlockTime(MacAddr, WLAN11AC_MODE, maxNum, &blocktime);
#endif
	}
	
	snprintf(tmpvalue, sizeof(tmpvalue), "%lu", blocktime);
	cfg_set_object_attr(path, "BlockTime", tmpvalue);
	
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_macblacklist_func_commit(char* path)
{
	wlan_evt_t param;
	
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_set_object_attr(MACWHITELIST_COMMON_NODE, "MACPolicy", 0, "2");
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_MACBLACKLIST_UPDATE, (void*)&param, sizeof(param));	

	return 0;
}

static cfg_node_ops_t cfg_type_macblacklist_entry_ops  = { 
	 .get = cfg_type_macblack_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_macblacklist_func_commit 
}; 


static cfg_node_type_t cfg_type_macblacklist_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 8, 
	 .parent = &cfg_type_macblacklist, 
	 .index = NULL, 
	 .ops = &cfg_type_macblacklist_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_macblacklist_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_macblacklist_func_commit 
}; 


static cfg_node_type_t cfg_type_macblacklist_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_macblacklist, 
	 .ops = &cfg_type_macblacklist_common_ops, 
}; 


static cfg_node_ops_t cfg_type_macblacklist_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_macblacklist_func_commit 
}; 


static cfg_node_type_t* cfg_type_macblacklist_child[] = { 
	 &cfg_type_macblacklist_entry, 
	 &cfg_type_macblacklist_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_macblacklist = { 
	 .name = "MacBlackList", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_macblacklist_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_macblacklist_child, 
	 .ops = &cfg_type_macblacklist_ops, 
}; 
