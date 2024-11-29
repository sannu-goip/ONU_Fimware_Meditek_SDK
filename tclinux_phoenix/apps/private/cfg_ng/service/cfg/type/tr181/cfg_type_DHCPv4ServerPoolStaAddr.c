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


static char* cfg_type_dhcpv4svrPoolStaAddr_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


int cfg_type_dhcpv4svrPoolStaAddr_func_commit(char *path)
{
	char tmp[64] = {0};
	char tr181_dhcpv4sps[MAXLEN_NODE_NAME]={0};	
	char node[MAXLEN_NODE_NAME]={0};
	int etyIdx=0;

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		snprintf(node, sizeof(node), WEBCURSET_ENTRY_NODE);
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(node, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp);
		}else{
			return -1;
		}
	}

	memset(tr181_dhcpv4sps,0,sizeof(tr181_dhcpv4sps));
	snprintf(tr181_dhcpv4sps,sizeof(tr181_dhcpv4sps),TR181_DHCPV4_SVRPOLSTAADRC_ENTRY_NODE,etyIdx);
	
	if(!((etyIdx-1)/DHCPV4_STA_NUM_PER_POOL))
	{	/*Dhcpd_Entry*/
		memset(node,0,sizeof(node));
		snprintf(node,sizeof(node), DHCPD_ENTRY_NODE, (etyIdx-1)%DHCPV4_STA_NUM_PER_POOL + 1);

		memset(tmp,0,sizeof(tmp));
		cfg_obj_get_object_attr(tr181_dhcpv4sps, "Chaddr" , 0, tmp,sizeof(tmp));
		cfg_obj_set_object_attr(node, "MAC", 0, tmp);

		memset(tmp,0,sizeof(tmp));
		cfg_obj_get_object_attr(tr181_dhcpv4sps, "Yiaddr", 0, tmp,sizeof(tmp));
		cfg_obj_set_object_attr(node, "IP", 0, tmp);

		memset(tmp,0,sizeof(tmp));
		cfg_obj_get_object_attr(tr181_dhcpv4sps, "Enable", 0, tmp,sizeof(tmp));
		cfg_obj_set_object_attr(node, "Enable", 0, tmp);

		cfg_tr181_commit_object(DHCPD_NODE);
	}

	return 0;	
}



static cfg_node_ops_t cfg_type_dhcpv4svrPoolStaAddr_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4svrPoolStaAddr_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV4_SPS_NUM_MAX, 
	 .parent = &cfg_type_dhcpv4svrPoolStaAddr, 
	 .index = cfg_type_dhcpv4svrPoolStaAddr_index, 
	 .ops = &cfg_type_dhcpv4svrPoolStaAddr_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv4svrPoolStaAddr_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4svrPoolStaAddr_child[] = { 
	 &cfg_type_dhcpv4svrPoolStaAddr_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4svrPoolStaAddr= { 
	 .name = "DHCPv4SPS", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4svrPoolStaAddr_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4svrPoolStaAddr_child, 
	 .ops = &cfg_type_dhcpv4svrPoolStaAddr_ops, 
}; 
