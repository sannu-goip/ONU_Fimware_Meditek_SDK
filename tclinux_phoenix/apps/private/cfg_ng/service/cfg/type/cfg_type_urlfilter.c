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
#include "cfg_types.h" 
#include <svchost_evt.h>
#include "utility.h" 

#define TMP_URL_FILE	"/tmp/urladdr"
#if defined(TCSUPPORT_CT_E8GUI)
#define MAX_URL_RULE 100
#else
#define MAX_URL_RULE 16
#endif

int svc_cfg_boot_urlfilter(void)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_URLFILTER_BOOT, NULL, 0); 
	return 0;
}

int cfg_type_urlfilter_entry_commit(char* path)
{
	wan_related_evt_t param;
	char pAttr[300] = {0};
	char strDel[300] = {0};
	char Activate[8] = {0};
	char list_type[4] = {0};

	/*set node to be UrlFilter_Common*/	
	if (cfg_query_object(URLFILTER_COMMON_NODE,NULL,NULL) <= 0)
	{
		printf("Error--UrlFilter Common is NULL.\n");
		return FAIL;
	}
	/*get UrlFilter_Common Activate value*/
	cfg_get_object_attr(URLFILTER_COMMON_NODE, "Activate", Activate, sizeof(Activate));
	if(Activate[0] == '\0')
	{
		printf("Error--UrlFilter_Common Activate value is NULL.\n");
		return FAIL;
	}

	cfg_get_object_attr(URLFILTER_COMMON_NODE, "Filter_Policy", list_type, sizeof(list_type));
	if(list_type[0] == '\0')
	{
		printf("Error--UrlFilter_Common Filter_Policy value is NULL.\n");
		return FAIL;
	}

	/*check attribute action and do unset action*/
	cfg_get_object_attr(URLFILTER_COMMON_NODE, "Action", pAttr, sizeof(pAttr));
	if(pAttr[0] != '\0')
	{
		if(!strncmp(pAttr, "Del", sizeof(pAttr)))
		{
			memset(pAttr, 0, sizeof(pAttr));
			cfg_get_object_attr(URLFILTER_COMMON_NODE, "DeleteIndex", pAttr, sizeof(pAttr));
			if(pAttr[0] != '\0'){
				strncpy(strDel, pAttr, sizeof(strDel)-1);
				unset_action(strDel, DEL_URL);
			}
		}			
	}
	
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_URLFILTER_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}


static int cfg_type_urlfilter_entry_func_get(char* path,char* attr, char* val,int len) 
{ 		
	char urlNodeName[64] = {0};
	int static_url_index = 0;
	char addnum[4] = {0};
	char count[4] = {0};
	int i = 0;
	int norecord = 0;
	
	FILE *fp = NULL;
	char p[50] = {0};
	char curl[48] = {0};
	char tmpbuf[50] = {0};
	
	/*printf("[%s]%d: path = %s\n", __FUNCTION__, __LINE__, path);*/

	if(strcmp(attr,"Url_num") && strcmp(attr,"add_num"))
	{
		goto default_get;
	}
	
	if (cfg_query_object(URLFILTER_NODE,NULL,NULL) <= 0)
	{
		printf("\r\nurl_filter_read:return fail\n");
		goto default_get;
	}

	fp= fopen(TMP_URL_FILE, "w");
	if(fp== NULL){
		printf("url_filter_read:Error--UrlFilter's temp file is NULL.\n");
		goto default_get;
	}
	
	memset(urlNodeName, 0, sizeof(urlNodeName));
			
	for(i = 1; i <= MAX_URL_RULE; i++)
	{
		snprintf(urlNodeName, sizeof(urlNodeName), URLFILTER_ENTRY_N_NODE, i);
		
		/*printf("[%s]%d: urlNodeName = %s\n", __FUNCTION__, __LINE__, urlNodeName);*/
		if (cfg_query_object(urlNodeName,NULL,NULL) <= 0)
		{
			/*1.find the entry index which can be used*/
			/*printf("[%s]%d: i = %d\n", __FUNCTION__, __LINE__, i);*/
			if(!norecord)
			{
				snprintf(addnum, sizeof(addnum), "%d", i-1);
				cfg_set_object_attr(URLFILTER_NODE, "add_num", addnum);
				norecord++;
			}
		}
		else
		{
			memset(count,0x00, sizeof(count));
			snprintf(count, sizeof(count), "%d\n", i - 1);
			fputs(count, fp);
			/*2.get Url value and save in file*/
			memset(p, 0, sizeof(p));
			cfg_obj_get_object_attr(urlNodeName, "URL", 0, p, sizeof(p));
			if(p[0] != '\0')
			{
				strncpy(curl, p, sizeof(curl)-1);
				snprintf(tmpbuf, sizeof(tmpbuf), "%s\n", curl);
				fputs_escape(tmpbuf, fp);	
			}
			else
			{
				fputs("\n", fp);
			}
			static_url_index++;
		}
	}
	fclose(fp);
	fp=NULL;
	memset(count,0x00, sizeof(count));
	snprintf(count, sizeof(count), "%d", static_url_index);
	cfg_set_object_attr(URLFILTER_NODE, "Url_num", count);

default_get:
	return cfg_type_default_func_get(path,attr,val,len); 
} 

static char* cfg_type_urlfilter_index[] = { 
	 "url_filter_id", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_urlfilter_entry_ops  = { 
	 .get = cfg_type_urlfilter_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_urlfilter_entry_commit 
}; 


static cfg_node_type_t cfg_type_urlfilter_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 100, 
	 .parent = &cfg_type_urlfilter, 
	 .index = cfg_type_urlfilter_index, 
	 .ops = &cfg_type_urlfilter_entry_ops, 
}; 


static int cfg_type_urlfilter_common_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_urlfilter_entry_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_urlfilter_common_ops  = { 
	 .get = cfg_type_urlfilter_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_urlfilter_entry_commit 
}; 


static cfg_node_type_t cfg_type_urlfilter_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_urlfilter, 
	 .ops = &cfg_type_urlfilter_common_ops, 
}; 


static cfg_node_ops_t cfg_type_urlfilter_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_urlfilter_entry_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_urlfilter_entry_commit 
}; 


static cfg_node_type_t* cfg_type_urlfilter_child[] = { 
	 &cfg_type_urlfilter_entry, 
	 &cfg_type_urlfilter_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_urlfilter = { 
	 .name = "UrlFilter", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_urlfilter_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_urlfilter_child, 
	 .ops = &cfg_type_urlfilter_ops, 
}; 
