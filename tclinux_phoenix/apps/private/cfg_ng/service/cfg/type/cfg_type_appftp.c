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
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"

#define APPFTPHISTORY_NUM 				10		/*how many history items */
#define APPFTP_HISTORYMAXID 			4096	/*max history id */

#define APPFTP_USERNAME				"username"
#define APPFTP_PASSWORD 				"password"
#define APPFTP_URL 						"url"
#define APPFTP_PORT 					"port"
#define APPFTP_DEVICEID 				"deviceid"
#define APPFTP_SAVEPATH 				"savepath"
#define APPFTP_RESULT 					"result"
#define APPFTP_HISTORYID 				"id"
#define APPFTP_HISTORYCOUNT 			"historycount"

#define APPFTP_INVALID 					"N/A"

#define APPFTP_RES_DOWNLOADING 		"10"    	/*dowanloading id */
#define APPFTP_RES_BAD_URL	 			"3"		/*url error id */


#define APPFTP_MAX_ID_LENGTH				5
#define APPFTP_MAX_COMMAND_LENGTH		500
#define APPFTP_MAX_URL_LENGTH				260		
#define APPFTP_MAX_USERNAME_LENGTH		25
#define APPFTP_MAX_PASSWORD_LENGTH		25
#define APPFTP_MAX_PORT_LENGTH			10
#define APPFTP_MAX_DEVICEID_LENGTH		10
#define APPFTP_MAX_SAVEPATH_LENGTH		90		
#define APPFTP_MAX_RESULT_LENGTH			5
#define APPFTP_MAX_HISTORYCOUNT_LENGTH	5
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
#define APPFTPCLIENT_NUM 				10		
#endif

int svc_cfg_boot_appftp(void)
{
	int i;
	char nodePath[64] = {0};
	
	snprintf(nodePath, sizeof(nodePath), APPFTP_DOWNLOADING_NODE);
	cfg_set_object_attr(nodePath, APPFTP_USERNAME, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_PASSWORD, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_URL, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_PORT, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_DEVICEID, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_SAVEPATH, APPFTP_INVALID);
	cfg_set_object_attr(nodePath, APPFTP_HISTORYCOUNT, "0");

	memset(nodePath, 0, sizeof(nodePath));
	for(i = 1; i <= APPFTPHISTORY_NUM; i++)
	{
		snprintf(nodePath, sizeof(nodePath), APPFTP_ENTRY_NODE, i);
		cfg_set_object_attr(nodePath, APPFTP_HISTORYID, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_USERNAME, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_PASSWORD, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_URL, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_PORT, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_DEVICEID, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_SAVEPATH, APPFTP_INVALID);
		cfg_set_object_attr(nodePath, APPFTP_RESULT, APPFTP_INVALID);
	}

	return SUCCESS;
}/* end appftp_boot */

int cfg_type_appftp_execute(char* path)
{
	char ftpCommand[APPFTP_MAX_COMMAND_LENGTH];
	char url[APPFTP_MAX_URL_LENGTH];
	char *host = NULL;
	char *hostPath = NULL;
	char username[APPFTP_MAX_USERNAME_LENGTH];
	char password[APPFTP_MAX_PASSWORD_LENGTH];
	char port[APPFTP_MAX_PORT_LENGTH];
	char deviceId[APPFTP_MAX_DEVICEID_LENGTH];
	char savePath[APPFTP_MAX_SAVEPATH_LENGTH];
	char historyCount[APPFTP_MAX_HISTORYCOUNT_LENGTH];
	char entryId[APPFTP_MAX_ID_LENGTH];
	char entryUrl[APPFTP_MAX_URL_LENGTH];
	char entryUsername[APPFTP_MAX_USERNAME_LENGTH];
	char entryPassword[APPFTP_MAX_PASSWORD_LENGTH];
	char entryPort[APPFTP_MAX_PORT_LENGTH];
	char entryDeviceId[APPFTP_MAX_DEVICEID_LENGTH];
	char entrySavePath[APPFTP_MAX_SAVEPATH_LENGTH];
	char entryResult[APPFTP_MAX_RESULT_LENGTH];
	int entryIndex = 0;
	int entryIdValue = -1;
	
	char downloadNodePath[64] = {0};
	char entryNodePath[64] = {0};
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)     
	char enable[8] = {0};
	int tr69FTPClientID = 0;
#endif
	int i;

	memset(downloadNodePath, 0, sizeof(downloadNodePath));
	snprintf(downloadNodePath, sizeof(downloadNodePath), APPFTP_DOWNLOADING_NODE);
	
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	if(strstr(path, "entry"))
	{
		if(get_entry_number_cfg2(path,"entry",&tr69FTPClientID) == SUCCESS )
		{
			if(tr69FTPClientID >= APPFTPHISTORY_NUM && tr69FTPClientID < (APPFTPHISTORY_NUM + APPFTPCLIENT_NUM))
			{
				if(cfg_obj_get_object_attr(path, APPFTP_PORT, 0, entryPort, sizeof(entryPort)) < 0 )
				{
					cfg_set_object_attr(path,APPFTP_PORT, "21" );
				}
				memset(entryPort, 0, sizeof(entryPort));
				cfg_obj_get_object_attr(path, "Enable", 0, enable, sizeof(enable));
				if(strcmp(enable,"1"))
				{
					return SUCCESS; 	
				}			
				/*copy_node(downloadNodeName,name);      */
				cfg_obj_copy_private(downloadNodePath, path);   
			}
			else
			{
				return SUCCESS;
			}	
		}
		else
		{
			return SUCCESS;
		}
			
	}
#endif
	
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_USERNAME, 0, username, sizeof(username)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_PASSWORD, 0, password, sizeof(password)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_URL, 0, url, sizeof(url)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_PORT, 0, port, sizeof(port)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_DEVICEID, 0, deviceId, sizeof(deviceId)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_SAVEPATH, 0, savePath, sizeof(savePath)) < 0 )
	{
		return FAIL;
	}
	if(cfg_obj_get_object_attr(downloadNodePath, APPFTP_HISTORYCOUNT, 0, historyCount, sizeof(historyCount)) < 0 )
	{	
		return FAIL;
	}

	entryIndex = atoi(historyCount);

	/*if the number of histories is 10; delete the first */
	if(entryIndex >= APPFTPHISTORY_NUM)
	{
		entryIndex = APPFTPHISTORY_NUM - 1;

		for(i=1;i<=entryIndex;i++)
		{
			memset(entryNodePath, 0, sizeof(entryNodePath));
			snprintf(entryNodePath, sizeof(entryNodePath), APPFTP_ENTRY_NODE, i+1);
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_HISTORYID, 0, entryId, sizeof(entryId)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_USERNAME, 0, entryUsername, sizeof(entryUsername)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_PASSWORD, 0, entryPassword, sizeof(entryPassword)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_URL, 0, entryUrl, sizeof(entryUrl)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_PORT, 0, entryPort, sizeof(entryPort)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_DEVICEID, 0, entryDeviceId, sizeof(entryDeviceId)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_SAVEPATH, 0, entrySavePath, sizeof(entrySavePath)) < 0 )
			{
				return FAIL;
			}
			if(cfg_obj_get_object_attr(entryNodePath, APPFTP_RESULT, 0, entryResult, sizeof(entryResult)) < 0 )
			{
				return FAIL;
			}
			
			memset(entryNodePath, 0, sizeof(entryNodePath));
			snprintf(entryNodePath, sizeof(entryNodePath), APPFTP_ENTRY_NODE, i);
			cfg_set_object_attr(entryNodePath, APPFTP_HISTORYID, entryId);
			cfg_set_object_attr(entryNodePath, APPFTP_USERNAME, entryUsername);
			cfg_set_object_attr(entryNodePath, APPFTP_PASSWORD, entryPassword);
			cfg_set_object_attr(entryNodePath, APPFTP_URL, entryUrl);
			cfg_set_object_attr(entryNodePath, APPFTP_PORT, entryPort);
			cfg_set_object_attr(entryNodePath, APPFTP_DEVICEID, entryDeviceId);
			cfg_set_object_attr(entryNodePath, APPFTP_SAVEPATH, entrySavePath);
			cfg_set_object_attr(entryNodePath, APPFTP_RESULT, entryResult);
		}
		entryIdValue = atoi(entryId);
		entryIdValue = (entryIdValue + 1) % APPFTP_HISTORYMAXID;
	}

	/*insert the Downloading node to history */
	memset(entryNodePath, 0, sizeof(entryNodePath));
	snprintf(entryNodePath, sizeof(entryNodePath), APPFTP_ENTRY_NODE,entryIndex+1);

	if(entryIdValue == -1)
	{
		entryIdValue = entryIndex;
	}
	snprintf(entryId, sizeof(entryId), "%d", entryIdValue);
	cfg_set_object_attr(entryNodePath, APPFTP_HISTORYID, entryId);
	cfg_set_object_attr(entryNodePath, APPFTP_USERNAME, username);
	cfg_set_object_attr(entryNodePath, APPFTP_PASSWORD, password);
	cfg_set_object_attr(entryNodePath, APPFTP_URL, url);
	cfg_set_object_attr(entryNodePath, APPFTP_PORT, port);
	cfg_set_object_attr(entryNodePath, APPFTP_DEVICEID, deviceId);
	cfg_set_object_attr(entryNodePath, APPFTP_SAVEPATH, savePath);
	cfg_set_object_attr(entryNodePath, APPFTP_RESULT, APPFTP_RES_DOWNLOADING);

	/*set all the values of Downloading node to "N/A" */
	cfg_set_object_attr(downloadNodePath, APPFTP_USERNAME, APPFTP_INVALID);
	cfg_set_object_attr(downloadNodePath, APPFTP_PASSWORD, APPFTP_INVALID);
	cfg_set_object_attr(downloadNodePath, APPFTP_URL, APPFTP_INVALID);
	cfg_set_object_attr(downloadNodePath, APPFTP_PORT, APPFTP_INVALID);
	cfg_set_object_attr(downloadNodePath, APPFTP_DEVICEID, APPFTP_INVALID);
	cfg_set_object_attr(downloadNodePath, APPFTP_SAVEPATH, APPFTP_INVALID);
	entryIndex ++;
	snprintf(historyCount, sizeof(historyCount), "%d", entryIndex);
	cfg_set_object_attr(downloadNodePath, APPFTP_HISTORYCOUNT, historyCount);

	/* parse the url */
	host = url + 6;
	hostPath = strchr(host, '/');
	if (hostPath)
	{
		*hostPath++ = '\0';
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
		if(tr69FTPClientID)
		snprintf(ftpCommand, sizeof(ftpCommand), "/usr/script/ftpdown.sh '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%d' &", entryId, host, port, username, password,hostPath, deviceId, savePath, tr69FTPClientID);
		else
#endif
		snprintf(ftpCommand, sizeof(ftpCommand), "/usr/script/ftpdown.sh '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' &", entryId, host, port, username, password,hostPath, deviceId, savePath);
		system(ftpCommand);
	}
	else
	{
		cfg_set_object_attr(entryNodePath, APPFTP_RESULT, APPFTP_RES_BAD_URL);
	}

	sleep(2);
	system("sync");/* sync data change to usb device */
	
	return SUCCESS;
}/* end appftp_execute */


int cfg_type_appftp_func_commit(char* path)
{
	 return  cfg_type_appftp_execute(path);
}

static char* cfg_type_appftp_index[] = { 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_appftp_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_appftp_func_commit 
}; 


static cfg_node_type_t cfg_type_appftp_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 10, 
	 .parent = &cfg_type_appftp, 
	 .index = cfg_type_appftp_index, 
	 .ops = &cfg_type_appftp_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_appftp_downloading_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_appftp_func_commit 
}; 


static cfg_node_type_t cfg_type_appftp_downloading = { 
	 .name = "Downloading", 
	 .flag = 1, 
	 .parent = &cfg_type_appftp, 
	 .ops = &cfg_type_appftp_downloading_ops, 
}; 


static cfg_node_ops_t cfg_type_appftp_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_appftp_func_commit 
}; 


static cfg_node_type_t* cfg_type_appftp_child[] = { 
	 &cfg_type_appftp_entry, 
	 &cfg_type_appftp_downloading, 
	 NULL 
}; 


cfg_node_type_t cfg_type_appftp = { 
	 .name = "appFTP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_appftp_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_appftp_child, 
	 .ops = &cfg_type_appftp_ops, 
}; 
