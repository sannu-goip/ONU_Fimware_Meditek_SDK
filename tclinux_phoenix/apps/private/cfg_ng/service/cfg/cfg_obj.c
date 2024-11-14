

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
#include <unistd.h>
#include <svchost_api.h>

#include "cfg_cli.h"
#include "cfg_xml.h"
#include "utility.h"

static cfg_node_obj_t*  cfg_obj_root = NULL; 

extern struct cfg_node_type* cfg_type_root;


#define cfg_obj_type(x)		CFG_OID_TO_TID(x)
#define cfg_obj_id(x)		CFG_OID_TO_ID(x)



int cfg_printf(char* format,...)
{
	char msg[256];
	va_list args;

	va_start(args, format);
	vsnprintf(msg, 256, format, args);	
	va_end(args);

	printf("%s",msg);
	
	return 0;
}

static struct cfg_node_type*  cfg_type_from_oid(unsigned int oid)
{
	return cfg_type_find_by_id(cfg_obj_type(oid));
}


static int cfg_obj_lock(void)
{

	return shm_pool_lock();
}

static int cfg_obj_unlock(void)
{

	return shm_pool_unlock();
}

cfg_node_obj_t* cfg_obj_get_root(void)
{
#if 1
	return shm_gettree();
#else
	return cfg_obj_root;
#endif
}

cfg_node_obj_t* cfg_obj_set_root(cfg_node_obj_t* node)
{

	cfg_node_obj_t* tmp = cfg_obj_root;

	cfg_obj_root = node;

	return tmp;
}

int cfg_obj_send_event(char* evt,unsigned int sub_evt,void * param,unsigned int len)
{
	struct host_service_pbuf buf;
	int ret = -1;
	
	svchost_init_pbuf(&buf);
	
	if (svchost_put_param(&buf, sizeof(sub_evt), (void *)&sub_evt) < 0){
		goto error;
	}
	if((NULL != param)&&(0 != len)){
		if (svchost_put_param(&buf, len, param) < 0){
			goto error;
		}
	}
	
	ret = svchost_send_event(evt,&buf);

error:
	svchost_free_pbuf(&buf);
	return ret;
}

int cfg_send_event(char* evt,unsigned int sub_evt,void * param,unsigned int len)
{	
	return cfg_obj_send_event(evt,sub_evt,param,len);
}

static int cfg_get_token(char** objpath, char* token)
{

	char* start = *objpath;
	char* end;
	
	if (start == NULL || start[0] == CFG_NODE_SPELIRATE_CHAR || !start[0])
		return -1;

	end = strchr(start,CFG_NODE_SPELIRATE_CHAR);

	if (end)
	{	
		if (end - start >= CFG_MAX_TYPE_NAME)
		return -1;
	
		memcpy(token,start,end-start);
		token[end-start] = 0;
		*objpath = end + 1;
		return 1;
	}
	strcpy(token,start);
	*objpath = start + strlen(start);
	return 0;
}

int cfg_str_to_lower(char* str)
{
	int i = 0;
	
	while(str[i])
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] =  str[i] + 32;
		i++;
	}
	return 0;
}

static int cfg_type_compare(char* s1,char* s2)
{
	char tmp1[CFG_MAX_TYPE_NAME] = {0}, tmp2[CFG_MAX_TYPE_NAME] = {0};
	
	if (s1 == NULL || s2 == NULL)
		return -1;

	strncpy(tmp1, s1, sizeof(tmp1) - 1);
	strncpy(tmp2, s2, sizeof(tmp2) - 1);
	tmp1[CFG_MAX_TYPE_NAME-1] = 0;
	tmp2[CFG_MAX_TYPE_NAME-1] = 0;
	cfg_str_to_lower(tmp1);
	cfg_str_to_lower(tmp2);
	
	return strcmp(tmp1,tmp2);
}

static struct cfg_node_type* cfg_type_query_child( struct cfg_node_type* type,char* child)
{
	struct cfg_node_type* subtype;

	if(child == NULL) 
		return NULL;

	subtype = cfg_type_get_root();

	if (type == NULL )
	{
		if (cfg_type_compare(subtype->name, child) == 0)
			return subtype;
		else
			goto exit;
	}
	
	subtype = cfg_type_get_child(type);
	
	while(subtype)
	{
		if (cfg_type_compare(subtype->name,child) == 0)
			return subtype;
		subtype = cfg_type_get_brother(subtype);
	}

exit:
	return NULL;
}



struct cfg_node_type* cfg_type_query(char* path)
{
	cfg_node_type_t* node = NULL;
	char token[CFG_MAX_TYPE_NAME] = {0};
	char* tmp = path;
	int need_alpha = 1, digit = 0, level = 0;

	while(cfg_get_token(&tmp,token) >= 0 && level < CFG_MAX_NODE_LEVEL)
	{
		if (token[0] >= '1' && token[0] <= '9')
			digit = atoi(token);
		else
			digit = 0;
		
		if (need_alpha && digit)
			return NULL;

		if (digit)
		{
			need_alpha = 1;
		}
		else
		{
			if ((node = cfg_type_query_child(node,token)) == NULL)
				return NULL;
			need_alpha = 0;
		}
		level++;
	}

	if ((tmp && *tmp) || level == CFG_MAX_NODE_LEVEL)	
		return NULL;
	
	return node;
}

int cfg_type_path_to_id(char* path)
{
	struct cfg_node_type* type ;

	if ((type = cfg_type_query(path)) == NULL)
		return -1;
	
	return type->id;
}



/*
	transform  the objectt path to obj id [obj type id << 16 | obj id] 
*/

int cfg_path_to_oid_array(char* objpath, int* id)
{
	int objid[CFG_MAX_NODE_LEVEL] = {0};
	cfg_node_type_t* node = NULL;
	char token[CFG_MAX_TYPE_NAME] = {0};
	char* tmp = objpath;
	int level= 0,need_alpha = 1,digit = 0,oid = 0,tid = 0;

	if (id == NULL)
		return -1;
	
	while(cfg_get_token(&tmp,token) >= 0 && level < CFG_MAX_NODE_LEVEL)
	{
		if (token[0] >= '1' && token[0] <= '9')
			digit = atoi(token);
		else
			digit = 0;
		
		if (need_alpha && digit)
			return -1;

		if (digit)
		{
			/*
			if (oid >= (1 << CFG_TYPE_SHIFT))
				return -1;
			*/
			oid = digit;
			need_alpha = 1;
		}
		else
		{
			if (oid && tid)
				objid[level++] = (tid << CFG_TYPE_SHIFT) | oid;
			
			if ((node = cfg_type_query_child(node,token)) == NULL)	
				return -1;
			
			tid = node->id;
			oid = 1;
			need_alpha = 0;
		}
	}
	
	if ((tmp && *tmp) || level == CFG_MAX_NODE_LEVEL)	
		return -1;
	
	objid[level++] = (tid << CFG_TYPE_SHIFT) | oid;
	memcpy(id,objid, sizeof(int) * level);
	
	return level;
}

static int cfg_path_to_oid(char* objpath)
{
	int objid[CFG_MAX_NODE_LEVEL];
	int level,isid;
	char* tmp;

	tmp = objpath + strlen(objpath);

	while ((*tmp) != CFG_NODE_SPELIRATE_CHAR && tmp > objpath) 
		tmp--;

	if (tmp == objpath)
		isid = 0;
	else
		isid = (atoi(tmp + 1) > 0) ? 1 : 0;
	
	level = cfg_path_to_oid_array(objpath,objid);
	
	if (level <= 0 || level >= CFG_MAX_NODE_LEVEL)
		return 0;
	
	
	return isid ? objid[level-1] : objid[level-1] & CFG_TYPE_MASK;
}

static cfg_node_obj_t* cfg_obj_get_child(cfg_node_obj_t* parent ,int oid)
{
	cfg_node_obj_t* child;

	if (parent == NULL)
		return NULL;

	child = parent->child;

	while(child)
	{
		if (child->id == oid)
			return child;
		child = child->brother;
	}
	
	return child;
}

static cfg_node_obj_t*  cfg_obj_get_obj(char* objpath)
{
	int id[CFG_MAX_NODE_LEVEL];
	int level =0, i = 1;
	cfg_node_obj_t* parent = cfg_obj_root;

	if (parent == NULL)
		return NULL;

	memset(id, 0, sizeof(id));
	
	level = cfg_path_to_oid_array(objpath,id);

	if (level <= 0)
		return NULL;
	
	if (level == 1)
		return parent;

	while(i < level)
	{
		if ((parent = cfg_obj_get_child(parent,id[i])) == NULL)
			return NULL;
		i++;
	}
	return parent;
}

static int cfg_obj_first_unused_oid(cfg_node_obj_t* obj,unsigned int oid)
{

	unsigned int first;
	cfg_node_obj_t* child;

	first = oid & CFG_TYPE_MASK;
	child = obj->child;

again:
	first++;
	while(child)
	{
		if (child->id  == first)
			break;
		child = child->brother;
	}

	if (child)
		goto again;

	return first;
}

static int cfg_obj_auto_oid(cfg_node_obj_t* obj,unsigned int oid)
{
	cfg_node_obj_t* child;
	unsigned int type = 0, maxid = 0;
	
	type = maxid = oid & CFG_TYPE_MASK;
	
	child = obj->child;

	while(child)
	{
		if ((child->id & CFG_TYPE_MASK) == type && child->id > maxid)
			maxid = child->id;
		child = child->brother;
	}

	if ((maxid & CFG_OBJ_MASK) == CFG_OBJ_MASK)
		return cfg_obj_first_unused_oid(obj,oid);
		
		
	return maxid + 1;
}



static cfg_node_obj_t* cfg_obj_alloc_node(int type)
{

	cfg_node_obj_t*  node = NULL;
	mxml_node_t* xmlnode = NULL;

	if ((node = shm_calloc(1,sizeof(cfg_node_obj_t))) == NULL)
		goto error;

	if ((xmlnode = shm_calloc(1,sizeof(mxml_node_t))) == NULL)
		goto error;

	node->private = xmlnode;

	return node;
error:
	if (node)
		shm_free(node);
	/*
	if (xmlnode)
		shm_free(xmlnode);
	*/
	return NULL;
}

static cfg_node_obj_t* cfg_obj_create_from_type(cfg_node_obj_t* parent,cfg_node_type_t* type)
{
	cfg_node_obj_t* node;	
	cfg_node_type_t* tmp;

	node = cfg_obj_alloc_node(type->id);

	if (node == NULL)
	{
		cfg_printf("cfg_obj_create_from_type: alloc obj eroor ! \n");
		return NULL;
	}
	
	node->id = (type->id << CFG_TYPE_SHIFT) + 1;

	node->parent = parent;

	tmp = cfg_type_get_child(type);
	if (tmp)
		node->child = cfg_obj_create_from_type(node,tmp);

	tmp = cfg_type_get_brother(type);
	if (tmp)
		node->brother = cfg_obj_create_from_type(parent,tmp);

	return node;
}

static int cfg_obj_del_from_type(cfg_node_obj_t* node);

static int cfg_obj_create_sub_obj(cfg_node_obj_t* obj,int oid)
{
	cfg_node_obj_t* node,*child;
	cfg_node_type_t* type;

	if (cfg_obj_get_child(obj,oid) != NULL)
	{
		cfg_printf("cfg_obj_create_sub_obj: child exist [%d:%d] ! \n",cfg_obj_type(oid), cfg_obj_id(oid));
		return -1;
	}

	if ((type = cfg_type_from_oid(oid)) == NULL)
	{
		cfg_printf("cfg_obj_create_sub_obj:find type [%d:%d] fail \n", cfg_obj_type(oid), cfg_obj_id(oid));
		return -1;
	}

	node = cfg_obj_alloc_node(cfg_obj_type(oid));

	if (node == NULL)
 	{
		cfg_printf("cfg_obj_create_sub_obj: alloc obj eroor ! \n");
		return -1;
	}
	
	node->id = cfg_obj_id(oid) ? oid : cfg_obj_auto_oid(obj,oid);
	node->brother = NULL;
	node->parent = obj;
	node->child = cfg_type_get_child(type) ? cfg_obj_create_from_type(node,cfg_type_get_child(type)) : NULL;

	if (node->child )
		cfg_obj_del_from_type(node->child);

	if (obj->child  == NULL)
		obj->child = node;
	else
	{
		child = obj->child;
		while(child->brother != NULL)
			child = child->brother;
		child->brother = node;
	}
	return (node->id);
}



static int cfg_get_parent_path(char* objpath, char* parent)
{
	int len,first = 1;

	if (objpath == NULL || parent == NULL)
		return -1;
	
	len = strlen(objpath);

	strcpy(parent,objpath);
	
again:
	
	while(len > 0)
	{
		if (parent[len] == CFG_NODE_SPELIRATE_CHAR)
			break;
		len--;
	}
	
	if (len == 0)
	{
		cfg_printf("cfg_get_parent_path: path eroor [%s]! \n",objpath);
		return -1;
	}
	
	if (first && parent[len+1] >= '0' && parent[len+1] <= '9')
	{
		len--;
		first = 0;
		goto again;
	}
	
	parent[len] = 0;
	
	return 0;
}



int cfg_obj_create_object(char* objpath)
{
	char parent[CFG_MAX_PATH_NAME];
	cfg_node_obj_t* node;
	int oid;
	struct cfg_node_type* type = NULL;

	if (cfg_get_parent_path(objpath,parent) < 0)
	{
		cfg_printf("cfg_obj_create_object: get parent eroor [%s] ! \n",objpath);
		return -1;
	}

	/*if the parent node not exist, we firstly need to create parent node, then create this node*/
	if (cfg_obj_query_object(parent,NULL,NULL) <= 0)
	{
		type = cfg_type_query(parent);
		if (type != NULL && cfg_type_is_dynamic_type(type->id))
		{
			if (cfg_obj_create_object(parent) <= 0)
			{
				cfg_printf("create %s Fail.\n", parent);
				return -1;
			}
		}else{
			return -1;
		}
	}
	
	if ((oid = cfg_path_to_oid(objpath)) <= 0)
	{
		cfg_printf("cfg_obj_create_object: path to oid eroor [%s]! \n",objpath);
		return -1;
	}
	
	cfg_obj_lock();
	
	if ((node = cfg_obj_get_obj(parent)) == NULL)
	{
		cfg_printf("cfg_obj_create_object: [%s] not exist \n",parent);
		oid = 0;
		goto unlock;
	}
	
	if ((oid = cfg_obj_create_sub_obj(node,oid)) <= 0)
		cfg_printf("cfg_obj_create_object: [%s] create fail \n",parent);
	
unlock:
	cfg_obj_unlock();
	
	return oid;
}


static int cfg_obj_free_object(cfg_node_obj_t* node)
{
	if (node == NULL)
		return  0;

	if (node->private)
		cfg_xml_release_node((mxml_node_t*)node->private);

	shm_free(node);
	
	return 0;
}

int cfg_save_attrs_to_file(char* path,char* file,int type)
{
	int ret = -1,num = 0,i = 0;
	cfg_node_obj_t* obj = NULL;
	mxml_node_t* node = NULL;
	mxml_attr_t* attrs = NULL;	
	FILE* fp	= NULL;
	char *escape_buf = NULL;
	char buf[MAX_TCAPI_MSG_SIZE*2 + 129] 	= {0};
	char cmd_buf[MAX_TCAPI_MSG_SIZE*2 + 1] = {0} ;
	
	if ((fp = fopen(file,"w")) == NULL)
		return -1;
	
	cfg_obj_lock();

	if ((obj = cfg_obj_get_obj(path)) == NULL)
		goto unlock;
	
	if ((node = (mxml_node_t*) obj->private) == NULL)
		goto unlock;

	num = node->value.element.num_attrs;
	attrs =  node->value.element.attrs;
	for(i = 0; i < num; i++){
		switch(type){
			case NO_QMARKS:
				snprintf(buf, sizeof(buf), "%s=%s\n", attrs[i].name, attrs[i].value);
				break;
			case QMARKS:
				escape_buf = escape_special_character(attrs[i].value, cmd_buf, sizeof(cmd_buf));
				snprintf(buf, sizeof(buf),"%s=\"%s\"\n", attrs[i].name, escape_buf);
				break;
			case NO_ATTRIBUTE:
				snprintf(buf, sizeof(buf), "%s\n", attrs[i].value);
				break;
			default:
				break;
		}
		fputs(buf, fp);
	}

	ret = 0;
unlock:
	cfg_obj_unlock();
	
	fclose(fp);
	
	return ret;
}

int cfg_save_attrs_to_file_fp(char* path,FILE* fp,int type)
{
	int ret = -1,num = 0,i = 0;
	cfg_node_obj_t* obj = NULL;
	mxml_node_t* node = NULL;
	mxml_attr_t* attrs = NULL;	
	char *escape_buf = NULL;
	char buf[MAX_TCAPI_MSG_SIZE*2 + 129] 	= {0};
	char cmd_buf[MAX_TCAPI_MSG_SIZE*2 + 1] = {0} ;
	
	if (fp == NULL)
		return -1;
	
	cfg_obj_lock();

	if ((obj = cfg_obj_get_obj(path)) == NULL)
		goto unlock;
	
	if ((node = (mxml_node_t*) obj->private) == NULL)
		goto unlock;

	num = node->value.element.num_attrs;
	attrs =  node->value.element.attrs;
	for(i = 0; i < num; i++){
		memset(buf, 0, sizeof(buf));
		switch(type){
			case NO_QMARKS:
				snprintf(buf, sizeof(buf), "%s=%s\n", attrs[i].name, attrs[i].value);
				break;
			case QMARKS:
				escape_buf = escape_special_character(attrs[i].value, cmd_buf, sizeof(cmd_buf));
				snprintf(buf, sizeof(buf),"%s=\"%s\"\n", attrs[i].name, escape_buf);
				break;
			case NO_ATTRIBUTE:
				snprintf(buf, sizeof(buf), "%s\n", attrs[i].value);
				break;
			default:
				break;
		}
		/*tcdbg_printf("cfg_save_attrs_to_file,buf = %s\n",buf);*/
		fputs(buf, fp);
	}

	ret = 0;
unlock:
	cfg_obj_unlock();
	
	return ret;
}

int cfg_obj_copy_private(char* dst,char* src)
{
	cfg_node_obj_t* dstobj,*srcobj;
	mxml_node_t* srcnode, *dstnode;
	mxml_attr_t* attr;
	
	int i,num,ret = -1;
	
	cfg_obj_lock();
	
	dstobj = cfg_obj_get_obj(dst);
	
	srcobj = cfg_obj_get_obj(src);

	if (dstobj == NULL || srcobj == NULL)
		goto unlock;
	
	
	srcnode = (mxml_node_t*) srcobj->private;
	
	dstnode = shm_calloc(1,sizeof(mxml_node_t));

	if (srcnode == NULL || dstnode == NULL)
		goto unlock;

	num = srcnode->value.element.num_attrs;
	attr =  srcnode->value.element.attrs;
	
	for(i=0;i < num;i++)
		cfg_xml_set_attr(dstnode,attr[i].name,attr[i].value);

	if (dstobj->private)
		cfg_xml_release_node((mxml_node_t*)dstobj->private);

	dstobj->private = dstnode;

	ret = 0;
unlock:
	cfg_obj_unlock();
	
	return ret;
}

int cfg_obj_copy_private_without_del(char* dst,char* src)
{
	cfg_node_obj_t* dstobj,*srcobj;
	mxml_node_t* srcnode, *dstnode;
	mxml_attr_t* attr, *attr_dst;
	
	int i,num,ret = -1;
	
	cfg_obj_lock();
	
	dstobj = cfg_obj_get_obj(dst);
	
	srcobj = cfg_obj_get_obj(src);

	if (dstobj == NULL || srcobj == NULL)
		goto unlock;
	
	
	srcnode = (mxml_node_t*) srcobj->private;
	dstnode = (mxml_node_t*) dstobj->private;

	if (srcnode == NULL || dstnode == NULL)
		goto unlock;

	num = srcnode->value.element.num_attrs;
	attr =  srcnode->value.element.attrs;

	for(i=0;i < num;i++)	
		cfg_xml_set_attr(dstnode,attr[i].name,attr[i].value);

	ret = 0;
unlock:
	cfg_obj_unlock();
	
	return ret;
}



static int cfg_obj_unlink_object(cfg_node_obj_t* node)
{

	cfg_node_obj_t* first, *prev = NULL;

	if (node->parent == NULL)
		goto unlink;

	first = node->parent->child;
	
	if (first == node)
	{
		node->parent->child = node->brother;
		goto unlink;
	}
		
	while(first && first != node) 
	{
		prev = first;
		first = first->brother; 
	}

	if(first)
		prev->brother = node->brother;
	else
		cfg_printf("cfg_obj_unlink_object: error node[%d:%d] \n",cfg_obj_type(node->id),cfg_obj_id(node->id));
	
unlink:
	node->brother = NULL;
	node->parent = NULL;
	return 0;
}


int cfg_obj_release_object(cfg_node_obj_t* obj)
{
	if (obj == NULL)
		return 0;

	cfg_obj_release_object(obj->child);
	cfg_obj_release_object(obj->brother);	
	cfg_obj_free_object(obj);
	return 0;
}

static int cfg_obj_del_object(cfg_node_obj_t* obj)
{
	int oid;
	
	if(obj == NULL)
		return -1;

	oid = obj->id;
	
	cfg_obj_unlink_object(obj);
	
	cfg_obj_release_object(obj);

	return oid;
}

int cfg_obj_delete_object(char* objpath)
{
	cfg_node_obj_t* node;
	int ret = -1;

	cfg_obj_lock();
	
	if ((node = cfg_obj_get_obj(objpath)) == NULL )
	{
		cfg_printf("cfg_obj_delete_object: [%s] not exist \n",objpath);
		goto unlock;
	}
	
	
	if ((ret = cfg_obj_del_object(node)) < 1)
		cfg_printf("cfg_obj_delete_object: [%s] delete fail  \n",objpath);
	
unlock:
	cfg_obj_unlock();
	
	return ret;
}


static int cfg_obj_get_obj_attr(cfg_node_obj_t* node,char* name,char* val,int size)
{

	char* tmp;
	if ((tmp = (char*)cfg_xml_get_attr((mxml_node_t*)node->private,name)) == NULL)
		return -2;

	strncpy(val, tmp, size - 1);
	val[size-1] = 0;
	
	return strlen(val);
}


int cfg_obj_get_object_attr(char* path,char* attr,int type,char* val,int size)
{

	cfg_node_obj_t* node;
	int ret = -1;
	
	if (path== NULL || attr == NULL|| val == NULL || size <= 0)
		return -1;
	

	cfg_obj_lock();
	
	if ((node = cfg_obj_get_obj(path)) == NULL)
	{
		/*cfg_printf("cfg_obj_get_object_attr: [%s] not exist \n",path);*/
		goto unlock;;
	}
	
	ret =  cfg_obj_get_obj_attr(node,attr,val,size);

unlock:	
	cfg_obj_unlock();

	return ret;
}


static int cfg_obj_set_obj_attr(cfg_node_obj_t* node,char* name,char* val)
{

	if (node == NULL || name == NULL || val == NULL)
		return -1;

	cfg_xml_set_attr((mxml_node_t*)node->private,name,val);
	
	return 0;
}


int cfg_obj_set_object_attr(char* path,char* attr,int type,char* val)
{

	cfg_node_obj_t* node;
	int ret = -1;
	
	if (path == NULL || attr == NULL|| val == NULL)
		return -1;
	
	if( attr[0] == '\0' ){
		printf("[%s:%d]: path = [%s], val = [%s] \n",__FUNCTION__,__LINE__, path, val);
		return -1;
	}
	
	cfg_obj_lock();
	
	if ((node = cfg_obj_get_obj(path)) == NULL)
	{
		/*cfg_printf("cfg_obj_set_object_attr: [%s] not exist \n",path);*/
		goto unlock;
	}
	
	ret = cfg_obj_set_obj_attr(node,attr,val);
	
unlock:
	cfg_obj_unlock();
	
	return ret ;
}


static int cfg_obj_get_child_oid(cfg_node_obj_t* node, int type, int* id)
{
	int num = 0;
	cfg_node_obj_t* child = node->child;

	while (child)
	{
		if (cfg_obj_type(child->id) == type)
		{
			if (id)
				id[num] = (child->id  & CFG_OBJ_MASK);
			num++;
		}
		child = child->brother;
	}
	
	return num;
}

/*
API : cfg_query_object
*/
int cfg_obj_query_object(char* objpath ,char* subtype,int* id)
{
	cfg_node_obj_t* node;
	cfg_node_type_t* type = NULL;
	int ret = -1;
	
	if (subtype != NULL)
	{
		if ((type = cfg_type_query(objpath)) == NULL)
			return -1;

		if ((type = cfg_type_query_child(type,subtype)) == NULL)
			return -1;
	}

	cfg_obj_lock();
	
	if ((node = cfg_obj_get_obj(objpath)) == NULL)
		goto unlock;
	
	
	if (subtype == NULL)
	{
		if (id)
			id[0] = (node->id & CFG_OBJ_MASK);
		ret = 1;
		goto unlock;
	}
	
	ret =  cfg_obj_get_child_oid(node,type->id,id);
	
unlock:
	cfg_obj_unlock();
	
	return ret;
}

/*
API : cfg_commit_object
*/

int cfg_obj_commit_object(char* objpath)
{
	cfg_node_obj_t* node;
	cfg_node_type_t* type;
	int ret = 0;
	
	cfg_obj_lock();
	node = cfg_obj_get_obj(objpath);
	cfg_obj_unlock();

	if (node == NULL)
		return -1;

	if ((type = cfg_type_find_by_id(node->id >> CFG_TYPE_SHIFT)) == NULL)
		return -1;

	if (type->ops->commit)
		type->ops->commit(objpath);
	
	return ret;
}




static int cfg_obj_del_from_type(cfg_node_obj_t* node)
{
	if (node == NULL)
		return 0;

	cfg_obj_del_from_type(node->child);

	cfg_obj_del_from_type(node->brother);
	
	if (cfg_type_is_dynamic_type(cfg_obj_type(node->id)))
		cfg_obj_del_object(node);
	
	return 1;
}

cfg_node_obj_t* cfg_obj_load_from_type(void)
{
	cfg_node_obj_t* node;

	node =  cfg_obj_create_from_type(NULL,cfg_type_get_root());

	if (node)
		cfg_obj_del_from_type(node);

	return node;
}

char* checkvalue(char *attrvalue, char *replacedstr, int replacedstr_len)
{
	int i = 0;

	for (i = 0; i < strlen(attrvalue); i++)		
	{
		if(replacedstr_len -1  >  strlen(replacedstr) )
		{
		if (attrvalue[i] == '&')			 
			 	strncpy(replacedstr + strlen(replacedstr), "&amp;", replacedstr_len - strlen(replacedstr) - 1);		 
		else if (attrvalue[i] == '<')
			 	strncpy(replacedstr + strlen(replacedstr), "&lt;", replacedstr_len - strlen(replacedstr) - 1);  
		else if (attrvalue[i] == '>')
			 	strncpy(replacedstr + strlen(replacedstr), "&gt;", replacedstr_len - strlen(replacedstr) - 1); 
		else if (attrvalue[i] == '\"') 		   
			 	strncpy(replacedstr + strlen(replacedstr), "&quot;", replacedstr_len - strlen(replacedstr)-1);
		else
		 replacedstr[strlen(replacedstr)] = attrvalue[i];
	}
	}
	replacedstr[strlen(replacedstr)+1] = '\0';
	
	return replacedstr;
}

char* xmlFormatFunc(char *attrvalue, char *replacedstr, int len)
{
	return checkvalue(attrvalue, replacedstr, len);
}

int cfg_obj_save_tree(FILE* fp,cfg_node_obj_t* node,int mode)
{

	int i,num;
	cfg_node_type_t* type;
	mxml_node_t* xmlnode = NULL;
	char replacedstr[1024] = {0};
	
	if (node == NULL)
		return 0;

	xmlnode = node->private;
	
	if ((type = cfg_type_from_oid(node->id)) == NULL)
	{

		printf("cfg_obj_save_tree: type [%d] is null \n",cfg_obj_type(node->id));
		return 0;
	}
	
	if (type->flag & CFG_TYPE_FLAG_MEMORY)	
		goto next;

	
	if (type->flag & CFG_TYPE_FLAG_MULTIPLE)
		fprintf(fp,"<%s%d",type->name,cfg_obj_id(node->id) - 1);
	else
	{
		if(strcmp(type->name, "root") == 0)
			fprintf(fp,"<ROMFILE");
		else
			fprintf(fp,"<%s",type->name);
	}

	num = xmlnode->value.element.num_attrs;

	for(i=0; i< num; i++){
		memset(replacedstr, 0, sizeof(replacedstr));
		fprintf(fp," %s=\"%s\"",xmlnode->value.element.attrs[i].name, 
			xmlFormatFunc(xmlnode->value.element.attrs[i].value, replacedstr, sizeof(replacedstr)));
	}

	if (num > 0)
		fprintf(fp," ");
	
	if(node->child)	
	{
		fprintf(fp,"> \n");	
		cfg_obj_save_tree(fp,node->child,mode);
		if (type->flag & CFG_TYPE_FLAG_MULTIPLE)
			fprintf(fp,"</%s%d> \n",type->name,cfg_obj_id(node->id) - 1);
		else
		{
			if(strcmp(type->name, "root") == 0)
				fprintf(fp,"</ROMFILE> \n");
			else
				fprintf(fp,"</%s> \n",type->name);
		}
	}
	else
	{
		fprintf(fp,"/> \n");
	}

next:	
	if (node->brother)
		cfg_obj_save_tree(fp,node->brother,mode);

	return 0;
}



static int cfg_obj_dump_attr(cfg_node_obj_t* obj,int tabs, int flag)
{
	cfg_node_type_t* type;
	int i,num;
	int j;
	int k;
	mxml_node_t* node = obj->private;
	
	if ((type = cfg_type_from_oid(obj->id))== NULL)
	{
		printf("cfg_obj_dump_attr: error obj [%d:%d] \n", cfg_obj_type(obj->id),cfg_obj_id(obj->id));
		return -1;
	}
	k = j = tabs;
	if(j > 0)
	{
		printf("\t");
	}
	while(tabs--) printf("\t");
	printf("OID [%d:%d][%s]:\n",cfg_obj_type(obj->id),cfg_obj_id(obj->id),type->name);

	num = node->value.element.num_attrs;
	
	for(i=0; i< num; i++)
	{
		j = k;
		if(j > 0)
		{
			printf("\t");
		}
		while(j--) 
		{
			printf("\t");
		}
		if(flag)
		{
			printf("\t");
		}
		printf("\t[%s = %s]\n",node->value.element.attrs[i].name,node->value.element.attrs[i].value);
	}
	
	printf("\n");

	return 0;
}

 int cfg_obj_dump_obj(cfg_node_obj_t* obj,int depth)
{
	static int tabs = 0;
	
	if (obj == NULL)
		return 0;
	
	printf("\t");
	
	cfg_obj_dump_attr(obj,tabs, 1);
	tabs++;

	if (depth)
		cfg_obj_dump_obj(obj->child,depth-1);

	tabs--;
	cfg_obj_dump_obj(obj->brother,depth);
	
	return 0;
}

int cfg_obj_dump(char* path,int level)
{
	cfg_node_obj_t* node;

	if (level <= 0)
		level = 1;
	
	cfg_obj_lock();

	if ((node = cfg_obj_get_obj(path)) == NULL)
		goto unlock;
		
	cfg_obj_dump_attr(node,0, 0);
	cfg_obj_dump_obj(node->child,level - 1);
	
	
unlock:	
	cfg_obj_unlock();
	return 0;
}

static int cfg_get_process_name(char* path, int size)
{
	int len = 0;

	memset(path, 0, size);
	
	if ((len = readlink("/proc/self/exe", path, size - 1)) < 0){
		path[0] = '\0';
		return 0;
	}

	path[len] = '\0';
	
	return 0;
}

static void __attribute__  ((constructor)) cfg_obj_lib_init(void);

static void cfg_obj_lib_init(void)
{
	char path[64];
	
	if (shm_lib_init() < 0)
		goto Exit;

	if (cfg_type_init() < 0)
		goto Exit;	

	cfg_obj_root = shm_gettree();


	return;
Exit:
	cfg_get_process_name(path, sizeof(path));
	printf("CFG lib: [%s] init fail \n",path);
	exit(0);
}

#if 0
void cfg_obj_lib_deinit(void)
{


	return;
}
#endif

int cfg_load_web_type_node(void)
{
	int i = 0;
	struct cfg_node_type* type = NULL;
	struct cfg_node_type* root = cfg_type_get_root();
	char path[128] = {0};
	char attr[32] = {0};
	char value[1024] = {0};
			
	type = root->subtype[i];
	while(type != NULL){
		snprintf(path,sizeof(path),"root.%s",type->name);
		if ((type->flag & CFG_TYPE_FLAG_MEMORY) && type->ops && type->ops->memory_type_read){
			type->ops->memory_type_read(path,attr,value,1024);
		}
		i++;
		type = root->subtype[i];
	}

	return 0;
}

int cfg_delete_node(void)
{
	int i = 0;
	struct cfg_node_type* type = NULL;
	struct cfg_node_type* root = cfg_type_get_root();
	char nodepath[64] = {0};
	
	for(i=0; root->subtype[i] != NULL; i++)
	{
		type = root->subtype[i];
		snprintf(nodepath, sizeof(nodepath), "root.%s", type->name);
		cfg_obj_delete_object(nodepath);
		cfg_obj_create_object(nodepath);
	}
	return 0;
}

void cfg_readAll(void)
{	
	cfg_delete_node();

#if defined(TCSUPPORT_BACKUPROMFILE)
	/*
		note:
		if execute function tc_parse_romfile() succeed,the function tc_parse_romfile() must return the true tree struct
	*/
	tc_parse_romfile();
#endif

	cfg_load_web_type_node();
}/*end tcapi_readAll_req*/

