

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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_msg.h>
#include <cfg_romfile.h>

#include "wan_related_mgr.h"
#include "wan_related_cfg.h"
#include "cfg_cli.h"
#include "../cfg/utility.h"
#include "blapi_system.h"
#include <ecnt_utility.h>

#define NTP_DEF_SERVER 	"ntp1.cs.wisc.edu"
#define TZ_NTP_TYPE	0
#define TZ_PC_TYPE	1
#define TZ_MANUAL_TYPE	2

#define NODENAME 32

#define MAX_URL_INPUT_LENGTH 48
#define URL_PATTER_PATH "/etc/l7-protocols/Url.pat"
#define HTTP "http://"
#if defined(TCSUPPORT_CT_E8GUI)
#define URL_TMP_SH	"/tmp/Urltmp.sh"
#define	DEL_URL		1
#define	DEL_IP		2
#define URL_WHITE_POLICY_CMD	"iptables -A INPUT -m string --algo bm --string %s -j ACCEPT\n"
#endif
#define URL_LEN	32
#define END_OF_ENTRY "-------end of entry-------\r\n"

#define MAX_MAC_LEN				260
#define HTTPS					"https://"

/* service list */
#define	INTERNET				"INTERNET"
#define	TR069          	 		"TR069"
#define	TR069_INTERNET   		"TR069_INTERNET"
#define	VOICE          	 		"VOICE"
#define OTHER               	"OTHER"

#if 0
int correct_sys_time(void)
{
	time_t tm;
	char optr = {0};
	char value[16]={0};
	char dayLightStr[8] = {0};
	char nodeName[64] = {0};
	char *tmp;
	const char *delim = ":";
	int hour=0, min=0;
	int diff = 0;
	int res = 0;

	memset(nodeName,0,sizeof(nodeName));

	strncpy(nodeName, TIMEZONE_ENTRY_NODE, sizeof(nodeName)-1);
	
	if(cfg_get_object_attr(nodeName, "TZ", value, sizeof(value)) < 0)
	{
		return FAIL;
	}
	else
	{
		/*if return value is "GMT, we do not need to do anything."*/
		if(!strcasecmp(value, "GMT")||!strcasecmp(value, "GMT-1"))
		{
			return SUCCESS;
		}

		/*    GMT-03:30    */
		/*get diff type, either '-' or '+'*/
		optr = (char)value[3];
		/* get time string, in this case, tmp = "03:30"*/
		tmp = &value[4];

		/*retrieve hour value, in this case, hour = "03"*/
		hour = atoi(strtok(tmp, delim));
		/*==20090118 pork added==*/
		/*If Day light saving is enabled, hour will be decreased 1.*/
		cfg_get_object_attr(nodeName, "DAYLIGHT", dayLightStr, sizeof(dayLightStr));
		if(dayLightStr[0] != '\0')
		{
			if(!strcmp(dayLightStr,"Enable"))
			{
				if(optr == '-')
				{
					hour--;
				}
				else if(optr == '+')
				{
					hour++;
				}
				else
				{
					return FAIL;
				}
			}
		}
		/*==20090118 pork added==*/
		/*retrieve minute value, in this case, min = "30"*/
		min = atoi(strtok(NULL, delim));

		/*calculate time difference*/
		diff = hour*3600+ min*60;
		/*retrieve system time*/
		time(&tm);

		/*minus or plus time difference*/
		if(optr == '-')
		{
			tm-=diff;
		}
		else if(optr == '+')
		{
			tm+=diff;
		}
		else
		{
			return FAIL;
		}

		/*set system time*/
		res = stime(&tm);
		if(res){
			/*do nothing,just for compile*/
		}
	}
	return SUCCESS;
}/* end correct_sys_time*/
#endif

#if defined(TCSUPPORT_CMCCV2)
int 
sendMsgToMobile(char *tz_gmt){
	nlk_msg_t nklmsg;
	traffic_msg_t *nfmsg = NULL;
	int ret = 0;

	memset(&nklmsg, 0, sizeof(nlk_msg_t));	
	nklmsg.nlmhdr.nlmsg_len = NLK_MSG_LEN(traffic_msg_t);
	nklmsg.nlmhdr.nlmsg_pid = getpid();
	nklmsg.nlmhdr.nlmsg_flags = 0;
	nklmsg.eventType = MSG_NOTIFY2MOBILE;

	nfmsg = &nklmsg.data.fmsg;

	snprintf(nfmsg->bundlename, sizeof(nfmsg->bundlename)
			, "%s", "ALL");

	snprintf(nfmsg->buf, sizeof(nfmsg->buf)
	, "{\"TZ\":\"%s\"}"
	,  tz_gmt);

	tcdbg_printf("nklmsg.data.payload=%s,nfmsg->bundlename=[%s]\n"
			, nfmsg->buf, nfmsg->bundlename);

	ret = nlkmsg_send(NLKMSG_GRP_MOBILE, NLK_MSG_LEN(traffic_msg_t), &nklmsg);
	if(ret < 0) 
		printf("send message err!\n");
	return SUCCESS;
}
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
void set_tz2file(char *tz_gmt)
{
	FILE *pFile = NULL;

	pFile = fopen(DIAG_TZ_FILE,"w");
	if(pFile==NULL)
	{
		return ;
	}
	fwrite(tz_gmt,1,strlen(tz_gmt),pFile);
	fflush(pFile);
	fclose(pFile);
	return;

}
#endif
int TZ_modify(char* path)
{
	char tz_gmt[16]={0};	
	char tz_str[16] = {0};
	FILE *pFile = NULL;
	char Bgmt[15]={0};
	int m=0;
#if !defined __UCLIBC__
	char tz_path[32] = {0}, cmd[64] = {0};
#else
	char dayLightStr[8] = {0}, tmp[12] = {0};
	int hour = 0 , min = 0;
#endif	
	char tz_Agmt [32][10]=
	{
		{"GMT-12:00"},
		{"GMT-11:00"},
		{"GMT-10:00"},
		{"GMT-09:00"},
		{"GMT-08:00"},
		{"GMT-07:00"},
		{"GMT-06:00"},
		{"GMT-05:00"},
		{"GMT-04:00"},
		{"GMT-03:30"},
		{"GMT-03:00"},
		{"GMT-02:00"},
		{"GMT-01:00"},
		{"GMT"},
		{"GMT+01:00"},
		{"GMT+02:00"},
		{"GMT+03:00"},
		{"GMT+03:30"},
		{"GMT+04:00"},
		{"GMT+04:30"},
		{"GMT+05:00"},
		{"GMT+05:30"},
		{"GMT+05:45"},
		{"GMT+06:00"},
		{"GMT+06:30"},
		{"GMT+07:00"},
		{"GMT+08:00"},
		{"GMT+09:00"},
		{"GMT+09:30"},
		{"GMT+10:00"},
		{"GMT+11:00"},
		{"GMT+12:00"},			
	};

		char ABB_TimeZone[32][12]=
			{
					{"IDLW+12:00"},
					{"NTT+11:00"},
					{"HAST+10:00"},
					{"AKST+09:00"},
					{"PST+08:00"},
					{"MST+07:00"},
					{"ACST+06:00"},
					{"EST+05:00"},
					{"AST+04:00"},
					{"NST+03:30"},
					{"BRT+03:00"},
					{"MAT+02:00"},
					{"WAT+01:00"},
					{"GMT+00:00"},
					{"MET-01:00"},
					{"EET-02:00"},
					{"BTT-03:00"},
					{"ITT-03:30"},
					{"MSK-04:00"},
					{"KBT-04:30"},
					{"PKT-05:00"},
					{"IST-05:30"},
					{"NPT-05:45"},
					{"KGT-06:00"},
					{"MMT-06:30"},
					{"CXT-07:00"},
					{"CST-08:00"},
					{"JST-09:00"},
					{"CAST-09:30"},
					{"LIGT-10:00"},
					{"MAGT-11:00"},
					{"IDLE-12:00"}, 				
			};

		memset(tz_gmt, 0, sizeof(tz_gmt));
		cfg_get_object_attr(path, "TZ", tz_gmt,sizeof(tz_gmt));
		if('\0' != tz_gmt[0])
		{
			tcdbg_printf("\n tz_gmt=%s\n",tz_gmt);

			strncpy(tz_str, tz_gmt, sizeof(tz_Agmt[0])-1);
#if defined(TCSUPPORT_CMCCV2)
			sendMsgToMobile(tz_str);
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
			set_tz2file(tz_str);
			SetDiagTz();
#endif
                  	for(m=0;m<32;m++)
			{
				if ((strncmp(tz_str, tz_Agmt[m], sizeof(tz_str)-1)==0))
				{
					snprintf(Bgmt,sizeof(Bgmt), "%s\n",ABB_TimeZone[m]);
					break;
				}
			}
#if !defined __UCLIBC__
			snprintf(tz_path,sizeof(tz_path),ZONEINFO_FILE"%s", tz_gmt);
			if (access(tz_path, 0) != 0) {
				memset(tz_path, 0, sizeof(tz_path));
				snprintf(tz_path,sizeof(tz_path),ZONEINFO_FILE"%s", tz_str);
				if(access(tz_path, 0) != 0)
				{
					tcdbg_printf("No timezone file[%s]\n",tz_path);
					return FAIL;
				}
			}
			
			unlink(LOCALTIME_FILE);
			snprintf(cmd,sizeof(cmd),"ln -s %s "LOCALTIME_FILE, tz_path);
			system(cmd);
#else
			
			if(cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "DAYLIGHT", dayLightStr,sizeof(dayLightStr))>0){
				if(!strcmp(dayLightStr,"Enable")){
					sscanf(Bgmt, "%[A-Z]%3d:%2d", tmp, &hour, &min);
					hour--;
					snprintf(Bgmt, sizeof(Bgmt),"%s%d:%d\n", tmp, hour, min);
				}
				
			}
#endif
			pFile = fopen(TZ_FILE,"w");
			if(pFile==NULL)
			{
				return FAIL;
			}
			fwrite(Bgmt,1,strlen(Bgmt),pFile);
			fflush(pFile);
			fclose(pFile);
		}
	return SUCCESS;
}

int updateNtpState(int syncst)
{
	char nodeName[64] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	
	strncpy(nodeName, PMINFORM_ENTRY_NODE, sizeof(nodeName)-1);

	if ( 0 == syncst )
		cfg_set_object_attr(nodeName, "NTPSync", "Yes");

	return 0;
}


unsigned int ntpdelayflag1_all[4] ={0,0,0,0};
unsigned int ntpdelayflag2_all[4] ={0,0,0,0};
struct timespec req_time_pre,req_time_cur,req_time_diff;
extern char ntpneedexecute;

void initNTPTimerStruct()
{
	memset(&req_time_pre,0,sizeof(req_time_pre));
	memset(&req_time_cur,0,sizeof(req_time_cur));
	memset(&req_time_diff,0,sizeof(req_time_diff));
	return;
}

void setNTPDelayFlagByIFindex(int if_index, int index)
{
	if(if_index > 31)
		ntpdelayflag2_all[index] |=	1 << (if_index - 32);
	else
		ntpdelayflag1_all[index] |=	1 << if_index;
}

void setNTPDelayFlag(char* service_app, int if_index)
{	
	if (strstr(service_app,INTERNET))
	{
		setNTPDelayFlagByIFindex(if_index,0);
	}
	
	if(strstr(service_app,VOICE))
	{
		setNTPDelayFlagByIFindex(if_index,1);
	}
	
	if(strstr(service_app,TR069))
	{
		setNTPDelayFlagByIFindex(if_index,2);
	}

	if(strstr(service_app,OTHER))
	{
		setNTPDelayFlagByIFindex(if_index,3);
	}
}

int resyncNTPServer(char *if_name, int if_index, int state)
{
	char nodeName[64] = {0};
	char tz_type[4]={0};
#if !defined(TCSUPPORT_NP_CMCC)
	int pvc_index = 0, entry_index = 0;
#endif
	char service_app[64] = {0};
#if defined(TCSUPPORT_CT_2NTP)
	char NTPServerType[4] = {0};
	char ntp_service_app[20] = {0};
	int i=0;
#endif
	char ntpdelayflag1str[18] = {0};
	char ntpdelayflag2str[18] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
	char ntp_first_boot[12] = {0};
#endif
	unsigned int ntpdelayflag1 =0;
	unsigned int ntpdelayflag2 =0;
	
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, TIMEZONE_ENTRY_NODE, sizeof(nodeName)-1);
	
	initNTPTimerStruct();
	if(cfg_get_object_attr(nodeName, "TYPE", tz_type, sizeof(tz_type)) < 0)
	{
		tcdbg_printf("\r\nresyncNTPServer:get type error!");
		return FAIL;
	}
#if defined(TCSUPPORT_CT_2NTP)
	if(cfg_get_object_attr(nodeName, "NTPServerType", NTPServerType, sizeof(NTPServerType)) < 0)
	{
		tcdbg_printf("\r\nresyncNTPServer:get NTPServerType error!");
		return FAIL;
	}
#endif

	if(1)
	{
#if defined(TCSUPPORT_NP_CMCC)
		memset(service_app, 0, sizeof(service_app));
		strncpy(service_app, "INTERNET", sizeof(service_app) - 1);
#else
#if defined(TCSUPPORT_CT_UBUS)
		if ( 0 == strcmp(if_name, WAN_ITF_BR0_1) )
		{
			memset(service_app, 0, sizeof(service_app));
			strncpy(service_app, "INTERNET", sizeof(service_app) - 1);
		}
		else
#endif
		{
		/* get service list of this interface */
		pvc_index = if_index / MAX_SMUX_NUM;
		entry_index = if_index - pvc_index * MAX_SMUX_NUM;
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);
		memset(service_app, 0, sizeof(service_app));
		cfg_get_object_attr(nodeName, "ServiceList", service_app, sizeof(service_app));

		if ('\0' == service_app[0]) 
		{
			tcdbg_printf("\r\nresyncNTPServer:without servicelist!");
			return FAIL;
		}
		}
#endif

#if defined(TCSUPPORT_CT_2NTP)
		if(1)
#else
		/*only handle wan link with internet */
		if (strstr(service_app, INTERNET)) /*not only tr069 link*/
#endif
		{
			switch (state) 
			{
				case IF_STATE_UP:
#if defined(TCSUPPORT_CT_2NTP)
					/***
					NTPServerType:
					0:INTERNET
					1:VOICE
					2:TR069
					3:OTHER
					***/
					setNTPDelayFlag(service_app,(if_index));
					switch (atoi(NTPServerType))
					{
						case 0:
							strncpy(ntp_service_app, INTERNET, sizeof(ntp_service_app)-1);
							break;
						case 1:
							strncpy(ntp_service_app, VOICE, sizeof(ntp_service_app)-1);
							break;
						case 2:
							strncpy(ntp_service_app, TR069, sizeof(ntp_service_app)-1);
							break;
						case 3:
							strncpy(ntp_service_app, OTHER, sizeof(ntp_service_app)-1);
							break;			
						default:
							tcdbg_printf("\r\nresyncNTPServer:get NTPServerType error.");
							return FAIL;
					}
					if (strstr(service_app,ntp_service_app))
					{
#endif
						if(if_index > 31)
						{
							ntpdelayflag2 |=	1 << (if_index - 32);
							snprintf(ntpdelayflag2str, sizeof(ntpdelayflag2str), "%d", ntpdelayflag2);
							cfg_set_object_attr(GLOBALSTATE_NODE, "ntpdelayflag2", ntpdelayflag2str);
						}
						else
						{
							ntpdelayflag1 |=	1 << if_index;
							snprintf(ntpdelayflag1str, sizeof(ntpdelayflag1str), "%d", ntpdelayflag1);
							cfg_set_object_attr(GLOBALSTATE_NODE, "ntpdelayflag1", ntpdelayflag1str);
						}
#if defined(TCSUPPORT_CT_2NTP)
					}
#endif
					cfg_set_object_attr(GLOBALSTATE_NODE, "ntpneedexecute", "1");

#if defined(TCSUPPORT_CT_JOYME4)
					if ( 0 == atoi(NTPServerType) )
					{
						/* only support INTERNET, other wan get wan ip slow. */
						if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", ntp_first_boot, sizeof(ntp_first_boot)) <= 0
							|| 0 == ntp_first_boot[0] )
						{
							/* thread only for first boot. */
							cfg_set_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", "1");
							creat_boot_wan_ntp_check_pthread();
						}
					}
					else
					{
						cfg_set_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", "2");
					}
#endif
					break;
				case IF_STATE_DOWN:
					if(if_index > 31)
					{
						ntpdelayflag2 &=	~(1 << (if_index - 32));
						snprintf(ntpdelayflag2str, sizeof(ntpdelayflag2str), "%d", ntpdelayflag2);
						cfg_set_object_attr(GLOBALSTATE_NODE, "ntpdelayflag2", ntpdelayflag2str);
					}
					else
					{
						ntpdelayflag1 &=	~(1 << if_index);
						snprintf(ntpdelayflag1str, sizeof(ntpdelayflag1str), "%d", ntpdelayflag1);
						cfg_set_object_attr(GLOBALSTATE_NODE, "ntpdelayflag1", ntpdelayflag1str);
					}
#if defined(TCSUPPORT_CT_2NTP)
					for(i = 0; i < 4; i++){
						if(if_index > 31)
							ntpdelayflag2_all[i] &=	~(1 << (if_index - 32));
						else
							ntpdelayflag1_all[i] &=	~(1 << if_index);	
					}
#endif
					break;
				default:
					break;	
			}	
		}
	}

	return SUCCESS;
}

void correctPPPtime(int difference){
	char pppUptimePath[32]={0};
	char pvc_info[16]={0};
	FILE *fp;
	int oldtime=0,i=0;

	for(i=0;i<PVC_NUM;i++){/*find all possible time stamp*/
		sprintf(pppUptimePath,"/tmp/pppuptime-ppp%d",i);
		fileRead(pppUptimePath, pvc_info, sizeof(pvc_info));
		oldtime=atoi(pvc_info);
		if(oldtime){
			fp = fopen(pppUptimePath, "w+");
			if(fp){
				oldtime=oldtime+difference;
				fprintf(fp,"%d",oldtime);
				fclose(fp);
			}
		}
	}
}


int timezone_set_time(int tz_type){
	char time_info[6][8];
	int i = 0;		
	time_t tm;
	struct tm tm_time;
	char *delim="/: ";
	char tz_pc_clock[32]={0};
	char *pValue=NULL;
	char time_attr[][8]={"Year","Month","Date","Hour","Min","Sec",""};

	memset(time_info, 0, sizeof(time_info));
	if(tz_type == TZ_PC_TYPE)
	{
		cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "PC_CLOCK", tz_pc_clock, sizeof(tz_pc_clock));
		strtok(tz_pc_clock,delim);
		strncpy(time_info[0], tz_pc_clock, sizeof(time_info[0])-1);
		for( i = 1; (pValue=strtok(NULL, delim))!= NULL; i++)
		{
			strncpy(time_info[i], pValue, sizeof(time_info[i])-1);
		}
	}
	else
	{
		for(i=0; strlen(time_attr[i])!=0; i++)
		{
			cfg_get_object_attr(TIMEZONE_ENTRY_NODE, time_attr[i], time_info[i], sizeof(time_info));
		}
	}
	memset(&tm, 0, sizeof(tm));
	time(&tm);
	memset(&tm_time, 0, sizeof(tm_time));

	tm_time.tm_year = atoi(time_info[0]);
	tm_time.tm_mon	= atoi(time_info[1]);
	tm_time.tm_mday = atoi(time_info[2]);
	tm_time.tm_hour = atoi(time_info[3]);
	tm_time.tm_min	= atoi(time_info[4]);
	tm_time.tm_sec	= atoi(time_info[5]);

	/* correct for century	- minor Y2K problem here? */
	if (tm_time.tm_year >= 1900)
	{
		tm_time.tm_year -= 1900;
	}
	/* adjust date */
	if (tm_time.tm_mon >= 1)
	{
		tm_time.tm_mon -= 1;
	}

	tm = mktime(&tm_time);
	if (tm < 0)
	{
		printf("mktime error\n");
	}
	else{
		if (stime(&tm) < 0)
		{
			printf("stime err!\n");
		}
	}
	return 0;
}

int svc_wan_handle_event_timezone_boot(void)
{
	if(TZ_modify(TIMEZONE_ENTRY_NODE) == SUCCESS)
	{
		printf("\r\n timezone_execute:TZ_modify SUCCESS!\n");
		return 0;
	}
	
	return -1;
}

int svc_wan_handle_event_timezone_update(wan_related_cfg_update_t* param)
{
	int ret=-1;
	int now_time=0,correct_time=0;
	time_t tm;
	char tz_type[4]={0};
	int i=0;
	char cmd[256] = {0};
	char server[64]={0};
#if defined(TCSUPPORT_CT_2NTP)
	char server2[64]={0};
#if defined(TCSUPPORT_CT_5NTP) || defined(TCSUPPORT_CT_2NTP)
	char server3[64]={0};
	char server4[64]={0};
	char server5[64]={0};
#endif
	char ntpservertype[4]={0};
	char route[32]={0};
	char addrouteCMD[128]={0};
	char convertIP[64]={0}; 
	char convertIP2[64]={0};
	int if_index=0;
#endif
#if defined(TCSUPPORT_CT_MONITORCOLLECTOR)
	char nodeName[64] = {0};
#endif
	int svrType = 0;
	int ntp_server_ret= -1;
	int flag1 =0;
	int flag2 =0, retry_time = 1;
	char syncRes[4] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
	char ntp_first_boot[12] = {0}, ip_addr[20], waninfo_nodename[32] = {0};
	char resolver_host[40] = {0}, wan_v4_up[12] = {0};
	int max_dns_retry = 6, cnt = 0, resolve_ok = 0;
#endif

	now_time=time(&tm);
	
	if ( NULL != strstr(param->path, "Success") || NULL != strstr(param->path, "success") )
	{
		ret = 0;
	}else if ( NULL != strstr(param->path, "Fail") || NULL != strstr(param->path, "fail") )
	{
		ret = 1;
	}

	
	if(cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "TYPE", tz_type, sizeof(tz_type)) > 0)
	{
		switch(atoi(tz_type)){
			case TZ_NTP_TYPE:/*NTP Server automatically*/
			if(0 == ret ) /* success*/
			{
				printf("\r\n timezone_execute:NTP Server Sync success!");
				ntp_server_ret = 0;
#if 0
				if(correct_sys_time()){
					printf("Cannot retrieve TimeZone data.\n");
				}
#endif
				if(TZ_modify(TIMEZONE_ENTRY_NODE)==SUCCESS)
				{
					printf("\r\n timezone_execute:TZ_modify SUCCESS!\n");
				}
				
#if defined(TCSUPPORT_CT_MONITORCOLLECTOR)		
				memset(nodeName,0, sizeof(nodeName));
				strncpy(nodeName, MONITORCOLLECTOR_COMMON_NODE, sizeof(nodeName)-1);			
				cfg_set_object_attr(nodeName, "ntpSync", "1");
#endif

				updateNtpState(ntp_server_ret);
				cfg_set_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", "1");
			}
			else if(1 == ret) /* fail */
			{
				ntp_server_ret = -1;				
				if( cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", syncRes, sizeof(syncRes)) < 0 )
				cfg_set_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", "0");
				return SUCCESS;
			}
			else/* excute */
			{ 
				ntp_server_ret = -1;
				if( cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", syncRes, sizeof(syncRes)) < 0 )
				cfg_set_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", "0");
			
#if defined(TCSUPPORT_CT_2NTP)
				memset(server,0,sizeof(server));
				memset(route,0,sizeof(route));
				memset(convertIP,0,sizeof(convertIP));
				memset(convertIP2,0,sizeof(convertIP2));
				
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER", server, sizeof(server));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER2", server2, sizeof(server2));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER3", server3, sizeof(server3));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER4", server4, sizeof(server4));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER5", server5, sizeof(server5));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "NTPServerType", ntpservertype, sizeof(ntpservertype));
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "AddRoute", route, sizeof(route));
				if((server[0] != '\0') && (server2[0] != '\0') &&
#if defined(TCSUPPORT_CT_5NTP)
					(server3[0] != '\0') && (server4[0] != '\0') && (server5[0] != '\0') &&
#endif
					(ntpservertype[0] != '\0'))
				{
#if defined(TCSUPPORT_CT_5NTP)
					if ( 0 == strcmp(server, "0.0.0.0")
						&& 0 == strcmp(server2, "0.0.0.0")
						&& 0 == strcmp(server3, "0.0.0.0")
						&& 0 == strcmp(server4, "0.0.0.0")
						&& 0 == strcmp(server5, "0.0.0.0") )
						strncpy(server, NTP_DEF_SERVER, sizeof(server)-1);
#else
					if ( 0 == strcmp(server, "0.0.0.0")
						&& 0 == strcmp(server2, "0.0.0.0") )
						strncpy(server, NTP_DEF_SERVER, sizeof(server)-1);
#endif
					/*get first match if_index*/
					/***
					NTPServerType:
					0:INTERNET
					1:VOICE
					2:TR069
					3:OTHER
					***/				
					i=atoi(ntpservertype);
					svrType = i;
					if(i < 0 || i >3)
						return FAIL;
					flag1=ntpdelayflag1_all[i];
					flag2=ntpdelayflag2_all[i];
					if(0 != flag1)
					{
						for(i = 0; i < 32;i++)
						{
							if(flag1&0x01)
								break;
							flag1=flag1>>1;
						}
						if_index=i;
					}
					else if(0 != flag2)
					{
						for(i = 0; i< 32;i++)
						{
							if(flag2&0x01)
								break;
							
							flag2=flag2>>1;
						}
						if_index=i+32;
					}
					else
					{
						return FAIL;
					}
					if ( 0 != svrType )
					{
						if(get_waninfo_by_index(if_index,"IFName",route, sizeof(route)) < 0)
						{
							return FAIL;
						}					
					}

#if defined(TCSUPPORT_CT_JOYME4)
					if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", ntp_first_boot, sizeof(ntp_first_boot)) > 0
								&& 1 == atoi(ntp_first_boot)
								&& cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "WanStatus", wan_v4_up, sizeof(wan_v4_up)) > 0
								&& 1 == atoi(wan_v4_up) )
					{
						retry_time = 3;

						cfg_set_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", "2");
						/*
						resolve dns first, ntp sync with ip directly when cpe boot.
						*/
						if ( 0 == strcmp(server, "0.0.0.0")
							||  0 == strcmp(server, NTP_DEF_SERVER) )
						{
							/* resolve dns by self. */
							bzero(waninfo_nodename, sizeof(waninfo_nodename));
							snprintf(waninfo_nodename, sizeof(waninfo_nodename), WANINFO_ENTRY_NODE, if_index + 1);
							cfg_get_object_attr(waninfo_nodename, "DNS", resolver_host, sizeof(resolver_host));

							for ( cnt = 0; cnt < max_dns_retry; cnt ++ )
							{
								/* parse domain, timeout is 1s. */
								if ( 0 == ecnt_dns_resolve_adv(resolver_host, NTP_DEF_SERVER, ip_addr, sizeof(ip_addr), AF_INET, 1) )
								{
									resolve_ok = 1;

									if ( resolve_ok )
									{
										snprintf(server, sizeof(server), "%s", ip_addr);
										break;
									}
								}
							}
						}
					}
#endif

#if defined(TCSUPPORT_CT_5NTP)
					sprintf(addrouteCMD, "-h \"%s %s %s %s %s\"", server, server2, server3, server4, server5);
#else
					sprintf(addrouteCMD, "-h \"%s %s\"", server, server2);
#endif
					if ( 0 != svrType )
					{
						strcat(addrouteCMD, " -w ");
						strcat(addrouteCMD, route); 				
						
					}

					snprintf(cmd, sizeof(cmd),"/userfs/bin/ntpclient -s -c %d -l %s &", retry_time, addrouteCMD);
					system_escape(cmd);
					return SUCCESS; 			
				}
#else
				cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER", server, sizeof(server));
				snprintf(cmd, sizeof(cmd),"/userfs/bin/ntpclient -s -c 1 -l -h %s &", strcmp(server,"0.0.0.0") ? server : NTP_DEF_SERVER);
				system_escape(cmd);
#endif
			}
				break;
			case TZ_PC_TYPE:/*PC CLOCK*/
			case TZ_MANUAL_TYPE:/*Manually*/	
				timezone_set_time(atoi(tz_type));
				break;
			default:
				break;
		}
	}	

	/*krammer add for bug 907*/
	memset(&tm, 0, sizeof(tm));
	correct_time=time(&tm);/*get correct time after time synchronization*/
	if(abs(correct_time - now_time) > 30)/*ppp old connect time should be changed when difference >30*/
	{
		correctPPPtime((correct_time - now_time));
	}

	return SUCCESS;
}


int svc_wan_related_appfilter_write(void)
{
	int blockAttributeIndex=0, res = 0;
	char blockAttributeName[][15]=
	{
		"Block_ICQ","Block_MSN","Block_YMSG","Block_RTSP"
	};
	char patternName[][20]=
	{
		"aim","msnmessenger","yahoo","rtsp"
	};
	char attributeValue[8] = {0};
	char Activate[8] = {0};
	char iptableCmd[100]={0};
	FILE *fp;
	cfg_node_type_t *node = NULL;

	/*node = AppFilter_Entry*/
	
	node = cfg_type_query(APPFILTER_ENTRY_NODE);
	if(node == NULL){
		printf("Error--AppFilter Entry is NULL.\n");
		return FAIL;
	}

	/*get Activate value*/
	
	cfg_get_object_attr(APPFILTER_ENTRY_NODE, "Activate", Activate, sizeof(Activate));
	if(Activate[0] == '\0'){
		printf("Error--AppFilter Activate value is NULL.\n");
		return FAIL;
	}

	fp = fopen(APP_FILTER_SH, "w+");
	if(fp == NULL){
		printf("Error--AppFilter's file pointer is NULL.\n");
		return FAIL;
	}

	/*appFilter is activated*/
	if(!strcmp(Activate, "1")){

		/*write the iptables cmd into AppFilter.sh*/
		fputs("iptables -t filter -F app_filter_chain\n", fp);
		fputs("iptables -t filter -Z app_filter_chain\n", fp);



		for(blockAttributeIndex=0; blockAttributeIndex < 4; blockAttributeIndex++){
			/*get a attribute according to the attribute index*/			
			cfg_get_object_attr(APPFILTER_ENTRY_NODE, blockAttributeName[blockAttributeIndex], attributeValue, sizeof(attributeValue));
			/*the value must be set*/
			if(attributeValue[0] != '\0'){
				/*if the value is 1 , we write the cmd into AppFilter.sh*/
				if(!strcmp(attributeValue, "1")){
					/*setup the iptableCmd and write it into shell script*/
					snprintf(iptableCmd, sizeof(iptableCmd), "iptables -t filter -A app_filter_chain -m layer7 --l7proto %s -j DROP\n", patternName[blockAttributeIndex]);
					fputs(iptableCmd, fp);
				}
				/*if the value is neither 1 nor 0, this is a wrong setting.*/
				else if(strcmp(attributeValue, "0")){
					/*if the value is not 1 and 0, it must be a wrong attribute value*/
					printf("Error--AppFilter get a wrong attribute value of %s.\n", blockAttributeName[blockAttributeIndex]);
				}
			}
			else{
				printf("Error--AppFilter get a NULL attribute value of %s.\n", blockAttributeName[blockAttributeIndex]);
			}
		}
	}

	fclose(fp);

	res = chmod(APP_FILTER_SH ,777);
	if(res){
		/*do nothing,just for compile*/
	}
	return SUCCESS;
}

int app_check_filter(void)
{
	int filter_on=0;
	char nodeName[64] = {0};
	char appfilter_status[5]={0};
	/*check app filter state*/

	/*flush the buffer*/
	memset(nodeName,0,sizeof(nodeName));
	/*build the node name = AppFilter_Entry*/
	strncpy(nodeName, APPFILTER_ENTRY_NODE, sizeof(nodeName)-1);

	/*check the Activate value*/
	if(cfg_get_object_attr(nodeName, "Activate", appfilter_status, sizeof(appfilter_status)) < 0)
	{
		printf("get app filter activate error in check filter state\n");
	}
	/*see if appfilter's attribute(Activate) = 1*/
	else if(!strcmp(appfilter_status,"1"))
	{
		filter_on=1;/*load the iptable_filter module*/
	}
	return filter_on;
}

int url_filter_deletewhiterules(int ipolicy)
{
	FILE *fr = NULL;
	FILE *fw = NULL;
	int	nCount = 0;
	char *pStart = NULL;
	char line[128] = {0};
	int res = 0;
	
	fr = fopen(URL_FILTER_SH, "r");
	if(fr == NULL)
	{
		return FAIL;
	}
	memset(line, 0x00, sizeof(line));
	res = fseek(fr,0,SEEK_SET);
	if(res){
		/*do nothing,just for compile*/
	}
	fgets(line, sizeof(line), fr);
	if((strstr(line, "--algo")) == NULL)
	{
		if(1 == ipolicy)
			system(URL_STOP_SH);
		fclose(fr);
		fr = NULL;
		return SUCCESS;
	}
	fw = fopen(URL_TMP_SH, "w");
	if(fw == NULL)
	{
		printf("url_filter_deletewhiterules:create %s failed!!\n", URL_TMP_SH);
		fclose(fr);
		fr = NULL;
		return FAIL;
	}
	res = fseek(fr,0,SEEK_SET);
	if(res){
		/*do nothing,just for compile*/
	}
	memset(line, 0x00, sizeof(line));
	while (fgets(line, sizeof(line), fr)) 
	{
		if(strstr(line, "iptables -P FORWARD ACCEPT") != NULL)
			break;
		if((pStart = strstr(line, "-A")) != NULL)
		{
			nCount = pStart - &line[0];
			line[nCount+1] = 'D';
		}
		fputs(line, fw);
		memset(line, 0x00, sizeof(line));
	}
	fclose(fr);
	fr = NULL;
	fclose(fw);
	fw = NULL;
	res = chmod(URL_TMP_SH ,777);
	if(res){
		/*do nothing,just for compile*/
	}
	system(URL_TMP_SH);
	usleep(100000);
	unlink(URL_TMP_SH);
	return SUCCESS;	
}

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)		
int clearUrlfilterIptablesIdx(int idx)
{	
	char chain_name[32] = {0};
	char cmd[128] = {0};
	char cmd6[128] = {0};
	
	if(idx > 0){
	memset(chain_name, 0, sizeof(chain_name));
		snprintf(chain_name, sizeof(chain_name), "url_%d_https_filter", idx - 1);
	}
	else if( 0 == idx){
		memset(chain_name, 0, sizeof(chain_name));
		snprintf(chain_name, sizeof(chain_name), "url_https_filter");
	}

	memset(cmd, 0, sizeof(cmd));
	memset(cmd6, 0, sizeof(cmd6));
	snprintf(cmd, sizeof(cmd), "iptables -t filter -F %s", chain_name);
	snprintf(cmd6, sizeof(cmd6), "ip6tables -t filter -F %s", chain_name);
	system_escape(cmd);
	system_escape(cmd6);

	memset(cmd, 0, sizeof(cmd));
	memset(cmd6, 0, sizeof(cmd6));
	snprintf(cmd, sizeof(cmd), "iptables -t filter -X %s", chain_name);
	snprintf(cmd6, sizeof(cmd6), "ip6tables -t filter -X %s", chain_name);
	system_escape(cmd);	
	system_escape(cmd6);

	memset(cmd, 0, sizeof(cmd));
	memset(cmd6, 0, sizeof(cmd6));
	snprintf(cmd, sizeof(cmd), "iptables -t filter -D FORWARD -j %s", chain_name);
	snprintf(cmd6, sizeof(cmd6), "ip6tables -t filter -D FORWARD -j %s", chain_name);
	system_escape(cmd); 
	system_escape(cmd6);

	if(0 == idx){
		memset(cmd, 0, sizeof(cmd));
		memset(cmd6, 0, sizeof(cmd6));
		snprintf(cmd, sizeof(cmd), "iptables -t filter -N %s", chain_name);
		snprintf(cmd6, sizeof(cmd6), "ip6tables -t filter -N %s", chain_name);	system_escape(cmd);
		system_escape(cmd6);

		memset(cmd, 0, sizeof(cmd));
		memset(cmd6, 0, sizeof(cmd6));
		snprintf(cmd, sizeof(cmd), "iptables -t filter -A FORWARD -j %s", chain_name);
		snprintf(cmd6, sizeof(cmd6), "ip6tables -t filter -A FORWARD -j %s", chain_name);
		system_escape(cmd);
		system_escape(cmd6);
	}

	return 0;
}
#endif

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
filter_setting_t urlfilter_setting[MAX_URLFILTER_NUM];
filter_setting_t dnsfilter_setting[MAX_DNSFILTER_NUM];

static int isTimeInDuration(filter_duration_t *duration)
{
#define GET_BIT_IN_BYTE(idx, byte)	(((unsigned char)(1<<(idx))) & ((unsigned char)byte))
	struct tm *cur_time = NULL;
	time_t t;
	int    week = 0;
	int cur_sec = 0;
	int i = 0;

	time(&t);
	cur_time = localtime(&t);

	cur_sec = cur_time->tm_hour * 3600 + cur_time->tm_min * 60 + cur_time->tm_sec;
	for (week = 0; week < 7; week++)
	{
		if (!GET_BIT_IN_BYTE(week, duration->repeat_days) || week != cur_time->tm_wday)
		{
			continue;
		}
		/* 00:00-00:00 */
		if (duration->allday_flag == 2)
		{
			return 1;
		}

		for(i = 0; i < MAX_DURATION_PER_ENTRY; i++)
		{
			if(duration->start[i] > duration->end[i] &&
				(cur_sec >= duration->start[i] || cur_sec <= duration->end[i]))
				return 1;
		
			if(cur_sec >= duration->start[i] && cur_sec <= duration->end[i])
				return 1;		
		}
	}

	return 0;
}

int isStepInOutBoundary(int node_idx, filter_duration_t *duration, char *nodeName)
{
	int old_stat = 0, now_stat = 0;

	old_stat = duration->matched_stat;
	now_stat = isTimeInDuration(duration);

	if(old_stat == 0 && now_stat == 1)
	{
		duration->matched_stat = 1;
		cfg_set_object_attr(nodeName, "induration", "1");	
		return 1;
	}
	else if(old_stat == 1 && now_stat == 0)
	{
		duration->matched_stat = 0;
		cfg_set_object_attr(nodeName, "induration", "0");	
		return 1;
	}

	return 0;
}

int initurlfilterduration(int idx)
{
	filter_duration_t *duration = NULL;
	char node_val[128] = {0};
	char attr_name[32], node_name[32];
	int dur_idx = 0, node_idx = 0, tmpduridx = 0;
	int maxUrlNum = MAX_URLFILTER_NUM, minUrlNum = 0;

	if(idx == 0)
		bzero(urlfilter_setting, sizeof(urlfilter_setting));

	if(idx > 0)
	{
		maxUrlNum = idx;
		minUrlNum = idx - 1;
	}
	
	for (node_idx = minUrlNum; node_idx < maxUrlNum; node_idx++)
	{
		duration = &urlfilter_setting[node_idx].duration;
		memset(duration, 0, sizeof(filter_duration_t));
		snprintf(node_name, sizeof(node_name), URLFILTER_ENTRY_N_NODE, (node_idx+1));
		
		cfg_set_object_attr(node_name, "induration", "0");
		for (dur_idx = 0; dur_idx < MAX_DURATION_PER_ENTRY; dur_idx++)
		{
			snprintf(attr_name, sizeof(attr_name), "%s%d", DURATION_START, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration->start[dur_idx] = atoi(node_val);
				if(duration->start[dur_idx] == 0)
				{
					duration->allday_flag++;
					tmpduridx = dur_idx;
				}
			}
			else
			{
				if(dur_idx == 0)
					duration->allday_flag++;
			}

			
			snprintf(attr_name, sizeof(attr_name), "%s%d", DURATION_END, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration->end[dur_idx] = atoi(node_val);
				if(tmpduridx == dur_idx && duration->end[dur_idx] == 0)
					duration->allday_flag++;
			}
			else
			{
				if(dur_idx == 0)
					duration->allday_flag++;
			}			

			snprintf(attr_name, sizeof(attr_name), "%s", DURATION_REPEAT);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val))
				&& node_val[0] != '\0' && strcmp(node_val, "0"))
			{
				duration->repeat_days = atoi(node_val);
			}
			else
			{
				duration->repeat_days = 127; /* 01111111 : every day in a week */
			}
		}
	}

	return 0;
}

int initdnsfilterduration(int idx)
{
	filter_duration_t *duration = NULL;
	char node_val[128] = {0};
	char attr_name[32], node_name[32];
	int dur_idx = 0, node_idx = 0;
	int maxDnsNum = MAX_DNSFILTER_NUM, minDnsNum = 0;

	if(idx == 0)
		bzero(dnsfilter_setting, sizeof(dnsfilter_setting));

	if(idx > 0)
	{
		maxDnsNum = idx;
		minDnsNum = idx - 1;
	}

	for (node_idx = minDnsNum; node_idx < maxDnsNum; node_idx++)
	{
		duration = &dnsfilter_setting[node_idx].duration;
		memset(duration, 0, sizeof(filter_duration_t));
		snprintf(node_name, sizeof(node_name), DNSFILTER_ENTRY_N_NODE, (node_idx+1));
		
		cfg_set_object_attr(node_name, "induration", "0");
		for (dur_idx = 0; dur_idx < MAX_DURATION_PER_ENTRY; dur_idx++)
		{
			snprintf(attr_name, sizeof(attr_name), "%s%d", DURATION_START, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration->start[dur_idx] = atoi(node_val);
				if(duration->start[dur_idx] == 0)
					duration->allday_flag++;				
			}
			else
			{
				if(dur_idx == 0)
					duration->allday_flag++;
			}
			
			snprintf(attr_name, sizeof(attr_name), "%s%d", DURATION_END, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration->end[dur_idx] = atoi(node_val);
				if(duration->end[dur_idx] == 0)
					duration->allday_flag++;
			}
			else
			{
				if(dur_idx == 0)
					duration->allday_flag++;
			}
			
			snprintf(attr_name, sizeof(attr_name), "%s", DURATION_REPEAT);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) 
				&& node_val[0] != '\0' && strcmp(node_val, "0"))
			{
				duration->repeat_days = atoi(node_val);
			}
			else
			{
				duration->repeat_days = 127; /* 01111111 : every day in a week */
			}
		}
	}

	return 0;
}
#endif

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
/*
idx: 0 ALL
1~  : each entry.
*/
int svc_wan_related_urlfilter_write(int idx)
#else
int svc_wan_related_urlfilter_write(void)
#endif
{
	int i=0,j=0;

	char nodeName[64] = {0};
	char temp[MAX_URL_INPUT_LENGTH*2+20]={0};
	char hostName[MAX_URL_INPUT_LENGTH*2+20]={0};
	char pattern[MAX_URL_INPUT_LENGTH*2+20]={0};
	char Activate[8] = {0};
	char URL[50] = {0};
	char list_type[4] = {0};
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
	char MacAddr[MAX_MAC_LEN] = {0};
#endif

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
	char chain_name[32] = {0};
	char cmd[128] = {0};
#endif

	unsigned char enable = 0;
	unsigned char mode = 0;
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
	urlfilter_ioctl_cfg_data data = {0};
	char Port[20] = {0};
	char induration[8] = {0};
	char urlnum_s[16] = {0};
	char attrURL[32] = {0};
	char attrPort[32] = {0};
	char mac_no_dot[16] = {0};
	int num = 0, urlnum_i = 0;
	urlfilter_obj_cfg_data *p_obj = NULL;
#else
	url_info_cfg urlInfo;
#endif
	int maxUrlRule = MAX_URL_RULE;
	int minUrlRule = 1;
	int len = 0;

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)		
	clearUrlfilterIptablesIdx(idx);
#else
#if defined(TCSUPPORT_CT_JOYME2)	
	clearUrlfilterIptablesIdx(0);
#endif
#endif

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
	if ( 0 == idx )
	{
		/* clear all rules */;
		blapi_system_url_filter_clear_all_rule();
	}
	else if ( idx > 0 )
	{
		minUrlRule = idx;
		maxUrlRule = idx;
	}
#else
	cfg_get_object_attr(URLFILTER_COMMON_NODE, "Filter_Policy", list_type, sizeof(list_type));
	if(!strncmp(list_type, "1", sizeof(list_type)))/*white list*/
	{	
		mode = 1;
	}else{
		mode = 0;
	}
#endif	
	/*do when url filter is activated*/
	cfg_get_object_attr(URLFILTER_COMMON_NODE, "Activate", Activate, sizeof(Activate));
	if(!strncmp(Activate,"1",sizeof(Activate)))
	{
		enable = 1;
		blapi_system_url_filter_set_enable((void *)&enable, sizeof(enable));
#if !defined(TCSUPPORT_CT_JOYME4) && !defined(TCSUPPORT_CT_UBUS)
		blapi_system_url_filter_set_mode((void *)&mode, sizeof(mode));
#endif

		for(i = minUrlRule; i <= maxUrlRule; i++){
			memset(pattern,0,sizeof(pattern));
			memset(hostName,0,sizeof(hostName));
			memset(temp,0,sizeof(temp));
			snprintf(nodeName,sizeof(nodeName), URLFILTER_ENTRY_N_NODE, i);
			if (cfg_query_object(nodeName,NULL,NULL) <= 0)
			{
				continue;
			}
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)			
			memset(chain_name, 0, sizeof(chain_name));
			snprintf(chain_name, sizeof(chain_name), "url_%d_https_filter", i - 1);
#endif

			/*get UrlFilter_Entry Activate*/
			memset(Activate, 0, sizeof(Activate));
			cfg_get_object_attr(nodeName, "Activate", Activate, sizeof(Activate));

#if !defined(TCSUPPORT_CT_JOYME4) && !defined(TCSUPPORT_CT_UBUS)
			if(Activate[0] == '\0')
			{
				/*we don't set rule if user does not set the Activate value*/
				continue;
			}
#endif
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
			if(idx == 0 && strcmp(Activate, "1"))
			{
				/*we don't set rule if Activate is not '1'*/
				continue;
			}
			
			/* is in timerange */
			memset(induration, 0, sizeof(induration));
			cfg_get_object_attr(nodeName, "induration", induration, sizeof(induration));
			if(idx == 0 && strcmp(induration, "1"))
			{
				printf("Entry_idx[%d] time not in range!!\n", i - 1);
				continue;
			}
			memset(list_type, 0, sizeof(list_type));			
			cfg_get_object_attr(nodeName, "Filter_Policy", list_type, sizeof(list_type));
			if(!strncmp(list_type, "1", sizeof(list_type)))/*white list*/
			{	
				mode = 1;
			}else{
				mode = 0;
			}	

			if(!strcmp(Activate, "1") && !strcmp(induration, "1"))
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "iptables -t filter -N %s", chain_name);
				system(cmd);
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ip6tables -t filter -N %s", chain_name);
				system(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "iptables -t filter -A FORWARD -j url_%d_https_filter", i - 1);
				system(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ip6tables -t filter -A FORWARD -j url_%d_https_filter", i - 1);
				system(cmd);
			}
			
			memset(urlnum_s ,0 ,sizeof(urlnum_s));
			urlnum_i = 0;
			if(0 < cfg_get_object_attr(nodeName, "url_num", urlnum_s, sizeof(urlnum_s)) && urlnum_s[0] != '\0')	
				sscanf(urlnum_s, "%d", &urlnum_i);

			memset(MacAddr, 0, sizeof(MacAddr));
			cfg_get_object_attr(nodeName, "MAC", MacAddr, sizeof(MacAddr));
	
			bzero(&data, sizeof(data));			
			if ( 0 != MacAddr[0] )
			{
				string_toUper(MacAddr);
				memset(mac_no_dot, 0, sizeof(mac_no_dot));
				mac_rm_dot(MacAddr, mac_no_dot);
				strncpy(MacAddr, mac_no_dot, sizeof(MacAddr) - 1);
				snprintf(data.mac_addr, sizeof(data.mac_addr), "%s", MacAddr);
			}
			cfg_get_object_attr(nodeName, "URL0", URL, sizeof(URL));
			if ( 0 == URL[0] )
				data.urlfilter_obj_cnt = urlnum_i;
			else
			{
#if !defined(TCSUPPORT_CT_UBUS)
				/* only URL0 can be "*" */
				if(!strcmp(URL, "*"))
					urlnum_i = 0;
			
				urlnum_i++;
#endif
				data.urlfilter_obj_cnt = urlnum_i;
			}
				
			if(strncmp(Activate,"1",sizeof(Activate)))
			{
				cfg_set_object_attr(nodeName, "dnsBlockedTimes", "0") ;
				data.clearbtflag = 1;
			}
			
			if ( data.urlfilter_obj_cnt )
			{
				data.obj = malloc(sizeof(urlfilter_obj_cfg_data) * data.urlfilter_obj_cnt);
				if ( NULL == data.obj )
					return -1;
				memset(data.obj, 0, sizeof(urlfilter_obj_cfg_data) * data.urlfilter_obj_cnt);
				p_obj = data.obj;
			}
			else
			{
				data.obj = malloc(sizeof(urlfilter_obj_cfg_data));
				if ( NULL == data.obj )
					return -1;
				memset(data.obj, 0, sizeof(urlfilter_obj_cfg_data));
				p_obj = data.obj;
				p_obj->index = i - 1;

			}
			if(!strcmp(Activate, "1") && !strcmp(induration, "1") && urlnum_i > 0)
			{
				for(num = 0; num < 100; num++)
			{
				p_obj->index = i - 1;
				memset(attrURL, 0, sizeof(attrURL));
				memset(URL, 0, sizeof(URL));
				memset(hostName, 0, sizeof(hostName));
				memset(temp, 0, sizeof(temp));
				snprintf(attrURL, sizeof(attrURL), "URL%d", num);
				cfg_get_object_attr(nodeName, attrURL, URL, sizeof(URL));
					if(URL[0] == 0)
						continue;
				memset(Port, 0, sizeof(Port));
				memset(attrPort, 0, sizeof(attrPort));
				snprintf(attrPort, sizeof(attrPort), "PORT%d", num);
				cfg_get_object_attr(nodeName, attrPort, Port, sizeof(Port));		
#else	
			/*get UrlFilter_Entry URL*/
			cfg_get_object_attr(nodeName, "URL", URL, sizeof(URL));
#endif
				
			if(URL[0] == '\0' || strlen(URL) == 0)
			{
				/*we don't set rule if user does not set the URL value*/
				continue;
			}
			/*48 is the longest string length on our web page*/
			else if(strlen(URL) > MAX_URL_INPUT_LENGTH)
			{
				continue;
			}
			/*write the rule into UrlFilter.sh when rule is activated*/
			else if(!strncmp(Activate,"1",sizeof(Activate)))
			{
				/*copy URL to temp*/
				strncpy(temp, URL, sizeof(temp)-1);

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)				
					if(strstr(URL, HTTPS))
					{
						p_obj ++;
						continue;
					}
#endif
				
				/*cut the "http://" string*/
				if(strstr(temp,HTTP))
				{
					if(strlen(temp) == strlen(HTTP))
					{
						printf("null host name\n");
						continue;
					}
					else{
						strncpy(temp, temp+strlen(HTTP), sizeof(temp)-1);
					}
				}
				len = strlen(temp);
				for(j=0; j<len; j++)
				{
					if(temp[j] == '/')
					{
						break;
					}
					snprintf(hostName + strlen(hostName), 
						sizeof(hostName) - strlen(hostName), "%c", temp[j]);
				}
				/*write the rule to Url.pat*/
				if(strlen(hostName) != 0)
				{
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
						p_obj->mode = mode;
						if ( 0 == strcmp(URL, "*") )
							p_obj->match_all = 1;
						else
							p_obj->match_all = 0;
						snprintf(p_obj->domain, sizeof(p_obj->domain), "%s", hostName); 	
						if ( 0 == Port[0] )
							p_obj->port = 80;
						else
							p_obj->port = atoi(Port);
	
						p_obj ++;			
#else
					memset(&urlInfo, 0, sizeof(urlInfo));
					urlInfo.index = i - 1;
					strncpy(urlInfo.host, hostName, sizeof(urlInfo.host) - 1);
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
					/* get UrlFilter_Entry MAC*/
					cfg_get_object_attr(nodeName, "MAC", MacAddr, sizeof(MacAddr));
					strncpy(urlInfo.MacAddr, MacAddr, sizeof(urlInfo.MacAddr) - 1);
#endif
					blapi_system_url_filter_set_url_host(&urlInfo);	
					if((j<len) && temp[j] == '/')
					{
						strncpy(temp, &temp[j], sizeof(temp)-1);
						memset(&urlInfo, 0, sizeof(urlInfo));
						urlInfo.index = i - 1;
						strncpy(urlInfo.path, temp, sizeof(urlInfo.path) - 1);
						blapi_system_url_filter_set_url_path(&urlInfo);
					}
#endif
				}
				else
				{
					printf("host name is null, the url is wrong!!\n");
				}

			}
			/*if the Activate value is neither 1 nor 0, the setting is wrong*/
			else if(strncmp(Activate,"0",sizeof(Activate)))
			{
				printf("Error--Wrong setting about UrlFilter_Entry Activate value. It can only be 1 or 0.\n");
			}

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
					if(urlnum_i == num + 1)
						break;
				}
			}
			
			if ( 0 == MacAddr[0] ) /* global setting. */
			{		
				blapi_system_url_filter_clear_global_setting(&data);
				if(!strcmp(induration, "1") && !strcmp(Activate, "1"))
				{
					blapi_system_url_filter_add_url_rule(&data);
		}
	}
	else
	{
				blapi_system_url_filter_clear_all_basicmac_rule(&data);
				if(!strcmp(induration, "1") && !strcmp(Activate, "1"))
				{
					blapi_system_url_filter_add_url_rule(&data);
				}
				}

			if ( data.obj )
			{
				free(data.obj);
			}
#endif
		}
	}
	else
	{
		enable = 0;
		blapi_system_url_filter_set_enable((void*)&enable, sizeof(enable));
	}
	return SUCCESS;
}


int svc_wan_related_urlfilter_boot(void)
{
	unsigned int new_filter_state=0;
	FILE *startupSh=NULL;
	char nodePath[64] = {0};
	char newFilterState[32] = {0};

	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
	if(url_check_filter())/*filter on*/
	{
		new_filter_state=atoi(newFilterState) | URL_VECTOR;
	}
	else/*filter down*/
	{
		new_filter_state=atoi(newFilterState) & (~URL_VECTOR);
	}
	check_and_set_l7filter(new_filter_state);
	
	check_and_set_filter(new_filter_state);
		
	startupSh=fopen("/tmp/etc/UrlFilter.sh","r");
	if(startupSh)
	{
		fclose(startupSh);
		system("chmod 755 /tmp/etc/UrlFilter.sh");
		system("/tmp/etc/UrlFilter.sh");
	}

#if defined(TCSUPPORT_CT_DNSBIND)
	cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1") ;
#endif
	return 0;
}

#if defined(TCSUPPORT_CT_UBUS)
int svc_wan_related_dnsfilter_write()
{
	urlfilter_ioctl_cfg_data data;
	dnsfilter_ioctl_cfg_lanip_t lan_data;
	urlfilter_obj_cfg_data *p_obj = NULL;
	int i = 0;
	char nodeName[32] = {0};
	char hostName[256] = {0};
	char attrName[32] = {0};
	char Enable[4] = {0}; 
	char MacAddr[18] = {0};
	char Mode[8] = {0};
	char induration[8] = {0};
	char blockTimes[64] = {0};
	int blocktimes = 0;	
	char hostnamenum_s[8] = {0};
	int hostnamenum_i = 0;
	char action_s[8] = {0};
	int action_i = 0;
	int mode = 0;
	int j = 0; 
	int white_polic_flag = 0;
	char mac_no_dot[16] = {0};
	char lan_ip[16] = {0}, lan_ip6[48] = {0};

	/* clear all rules */;
	blapi_system_dns_filter_clear_all_rule();

	/* update lanip */
	bzero(&lan_data, sizeof(lan_data));
	memset(lan_ip, 0, sizeof(lan_ip));
	memset(lan_ip6, 0, sizeof(lan_ip6));
	if ( cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", lan_ip, sizeof(lan_ip)) > 0 && 0 != lan_ip[0] )
		strncpy(lan_data.lanip, lan_ip, sizeof(lan_data.lanip) - 1);
	else
		strcpy(lan_data.lanip, "192.168.2.1");
	
	if ( cfg_get_object_attr(LAN_ENTRY0_NODE, "IP6", lan_ip6, sizeof(lan_ip6)) > 0 && 0 != lan_ip6[0] )
		strncpy(lan_data.lanip6, lan_ip6, sizeof(lan_data.lanip6) - 1);
	else
		strcpy(lan_data.lanip6, "fe80::1");
	
	blapi_system_dns_filter_set_lanip(&lan_data);

	/* max 32 entry */
	for ( i = 0; i < 32; i++ )
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DNSFILTER_ENTRY_N_NODE, i + 1);
		if ( cfg_get_object_attr(nodeName, "Enable", Enable, sizeof(Enable)) < 0 
			|| 0 == Enable[0] 
			|| 0 == strcmp(Enable, "0") )
			continue;

		memset(Mode, 0, sizeof(Mode));
		if ( cfg_get_object_attr(nodeName, "Filter_Policy", Mode, sizeof(Mode)) > 0 && 0 != Mode[0] )
			mode = atoi(Mode);
		else
			mode = 0;

		memset(action_s, 0, sizeof(action_s));
		if ( cfg_get_object_attr(nodeName, "Action", action_s, sizeof(action_s)) > 0 && 0 != action_s[0] )
			action_i = atoi(action_s);
		else
			action_i = 0;

		memset(blockTimes, 0, sizeof(blockTimes));
		blocktimes = 0;
		if ( cfg_get_object_attr(nodeName, "dnsBlockedTimes", blockTimes, sizeof(blockTimes)) > 0 && 0 != blockTimes[0] )
			blocktimes = atoi(blockTimes);

		memset(hostnamenum_s, 0, sizeof(hostnamenum_s));
		if ( cfg_get_object_attr(nodeName, "hostname_num", hostnamenum_s, sizeof(hostnamenum_s)) > 0 && 0 != hostnamenum_s[0] )
			hostnamenum_i = atoi(hostnamenum_s);
		else
			continue;

		if ( 0 == hostnamenum_i )
			continue;

		memset(induration, 0, sizeof(induration));
		cfg_get_object_attr(nodeName, "induration", induration, sizeof(induration));
		
		if( 0 != strcmp(induration, "1") )
		{
			/* whitelist enable but not in duration: block all
			   blacklist enable but not in duration: disable */
			if ( mode == 1 )
			{
				mode = 0;
				white_polic_flag = 1;
				hostnamenum_i = 1;
			}
			else
				continue;
		}

		memset(MacAddr, 0, sizeof(MacAddr));
		cfg_get_object_attr(nodeName, "MAC", MacAddr, sizeof(MacAddr));
		if ( 0 != MacAddr[0] )
		{
			string_toUper(MacAddr);
			memset(mac_no_dot, 0, sizeof(mac_no_dot));
			mac_rm_dot(MacAddr, mac_no_dot);
			strncpy(MacAddr, mac_no_dot, sizeof(MacAddr) - 1);
		}

		bzero(&data, sizeof(data));
		strncpy(data.mac_addr, MacAddr, sizeof(data.mac_addr) - 1);
		data.urlfilter_obj_cnt = hostnamenum_i;

		if ( data.urlfilter_obj_cnt )
		{
			data.obj = malloc(sizeof(urlfilter_obj_cfg_data) * data.urlfilter_obj_cnt);
			if ( NULL == data.obj )
				return -1;
			
			memset(data.obj, 0, sizeof(urlfilter_obj_cfg_data) * data.urlfilter_obj_cnt);
			p_obj = data.obj;
		}
		else
			continue;

		/* each entry's has max 32 hostname */
		for ( j = 0; j < hostnamenum_i; j++ )
		{	
			memset(attrName, 0, sizeof(attrName));
			snprintf(attrName, sizeof(attrName), "HostName_new%d", j);
			memset(hostName, 0, sizeof(hostName));
			cfg_get_object_attr(nodeName, attrName, hostName, sizeof(hostName));
			if ( 0 == hostName[0] )
				continue;

			if ( NULL != strstr(hostName, "https://") )
				strncpy(p_obj->domain, hostName + strlen("https://"), sizeof(p_obj->domain) - 1);
			else if ( NULL != strstr(hostName, "http://") )
				strncpy(p_obj->domain, hostName + strlen("http://"), sizeof(p_obj->domain) - 1);
			else
				strncpy(p_obj->domain, hostName, sizeof(hostName) - 1);

			p_obj->mode = mode;
			p_obj->action = action_i;
			p_obj->times = blocktimes;
			
			if ( white_polic_flag )
				p_obj->match_all = 1;
			else
				p_obj->match_all = 0;			

			p_obj->index = i;
			p_obj++;
		}

		if ( 0 == strcmp(induration, "1") && 0 == strcmp(Enable, "1") )
		{
			blapi_system_dns_filter_add_dns_rule(&data);
		}
		
		if ( data.obj )
		{
			free(data.obj);
		}
	}
	
	return 0;
}
#endif

int svc_wan_related_ipmacfilter_boot(void)
{
	char MACFilterInputTable[64] = {0};
	char ebtablesCmd[128] = {0};
	char ipv4FilterInputTable[128] = {0};
	unsigned int new_filter_state=0;
	char nodePath[64] = {0};
	char newFilterState[32] = {0};
	
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
	if(ipfilter_check_filter())/*filter on*/
	{
		new_filter_state=atoi(newFilterState) | IPFILTER_VECTOR;
	}
	else/*filter down*/
	{
		new_filter_state=atoi(newFilterState) & (~IPFILTER_VECTOR);
	}
	
	check_and_set_filter(new_filter_state);
	
	strncpy(MACFilterInputTable, "ebmacfilter", sizeof(MACFilterInputTable)-1);
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -N %s 2>/dev/null", MACFilterInputTable);
	system(ebtablesCmd);

	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -P %s RETURN", MACFilterInputTable);
	system(ebtablesCmd);

	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -A INPUT -j %s", MACFilterInputTable);
	system(ebtablesCmd);
	
	strncpy(ipv4FilterInputTable, "ipv4upfilter", sizeof(ipv4FilterInputTable)-1);
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -N %s 2>/dev/null", ipv4FilterInputTable);
	system(ebtablesCmd);
	
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -P %s RETURN", ipv4FilterInputTable);
	system(ebtablesCmd);
	
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -A FORWARD -j %s", ipv4FilterInputTable);
	system(ebtablesCmd);
	
	memset(ipv4FilterInputTable, 0, sizeof(ipv4FilterInputTable));
	strncpy(ipv4FilterInputTable, "ipv4downfilter", sizeof(ipv4FilterInputTable)-1);
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -N %s 2>/dev/null", ipv4FilterInputTable);
	system(ebtablesCmd);
	
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -P %s RETURN", ipv4FilterInputTable);
	system(ebtablesCmd);
	
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -A FORWARD -j %s", ipv4FilterInputTable);
	system(ebtablesCmd);	

	strncpy(MACFilterInputTable, "ebmacfwfilter", sizeof(MACFilterInputTable)-1);
	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -N %s 2>/dev/null", MACFilterInputTable);
	system(ebtablesCmd);

	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -P %s RETURN", MACFilterInputTable);
	system(ebtablesCmd);

	memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -t filter -A FORWARD -j %s", MACFilterInputTable);
	system(ebtablesCmd);
	
	system(MACFILTER_SH);
	system(IPUPFILTER_SH);
	system(IPDOWNFILTER_SH);

	return 0;
}

int get_def_wan_v4_itf(char *if_name, int itf_len)
{
	char node_name[32] = {0};
	char def_route_wan_idx[16] = {0};

	if (if_name == NULL)
		return -1;
	
	/*Get default route wan interface name*/
	memset(def_route_wan_idx, 0, sizeof(def_route_wan_idx));
	if (cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", 0, def_route_wan_idx, sizeof(def_route_wan_idx)) < 0) {
		return -1;
	}

	if (0 == strcmp(def_route_wan_idx, "N/A"))
		return -1;

	snprintf(node_name, sizeof(node_name), WAN_PVC_ENTRY_NODE, 
		atoi(def_route_wan_idx) / 8 + 1, atoi(def_route_wan_idx) % 8 + 1);
	if(cfg_obj_get_object_attr(node_name, "IFName", 0, if_name, itf_len) <= 0){
		return -1;
	}

	if ( 0 == strcmp(if_name, "N/A") )
		return -1;

	return 0;
}

int get_def_wan_v6_itf(char *if_name, int itf_len)
{
	char node_name[32] = {0};
	char def_route_wan_idx[16] = {0};

	if (if_name == NULL)
		return -1;
	
	/*Get default route wan interface name*/
	memset(def_route_wan_idx, 0, sizeof(def_route_wan_idx));
	if (cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", 0, def_route_wan_idx, sizeof(def_route_wan_idx)) < 0) {
		return -1;
	}

	if (0 == strcmp(def_route_wan_idx, "N/A"))
		return -1;

	snprintf(node_name, sizeof(node_name), WAN_PVC_ENTRY_NODE, 
		atoi(def_route_wan_idx) / 8 + 1, atoi(def_route_wan_idx) % 8 + 1);
	if(cfg_obj_get_object_attr(node_name, "IFName", 0, if_name, itf_len) <= 0){
		return -1;
	}

	if ( 0 == strcmp(if_name, "N/A") )
		return -1;

	return 0;
}

int svc_wan_related_igmpproxy_write(void)
{
	char nodeName[128] 	= {0};
	char quickleave[5] 	= {0};
	char upstreamif[16] = {0};
	char buf[128]		= {0};
	FILE *fp 			= NULL;

  	snprintf(nodeName, sizeof(nodeName), IGMPPROXY_ENTRY_NODE);
#if !defined(TCSUPPORT_CT_CONGW_CH) 
	if(cfg_get_object_attr(nodeName, "UpstreamIF", upstreamif, sizeof(upstreamif)) > 0)
#endif
	{
		fp = fopen(IGMPPROXY_CONF_PATH, "w");
		if(NULL == fp){
			return FAIL;
		}

		if (cfg_get_object_attr(nodeName, "QuickLeave", quickleave, sizeof(quickleave)) <= 0){
			strcpy(quickleave, "Yes");
		}
			
#if defined(TCSUPPORT_IGMP_PROXY_V3)
		/*igmpproxy.conf sample
# Global igmp configurationigmp 
		robustness-variable 		2
		igmp query-interval 			 70
		igmp query-response-interval	 10
		igmp last-member-query-interval  1
		igmp unsolicited-report-interval 1
		igmp ssm-enable 				 true
##add interfaces -- as available in the system.# #
		interface br0 
		interface nas2 upstream
		*/
		char robustness[5] = {0}, query_inter[5] = {0}, query_response_inter[5] = {0};
		char last_query_inter[5] = {0}, unsolicited_report_inter[5] = {0}, ssm_en[8] = {0};

		cfg_get_object_attr("root.IGMPproxy.Entry", "Robustness", robustness, sizeof(robustness));
		cfg_get_object_attr("root.IGMPproxy.Entry", "QueryInterval", query_inter, sizeof(query_inter));
		cfg_get_object_attr("root.IGMPproxy.Entry", "QueryResInterval", query_response_inter, sizeof(query_response_inter));
		cfg_get_object_attr("root.IGMPproxy.Entry", "LastMemQueryInterval", last_query_inter, sizeof(last_query_inter));
		cfg_get_object_attr("root.IGMPproxy.Entry", "UnsolicitedReportInterval", unsolicited_report_inter, sizeof(unsolicited_report_inter));
		cfg_get_object_attr("root.IGMPproxy.Entry", "SSMEnable", ssm_en, sizeof(ssm_en));

		if(strlen(robustness) > 0){
			sprintf(buf,"igmp robustness-variable %s\n", robustness);
			fputs_escape(buf, fp);
		}
		if(strlen(query_inter) > 0){
			sprintf(buf,"igmp query-interval %s\n", query_inter);
			fputs_escape(buf, fp);
		}
		if(strlen(query_response_inter) > 0){
			sprintf(buf,"igmp query-response-interval %s\n", query_response_inter);
			fputs_escape(buf, fp);
		}
		if(strlen(last_query_inter) > 0){
			sprintf(buf,"igmp last-member-query-interval %s\n", last_query_inter);
			fputs_escape(buf, fp);
		}
		if(strlen(unsolicited_report_inter) > 0){
			sprintf(buf,"igmp unsolicited-report-interval %s\n", unsolicited_report_inter);
			fputs_escape(buf, fp);
		}
		if(strlen(ssm_en) > 0){
			sprintf(buf,"igmp ssm-enable %s\n", ssm_en);
			fputs_escape(buf, fp);
		}

#endif
			
		/*pass condition 1~4, then generate igmpproxy.conf according to isp, quickleave, pvc*/
		if(0 == strcmp(quickleave,"Yes"))
			snprintf(buf, sizeof(buf), "quickleave\n");
		else{
			snprintf(buf, sizeof(buf), "#quickleave\n");
		}
		fputs(buf, fp);		
		
#if defined(TCSUPPORT_IGMP_PROXY_V3)
		sprintf(buf,"interface br0\n");
		fputs(buf, fp);
		sprintf(buf,"interface %s upstream\n",upstreamif);
		fputs(buf, fp);
#else
		snprintf(buf, sizeof(buf), "phyint %s upstream  ratelimit 0  threshold 1\n", upstreamif);		
		fputs_escape(buf, fp);
		snprintf(buf, sizeof(buf), "phyint br0 downstream  ratelimit 0  threshold 1 \n");
		fputs(buf, fp);
#endif
		fclose(fp);

	}
	return SUCCESS;
}/* end dproxy_write */

int svc_wan_related_igmpproxy_execute(void)
{
	char nodeName[128] 	= {0};
	char active[5] 		= {0};
	char upstreamif[16] = {0};
	char wan_ip[19] 	= {0};
	char lan_ip[19] 	= {0};
	char isp[3] 		= {0};
	int wan_if_index 	= 0;

	/*kill igmpproxy process*/
	system("killall igmpproxy");
	printf("IGMP proxy process down!\n");

	snprintf(nodeName, sizeof(nodeName), IGMPPROXY_ENTRY_NODE);
	if(cfg_get_object_attr(nodeName, "Active", active, sizeof(active)) > 0 &&
		cfg_get_object_attr(nodeName, "UpstreamIF", upstreamif, sizeof(upstreamif)) > 0){
		if(!strcmp(active, "Yes")){
#if defined(TCSUPPORT_CMCCV2)
			#ifdef TCSUPPORT_SNOOPING_SEPERATION
			system("brctl igmpsnoop br0 on");
			#else
			system("brctl snoop br0 on");			
			#endif
#endif
			/*Check wan interface whether get IP*/
			if((wan_if_index = get_wanindex_by_name(upstreamif)) < 0){
				printf("%s:get wan index failed\n", __func__);
				return FAIL;
			}
#if defined(TCSUPPORT_CT_JOYME4)			
			cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "DefaultWAN", upstreamif);
#endif			
			if(get_waninfo_by_index(wan_if_index, "ISP", isp, sizeof(isp)) != SUCCESS){
				printf("%s:get wan ISP failed\n", __func__);
				return FAIL;
			}

			if(!strcmp(isp, "3")){/*BRIDGE_MODE*/
				printf("%s:upstream interface %s is bridge mode, igmpproxy will not be start\n", __func__, upstreamif);
				return FAIL;
			}						
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, wan_if_index + 1);
			cfg_get_object_attr(nodeName, "IP", wan_ip, sizeof(wan_ip));
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), LAN_ENTRY0_NODE); 
			cfg_get_object_attr(nodeName, "IP", lan_ip, sizeof(lan_ip));
			if(strlen(wan_ip) != 0 && strlen(lan_ip) != 0){ 							
#if defined(TCSUPPORT_IGMP_PROXY_V3)
				system("/userfs/bin/igmpproxy -c /etc/igmpproxy.conf -d");
				printf("IGMP proxy V3 process on!\n");
#else
				system(IGMPPROXY_DEA_PATH);
				printf("IGMP proxy process on!\n");
#endif
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "proxyActive", "Yes");
			}
		}	
	}
	
	return SUCCESS;
}

int svc_wan_related_mldproxy_execute(void)
{
	char nodeName[128] 	= {0};
	char active[5] 		= {0};
	char upstreamif[16] = {0};
	char status6[6] 	= {0};
	char isp[3] 		= {0};
	char buf[32]		= {0};
#ifdef TCSUPPORT_CUC
	char ServiceList[48]= {0};
#endif
	int wan_if_index 	= 0;
	
	/*kill mld proxy process*/
	system("killall ecmh");
	system("echo 6 > /proc/tc3162/hwnat_off");	//only clean multicast hwnat entry
	system("echo 0 > /proc/tc3162/hwnat_off");
	printf("MLD proxy process down!\n");

	snprintf(nodeName, sizeof(nodeName), MLDPROXY_ENTRY_NODE);
	if ( cfg_get_object_attr(nodeName, "Active", active, sizeof(active)) > 0 && 
#if !defined(TCSUPPORT_CT_CONGW_CH) 
		cfg_get_object_attr(nodeName, "UpstreamIF", upstreamif, sizeof(upstreamif)) > 0 ) 
#endif
	{
		if(!strcmp(active, "Yes")){
#if defined(TCSUPPORT_CMCCV2)			
			#ifdef TCSUPPORT_SNOOPING_SEPERATION
			system("brctl mldsnoop br0 on");
			#else
			system("brctl snoop br0 on");			
			#endif
#endif
			/*Check wan interface whether get IP*/
			if((wan_if_index = get_wanindex_by_name(upstreamif)) < 0){
				printf("%s:get wan index failed\n", __func__);
				return FAIL;
			}
#if defined(TCSUPPORT_CT_JOYME4)			
			cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "DefaultWAN", upstreamif);
#endif
			if(get_waninfo_by_index(wan_if_index, "ISP", isp, sizeof(isp)) != SUCCESS){
				printf("%s:get wan ISP failed\n", __func__);
				return FAIL;
			}

			if(!strcmp(isp, "3")){/*BRIDGE_MODE*/
				printf("%s:upstream interface %s is bridge mode, mldproxy will not be start\n", __func__, upstreamif);
				return FAIL;
			}
			
#ifdef TCSUPPORT_CUC 
			/* only internet and IPTV wan can be used to be igmp/mld proxy*/
			if(get_waninfo_by_index(wan_if_index, "ServiceList", ServiceList, sizeof(ServiceList)) != SUCCESS){
				printf("%s:get wan ServiceList failed\n", __func__);
				return FAIL;
			}

			if(NULL == strstr(ServiceList, "IPTV") && NULL == strstr(ServiceList, "INTERNET")){
				printf("%s:upstream interface %s is not for internet nor for IPTV, mldproxy will not be start\n", __func__, upstreamif);
				return FAIL;
			}
#endif
			
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, wan_if_index+1);
			cfg_get_object_attr(nodeName, "Status6", status6, sizeof(status6));

			if(!strcmp(status6, "up")){
				sprintf(buf,"/userfs/bin/ecmh -i %s"
#ifdef TCSUPPORT_IGMP_QUICK_LEAVE
							" -1 -Q"
#endif
					,upstreamif); 
				system_escape(buf);
				printf("MLD proxy process on!\n");
			}
		}
	}

	return SUCCESS;
}

static void add_DDNS_CommonInfo(int index)
{
	char Entry_index[3]={0};
	char nodeName[128]={0};
	char value[32] = {0};
	
	snprintf( Entry_index, sizeof(Entry_index), "%d", index);
	snprintf(nodeName, sizeof(nodeName), DDNS_COMMON_NODE);
	if(index < 40){
		cfg_get_object_attr(nodeName, "Entrylist1", value, sizeof(value));
		strcat(value, Entry_index);
		strcat(value, " ");
		cfg_set_object_attr(nodeName, "Entrylist1", value);
	}
	else{
		cfg_get_object_attr(nodeName, "Entrylist2", value, sizeof(value));
		strcat(value, Entry_index);
		strcat(value, " ");
		cfg_set_object_attr(nodeName, "Entrylist2", value);
	}
	return;
}

int oray_ddns_conf(char *nodeName)
{
	char dtmp[32] 			= {0};
	char dactbuf[32] 		= {0};
	char dfpbuf[32] 			= {0};
	FILE *dorayFP 			= NULL;
	char dbuf[256]			= {0};
	char WanInfoNode[128]	= {0};
	char if_name[32] = {0};
	int entry_idx = -1;

	if(cfg_obj_query_object(nodeName,NULL,NULL) > 0)
			add_DDNS_CommonInfo(0);

	if (get_def_wan_v4_itf(if_name, sizeof(if_name)) == -1){
		cfg_set_object_attr(nodeName, "DDNSStatus", "1");
		return FAIL;
	}
	
	memset(dtmp, 0, sizeof(dtmp));
	if((cfg_get_object_attr(nodeName, "DDNS_Name", dtmp, sizeof(dtmp)) < 0)
		|| dtmp[0] == '\0')
	{
		cfg_set_object_attr(nodeName, "DDNS_Name", "ORAY");
		strncpy(dtmp, "ORAY", sizeof(dtmp) - 1);
	}
	
	if (0 == strcmp(dtmp, "ORAY")){
		cfg_get_object_attr(nodeName, "Active", dactbuf, sizeof(dactbuf));
	
		if (0 == strcmp(dactbuf, "Yes")){
			dorayFP = fopen("/etc/phlinux.conf", "w+"); 
			if (dorayFP == NULL){				
				printf("\r\n@@ddns_write:Fail to open file %s in %s %s %d\n", "/etc/phlinux.conf", __FUNCTION__,__FILE__, __LINE__);
				return FAIL;
			}
			fputs("[settings]\n", dorayFP);	
			fputs("szHost = Phlinux3.Oray.Net\r\n", dorayFP);	
			if(cfg_get_object_attr(nodeName, "DDNS_Domain", dfpbuf, sizeof(dfpbuf)) <= 0 || dfpbuf[0] == '\0')
			{
				cfg_set_object_attr(nodeName, "DDNSStatus", "7");
				cfg_set_object_attr(nodeName, "DDNSErrMsg", "");
			}		
			/*snprintf(buf, sizeof(buf), "szHost = %s\r\n", fpbuf); 
			fputs_escape(buf, orayFP);*/
			
			memset(dfpbuf, 0, sizeof(dfpbuf));			
			memset(dbuf, 0, sizeof(dbuf));
			if(cfg_get_object_attr(nodeName, "USERNAME", dfpbuf, sizeof(dfpbuf)) <= 0 || dfpbuf[0] == '\0')
			{
				cfg_set_object_attr(nodeName, "DDNSStatus", "5");
				cfg_set_object_attr(nodeName, "DDNSErrMsg", "");
				/*cfg_set_object_attr(nodeName, "Active", "No");*/
				fclose(dorayFP);
				unlink("/etc/phlinux.conf");
				return FAIL;
			}
			snprintf(dbuf, sizeof(dbuf), "szUserID = %s\r\n", dfpbuf); 		
			fputs_escape(dbuf, dorayFP);	

			memset(dfpbuf, 0, sizeof(dfpbuf));			
			memset(dbuf, 0, sizeof(dbuf));				
			if(cfg_get_object_attr(nodeName, "PASSWORD", dfpbuf, sizeof(dfpbuf)) <= 0 || dfpbuf[0] == '\0')
			{
				cfg_set_object_attr(nodeName, "DDNSStatus", "6");
				cfg_set_object_attr(nodeName, "DDNSErrMsg", "");
				/*cfg_set_object_attr(nodeName, "Active", "No");*/
				fclose(dorayFP);
				unlink("/etc/phlinux.conf");
				return FAIL;
			}
			snprintf(dbuf, sizeof(dbuf), "szUserPWD = %s\r\n",dfpbuf); 		
			fputs_escape(dbuf, dorayFP);	

		/*
			memset(dfpbuf, 0, sizeof(dfpbuf));			
			memset(dbuf, 0, sizeof(dbuf));			
			cfg_get_object_attr(nodeName, "DDNS_Interface", dfpbuf, sizeof(dfpbuf));
			snprintf(dbuf, sizeof(dbuf), "nicName = %s\r\n", dfpbuf);			
			fputs_escape(dbuf, dorayFP);	
		*/
			snprintf(dbuf, sizeof(dbuf), "nicName = %s\r\n", if_name);	
			cfg_set_object_attr(nodeName, "DDNS_Interface", if_name);
			fputs_escape(dbuf, dorayFP);	

			/* get WanInfo entry index by IFName */
			memset(WanInfoNode, 0, sizeof(WanInfoNode));
			entry_idx = get_wanindex_by_name(if_name);
			if(entry_idx < 0){
				tcdbg_printf("\r\n[%s:%s:%d]get_wanindex_by_name fail!\n",  __FUNCTION__,__FILE__, __LINE__);				
				fclose(dorayFP);	
				return FAIL;
			}
			snprintf(WanInfoNode, sizeof(WanInfoNode), WANINFO_ENTRY_NODE, entry_idx + 1);			
			cfg_get_object_attr(WanInfoNode, "IP", dfpbuf, sizeof(dfpbuf));
			cfg_set_object_attr(nodeName, "DDNSIP", dfpbuf);

			memset(dbuf, 0, sizeof(dbuf));
			snprintf(dbuf, sizeof(dbuf), "szLog = %s\r\n", "/var/log/phddns.log");			
			fputs(dbuf, dorayFP);
			fclose(dorayFP); 	

			/* success */
			/*ret = 0;*/
			
			memset(dfpbuf, 0, sizeof(dfpbuf));
			cfg_get_object_attr(nodeName, "SERVICEPORT", dfpbuf, sizeof(dfpbuf));
			if(0 == strcmp(dfpbuf,""))
			{
				cfg_set_object_attr(nodeName, "DDNSStatus", "2");
				cfg_set_object_attr(nodeName, "DDNSErrMsg", "");
			}

			memset(dfpbuf, 0, sizeof(dfpbuf));
			cfg_get_object_attr(nodeName, "MYHOST", dfpbuf, sizeof(dfpbuf));
			if(0 == strcmp(dfpbuf,""))
			{
				cfg_set_object_attr(nodeName, "DDNSStatus", "8");
				cfg_set_object_attr(nodeName, "DDNSErrMsg", "");
			}
			/*add_DDNS_CommonInfo(0);*/
		}
	}
	
	return 0;
}

int svc_wan_related_ddns_write(void)
{
	int j = 0;
	int flag = 1;
	FILE *fp				= NULL;
	char nodeName[128]		= {0};
#if defined(TCSUPPORT_CT_JOYME)
	char tmp[32] 			= {0};
	char actbuf[32] 		= {0};
	char fpbuf[32] 			= {0};
	FILE *orayFP 			= NULL;
	char buf[256]			= {0};
#endif

#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	char srcNode[128] 		= {0};
	char dstNode[128] 		= {0};
	int setindex 			= -1;
#endif

#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	char value[16]			= {0};
#endif

	snprintf(nodeName, sizeof(nodeName), DDNS_COMMON_NODE);
	cfg_set_object_attr(nodeName, "Entrylist1", "");
	cfg_set_object_attr(nodeName, "Entrylist2", "");

#if defined(TCSUPPORT_CT_DBUS)
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), DDNS_ENTRY_NODE, 1);
	return oray_ddns_conf(nodeName);
#else
	char useOray[16]= {0};
	if(cfg_get_object_attr(DDNS_COMMON_NODE, "useOray", useOray, sizeof(useOray)) > 0 && !strcmp(useOray, "Yes"))
	{
		return oray_ddns_conf(DDNS_ENTRY_1_NODE);
	}

#endif
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	cfg_get_object_attr(nodeName, "tr69Commit", value, sizeof(value));
	if(0 == strcmp(value, "1")){		
		cfg_get_object_attr(nodeName, "tr69SetEntry", value, sizeof(value));
		setindex = atoi(value);
		if(setindex >= 0 && setindex < MAX_DDNS_NUM){
			memset(dstNode, 0, sizeof(dstNode));
			snprintf(dstNode, sizeof(dstNode), DDNS_ENTRY_NODE, setindex+1);	
			snprintf(srcNode, sizeof(srcNode), DDNS_TR69ENTRY_NODE);
			cfg_obj_copy_private(dstNode, srcNode);
		}
		cfg_set_object_attr(nodeName, "tr69Commit", "");
	}	
#endif
	
	fp = fopen(DDNS_PATH, "w");
	if(NULL == fp){
		printf("Fail to open file %s in %s %s %d\n", DDNS_PATH, __FUNCTION__,__FILE__, __LINE__);
		return FAIL;
	}

	for(j = 0; j < MAX_DDNS_NUM; j++){	
#if defined(TCSUPPORT_CT_JOYME)
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), DDNS_ENTRY_NODE, j+1);
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(nodeName, "type", value, sizeof(value));
		
		if (0 == strcmp(value, "plugin")){
			cfg_get_object_attr(nodeName, "DDNS_Name", tmp, sizeof(tmp));	
			if (0 == strcmp(tmp, "ORAY")){
				cfg_get_object_attr(nodeName, "Active", actbuf, sizeof(actbuf));
				if (0 == strcmp(actbuf, "Yes")){
					orayFP = fopen(ORAY_CONFIG_PATH, "w+"); 			
					if (orayFP == NULL){				
						printf("\r\n@@ddns_write:Fail to open file %s in %s %s %d\n", ORAY_CONFIG_PATH, __FUNCTION__,__FILE__, __LINE__); 
						break;				
					}	
					
					fputs("[settings]\n", orayFP);			
					fputs("szHost = Phlinux3.Oray.Net\r\n", orayFP);						
					cfg_get_object_attr(nodeName, "USERNAME", fpbuf, sizeof(fpbuf)); 		
					snprintf(buf, sizeof(buf), "szUserID = %s\r\n", fpbuf);			
					fputs_escape(buf, orayFP);								
					memset(fpbuf, 0, sizeof(fpbuf));			
					memset(buf, 0, sizeof(buf));				
					cfg_get_object_attr(nodeName, "PASSWORD", fpbuf, sizeof(fpbuf)); 			
					snprintf(buf, sizeof(buf), "szUserPWD = %s\r\n",fpbuf);			
					fputs_escape(buf, orayFP);				
					memset(fpbuf, 0, sizeof(fpbuf));			
					memset(buf, 0, sizeof(buf));			
					cfg_get_object_attr(nodeName, "DDNS_Interface", fpbuf, sizeof(fpbuf));			
					snprintf(buf, sizeof(buf), "nicName = %s\r\n", fpbuf); 			
					fputs_escape(buf, orayFP);			
					memset(buf, 0, sizeof(buf));			
					snprintf(buf, sizeof(buf), "szLog = %s\r\n", ORAY_LOG_PATH);			
					fputs(buf, orayFP); 			
					fclose(orayFP); 					
				}
			}
		}
		else
		{
			if(cfg_query_object(nodeName,NULL,NULL) > 0)
			{
				/*tcdbg_printf("cfg_type_query(%s) success,flag = 0\n",nodeName);*/
				flag = 0;
				if(cfg_save_attrs_to_file_fp(nodeName, fp, NO_QMARKS) < 0)
				{
					fclose(fp);
						return -1;
				}

				fputs(END_OF_ENTRY, fp);
				add_DDNS_CommonInfo(j);
			}
		}
#else
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), DDNS_ENTRY_NODE, j+1);
		if(cfg_obj_query_object(nodeName,NULL,NULL) > 0)
		{
			/*tcdbg_printf("cfg_type_query(%s) success,flag = 0\n",nodeName);*/
			flag = 0;
			if(cfg_save_attrs_to_file_fp(nodeName, fp, NO_QMARKS) < 0)
			{
				fclose(fp);
				return -1;
			}

			fputs(END_OF_ENTRY, fp);
			add_DDNS_CommonInfo(j);
		}
#endif
	}
	
	fclose(fp);
	
	if(flag){
		unlink(DDNS_PATH);
	}	

	return SUCCESS;
}

int start_ezipupdate_process(void)
{
	char cmd[128] = {0};
	snprintf(cmd, sizeof(cmd), "/userfs/bin/ez-ipupdate -d -F %s -P 60", DDNS_PID_PATH );
	system(cmd);
	
	return SUCCESS;
}

int start_phddns_process(void)
{
	char dtmp[32] 		= {0};
	char dprobuf[32] 	= {0};
	char dnodeName[128]	= {0};
	FILE *fp			= NULL;
	char cmd[128] = {0};
	
	system("killall -9 phddns");
	fp = fopen("/etc/phlinux.conf", "r");
	if( NULL == fp )
		return 0;
	fclose(fp);
	snprintf(dnodeName, sizeof(dnodeName), DDNS_ENTRY_NODE, 1);
	cfg_get_object_attr(dnodeName, "DDNS_Name", dprobuf, sizeof(dprobuf));
	if (!strcmp(dprobuf, "ORAY")){
		cfg_get_object_attr(dnodeName, "Active", dtmp, sizeof(dtmp));	
		if (!strcmp(dtmp, "Yes")){	
			snprintf(cmd, sizeof(cmd), "chmod 777 %s", "/userfs/bin/phddns");
			system(cmd);
			snprintf(cmd, sizeof(cmd), "%s -c /etc/phlinux.conf -d", "/userfs/bin/phddns");			
			system(cmd);
			tcdbg_printf("%s\n",cmd);
			unlink("/etc/phlinux.conf");
		}
	}
	
	return SUCCESS;
}

int start_phddns_or_ezipupdate_process(void)
{
	char value[32] = {0};
	if(cfg_get_object_attr(DDNS_COMMON_NODE, "useOray", value, sizeof(value)) > 0 && !strcmp(value, "Yes"))
	{
		start_phddns_process();
	}
	else
	{
		start_ezipupdate_process();
	}

	return SUCCESS;
}

#if defined(TCSUPPORT_CT_JOYME)
int start_plugin_phddns_process(void)
{
	char value[32] 		= {0};
	char tmp[32] 		= {0};
	char probuf[32] 	= {0};
	char nodeName[128]	= {0};
	char cmd[128] = {0};
	
	system("killall -9 phddns");
	snprintf(nodeName, sizeof(nodeName), DDNS_ENTRY_1_NODE);
	if(cfg_get_object_attr(nodeName, "type", value, sizeof(value)) > 0 && !strcmp(value, "plugin")){	
		cfg_get_object_attr(nodeName, "DDNS_Name", probuf, sizeof(probuf));
		if (!strcmp(probuf, "ORAY")){
			cfg_get_object_attr(nodeName, "Active", tmp, sizeof(tmp));	
			if (!strcmp(tmp, "Yes")){			
				snprintf(cmd, sizeof(cmd), "cp %s /etc/", ORAY_CONFIG_PATH);
				system(cmd);
				snprintf(cmd, sizeof(cmd), "chmod 777 %s", ORAY_PATH);
				system(cmd);
				snprintf(cmd, sizeof(cmd), "%s -c /etc/phlinux.conf -d", ORAY_PATH);			
				system(cmd);		
				unlink("/etc/phlinux.conf");
			}	
		}
	}	
	else
	{				 	
	 	start_ezipupdate_process();	
	}

	return SUCCESS;
	}
#endif

int svc_wan_related_ddns_execute(void)
{
	kill_process(DDNS_PID_PATH);
	
#if defined(TCSUPPORT_CT_DBUS)
	return start_phddns_process();
#endif

#if defined(TCSUPPORT_CT_JOYME)
	return start_plugin_phddns_process();
#else
	return start_phddns_or_ezipupdate_process();
#endif
}

void svc_wan_related_del_dmz_entry(int if_index)
{
	char nodeName[128]	= {0};
	
	snprintf(nodeName, sizeof(nodeName), DMZ_ENTRY_NODE, if_index + 1);
	cfg_delete_object(nodeName);
}


void svc_wan_related_del_virserver_entry(int if_index)
{
	char nodeName[128]	= {0};
	
	snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, if_index + 1);
	cfg_delete_object(nodeName);
}


void svc_wan_related_del_igmp_mld_proxy(char *if_name)
{
	char value[32] = {0};
	
	cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", value, sizeof(value));
	if(value[0] != '\0' && !strcmp(if_name, value)){
		cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", "");
		cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "No");

		/*kill igmpproxy process*/
		system("killall igmpproxy");
	}

#ifdef IPV6
	cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", value, sizeof(value));
	if(value[0] != '\0' && !strcmp(if_name, value)){
		cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", "");
		cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "No");

		/*kill mld proxy process*/
		system("killall ecmh");
	}
#endif
}


#if defined(TCSUPPORT_IGMP_PROXY)
void restart_igmpproxy(char *if_name)
{
	char active[5] = {0};
	char upstreamif[16] = {0};
	
#if defined(TCSUPPORT_NP_CMCC)
	cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF" , if_name);
#endif		
	memset(active, 0, sizeof(active));
	memset(upstreamif, 0, sizeof(upstreamif));
	if(cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "Active", active, sizeof(active)) > 0 &&
		cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", upstreamif, sizeof(upstreamif)) > 0)
	{
		if( (0 == strcmp(active, "Yes")) && (0 == strcmp(upstreamif, if_name)) )
		{
			svc_wan_related_igmpproxy_update();
		}
	}
}
#endif


#ifdef TCSUPPORT_MLD_PROXY
void restart_mldproxy(char *if_name, int if_index)
{
	char active[5] = {0};
	char upstreamif[16] = {0};
	
	memset(active, 0, sizeof(active));
	memset(upstreamif, 0, sizeof(upstreamif));
	
	if(cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "Active", active, sizeof(active)) > 0 &&
		cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", upstreamif, sizeof(upstreamif)) > 0)
	{
		if( (0 == strcmp(active, "Yes")) && (0 == strcmp(upstreamif, if_name)) )
		{
			svc_wan_related_mldproxy_update();
		}
	}
}
#endif

#if !defined(TCSUPPORT_CMCCV2)
void restart_dhcprelay(int if_index)
{
	char buf[32] = {0}, nodeName[64] = {0};
	char restart_dhcprelay[4] = {0};

	cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "restart_dhcprelay", restart_dhcprelay, sizeof(restart_dhcprelay));
	if(atoi(restart_dhcprelay))
		return;

	memset(nodeName, 0, sizeof(nodeName) );
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, if_index/MAX_PVC_NUM + 1, if_index%MAX_PVC_NUM + 1);

	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "ISP", buf, sizeof(buf)) <= 0)
	{
		tcdbg_printf("get ISP value fail\n");
		return;
	}

	if(atoi(buf) != 0 && atoi(buf) != 1)
	{
		tcdbg_printf("ISP value fail\n");
		return;
	}

	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "ServiceList", buf, sizeof(buf)) <= 0)
	{
		tcdbg_printf("get ISP ServiceList fail\n");
		return;
	}
			
	if(strstr(buf, "INTERNET") == NULL)
	{
		tcdbg_printf("get INTERNET value fail\n");
		return;
	}

	if(cfg_get_object_attr(LAN_DHCP_NODE, "type", buf, sizeof(buf)) <= 0){
		tcdbg_printf("get dhcp relay type value fail\n");
		return;
	}

	if(0 != strcmp(buf,"2")){/*2:dhcprelay*/
		tcdbg_printf("dhcp relay value not open\n");
		return;
	}

	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "restart_dhcprelay" , "1");

	return ;
}
#endif

#if defined(TCSUPPORT_WLAN_8021X) 
void restart_rtdot1xd(void)
{ 
	system("killall -9 rtdot1xd");
	system("/userfs/bin/rtdot1xd");
#if defined(TCSUPPORT_WLAN_AC)	
	system("killall -9 rtdot1xd_AC");
	system("/userfs/bin/rtdot1xd_AC");
#endif

	return;
}	
#endif
#if defined(TCSUPPORT_CMCCV2)
void restart_wantelnet()
{
	char def_route_index_v4[16] = {0};
	char nodeName[32] = {0}, wan_itf[8] = {0}, telnet_wanitf[8] = {0};
	int def_index4 = -1;

	/* get last telnet wan interface */
	memset(telnet_wanitf, 0, sizeof(telnet_wanitf));
	cfg_get_object_attr(ACCOUNT_WANTELNETENTRY_NODE, "IFName", telnet_wanitf, sizeof(telnet_wanitf));

	/* based on default route wan */
	memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
	cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4));
	if ( 0 == def_route_index_v4[0] || 0 == strcmp(def_route_index_v4, "N/A") )
	{
		cfg_set_object_attr(ACCOUNT_WANTELNETENTRY_NODE, "IFName", "");
		cfg_commit_object(ACCOUNT_WANTELNETENTRY_NODE);
	}
	else
	{
		def_index4 = atoi(def_route_index_v4);
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 1, def_index4%MAX_SMUX_NUM + 1);
		memset(wan_itf, 0, sizeof(wan_itf));
		cfg_get_object_attr(nodeName, "IFName", wan_itf, sizeof(wan_itf));
		if ( 0 == wan_itf[0] || 0 == strcmp(wan_itf, "N/A") ) /* Get default route wan IFName failed */
		{
			cfg_set_object_attr(ACCOUNT_WANTELNETENTRY_NODE, "IFName", "");
			cfg_commit_object(ACCOUNT_WANTELNETENTRY_NODE);
		}
		else if ( 0 != strcmp(wan_itf, telnet_wanitf) ) /* Donot restart wantelnet unless telnetwan interface is changed */
		{
			cfg_set_object_attr(ACCOUNT_WANTELNETENTRY_NODE, "IFName", wan_itf);
			cfg_commit_object(ACCOUNT_WANTELNETENTRY_NODE);
		}
	}
	
}
#endif

