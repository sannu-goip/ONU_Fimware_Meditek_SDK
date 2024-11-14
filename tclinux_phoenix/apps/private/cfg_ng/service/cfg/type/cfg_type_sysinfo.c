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
#include <time.h>
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 

static int cfg_type_sysinfo_func_write(char* path)
{
	time_t sysStarttime = time((time_t *)NULL);
	char tmpSysTime[20] = {0};
	if(cfg_query_object(SYSINFO_ENTRY_NODE, NULL, NULL) <= 0){
		snprintf(tmpSysTime,sizeof(tmpSysTime),"%d",(int)sysStarttime);
		cfg_set_object_attr(SYSINFO_ENTRY_NODE,"sysStartTime",tmpSysTime);
		return 0;
	}
	else{
		return -1;
	}
}

static int cfg_type_sysinfo_func_execute(char* path)
{
	char attrName[][64] = {{"etherPktsClearSet"},{"adslPktsClearSet"},{"usbPktsClearSet"},{""}};
	char procLocation[][64] = {{"/proc/tc3162/eth_pktsclear"},{"/proc/tc3162/tsarm_pktsclear"},{"/proc/tc3162/usb_link_state_clear"},{""}};
	char cmd[64]={0};
	int i;
	char nodepath[64]={0};
	char tmp[64]={0};
	strncpy(nodepath,SYSINFO_ENTRY_NODE,sizeof(nodepath)-1);
	for(i = 0;strlen(attrName[i])!=0;i++){
		if(cfg_obj_get_object_attr(nodepath,attrName[i],0,tmp,sizeof(tmp))>0){
			if(!strcmp(tmp,"Yes")){
				doValPut(procLocation[i], "1\n");
			}
			strncpy(tmp,"No",sizeof(tmp)-1);
			cfg_set_object_attr(nodepath,attrName[i],tmp);
		}
	}

	return 0;
}


static int cfg_type_sysinfo_func_commit(char* path)
{
	cfg_type_sysinfo_func_write(path);
	cfg_type_sysinfo_func_execute(path);
	
	return 0;
} 

int cfg_type_sysinfo_entry_func_get(char* path,char* attr,char* val,int len)
{
	char buf[32] = {0};
	char* p = NULL;
	char swVer[64] = {0};
	
	if(0 == strcmp(attr, "SWVer"))
	{
		if(cfg_get_object_attr(DEVICEINFO_NODE, "FwVer", buf, sizeof(buf))	> 0 && '\0' != buf[0])
		{
			for(p = buf; *p != '\0'; p++)	
			{
				if(*p == '_')
				{
					*p = ' ';
				}
			}
			snprintf(swVer, sizeof(swVer), "TCLinux Fw %s", buf);
			cfg_set_object_attr(path, attr, swVer);
		}
	}

	return cfg_type_default_func_get(path,attr,val,len);
}

static cfg_node_ops_t cfg_type_sysinfo_entry_ops  = { 
	 .get = cfg_type_sysinfo_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sysinfo_func_commit 
}; 


static cfg_node_type_t cfg_type_sysinfo_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_sysinfo, 
	 .ops = &cfg_type_sysinfo_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_sysinfo_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sysinfo_func_commit 
}; 


static cfg_node_type_t* cfg_type_sysinfo_child[] = { 
	 &cfg_type_sysinfo_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_sysinfo = { 
	 .name = "SysInfo", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_sysinfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_sysinfo_child, 
	 .ops = &cfg_type_sysinfo_ops, 
}; 
