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


int getCurProcessNum(){
	int entryNum = 0;
	FILE *fp = NULL;
	char buf[128] = {0};

	system("ps > /tmp/procInfo1");	
	fp = fopen("/tmp/procInfo1", "r");	
	if(NULL == fp) {
		tcdbg_printf("\ncan't open file procInfo");
		return -1;
	}
	
	while (fgets(buf, sizeof(buf), fp) != NULL) {	
		entryNum++;		
	}
	fclose(fp);
		
	return entryNum - 1;
}

void clearPSNode(int entryNum){
	int i = 0;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	for(i = 0; i < entryNum; i++){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TR181_DEVINF_PS_ENTRY_NODE , i + 1);
		cfg_delete_object(nodeName);
	}	
	
	cfg_obj_set_object_attr(TR181_DEVINF_PS_COMMON_NODE, "Num",0, "0");
}

void setPSStatus(int entryNum, char * stat) {
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char status[16] = {0};

	if(entryNum < 1 || NULL == stat){
		return ;
	}
	snprintf(nodeName, sizeof(nodeName), TR181_DEVINF_PS_ENTRY_NODE , entryNum);
	if(strcmp("R", stat) == 0){
		strncpy(status, "Running",sizeof(status));
	}else if(strcmp("T", stat) == 0){
		strncpy(status, "Stopped",sizeof(status));
	}else if(strcmp("Z", stat) == 0){
		strncpy(status, "Zombie",sizeof(status));
	}else if(strstr(stat, "S") != NULL){
		strncpy(status, "Sleeping",sizeof(status));
	}else{
		strncpy(status, "Idle",sizeof(status));
	}
	cfg_obj_set_object_attr(nodeName, "Status",0, status);
	return ;
}

int cfg_type_devinfps_get(char* path,char* attr,char* val,int len) {

	FILE *fp = NULL;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int entryNum = 0;
	int  curEntryNum = 0;
	char *str = NULL;
	char tmp[64] = {0};
	char *p = NULL;
	int i = 0;
	char pid_s[10] = {0};
	char size_s[10] = {0};
	char stat_s[10] = {0};
	char cmd_s[512] = {0};
	int  devinfpsEntryIdx = -1;
	if(strcmp(attr, "NumModify")!= 0 ){
		return 0;
	}
	curEntryNum = getCurProcessNum();
		
	if(cfg_obj_get_object_attr(TR181_DEVINF_PS_COMMON_NODE, "Num", 0, tmp, sizeof(tmp)) >= 0 && atoi(tmp) == curEntryNum)	
	{
		cfg_obj_set_object_attr(TR181_DEVINF_PS_COMMON_NODE, "NumModify", 0, "0");
		return 0;		
	}

	clearPSNode(atoi(tmp));
	
	system("ps > /tmp/procInfo");	
	fp = fopen("/tmp/procInfo", "r");	
	if(NULL == fp) {
		tcdbg_printf("\ncan't open file procInfo");
		return -1;
	}
	
	str = malloc(512*sizeof(char));
	if(NULL==str)
	{
		fclose(fp);
		return -1;
	}
	while (fgets(str, 512, fp) != NULL) 
	{		
		if(entryNum != 0)	
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_DEVINF_PS_ENTRY_NODE , entryNum);
			/*get pid*/
			p=str;
			i = 0;
			while(*p==' ')p++;			
			while(*p!=' ')
			{
				pid_s[i]=*p;
				p++;
				i++;
			}
			pid_s[i]='\0';

			if(cfg_query_object(nodeName,NULL,NULL) <= 0)
			{		
				devinfpsEntryIdx = cfg_obj_create_object(nodeName);
				if(devinfpsEntryIdx < 0) 
				{
					tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
					free(str);
					fclose(fp);
					return -1;
				}
			}
			cfg_obj_set_object_attr(nodeName, "PID",0, pid_s);

			while(*p==' ')p++;
			while(*p!=' ')p++;
			

			/*get size*/
			while(*p==' ')p++;				
			i=0;
			while(*p!=' ')
			{
				size_s[i]=*p;
				p++;
				i++;
			}
			size_s[i]='\0';
			if(atoi(size_s) > 0){
				cfg_obj_set_object_attr(nodeName, "Size",0, size_s);
				/*get status*/
				while(*p==' ')p++;
				i = 0;
				while(*p!=' ')
				{
					stat_s[i]=*p;
					p++;
					i++;
				}
				stat_s[i]='\0';
				setPSStatus(entryNum , stat_s);
			}else{	
				cfg_obj_set_object_attr(nodeName, "Size",0, size_s);
				setPSStatus(entryNum , size_s);
			}			

			/*get cmd*/
			while(*p==' ')p++;				
			i=0;
			while(*p!='\0')
			{
				cmd_s[i]=*p;
				p++;
				i++;
			}
			cmd_s[i-1]='\0';
			cfg_obj_set_object_attr(nodeName, "Command",0, cmd_s);
		}
		entryNum++;
	}
	unlink("/tmp/procInfo");
	free(str);
	fclose(fp);

	/*Num*/
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp,sizeof(tmp), "%d", entryNum - 1);
	cfg_obj_set_object_attr(TR181_DEVINF_PS_COMMON_NODE, "Num",0, tmp);
	cfg_obj_set_object_attr(TR181_DEVINF_PS_COMMON_NODE, "NumModify", 0 , "1");
	
	return 0;
}


int cfg_type_devinfps_func_get(char* path,char* attr,char* val,int len) {
	
	cfg_type_devinfps_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len);

}


static cfg_node_ops_t cfg_type_devinfps_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_devinfps_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_FLAG_MEMORY | TR181_DEVINF_PS_NUM, 
	 .parent = &cfg_type_devinfps, 
	 .ops = &cfg_type_devinfps_entry_ops, 
};

static cfg_node_ops_t cfg_type_devinfps_common_ops  = { 
	 .get = cfg_type_devinfps_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_devinfps_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_devinfps, 
	 .ops = &cfg_type_devinfps_common_ops, 
}; 


static cfg_node_ops_t cfg_type_devinfps_ops  = { 
	 .get = cfg_type_devinfps_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_devinfps_child[] = { 
	 &cfg_type_devinfps_common, 
	 &cfg_type_devinfps_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_devinfps = { 
	 .name = "DevInfPS", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_devinfps_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_devinfps_child, 
	 .ops = &cfg_type_devinfps_ops, 
}; 
