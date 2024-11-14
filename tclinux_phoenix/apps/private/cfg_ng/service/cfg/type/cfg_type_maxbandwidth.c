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
#include "blapi_traffic.h"

static char* cfg_type_maxbandwidth_index[] = { 
	 NULL 
}; 

int devMaxBandwidth_configure(char* path)
{
	int i = 0;
	int dslflag = 0;
	int isRuleExist = -1;
	char nodeName[64] = {0};	
	char dslimitDownMode[8] = {0};
	char buf[64] = {0};
	struct MaxBandWidth_node_info_s MaxBandWidthNodeInfo[TRTCM_MATCH_MAC_RULE_NUM];

	for(i = 0; i < TRTCM_MATCH_MAC_RULE_NUM; i++)
{
		bzero(nodeName, sizeof(nodeName));
		bzero(MaxBandWidthNodeInfo[i].macAddr, sizeof(MaxBandWidthNodeInfo[i].macAddr));
		bzero(MaxBandWidthNodeInfo[i].upRate, sizeof(MaxBandWidthNodeInfo[i].upRate));
		bzero(MaxBandWidthNodeInfo[i].downRate, sizeof(MaxBandWidthNodeInfo[i].downRate));
		snprintf(nodeName,sizeof(nodeName), MAXBANDWIDTH_ENTRY_NODE,i+1);

		bzero(buf, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "mac", 0, buf, sizeof(buf)) > 0 &&  '\0' != buf[0]){
			strncpy(MaxBandWidthNodeInfo[i].macAddr, buf, sizeof(MaxBandWidthNodeInfo[i].macAddr)-1);
				}
		else{
				continue;
			}
		
		bzero(buf, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "upRate", 0, buf, sizeof(buf)) > 0 && '\0' != buf[0]){
			strncpy(MaxBandWidthNodeInfo[i].upRate, buf, sizeof(MaxBandWidthNodeInfo[i].upRate)-1);
				}
		else{
				continue;
			}

		bzero(buf, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "downRate", 0, buf, sizeof(buf)) > 0 && '\0' != buf[0]){
			strncpy(MaxBandWidthNodeInfo[i].downRate, buf, sizeof(MaxBandWidthNodeInfo[i].downRate)-1);
		}
		else{
			continue;
		}	
	}
	
#if defined(TCSUPPORT_CT_DS_LIMIT)
	if(cfg_obj_get_object_attr(DATASPEEDLIMIT_ENTRY_NODE, "SpeedLimitModeDOWN", 0, dslimitDownMode, sizeof(dslimitDownMode)) > 0)
	{
		dslflag = atoi(dslimitDownMode);	
	}
#endif
	
	 blapi_traffic_set_max_bandwidth_data(dslflag, MaxBandWidthNodeInfo, &isRuleExist);	
		
#if defined(TCSUPPORT_CT_DS_LIMIT)
	if(isRuleExist == 0 && dslflag > 0)
	{
		cfg_obj_commit_object(DATASPEEDLIMIT_ENTRY_NODE);
	}

#endif	
	 
	return SUCCESS;
}

int cfg_type_MaxBandWidth_func_commit(char* path)
{
	devMaxBandwidth_configure(path);
	return 0;
}

static cfg_node_ops_t cfg_type_maxbandwidth_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_MaxBandWidth_func_commit 
}; 


static cfg_node_type_t cfg_type_maxbandwidth_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 10, 
	 .parent = &cfg_type_maxbandwidth, 
	 .index = cfg_type_maxbandwidth_index, 
	 .ops = &cfg_type_maxbandwidth_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_maxbandwidth_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_MaxBandWidth_func_commit 
}; 


static cfg_node_type_t* cfg_type_maxbandwidth_child[] = { 
	 &cfg_type_maxbandwidth_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_maxbandwidth = { 
	 .name = "MaxBandWidth", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_maxbandwidth_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_maxbandwidth_child, 
	 .ops = &cfg_type_maxbandwidth_ops, 
}; 
