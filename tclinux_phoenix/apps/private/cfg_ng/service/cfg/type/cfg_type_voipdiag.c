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

#define     IAD_SIP_LINE_ID         0
#define     IAD_TEST_RESULT_PATH    "/tmp/IADTestResult"
#define     IAD_TIMEOUT             (60 * 2)

#if defined(TCSUPPORT_CT)
/*********************IAD*********************************************/
#define VOIP_WAN_IFNAME                "SC_SYS_CFG_WAN_IFNAME"
#define VOIP_WAN_IP                    "SC_SYS_CFG_WAN_IP"
#define VOIPBASIC_COMMON               "VoIPBasic_Common"
#define SIP_PROXY_ADDR                 "SBRegistrarServer"
#define SIP_REG_SERVER                 "RegistrarServer"
#define VOIPH248_COMMON                "VoIPH248_Common"
#define H248_MGC                       "MediaGatewayControler"
#define H248_SB_MGC                    "SBMediaGatewayControler"
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

static void resetIADResult(void)
{
	char nodeName[64] = {0};

    strncpy(nodeName, VOIPDIAGNOSTIC_ENTRY_NODE, sizeof(nodeName)-1);
    cfg_set_object_attr(nodeName, "RegistResult", "0");    
    cfg_set_object_attr(nodeName, "Reason", "0");
	return;
}

static int checkIADParams(int* serverIdx)
{
    int ret = -1;
    char IADState[16] = {0};
    char testServer[8] = {0};
    char nodeName[64] = {0};

    strncpy(nodeName, VOIPDIAGNOSTIC_ENTRY_NODE, sizeof(nodeName)-1);
    cfg_obj_get_object_attr(nodeName, "IADDiagnosticsState", 0, IADState, sizeof(IADState));    
    cfg_obj_get_object_attr(nodeName, "TestServer", 0, testServer, sizeof(testServer));
    *serverIdx = atoi(testServer);

    if (!strcmp(IADState, "Requested") && (*serverIdx == 1 || *serverIdx == 2))
    {
        ret = 0;
    }

    return ret;
}


static int getPrevSipStatus(void)
{
	int prevSipStatus = 0;
	char tmp[32] = {0};

	if(cfg_obj_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	
	prevSipStatus = atoi(tmp);
	printf("(%s)prevSipStatus: %d\n", __func__, prevSipStatus);
	return prevSipStatus;

}
static int checkIADVoipWan()
{
    int  ret = -1;
	int  prevSipStatus = getPrevSipStatus();
    char wanName[32] = {0};
    char IPAddr[32] = {0};
    char nodeName[64] = {0};

    strncpy(nodeName, VOIPSYSPARAM_COMMON_NODE, sizeof(nodeName)-1);
#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
	if (0 == prevSipStatus)
    {
	    cfg_obj_get_object_attr(nodeName, VOIP_WAN_IFNAME, 0, wanName, sizeof(wanName));
	    cfg_obj_get_object_attr(nodeName, VOIP_WAN_IP, 0, IPAddr, sizeof(IPAddr));
	    if ((0 != strlen(wanName)) && (0 != strlen(IPAddr)) && (strcmp(IPAddr, "0.0.0.0")!=0)){
	        ret = 0;
	    }
	}
    else{
		ret = 0;
	}
#endif

    return ret;
}

static int checkIADVoipServer(int serverIdx)
{
    int  ret = -1;
	int  prevSipStatus = getPrevSipStatus();
    char nodeName[64] = {0};
    char serName[2][64] = {{0},}; 

    strncpy(nodeName, VOIPBASIC_COMMON_NODE, sizeof(nodeName)-1);    
#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
	if (0 == prevSipStatus)
    {
	    cfg_obj_get_object_attr(nodeName, SIP_REG_SERVER, 0, serName[0], sizeof(serName[0]));
	    cfg_obj_get_object_attr(nodeName, SIP_PROXY_ADDR, 0, serName[1], sizeof(serName[1]));
	    if (strlen(serName[serverIdx-1]) && (strcmp((serName[serverIdx-1]), "0.0.0.0")!=0)){
	        ret = 0;
	    }
	}
    else
    {
	    memset(nodeName, 0x0, sizeof(nodeName));
        strncpy(nodeName, VOIPH248_COMMON_NODE, sizeof(nodeName)-1);    
		cfg_obj_get_object_attr(nodeName, H248_MGC, 0, serName[0], sizeof(serName[0]));
		cfg_obj_get_object_attr(nodeName, H248_SB_MGC, 0, serName[1], sizeof(serName[1]));
	    if (strlen(serName[serverIdx-1]) && (strcmp((serName[serverIdx-1]), "0.0.0.0")!=0))
        {
	        ret = 0;
	    }
	}
#endif

    return ret;
}

static int checkIADModule(int instance)
{
    int    ret = 0;
    char   tempValue[16] = {0};
    char   nodeName[64] = {'\0'}; 
    char   lastErrorReason[16] = {0};
    int    currErrorReason = 0;
	int  prevSipStatus = getPrevSipStatus();

#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)  
    if(0 == prevSipStatus)
        snprintf(nodeName, sizeof(nodeName), "InfoVoIP_Entry%d", instance);
    else
        snprintf(nodeName, sizeof(nodeName), "InfoVoIPH248_Entry%d", instance);
   
    cfg_obj_get_object_attr(nodeName, "RegFailReason", 0, lastErrorReason, sizeof(lastErrorReason));/* save lastErrorReason*/
    cfg_set_object_attr(nodeName, "RegFailReason", "0");  /* iad state restore*/
    /* check sipclient or  mgapp is down or not when cfg_obj_get_object_attr RegFailReason */ 
    cfg_obj_get_object_attr(nodeName, "RegFailReason", 0, tempValue, sizeof(tempValue));       
    currErrorReason = atoi(tempValue);

    if (IAD_INNER_ERROR == currErrorReason)
        ret = -1;
    else
        cfg_set_object_attr(nodeName, "RegFailReason", lastErrorReason); /* restore  lastErrorReason */        
#endif

    return ret;
}

int cfg_type_voipdiag_commit(char* path)
{
    int ret = 0;
    int reason = IAD_OTHER_ERROR;
    int serverIdx = 1;
	int prevSipStatus = getPrevSipStatus();
	voip_evt_t voip_evt;

	memset(&voip_evt, 0, sizeof(voip_evt));

	if((NULL == path) || (strlen(path) >= EVT_BUF_LENGTH))
	{
		printf("error path is NULL || path length is too long.\n");
		return -1;
	}
    voip_evt.prevSipStatus = prevSipStatus;
	strncpy(voip_evt.buf, path, sizeof(voip_evt.buf)-1);

    resetIADResult();//reset the IAD Result before diag.

    /* check itms set params */
    checkIADParams(&serverIdx);   

    ret = checkIADVoipWan();
    if (-1 == ret){
        reason = IAD_ROUTE_FAIL;
    }

    ret = checkIADVoipServer(serverIdx);
    if (-1 == ret){
        reason = IAD_SERVER_UNREACHABLE;
    }

    /*check IAD module*/
    ret = checkIADModule(IAD_SIP_LINE_ID);
    if (-1 == ret){
        reason = IAD_INNER_ERROR;
    }
    voip_evt.registReason = reason;
    cfg_obj_send_event(EVT_VOIP_INTERNAL, EVT_VOIP_IAD_START, (void*)&voip_evt, sizeof(voip_evt));

	return 0;

}

static cfg_node_ops_t cfg_type_voipdiag_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voipdiag_commit, 
}; 

static cfg_node_ops_t cfg_type_voipdiag_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_voipdiag_commit,
};  

cfg_node_type_t cfg_type_voipdiag_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_voipdiag, 
	 .ops = &cfg_type_voipdiag_entry_ops, 
}; 


static cfg_node_type_t* cfg_type_voipdiag_child[] = { 
	 &cfg_type_voipdiag_entry, 
	 NULL 
};


cfg_node_type_t cfg_type_voipdiag= { 
	 .name = "VoIPDiagnostic", 
	 .flag =  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_voipdiag_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_voipdiag_child, 
	 .ops = &cfg_type_voipdiag_ops, 
};

