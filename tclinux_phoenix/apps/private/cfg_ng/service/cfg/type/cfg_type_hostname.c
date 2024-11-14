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
#include <sys/time.h> 
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "cfg_romfile.h"
#include "utility.h"


#define MAX_HOST_NUM 64

static int cfg_type_hostname_func_commit(void)
{
	char nodeName[32] = {0};
	int i = 0, count = 0;
	char tmpMac[20]= {0};
	char totalNum[4] = {0};

	for(i = 0; i < MAX_HOST_NUM; i++)
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), HOSTNAME_ENTRY_NODE, (i+1));
		if( 0 < cfg_get_object_attr(nodeName, "Mac", tmpMac, sizeof(tmpMac)))
		{
			count++;
		}
	}
	
	bzero(totalNum, sizeof(totalNum));
	snprintf(totalNum, sizeof(totalNum), "%d", count);
	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), HOSTNAME_COMMON_NODE);
	cfg_set_object_attr(nodeName, "Num", totalNum);

	return 0;
}


static cfg_node_ops_t cfg_type_host_name_entry_entry_ops  = { 
	.get = cfg_type_default_func_get, 
	.set = cfg_type_default_func_set, 
	.create = cfg_type_default_func_create, 
	.delete = cfg_type_default_func_delete, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_hostname_func_commit 
}; 


static cfg_node_type_t cfg_type_host_name_entry = { 
	.name = "Entry", 
	.flag = CFG_TYPE_FLAG_MULTIPLE | MAX_HOST_NUM, 
	.parent = &cfg_type_host_name, 
	.ops = &cfg_type_host_name_entry_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_host_name_common_ops  = { 
	.get = cfg_type_default_func_get, 
	.set = cfg_type_default_func_set, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_hostname_func_commit 
}; 

static cfg_node_type_t cfg_type_host_name_common = { 
	.name = "Common", 
	.flag = 1, 
	.parent = &cfg_type_host_name, 
	.ops = &cfg_type_host_name_common_ops, 
}; 


static cfg_node_ops_t cfg_type_host_name_ops  = { 
	.set = cfg_type_default_func_set, 
	.get = cfg_type_default_func_get, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_hostname_func_commit 
}; 


static cfg_node_type_t* cfg_type_host_name_child[] = { 
	&cfg_type_host_name_entry, 
	&cfg_type_host_name_common, 
NULL 
}; 


cfg_node_type_t cfg_type_host_name = { 
	.name = "HostName", 
	.flag = 1 , 
	.parent = &cfg_type_root, 
	.nsubtype = sizeof(cfg_type_host_name_child) / sizeof(cfg_node_type_t*) - 1 , 
	.subtype = cfg_type_host_name_child, 
	.ops = &cfg_type_host_name_ops, 
}; 

