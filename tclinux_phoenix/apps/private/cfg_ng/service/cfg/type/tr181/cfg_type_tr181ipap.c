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

int cfg_type_tr181ipap_get(char* path,char* attr,char* val,int len) {
	FILE *fp = NULL;
	char buf[128] = {0};
	int count = 0;
	int nodeNum = 0;
	char nodeNumStr[10] = {0};
	char tmp[3][64] = {0};
	char nodeName[MAXLEN_NODE_NAME]={0};
	char localIPAddress[16] = {0};
	char localIPort[16] = {0};
	char remoteIPAddress[16] = {0};
	char remotePort[16] = {0};
	char status[16] = {0};
	int  ipapEntryIdx = -1;
	char oldNum[16] = {0};

	if(attr == NULL ){
		return -1;
	}

	if(strcmp(attr, "NumModify") != 0 ){
		return 0;
	}
	system("netstat -ant > /tmp/IPActionPortInfo");
	fp = fopen("/tmp/IPActionPortInfo", "r");
	if(fp == NULL) {
		tcdbg_printf("\ncan't open file IPActionPortInfo");
		return -1;
	}

	while (fgets(buf,sizeof(buf), fp) != NULL)	{
		if(count > 1)	{
			sscanf(buf, FORMAT_NETSTAT_CMD, tmp[0], tmp[1],tmp[2]);
			if(NULL == tmp[0][0] || NULL == tmp[1][0] || NULL == tmp[2][0]) 
			{
				break;
			}
			else 
			{
				memset(localIPAddress, 0, sizeof(localIPAddress));
				memset(localIPort, 0, sizeof(localIPort));
				memset(remoteIPAddress, 0, sizeof(remoteIPAddress));
				memset(remotePort, 0, sizeof(remotePort));
				memset(status, 0, sizeof(status));
				memset(nodeName, 0, sizeof(nodeName));
				
				snprintf(nodeName,sizeof(nodeName),TR181_IPACTIVEPORT_ENTRY_NODE, nodeNum + 1);

				if(strcmp(tmp[2], "LISTEN") == 0) {
					strncpy(status, "LISTEN",sizeof(status));
				}
				else if(strcmp(tmp[2], "ESTABLISTEN") == 0){
					strncpy(status, "ESTABLISTEN",sizeof(status));
				}
				else{
					continue;
				}				
				if(NULL == strstr(tmp[0], ":::")) {
					sscanf(tmp[0], "%[^:]:%s", localIPAddress, localIPort);
				}
				else {
					sscanf(tmp[0], ":::%s", localIPAddress);
				}
				if(NULL == strstr(tmp[1], ":::")) {
					sscanf(tmp[1], "%[^:]:%s", remoteIPAddress, remotePort);
				}
				else {
					sscanf(tmp[1], ":::%s", remoteIPAddress);
				}

				if(cfg_query_object(nodeName,NULL,NULL) <= 0)
				{		
					ipapEntryIdx = cfg_obj_create_object(nodeName);
					if(ipapEntryIdx < 0) 
					{
						tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
						fclose(fp);
						return -1;
					}
				}
				cfg_obj_set_object_attr(nodeName, "LocalIPAddress", 0, localIPAddress);
				cfg_obj_set_object_attr(nodeName, "LocalPort", 0, localIPort);
				cfg_obj_set_object_attr(nodeName, "RemoteIPAddress", 0, remoteIPAddress);
				cfg_obj_set_object_attr(nodeName, "RemotePort", 0, remotePort);
				cfg_obj_set_object_attr(nodeName, "Status", 0, status);
				nodeNum++;
			}
		}
		count++;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeNumStr,sizeof(nodeNumStr), "%d", nodeNum);
	cfg_obj_get_object_attr(TR181_IPACTIVEPORT_COMMON_NODE, "Num", 0, oldNum,sizeof(oldNum));
	
	if(atoi(oldNum) != nodeNum){
		cfg_obj_set_object_attr(TR181_IPACTIVEPORT_COMMON_NODE, "NumModify", 0, "1");
	}

	cfg_obj_set_object_attr(TR181_IPACTIVEPORT_COMMON_NODE, "Num", 0, nodeNumStr);
	unlink("/tmp/IPActionPortInfo");
	fclose(fp);
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

int cfg_type_tr181ipap_func_get(char* path,char* attr,char* val,int len) {
	
	cfg_type_tr181ipap_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len);

}

static cfg_node_ops_t cfg_type_tr181ipap_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipap_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_FLAG_MEMORY |TR181_IPACTIVEPORT_NUM, 
	 .parent = &cfg_type_tr181ipap, 
	 .ops = &cfg_type_tr181ipap_entry_ops, 
};

static cfg_node_ops_t cfg_type_tr181ipap_common_ops  = { 
	 .get = cfg_type_tr181ipap_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ipap_common = { 
	 .name = "Common", 
	 .flag =  CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_tr181ipap, 
	 .ops = &cfg_type_tr181ipap_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ipap_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ipap_child[] = { 
	 &cfg_type_tr181ipap_common, 
	 &cfg_type_tr181ipap_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ipap = { 
	 .name = "TR181IPAP", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ipap_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ipap_child, 
	 .ops = &cfg_type_tr181ipap_ops, 
}; 
