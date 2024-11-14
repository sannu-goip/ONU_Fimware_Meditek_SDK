

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
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <mxml.h>
#include "cfg_cli.h"


#define MAX_NODE_NAME		32
#define MAX_NODE_LEVEL		4
#define CFG_WEB_INDEX_NODE 	"root.webcurset.entry"

int cfg_create_object(char* path)
{

	struct cfg_node_type* type;

	type = cfg_type_query(path);

	if (type == NULL)
		return -1;

	if (type->ops && type->ops->create)
		return type->ops->create(path);
	
	return -1;
}

int cfg_check_create_object(char* path)
{	
	struct cfg_node_type* type = NULL;
	if (cfg_obj_query_object(path,NULL,NULL) <= 0)
	{
		type = cfg_type_query(path);
		if (type != NULL && cfg_type_is_dynamic_type(type->id))
		{
			if (cfg_obj_create_object(path) <= 0)
			{
				printf("create %s Fail.\n", path);
				return -1;
			}
		}else{
			return -1;
		}
	}
	return 0;	
}

int cfg_delete_object(char* path)
{

	struct cfg_node_type* type;

	type = cfg_type_query(path);

	if (type == NULL)
		return -1;

	if (type->ops && type->ops->delete)
		return type->ops->delete(path);
	
	return -1;
}



static int cfg_commit_lock(char* path)
{
	char fpath[128],node[128];
	int fd= 0, res = 0;
	char* tmp = NULL;

	strncpy(node,path, sizeof(node) - 1);
	
	tmp = strchr(node,CFG_NODE_SPELIRATE_CHAR);
	
	if(tmp == NULL)
		return -1;
	
	tmp = strchr(tmp + 1,CFG_NODE_SPELIRATE_CHAR);
	
	if(tmp)
		*tmp = 0;
	
	sprintf(fpath,"/tmp/%s.lock",node);
	
	if ((fd = open(fpath,O_RDWR | O_CREAT)) <0)
		return -1;

	res = flock(fd,LOCK_EX);
	
	return fd;
}

static int cfg_commit_unlock(int fd)
{
	int res = 0;
	
	res = flock(fd,LOCK_UN);
	
	close(fd);
	
	return 0;
}

int cfg_commit_object(char* path)
{

	struct cfg_node_type* type;
	int ret,fd;
	
	type = cfg_type_query(path);

	if (type == NULL)
		return -1;

	if (type->ops == NULL)
		return -1;
	
	if(type->ops->commit == NULL)
		return -1;

	if ((fd = cfg_commit_lock(path)) < 0)
		return -1;
	
	ret =  type->ops->commit(path);
	
	cfg_commit_unlock(fd);
	
	return ret;
}	



int cfg_query_object(char* path, char* subtype, int* id)
{
	struct cfg_node_type* node;

	node = cfg_type_query(path);

	if (node == NULL)
		return -1;

	if (node->ops && node->ops->query)
		return node->ops->query(path,subtype,id);
	
	return -1;
}

static int cfg_debug_on(void)
{

	FILE* fp;

	if ((fp = fopen("/tmp/cfg_debug_on","r")) == NULL)
		return 0;

	fclose(fp);
	
	return 1;
}

int cfg_get_object_attr(char* path,char* attr,char* val,int size)
{
	struct cfg_node_type* type;

	if (cfg_debug_on())
		printf("cfg_get_object_attr: [%s] [%s] \n",path,attr);
	
	type = cfg_type_query(path);
	
	if (type == NULL)
		return -1;
	
	if (type->ops && type->ops->get)
		return type->ops->get(path,attr,val,size);

	return -1;
}


int cfg_set_object_attr(char* path,char* attr,char* val)
{
	struct cfg_node_type* type;
	
	if (cfg_debug_on())
		printf("cfg_set_object_attr: [%s] [%s] [%s] \n",path,attr,val);

	type = cfg_type_query(path);

	if (type == NULL)
		return -1;
	
	if (type->ops && type->ops->set)
		return type->ops->set(path,attr,val);

	return -1;
}


int cfg_save_file(char* file)
{
	char cmdbuf[128];

/*	sprintf(cmdbuf,"mtd -f write %s reservearea",file); */

	sprintf(cmdbuf,"cp -f %s %s",file,CFG_OBJ_FILE_PATH);

	system(cmdbuf);
	
	return 0;
}

int cfg_save_object(char* file)
{

	FILE* fp;
	int ret = 0;
	
	if ((fp = fopen(file,"w")) == NULL)
	{
		printf("cfg_save_object: open file [%s] fail \n",file);
		return -1;
	}
	
	shm_pool_lock();

	ret = cfg_obj_save_tree(fp,cfg_obj_get_root(),1); 

	shm_pool_unlock();
	
	fclose (fp);
	
	return ret;
}

int cfg_load_file(char* file)
{

	FILE* fp,*tmpfp;	

	int ch;
	
	system("/userfs/bin/mtd readflash /tmp/cfg_obj_tmp 131072 0 reservearea");

	if ((tmpfp = fopen("/tmp/cfg_obj_tmp","r")) == NULL)
	{
		printf("cfg_save_object: open file /tmp/cfg_obj_tmp fail \n");
		return -1;
	}
	
	if ((fp = fopen(file,"w")) == NULL)
	{
		printf("cfg_save_object: open file [%s] fail \n",file);
		fclose(tmpfp);
		return -1;
	}

	ch = fgetc(tmpfp);
	
	while(ch != EOF && ch != 0xff)
	{
		fputc(ch,fp);
		ch = fgetc(tmpfp);
	}
	
	fclose(fp);
	fclose(tmpfp);
	
	return 0;
}


int cfg_xml_node_to_path(mxml_node_t * node,char* path)
{
	char tmp[CFG_MAX_NODE_LEVEL][CFG_MAX_TYPE_NAME];
	int i = 0, len = 0;
	int k = 0,num = 0;

	while(node != NULL && i < CFG_MAX_NODE_LEVEL)
	{
		strncpy(&tmp[i][0],node->value.element.name, sizeof(tmp[0]) - 1);
		i++;
		node = node->parent;
	}
	
	if (i >= 1)
		strncpy(&tmp[i-1][0],cfg_type_get_root()->name, sizeof(tmp[0]) - 1);

	while(k < i && k < CFG_MAX_NODE_LEVEL)
	{
		cfg_str_to_lower(&tmp[k][0]);
		
		if (memcmp(&tmp[k][0],"entry",5) == 0 && tmp[k][5] >= '0' && tmp[k][5] <= '9')
		{
			num = atoi(&tmp[k][5]) + 1;
			sprintf(tmp[k],"entry.%d",num);
		}
		
		if (memcmp(&tmp[k][0],"pvc",3) == 0 && tmp[k][3] >= '0' && tmp[k][3] <= '9')
		{
			num = atoi(&tmp[k][3]) + 1;
			sprintf(tmp[k],"pvc.%d",num);	
		}
		
		k++;
	}


	while(i > 0)
	{
		i--;
		len += sprintf(path + len,"%s.",&tmp[i][0]);
	}
	
	path[len - 1] = 0;
	
	return 0;

}

int cfg_load_from_xml(mxml_node_t* node)
{
	int exist= 0;
	char path[CFG_MAX_PATH_NAME] = {0};
	int num = 0, i = 0;
	cfg_node_type_t* type;
	
	if (node == NULL)
		return 0;
	
	cfg_xml_node_to_path(node,path);
	
	exist = cfg_obj_query_object(path,NULL,NULL);

	if (exist <= 0)
	{
		if ((type = cfg_type_query(path)) == NULL)
			return cfg_load_from_xml(node->next);;
		
		if (cfg_type_is_dynamic_type(type->id))
		{
			if (cfg_obj_create_object(path) > 0)
 				exist = 1;
		}
	}
	
	if (exist > 0)
	{
		/*printf("cfg_load_from_xml: load object [%s][%d] ...... \n",path,exist);*/
		num = node->value.element.num_attrs;
		for(i=0; i< num; i++)
		{
			/*printf("cfg_load_from_xml: set attr [%s] [%s] [%s] \n",path,node->value.element.attrs[i].name,node->value.element.attrs[i].value);*/
			cfg_obj_set_object_attr(path, node->value.element.attrs[i].name,0,node->value.element.attrs[i].value);
		}
		cfg_load_from_xml(node->child);	
	}
	
	cfg_load_from_xml(node->next);
	
	return 0;
}


int cfg_load_object(char* file)
{
	mxml_node_t* xmlnode = NULL;
	FILE* fp = NULL;
	int ret = -1, res = -1;
	char cmd[64] = {0};
	if ((fp = fopen(file,"r")) == NULL)
	{
		tcdbg_printf("cfg_load_object: open file [%s] fail \n",file);
		return ret;
	}
	
	if ((xmlnode = mxmlLoadFile(NULL, fp, MXML_IGNORE_CALLBACK)) == NULL)
	{
		tcdbg_printf("cfg_load_object: load [%s] xml fail, will save it to [/tmp/load_fail_romfile.cfg]. \n",file);
		snprintf(cmd, sizeof(cmd), "cp %s /tmp/load_fail_romfile.cfg", file);
		system(cmd);
		goto exit;
	}
	
	ret = cfg_load_from_xml(xmlnode);
	
exit:
	if (xmlnode)
		res = mxmlRelease(xmlnode);

	if (fp)
		fclose(fp);
	
	return ret;
}

static int  cfg_parse_entry_pvc_number(char* path,char* name)
{
	int num = -1;
	struct cfg_node_type* type;
	char** index;
	int i = 0;
	char val[32];
	
	type = cfg_type_query(path);
	
	if (type == NULL)
		return 0;
	
	if ((type->flag & CFG_TYPE_FLAG_MULTIPLE) == 0)
		return 1;
	
	if (memcmp(name,"entry",5) == 0 && name[5] > 0)
		num = atoi(&name[5]);
	else if (memcmp(name,"pvc",3) == 0 && name[3] > 0)
		num = atoi(&name[3]);
	else if (memcmp(name,"rai",3) == 0 && name[3] > 0)
		num = atoi(&name[3]);
	else if (memcmp(name,"ra",2) == 0 && name[2] > 0)
		num = atoi(&name[2]);
	
	if (num >= 0)
		return num+1;

	index = type->index;
	if (index)
	{
		i = 0;
		while(index[i])
		{
			memset(val, 0, sizeof(val));
			if (cfg_obj_get_object_attr(CFG_WEB_INDEX_NODE,index[i],0,val,sizeof(val)) > 0)
				break;
			i++;		
		}
		if (index[i])
			return atoi(val) + 1;
	}
	

	return 0;
}

static int  cfg_parse_split_name(char *name, char buf[][MAX_NODE_NAME], const char* delim)
{
	char *pValue=NULL;
	char *safep1 = NULL;
	int i=0;

	strtok_r(name,delim, &safep1);
	strncpy(buf[0], name, MAX_NODE_NAME-1);
	for(i=1; i < MAX_NODE_LEVEL; i++){
		pValue=strtok_r(NULL, delim, &safep1);
		if(pValue){
			strncpy(buf[i], pValue, sizeof(buf[i]) - 1);
		}
		else{
			break;
		}
	}
	return i;
}



int cfg_parse_node_name(char* name,char* path,int size)
{
	int len = strlen(name);
	char buf[MAX_NODE_LEVEL][MAX_NODE_NAME];
	int num,level;
	char* rname;
	char* ptmp = NULL;
	
	if (len > size)
		return -1;

	strcpy(path,name);

	cfg_str_to_lower(path);
	
	level = cfg_parse_split_name(path,buf,"_");
	
	if (level < 1 || level > 3)
		return -1;
	
	rname = cfg_type_get_root()->name;

	if (level == 1)
	{
		len = sprintf(path,"%s.%s",rname,buf[0]);
	}
	else if (memcmp(buf[1],"entry",5) == 0)
	{
		len = sprintf(path,"%s.%s.entry",rname,buf[0]);
		num = cfg_parse_entry_pvc_number(path,buf[1]);
		len += sprintf(path + len,".%d",num);
	}
	else if (memcmp(buf[1],"pvc",3) == 0){
		len = sprintf(path,"%s.%s.pvc",rname,buf[0]);
		num = cfg_parse_entry_pvc_number(path,buf[1]);
		len += sprintf(path + len,".%d",num);
	}
	else if (memcmp(buf[0],"info",4) == 0){
		if (memcmp(buf[1],"nas",3) == 0){
			if((NULL != (ptmp = strchr(buf[1], '.'))) && ('\n' != *(ptmp + 1))
								&& ('\0' != *(ptmp + 1)) && ('\r' != *(ptmp + 1))){
				*(ptmp + 1) = *(ptmp + 1) + 1;
			}
			len = sprintf(path,"%s.%s.%s",rname,buf[0],buf[1]);
		}
		else if (memcmp(buf[1],"rai",3) == 0){
			len = sprintf(path,"%s.%s.rai",rname,buf[0]);
			num = cfg_parse_entry_pvc_number(path,buf[1]);
			len += sprintf(path + len,".%d",num);
		}
		else if (memcmp(buf[1],"ra",2) == 0){
			len = sprintf(path,"%s.%s.ra",rname,buf[0]);
			num = cfg_parse_entry_pvc_number(path,buf[1]);
			len += sprintf(path + len,".%d",num);
		}
		else{
			len = sprintf(path,"%s.%s.%s",rname,buf[0],buf[1]);
		}
	}
	else{
		len = sprintf(path,"%s.%s.%s",rname,buf[0],buf[1]);
	}

	if (level == 3) 
	{
		if (memcmp(buf[2],"entry",5) == 0){
			len += sprintf(path + len,".entry");
			num = cfg_parse_entry_pvc_number(path,buf[2]);	
			len += sprintf(path + len,".%d",num);	
		}else{
			len += sprintf(path + len,".%s",buf[2]);
		}
	}

	return 0;
}



