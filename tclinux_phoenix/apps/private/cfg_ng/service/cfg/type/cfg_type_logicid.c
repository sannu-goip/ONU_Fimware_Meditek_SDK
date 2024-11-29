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
#include <sys/ipc.h>
#include<sys/msg.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "cfg_msg.h"
#include "utility.h"

#define CWMP_MAX_MSG_LEN 256
#define CWMP_MQ_FLAG_PATH "/tmp/cwmp/tr069agent_mq"
#define PROJID 2

/* 
return value: 
0:no need to restore loid
1:need to restore loid from node logicid to epon/gpon
*/
int check_restore_logicid()
{
	char sbuf[128] = {0}, dbuf[128] = {0};

	/* checking for loid */
	memset(sbuf, 0, sizeof(sbuf));
	cfg_get_object_attr(LOGICID_ENTRY_NODE, LOGICID_USERNAME, sbuf, sizeof(sbuf));

	/* get loid from epon only, because epon loid will be same with gpon */
	memset(dbuf, 0, sizeof(dbuf));
	cfg_get_object_attr(EPON_LOIDAUTH_NODE, EPON_LOID_USR, dbuf, sizeof(dbuf));
	/* restore loid when it is different between source and dest */
	if ( 0 != strcmp(sbuf, dbuf) )
		return 1;

	/* checking for password */
	memset(sbuf, 0, sizeof(sbuf));
	cfg_get_object_attr(LOGICID_ENTRY_NODE, LOGICID_PSW, sbuf, sizeof(sbuf));

	/* get password from epon only, because epon loid will be same with gpon */
	memset(dbuf, 0, sizeof(dbuf));
	cfg_get_object_attr(EPON_LOIDAUTH_NODE, EPON_LOID_PSW, dbuf, sizeof(dbuf));
	/* restore password when it is different between source and dest */
	if ( 0 != strcmp(sbuf, dbuf) )
		return 1;
	
	return 0;
}

int svc_cfg_boot_logicid(void)
{
	char v_value[128] = {0};
	char isTR69Chg[12] = {0};
	int intIsTR69Chg = 0;
	int intisUserNamechg = 0;
	char username[128] = {0}, password[128] = {0};
	char logicidPath[64] = {0};
	char webcurPath[64] = {0};
#if defined(CT_COM_DEVICEREG) || defined(TCSUPPORT_CT_PON_BIND2_WEB)
	char devPath[64] = {0};
	char cwmpPath[64] = {0};
	char tmpval[10] = {0};
	int isNOBootRegister = 0;
#ifdef CWMP
	cwmp_msg_t message;
	long type = 1;	/*tr69 must be 1 */
	int msgFlag = IPC_NOWAIT;
	int mq = VALUE_CHANGED; 		/*value change type  */
#endif
#endif
#if defined(TCSUPPORT_CT_PON)
	char eponPath[64] = {0};
	char gponPath[64] = {0};
#endif
	int isNeedRestore = 0;

	snprintf(logicidPath, sizeof(logicidPath), LOGICID_ENTRY_NODE);
	snprintf(webcurPath, sizeof(webcurPath), WEBCURSET_ENTRY_NODE);
	
#if defined(CT_COM_DEVICEREG) || defined(TCSUPPORT_CT_PON_BIND2_WEB)
	snprintf(devPath, sizeof(devPath), DEVICEACCOUNT_ENTRY_NODE);
	snprintf(cwmpPath, sizeof(cwmpPath), CWMP_ENTRY_NODE);
#endif

#if defined(TCSUPPORT_CT_PON)
	snprintf(eponPath, sizeof(eponPath), EPON_LOIDAUTH_NODE);
	snprintf(gponPath, sizeof(gponPath), GPON_LOIDAUTH_NODE);
#endif


	isNeedRestore = check_restore_logicid();

	memset(isTR69Chg, 0, sizeof(isTR69Chg));
	if(cfg_obj_get_object_attr(webcurPath, "isTR69Chg", 0, isTR69Chg, sizeof(isTR69Chg)) > 0 && 0 == strcmp(isTR69Chg, "1"))
	{
		cfg_set_object_attr(webcurPath, "isTR69Chg", "0");
		intIsTR69Chg = 1;
	}
	else
	{
		intIsTR69Chg= 0;
	}
	 /* restore the correct informations */
#ifdef CT_COM_DEVICEREG
#if !defined(TCSUPPORT_CT_PON) && !defined(TCSUPPORT_CT_E8B_ADSL)
	 memset(v_value, 0, sizeof(v_value) );
	if(cfg_obj_get_object_attr(logicidPath, LOGICID_REG_STATUS, 0, v_value, sizeof(v_value)) < 0)
	 {
		strncpy(v_value, "99", sizeof(v_value)-1);
	 }	
	cfg_set_object_attr(devPath, DEVACCOUNT_STATUS, v_value);
	 
	memset(v_value, 0, sizeof(v_value) );
	if(cfg_obj_get_object_attr(logicidPath, LOGICID_REG_RESULT, 0, v_value, sizeof(v_value)) < 0)
	{
		strncpy(v_value, "99",sizeof(v_value)-1);
	}	
	cfg_set_object_attr(devPath, DEVACCOUNT_RESULT, v_value);
#endif
#endif

	memset(v_value, 0, sizeof(v_value) );
	if(cfg_obj_get_object_attr(logicidPath, LOGICID_USERNAME, 0, v_value, sizeof(v_value)) > 0
		&& (('\0' != v_value[0]) || ('\0' == v_value[0] && (1 == intIsTR69Chg
#if defined(TCSUPPORT_CT_PON_C7)
		|| (1) /* alway use the loid from <LogicID> node. */
#endif
		))))
	{
#if defined(CT_COM_DEVICEREG) || defined(TCSUPPORT_CT_PON_BIND2_WEB)
		cfg_set_object_attr(devPath, DEVACCOUNT_USERNAME, v_value);
#endif
#if defined(TCSUPPORT_CT_PON)
		cfg_set_object_attr(eponPath, EPON_LOID_USR, v_value);
		cfg_set_object_attr(gponPath, GPON_LOID_USR, v_value);
#endif
		intisUserNamechg = 1;
	}

	memset(v_value, 0, sizeof(v_value) );
	if(cfg_obj_get_object_attr(logicidPath, LOGICID_PSW, 0, v_value, sizeof(v_value)) > 0
		&& (('\0' != v_value[0]) || ('\0' == v_value[0] && (1 == intIsTR69Chg
		|| 1 == intisUserNamechg
		))))
	{
#if defined(CT_COM_DEVICEREG) || defined(TCSUPPORT_CT_PON_BIND2_WEB)
		cfg_set_object_attr(devPath, DEVACCOUNT_USERPASSWD, v_value);
#endif
#if defined(TCSUPPORT_CT_PON)
		cfg_set_object_attr(eponPath, EPON_LOID_PSW, v_value);
		cfg_set_object_attr(gponPath, GPON_LOID_PSW, v_value);
#endif
	}

#if defined(TCSUPPORT_CT_PON)
	if ( isNeedRestore )
	{
		cfg_commit_object(EPON_LOIDAUTH_NODE);    
		cfg_commit_object(GPON_LOIDAUTH_NODE);    
	} 
#endif
	
#ifdef CT_COM_DEVICEREG
	memset(tmpval, 0 , sizeof(tmpval));
	if(cfg_obj_get_object_attr(devPath, "isNOBootRegister", 0, tmpval, sizeof(tmpval)) < 0 )
	{
		strncpy(tmpval, "0", sizeof(tmpval)-1);
	}	
	isNOBootRegister = atoi(tmpval);
	
#if defined(TCSUPPORT_CT_ADSL_HN)
	cfg_set_object_attr(cwmpPath, "devregInform", "0");
#endif

	cfg_commit_object(DEVICEACCOUNT_ENTRY_NODE);    

	if(cfg_obj_get_object_attr(devPath, DEVACCOUNT_USERNAME, 0, username, sizeof(username)) < 0 )
	{
		username[0] = '\0';
	}
	
	if(cfg_obj_get_object_attr(devPath, DEVACCOUNT_USERPASSWD, 0, password, sizeof(password)) < 0 )
	{
		password[0] = '\0';
	}
		
	/* register ITMS+ again if device state is locked. */
	memset(v_value, 0, sizeof(v_value) );
	if( 0 == isNOBootRegister
		&& (cfg_obj_get_object_attr(devPath, DEVACCOUNT_WEBPAGELOCK, 0, v_value, sizeof(v_value)) >0 )
		&& 0 != strcmp(v_value, "0")
		&& '\0' != username[0] && '\0' != password[0] )
	{
#if !defined(TCSUPPORT_CT_FJ)
		cfg_set_object_attr(cwmpPath, "devregInform", "1");
#endif
		cfg_set_object_attr(devPath, "newStatus", "0");
		cfg_set_object_attr(devPath, "newResult", "0");
#ifdef CWMP
		memset(&message, 0, sizeof(cwmp_msg_t));
		message.cwmptype = mq;
		if ( sendmegq(type,&message,msgFlag) < 0 )
		{
			printf("\nLogicID_boot==> send message error!\n");
		}	
#endif
	}
#endif

	return SUCCESS;
}

int cfg_type_logicid_execute()
{
	return  svc_cfg_boot_logicid();
}

int cfg_type_logicid_func_commit(char* path)
{
	 return cfg_type_logicid_execute( );
}

static cfg_node_ops_t cfg_type_logicid_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_logicid_func_commit 
}; 


static cfg_node_type_t cfg_type_logicid_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_logicid, 
	 .ops = &cfg_type_logicid_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_logicid_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_logicid_func_commit 
}; 


static cfg_node_type_t* cfg_type_logicid_child[] = { 
	 &cfg_type_logicid_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_logicid = { 
	 .name = "LogicID", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_logicid_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_logicid_child, 
	 .ops = &cfg_type_logicid_ops, 
}; 
