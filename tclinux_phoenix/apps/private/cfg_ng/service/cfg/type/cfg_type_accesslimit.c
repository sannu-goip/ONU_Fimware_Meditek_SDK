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


#define ACCESSLIMIT_MODE			"mode"
#define ACCESSLIMIT_DEBUG		 	"debug"
#define ACCESSLIMIT_TOTALNUM	 	"totalnum"
#define ACCESSLIMIT_LIVETIME 		"livetime"
#define ACCESSLIMIT_ARP_TIMEOUT 	"arptimeout"
#define ACCESSLIMIT_ARP_COUNT 		"arpcount"
#define ACCESSLIMIT_TERMINALTYPE 	"terminaltype"

int svc_cfg_boot_accesslimit(void)
{
	int i = 0;
	char tmp[32] = {0};
	char tmp1[32] = {0};
	char tmp2[32] = {0};
	char cmdbuf[128] = {0};
	char nodePath[64] = {0};

#if defined(TCSUPPORT_CMCC)
	system_escape("/usr/bin/accesslimitcmd mode 0");
#endif

	/*set totalnum*/
	snprintf(nodePath, sizeof(nodePath), ACCESSLIMIT_COMMON_NODE);
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_TOTALNUM, 0, tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd totalnum %s", tmp);
		system_escape(cmdbuf);
	}
	
	/*set debug */
	memset(cmdbuf,0,sizeof(cmdbuf));
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_DEBUG, 0, tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd debug %s", tmp);
		system_escape(cmdbuf);
	}

	/*set livetime */
	memset(cmdbuf,0,sizeof(cmdbuf));
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_LIVETIME, 0, tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd livetime %s", tmp);
		system_escape(cmdbuf);
	}

	/*set arp leavecount and timeout */
	memset(cmdbuf,0,sizeof(cmdbuf));
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_ARP_TIMEOUT, 0, tmp, sizeof(tmp)) > 0)
	{
		if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_ARP_COUNT, 0, tmp1, sizeof(tmp1)) > 0)
		{
			snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd arpinfo %s %s",tmp,tmp1);
			system_escape(cmdbuf);
		}
	}

#if defined(TCSUPPORT_CMCC)
	/*set type,0:ip type,1:mac type*/
	memset(cmdbuf,0,sizeof(cmdbuf));
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_TERMINALTYPE, 0, tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd terminaltype %s", tmp);
		system_escape(cmdbuf);
	}
#endif
	
	/*set mode (because mode is switch ,so execute at last!) */
	memset(cmdbuf,0,sizeof(cmdbuf));
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(nodePath, ACCESSLIMIT_MODE, 0, tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd mode %s",tmp);
		system_escape(cmdbuf);
	}
	
	/*set device type info(accesslimitcmd deviceinfo <switch> <devicename> <limitnum> on/off computer/stb/phone/camera) */
	for(i=1; i<=5;i++)
	{
		memset(nodePath, 0, sizeof(nodePath));
		memset(cmdbuf,0,sizeof(cmdbuf));
		memset(tmp,0,sizeof(tmp));
		memset(tmp1,0,sizeof(tmp1));
		memset(tmp2,0,sizeof(tmp2));
		snprintf(nodePath, sizeof(nodePath), ACCESSLIMIT_ENTRY_NODE, i);
		if((cfg_obj_get_object_attr(nodePath, "Active", 0, tmp, sizeof(tmp)) > 0)
			&&(cfg_obj_get_object_attr(nodePath, "devtype", 0, tmp1, sizeof(tmp1)) > 0)
			 &&(cfg_obj_get_object_attr(nodePath, "limitnum", 0, tmp2, sizeof(tmp2)) > 0))
		{
			if(!strcasecmp(tmp,"Yes"))
			{
				snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/accesslimitcmd devicetypeinfo on %s %s",tmp1, tmp2);
				system_escape(cmdbuf);
			}
			else
			{
				snprintf(cmdbuf,sizeof(cmdbuf), "/usr/bin/accesslimitcmd devicetypeinfo off %s %s",tmp1, tmp2);
				system_escape(cmdbuf);
			}		
		}
	}
	system("/userfs/bin/hw_nat -!");
	
	return 0;
}

int cfg_type_accesslimit_execute(void)
{
	return  svc_cfg_boot_accesslimit( );
}

int cfg_type_accesslimit_func_commit(char* path)
{
	return cfg_type_accesslimit_execute( );
}

static char* cfg_type_accesslimit_index[] = { 
	 "accesslimit_id", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_accesslimit_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesslimit_func_commit 
}; 


static cfg_node_type_t cfg_type_accesslimit_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_accesslimit, 
	 .index = cfg_type_accesslimit_index, 
	 .ops = &cfg_type_accesslimit_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_accesslimit_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesslimit_func_commit 
}; 


static cfg_node_type_t cfg_type_accesslimit_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_accesslimit, 
	 .ops = &cfg_type_accesslimit_common_ops, 
}; 


static cfg_node_ops_t cfg_type_accesslimit_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_accesslimit_func_commit 
}; 


static cfg_node_type_t* cfg_type_accesslimit_child[] = { 
	 &cfg_type_accesslimit_entry, 
	 &cfg_type_accesslimit_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_accesslimit = { 
	 .name = "Accesslimit", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_accesslimit_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_accesslimit_child, 
	 .ops = &cfg_type_accesslimit_ops, 
}; 
