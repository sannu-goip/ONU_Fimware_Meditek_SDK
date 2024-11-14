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
#include "utility.h"
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h" 


double transTemperature(char * tmp){

	double num = 0;
	if(NULL == tmp || !isNumber(tmp)){
		return 0;
	}else{
		num = abs(atoi(tmp));
	}
	num = abs(atoi(tmp));

	if(num >= pow(2,15)){
		return 0 - (pow(2,16) - num/256);
	}else{
		return num/256;
	}	
}


int cfg_type_devinftem_get(char* path,char* attr,char* val,int len) {
	char temNodeName[MAXLEN_NODE_NAME] = {0};
	int entryNum = 1;
	int ertryidx = -1;
	char tmp[64] = {0};
	char buf[64] = {0};
	char temperature[32] = {0};
	if(cfg_query_object(TR181_DEVINF_TEM_NODE,NULL,NULL) <= 0)
	{
		return -1;
	}
	
	/*Temperature of CPU*/
	memset(temNodeName, 0, sizeof(temNodeName));
	snprintf(temNodeName, sizeof(temNodeName),TR181_DEVINF_TEM_ENTRY_NODE, entryNum);
	if(cfg_query_object(temNodeName,NULL,NULL) <= 0)
	{		
		ertryidx = cfg_obj_create_object(temNodeName);
		if(ertryidx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,temNodeName);
			return -1;
		}
	}
	cfg_obj_set_object_attr(temNodeName, "Name",0, "CPUTemperature");
	entryNum++;
	
	/*Temperature of Module*/
	memset(temNodeName, 0, sizeof(temNodeName));
	snprintf(temNodeName, sizeof(temNodeName),TR181_DEVINF_TEM_ENTRY_NODE, entryNum);
	if(cfg_query_object(temNodeName,NULL,NULL) <= 0)
	{	
		ertryidx = cfg_obj_create_object(temNodeName);
		if(ertryidx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,temNodeName);
			return -1;
		}
	}
	cfg_obj_set_object_attr(temNodeName, "Name",0, "ModuleTemperature");
	entryNum++;
	
	/*Temperature of Board*/
	memset(temNodeName, 0, sizeof(temNodeName));
	snprintf(temNodeName, sizeof(temNodeName),TR181_DEVINF_TEM_ENTRY_NODE, entryNum);
	if(cfg_query_object(temNodeName,NULL,NULL) <= 0)
	{	
		ertryidx = cfg_obj_create_object(temNodeName);
		if(ertryidx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,temNodeName);
			return -1;
		}
	}
	cfg_obj_set_object_attr(temNodeName, "Name",0, "BoardTemperature");
	entryNum++;
	
	/*Temperature of PON*/
	memset(temNodeName, 0, sizeof(temNodeName));
	snprintf(temNodeName, sizeof(temNodeName),TR181_DEVINF_TEM_ENTRY_NODE, entryNum);
	if(cfg_query_object(temNodeName,NULL,NULL) <= 0)
	{
		ertryidx = cfg_obj_create_object(temNodeName);
		if(ertryidx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,temNodeName);
			return -1;
		}
	}
	if(cfg_obj_get_object_attr("root.info.ponphy", "Temperature",0, temperature, sizeof(temperature)) >= 0){
		snprintf(tmp,sizeof(tmp), "%u", atoi(temperature));
		snprintf(buf, sizeof(buf),"%lf", transTemperature(tmp));
		cfg_obj_set_object_attr(temNodeName, "Value",0, buf);
	}
	cfg_obj_set_object_attr(temNodeName, "Name",0, "PONTemperature");
	entryNum++;

	/*Temperature Num*/
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp,sizeof(tmp), "%d", entryNum - 1);
	cfg_obj_set_object_attr(TR181_DEVINF_TEM_COMMON_NODE, "Num",0, tmp);

	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_devinftem_func_get(char* path,char* attr,char* val,int len) {
	
	cfg_type_devinftem_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len);

}

static cfg_node_ops_t cfg_type_devinftem_entry_ops  = { 
	 .get = cfg_type_devinftem_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_devinftem_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_FLAG_MEMORY | TR181_DEVINF_TEM_NUM, 
	 .parent = &cfg_type_devinftem, 
	 .ops = &cfg_type_devinftem_entry_ops, 
};

static cfg_node_ops_t cfg_type_devinftem_common_ops  = { 
	 .get = cfg_type_devinftem_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_devinftem_common = { 
	 .name = "Common", 
	 .flag =CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_devinftem, 
	 .ops = &cfg_type_devinftem_common_ops, 
}; 


static cfg_node_ops_t cfg_type_devinftem_ops  = { 
	 .get = cfg_type_devinftem_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_devinftem_child[] = { 
	 &cfg_type_devinftem_common, 
	 &cfg_type_devinftem_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_devinftem= { 
	 .name = "DevInfTem", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_devinftem_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_devinftem_child, 
	 .ops = &cfg_type_devinftem_ops, 
}; 
