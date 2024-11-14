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
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>

#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_msg.h" 
#include <crypt.h>
#include <svchost_evt.h>

int svc_cfg_boot_account(void)
{
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ACCOUNT_BOOT, NULL, 0);	
	return 0;
}

int cfg_type_account_func_commit(char* path)
{
	other_evt_t param;

#if defined(TCSUPPORT_CWMP_TR181)
	char tmpValue[8]={0};
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	if(strcmp(tmpValue,"1"))
	{	
		/*not commit from tr181*/
		updateUsers();
	}
#endif

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ACCOUNT_UPDATE, (void*)&param, sizeof(param));
	
#if defined(TCSUPPORT_CT_JOYME2)	
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
	param.flag = SAMBA_SERVER_ACCOUNT_COMMIT_FLAG;
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_SAMBA_UPDATE, (void*)&param, sizeof(param));
#endif
#endif

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
int cfg_type_account_logout_func_commit(char* path)
{
#ifdef TCSUPPORT_SYSLOG_ENHANCE
	char log[128] = {0};
	char nodeName[64] = {0}, username[32] = {0}, log_state[8] = {0}, str_cur[8] = {0};
	int i = 0;

	bzero(log_state, sizeof(log_state));
	bzero(str_cur, sizeof(str_cur));
	bzero(nodeName, sizeof(nodeName));
	
	cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "CurrentAccess", str_cur, sizeof(str_cur));
	snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, atoi(str_cur) + 1);
	
	cfg_obj_get_object_attr(nodeName, "Logoff", 0, log_state, sizeof(log_state));
	if (!strcmp(log_state, "1")) 
	{
		bzero(username, sizeof(username));
		cfg_obj_get_object_attr(nodeName, "username", 0, username, sizeof(username));
		snprintf(log, sizeof(log), "WEB user <%s> logout.\n", username);
		openlog("TCSysLog WEB", 0, LOG_LOCAL2);
		syslog(LOG_INFO, log);
		closelog();
	}
#endif
	return 0;
}
#endif
#if defined(TCSUPPORT_CMCCV2)
static cfg_node_ops_t cfg_type_account_wantelnetentry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 

static cfg_node_type_t cfg_type_account_wantelnetentry = { 
	 .name = "WANTelnetEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_wantelnetentry_ops, 
}; 
#endif
static char* cfg_type_account_index[] = { 
	 "account_id", 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_account_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 3, 
	 .parent = &cfg_type_account, 
	 .index = cfg_type_account_index, 
	 .ops = &cfg_type_account_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_account_telnetentry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_telnetentry = { 
	 .name = "TelnetEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_telnetentry_ops, 
}; 
static cfg_node_ops_t cfg_type_account_telnetentry1_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_telnetentry1 = { 
	 .name = "TelnetEntry1", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_telnetentry1_ops, 
}; 
static cfg_node_ops_t cfg_type_account_ftpcommon_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpcommon = { 
#if defined(TCSUPPORT_CT_JOYME2)
	 .name = "FtpCommon",
#else
	 .name = "FtpEntry", 
#endif
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpcommon_ops, 
}; 


static cfg_node_ops_t cfg_type_account_ftpentry0_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 

static cfg_node_type_t cfg_type_account_ftpentry0 = { 
	 .name = "FtpEntry0", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry0_ops, 
}; 

static cfg_node_ops_t cfg_type_account_ftpentry1_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpentry1 = { 
	 .name = "FtpEntry1", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry1_ops, 
}; 
static cfg_node_ops_t cfg_type_account_ftpentry2_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpentry2 = { 
	 .name = "FtpEntry2", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry2_ops, 
}; 
static cfg_node_ops_t cfg_type_account_ftpentry3_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpentry3 = { 
	 .name = "FtpEntry3", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry3_ops, 
}; 
static cfg_node_ops_t cfg_type_account_ftpentry4_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpentry4 = { 
	 .name = "FtpEntry4", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry4_ops, 
}; 
static cfg_node_ops_t cfg_type_account_ftpentry5_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_ftpentry5 = { 
	 .name = "FtpEntry5", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_ftpentry5_ops, 
}; 


static cfg_node_ops_t cfg_type_account_consoleentry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_consoleentry = { 
	 .name = "ConsoleEntry", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_consoleentry_ops, 
}; 


static cfg_node_ops_t cfg_type_account_tr64entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 


static cfg_node_type_t cfg_type_account_tr64entry = { 
	 .name = "TR64Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_tr64entry_ops, 
}; 


static cfg_node_ops_t cfg_type_account_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_func_commit 
}; 

#if defined(TCSUPPORT_CT_JOYME2)
static cfg_node_ops_t cfg_type_account_logout_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_account_logout_func_commit 
}; 


static cfg_node_type_t cfg_type_account_logout = { 
	 .name = "LogOut", 
	 .flag = 1, 
	 .parent = &cfg_type_account, 
	 .ops = &cfg_type_account_logout_ops, 
}; 
#endif

static cfg_node_type_t* cfg_type_account_child[] = { 
	 &cfg_type_account_entry, 
	 &cfg_type_account_telnetentry, 
#if defined(TCSUPPORT_CMCCV2)
	 &cfg_type_account_wantelnetentry,
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_account_telnetentry1,
#endif
	 &cfg_type_account_ftpcommon, 
	 &cfg_type_account_ftpentry0, 
	 &cfg_type_account_ftpentry1,
 	 &cfg_type_account_ftpentry2,
 	 &cfg_type_account_ftpentry3,
 	 &cfg_type_account_ftpentry4,
 	 &cfg_type_account_ftpentry5,	 
	 &cfg_type_account_consoleentry, 
	 &cfg_type_account_tr64entry, 
#if defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_account_logout, 
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_account = { 
	 .name = "Account", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_account_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_account_child, 
	 .ops = &cfg_type_account_ops, 
}; 
