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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 

#define DEVICE_INFO_FILE         "/etc/devices.conf"
#define COUNT_LENGTH 8
#define OUI_LENGTH 6
#define SN_LENGTH 64
#define PCLASS_LENGTH 64

unsigned long get_dhcpClient_count_vlaue(char *str, char *keyword, int length, int base)
{
	char *pch_itemname = NULL;
	char *pch_temp = NULL;
	char value[11]={0};
	
	pch_itemname = strstr(str, keyword);
	if (pch_itemname != NULL){
		pch_temp = strstr(pch_itemname,"= ");
		if (pch_temp != NULL){
			strncpy(value,pch_temp+2, length);
			return strtoul(value,NULL, base);
		}
	}
	return 0;
}/*end get_key_value*/

int get_dhcpClient_Info_vlaue(char *str, char *keyword, int length, char *val)
{
	char *pch_itemname = NULL;
	char *pch_temp = NULL;
	
	pch_itemname = strstr(str, keyword);
	if (pch_itemname != NULL){
		pch_temp = strstr(pch_itemname,"= ");
		if (pch_temp != NULL){
			strncpy(val,pch_temp+2, length);
			val[length] = '\0';
			return SUCCESS;
		}
	}
	return FAIL;
}/*end get_key_value*/


static int cfg_type_dhcpclient_read(char* path,char* attr, char* val,int len) 
{
	unsigned long iCount = 0;
	char attrStr[16] = {0};
	char *buf = NULL;
	char value[65] = {0}; 
	struct stat fileStat;
	unsigned long i;
	char nodePath[64] = {0};

	snprintf(nodePath, sizeof(nodePath), DHCPCLIENT_INFO_NODE);
	if(cfg_query_object(nodePath, NULL, NULL) < 0)
	{
		cfg_create_object(nodePath); 
	}
	
	/*set attribute: Count for default value */
	snprintf(value, sizeof(value), "%lu", iCount);
	cfg_set_object_attr(nodePath, "Count", value);
	
	memset(&fileStat, 0, sizeof(struct stat));
	if(stat(DEVICE_INFO_FILE, &fileStat) != 0) 
	{
		return FAIL;
	}
	if(fileStat.st_size == 0)
	{
		return FAIL;
	}
		
	buf = (char *)malloc(fileStat.st_size + 1);
	if(buf==NULL)
	{
		return FAIL;
	}
	memset(buf,0, fileStat.st_size + 1);
	fileRead(DEVICE_INFO_FILE, buf, fileStat.st_size);
	if(0 == strlen(buf))
	{
		free(buf);
		return FAIL;
	}
	/*get total count of Dhcp client */
	iCount = get_dhcpClient_count_vlaue(buf, "totalCount", COUNT_LENGTH, 10);

	/*set attribute: Count */
	if(0 != iCount)
	{
		snprintf(value, sizeof(value), "%lu", iCount);
		cfg_set_object_attr(nodePath, "Count", value);
	}
	
	/*set Attributes: oui(i) sn(i) pclass(i) */
	for(i = 0; i < iCount; i++)
	{
		/*set oui value */
		memset(value, 0, sizeof(value));
		snprintf(attrStr, sizeof(attrStr), "oui%lu", i);
		get_dhcpClient_Info_vlaue(buf, attrStr, OUI_LENGTH, value);
		cfg_set_object_attr(nodePath, attrStr, value);

		/*set sn value */
		memset(value, 0, sizeof(value));
		snprintf(attrStr, sizeof(attrStr), "sn%lu", i);
		get_dhcpClient_Info_vlaue(buf, attrStr, SN_LENGTH, value);
		cfg_set_object_attr(nodePath, attrStr, value);

		/*set pclass value */
		memset(value, 0, sizeof(value));
		snprintf(attrStr, sizeof(attrStr), "pclass%lu", i);
		get_dhcpClient_Info_vlaue(buf, attrStr, PCLASS_LENGTH, value);
		cfg_set_object_attr(nodePath, attrStr, value);
	}

	free(buf);
	return SUCCESS;
}/*end info_read*/

static int cfg_type_dhcpclient_func_get(char* path,char* attr, char* val,int len) 
{	
	cfg_type_dhcpclient_read(path, attr, val, len);
	return cfg_type_default_func_get(path,attr,val,len);
}

static cfg_node_ops_t cfg_type_dhcpclient_info_ops  = { 
	 .get = cfg_type_dhcpclient_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_dhcpclient_info = { 
	 .name = "Info", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpclient, 
	 .ops = &cfg_type_dhcpclient_info_ops, 
}; 

static cfg_node_ops_t cfg_type_dhcpclient_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t* cfg_type_dhcpclient_child[] = { 
	 &cfg_type_dhcpclient_info, 
	 NULL 
}; 

cfg_node_type_t cfg_type_dhcpclient = { 
	 .name = "DhcpClient", 
	 .flag = CFG_TYPE_FLAG_UPDATE , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpclient_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpclient_child, 
	 .ops = &cfg_type_dhcpclient_ops, 
}; 

