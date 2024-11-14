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
#include <unistd.h>
#include <sys/wait.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"


#define USBRESTORE_OPT 		"opt"
#define USBRESTORE_TARGET 	"target"
#define USBRESTORE_DEV 		"dev"
#define USBRESTORE_RESULT 	"rstresult"
#define USBRESTORE_EXECCMDFORMAT 		"backuprestorecmd -b -%s -/tmp/mnt/%s/%s"
#define USBRESTORE_EXECCMDDEL 			"rm -f /tmp/mnt/%s/%s"
#define USBRESTORE_SOURCE					"/tmp/var/romfile.cfg~~"
#define USBRESTORE_MAX_COMMAND_LENGTH	500
#define USBRESTORE_DEV_LENGTH				50
#define USBRESTORE_TARGET_LENGTH			50
#define USBRESTORE_OPT_LENGTH 			3

enum OPTION{
	OPT_NONE,
	OPT_DOACT,
	OPT_DELSAMEFILE,
	OPT_SETRESULT,
};

enum RESULT{
	RESULT_NOACT = 0,
	RESULT_SUCCESS,
	RESULT_USBCONERR,
	RESULT_SPACENOTENOUGH,
	RESULT_SAMEFILE,
	RESULT_NOBKFILE,
	RESULT_NOROMFILE,
	RESULT_BKFILECHGED,
	RESULT_FAIL,
};

void replacename(char *str, char *oldname, char *newname)
{
	int i = 0;
	
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == oldname)
			str[i] = newname;
	}
	return;	
}

int cfg_type_usbrestore_execute(char* path)
{
	char execCMD[USBRESTORE_MAX_COMMAND_LENGTH];
	char target[USBRESTORE_TARGET_LENGTH];
	char dev[USBRESTORE_DEV_LENGTH];
	char opt[USBRESTORE_OPT_LENGTH];
	int ret = -1;
	char result[200] = {0};
	char ModelName[32] = {0};
	char usbRestorePath[64] = {0};
	char webCurSetPath[64] = {0};
	char *oldname = ' ';
	char *newname = '_';
	
	strncpy(usbRestorePath, USBRESTORE_ENTRY_NODE, sizeof(usbRestorePath)-1);
	strncpy(webCurSetPath, WEBCURSET_ENTRY_NODE, sizeof(webCurSetPath)-1);
	memset(target, 0x00, sizeof(target));
	memset(dev, 0x00, sizeof(dev));
	memset(opt, 0x00, sizeof(opt));

	if(cfg_obj_get_object_attr(webCurSetPath, USBRESTORE_OPT, 0, opt, sizeof(opt)) < 0)
	{
		return FAIL;
	}
	
	if(cfg_obj_get_object_attr(DEVICEINFO_DEVPARASTATIC_NODE, "ModelName", 0, ModelName, sizeof(ModelName)) > 0)
	{
		replacename(ModelName,oldname,newname);
		snprintf(target, sizeof(target), "e8_Config_backup/ctce8_%s.cfg",ModelName);
		if(cfg_set_object_attr(usbRestorePath, USBRESTORE_TARGET, target) > 0)
		{
			return FAIL;
		}
	}
	else
	{
		if(cfg_obj_get_object_attr(usbRestorePath, USBRESTORE_TARGET, 0, target, sizeof(target)) < 0)
		{
			return FAIL;
		}
	}
	
	if(cfg_obj_get_object_attr(usbRestorePath, USBRESTORE_DEV, 0, dev, sizeof(dev)) < 0)
	{
		return FAIL;
	}

	int op = atoi(opt);
	memset(execCMD, 0x00, sizeof(execCMD));

	switch(op)
	{
		case OPT_NONE:
			return SUCCESS;
		case OPT_DELSAMEFILE:
			snprintf(execCMD, sizeof(execCMD), USBRESTORE_EXECCMDDEL, dev, target);
			system_escape(execCMD);
			break;
		case OPT_SETRESULT:
			memset(result, 0x00, sizeof(result));
			snprintf(result, sizeof(result), "%d", RESULT_NOACT);
			cfg_set_object_attr(webCurSetPath, USBRESTORE_RESULT, result);

			memset(opt, 0x00, sizeof(opt));
			snprintf(opt,sizeof(opt), "%d", OPT_NONE);
			cfg_set_object_attr(webCurSetPath, USBRESTORE_OPT, opt);
			return SUCCESS;
		default:
			break;
	}

	memset(opt, 0x00, sizeof(opt));
	snprintf(opt, sizeof(opt), "%d", OPT_NONE);
	cfg_set_object_attr(webCurSetPath, USBRESTORE_OPT, opt);
	
	memset(result, 0x00, sizeof(result));
	snprintf(result, sizeof(result), "%d", RESULT_NOACT);
	cfg_set_object_attr(webCurSetPath, USBRESTORE_RESULT, result);
	
	memset(execCMD, 0x00, sizeof(execCMD));
	snprintf(execCMD, sizeof(execCMD), "/userfs/bin/mtd readflash %s 65536 0 romfile", USBRESTORE_SOURCE);
	system_escape(execCMD);

	memset(execCMD, 0x00, sizeof(execCMD));
	snprintf(execCMD, sizeof(execCMD), USBRESTORE_EXECCMDFORMAT, USBRESTORE_SOURCE, dev, target);
	ret = system_escape(execCMD);
	sleep(1);
	ret = WEXITSTATUS(ret);

	unlink(USBRESTORE_SOURCE);

	memset(result, 0x00, sizeof(result));
	snprintf(result, sizeof(result), "%d", ret);
	cfg_set_object_attr(webCurSetPath, USBRESTORE_RESULT, result);

	system("sync");/*sync data change to usb device */

	return SUCCESS;
}

int cfg_type_usbrestore_func_commit(char* path)
{
	return  cfg_type_usbrestore_execute(path);
}


static cfg_node_ops_t cfg_type_usbrestore_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_usbrestore_func_commit 
}; 


static cfg_node_type_t cfg_type_usbrestore_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_usbrestore, 
	 .ops = &cfg_type_usbrestore_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_usbrestore_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_usbrestore_func_commit 
}; 


static cfg_node_type_t* cfg_type_usbrestore_child[] = { 
	 &cfg_type_usbrestore_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_usbrestore = { 
	 .name = "usbRestore", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_usbrestore_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_usbrestore_child, 
	 .ops = &cfg_type_usbrestore_ops, 
}; 
