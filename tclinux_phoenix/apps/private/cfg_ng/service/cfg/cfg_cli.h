
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

#ifndef __CFG_CLIENT_H__
#define __CFG_CLIENT_H__

#include "cfg_api.h"

struct cfg_attr_type;
struct mxml_node_s;

typedef struct cfg_attr_type_ops{
	int (*setattr)(char* objpath,char* attr,int type,char* val);
	int (*getattr)(char* objpath,char* attr,int type,char* val,int size);
}cfg_attr_ops_t;


typedef struct cfg_attr_type{
	char* name;
	int flag;
	int size;
	struct cfg_attr_type_ops* ops;
}cfg_attr_type_t;


typedef struct cfg_node_type_ops {
	int (*create)(char* path);
	int (*delete)(char* path);	
	int (*set)(char* path,char* attr,char* val);	
	int (*get)(char* path,char* attr,char* val,int len);
	int (*commit)(char* path);
	int (*query)(char* path,char* subtype, int* id);
	int (*memory_type_read)(char* path,char* attr,char* val,int len);
 } cfg_node_ops_t;


typedef struct cfg_node_type{
	unsigned int id;
	char* name;
	unsigned int flag;
	int nsubtype;
	struct cfg_node_type* parent;
	struct cfg_node_type** subtype;	
	struct cfg_node_type_ops* ops;
	char** index;
}cfg_node_type_t;


typedef struct cfg_node_obj{
	unsigned int id;
	struct cfg_node_obj* parent;
	struct cfg_node_obj* child;
	struct cfg_node_obj* brother;
	void* private;
}cfg_node_obj_t;


int cfg_type_id_to_path(unsigned int id,char* buf,int buflen);

struct cfg_node_type* cfg_type_query(char* path);

struct cfg_node_type* cfg_type_get_root(void);

struct cfg_node_type* cfg_type_find_by_id(unsigned int id);

struct cfg_node_type* cfg_type_find_by_name(char* name);

struct cfg_node_type* cfg_type_get_child(struct cfg_node_type* node);

struct cfg_node_type* cfg_type_get_brother(struct cfg_node_type* node);

int cfg_attr_is_storage(cfg_attr_type_t* attr);

int cfg_type_is_dynamic_type(int type);

int cfg_type_dump(struct cfg_node_type* node);

int cfg_type_init(void);

void* shm_alloc(unsigned int size);

void shm_free(void* ptr);

void* shm_calloc(unsigned int n,unsigned int size);

void* shm_realloc(void* ptr, unsigned int size);

void shm_lib_deinit(void);

int shm_lib_init(void);

void* shm_gettree(void);

void shm_settree(void* tree);

int shm_pool_lock(void);

int shm_pool_unlock(void);

int cfg_obj_create_object(char* objpath);

int cfg_obj_delete_object(char* objpath);

int cfg_obj_commit_object(char* objpath);

int cfg_obj_query_object(char* objpath,char* type, int* id);

int cfg_obj_get_object_attr(char* objpath,char* attr,int type,char* val,int size);

int cfg_obj_set_object_attr(char* objpath,char* attr,int type,char* val);

int cfg_obj_get_all_attr(char* objpath,char* buf);

int cfg_obj_set_all_attr(char* objpath,char* buf);

int cfg_obj_dump(char* path,int depth);

int cfg_obj_dump_obj(cfg_node_obj_t* obj,int depth);

int cfg_obj_merge_tree(cfg_node_obj_t* node,cfg_node_obj_t* from);

int cfg_obj_save_tree(FILE* fp,cfg_node_obj_t* node,int mode);

int cfg_obj_send_event(char* evt,unsigned int sub_evt,void * param,unsigned int len);

int cfg_obj_copy_private(char* dst,char* src);

int cfg_obj_copy_private_without_del(char* dst,char* src);

cfg_node_obj_t* cfg_obj_load_from_type(void);

cfg_node_obj_t* cfg_obj_load_from_xml(void* xmlnode);

cfg_node_obj_t* cfg_obj_get_root(void);

cfg_node_obj_t* cfg_obj_set_root(cfg_node_obj_t* node);

int cfg_obj_release_object(cfg_node_obj_t* obj);

int cfg_type_default_func_create(char* path);

int cfg_type_default_func_delete(char* path);

int cfg_type_default_func_set(char* path,char* attr,char* val);

int cfg_type_default_func_get(char* path,char* attr,char* val,int len);

int cfg_type_default_func_commit(char* path);

int cfg_type_default_func_query(char* path,char* subtype,int* id);

int cfg_str_to_lower(char* str);

int cfg_xml_node_to_path(struct mxml_node_s* node,char* path);

int cfg_printf(char* format,...);

int cfg_load_from_xml(struct mxml_node_s* node);

void cfg_readAll(void);

int cfg_load_web_type_node(void);

#endif





