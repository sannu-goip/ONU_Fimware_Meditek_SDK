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
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <unistd.h>
#include <arpa/inet.h>
#include "cfg_romfile.h"
#include "utility.h"
#include "cfg_msg.h"
#include <svchost_evt.h>

#define CTCOM_PING_PID_PATH "/tmp/cwmp/ct_ping%d.pid"
#define CTCOM_MAX_IPPINGDIAGNOSTIC_NUM 3

int svc_cfg_boot_ctdiagnostic(void){
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_CTDIAGNOSTIC_BOOT, NULL, 0);	
	return 0;
}

static int cfg_type_ctdiagnostic_func_commit(char *path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path,sizeof(param.buf)-1 );
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_CTDIAGNOSTIC_UPDATE, (void*)&param, sizeof(param));
	return 0;
}

static char* cfg_type_ctdiagnostic_index[] = { 
	"ippdiag_id", 
	NULL 
}; 

static cfg_node_ops_t cfg_type_ctdiagnostic_entry_ops  = { 
	.get = cfg_type_default_func_get, 
	.set = cfg_type_default_func_set, 
	.create = cfg_type_default_func_create, 
	.delete = cfg_type_default_func_delete, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_ctdiagnostic_func_commit 
}; 

static cfg_node_type_t cfg_type_ctdiagnostic_entry = { 
	.name = "Entry", 
	.flag = CFG_TYPE_FLAG_MULTIPLE | 3, 
	.parent = &cfg_type_ctdiagnostic, 
	.index = cfg_type_ctdiagnostic_index, 
	.ops = &cfg_type_ctdiagnostic_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_ctdiagnostic_common_ops  = { 
	.get = cfg_type_default_func_get, 
	.set = cfg_type_default_func_set, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_ctdiagnostic_func_commit 
}; 

static cfg_node_type_t cfg_type_ctdiagnostic_common = { 
	.name = "Common", 
	.flag = 1, 
	.parent = &cfg_type_ctdiagnostic, 
	.ops = &cfg_type_ctdiagnostic_common_ops, 
}; 

static cfg_node_ops_t cfg_type_ctdiagnostic_ops  = { 
	.set = cfg_type_default_func_set, 
	.get = cfg_type_default_func_get, 
	.query = cfg_type_default_func_query, 
	.commit = cfg_type_ctdiagnostic_func_commit 
}; 


static cfg_node_type_t* cfg_type_ctdiagnostic_child[] = { 
	&cfg_type_ctdiagnostic_entry, 
	&cfg_type_ctdiagnostic_common, 
	NULL 
}; 


cfg_node_type_t cfg_type_ctdiagnostic = { 
	.name = "CtDiagnostic", 
	.flag = 1 , 
	.parent = &cfg_type_root, 
	.nsubtype = sizeof(cfg_type_ctdiagnostic_child) / sizeof(cfg_node_type_t*) - 1 , 
	.subtype = cfg_type_ctdiagnostic_child, 
	.ops = &cfg_type_ctdiagnostic_ops, 
}; 
