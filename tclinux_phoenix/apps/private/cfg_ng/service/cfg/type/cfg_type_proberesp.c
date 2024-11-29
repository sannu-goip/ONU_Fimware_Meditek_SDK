#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <sys/socket.h>    /* for connect and socket*/
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/if.h>
#include <fcntl.h>
#include <linux/wireless.h>
#include <linux/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <cfg_cli.h> 
#include <cfg_api.h>
#include "cfg_types.h"
#include <svchost_evt.h>


#include <utility.h>

int svc_cfg_boot_proberespvsie(void)
{
	char nodeName1[48] = {0};
	memset(nodeName1,0,sizeof(nodeName1));
	snprintf(nodeName1, sizeof(nodeName1), PROBERESPVSIE_COMMON_NODE);
	cfg_set_object_attr(nodeName1, "MaxEntryNum", "3");
	return 0;
}

static int cfg_type_proberesp_func_commit(char* path) 
{
	printf("enter proberesp_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_PROBERESP_UPDATE, (void*)&param, sizeof(param));	
	return 0;
}

static int cfg_type_proberesp_func_entry_delete(char* path) 
{
	printf("\n enter proberesp_entry delete action.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_PROBERESP_DELETE, (void*)&param, sizeof(param));	

	return 0;
}

int cfg_type_proberesp_func_get(char* path,char* attr,char* val,int len)
{
	return cfg_obj_get_object_attr(path,attr,0,val,len); 
}


static cfg_node_ops_t cfg_type_proberesp_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_proberesp_func_entry_delete,
};

static cfg_node_ops_t cfg_type_proberesp_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static char* cfg_type_proberesp_index[] = { 
	 "proberesp_id", 
	 NULL 
}; 

static cfg_node_type_t cfg_type_proberesp_common = { 
	 .name = "Common", 
	 .flag = 1,
	 .parent = &cfg_type_proberesp, 
	 .ops = &cfg_type_proberesp_common_ops, 
}; 

static cfg_node_type_t cfg_type_proberesp_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 11, 
	 .parent = &cfg_type_proberesp,
	 .index = cfg_type_proberesp_index, 
	 .ops = &cfg_type_proberesp_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_proberesp_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .query = cfg_type_default_func_query, 	 
	 .commit = cfg_type_proberesp_func_commit 
}; 

static cfg_node_type_t* cfg_type_proberesp_child[] = { 
	 &cfg_type_proberesp_common,
	 &cfg_type_proberesp_entry,
	 NULL 
}; 

cfg_node_type_t cfg_type_proberesp = { 
	 .name = "ProbeRespVSIE", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_proberesp_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_proberesp_child, 
	 .ops = &cfg_type_proberesp_ops, 
}; 

