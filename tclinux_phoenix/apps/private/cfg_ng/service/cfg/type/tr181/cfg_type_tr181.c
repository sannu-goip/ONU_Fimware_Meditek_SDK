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

void svc_cfg_boot_tr181(void)
{
	char buf[64] = {0};
	int ret = -1;
		
	if(cfg_obj_get_object_attr(TR181_COMMON_NODE, TR181_SYNCFLAG_ATTR , 0, buf,sizeof(buf)) < 0 || strcmp(buf,"100") == 0)
	{
		ret = updateTR181AllInterface();
		if(ret < 0){
			return FAIL;
		}
		cfg_obj_set_object_attr(TR181_COMMON_NODE, TR181_SYNCFLAG_ATTR , 0, "1");
	}

	return SUCCESS;
}

void checkAndCommitWanByTR181(void)
{
	char buf[64] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanMaskStr[16] = {0};	
	int wanMask0 = 0;
	int wanMask1 = 0;
	int pvcIdx = -1;
	int EtyIdx = -1;
	int wanIndex = -1;

	if(cfg_obj_get_object_attr(TR181_COMMON_NODE, "commitFormTR69" , 0, buf,sizeof(buf)) >= 0 &&  strcmp(buf,"1") == 0)
	{			
		memset(wanMaskStr, 0, sizeof(wanMaskStr));
		cfg_obj_get_object_attr(TR181_COMMON_NODE, TR181_WAN_MASK0_ATTR , 0,wanMaskStr,sizeof(wanMaskStr)); 
		wanMask0 = atoi(wanMaskStr);
		
		memset(wanMaskStr, 0, sizeof(wanMaskStr));
		cfg_obj_get_object_attr(TR181_COMMON_NODE, TR181_WAN_MASK1_ATTR , 0,wanMaskStr,sizeof(wanMaskStr)); 
		wanMask1 = atoi(wanMaskStr);
		
		for(pvcIdx = 0; pvcIdx < PVC_NUM; pvcIdx++ )
		{
			for(EtyIdx = 0; EtyIdx < MAX_SMUX_NUM; EtyIdx++ )
			{
				wanIndex = pvcIdx*PVC_NUM + EtyIdx;
				if(wanIndex >= MAX_WAN_IF_INDEX/2)
				{
					if(0 == (wanMask1 & (1 << (wanIndex - MAX_WAN_IF_INDEX/2))))
					{
						continue;
					}
				}
				else
				{
					if(0 == (wanMask0 & (1 << wanIndex)))
					{
						continue;
					}
				}

				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "root.wan.pvc.%d.entry.%d", pvcIdx + 1,EtyIdx + 1);
				cfg_tr181_commit_object(nodeName);
			}
		}
		cfg_obj_set_object_attr(TR181_COMMON_NODE, TR181_WAN_MASK0_ATTR , 0, "0");
		cfg_obj_set_object_attr(TR181_COMMON_NODE, TR181_WAN_MASK1_ATTR , 0, "0");
		
		cfg_obj_set_object_attr(TR181_COMMON_NODE, "commitFormTR69" , 0, "0"); 
		
		return 0;
	}

}



int cfg_type_tr181_func_commit(void)
{
	char buf[64] = {0};

	checkAndCommitWanByTR181();

	if(cfg_obj_get_object_attr(TR181_COMMON_NODE, "syncFlag", 0, buf, sizeof(buf)) < 0 || strcmp(buf,"100") == 0)
	{	
		updateTR181AllInterface();
		cfg_obj_set_object_attr(TR181_COMMON_NODE, "syncFlag" , 0, "1");		/*already sync*/
		tcdbg_printf("tr181_execute:updateTR181AllInterface\n");	
		return 0;
	}
	
	if(strcmp(buf,"0") == 0)
	{	
		tcdbg_printf("tr181_execute:clearInterfaceNode\n");		
		clearAllInterfaceNode();
		cfg_obj_set_object_attr(TR181_COMMON_NODE, "syncFlag" , 0, "1");		/*already sync*/	
		return 0;
	}

	if(strcmp(buf,"2") == 0)
	{
		tcdbg_printf("tr181_execute:updateTR181AllWanInterface\n"); 	
		updateTR181AllWanInterface();
		cfg_obj_set_object_attr(TR181_COMMON_NODE, "syncFlag" , 0, "1");		/*already sync*/		
		return 0;
	}
	
	return 0;
}



static cfg_node_ops_t cfg_type_tr181_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181, 
	 .ops = &cfg_type_tr181_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181_child[] = { 
	 &cfg_type_tr181_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181 = { 
	 .name = "TR181", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181_child, 
	 .ops = &cfg_type_tr181_ops, 
}; 
