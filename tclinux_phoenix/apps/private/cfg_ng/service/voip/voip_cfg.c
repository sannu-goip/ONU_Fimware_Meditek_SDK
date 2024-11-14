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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <unistd.h>

#include "cfg_cli.h"
#include "../wan/msg_notify.h"

#include "voip_common.h"
#include "voip_mgr.h"
#include "utility.h" 


int line_error_restartsip_timer_cnt[MAX_DSP_LINE] = {0};
#if defined(TCSUPPORT_CMCC)
int loadVoip = 0;
#else
int loadVoip = 1;
#endif

int voice_wan_path_record[MAX_WAN_IF_INDEX] = {0};

int get_wanindex_by_path(char *wanPath)
{
	int pvc_index = -1, entry_index = -1;
	char* pvcStr = NULL;

	if(wanPath)
	{
		cfg_str_to_lower(wanPath);
		if(strstr(wanPath, "root.wan.") && NULL != (pvcStr = strstr(wanPath, "pvc.")) && 
			strstr(wanPath, "entry."))
		{
			if(-1!=sscanf(pvcStr, "pvc.%d.entry.%d", &pvc_index, &entry_index))
			{
				SVC_VOIP_DEBUG_INFO("pvcStr = %s, pvc_index = %d, entry_index = %d\n", pvcStr, pvc_index, entry_index);	
				return (pvc_index - 1)*MAX_SMUX_NUM + (entry_index -1);
			}
		}
	}

	return -1;
}

#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
int setVoIPRtpBindWanIP(int voipProtocol, int if_index)
{
	char tmp[32] = {0};

	SVC_VOIP_DEBUG_INFO("if_index = %d\n", if_index);
	memset(tmp, 0x0, sizeof(tmp));
	if((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "RtpBindIFIndex", tmp, sizeof(tmp)) < 0)
		||(0 == strlen(tmp)) ||(if_index != atoi(tmp)))
		return -1;

	if(0 == voipProtocol)
	{
		/*reload sip*/
		svc_voip_sip_normal_handle_event_update(VOIPADVANCED_NODE);
	}
#if defined(TCSUPPORT_ECN_MEGACO)
	else
	{
		/*reload H.248*/
		svc_voip_h248_normal_handle_event_update(VOIPH248_NODE);
	}
#endif

	return 0;
}
#endif


#if defined(TCSUPPORT_ECN_MEGACO)
int setVoIPWanIP(int if_index,int if_state)
{
	char wanInfoNode[64] = {0};
	char nodeName[64] = {0};
	char tempip[64] = {0};
	char ip[64] = {0};
	char IP_Protocol[8] = {0};
	char Wan_IP_Protocol[8] = "IP";

#ifdef TCSUPPORT_IPV6
	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
		strcpy(IP_Protocol, "IPV4");
	}
	if(!strcmp(IP_Protocol, "IPV6"))
		strcpy(Wan_IP_Protocol, "IP6");
#endif
	snprintf(wanInfoNode, sizeof(wanInfoNode), WANINFO_ENTRY_NODE, if_index+1);

	SVC_VOIP_DEBUG_INFO("wanInfoNode=%s, if_state=%d\n", wanInfoNode, if_state);

	if((cfg_get_object_attr(wanInfoNode, Wan_IP_Protocol, tempip, sizeof(tempip)) < 0)||(if_state == 0) || !strcmp(tempip,"0.0.0.0")|| !strcmp(tempip,"::")|| (0 == strlen(tempip)))
	{
		SVC_VOIP_DEBUG_INFO("tempip=%s\n", tempip);
		/*get wan ip fail, set with lan ip*/
		snprintf(nodeName, sizeof(nodeName), LAN_ENTRY0_NODE);
#ifdef TCSUPPORT_IPV6
		if(!strcmp(IP_Protocol, "IPV6")){		
			if(cfg_get_object_attr(nodeName, "IP6", ip, sizeof(ip)) < 0)
			{
				/*get lan ip6 fail*/
				strncpy(ip, "::FFFF:192.168.1.1", sizeof(ip) - 1);
			}
		}else
#endif
		{
			if(cfg_get_object_attr(nodeName, "IP", ip, sizeof(ip)) < 0)
			{
				/*get lan ip fail*/
				strncpy(ip, "192.168.1.1", sizeof(ip) - 1);
			}
		}
	}
	else
	{
		SVC_VOIP_DEBUG_INFO("tempip=%s\n", tempip);
		/*get wan ip ok, set with wan ip*/
		strncpy(ip, tempip, sizeof(ip) - 1);
	}

	SVC_VOIP_DEBUG_INFO("setVoIPWanIP:ip=%s\n",ip);

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, INFOVOIPH248_COMMON_NODE, sizeof(nodeName) - 1);
	cfg_set_object_attr(nodeName, "IP", ip);

	return 0;
}

#define VOIP_PID_PATH "/tmp/tcVoIPApiServer.pid"
void _mgappRestart(void)
{
	FILE *fp = NULL;
	char cmdbuf[128] = {0};
	char voip_pid_tmp[100] = {0};
	char FlagFwUp_s[20] = {0};
	char nodeName[64] = {0};

	strncpy(nodeName,SYSTEM_ENTRY_NODE, sizeof(nodeName) - 1);
	if(cfg_get_object_attr(nodeName, "upgrade_status", FlagFwUp_s, sizeof(FlagFwUp_s)) < 0)
		return;

	fp = fopen(VOIP_PID_PATH, "r");
	if(fp == NULL)
	{
		/*get tcVoIPApiServer process pid*/
		snprintf(cmdbuf, sizeof(cmdbuf), "pidof %s > %s", "tcVoIPApiServer", VOIP_PID_PATH);
		svc_voip_execute_cmd(cmdbuf);  
		fp = fopen(VOIP_PID_PATH, "r");
	}
	
	if(fp == NULL){
		return;
	}

	fgets(voip_pid_tmp, sizeof(voip_pid_tmp), fp);
	fclose(fp);
	unlink(VOIP_PID_PATH);
	SVC_VOIP_DEBUG_INFO("\n_mgappRestart, tcVoIPApiServer pid :%s, upgrade_status:%s\n", voip_pid_tmp, FlagFwUp_s);
	if((atoi(voip_pid_tmp) != 0)&&(0 != strcmp(FlagFwUp_s,"true"))){  /*if voip_pid_tmp != 0, that's mean need restart mgappt*/
		SVC_VOIP_TRACE_INFO("mgappRestart restart..\n");
		svc_voip_execute_cmd("killall -9 mgapp");
		svc_voip_execute_cmd("/userfs/bin/mgapp -id 0 &");
	}

	return;
}

void mgappKeepAlive(void)
{
	char nodeName[64]={0};
	char reset[8]={0};
	char FlagFwUp_s[20] = {0};

	strncpy(nodeName, SYSTEM_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_get_object_attr(nodeName, "upgrade_status", FlagFwUp_s, sizeof(FlagFwUp_s));

	memset(nodeName, 0x0, sizeof(nodeName));
	strncpy(nodeName,INFOVOIPH248_COMMON_NODE, sizeof(nodeName) - 1);
	if(cfg_get_object_attr(nodeName, "ResetFlag", reset, sizeof(reset)) < 0)
		return;

	SVC_VOIP_DEBUG_INFO("FlagFwUp_s = %s, reset = %s\n", FlagFwUp_s, reset);
	if((strcmp(reset, "1")==0) && (0 != strcmp(FlagFwUp_s, "true")))
	{
		system("killall -9 mgapp");
		system("/userfs/bin/mgapp -id 0 &");
		cfg_set_object_attr(nodeName, "ResetFlag", "0");
	}

	return;

}

void restart_mgapp_when_down(void)
{
	FILE *fp = NULL;
	char mgapp_pid_tmp[100] = {0};
	char FlagFwUp_s[20] = {0};
	int  status = 0;
	int  ret = 0;
	char pidPath[] = "/tmp/mgapp.pid";
	static int mgappDownCheckCnt = 0;
	int  mgappDownCheckCycle = 5;

	/* each 5s run */
	if (mgappDownCheckCycle != mgappDownCheckCnt){
		mgappDownCheckCnt++;
		return ret;
	}

	mgappDownCheckCnt = 0;
	fp = fopen(pidPath,"r");
	if(fp == NULL){
		/*get mgapp process pid*/
		status = system("pidof mgapp > /tmp/mgapp.pid");
		if (-1 == status || 0 == WIFEXITED(status)){
			/* system or run shell fail */
			SVC_VOIP_WARN_INFO("\nrestart_mgapp_when_down:xxxx system run fail, \
				status:%d, WIFEXITED:%d, WEXITSTATUS:%d !!\n", status, 
				WIFEXITED(status), WEXITSTATUS(status));

			unlink(pidPath);
			return ret;
		}
		fp = fopen(pidPath, "r");
	}
	if(fp == NULL){
		return ret;
	}
	fgets(mgapp_pid_tmp, sizeof(mgapp_pid_tmp), fp);
	fclose(fp);
	unlink(pidPath);
	cfg_get_object_attr(SYSTEM_ENTRY_NODE, "upgrade_status", FlagFwUp_s, sizeof(FlagFwUp_s));
	/*if mgapp_pid_tmp == 0 && not upgrade firmware, that's mean need restart mgapp*/
	if((atoi(mgapp_pid_tmp) == 0)&&(0 != strcmp(FlagFwUp_s,"true"))){  
		SVC_VOIP_TRACE_INFO("\nrestart mgapp when mgapp is down!!\n");
		system("killall -9 mgapp");
		system("/userfs/bin/mgapp -id 0 &");
		ret = 1;
	}
	return ret;
}

int check_mgw_active_state(void)
{
	int idx = 0;
	int ret = 0;
	int voipLineNumber = svc_voip_update_line_num();
	char nodeName[32] = {0};
	char tmp[8] = {0};

	for(idx = 0; idx < voipLineNumber; idx++)
	{
		memset(nodeName, 0x0, sizeof(nodeName));
		memset(tmp, 0x0, sizeof(tmp));

		snprintf(nodeName, sizeof(nodeName), INFOVOIPH248_ENTRY_NODE, idx+1);
		cfg_get_object_attr(nodeName,"ServiceChangeState", tmp, sizeof(tmp));

		if(strcmp(tmp, "8"))/*8: disabled state*/
			break;
	}

	if((idx > 0) && (idx == voipLineNumber))
	{
		SVC_VOIP_DEBUG_INFO("reload mgapp to catch the register chance when MGW become active again.\n");
		ret=voipCmdSend("callCmd {test.reload}");
		if(-1 == ret)
			goto restart_mgapp;
		else
			return ret;
	}
	else
		return ret;

restart_mgapp:
	_mgappRestart();
	return 0;
}

int megaco_start_upgrade_wan_up(char* path)
{
    char if_name[16] = {0};
    char InfoVoIPNode[64] = {0};
    char str_servicelist[32] = {0};
    char wanPath_used[32] = {0};
    int  if_index = 0;
    int  ret=0;
#if defined(TCSUPPORT_CT_JOYME2)
	char loadFlag[4] = {0};
#endif

    strncpy(InfoVoIPNode, INFOVOIPH248_COMMON_NODE, sizeof(InfoVoIPNode) - 1);
    cfg_get_object_attr(InfoVoIPNode, "WAN_PATH_USED", wanPath_used, sizeof(wanPath_used));

    if(cfg_get_object_attr(path, "IFName", if_name, sizeof(if_name)) < 0)
    {
        SVC_VOIP_WARN_INFO("Can't get wan interface name from %s.\n", path);
        return -1;
    }

    SVC_VOIP_DEBUG_INFO("path = %s, if_name = %s\n", path, if_name);
    if((if_index = get_wanindex_by_name(if_name)) < 0)
    {
        SVC_VOIP_WARN_INFO("Can't get wan interface index by interface name, if_index=%d, ifName=%s.\n", if_index, if_name);
        return -1;
    }

    SVC_VOIP_DEBUG_INFO("if_index = %d\n", if_index);
#if defined(TCSUPPORT_CT_JOYME2)
	cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", loadFlag, sizeof(loadFlag));
    if(!strcmp(loadFlag, "1"))
#else
	if(1 == loadVoip)
#endif
    {
    setVoIPRtpBindWanIP(1, if_index);
    }

    if(0 == strcmp(wanPath_used, path))
        return 0;

    if(cfg_get_object_attr(path, "ServiceList", str_servicelist, sizeof(str_servicelist)) < 0 
        || NULL == strstr(str_servicelist, "VOICE"))
        return -1; /*not voip wan*/
	
#if defined(TCSUPPORT_CT_JOYME2)	
	if(strcmp(loadFlag, "1"))
#else
	if(0 == loadVoip)
#endif

    {
#if defined(TCSUPPORT_CT_JOYME2)
		cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", "1");
#else
		loadVoip = 1;
#endif
        svc_voip_execute_cmd("/usr/script/voip_load.sh &");
        sleep(10);
    }

    setVoIPWanIP(if_index, 1);
    cfg_set_object_attr(InfoVoIPNode, "bindIf", if_name);
    cfg_set_object_attr(InfoVoIPNode, "WAN_PATH_USED", path);
    SVC_VOIP_DEBUG_INFO("if_name = %s\n", if_name);	
    ret=voipCmdSend("callCmd {test.reload}");
    if(-1 == ret)
        goto restart_mgapp;
    else
        return ret;

restart_mgapp:
    _mgappRestart();
    return 0;

}

int megaco_start_upgrade_wan_down(char* path)
{
	int voipLineNumber = 1;
	int lineIdx = 0;
	int ret=0;
	int if_index = -1;

	char wanUpPath[32] = {0};
	char InfoVoIPH248[64] = {0};
	char nodeName[32] = {0};
	char ip[64] = {0};
	char IP_Protocol[8] = {0};

	cfg_get_object_attr(INFOVOIPH248_COMMON_NODE, "WAN_PATH_USED", wanUpPath, sizeof(wanUpPath));
#ifdef TCSUPPORT_IPV6
	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
		strcpy(IP_Protocol, "IPV4");
	}
#endif

	if_index = get_wanindex_by_path(path);
	SVC_VOIP_DEBUG_INFO("if_index = %d\n", if_index);
	setVoIPRtpBindWanIP(1, if_index);

	SVC_VOIP_DEBUG_INFO("wanUpPath = %s\n", wanUpPath);

	if(path &&(0 == strcasecmp(path, wanUpPath)))
	{
		snprintf(nodeName, sizeof(nodeName), LAN_ENTRY0_NODE);
#ifdef TCSUPPORT_IPV6
		if(!strcmp(IP_Protocol, "IPV6")){
			if(cfg_get_object_attr(nodeName, "IP6", ip, sizeof(ip)) < 0)
			{
				/*get lan ip fail*/
				strncpy(ip,"::FFFF:192.168.1.1", sizeof(ip) - 1);
			}
		}else
#endif
		{
			if(cfg_get_object_attr(nodeName, "IP", ip, sizeof(ip)) < 0)
			{
				/*get lan ip fail*/
				strncpy(ip,"192.168.1.1", sizeof(ip) - 1);
			}	
		}

		voipLineNumber = svc_voip_update_line_num();
		for(lineIdx = 1; lineIdx <= voipLineNumber; lineIdx++)
		{
			memset(InfoVoIPH248, 0, sizeof(InfoVoIPH248));
			snprintf(InfoVoIPH248, sizeof(InfoVoIPH248), INFOVOIPH248_ENTRY_NODE, lineIdx);
#if !defined(TCSUPPORT_CMCC)
			cfg_set_object_attr(InfoVoIPH248, "RegFailReason", VOICE_NO_WAN_ERROR);
#endif
		}
		memset(InfoVoIPH248, 0, sizeof(InfoVoIPH248));
		strncpy(InfoVoIPH248, INFOVOIPH248_COMMON_NODE, sizeof(InfoVoIPH248) - 1);
		cfg_set_object_attr(InfoVoIPH248, "IP", ip);
		cfg_set_object_attr(InfoVoIPH248, "bindIf", "");
		cfg_set_object_attr(InfoVoIPH248, "WAN_PATH_USED", "");
		ret=voipCmdSend("callCmd {test.wanDown}");
		if(ret==-1){
			goto restart_mgapp;
		}

	}

	return 0;

restart_mgapp:
	_mgappRestart();
	return 0;

}

#endif


#define SIP_PID_FILE "/var/run/voip_sip.pid"
int chkSipProcess(void)
{
	FILE *fp = NULL;
	char pid_buf[32] = {0};
	char cmd_line_path[64] = {0};
	char cmd_line[128] = {0};
	char *cmd_chk = NULL;
	int status = 0;
	int pid = 0;
	char FlagFwUp_s[20] = {0};
	int count = 0;

	if(NULL == (fp = fopen(SIP_PID_FILE, "r")))
	{
		/* tcdbg_printf("\ncan't open file %s.\n", SIP_PID_FILE); */
		return -1;
	}

	count = fread(pid_buf, sizeof(char), sizeof(pid_buf) - 1, fp);
	fclose(fp);

	pid = atoi(pid_buf);
	if ( pid > 0 )
	{
		snprintf(cmd_line_path, sizeof(cmd_line_path), "/proc/%d/cmdline", pid);
		fp = fopen(cmd_line_path, "r");
		if(fp)
		{
			count = fread(cmd_line, sizeof(char), sizeof(cmd_line) - 1, fp);
			fclose(fp);
		}
		cmd_line[sizeof(cmd_line) - 1] = '\0';
		cmd_chk = strstr(cmd_line, "sip");		
		if ( cmd_chk )
			status = 1;
	}
	else
	{
		/* tcdbg_printf("\nwrong pid.\n"); */
		return -1;
	}

	cfg_get_object_attr(SYSTEM_ENTRY_NODE, "upgrade_status", FlagFwUp_s, sizeof(FlagFwUp_s));
	/*if sip_pid_tmp == 0 && not upgrade firmware, that's mean need restart sipclient*/
	if ((0 == status)&&(0 != strcmp(FlagFwUp_s,"true")))
	{
		tcdbg_printf("\nrestart sip app for voip.\n");

		system("/usr/bin/killall -9 sipclient");
		unlink(SIP_PID_FILE);
		system("/userfs/bin/sipclient &");
	}

	return 0;

}
/*______________________________________________________________________________
**function :restart_sip_when_sip_down
**
**description:
*    check sipclient process every second, when sipclient crash, restart it
**parameters:
*    none
**global:
*    sipStatusFlag
**return: 1-restart sipclient; 0-other
*    
*    
**call:
*    none
**revision:
*  
**____________________________________________________________________________*/

int restart_sip_when_sip_down()
{
	static int sipDownCheckCnt = 0;
	int  SIPDownCheckCycle = 5;

	/* each 5s run */
	if (SIPDownCheckCycle != sipDownCheckCnt){
		sipDownCheckCnt++;
		return 0;
	}

	sipDownCheckCnt = 0;
	chkSipProcess();

	return 0;
}

#if defined(TCSUPPORT_SDN_OVS)
void voip_netns_start_cc(char *cmd){
	char tmp[32] = {0};
	char cmdbuf[128]= {0};

	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "netns", tmp,sizeof(tmp)) && strlen(tmp))
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/ip netns exec %s %s", tmp, cmd);
		system_escape(cmdbuf);
		system("pidof sipclient > /proc/ksocket_ns");  
	}
	else{
		system_escape(cmd);
	}

	SVC_VOIP_DEBUG_INFO("voip_netns_start_cc: voip cmd:%s %s!!\r\n", cmdbuf, cmd);
}
#endif

/*______________________________________________________________________________
**function :restart_sip_when_line_unreg
**
**description:
*    check line status every second, when wan up ,line enable and line unreg for longer time than rereg timer, restart sipclient
**parameters:
*    none
**global:
*    sipStatusFlag, line_error_restartsip_timer_cnt
**return:
*    
*    
**call:
*    none
**revision:
*  
**____________________________________________________________________________*/

void restart_sip_when_line_unreg()
{
	char nodeName[128] = {0};
	char lineEnable[32] = {0};
	char strRegStatus[32] = {0};
	char regRetryTimestr[32] = {0};
	char FlagFwUp_s[20] = {0};
	int j = 0;
	int regRetryTime = 0;
	int line_no_unreg_cnt=0;/*count for unregistered line num*/
	int active_line_num=0;
	int voipLineNumber = svc_voip_update_line_num();

	cfg_get_object_attr(SYSTEM_ENTRY_NODE, "upgrade_status", FlagFwUp_s, sizeof(FlagFwUp_s));
	cfg_get_object_attr(VOIPADVANCED_COMMON_NODE,"RegisterRetryInterval", regRetryTimestr, sizeof(regRetryTimestr));
	regRetryTime = atoi(regRetryTimestr);
	regRetryTime = regRetryTime + 30;/*when retry register, it will take about 20s*/

	for(j = 0; j < voipLineNumber; j++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VOIPBASIC_ENTRY_NODE, j+1);
		/*make sure line is enable*/
		if(cfg_get_object_attr(nodeName, "Enable", lineEnable, sizeof(lineEnable)) >= 0 && (strcmp(lineEnable, "Yes") == 0))
		{
			active_line_num++;
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VOIPSYSPARAM_ENTRY_NODE, j+1);
			/*check whether the  line status is unregistered*/
			if(cfg_get_object_attr(nodeName, "SC_ACCT_INFO_REG_STATUS", strRegStatus, sizeof(strRegStatus)) >= 0
				&& (strcmp(strRegStatus,"Unregistered") == 0))
			{
				line_no_unreg_cnt++;/*line_no_unreg_cnt count unregister line num*/
				if(line_error_restartsip_timer_cnt[j] <= regRetryTime)
				{
					line_error_restartsip_timer_cnt[j]++;
				}
				else
				{
					if(0 != strcmp(FlagFwUp_s,"true") && line_no_unreg_cnt==active_line_num)
					{
						/*all line unregistered*/
						SVC_VOIP_TRACE_INFO("\nrestart sipclient for line error!!\n");
						system("killall -9 sipclient");
#if defined(TCSUPPORT_SDN_OVS)
						voip_netns_start_cc("/userfs/bin/sipclient &");
#else 
					    system("/userfs/bin/sipclient &");
#endif
						memset(line_error_restartsip_timer_cnt, 0, MAX_DSP_LINE*sizeof(int));
						break;
					}
				}

			}
		}
	}
	if(line_no_unreg_cnt != active_line_num)
	{
		/*there are some line not unregistered status*/
		memset(line_error_restartsip_timer_cnt, 0, MAX_DSP_LINE*sizeof(int));
	}
	return;
}



int voipStatUpgrade(void)
{
	char nodeName[64] = {0};
	char szCurrIP[128] = {0};
	char lanIP[64] = {0};
	static char wanIP[128] = {0};
	static char wanIFName[16] = {0};
	char scStatus[8] = {0};
	char lanEnable[8] = {0};
	int  voipLineNumber = 1;
	static int prevLanBind = 0;
	int  j = 0;
	char IP_Protocol[8] = {0};

#if !defined(TCSUPPORT_SDN_OVS)
#ifdef TCSUPPORT_IPV6
	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
		strcpy(IP_Protocol, "IPV4");
	}
	if(!strcmp(IP_Protocol, "IPV6")){
		if(cfg_get_object_attr(LAN_ENTRY0_NODE,"IP6", lanIP, sizeof(lanIP)) < 0)
		 return -1;
	}else
#endif
	{
		if(cfg_get_object_attr(LAN_ENTRY0_NODE,"IP", lanIP, sizeof(lanIP)) < 0)
		 	return -1;
	}
#endif

	if(cfg_get_object_attr(VOIPSYSPARAM_COMMON_NODE,"SC_SYS_CFG_WAN_IP", szCurrIP, sizeof(szCurrIP)) < 0)
		return -1;

	if(cfg_get_object_attr(INFOVOIP_COMMON_NODE, "SC_START_STATUS", scStatus, sizeof(scStatus)) < 0)
		return -1;

	/*Lan Binding*/
	if((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "LanBindEnable", lanEnable, sizeof(lanEnable)) >= 0) && (!strcmp(lanEnable, "1"))) 
	{
		if ((0 != strcmp(lanIP, szCurrIP)) && (0 == strcmp(scStatus, "Yes"))) 
		{
			prevLanBind = 1;
			/*wan->lan*/
			/*SVC_VOIP_DEBUG_INFO(" lan side binding, Lan IP: %s\n", lanIP);*/
			strncpy(wanIP, szCurrIP, sizeof(wanIP) - 1);
			cfg_get_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", wanIFName, sizeof(wanIFName));
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", lanIP);
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", "br0");

			voipLineNumber = svc_voip_update_line_num();
			for(j = 1; j <= voipLineNumber; j++)
			{
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), INFOVOIP_ENTRY_NODE, j);

				if(strlen(lanIP)!=0)
				    cfg_set_object_attr(nodeName, "WAN_STATUS_CHANGE_FLAG", "up");/*always directly send message in Lan side*/
				else
				    cfg_set_object_attr(nodeName, "WAN_STATUS_CHANGE_FLAG", "");
			}
			svc_voip_sip_normal_handle_event_update(VOIPSYSPARAM_NODE);
			return 0;
		}
	}
	else if(prevLanBind)
	{
		SVC_VOIP_DEBUG_INFO("lan to wan, Wan IP: %s, wan interface name: %s\n", wanIP, wanIFName);
		cfg_set_object_attr(nodeName,"WAN_STATUS_CHANGE_FLAG","down_up");
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", wanIP);
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", wanIFName);
		svc_voip_sip_normal_handle_event_update(VOIPSYSPARAM_NODE);
		prevLanBind = 0;
	}
	return 0;
}

/*______________________________________________________________________________
**function :update_sip_reg_param
**
**description:
*    update wan ip and interface name attributes for VoIPSysParam_Common node
**
**parameters:
*    lineNum: voip line number
*    if_index_used, 1: use wan bind index, 0: no wan bind index is used
*    ifName: interface name for link used
*    wanBindChange: whether change from one wan bind to another wan bind
  
**return:

**____________________________________________________________________________*/

void update_sip_reg_param(int lineNum, int if_index_used, char* ifName, char* wanPath, int wanBindChange)
{
	char nodeName[32] = {0};
	char str_IP[128] = {0};
	char IP_Protocol[8] = {0};

	int i = 0;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, if_index_used+1);
	
#ifdef TCSUPPORT_IPV6
		if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
			strcpy(IP_Protocol, "IPV4");
		}
		if(!strcmp(IP_Protocol, "IPV6")){
			if (cfg_get_object_attr(nodeName, "IP6", str_IP, sizeof(str_IP))< 0)
			{
				SVC_VOIP_WARN_INFO("Can't get attribute IP6 from node %s.\n", nodeName);
			}
		}
		else
#endif
		{
			if (cfg_get_object_attr(nodeName, "IP", str_IP, sizeof(str_IP))< 0)
			{
				SVC_VOIP_WARN_INFO("Can't get attribute IP from node %s.\n", nodeName);
			}
		}
	SVC_VOIP_DEBUG_INFO("str_IP = %s, ifName = %s, wanBindChange = %d\n", str_IP, ifName, wanBindChange);

	for(i = 1; i <= lineNum; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), INFOVOIP_ENTRY_NODE, i);
		if(wanBindChange)
		{/*give up this wan bind, prepare to change to another wan bind*/
			if(strlen(str_IP)>0)
			{
				cfg_set_object_attr(nodeName,"WAN_STATUS_CHANGE_FLAG","up");
			}
			else
			{
				cfg_set_object_attr(nodeName,"WAN_STATUS_CHANGE_FLAG","down");
			}

		}
		else
		{
			cfg_set_object_attr(nodeName,"WAN_STATUS_CHANGE_FLAG","down_up");
		}
	}

	if(strlen(str_IP)>0)
	{
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", str_IP);
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", ifName);
		svc_voip_sip_update_handle(VOIPSYSPARAM_NODE);
		
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFINDEX_USED", itoa(if_index_used));
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_PATH_USED", wanPath);
	}
	else
	{
#ifdef TCSUPPORT_IPV6
		if(!strcmp(IP_Protocol, "IPV6"))
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", "::");
		else
#endif
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", "0.0.0.0");
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", "");
		svc_voip_sip_update_handle(VOIPSYSPARAM_NODE);

		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFINDEX_USED", "");
		cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_PATH_USED", "");
	}

	return;
}

void get_reg_param_from_wan_info(char* wanPath, int bind_if_index, int lineNum, int useCurIfIndex)
{
	char wanNodeName[32] = {0};
	char str_servicelist[32] = {0};
	char str_wanmode[8] = {0};
	char ifName[32] = {0};

	int pvc_index = -1;
	int entry_index = -1;
	int up_if_index = -1;

	cfg_get_object_attr(wanPath, "IFName", ifName, sizeof(ifName));
	if((up_if_index = get_wanindex_by_name(ifName)) < 0)
	{
		SVC_VOIP_WARN_INFO("Can't get wan interface index by interface name, up_if_index=%d, ifName=%s.\n", up_if_index, ifName);
		return;
	}
	setVoIPRtpBindWanIP(0, up_if_index);

	if(1 == useCurIfIndex)/*wan binded*/
	{
		pvc_index = bind_if_index / MAX_SMUX_NUM;
		entry_index = bind_if_index % MAX_SMUX_NUM;
		SVC_VOIP_DEBUG_INFO("pvc_index = %d, entry_index = %d\n", pvc_index, entry_index);
		memset(wanNodeName, 0, sizeof(wanNodeName));
		snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);

		if(cfg_get_object_attr(wanNodeName, "Active", str_wanmode, sizeof(str_wanmode))>= 0 &&
			0 == strcmp(str_wanmode, "Yes") &&
			cfg_get_object_attr(wanNodeName, "WanMode", str_wanmode, sizeof(str_wanmode))>= 0 &&
			0 == strcmp(str_wanmode, "Route") &&
			cfg_get_object_attr(wanPath, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
			NULL != strstr(str_servicelist, "VOICE"))/*wan bind valid*/
		{
			SVC_VOIP_DEBUG_INFO("up_if_index = %d, bind_if_index = %d\n", up_if_index, bind_if_index);		
			if(up_if_index == bind_if_index)
			{
				update_sip_reg_param(lineNum, up_if_index, ifName, wanPath, 0);
			}
			else
			{
				SVC_VOIP_NOTICE_INFO("Bind ifindex is not equal to wan up index.\n");
				return;
			}
			
		}
		else if(cfg_get_object_attr(wanPath, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
			NULL != strstr(str_servicelist, "VOICE"))/*wan bind is invalid*/
		{
			update_sip_reg_param(lineNum, up_if_index, ifName, wanPath, 0);

			cfg_set_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", itoa(up_if_index));
		}

	}
	else/*no wan binded*/
	{
		if(cfg_get_object_attr(wanPath, "Active", str_wanmode, sizeof(str_wanmode))>= 0 &&
			0 == strcmp(str_wanmode, "Yes") &&
			cfg_get_object_attr(wanPath, "WanMode", str_wanmode, sizeof(str_wanmode))>= 0 &&
			0 == strcmp(str_wanmode, "Route") &&
			cfg_get_object_attr(wanPath, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
			NULL != strstr(str_servicelist, "VOICE"))
		{
			update_sip_reg_param(lineNum, up_if_index, ifName, wanPath, 0);

			cfg_set_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", itoa(up_if_index));
		}
	}

	return;
}


int sip_start_upgrade_wan_up(char* path)
{
    char nodeName[32] = {0};
    char tmp[64] = {0};
    char curIfIndex[8] = {0};

    int wan_if_index = -1;
    int bind_if_index = 0;
    int useCurIfIndex = 0;
    int lineNum = 0;
    char str_servicelist[32] = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	char loadFlag[4] = {0};
#endif

    lineNum = svc_voip_update_line_num();

    if(cfg_get_object_attr(path, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
         NULL != strstr(str_servicelist, "VOICE"))
    {
        wan_if_index = get_wanindex_by_path(path);
        if(wan_if_index >= 0)
        {
            SVC_VOIP_DEBUG_INFO("wan_if_index = %d\n", wan_if_index);
            voice_wan_path_record[wan_if_index] = 1;
        }
    }

    strncpy(nodeName, VOIPADVANCED_COMMON_NODE,sizeof(nodeName) - 1);

    /*Get wan bind index*/
    if((cfg_get_object_attr(nodeName, "LanBindEnable", tmp, sizeof(tmp))>=0)&& (!strcmp(tmp, "1")))
    {
        SVC_VOIP_DEBUG_INFO("Lan binded.\n");
        return 0;
    }

    /*Get wan bind index in VoIP app basic set web*/
    if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", curIfIndex, sizeof(curIfIndex)) < 0)
    {
        SVC_VOIP_WARN_INFO("Can't get CurIFIndex attribute from root.VoIPAdvanced.Common.\n");
    }

#if defined(TCSUPPORT_CT_JOYME2)	
	cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", loadFlag, sizeof(loadFlag));
    if(strcmp(loadFlag, "1"))
#else
	if(0 == loadVoip)
#endif
    {
        if(cfg_get_object_attr(path, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
	        NULL != strstr(str_servicelist, "VOICE"))
        {
#if defined(TCSUPPORT_CT_JOYME2)
			cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", "1");
#else
			loadVoip = 1;
#endif
            svc_voip_execute_cmd("/usr/script/voip_load.sh &");
            sleep(10);
        }
    }

    if(isNumber(curIfIndex))/*Conditions:1, the node is not NULL; 2, the node is all digit number value;*/
    {
        /*wan binded*/
        bind_if_index = atoi(curIfIndex);
        useCurIfIndex = 1;
        get_reg_param_from_wan_info(path, bind_if_index, lineNum, useCurIfIndex);
    }
    else if(path)/*no wan binded*/
    {
        useCurIfIndex = 0;
        get_reg_param_from_wan_info(path, -1, lineNum, useCurIfIndex);
    }
    else
    {
        SVC_VOIP_WARN_INFO("no valid interface index.\n");
        return -1;
    }

    return 0;

}

int sip_start_upgrade_wan_down(char* path)
{
	int  j = 0;
	int  if_index = -1;
	int  wan_if_index = 0;
	int  pvc_index = -1;
	int  entry_index = -1;

	int  lineNum = svc_voip_update_line_num();
	char nodeName[32] = {0};
	char wanUpPath[32] = {0};
	char tmp[32] = {0};
	char IP_Protocol[8] = {0};
	char str_servicelist[32] = {0};
	char ifName[32] = {0};
	char isStaticWan[4] = {0};

#ifdef TCSUPPORT_IPV6
	if(cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
		strcpy(IP_Protocol, "IPV4");
	}
#endif
	if((cfg_get_object_attr(nodeName, "LanBindEnable", tmp, sizeof(tmp))>=0)&& (!strcmp(tmp, "1")))
	{
		SVC_VOIP_DEBUG_INFO("Lan binded.\n");
		return 0;
	}

	if(cfg_get_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_PATH_USED", wanUpPath, sizeof(wanUpPath)) < 0)
	{
		SVC_VOIP_WARN_INFO("Can't get SC_SYS_CFG_WAN_PATH_USED attribute from root.VoIPSysParam.Common.\n");
		return -1;
	}

	if(cfg_get_object_attr(wanUpPath, "ISP", isStaticWan, sizeof(isStaticWan)) < 0)
	{
		SVC_VOIP_WARN_INFO("Can't get ISP attribute from %s.\n", wanUpPath);
		return -1;
	}

	SVC_VOIP_DEBUG_INFO("isStaticWan = %s\n", isStaticWan);

	if(!strlen(path) && !strcmp(isStaticWan, "1"))
		path = wanUpPath;

	if(path)
	{
		if_index = get_wanindex_by_path(path);
		SVC_VOIP_DEBUG_INFO("if_index = %d\n", if_index);

		if(if_index>=0)
		{
			voice_wan_path_record[if_index] = 0;
		}
		setVoIPRtpBindWanIP(0, if_index);

		SVC_VOIP_DEBUG_INFO("wanUpPath = %s, path = %s\n", wanUpPath, path);
		if(0 == strcasecmp(path, wanUpPath))
		{
#ifdef TCSUPPORT_IPV6
			if(!strcmp(IP_Protocol, "IPV6"))
				cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", "::");
			else
#endif
				cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IP", "0.0.0.0");
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFNAME", "");
		
			for(j = 1; j <= lineNum; j++)
			{
				/*make sure every line has its independent flag*/
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), INFOVOIP_ENTRY_NODE, j);
		
				/*directly send deregister message*/
				cfg_set_object_attr(nodeName, "WAN_STATUS_CHANGE_FLAG", "up_down");
				cfg_set_object_attr(nodeName, "Status", "Error");
#if !defined(TCSUPPORT_CMCC)
				cfg_set_object_attr(nodeName, "RegFailReason", VOICE_NO_WAN_ERROR);/*error 6: no voice wan*/
#endif
			}
		
			svc_voip_sip_update_handle(VOIPSYSPARAM_NODE);
			cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_IFINDEX_USED", "");
			if(strcmp(isStaticWan, "1")){
				cfg_set_object_attr(VOIPSYSPARAM_COMMON_NODE, "SC_SYS_CFG_WAN_PATH_USED", "");
			}

			if(cfg_get_object_attr(path, "ServiceList", str_servicelist, sizeof(str_servicelist))>= 0 &&
				NULL == strstr(str_servicelist, "VOICE"))
			{
				cfg_set_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", "");

				for(wan_if_index = 0; wan_if_index < MAX_WAN_IF_INDEX; wan_if_index++)
				{
					if(1 == voice_wan_path_record[wan_if_index])
						break;
				}
				SVC_VOIP_DEBUG_INFO("wan_if_index = %d\n", wan_if_index);

				if((wan_if_index < MAX_WAN_IF_INDEX) && (1 == voice_wan_path_record[wan_if_index]))
				{
					pvc_index = wan_if_index / MAX_SMUX_NUM;
					entry_index = wan_if_index % MAX_SMUX_NUM;
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);
					cfg_get_object_attr(nodeName, "IFName", ifName, sizeof(ifName));
					SVC_VOIP_DEBUG_INFO("pvc_index = %d, entry_index = %d, ifName = %s\n", pvc_index, entry_index, ifName);
					update_sip_reg_param(lineNum, wan_if_index, ifName, nodeName, 1);
					cfg_set_object_attr(VOIPADVANCED_COMMON_NODE, "CurIFIndex", itoa(wan_if_index));
				}

			}
		}
	}

	return 0;
}

