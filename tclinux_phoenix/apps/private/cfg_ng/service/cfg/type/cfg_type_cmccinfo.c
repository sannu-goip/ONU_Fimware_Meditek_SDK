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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "svchost_evt.h"
#include "utility.h"

#define CMCCINFO_MAX_NUM 4

int svc_cfg_cmcc_info_boot(void)
{
	char nodeName[32];
	char corkmark_test[12];
	char hwnat_set[12];

	bzero(nodeName, sizeof(nodeName));
	bzero(corkmark_test, sizeof(corkmark_test));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", CMCCINFO_ENTRY_NODE, 4);
	
	if(0 < cfg_get_object_attr(nodeName, "coremarktest", corkmark_test, sizeof(corkmark_test)) \
		&& 0 == strcmp(corkmark_test, "Yes"))
	{
		doValPut("/tmp/coremark_optimization", "coremark_optimization\n");
	}

	bzero(hwnat_set, sizeof(hwnat_set));
	if(0 < cfg_get_object_attr(nodeName, "hwnat", hwnat_set, sizeof(hwnat_set)) \
		&& 0 == strcmp(hwnat_set, "Yes"))
	{
		system("/userfs/bin/hw_nat -N 1");
		system("/userfs/bin/hw_nat -U 300 300 300 300");
	}

	return 0;

}


static cfg_node_ops_t cfg_type_cmcc_info_entry_ops  = { 
	.get = cfg_type_default_func_get, 
	.set = cfg_type_default_func_set, 
	.create = cfg_type_default_func_create, 
	.delete = cfg_type_default_func_delete, 
	.commit = cfg_type_default_func_commit,
	.query = cfg_type_default_func_query 
}; 

static cfg_node_type_t cfg_type_cmcc_info_entry = { 
	.name = "Entry", 
	.flag =  CFG_TYPE_FLAG_MULTIPLE | CMCCINFO_MAX_NUM , 
	.parent = &cfg_type_cmcc_info, 
	.ops = &cfg_type_cmcc_info_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_cmcc_info_ops  = { 
	.set = cfg_type_default_func_set, 
	.get = cfg_type_default_func_get, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_default_func_commit 
};


static cfg_node_type_t* cfg_type_cmcc_info_child[] = { 
	&cfg_type_cmcc_info_entry, 
	NULL 
}; 

cfg_node_type_t cfg_type_cmcc_info = { 
	.name = "CMCCInfo", 
	.flag =  1 , 
	.parent = &cfg_type_root, 
	.nsubtype = sizeof(cfg_type_cmcc_info_child) / sizeof(cfg_node_type_t*) - 1 , 
	.subtype = cfg_type_cmcc_info_child, 
	.ops = &cfg_type_cmcc_info_ops, 
}; 

