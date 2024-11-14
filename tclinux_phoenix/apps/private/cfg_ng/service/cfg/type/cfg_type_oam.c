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

#include "../../common/pon_svc.h" 

extern int cfg_pon_send_event(unsigned char parent,unsigned char child,unsigned int sub_event,char* attr, char* val); 

static int cfg_type_oam_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_oam_entry_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_OAM,OAM_CHILD_ENTRY,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_oam_entry_ops  = { 
	 .get = cfg_type_oam_entry_func_get, 
	 .set = cfg_type_oam_entry_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_oam_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_oam, 
	 .ops = &cfg_type_oam_entry_ops, 
}; 

static int cfg_type_oam_ctc_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_oam_ctc_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_OAM,OAM_CHILD_CTC,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_oam_ctc_ops  = { 
	 .get = cfg_type_oam_ctc_func_get, 
	 .set = cfg_type_oam_ctc_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_oam_ctc = { 
	 .name = "CTC", 
	 .flag = 1, 
	 .parent = &cfg_type_oam, 
	 .ops = &cfg_type_oam_ctc_ops, 
}; 

static int cfg_type_oam_cuc_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_oam_cuc_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_OAM,OAM_CHILD_CUC,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 


static cfg_node_ops_t cfg_type_oam_cuc_ops  = { 
	 .get = cfg_type_oam_cuc_func_get, 
	 .set = cfg_type_oam_cuc_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_oam_cuc = { 
	 .name = "CUC", 
	 .flag = 1, 
	 .parent = &cfg_type_oam, 
	 .ops = &cfg_type_oam_cuc_ops, 
}; 


static cfg_node_ops_t cfg_type_oam_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_oam_child[] = { 
	 &cfg_type_oam_entry,
	 &cfg_type_oam_ctc, 
	 &cfg_type_oam_cuc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_oam = { 
	 .name = "Oam", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_oam_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_oam_child, 
	 .ops = &cfg_type_oam_ops, 
}; 
