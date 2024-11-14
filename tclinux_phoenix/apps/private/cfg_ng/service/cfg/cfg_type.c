
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
#include <mxml.h>

#include "cfg_cli.h"

extern struct cfg_node_type  cfg_type_root;
static struct cfg_node_type* cfg_type_root_ptr = &cfg_type_root;
static struct cfg_node_type* cfg_type_cache[CFG_TYPE_MAX_NUM];
static int cfg_type_total_cnt = 0;


int cfg_type_default_func_create(char* path)
{

	return cfg_obj_create_object(path);
}

int cfg_type_default_func_delete(char* path)
{
	return cfg_obj_delete_object(path);
}

int cfg_type_default_func_set(char* path,char* attr,char* val)
{
	return cfg_obj_set_object_attr(path,attr,0,val);
}

int cfg_type_default_func_get(char* path,char* attr,char* val,int len)
{
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_default_func_commit(char* path)
{
	 return 0;
}

int cfg_type_default_func_query(char* path,char* subtype,int* id)
{
	return cfg_obj_query_object(path,subtype,id);
}



struct cfg_node_type*cfg_type_get_root(void)
{
	return cfg_type_root_ptr;
}


struct cfg_node_type* cfg_type_find_by_id(unsigned int id)
{
	int size = cfg_type_total_cnt;
	
	id -= CFG_TYPE_BASE_VAL;

	return id < size ? cfg_type_cache[id] : NULL;
}


int cfg_type_id_to_path(unsigned int id,char* buf,int buflen)
{

	struct cfg_node_type* type;
	char tmp[CFG_MAX_NODE_LEVEL][CFG_MAX_TYPE_NAME];
	char path[CFG_MAX_PATH_NAME];
	int i = 0, len = 0;

	if ((type = cfg_type_find_by_id(id)) == NULL)
		return -1;
	
	memset(tmp, 0, sizeof(tmp));
	while(type)
	{
		strncpy(&tmp[i][0],type->name,sizeof(tmp[0]) - 1);
		i++;
		type = type->parent;
	}

	while(i > 0)
	{
		i--;
		len += snprintf(path + len, sizeof(path)-len, "%s.",&tmp[i][0]);
	}
	
	path[len - 1] = 0;
	
	strncpy(buf, path, buflen - 1);
	
	buf[buflen-1] = 0;
	
	return 0;
}


int cfg_type_is_dynamic_type(int type)
{
	struct cfg_node_type* node;

	node = cfg_type_find_by_id(type);
	
	return (node && node->ops && node->ops->create);

}

struct cfg_node_type* cfg_type_get_child(struct cfg_node_type* node)
{
	if (node == NULL)
		return NULL;
	
	return node->subtype ? node->subtype[0] : NULL;
}

struct cfg_node_type* cfg_type_get_brother(struct cfg_node_type* node)
{
	struct cfg_node_type**  child;
	int i;
	
	if (node == NULL)
		return NULL;
	
	if (node->parent == NULL || (child = node->parent->subtype) == NULL)
		return NULL;
	
	for(i = 0; (child[i]) && (child[i] != node); i++);
	
	if (child[i] == node)
		return child[i+1];

	printf("cfg_type_get_brother: not found type [%s] in parent [%s] \n",node->name,node->parent->name);

	return NULL;
}



static int cfg_type_alloc_id(struct cfg_node_type* node)
{
	struct cfg_node_type* tmp;

	if (node == NULL)
		return 0;

	node->id = (CFG_TYPE_BASE_VAL + cfg_type_total_cnt) ;
	
	cfg_type_cache[cfg_type_total_cnt] = node;
	
	cfg_type_total_cnt++;
	
	if ((tmp = cfg_type_get_child(node)) != NULL)
		cfg_type_alloc_id(tmp);
	
	if ((tmp = cfg_type_get_brother(node)) != NULL)
		cfg_type_alloc_id(tmp);
	
	return 0;
}


int cfg_type_init(void)
{

	return cfg_type_alloc_id(cfg_type_root_ptr);
}


int cfg_type_dump(struct cfg_node_type* node)
{

	static int tabs  =0;
	struct cfg_node_type* type;
	int tab = tabs;
	
	if (node == NULL)
		return 0;
	
	while(tab--) printf("\t");
	
	printf("[%s:%d  0x%04x] \n",node->name,node->id,node->flag);

	tabs++;
	if ((type = cfg_type_get_child(node)) != NULL)
		cfg_type_dump(type);

	tabs--;
	if ((type = cfg_type_get_brother(node)) != NULL)
		cfg_type_dump(type);

	return 0;
}

