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
#include <unistd.h>
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "../utility.h"
#include "cfg_msg.h"
#include <svchost_evt.h>

extern cfg_node_type_t cfg_type_virserver_entry;

static char* cfg_type_virserver_index[] = { 
	 "nat_pvc",  
	 NULL 
}; 

static char* cfg_type_virserver_entry_index[] = { 
	 "virServ_id", 
	 NULL 
}; 

#define VIRSERV_SH		"/usr/script/vserver.sh"
#define VIRSERV_PATH 	"/var/run/%s/vserver%s" 

#ifdef TCSUPPORT_CWMP_TR181
void updatePortMapByVirServer(char* path, int flag)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[2] = {0};
	int entry1 = -1, entry2 = -1;

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, GLOBALSTATE_COMMON_NODE, sizeof(nodeName) - 1);
	if(cfg_obj_get_object_attr(nodeName, "CommitFromTR181", 0, tmp, sizeof(tmp)) < 0
		|| strcmp(tmp, "1") != 0)
	{
#if  defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
		sscanf(path, VIRSERVER_ENTRY_NODE, &entry1);
		if(entry1 > 0)
		{
			if(flag == 0)
			{
				deleteNatPortMap(entry1);
			}
			else
			{
				updateNatPortMap(entry1);
			}
		}	
#else
		sscanf(path, VIRSERVER_ENTRY_ENTRY_NODE, &entry1, &entry2);
		if(entry1 > 0 && entry2 > 0)
		{
			if(flag == 0)
			{
				ClearPortMapNodeByIdx(entry1, entry2);
			}
			else
			{
				updatePortMapNodeByIdx(entry1, entry2);
			}
		}
#endif 
	}

	return;
}
#endif

#if  defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
static int cfg_type_virServer_entry_func_commit(char* path){
#else 
static int cfg_type_virServer_entry_entry_func_commit(char* path){
#endif 

#ifdef TCSUPPORT_CWMP_TR181
	updatePortMapByVirServer(path, 1);
#endif

	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VIRSERVER_ENTRY_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
static int cfg_type_virServer_func_commit(char* path){
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VIRSERVER_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}
#endif 

int cfg_type_virServer_entry_entry_func_delete_core(char* path)
{
	char nodeName[128]		= {0};
	char cur_id_virSer[4]	= {0};
	char buf[64]			= {0};
	int pvcIndex			= 0;
	int entryIndex			= 0;
	char virServ_file[32]	= {0};
	char if_name[32]		= {0};
	char* ptmp_path 		= NULL;
	int virServer_idx 		= -1;

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
	if(get_entry_number_cfg2(path, "entry.", &entryIndex) == 0){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, entryIndex);
		if(cfg_obj_get_object_attr(nodeName, "ActiveIFName", 0, if_name, sizeof(if_name)) <= 0){
			return -1;
		}

		if(cfg_obj_query_object(path, NULL, NULL) > 0){
			virServer_idx = entryIndex -1;
			snprintf(cur_id_virSer, sizeof(cur_id_virSer), "%d", virServer_idx);
			snprintf(buf, sizeof(buf), "%s del %s %s", VIRSERV_SH, if_name, cur_id_virSer);
			system_escape(buf);
			/*Delete the configuration file*/
			snprintf(virServ_file, sizeof(virServ_file), VIRSERV_PATH, if_name, cur_id_virSer);
			unlink(virServ_file);
		}
	}
	else{
		return -1;
	}
#else 
	if(get_entry_number_cfg2(path, "entry.", &pvcIndex) == 0){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, pvcIndex);
		if(cfg_obj_get_object_attr(nodeName, "IFName", 0, if_name, sizeof(if_name)) <= 0){
			return -1;
		}

		ptmp_path = strstr(path, "entry.") + strlen("entry.");
		if(get_entry_number_cfg2(ptmp_path, "entry.", &entryIndex) == 0){
			if(cfg_obj_query_object(path, NULL, NULL) > 0){
				virServer_idx = entryIndex -1;
				snprintf(cur_id_virSer, sizeof(cur_id_virSer), "%d", virServer_idx);
				snprintf(buf, sizeof(buf), "%s del %s %s", VIRSERV_SH, if_name, cur_id_virSer);
				system_escape(buf);
				/*Delete the configuration file*/
				snprintf(virServ_file, sizeof(virServ_file), VIRSERV_PATH, if_name, cur_id_virSer);
				unlink(virServ_file);
			}	
		}else{
			return -1;
		}
	}
	else{
		return -1;
	}
#endif 
	return 0;
}


static int cfg_type_virServer_entry_entry_func_delete(char* path)
{
#if defined(TCSUPPORT_CUC)
	char type[8] = {0};

	if(cfg_obj_get_object_attr(VIRSERVER_NODE, "web_flag", 0, type, sizeof(type)) > 0 && 1 == atoi(type))
	{
		cfg_type_virServer_entry_entry_func_delete_core(path);
		cfg_obj_set_object_attr(VIRSERVER_NODE, "web_flag", 0, "0");
		cfg_type_default_func_delete(path);
		return 0;
	}
#endif
#ifdef TCSUPPORT_CWMP_TR181
	updatePortMapByVirServer(path, 0);
#endif

#if defined(TCSUPPORT_CMCCV2)
	cfg_type_virServer_entry_entry_func_delete_core(path);
	cfg_type_default_func_delete(path);
#else
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VIRSERVER_ENTRY_DELETE, (void *)&param, sizeof(param)); 
#endif
	
	return 0;
}

static int cfg_type_virServer_entry_func_delete(char* path)
{
	char nodeName[128]	= {0};
	int i 				= 0;
	
#if defined(TCSUPPORT_CT_JOYME2)	
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VIRSERVER_ENTRY_DELETE, (void *)&param, sizeof(param)); 
	
#ifdef TCSUPPORT_CWMP_TR181
	updatePortMapByVirServer(path, 0);
#endif
#else
	for(i = 0; i < MAX_VIRSERV_RULE; i++) {
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", path, i+1);
		cfg_type_virServer_entry_entry_func_delete_core(nodeName);
	}
	cfg_type_default_func_delete(path);
#endif

	return 0;
}

int svc_cfg_boot_virServ(void)
{
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_VIRSERVER_BOOT, NULL, 0); 
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
/*virserver_entry*/
static cfg_node_ops_t cfg_type_virserver_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_virServer_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_virServer_entry_func_commit 
}; 

cfg_node_type_t cfg_type_virserver_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_VIRSERV_RULE, 
	 .parent = &cfg_type_virserver, 
	 .index = cfg_type_virserver_entry_index, 
	 .ops = &cfg_type_virserver_entry_ops, 
}; 

/*virserver*/
static cfg_node_ops_t cfg_type_virserver_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_virServer_func_commit 
}; 
#else
/*virserver_entry_entry*/
static cfg_node_ops_t cfg_type_virserver_entry_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_virServer_entry_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_virServer_entry_entry_func_commit 
}; 

static cfg_node_type_t cfg_type_virserver_entry_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_VIRSERV_RULE, 
	 .parent = &cfg_type_virserver_entry, 
	 .index = cfg_type_virserver_entry_index,
	 .ops = &cfg_type_virserver_entry_entry_ops, 
};

/*virserver_entry*/
static cfg_node_ops_t cfg_type_virserver_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_virServer_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_virServer_entry_entry_func_commit 
}; 

static cfg_node_type_t* cfg_type_virserver_entry_child[] = { 
	 &cfg_type_virserver_entry_entry, 
	 NULL 
};

cfg_node_type_t cfg_type_virserver_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_WAN_IF_INDEX, 
	 .parent = &cfg_type_virserver, 
	 .index = cfg_type_virserver_index, 
	 .nsubtype = sizeof(cfg_type_virserver_entry_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_virserver_entry_child, 
	 .ops = &cfg_type_virserver_entry_ops, 
}; 

/*virserver*/
static cfg_node_ops_t cfg_type_virserver_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_virServer_entry_entry_func_commit 
}; 

#endif
static cfg_node_type_t* cfg_type_virserver_child[] = { 
	 &cfg_type_virserver_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_virserver = { 
	 .name = "VirServer", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_virserver_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_virserver_child, 
	 .ops = &cfg_type_virserver_ops, 
}; 
