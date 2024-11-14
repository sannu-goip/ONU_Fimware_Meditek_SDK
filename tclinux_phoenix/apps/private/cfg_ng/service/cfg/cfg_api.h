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

#ifndef __CFG_API_H__
#define __CFG_API_H__

#include "cfg_type.h"

#define CFG_NODE_SPELIRATE_CHAR  '.'
#define CFG_MAX_TYPE_NAME	32
#define CFG_MAX_PATH_NAME 	512

#define CFG_MAX_ATTR_VALUE 	1024
#define CFG_MAX_ATTR_NAME	32

#define CFG_MAX_NODE_LEVEL	8
#define CFG_MAX_NODE_NAME	CFG_MAX_TYPE_NAME

#define CFG_TYPE_SHIFT		16
#define CFG_TYPE_MASK		0xffff0000
#define CFG_OBJ_MASK		0xffff


#define CFG_OID_TO_TID(x)		((x>>16) & 0xffff)
#define CFG_OID_TO_ID(x)		(x & 0xffff)

#define CFG_EVT_LOG_PATH		"/tmp/ubus_evt.log"
/*#define CFG_OBJ_FILE_PATH		"/usr/osgi/cfg.save" */
#define CFG_OBJ_FILE_PATH		"/var/romfile.cfg"

#define CFG_LOAD_STATUS_FLAG_PATH  		"root.XPON.Common"
#define CFG_LOAD_STATUS_FLAG			"CfgLoading"
#define CFG_LOAD_DONE					"Done"

#define NO_QMARKS 				0
#define QMARKS 					1
#define NO_ATTRIBUTE 			2
#define MAX_TCAPI_MSG_SIZE		1024

int cfg_create_object(char* objpath);

int cfg_check_create_object(char* objpath);

int cfg_delete_object(char* objpath);

int cfg_commit_object(char* objpath);

int cfg_query_object(char* objpath,char* subtype, int* id);

int cfg_get_object_attr(char* path,char* attr,char* val,int size);

int cfg_set_object_attr(char* path,char* attr,char* val);

int cfg_path_to_oid_array(char* path, int* id);

int cfg_send_event(char* evt,unsigned int sub_evt,void * param,unsigned int len);

int cfg_save_object(char* file);

int cfg_load_object(char* file);

int cfg_parse_node_name(char* name,char* path,int size);

int cfg_save_attrs_to_file(char* path,char* file,int type);

int cfg_save_attrs_to_file_fp(char* path, FILE* fp,int type);

int cfg_load_file(char* file);

int cfg_save_file(char* file);


#endif
