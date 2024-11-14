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
#include <cfg_cli.h> 
#include <svchost_evt.h>

#include "cfg_types.h" 
#include "cfg_type_voip.h"
#include "utility.h"


extern cfg_node_type_t cfg_type_infovoippoorql_pvc;

#define VOIP_POOR_QL_NUM                    10
#define MAX_INFOVOIPPOORQL_PVC_NUMBER		VOIP_POOR_QL_NUM
#define MAX_INFOVOIPPOORQL_ENTRY_NUMBER	    CFG_TYPE_MAX_VOIP_LINE_NUM

#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
/*********record 10 poor quailty voice communication*************************/
typedef struct s_voip_poor_ql_attr{
    char           dateTime[32];
    unsigned int   txPkgCnt;
    unsigned int   rxPkgCnt;
    unsigned int   meanDelay;
    unsigned int   meanJitter;
    unsigned int   fractionLoss;
    unsigned int   localIPAddr;
    unsigned int   localUdpPort;
    unsigned int   remoteIPAddr;
    unsigned int   remoteUdpPort;
    unsigned int   mosLq;
    char           codec[16];
}voip_poor_ql_attr;

/*********************************************************************/

#endif


#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
static void infovoippoorql_attr_init(char* nodeName)
{
    char strTmp[] = "0";

    if (nodeName == NULL)
        return;
    
    cfg_set_object_attr(nodeName, "StatTime", "");
    cfg_set_object_attr(nodeName, "TxPackets", strTmp);
    cfg_set_object_attr(nodeName, "RxPackets", strTmp);            
    cfg_set_object_attr(nodeName, "MeanDelay", strTmp);
    cfg_set_object_attr(nodeName, "MeanJitter", strTmp);
    cfg_set_object_attr(nodeName, "FractionLoss", strTmp);
    cfg_set_object_attr(nodeName, "LocalIPAddress", strTmp);
    cfg_set_object_attr(nodeName, "LocalUDPPort", strTmp);
    cfg_set_object_attr(nodeName, "FarEndIPAddress", strTmp);
    cfg_set_object_attr(nodeName, "FarEndUDPPort", strTmp);
    cfg_set_object_attr(nodeName, "MosLq", strTmp);
    cfg_set_object_attr(nodeName, "Codec", "");

    return ;
}

int svc_cfg_boot_infovoippoorql(void)
{
	int i = 1, j = 1;
	char path[64] = {0};
	char tmp[64] = {0};
	for (i =1; i<=4; i++)
	{
		snprintf(tmp, sizeof(tmp), INFOVOIPPOORQL_PVC_NODE, i);		
		if (cfg_obj_query_object(tmp, NULL, NULL) <= 0)
			cfg_obj_create_object(tmp);
		for(j = 1; j <= 10; j++)
		{
			snprintf(path, sizeof(path), INFOVOIPPOORQL_PVC_ENTRY_NODE, i, j);			
			if (cfg_obj_query_object(path, NULL, NULL) <= 0)
			{
				cfg_obj_create_object(path);
				infovoippoorql_attr_init(path);
			}
			else
			{
				infovoippoorql_attr_init(path);
			}
		}
	}
    return 0;
}
#endif

static char* cfg_type_infovoippoorql_index[] = { 
	 "infovoippoorql_pvc", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_infovoippoorql_pvc_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voipnormal_commit, 
}; 

cfg_node_type_t cfg_type_infovoippoorql_pvc_entry = { 
	 .name = "Entry", 
	 .flag = /*CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MEMORY|*/CFG_TYPE_FLAG_MULTIPLE|CFG_TYPE_VOIP_POOR_QL_NUM,
	 .parent = &cfg_type_infovoippoorql_pvc,
	 .index = cfg_type_infovoippoorql_index,
	 .ops = &cfg_type_infovoippoorql_pvc_entry_ops, 
}; 

static cfg_node_type_t* cfg_type_infovoippoorql_pvc_child[] = { 
	 &cfg_type_infovoippoorql_pvc_entry, 
	 NULL 
};

static cfg_node_ops_t cfg_type_infovoippoorql_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,	 
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voipnormal_commit,
}; 

cfg_node_type_t cfg_type_infovoippoorql_pvc = { 
	 .name = "PVC", 
	 .flag = /*CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MEMORY|*/CFG_TYPE_FLAG_MULTIPLE|CFG_TYPE_MAX_VOIP_LINE_NUM,
	 .parent = &cfg_type_infovoippoorql,
	 .index = cfg_type_infovoippoorql_index,
	 .nsubtype = sizeof(cfg_type_infovoippoorql_pvc_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_infovoippoorql_pvc_child,
	 .ops = &cfg_type_infovoippoorql_pvc_ops, 
}; 

static cfg_node_ops_t cfg_type_infovoippoorql_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voipnormal_commit, 
}; 

static cfg_node_type_t* cfg_type_infovoippoorql_child[] = { 
	 &cfg_type_infovoippoorql_pvc, 
	 NULL 
};

cfg_node_type_t cfg_type_infovoippoorql= { 
	 .name = "InfoVoIPPoorQL", 
	 .flag = /*CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MEMORY|*/1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_infovoippoorql_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_infovoippoorql_child, 
	 .ops = &cfg_type_infovoippoorql_ops, 
}; 

