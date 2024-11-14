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
#include "../utility.h"

#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE6_NUM 32
#else
#define MAX_STATIC_ROUTE6_NUM 16 
#endif

#define SVC_WAN_BUF_64_LEN		64
#define ROUTE6_BACK_UP_PATH 	"/etc/route6_backup"
#define MAX_ROUTE6_PARA_LEN 	45
#define ROUTE6_ADD 				1
#define ROUTE6_DEL 				2	

enum route6_para_type
{
	DST_IP_TYPE=0,
	PREFIX_LEN_TYPE,
	GATEWAY_TYPE,
	DEVICE_TYPE,
};

static char* cfg_type_route6_index[] = { 
	 "route6_id", 
	 NULL 
}; 

static int wan_related_route6_read(char* path, char *attr)
{
	char routeNodeName[128] = {0};
	cfg_node_type_t* group 	= NULL;  
	int static_route_index 	= 0;
	char addnum[4] 			= {0};
	char count[4] 			= {0};	
	int i = 0;	
	int norecord = 0;
	char value[8];

	/* do this function only the attribute is "Route_num" or "add_num".*/
	if(0 != strcmp(attr, "Route_num") && 0 != strcmp(attr, "add_num"))
	{
		return 0;
	}
	
	group = cfg_type_query(path);
	if(NULL == group)
	{
		tcdbg_printf("\r\nroute6_read:get Route6 node fail\n");
		return -1;
	}

	for(i = 0; i < MAX_STATIC_ROUTE6_NUM; i++)
	{
		snprintf(routeNodeName, sizeof(routeNodeName), ROUTE6_ENTRY_NODE, i+1);
		memset(value,0,sizeof(value));
		cfg_obj_get_object_attr(routeNodeName, "Active", 0, value, sizeof(value));
		if ('\0' == value[0])
		{
			/*1.find the entry index which can be used*/
			if(!norecord)
			{
				snprintf(addnum, sizeof(addnum), "%d", i);
				cfg_set_object_attr(path, "add_num", addnum);
				norecord++;
			}
		}
		else
		{
			/*2.compute the static route number*/
			static_route_index++;
		}	
	}

	/*set route number now*/
	snprintf(count, sizeof(count), "%d", static_route_index);
	cfg_set_object_attr(path, "Route_num", count);
	cfg_set_object_attr(path, "User_def_num", count);

	return 0;
}

static int cfg_type_route6_func_get(char* path,char* attr, char* val,int len) 
{ 
	int ret = -1;

	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
	{
		return -1;
	}

	if(0 == wan_related_route6_read(path, attr))
	{
		ret = cfg_type_default_func_get(path,attr,val,len); 
	}

	return ret; 
} 

static void route6_cmd(int action, char route6_para[][MAX_ROUTE6_PARA_LEN])
{
	char cmd[130] 	= {0};
	char tmp[45] 	= {0};

	switch(action)
	{
		case ROUTE6_ADD:
		{
			snprintf(cmd, sizeof(cmd), "/sbin/route -A inet6 add ");
			break;
		}
		case ROUTE6_DEL:
		{
			snprintf(cmd, sizeof(cmd), "/sbin/route -A inet6 del ");
			break;
		}
		default:
			return ;
	}
	
	if(strlen(route6_para[DST_IP_TYPE]) == 0 || strlen(route6_para[PREFIX_LEN_TYPE]) == 0)
	{
		tcdbg_printf("\n %s:no des ip or prefix length\n", __func__);
		return ;
	}

	snprintf(tmp, sizeof(tmp), "%s/%s ", route6_para[DST_IP_TYPE], route6_para[PREFIX_LEN_TYPE]);
	strcat(cmd, tmp);

	if(strlen(route6_para[GATEWAY_TYPE]) == 0 && strlen(route6_para[DEVICE_TYPE]) == 0)
	{
		tcdbg_printf("\n %s:no gateway or device\n", __func__);
		return ;
	}

	/*Add gateway*/
	if(strcmp(route6_para[GATEWAY_TYPE], "*") && strcmp(route6_para[GATEWAY_TYPE], "") \
		&& strcmp(route6_para[GATEWAY_TYPE], "::"))
	{
		snprintf(tmp, sizeof(tmp), "gw %s ", route6_para[GATEWAY_TYPE]);
		strcat(cmd, tmp);
	}

	/*Add dev*/
	if(strcmp(route6_para[DEVICE_TYPE], "*") && strcmp(route6_para[DEVICE_TYPE], ""))
	{
		snprintf(tmp, sizeof(tmp), "dev %s ", route6_para[DEVICE_TYPE]);
		strcat(cmd, tmp);
	}

	system(cmd);
	
	return ;
}


static int create_route6_backup_file(void)
{
	FILE *fp = NULL;
	char Value[64];
	char nodeName[128] = {0};
	char cmd[128] = {0}, tmp[60] = {0};
	int i;

	fp = fopen(ROUTE6_BACK_UP_PATH, "w");
	if(fp == NULL)
	{
		tcdbg_printf("\n %s:open %s failed!\n", __func__, ROUTE6_BACK_UP_PATH);
		return -1;
	}
	
	for(i = 0; i < MAX_STATIC_ROUTE6_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ROUTE6_ENTRY_NODE, i+1);
		memset(cmd, 0, sizeof(cmd));
		if(cfg_obj_query_object(nodeName, NULL, NULL) <= 0)
		{
			strcpy(cmd, "#");
		}
		else
		{
			/*write destnation IP*/
			memset(Value, 0, sizeof(Value));
			cfg_obj_get_object_attr(nodeName, "DST_IP", 0, Value, sizeof(Value));
			if(strlen(Value) > 0)
			{
				snprintf(tmp, sizeof(tmp), "%s ", Value);
				strcat(cmd, tmp);
			}
			/*write prefix length*/
			memset(Value, 0, sizeof(Value));
			cfg_obj_get_object_attr(nodeName, "Prefix_len", 0, Value, sizeof(Value));
			if(strlen(Value) > 0)
			{
				snprintf(tmp, sizeof(tmp), "%s ", Value);
				strcat(cmd, tmp);
			}
			/*write Gateway information*/
			memset(Value, 0, sizeof(Value));
			cfg_obj_get_object_attr(nodeName, "Gateway", 0, Value, sizeof(Value));
			if(strlen(Value) > 0)
			{
				snprintf(tmp, sizeof(tmp), "%s ", Value);
				strcat(cmd, tmp);
			}
			else
			{				
				strcat(cmd, "* ");
			}
			/*write device information*/
			memset(Value, 0, sizeof(Value));
			cfg_obj_get_object_attr(nodeName, "Device", 0, Value, sizeof(Value));
			if(strlen(Value) > 0)
			{
				snprintf(tmp, sizeof(tmp), "%s ", Value);
				strcat(cmd, tmp);
			}
			else
			{				
				strcat(cmd, "* ");
			}
		}
		strcat(cmd, "\n");
		fputs_escape(cmd, fp);
	}
	
	fclose(fp);
	return 0;
}

static int wan_related_route6_execute(char* path)
{
	int route_index 		= 0;
	int wan_device_index 	= -1;
	int i 					= 0;
	char nodeName[128] 		= {0};
	char str_route_index[5] = {0};	
	char route6_para[4][MAX_ROUTE6_PARA_LEN];
	char wan_active[5] 		= {0};
	char cmd[128] 			= {0};
	FILE *fp				= NULL;

	if(get_entry_number_cfg2(path, "entry.", &route_index) != 0)
	{
		/*get route index from webcurset if resolve Entry failed*/
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		/*get route id*/
		if(cfg_obj_get_object_attr(nodeName, "route6_id", 0, str_route_index, sizeof(str_route_index)) > 0)
		{
			route_index = atoi(str_route_index) + 1;
		}
		else
		{
			return -1;
		}
	}
	/*Step1:Delete static route*/
	fp = fopen(ROUTE6_BACK_UP_PATH, "r");
	if(NULL == fp)
	{
		tcdbg_printf("\n %s:open %s failed!\n", __func__, ROUTE6_BACK_UP_PATH);
		return -1;
	}
	/*Get static route rule from back up file base on route_index*/
	for(i = 0; i < route_index; i++)
	{
		if(NULL == fgets(cmd, 128, fp))
		{
			tcdbg_printf("\n %s:can not read line %d from %s !\n", __func__, route_index, ROUTE6_BACK_UP_PATH);
			fclose(fp);
			return -1;
		}
	}
	
	fclose(fp);

	memset(route6_para, 0, sizeof(route6_para));
	sscanf(cmd, "%s %s %s %s", route6_para[DST_IP_TYPE], route6_para[PREFIX_LEN_TYPE],
							   route6_para[GATEWAY_TYPE], route6_para[DEVICE_TYPE]);
	if(strcmp(route6_para[DST_IP_TYPE], "#"))
	{	
		/*"#" means this entry is emtry befor commiting*/
		route6_cmd(ROUTE6_DEL, route6_para);
	}
	/*Step2:Add new static route*/
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), ROUTE6_ENTRY_NODE, route_index);

	/*Check entry is existed or not*/
	if(cfg_obj_query_object(nodeName, NULL, NULL) > 0)
	{
		memset(route6_para, 0, sizeof(route6_para));
		if(cfg_obj_get_object_attr(nodeName, "DST_IP", 0, route6_para[DST_IP_TYPE], MAX_ROUTE6_PARA_LEN) <= 0 ||
			cfg_obj_get_object_attr(nodeName, "Prefix_len", 0, route6_para[PREFIX_LEN_TYPE], MAX_ROUTE6_PARA_LEN) <= 0)
		{
			tcdbg_printf("\n %s:get attribute failed,dst_ip,prefix_len!\n", __func__);
			return -1;
		}
		cfg_obj_get_object_attr(nodeName, "Gateway", 0, route6_para[GATEWAY_TYPE], MAX_ROUTE6_PARA_LEN);

		/*Check device status*/	
		if(cfg_obj_get_object_attr(nodeName, "Device", 0, route6_para[DEVICE_TYPE], MAX_ROUTE6_PARA_LEN) > 0)
		{
			if((wan_device_index = get_wanindex_by_name(route6_para[DEVICE_TYPE])) < 0)
			{
				tcdbg_printf("\n %s:get wan index from %s failed!\n", __func__, route6_para[DEVICE_TYPE]);
				return -1;
			}

			if(get_waninfo_by_index(wan_device_index, "Active", wan_active, sizeof(wan_active)) == 0)
			{
				if(!strcmp(wan_active, "No"))
				{
					tcdbg_printf("\n %s:%s is not active,add static route failed!\n", __func__, route6_para[DEVICE_TYPE]);
					cfg_obj_delete_object(nodeName);
					return -1;			
				}		
			}
			else
			{
				tcdbg_printf("\n %s:%s is not exist,add static route failed!\n", __func__, route6_para[DEVICE_TYPE]);
				cfg_obj_delete_object(nodeName);
				return -1;	
			}
		}
		route6_cmd(ROUTE6_ADD, route6_para);	
	}
	else
	{
		tcdbg_printf("\n %s:%s is not exit!\n", __func__, nodeName);
	}
	
	/*Step3: Backup static route rule*/
	if(create_route6_backup_file() != SUCCESS)
	{
		tcdbg_printf("\n %s:create backup file failed!\n", __func__);
		return -1;
	}

	return 0;
}

int cfg_type_route6_entry_func_commit(char* path)
{
	int ret = -1;
	FILE *fp = NULL;

	fp = fopen(ROUTE6_BACK_UP_PATH, "r");
	if (fp == NULL)
	{
		create_route6_backup_file();
	}
	else
	{
		close(fp);
	}
	ret = wan_related_route6_execute(path);

#if defined(TCSUPPORT_CWMP_TR181)
	syncRouteFwd6ByRouteNode(path);
#endif
	return ret;
}

static int wan_related_route6_pre_unset(char* path)
{
	int route_index = 0;
	char route6_para[4][MAX_ROUTE6_PARA_LEN];

	if(get_entry_number_cfg2(path, "entry.", &route_index) != 0 )
	{
		return -1;
	}

	memset(route6_para, 0, sizeof(route6_para));
	if(cfg_obj_get_object_attr(path, "DST_IP", 0, route6_para[DST_IP_TYPE], MAX_ROUTE6_PARA_LEN) > 0 &&
		cfg_obj_get_object_attr(path, "Prefix_len", 0, route6_para[PREFIX_LEN_TYPE], MAX_ROUTE6_PARA_LEN) > 0)
	{
		cfg_obj_get_object_attr(path, "Gateway", 0, route6_para[GATEWAY_TYPE], MAX_ROUTE6_PARA_LEN);
		cfg_obj_get_object_attr(path, "Device", 0, route6_para[DEVICE_TYPE], MAX_ROUTE6_PARA_LEN);
		route6_cmd(ROUTE6_DEL, route6_para);
	}

	return 0;
}

int svc_cfg_boot_route6(void)
{
	create_route6_backup_file();
	return 0;
}


int cfg_type_route6_entry_func_delete(char* path)
{
	int ret = -1;

	if (cfg_obj_query_object(path,NULL,NULL) <= 0)
	{
		return -1;
	}

	ret = wan_related_route6_pre_unset(path);
	if(0 == ret)
	{
		cfg_type_default_func_delete(path);
	}

#if defined(TCSUPPORT_CWMP_TR181)
	delRouteFwd6ByRouteNode(path);
#endif

	return ret;	
}

static cfg_node_ops_t cfg_type_route6_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_route6_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_route6_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_route6_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_ROUTE6_ENTRY_NUM, 
	 .parent = &cfg_type_route6, 
	 .index = cfg_type_route6_index, 
	 .ops = &cfg_type_route6_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_route6_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_route6_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_route6_entry_func_commit 
}; 


static cfg_node_type_t* cfg_type_route6_child[] = { 
	 &cfg_type_route6_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_route6 = { 
	 .name = "Route6", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_route6_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_route6_child, 
	 .ops = &cfg_type_route6_ops, 
}; 
