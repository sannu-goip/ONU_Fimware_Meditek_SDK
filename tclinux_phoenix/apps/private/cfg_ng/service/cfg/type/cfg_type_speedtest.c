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
#include "cfg_cli.h"
#include <svchost_evt.h>
#include "cfg_types.h" 

int svc_cfg_speedtest_boot(void)
{
	return 0;
}

int cfg_type_speedtest_func_commit(char* path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_SPEEDTEST_START, (void*)&param, sizeof(param));
	return 0;
}

#if defined(TCSUPPORT_CUC)
static cfg_node_ops_t cfg_type_speedtest_info_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
};

static cfg_node_type_t cfg_type_speedtest_info = { 
	 .name = "Info", 
	 .flag = 1, 
	 .parent = &cfg_type_speedtest, 
	 .ops = &cfg_type_speedtest_info_ops, 
}; 

static cfg_node_ops_t cfg_type_speedtest_httpentry_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_speedtest_func_commit 
};

static cfg_node_type_t cfg_type_speedtest_httpentry = { 
	 .name = "HttpEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_speedtest, 
	 .ops = &cfg_type_speedtest_httpentry_ops, 
}; 


static cfg_node_ops_t cfg_type_speedtest_rmsentry_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_speedtest_func_commit 
};

static cfg_node_type_t cfg_type_speedtest_rmsentry = { 
	 .name = "RMSEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_speedtest, 
	 .ops = &cfg_type_speedtest_rmsentry_ops, 
}; 

#else
static cfg_node_ops_t cfg_type_speedtest_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_speedtest_func_commit 
}; 

/*
Entry image:
DestIP:192.168.40.200
DestPort:80
FileName:
*/
static cfg_node_type_t cfg_type_speedtest_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 17, /* 0~3:http, 4:ftp, 5:postman, */
	 .parent = &cfg_type_speedtest, 
	 .ops = &cfg_type_speedtest_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_speedtest_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_speedtest_func_commit 
}; 

/*
Common image:
ActionType:http/ftp/postman
DbusIF:0 - HttpDownloadTest, 1 - SpeedTestFF

Status:0 - success, 1 - fail
Result:Kbyte
TestingStatus:true - is testing, false - test end
Ticket:
SpeedType:1 - down speedtest, 2 - up speedtest, 3 - down/up speedtest
DW_SPEED_MAX_RATE:
DW_SPEED_RATE:
UP_SPEED_MAX_RATE:
UP_SPEED_RATE:
BEGIN_TIME:
END_TIME:
*/

static cfg_node_type_t cfg_type_speedtest_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_speedtest, 
	 .ops = &cfg_type_speedtest_common_ops, 
}; 

#endif

static cfg_node_ops_t cfg_type_speedtest_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_speedtest_func_commit 
}; 


static cfg_node_type_t* cfg_type_speedtest_child[] = { 
#if defined(TCSUPPORT_CUC)
	 &cfg_type_speedtest_info, 
	 &cfg_type_speedtest_httpentry, 
	 &cfg_type_speedtest_rmsentry, 
#else
	 &cfg_type_speedtest_entry, 
	 &cfg_type_speedtest_common, 
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_speedtest = { 
	 .name = "SpeedTest", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_speedtest_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_speedtest_child, 
	 .ops = &cfg_type_speedtest_ops, 
}; 

