
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
#include <sys/ipc.h>

#include <svchost_api.h> 
#include <cfg_api.h>

#include "voip_cfg.h"
#include "voip_common.h"
#include "voip_mgr.h"
#include "utility.h"


#define SVC_VOIP_CMD_BUFF_LEN	128
#define SVC_VOIP_MAX_LINE       4
#define IAD_SIP_EMULTE_REG_CMD  "/userfs/bin/sipclient -e \"%d %d\""
#define IAD_SIP_DEL_CMD         "/userfs/bin/sipclient -d %d"
#define IAD_SIP_UPDATE_CMD      "/userfs/bin/sipclient -u %d"
#define IAD_SIP_EMULTE_REG_CMD  "/userfs/bin/sipclient -e \"%d %d\""
#define IAD_SIP_KILLALL_CMD     "killall -9 sipclient"
#define IAD_SIP_START_CMD       "/userfs/bin/sipclient &"

voipRegInfo_t voipRegInfo;
static char protocol[8] = "IPV4";
static VoIP_Wan_info_t VoIP_Wan_info_array[MAX_VOIP_WAN] = {0};


void update_infovoip_status(int lineId)
{
	char node[64] = {0};
	char tmp[32] = {0};

	memset(node, 0x0, sizeof(node));
	snprintf(node, sizeof(node), VOIPBASIC_ENTRY_NODE, lineId+1);
	if(-1 != cfg_get_object_attr(node, "Enable", tmp, sizeof(tmp)))
	{
		if (0 == strcmp(tmp, "Yes"))
		{
			memset(node, 0x0, sizeof(node));
			snprintf(node, sizeof(node), INFOVOIP_ENTRY_NODE, lineId+1);

			if(cfg_get_object_attr(node, "RegFailReason", tmp, sizeof(tmp)) >= 0)
			{
				if(0 != strcmp(tmp, VOICE_NO_WAN_ERROR))
					cfg_set_object_attr(node, "Status", "Initializing");
			}
			else
			{
				cfg_set_object_attr(node, "Status", "Initializing");
			}

		}
		else
		{
			memset(node, 0x0, sizeof(node));
			snprintf(node, sizeof(node), INFOVOIP_ENTRY_NODE, lineId+1);
			cfg_set_object_attr(node, "Status", "Disabled");

		}

	}
	return;
}

int svc_voip_sip_update_handle(char* path)
{
    char cmd[SVC_VOIP_CMD_BUFF_LEN] = {0};
    char tmp[32] = {0};
    char tmpStr[64] = {0};
#if 0
    char tmpStr1[64] = {0};
    char tmpStr2[64] = {0};
#endif
    char node[64] = {0};
    char* entryStr = NULL;
    int  voipLineNumber = 0;
    /*lineId is used to distinguish different fxs line, the default value 17 represent all fxs line*/
    int  lineId = -1;
    int  i = 0;

    memset(node, 0, sizeof(node));
    strncpy(node, INFOVOIP_COMMON_NODE, sizeof(node) - 1);
    cfg_get_object_attr(node, "WAN_SET_ENABLE", tmp, sizeof(tmp));

    /*deregister sipclient when reboot command is sent by tr069*/
    if (0 == strcasecmp(tmp, "Yes")){
        SVC_VOIP_NOTICE_INFO("===reboot===>\n");
        voipLineNumber = svc_voip_update_line_num();
        for(i = 0; i < voipLineNumber; i++){
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/userfs/bin/sipclient -d %d",i);
#if defined(TCSUPPORT_SDN_OVS)
            voip_netns_start(cmd);
#else 
            svc_voip_execute_cmd(cmd);
#endif
        }
        cfg_set_object_attr(node, "WAN_SET_ENABLE", "");

    }

#if 0
    /*speed up dialing*/
    sprintf(node,"root.%s.common", VOIPSPEED);
    cfg_set_object_attr(node, "MTKSDEnable", "0");

    for(i = 0; i < 10; i++){
        memset(node, 0x0, sizeof(node));
        sprintf(node,"root.%s.entry.%d", VOIPSPEED, i);
        cfg_get_object_attr(node, "WAN_SET_ENABLE",tmp,sizeof(tmp));
        if((-1!=cfg_get_object_attr(node, "SpeedDialNumber", tmpStr1, sizeof(tmpStr1))) && \
            (-1!=cfg_get_object_attr(node, "UserId", tmpStr2, sizeof(tmpStr2))))
        {
            memset(node, 0x0, sizeof(node));
            sprintf(node,"root.%s.common", VOIPSPEED);
            cfg_set_object_attr(node, "MTKSDEnable", "1");
            break;

        }
    }
#endif

    /*Get lineId from in parameter name */
    if (NULL != (entryStr = strstr(path, "voipcodecs"))){
        if(NULL != (entryStr = strstr(path, "pvc.")))
        {
            SVC_VOIP_DEBUG_INFO("pvc = %s\n", entryStr);
            sscanf(entryStr, "pvc.%d", &i);
            lineId = i-1;
        }
    }
    else if(NULL != (entryStr = strstr(path, "entry.")))
    {
        SVC_VOIP_DEBUG_INFO("entryStr = %s\n", entryStr);
        sscanf(entryStr, "entry.%d", &i);
        lineId = i-1;
        update_infovoip_status(lineId);
    }
    else
    {
        voipLineNumber = svc_voip_update_line_num();
        for(i = 0; i < voipLineNumber; i++)
        {
           update_infovoip_status(i);
        }
    }

    SVC_VOIP_DEBUG_INFO("lineId = %d\n", lineId);
    memset(tmpStr, 0, sizeof(tmpStr));
    snprintf(tmpStr, sizeof(tmpStr), "/userfs/bin/sipclient -r %d", lineId);
#if defined(TCSUPPORT_SDN_OVS)
    voip_netns_start(tmpStr);
#else
    svc_voip_execute_cmd(tmpStr);
#endif
    return 0;
}

#if defined(TCSUPPORT_ECN_MEGACO)
int svc_voip_h248_update_handle(char* path)
{
	char nodeName[32] = {0};
	char infoH248Node[32] = {0};
	char MandatoryReg[8] ={0};
	char MandatoryRegFlag[8] ={0};

	strncpy(nodeName, VOIPH248_COMMON_NODE, sizeof(nodeName) - 1);
	if(cfg_get_object_attr(nodeName, MANDATORY_REGISTER, MandatoryReg, sizeof(MandatoryReg)) < 0)
		return -1;

	if(0 == strcmp(MandatoryReg,"Yes"))
	{
		strncpy(infoH248Node, INFOVOIPH248_COMMON_NODE, sizeof(infoH248Node) -1);
		if(cfg_get_object_attr(infoH248Node, MANDATORY_REGISTER_FLAG, MandatoryRegFlag, sizeof(MandatoryRegFlag)) < 0)
			return -1;
		if(0 == strcmp(MandatoryRegFlag,"1"))
		{
			SVC_VOIP_NOTICE_INFO("h248RealExecute: MandatoryRegister is True!!!\n");
			cfg_set_object_attr(infoH248Node, MANDATORY_REGISTER_FLAG, "0");
			voipCmdSend("callCmd {test.Restart}");
			return 0;
		}
	}

	SVC_VOIP_NOTICE_INFO("Reload H248.\n");
	voipCmdSend("callCmd {test.reload}");
	return 0;
}

int svc_voip_h248_normal_handle_event_update(char* path)
{
    svc_voip_h248_update_handle(path);
    return 0;
}
#endif

int svc_voip_sip_normal_handle_event_update(char* path)
{
	char curIfIndex[32] = {0};
	char ifIndexUsed[32] = {0};
	char wanNodeName[32] = {0};
	char lanEnable[8] = {0};

	char str_servicelist[32] = {0};
	char str_wanmode[8] = {0};
	char ifName[32] = {0};

	int if_index = 0;
	int pvc_index = -1;
	int entry_index = -1;
	int lineNum = 0;
	char IP_Protocol[8] = {0};
	int flag = 0;
#ifdef TCSUPPORT_IPV6
	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
		strcpy(IP_Protocol, "IPV4");
	}
	if(strcmp(protocol, IP_Protocol)){
		flag = 1;
		strcpy(protocol, IP_Protocol);	
	}
#endif
	lineNum = svc_voip_update_line_num();
	cfg_get_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFINDEX_USED", ifIndexUsed, sizeof(ifIndexUsed));

	if((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", curIfIndex, sizeof(curIfIndex)) >=0)&&
		((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "LanBindEnable", lanEnable, sizeof(lanEnable)) < 0)
		||((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "LanBindEnable", lanEnable, sizeof(lanEnable)) >= 0) && (strcmp(lanEnable, "1")))))
	{
		if((strcmp(ifIndexUsed, curIfIndex) || (1 == flag))&& isNumber(curIfIndex))
		{
			/*wan bind changed*/
			if_index = atoi(curIfIndex);	
			pvc_index = if_index / MAX_SMUX_NUM;
			entry_index = if_index % MAX_SMUX_NUM;
			SVC_VOIP_DEBUG_INFO("pvc_index = %d, entry_index = %d\n", pvc_index, entry_index);
			memset(wanNodeName, 0, sizeof(wanNodeName));
			snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);
			if(cfg_get_object_attr(wanNodeName, "Active", str_wanmode, sizeof(str_wanmode))>= 0 &&
				0 == strcmp(str_wanmode, "Yes") &&
				cfg_get_object_attr(wanNodeName, "WanMode", str_wanmode, sizeof(str_wanmode))>= 0 &&
				0 == strcmp(str_wanmode, "Route") &&
				cfg_get_object_attr(wanNodeName, "IFName", ifName, sizeof(ifName))>= 0 &&
				cfg_get_object_attr(wanNodeName, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
				NULL != strstr(str_servicelist, "VOICE"))
			{
				update_sip_reg_param(lineNum, if_index, ifName, wanNodeName, 1);
				return 0;
			}
		}	

	}

	svc_voip_sip_update_handle(path);
	return 0;
}

static int svc_voip_sip_simulate_handle_event_update(char* path)
{
    int  i = 0;
    int  testType = 0;
    int  lineNumber = 0;
    char cmd[64]   = {0};
    char node1[32] = {0};
    char node2[32] = {0};
    char tmp1[32]  = {0};
    char tmp2[32]  = {0};
    char tmp3[32]  = {0};
    char tmp4[32]  = {0};

    lineNumber = svc_voip_update_line_num();

    for(i = 0; i < lineNumber; i++)
    {
        memset(node1, 0x0, sizeof(node1));
        snprintf(node1, sizeof(node1), VOIPSIMULATETEST_ENTRY_NODE, i+1);

        memset(node2, 0x0, sizeof(node2));
        snprintf(node2, sizeof(node2), VOIPTEST_ENTRY_NODE, i+1);

        if((cfg_get_object_attr(node2, "TestState", tmp1, sizeof(tmp1)) >= 0)&&
           (cfg_get_object_attr(node2, "TestSelector", tmp2, sizeof(tmp2)) >= 0)&&
           (cfg_get_object_attr(node2, "IsTestOnBusy", tmp3, sizeof(tmp3)) >= 0)&&
           (cfg_get_object_attr(node1, "TestType", tmp4, sizeof(tmp4)) >= 0)&&
           (!strcmp(tmp1, "Requested"))&&
           (!strcmp(tmp3, "1")))
           
        {
            SVC_VOIP_DEBUG_INFO("tmp1: %s, tmp2: %s, tmp3: %s, tmp4: %s\n", tmp1, tmp2, tmp3, tmp4);
            if (!strcmp(tmp4, LINE_CALLER_SIMULATE)){
                testType = SIMULATE_CALLER;
            }
            else if (!strcmp(tmp4, LINE_CALLED_SIMULATE)){
                testType = SIMULATE_CALLED;
            }
            else{
                SVC_VOIP_TRACE_INFO("unsupported simulate type for line %d!\n", i);
                testType = SIMULATE_NONE;
                cfg_set_object_attr(node2, "TestState", "None");
                cfg_set_object_attr(node2, "IsTestOnBusy", "0");
                continue;
            }
            snprintf(cmd, sizeof(cmd), "/userfs/bin/sipclient -h \"%d %d\"", i, testType);
            svc_voip_execute_cmd(cmd);/*TODO: when simulate test has completed, notify tr069 by signalCwmpInfo()*/
            cfg_set_object_attr(node2, "TestState", "None");
            cfg_set_object_attr(node2, "IsTestOnBusy", "0");
        }
    }

    return 0;
}

#if defined(TCSUPPORT_ECN_MEGACO)
static int svc_voip_h248_simulate_handle_event_update(char* path)
{
    int i = 0;
    int lineNumber = 0;

    char cmd[64]   = {0};
    char node1[32] = {0};
    char node2[32] = {0};
    char tmp1[32]  = {0};
    char tmp2[32]  = {0};
    char tmp3[32]  = {0};
    char tmp4[32]  = {0};
    char testType = 0;

    lineNumber = svc_voip_update_line_num();

    for(i = 0; i < lineNumber; i++)
    {
        memset(node1, 0x0, sizeof(node1));
        snprintf(node1, sizeof(node1), VOIPSIMULATETEST_ENTRY_NODE, i+1);

        memset(node2, 0x0, sizeof(node2));
        snprintf(node2, sizeof(node2), VOIPTEST_ENTRY_NODE, i+1);

        if((cfg_get_object_attr(node2, "TestState", tmp1, sizeof(tmp1)) >= 0)&&
           (cfg_get_object_attr(node2, "TestSelector", tmp2, sizeof(tmp2)) >= 0)&&
           (cfg_get_object_attr(node2, "IsTestOnBusy", tmp3, sizeof(tmp3)) >= 0)&&
           (cfg_get_object_attr(node1, "TestType", tmp4, sizeof(tmp4)) >= 0)&&
           (!strcmp(tmp1, "Requested"))&&
           (!strcmp(tmp3, "1")))
        {
            SVC_VOIP_DEBUG_INFO("tmp1: %s, tmp2: %s, tmp3: %s, tmp4: %s\n", tmp1, tmp2, tmp3, tmp4);

            if (!strcmp(tmp4, LINE_CALLER_SIMULATE)){
                testType = SIMULATE_CALLER;
            }
            else if (!strcmp(tmp4, LINE_CALLED_SIMULATE)){
                testType = SIMULATE_CALLED;
            }
            else{
                SVC_VOIP_TRACE_INFO("unsupported simulate type for line %d!\n", i);
                testType = SIMULATE_NONE;
                continue;
            }
            snprintf(cmd, sizeof(cmd), "callCmd {test.simulate}{%d}{%d}", i, testType);
            voipCmdSend(cmd);/*TODO: when simulate test has completed, notify tr069 by signalCwmpInfo()*/
            cfg_set_object_attr(node2, "TestState", "None");
            cfg_set_object_attr(node2, "IsTestOnBusy", "0");

        }
    }

    return 0;
}
#endif

#if 0
static void svc_voip_dereg_before_reboot(char* path)
{
    char node[32] = {0};
    char cmd[64]  = {0};
    char tmp[32]  = {0};
    int i = 0;
    int voipLineNumber = svc_voip_update_line_num();

    for(i = 0; i < voipLineNumber; i++)
    {
        memset(node, 0x0, sizeof(node));
        sprintf(node, "root.%s.entry.%d", VOIPBASIC, i);
        if(-1 != (cfg_get_object_attr(node, "Enable", tmp, sizeof(tmp))))
        {
            memset(cmd, 0, sizeof(cmd));
            if(0 == strcmp(tmp, "Yes"))
            {
                sprintf(cmd,"/userfs/bin/sipclient -d %d",i);
                printf("Before reboot, deregister [cmd:%s]\n", cmd);
                svc_voip_execute_cmd(cmd);
            } 

        }
    }
    return;
}
#endif

static int svc_voip_handle_event_iad_test(char* path, int voipProtocol, int reason)
{
    char cmd[64] = {0};
    char nodeName[64] = {0};
    char testServer[8] = {0};
    int serverIdx = 1;

    cwmp_msg_t message;
    long type = 1;	//tr69 must be 1
    int msgFlag = IPC_NOWAIT;

    strncpy(nodeName, VOIPDIAGNOSTIC_ENTRY_NODE, sizeof(nodeName) - 1);
    cfg_get_object_attr(nodeName, "TestServer", testServer, sizeof(testServer));
    serverIdx = atoi(testServer);

    SVC_VOIP_DEBUG_INFO("voipProtocol = %d, reason = %d\n", voipProtocol, reason);
    if((IAD_ROUTE_FAIL == reason) || (IAD_SERVER_UNREACHABLE == reason) || (IAD_INNER_ERROR == reason))
    {
        cfg_set_object_attr(VOIPDIAGNOSTIC_ENTRY_NODE, "IADDiagnosticsState", "Complete");
        cfg_set_object_attr(nodeName, "RegistResult", "1");
        cfg_set_object_attr(nodeName, "Reason", itoa(reason));
        /*return app to normal state*/
        if(0 == voipProtocol)
            svc_voip_sip_update_handle(path);
#if defined(TCSUPPORT_ECN_MEGACO)
        else
            svc_voip_h248_update_handle(VOIPH248_NODE);
#endif

        memset(&message, 0, sizeof(cwmp_msg_t));
        message.cwmptype = DIAG_COMPLETE;
        sendmegq(type,&message,msgFlag);

    }
    else
    {
        if(0 == voipProtocol)
        {
            snprintf(cmd, sizeof(cmd), IAD_SIP_EMULTE_REG_CMD, IAD_SIP_LINE_ID, serverIdx);
            svc_voip_execute_cmd(cmd);
        }
#if defined(TCSUPPORT_ECN_MEGACO)
        else
        {
            /* Megaco simulate */
            cfg_set_object_attr(INFOVOIPH248_COMMON_NODE, "is_IADSimulate", "1");
            cfg_set_object_attr(INFOVOIPH248_COMMON_NODE, "IAD_testServer", testServer);
            svc_voip_execute_cmd("/usr/bin/killall -9 mgapp");
            svc_voip_execute_cmd("/userfs/bin/mgapp -id 0&");
        }
#endif
    }

    return 0;
}

static int svc_voip_handle_event_simu_complete(char* path)
{
	cwmp_msg_t message;
	long type = 1;	//tr69 must be 1
	int msgFlag = IPC_NOWAIT;

	SVC_VOIP_NOTICE_INFO("send complete message to cwmp.\n");
	memset(&message, 0, sizeof(cwmp_msg_t));
	message.cwmptype = DIAG_COMPLETE;
	sendmegq(type,&message,msgFlag);

	return 0;
}

static int svc_voip_handle_event_iad_complete(char* path, int voipProtocol, int result, int reason)
{
    cwmp_msg_t message;
    long type = 1;	//tr69 must be 1
    int msgFlag = IPC_NOWAIT;

    SVC_VOIP_NOTICE_INFO("voipProtocol = %d, result = %d, reason = %d\n", voipProtocol, result, reason);
    cfg_set_object_attr(path, "RegistResult", itoa(result));
    cfg_set_object_attr(path, "Reason", itoa(reason));
    /*return app to normal state*/
    if(0 == voipProtocol)
        svc_voip_sip_update_handle(VOIPSYSPARAM_NODE);
#if defined(TCSUPPORT_ECN_MEGACO)
    else
        svc_voip_h248_update_handle(VOIPH248_NODE);
#endif

    cfg_set_object_attr(VOIPDIAGNOSTIC_ENTRY_NODE, "IADDiagnosticsState", "Complete");
    SVC_VOIP_NOTICE_INFO("send complete message to cwmp.\n");

    memset(&message, 0, sizeof(cwmp_msg_t));
    message.cwmptype = DIAG_COMPLETE;
    sendmegq(type,&message,msgFlag);

    return 0;
}

static int svc_voip_handle_event_reged(char* path, long currentTime, int line_idx)
{
    char nodeName[64] = {0};
    if((NULL == strstr(path, "entry")) && (NULL == strstr(path, "Entry")))
        return -1;

    SVC_VOIP_DEBUG_INFO("currentTime = %ld, voipPrevRegFlag[%d] = %d, voipPrevCurTime[%d] = %ld\n", currentTime, 
        line_idx, voipRegInfo.voipPrevRegFlag[line_idx], line_idx, voipRegInfo.voipPrevCurTime[line_idx]);

    if(1 == voipRegInfo.voipPrevRegFlag[line_idx])
    {
        voipRegInfo.voipServerDownTime[line_idx] = 0;
        voipRegInfo.voipPrevCurTime[line_idx] = currentTime;
    }
    else
    {
        voipRegInfo.voipPrevRegFlag[line_idx] = 1;
        voipRegInfo.voipServerDownTime[line_idx] += (currentTime - voipRegInfo.voipPrevCurTime[line_idx]); 
        voipRegInfo.voipPrevCurTime[line_idx] = currentTime;
    }
    snprintf(nodeName, sizeof(nodeName), INFOVOIP_ENTRY_NODE, line_idx+1);
    cfg_set_object_attr(nodeName, "VoipPrevRegFlag", itoa(voipRegInfo.voipPrevRegFlag[line_idx]));
    cfg_set_object_attr(nodeName, "VoipPrevCurTime", itoa(voipRegInfo.voipPrevCurTime[line_idx]));
    cfg_set_object_attr(nodeName, "ServerDownTime", itoa(voipRegInfo.voipServerDownTime[line_idx]));

    SVC_VOIP_DEBUG_INFO("VoIPServerDownTime[%d] = %d, voipPrevCurTime[%d] = %ld\n", line_idx,
        voipRegInfo.voipServerDownTime[line_idx], line_idx, voipRegInfo.voipPrevCurTime[line_idx]);

    return 0;
}

static int svc_voip_handle_event_unreged(char* path, long currentTime, int line_idx)
{
    char nodeName[64] = {0};
    if((NULL == strstr(path, "entry")) && (NULL == strstr(path, "Entry")))
        return -1;

    SVC_VOIP_DEBUG_INFO("currentTime = %ld, voipPrevRegFlag[%d] = %d, voipPrevCurTime[%d] = %ld\n", currentTime, 
        line_idx, voipRegInfo.voipPrevRegFlag[line_idx], line_idx, voipRegInfo.voipPrevCurTime[line_idx]);

    if(1 == voipRegInfo.voipPrevRegFlag[line_idx])
    {
        voipRegInfo.voipPrevRegFlag[line_idx] = 0;
        voipRegInfo.voipServerDownTime[line_idx] = 0;
    }
    else
    {
        voipRegInfo.voipServerDownTime[line_idx] += (currentTime - voipRegInfo.voipPrevCurTime[line_idx]);
    }

    voipRegInfo.voipPrevCurTime[line_idx] = currentTime;
    snprintf(nodeName, sizeof(nodeName), INFOVOIP_ENTRY_NODE, line_idx+1);
    cfg_set_object_attr(nodeName, "VoipPrevRegFlag", itoa(voipRegInfo.voipPrevRegFlag[line_idx]));
    cfg_set_object_attr(nodeName, "VoipPrevCurTime", itoa(voipRegInfo.voipPrevCurTime[line_idx]));
    cfg_set_object_attr(nodeName, "ServerDownTime", itoa(voipRegInfo.voipServerDownTime[line_idx]));

    SVC_VOIP_DEBUG_INFO("VoIPServerDownTime[%d] = %d, voipPrevCurTime[%d] = %ld\n", line_idx,
        voipRegInfo.voipServerDownTime[line_idx], line_idx, voipRegInfo.voipPrevCurTime[line_idx]);

    return 0;
}

int voipIdleProcess(char* busy, int voipProtocol)
{	
    SVC_VOIP_DEBUG_INFO("voipProtocol = %d busy = %s\n", voipProtocol, busy);

	if(!strcmp(busy, "0")){
		blapi_perform_update_cpubind_byvoipstate(1);
	}
	else if(!strcmp(busy, "1")){
		blapi_perform_update_cpubind_byvoipstate(0);
	}

    return 0;

}

static int svc_voip_handle_event_line_status_change(char* busy, int voipProtocol)
{
    voipIdleProcess(busy, voipProtocol);
    return 0;
}

static int svc_voip_handle_event_wan_up(char* path, int voipProtocol)
{
    SVC_VOIP_NOTICE_INFO("path = %s voipProtocol = %d\n", path, voipProtocol);

    if(0 == voipProtocol)
        sip_start_upgrade_wan_up(path);
#if defined(TCSUPPORT_ECN_MEGACO)
    else
        megaco_start_upgrade_wan_up(path);
#endif

    return 0;

}

static int svc_voip_handle_event_wan_down(char* path, int voipProtocol)
{
    SVC_VOIP_NOTICE_INFO("path = %s voipProtocol = %d\n", path, voipProtocol);

    if(0 == voipProtocol)
        sip_start_upgrade_wan_down(path);
#if defined(TCSUPPORT_ECN_MEGACO)
    else
    	megaco_start_upgrade_wan_down(path);
#endif

    return 0;
}

int svc_voip_mgr_handle_event(int event, void* evt_buf)
{
    int ret = 0;
    int dbg_level = E_NO_INFO_LEVEL;
    int IADResult = 0;
    int IADReason = 0;
    int voipProtocol = 0;
    char* path = NULL;

    path = ((pt_voip_evt)evt_buf)->buf;
    if(NULL == path)
    {
        ret = -1;
        return ret;
    }

    SVC_VOIP_NOTICE_INFO("event [0x%08x] [%s]\n", event, path);

    switch(event)
    {
        case EVT_CFG_VOIP_SIP_NORMAL_UPDATE:
            ret = svc_voip_sip_normal_handle_event_update(path);
            break;
#if defined(TCSUPPORT_ECN_MEGACO)
        case EVT_CFG_VOIP_H248_NORMAL_UPDATE:
            ret = svc_voip_h248_normal_handle_event_update(path);
            break;
#endif
        case EVT_CFG_VOIP_SIP_SIMULATE_UPDATE:
            ret = svc_voip_sip_simulate_handle_event_update(path);
            break;
#if defined(TCSUPPORT_ECN_MEGACO)
        case EVT_CFG_VOIP_H248_SIMULATE_UPDATE:
            ret = svc_voip_h248_simulate_handle_event_update(path);
            break;
#endif
        case EVT_VOIP_IAD_START:
        {
            IADResult = ((pt_voip_evt)evt_buf)->registResult;
            IADReason = ((pt_voip_evt)evt_buf)->registReason;
            voipProtocol = ((pt_voip_evt)evt_buf)->prevSipStatus;
            ret = svc_voip_handle_event_iad_test(path, voipProtocol, IADReason);
            break;
        }
        case EVT_VOIP_IAD_COMPLETE:
        {
            IADResult = ((pt_voip_evt)evt_buf)->registResult;
            IADReason = ((pt_voip_evt)evt_buf)->registReason;
            voipProtocol = ((pt_voip_evt)evt_buf)->prevSipStatus;
            SVC_VOIP_DEBUG_INFO("IADResult : %d, IADReason: %d\n", IADResult, IADReason);
            ret = svc_voip_handle_event_iad_complete(path, voipProtocol, IADResult, IADReason);   
            break;
        }
        case EVT_VOIP_SIMULATE_COMPLETE:
            ret = svc_voip_handle_event_simu_complete(path);
            break;
        case EVT_VOIP_LINE_STATE:
            ret = svc_voip_handle_event_line_status_change(path, ((pt_voip_evt)evt_buf)->prevSipStatus);
            break;
        case EVT_VOIP_REG_STATE_UNREGED:
            if(0 == ((pt_voip_evt)evt_buf)->prevSipStatus)
                ret = svc_voip_handle_event_unreged(path, ((pt_voip_evt)evt_buf)->currentTime, ((pt_voip_evt)evt_buf)->voipLineIdx);
            break;
        case EVT_VOIP_REG_STATE_REGED:
            if(0 == ((pt_voip_evt)evt_buf)->prevSipStatus)
                ret = svc_voip_handle_event_reged(path, ((pt_voip_evt)evt_buf)->currentTime, ((pt_voip_evt)evt_buf)->voipLineIdx);
            break;
        case EVT_VOIP_SRV_DEBUG_LEVEL:
            dbg_level = atoi(((pt_voip_evt)evt_buf)->buf);
            svc_voip_debug_level(dbg_level);
            break;
        default:
            SVC_VOIP_WARN_INFO("path[%s]  event [%d]\n", path, event);
            break;
    }

    return ret;
}

void svc_voip_mgr_handle_boot2_ready(void){
	int i=0;
	wan_evt_t tmp = {0};
	
	SVC_VOIP_NOTICE_INFO("handle boot2 ready\n");
	for(i=0; i<MAX_VOIP_WAN; i++){
		if(0 != VoIP_Wan_info_array[i].event){
			strncpy(tmp.buf, VoIP_Wan_info_array[i].evt_buf, sizeof(tmp.buf)-1);
			SVC_VOIP_DEBUG_INFO("boot2 send message to load voip:%s\n", tmp.buf);
			svc_voip_mgr_handle_wan_event(VoIP_Wan_info_array[i].event, (void*)(&tmp));
			memset(VoIP_Wan_info_array+i, 0, sizeof(VoIP_Wan_info_t));
		}	
	}
}

int svc_voip_mgr_handle_wan_event(int event, void* evt_buf)
{
    int ret = 0;
    int voipProtocol = 0;
    char tmp[32] = {0};
    char* path = NULL;
    char str_servicelist[32] = {0};
    char usedPath[256] = {0};

    int i = 0;

    path = ((pt_wan_evt)evt_buf)->buf;
    if(NULL == path)
    {
        ret = -1;
        return ret;
    }

    SVC_VOIP_NOTICE_INFO("event [0x%08x] [%s]  \n", event, path);
	cfg_get_object_attr(INFOVOIP_COMMON_NODE, "boot2VoIPReady", tmp, sizeof(tmp)) ;
	if(strcmp(tmp, "Yes") != 0){
		if(cfg_get_object_attr(path, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
			NULL != strstr(str_servicelist, "VOICE")){
			if(EVT_WAN_IPV4_UP == event || EVT_WAN_IPV6_UP == event){
				for(i=0; i<MAX_VOIP_WAN; i++){
					if(0 == VoIP_Wan_info_array[i].event){
						VoIP_Wan_info_array[i].event = event;
						strncpy(VoIP_Wan_info_array[i].evt_buf, path, sizeof(VoIP_Wan_info_array[i].evt_buf)-1);
						break;
					}
				}
				SVC_VOIP_DEBUG_INFO("VoIP Wan number max:%d, i:%d\n", MAX_VOIP_WAN, i);
			}
			cfg_set_object_attr("root.InfoVoIP.Common", "boot2delay", "Yes");
		}
		if(EVT_WAN_IPV4_DOWN == event || EVT_WAN_IPV6_DOWN == event){
			for(i=0; i<MAX_VOIP_WAN; i++){
				if(!strcmp(VoIP_Wan_info_array[i].evt_buf, path)){
					memset(VoIP_Wan_info_array+i, 0, sizeof(VoIP_Wan_info_t));
					break;
				}
			}
		}
		return -1;
	}
		
	if(cfg_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", tmp, sizeof(tmp)) < 0){
		return -1;
	}

    voipProtocol = atoi(tmp);

    switch(event)
    {
        case EVT_WAN_XPON_UP :
            cfg_get_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_PATH_USED", usedPath, sizeof(usedPath));
            if(strlen(usedPath)){
                cfg_get_object_attr(usedPath, "ISP", tmp, sizeof(tmp));
                if(!strcmp(tmp, "1"))/*static wan*/
                    ret = svc_voip_handle_event_wan_up(usedPath, voipProtocol);
            }
            break;
        case EVT_WAN_IPV4_UP :
        case EVT_WAN_IPV6_UP :
            ret = svc_voip_handle_event_wan_up(path, voipProtocol); 
            break;
        case EVT_WAN_XPON_DOWN :
        case EVT_WAN_IPV4_DOWN :
        case EVT_WAN_IPV6_DOWN :
            ret = svc_voip_handle_event_wan_down(path, voipProtocol);
            break;
        default:
            SVC_VOIP_WARN_INFO("path[%s]  event [%d]\n", path, event);
            break;
    }

    return ret;

}


