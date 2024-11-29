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

char QosAppList[][8] = {
	{"IGMP"},
	{"SIP"},
	{"H.323"},
	{"MGCP"},
	{"SNMP"},
	{"DNS"},
	{"DHCP"},
	{"RIP"},
	{"RSTP"},
	{"RTCP"},
	{"RTP"}
};

#define QOS_APP_NUM_MAX sizeof(QosAppList) / sizeof(QosAppList[0])

int clearQosApp()
{
	int i = 0;
	char tr181node[MAXLEN_NODE_NAME] = {0};	

	for(i = 0; i < QOS_APP_NUM_MAX ; i++)
	{
		memset(tr181node, 0, sizeof(tr181node));
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSAPP_ENTRY_NODE , i+1);	
		if(cfg_query_object(tr181node,NULL,NULL) >= 0)
		{
			cfg_tr181_delete_object(tr181node);
		}
	}

	return 0;
}


int initQosApp()
{
	int i = 0;
	char tr181node[MAXLEN_NODE_NAME] = {0};
	
	for(i = 0; i < QOS_APP_NUM_MAX ; i++)
	{
		memset(tr181node, 0, sizeof(tr181node));
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSAPP_ENTRY_NODE , i+1);	

		cfg_obj_set_object_attr(tr181node, "Enable", 0, "1");
		cfg_obj_set_object_attr(tr181node, "Alias", 0, QosAppList[i]);
		cfg_obj_set_object_attr(tr181node, "Name", 0, QosAppList[i]);
		switch(i)
		{
			case 0:/*None*/
			case 1:/*IGMP*/
				break;
			case 2:/*SIP */
			case 3:/*H.323 */
			case 4:/*MGCP */
			case 6:/*DNS */
			case 7:/*DHCP*/
			case 8:/*RIP*/
			case 9:/*RSTP*/
			case 10:/*RTCP*/
			case 11:/*RTP*/
				cfg_obj_set_object_attr(tr181node, "PrtoId", 0, "UDP");
				break;
			case 5:/*SNMP*/
				cfg_obj_set_object_attr(tr181node, "PrtoId", 0, "TCP/UDP");
			default:
				break;
		}
	}

	return 0;
}


static cfg_node_ops_t cfg_type_tqosapp_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_ops_t cfg_type_tqosapp_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_tqosapp_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tqosapp, 
	 .ops = &cfg_type_tqosapp_common_ops, 
};


static cfg_node_type_t cfg_type_tqosapp_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE |QOS_APP_NUM_MAX | 1, 
	 .parent = &cfg_type_tqosapp, 
	 .ops = &cfg_type_tqosapp_entry_ops, 
};


static cfg_node_ops_t cfg_type_tqosapp_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_tqosapp_child[] = { 
	 &cfg_type_tqosapp_entry,
	 &cfg_type_tqosapp_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_tqosapp= { 
	 .name = "TQosApp", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tqosapp) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tqosapp_child, 
	 .ops = &cfg_type_tqosapp_ops, 
}; 

