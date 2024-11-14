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
#include <cfg_xml.h> 
#include "cfg_types.h" 
#include <unistd.h>
#include <stdlib.h>
#include "../utility.h"

#define DMSCFG_PATH "/etc/dms_basic.cfg"
#define DMSDEVICECFG_PATH "/etc/dms_device.cfg"
#define DMS_START_PATH "/usr/script/dms_start.sh"
#if defined(TCSUPPORT_CT_JOYME)
#define DMSTCAPP_PLUGIN_PATH "/usr/osgi/plugin-b/com.chinatelecom.econet.smartgateway.dlna/userfs/dlna/bin/dlna_dmsTcApp"
#define DLNA_PATH "/usr/osgi/plugin-b/com.chinatelecom.econet.smartgateway.dlna/"
#define DMS_START_PLUGIN_PATH "/usr/script/dms_start_plugin.sh"
#endif

int dms_write(void){
	char if_addr[19] = {0};
	cfg_node_type_t *node = NULL;
#ifdef TCSUPPORT_AUTO_IP
	char autoip_en[6] = {0};
#endif
	
	/*Get address of LAN interface*/	
#ifdef TCSUPPORT_AUTO_IP	
	if((cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "AUTOIPEnable", 0, autoip_en, sizeof(autoip_en)) > 0) 
		&& (!strcmp(autoip_en, "Yes")))
	{	
	
		if(cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IPAuto", 0, if_addr, sizeof(if_addr)) < 0)
		{
			printf("dms_write:get br0 auto address failed\n");
			return FAIL;
		}
	}
	else
	{
#endif	
		if(cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IP", 0, if_addr, sizeof(if_addr)) < 0)
		{
			printf("dms_write:get br0 address failed\n");
			return FAIL;
		}
#ifdef TCSUPPORT_AUTO_IP	
	}
#endif
		
	cfg_set_object_attr(DMS_BASIC_NODE, "IPAddress", if_addr);
	
	printf("dms:get new Lan ipaddr %s\n", if_addr);

	/*Create dms.cfg*/
	if(cfg_save_attrs_to_file(DMS_BASIC_NODE, DMSCFG_PATH, QMARKS) != 0){
		tcdbg_printf("\n[%s:%d]write %s fail!",__FUNCTION__,__LINE__,DMSCFG_PATH);
		return -1;
	}
	
	/*Create device.cfg*/
	if(cfg_save_attrs_to_file(DMS_DEVICE_NODE, DMSDEVICECFG_PATH, QMARKS) != 0){
		tcdbg_printf("\n[%s:%d]write %s fail!",__FUNCTION__,__LINE__,DMSDEVICECFG_PATH);
		return -1;
	}
	return SUCCESS;
}

int svc_cfg_boot_dms(void)
{
	char dms_en[6] = {0};
	char cmd[64] = {0};

	cfg_set_object_attr(DMS_ENTRY_NODE, "restart", "1");

	return 0;
}

int cfg_type_dms_func_commit(char* path){
	char dms_en[6] = {0};
	char cmd[64] = {0};
#if defined(TCSUPPORT_CT_JOYME)
	char dms_type[16] = {0};
	FILE *fp = NULL;
#endif

	static int commit_flag = 0;

	if (commit_flag == 0)
		commit_flag = 1;
	else {
		cfg_set_object_attr(DMS_ENTRY_NODE, "restart", "1");
		return 0;
	}
	dms_write();
	
	system("killall -SIGTERM dlna_dmsTcApp");
	
	sleep(2);

	memset(dms_en, 0, sizeof(dms_en));
	cfg_obj_get_object_attr(DMS_ENTRY_NODE, "Enable", 0, dms_en, sizeof(dms_en));
	if(dms_en[0] != '\0')
	{
		if(!strcmp(dms_en, "Yes"))
		{
#if defined(TCSUPPORT_CT_JOYME)
			cfg_obj_get_object_attr(DMS_ENTRY_NODE, "type", 0, dms_type, sizeof(dms_type));
			if (!strcmp(dms_type, "plugin")){
				fp = fopen(DMSTCAPP_PLUGIN_PATH, "r");
				if(fp == NULL){
					snprintf(cmd, sizeof(cmd), "cd %s", DLNA_PATH);
					system(cmd);
					
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "tar -xvf %sdlna.tar -C %s", DLNA_PATH, DLNA_PATH);
					system(cmd);
				}
				else {
					fclose(fp);
				}

				memset(cmd, 0, sizeof(cmd));
				printf("dms:plugin DMS service start...\n");
				snprintf(cmd, sizeof(cmd), "%s %s %s", DMS_START_PLUGIN_PATH, DMSDEVICECFG_PATH, DMSCFG_PATH);
				system(cmd);
			}
			else{
			/*We need check does USB disk mount*/
			printf("dms:DMS service start...\n");
			snprintf(cmd, sizeof(cmd), "%s %s %s", DMS_START_PATH, DMSDEVICECFG_PATH, DMSCFG_PATH);
			system(cmd);
		}
#else
			/*We need check does USB disk mount*/
			printf("dms:DMS service start...\n");
			snprintf(cmd, sizeof(cmd), "%s %s %s", DMS_START_PATH, DMSDEVICECFG_PATH, DMSCFG_PATH);
			system(cmd);
			sleep(2);
#endif
		}
		else{
			printf("dms:DMS service stop\n");
		}
	}

	commit_flag = 0;
	
	return SUCCESS;
}

static cfg_node_ops_t cfg_type_dms_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dms_func_commit 
}; 


static cfg_node_type_t cfg_type_dms_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_dms, 
	 .ops = &cfg_type_dms_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dms_basic_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dms_func_commit 
}; 


static cfg_node_type_t cfg_type_dms_basic = { 
	 .name = "Basic", 
	 .flag = 1, 
	 .parent = &cfg_type_dms, 
	 .ops = &cfg_type_dms_basic_ops, 
}; 


static cfg_node_ops_t cfg_type_dms_device_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dms_func_commit 
}; 


static cfg_node_type_t cfg_type_dms_device = { 
	 .name = "Device", 
	 .flag = 1, 
	 .parent = &cfg_type_dms, 
	 .ops = &cfg_type_dms_device_ops, 
}; 


static cfg_node_ops_t cfg_type_dms_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dms_func_commit 
}; 


static cfg_node_type_t* cfg_type_dms_child[] = { 
	 &cfg_type_dms_entry, 
	 &cfg_type_dms_basic, 
	 &cfg_type_dms_device, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dms = { 
	 .name = "DMS", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dms_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dms_child, 
	 .ops = &cfg_type_dms_ops, 
}; 
