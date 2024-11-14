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

#define SDN_INFO_PATH		"/tmp/sdn_info"

int svc_cfg_boot_sdn(void)
{
	wan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, SDN_COMMON_NODE, sizeof(param)-1);
    cfg_obj_send_event(EVT_WAN_INTERNAL, EVT_CFG_WAN_SDN_UPDATE, (void*)&param, sizeof(param)); 
	return 0;
}
int cfg_type_sdn_commit(char* path)
{
	wan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_INTERNAL, EVT_CFG_WAN_SDN_UPDATE, (void*)&param, sizeof(param)); 
    return 0;
}
static int cfg_type_sdn_common_get(char* path,char* attr,char* val,int len)
{
	char cmd[256] = {0},tmp[128] = {0},dpid[128]={0};
	char brname[16] ={0},enable[8] = {0};
	FILE *fp = NULL;
	char *str = NULL;
	
	if(attr && !strcmp(attr,"dpid"))
	{
		memset(brname, 0, sizeof(brname));
		memset(enable, 0, sizeof(enable));
		cfg_get_object_attr(SDN_COMMON_NODE, "brname", brname, sizeof(brname));
		cfg_get_object_attr(SDN_COMMON_NODE, "Enable", enable, sizeof(enable));
		if(brname[0] != '\0' && enable[0] == '1')
		{
			snprintf(cmd, sizeof(cmd),"/usr/ovs/bin/ovs-ofctl show %s > %s", brname,SDN_INFO_PATH);
			system(cmd);
			if((fp=fopen(SDN_INFO_PATH,"r")) != NULL && fgets(tmp, 128, fp) != NULL)
			{
				if((str=strstr(tmp,"dpid:")) != NULL)
				{
					strncpy(dpid,str+strlen("dpid:"),sizeof(dpid)-1);
					if(dpid[strlen(dpid)-1] == '\n')
					{
						dpid[strlen(dpid) - 1] = '\0';
					}
					cfg_set_object_attr(SDN_COMMON_NODE, "dpid", dpid);
				}
			}
			if(fp)
			{
				fclose(fp);
			}
			/*if(get_profile_str("dpid:",dpid,sizeof(dpid),NO_QMARKS, SDN_INFO_PATH) == -1)
		    {
		        printf("[%s %d] no dpif file!!\n",__FUNCTION__,__LINE__);
		    }*/
		}
		
	}
	
	return cfg_type_default_func_get(path,attr,val,len);
}

static cfg_node_ops_t cfg_type_sdn_common_ops  = { 
	 .get = cfg_type_sdn_common_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sdn_commit 
}; 


static cfg_node_type_t cfg_type_sdn_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_sdn, 
	 .ops = &cfg_type_sdn_common_ops, 
}; 


static cfg_node_ops_t cfg_type_sdn_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sdn_commit 
}; 


static cfg_node_type_t* cfg_type_sdn_child[] = { 
	 &cfg_type_sdn_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_sdn = { 
	 .name = "SDN", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_sdn_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_sdn_child, 
	 .ops = &cfg_type_sdn_ops, 
}; 
