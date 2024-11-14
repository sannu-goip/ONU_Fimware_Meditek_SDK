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
#include <string.h> 
#include <cfg_cli.h> 
#include <svchost_evt.h>

#include "cfg_types.h" 
#include "utility.h" 
#include "cfg_type_voip.h"

#define LINE_TEST_PROC_NAME "/proc/fxs/sliclinetest"
#define LINE_TEST_PHONE_CONN "PhoneConnectivityTest"
#define LINE_TEST_TID_LINE_V  "LineVoltageTest"     
#define LINE_TEST_TID_RES_FLT "LineResistiveTest"

#if defined(TCSUPPORT_CUC)
#define LINE_SIMULATE_TEST "X_CU_SimulateTest"
#else
#if defined(TCSUPPORT_CMCC)
#define LINE_SIMULATE_TEST "X_CMCC_SimulateTest"
#else
#define LINE_SIMULATE_TEST "X_CT-COM_SimulateTest"
#endif
#endif


typedef enum {
    IAD_ERROR_NONE,
    IAD_INNER_ERROR,        /* 1- IAD inner error */
    IAD_ROUTE_FAIL,
    IAD_SERVER_UNREACHABLE,
    IAD_ACCOUNT_FAIL,
    IAD_OTHER_ERROR,        /* 5- other error*/
    IAD_ERRORMAX,
}IAD_ERROR_E;


int cfg_type_voiptest_commit(char* path)
{
	int i = 0;
	int entry[CFG_TYPE_MAX_VOIP_LINE_NUM];
	int simuTestFlag[CFG_TYPE_MAX_VOIP_LINE_NUM] = {0};
	int voipLineNumber = svc_voip_update_line_num();
	int prevSipStatus = 0;
	char tmp[32] = {0};
	char tmpStr[64],tmpStr2[64],tmpStr3[64];
	char testCmd[64];
	char nodeName[64]; 
	char scStatus1[8] = {0};
	char scStatus2[8] = {0};

	if(cfg_obj_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	
	prevSipStatus = atoi(tmp);
	printf("(%s)prevSipStatus: %d\n", __func__, prevSipStatus);

	cfg_get_object_attr(INFOVOIP_COMMON_NODE,"SC_START_STATUS",scStatus1, sizeof(scStatus1));
	cfg_get_object_attr(INFOVOIPH248_COMMON_NODE, "START_STATUS", scStatus2, sizeof(scStatus2));

	memset(tmpStr, 0, sizeof(tmpStr));
	memset(tmpStr2, 0, sizeof(tmpStr2));	
	memset(tmpStr3, 0, sizeof(tmpStr3));

	for(i = 0; i < voipLineNumber; i++)
	{
		memset(nodeName, 0x0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), VOIPTEST_ENTRY_NODE, i+1);
		if((cfg_obj_get_object_attr(nodeName, "TestState", 0, tmpStr, sizeof(tmpStr)) >= 0)
			&& (cfg_obj_get_object_attr(nodeName, "TestSelector", 0, tmpStr2, sizeof(tmpStr2)) >= 0)
			&& (cfg_obj_get_object_attr(nodeName, "IsTestOnBusy", 0, tmpStr3, sizeof(tmpStr3)) >= 0)
			&& (!strcmp(tmpStr,"Requested"))&&(!strcmp(tmpStr3,"0"))){

			if(strcmp(scStatus1, "Yes") && strcmp(scStatus2, "Yes"))
			{
				tcdbg_printf("(%s)no voip app start yet, please don't start tesing until voip app is ready.\n", __func__);
				cfg_obj_set_object_attr(nodeName, "TestState", 0, "Error_TestNotSupported");
				return -1;
			}

			printf("(%s)tmpStr: %s, tmpStr2 = %s, tmpStr3 = %s\n", __func__, tmpStr, tmpStr2, tmpStr3); 	
			if(!strcmp(tmpStr2, LINE_TEST_PHONE_CONN)){
				simuTestFlag[i] = 0;
				memset(testCmd, 0, sizeof(testCmd));
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
				cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
				if(0 == prevSipStatus)
				{
					snprintf(testCmd, sizeof(testCmd), "/userfs/bin/sipclient -b \"%d 3\"", i);
					system_escape(testCmd);
				}
#if defined(TCSUPPORT_ECN_MEGACO)
				else
				{
					snprintf(testCmd, sizeof(testCmd), "callCmd {test.linetest}{%d}{%d}", i, 3);
					voipCmdSend(testCmd);
				}
#endif

			}
			/* Line Voltage Test */
			else if(!strcmp(tmpStr2, LINE_TEST_TID_LINE_V)){
				simuTestFlag[i] = 0;
				memset(testCmd, 0, sizeof(testCmd));
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
				cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
				if(0 == prevSipStatus)
				{
					snprintf(testCmd, sizeof(testCmd), "/userfs/bin/sipclient -b \"%d 1\"", i);
					system_escape(testCmd);
				}
#if defined(TCSUPPORT_ECN_MEGACO)
				else
				{
					snprintf(testCmd, sizeof(testCmd), "callCmd {test.linetest}{%d}{%d}", i, 1);
					voipCmdSend(testCmd);
				}
#endif
			}
			/* Resistive Fault */
			else if(!strcmp(tmpStr2, LINE_TEST_TID_RES_FLT)){
				simuTestFlag[i] = 0;
				memset(testCmd, 0, sizeof(testCmd));
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
				cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
				if(0 == prevSipStatus)
				{
					snprintf(testCmd, sizeof(testCmd), "/userfs/bin/sipclient -b \"%d 4\"", i);
					system_escape(testCmd);
				}
#if defined(TCSUPPORT_ECN_MEGACO)
				else
				{
					snprintf(testCmd, sizeof(testCmd), "callCmd {test.linetest}{%d}{%d}", i, 4);
					voipCmdSend(testCmd);
				}
#endif
			}
			else if(!strcmp(tmpStr2, LINE_SIMULATE_TEST))
			{
				simuTestFlag[i] = 1;
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
			}

		}

	}

	voip_update_protocol();

	if(simuTestFlag[0]||simuTestFlag[1])
	{
		memset(simuTestFlag, 0x0, sizeof(simuTestFlag));
		if(0 == prevSipStatus)
			cfg_type_voip_send_evt(EVT_VOIP_INTERNAL_TYPE, EVT_CFG_VOIP_SIP_SIMULATE_UPDATE, prevSipStatus, path);
		
		if(1 == prevSipStatus)
			cfg_type_voip_send_evt(EVT_VOIP_INTERNAL_TYPE, EVT_CFG_VOIP_H248_SIMULATE_UPDATE, prevSipStatus, path);
	}

	return SUCCESS;
}

int cfg_type_voiptest_set(char* path,char* attr,char* val)
{
	return cfg_obj_set_object_attr(path, attr, 0, val);

#if 0
	int i = 0;
	int entry[CFG_TYPE_MAX_VOIP_LINE_NUM];
	int voipLineNumber = svc_voip_update_line_num();
	char tmpStr[64],tmpStr2[64],tmpStr3[64];
    char testCmd[64];
	char nodeName[64]; 
	char* lineProcFile = LINE_TEST_PROC_NAME;

	memset(tmpStr, 0, sizeof(tmpStr));
	memset(tmpStr2, 0, sizeof(tmpStr2));	
	memset(tmpStr3, 0, sizeof(tmpStr3));

	for(i = 0; i < voipLineNumber; i++)
	{
		memset(nodeName, 0x0, sizeof(nodeName));	
		sprintf(nodeName, "root.voiptest.entry.%d", i+1);
		printf("(%s)nodeName: %s\n", __func__, nodeName);
		if((cfg_obj_get_object_attr(nodeName, "TestState", 0, tmpStr, sizeof(tmpStr)) >= 0)
			&& (cfg_obj_get_object_attr(nodeName, "TestSelector", 0, tmpStr2, sizeof(tmpStr2)) >= 0)
			&& (cfg_obj_get_object_attr(nodeName, "IsTestOnBusy", 0, tmpStr3, sizeof(tmpStr3)) >= 0)
			&& (!strcmp(tmpStr,"Requested"))&&(!strcmp(tmpStr3,"0"))){
			printf("(%s)tmpStr: %s, tmpStr2 = %s, tmpStr3 = %s\n", __func__, tmpStr, tmpStr2, tmpStr3);		
			if(!strcmp(tmpStr2, LINE_TEST_PHONE_CONN)){
				lineTestFlag[i] = 1;		
				memset(testCmd, 0, sizeof(testCmd));
				sprintf(testCmd, "echo \"%d 3\" > %s", i, lineProcFile);
	            cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
	            cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
			}
			/* Line Voltage Test */
			else if(!strcmp(tmpStr2, LINE_TEST_TID_LINE_V)){
				lineTestFlag[i] = 1;
				memset(testCmd, 0, sizeof(testCmd));
				sprintf(testCmd, "echo \"%d 1\" > %s", i, lineProcFile);
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
	            cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
			}
			/* Resistive Fault */
			else if(!strcmp(tmpStr2, LINE_TEST_TID_RES_FLT)){
				lineTestFlag[i] = 1;
				memset(testCmd, 0, sizeof(testCmd));
				sprintf(testCmd, "echo \"%d 4\" > %s", i, lineProcFile);
				cfg_obj_set_object_attr(nodeName, "IsTestOnBusy", 0, "1");
	            cfg_obj_set_object_attr(nodeName, "TestState", 0, "None");
			}
			else
			{
				lineTestFlag[i] = 0;
				return FAIL;
			}
		
			system(testCmd);				

		}

	}
	printf("(%s(%d))lineTestFlag[0] = %d, lineTestFlag[1] = %d\n", __func__, __LINE__, lineTestFlag[0], lineTestFlag[1]);
    return SUCCESS;
#endif
}

static cfg_node_ops_t cfg_type_voiptest_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voiptest_commit, 
}; 

static cfg_node_ops_t cfg_type_voiptest_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voiptest_commit,
};  

cfg_node_type_t cfg_type_voiptest_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE|CFG_TYPE_MAX_VOIP_LINE_NUM, 
	 .parent = &cfg_type_voiptest, 
	 .ops = &cfg_type_voiptest_entry_ops, 
}; 


static cfg_node_type_t* cfg_type_voiptest_child[] = { 
	 &cfg_type_voiptest_entry, 
	 NULL 
};


cfg_node_type_t cfg_type_voiptest= { 
	 .name = "VoIPTest", 
	 .flag = 1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_voiptest_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_voiptest_child, 
	 .ops = &cfg_type_voiptest_ops, 
}; 

