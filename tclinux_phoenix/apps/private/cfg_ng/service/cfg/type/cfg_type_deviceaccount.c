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
#include <time.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <unistd.h>
#include "utility.h"
#include <svchost_evt.h>

static int cfg_type_deviceaccount_func_read(char* path,char* attr, char* val,int len)
{
	char nodeName[64];
	char tmpValue[8] = {0};
	char webcommitflag[8] = {0};
	int retryTimes = 0, retryLimit = 0, recTime = 0, nowTime = 0;

	if(cfg_query_object(path,NULL,NULL) <= 0){
		return FAIL;
	}

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, DEVICEACCOUNT_ENTRY_NODE, sizeof(nodeName)-1);

	if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "retryTimesCommit", 0, webcommitflag, sizeof(webcommitflag)) > 0
		&& !strcmp(webcommitflag, "1"))
	{
		return SUCCESS;
	}	

	if(attr != NULL){
		if(strcmp(attr,"retryTimes") == 0){
			memset(tmpValue, 0, sizeof(tmpValue));
			if(cfg_obj_get_object_attr(nodeName, "retryTimes", 0, tmpValue, sizeof(tmpValue)) < 0 ){
				return FAIL;
			}	

			retryTimes = atoi(tmpValue);
			memset(tmpValue, 0, sizeof(tmpValue));
			if(cfg_obj_get_object_attr(nodeName, "retryLimit", 0, tmpValue, sizeof(tmpValue)) < 0 ){
				return FAIL;
			}	

			retryLimit = atoi(tmpValue);
			if(retryTimes >= retryLimit )
            {
	            struct timespec t1;
            	memset(tmpValue, 0, sizeof(tmpValue));
				if(cfg_obj_get_object_attr(nodeName, "recTime", 0, tmpValue, sizeof(tmpValue)) < 0 ){
					return FAIL;
				}

				recTime = atoi(tmpValue);
				clock_gettime(CLOCK_MONOTONIC, &t1);
				nowTime = t1.tv_sec;

				if(nowTime - recTime > 180)
				{
					cfg_obj_set_object_attr(nodeName, "retryTimes", 0, "0");
				}
			}	
		}
	}

	return SUCCESS;

}

int svc_cfg_boot_deviceaccount(void)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DEVICE_ACCOUNT_BOOT, NULL, 0); 
	return 0;
}


static int cfg_type_deviceaccount_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_deviceaccount_func_read(path,attr,val,len);
	cfg_type_default_func_get(path,attr,val,len); 
	return 0;
} 


static int cfg_type_deviceaccount_func_commit(char* path) 
{ 
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_DEVICE_ACCOUNT_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
} 


static cfg_node_ops_t cfg_type_deviceaccount_entry_ops  = { 
	 .get = cfg_type_deviceaccount_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_deviceaccount_func_commit 
}; 


static cfg_node_type_t cfg_type_deviceaccount_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_deviceaccount, 
	 .ops = &cfg_type_deviceaccount_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_deviceaccount_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_deviceaccount_func_commit 
}; 


static cfg_node_type_t* cfg_type_deviceaccount_child[] = { 
	 &cfg_type_deviceaccount_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_deviceaccount = { 
	 .name = "deviceAccount", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_deviceaccount_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_deviceaccount_child, 
	 .ops = &cfg_type_deviceaccount_ops, 
}; 
