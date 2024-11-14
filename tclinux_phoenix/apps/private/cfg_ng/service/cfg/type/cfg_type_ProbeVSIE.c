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
#ifdef TCSUPPORT_WLAN_VENDIE
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

//#include "../event_api.h"
#include <utility.h>
#include <svchost_evt.h>

int svc_cfg_boot_proberxvsie(void)
{
	char nodeName1[48] = {0};
	char nodeName2[48] ={0};
	char buf1[64] = {0};
	char buf2[64] = {0};
	char buf3[64] = {0};
	int i,j,k;
	//tcdbg_printf("svc_cfg_boot_proberxvsie testboot\n");
	snprintf(nodeName1, sizeof(nodeName1), PROBERXVSIE_COMMON_NODE);
	cfg_set_object_attr(nodeName1, "Enable", "0");
	snprintf(nodeName2, sizeof(nodeName2), "root.proberxvsie.entry.1");
	for(i=0;i<3;i++)
	{
		snprintf(nodeName2,sizeof(nodeName2),"root.wifiprobeinfo.entry.%d",i+1);
		for(j=0;j<8;j++)
		{
			snprintf(buf1,sizeof(buf1),"BAND%d",j);
			snprintf(buf2,sizeof(buf2),"MAC%d",j);
			snprintf(buf3,sizeof(buf3),"VSIE%d",j);
			cfg_set_object_attr(nodeName2,buf1,"");
			cfg_set_object_attr(nodeName2,buf2,"");
			cfg_set_object_attr(nodeName2,buf3,"");
		}
	}
	return 0;
}

static int cfg_type_probevsie_func_commit(char* path) 
{
	printf("enter probevsie_commit.\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_PROBEVSIE_UPDATE, (void*)&param, sizeof(param));	
	return 0;
}

int cfg_type_probevsie_get(char* path,char* attr,char* val,int len)
{
	return 0;
}


int cfg_type_probevsie_func_get(char* path,char* attr,char* val,int len)
{
	cfg_type_probevsie_get(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len); 
}

static char* cfg_type_probevsie_index[] = { 
	 "probevsie_id", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_probevsie_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_probevsie_common = { 
	 .name = "Common", 
	 .flag = 1,
	 .parent = &cfg_type_probevsie, 
	 .ops = &cfg_type_probevsie_common_ops, 
}; 


static cfg_node_ops_t cfg_type_probevsie_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_probevsie_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 11, 
	 .parent = &cfg_type_probevsie,
	 .index = cfg_type_probevsie_index, 
	 .ops = &cfg_type_probevsie_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_probevsie_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .query = cfg_type_default_func_query, 	 
	 .commit = cfg_type_probevsie_func_commit 
}; 

static cfg_node_type_t* cfg_type_probevsie_child[] = { 
	 &cfg_type_probevsie_common,
	 &cfg_type_probevsie_entry,
	 NULL 
}; 

cfg_node_type_t cfg_type_probevsie = { 
	 .name = "ProbeRxVSIE", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_probevsie_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_probevsie_child, 
	 .ops = &cfg_type_probevsie_ops, 
}; 
#endif
