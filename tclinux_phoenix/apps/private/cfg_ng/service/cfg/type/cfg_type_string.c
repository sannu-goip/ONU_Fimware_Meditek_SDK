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
#include <unistd.h>
#include <stdlib.h>

#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "cfg_xml.h"
#include "utility.h"

#define STRINGS_PATH	"/userfs/string%s.conf"
#define MAX_VALUE_BUF_SIZE 608


int get_string_value(char *keyname,char *str_return, int size, int type, char *path)
{

	FILE *fp = NULL;
	char *str_key = NULL;
	char stream[MAX_VALUE_BUF_SIZE] = {0};
	int enterOffSet = 1;
	int qmarkLength = 0;
	int skipQmark = 0;
	int totalLength = 0;

	fp=fopen(path,"r");
	if(fp==NULL)
	{
		tcdbg_printf("\r\nCan't open %s\n",path);
		return FAIL;
	}

	memset(str_return, 0, size);
	fseek(fp, 0, SEEK_SET);
	if(type==QMARKS)
	{
		qmarkLength=2;
		skipQmark=1;
	}
	else if(type == NO_QMARKS)
	{
		qmarkLength=0;
		skipQmark=0;
	}
	else
	{
		tcdbg_printf("\r\nThe input qmark type of get_string_value is wrong \n");
		fclose(fp);
		return FAIL;
	}

	while(fgets(stream, MAX_VALUE_BUF_SIZE, fp) != NULL)
	{
		if(strrchr(stream,'\n') == NULL)
		{
			enterOffSet=0;
		}
		else
		{
			enterOffSet=1;
		}

		str_key = strstr(stream,keyname);
		if(str_key == NULL || str_key != stream)
		{
			continue;
		}

		totalLength=strlen(stream)-strlen(keyname)-enterOffSet-qmarkLength -1;
		if(size<totalLength+1) 		/*total length + '\0' should not less than buffer*/
		{
			tcdbg_printf("\r\nToo small buffer to catch the %s frome get_string_value\n", keyname);
			//fclose(fp);
		}
		else if(totalLength<0)		/*can't get a negative length string*/ 
		{
			tcdbg_printf("\r\nNo profile string can get\n");
			fclose(fp);
			return FAIL;
		}
		else
		{
			strncpy(str_return, stream+strlen(keyname)+skipQmark, totalLength);
			str_return[totalLength]= '\0';
			fclose(fp);
			return strlen(str_return);
		}
		memset(stream, 0, MAX_VALUE_BUF_SIZE);
	}
	fclose(fp);
	tcdbg_printf("\r\nFile %s content %s is wrong!!\n",path,keyname);
	return FAIL;
}/* end get_string_value */


int cfg_type_string_entry_read(char* path,char* attr)
{
	char languagetype[2] = {0};
	char stringPath[32] = {0};
	int ret = 0;
	char attrName[36] = {0};
	char attrVal[572]= {0};
	int  getTimes = 0;
    	
	if(path == NULL)
	{
		return FAIL;
	}

	memset(attrVal, 0, sizeof(attrVal));
	if(cfg_obj_get_object_attr(STRING_ENTRY_NODE, attr, 0,attrVal, sizeof(attrVal)) >= 0)
	{
		/*Get the value from shared memory firstly, if successfully,return.Otherwise get it from sting file.*/
		return 0 ;
	}
	
	memset(attrVal, 0, sizeof(attrVal));
	if(cfg_obj_get_object_attr(STRING_COMMON_NODE, "getTimes", 0, attrVal, sizeof(attrVal)) >= 0)	
	{
		getTimes = atoi(attrVal);
	}

	if(cfg_obj_query_object(STRING_ENTRY_NODE, NULL, NULL) > 0)
	{
		/* when get value 100 times, delete the string entry node. */
		if(getTimes == 100)
		{
		cfg_delete_object(STRING_ENTRY_NODE);
			cfg_create_object(STRING_ENTRY_NODE);
			getTimes = 0;
		}	
	}
	else
	{
	cfg_create_object(STRING_ENTRY_NODE);
		getTimes = 0;
	}
	
	if(cfg_obj_get_object_attr(LANGUAGESWITCH_ENTRY_NODE, "Type", 0, languagetype, sizeof(languagetype)) < 0)	
	{
		return FAIL;
	}

	snprintf(stringPath, sizeof(stringPath), STRINGS_PATH, languagetype);
	snprintf(attrName, sizeof(attrName), "%s=", attr );
	ret = get_string_value( attrName, attrVal, sizeof(attrVal), NO_QMARKS, stringPath);
	if ( ret != -1 ) 
	{
		cfg_obj_set_object_attr(STRING_ENTRY_NODE, attr, 0, attrVal);
		getTimes++;
		memset(attrVal, 0, sizeof(attrVal));
		snprintf(attrVal, sizeof(attrVal), "%d", getTimes);
		cfg_obj_set_object_attr(STRING_COMMON_NODE, "getTimes", 0, attrVal);
	}
	else
	{
		tcdbg_printf("\r\nString read error!");
		return FAIL;
	}	
	return SUCCESS;
}/* end String_read */

static int cfg_type_string_entry_func_get(char* path,char* attr, char* val,int len) 
{	
	cfg_type_string_entry_read(path, attr);
	return cfg_type_default_func_get(path,attr,val,len);
}

static cfg_node_ops_t cfg_type_string_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_ops_t cfg_type_string_entry_ops  = { 
	 .get = cfg_type_string_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_string_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY|1, 
	 .parent = &cfg_type_string, 
	 .ops = &cfg_type_string_common_ops, 
}; 

static cfg_node_type_t cfg_type_string_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |CFG_TYPE_FLAG_UPDATE|1, 
	 .parent = &cfg_type_string, 
	 .ops = &cfg_type_string_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_string_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_string_child[] = { 
	&cfg_type_string_common,
	 &cfg_type_string_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_string = { 
	 .name = "String", 
	 .flag = CFG_TYPE_FLAG_MEMORY|1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_string_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_string_child, 
	 .ops = &cfg_type_string_ops, 
}; 


