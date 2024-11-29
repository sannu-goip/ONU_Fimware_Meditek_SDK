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
#include <unistd.h>
#include <stdlib.h>

#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "cfg_xml.h"
#include "utility.h"


#define MAXLEN_TCAPI_MSG	1024
#define NO_QMARKS 0
#define QMARKS 1
#define NO_ATTRIBUTE 2
#define AUTOEXEC_PATH "/etc/autoexec.sh"

static int cfg_type_autoexec_write(void)
{
	int retval = 0;
	char nodePath[64] = {0};
	
	snprintf(nodePath,sizeof(nodePath), AUTOEXEC_ENTRY_NODE);
	retval = cfg_save_attrs_to_file(nodePath, AUTOEXEC_PATH, NO_ATTRIBUTE);
	
	return retval;
}

int svc_cfg_boot_autoexec(void)
{
	FILE *startupSh=NULL;

	cfg_type_autoexec_write( );
	startupSh=fopen("/etc/autoexec.sh","r");
	if(startupSh)
	{
		fclose(startupSh);
		system("chmod 755 /etc/autoexec.sh");
		system("/etc/autoexec.sh");
	}

	return SUCCESS;
}

static int cfg_type_autoexec_func_commit(char* path)
{	
	return cfg_type_autoexec_write( );
}


static cfg_node_ops_t cfg_type_autoexec_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_autoexec_func_commit 
}; 


static cfg_node_type_t cfg_type_autoexec_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_autoexec, 
	 .ops = &cfg_type_autoexec_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_autoexec_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_autoexec_func_commit 
}; 


static cfg_node_type_t* cfg_type_autoexec_child[] = { 
	 &cfg_type_autoexec_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_autoexec = { 
	 .name = "Autoexec", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_autoexec_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_autoexec_child, 
	 .ops = &cfg_type_autoexec_ops, 
}; 
