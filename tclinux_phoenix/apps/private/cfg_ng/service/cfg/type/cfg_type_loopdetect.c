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



#define LOOPDETECT_CMD 			"echo %d %s > /proc/tc3162/loop_detect"
#define LOOP_ENABLE             		0
#define LOOP_EXIST_PERIOD     		1
#define LOOP_CANCEL_PERIOD      		2
#define LOOP_VLANTAG            		3
#define LOOP_ETHERNET_TYPE      		4
#define LOOP_DETECT_EANBLE_PORT 	5
#define LOOP_AUTO_SHUT_LAN      	6


int svc_cfg_boot_loopdetect(void)
{
	char loopdetectbuf[260]={0};
	char valuebuf[256] = {0};
	char nodePath[64] = {0};
		
	snprintf(nodePath, sizeof(nodePath), LOOPDETECT_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodePath, "LoopExistPeriod", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_EXIST_PERIOD, valuebuf);
	system_escape(loopdetectbuf);	

	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "LoopCancelPeriod", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_CANCEL_PERIOD, valuebuf);
	system_escape(loopdetectbuf);	

	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "VlanTag", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_VLANTAG, valuebuf);
	system_escape(loopdetectbuf);		

	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "EthernetType", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_ETHERNET_TYPE, valuebuf);
	system_escape(loopdetectbuf);

	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "Enable", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_ENABLE, valuebuf);
	system_escape(loopdetectbuf);
	
#if defined(TCSUPPORT_CUC)
	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "LoopDetectMask", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_DETECT_EANBLE_PORT, valuebuf);
	system_escape(loopdetectbuf);
	
	memset(valuebuf, 0, sizeof(valuebuf));
	memset(loopdetectbuf, 0, sizeof(loopdetectbuf));
	if(cfg_obj_get_object_attr(nodePath, "LoopAutoShutMask", 0, valuebuf, sizeof(valuebuf)) < 0)
	{
		return FAIL;
	}
	snprintf(loopdetectbuf, sizeof(loopdetectbuf), LOOPDETECT_CMD, LOOP_AUTO_SHUT_LAN, valuebuf);
	system_escape(loopdetectbuf);

#endif

	return 0;
}

int cfg_type_loopdetect_execute(void)
{
	return svc_cfg_boot_loopdetect( );
}

int cfg_type_loopdetect_func_commit(char* path)
{
	return cfg_type_loopdetect_execute( );
}

static cfg_node_ops_t cfg_type_loopdetect_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_loopdetect_func_commit 
}; 


static cfg_node_type_t cfg_type_loopdetect_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_loopdetect, 
	 .ops = &cfg_type_loopdetect_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_loopdetect_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_loopdetect_func_commit 
}; 


static cfg_node_type_t* cfg_type_loopdetect_child[] = { 
	 &cfg_type_loopdetect_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_loopdetect = { 
	 .name = "LoopDetect", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_loopdetect_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_loopdetect_child, 
	 .ops = &cfg_type_loopdetect_ops, 
}; 
