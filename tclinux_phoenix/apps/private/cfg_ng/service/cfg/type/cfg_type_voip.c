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
#include "unistd.h"

#include <cfg_cli.h> 
#include <svchost_evt.h>

#include "cfg_types.h"
#include "utility.h"

int svc_voip_update_line_num()
{
	char tmp[32] = {0};
	char node[64] = {0};
	int  voipLineNumber = 0;

	strncpy(node, VOIPBASIC_COMMON_NODE, sizeof(node)-1);
	if (cfg_obj_get_object_attr(node, "VoIPLineNumber", 0, tmp, sizeof(tmp)) < 0)
		voipLineNumber = 1;
	else
		voipLineNumber = atoi(tmp);

	return voipLineNumber;

}

int getTokens(const char *inStr, const char* delim, char *pArgv[])
{
	int numOfToken = 0;
	static char tmp[128] = {0};
	char *t;

	strncpy(tmp, inStr, sizeof(tmp)-1);
	t = strtok(tmp, delim);
	
	while(t && numOfToken < 5)	{
		pArgv[numOfToken++] = t;
		t = strtok(NULL, delim);
	}

	return numOfToken;
}

int svc_cfg_boot_voip(void)
{
	FILE *fp = NULL;
	char strLine[128] = {0};
	char *param[5];
	char nodeName[64] = {0};
	char tmp[32] = {0};

	int i = 0;	
	int voipLineNumber = svc_voip_update_line_num();
	
	strncpy(nodeName, VOIPBASIC_COMMON_NODE, sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "SIPProtocol", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	if(voipLineNumber >= 2)
		cfg_set_object_attr(nodeName, "VoIPLine2Enable", "Yes");
	else
		cfg_set_object_attr(nodeName, "VoIPLine2Enable", "No");
	if(strcmp(tmp,"H.248") == 0)
		cfg_set_object_attr(nodeName, "prevSipStatus", "1");
	else 
		cfg_set_object_attr(nodeName, "prevSipStatus", "0");

	printf("VoIPBasic_Common SIPProtocol is %s\r\n", tmp);

	memset(nodeName,0,sizeof(nodeName));

	fp = fopen("/etc/voip_sys.cfg","r");
	if(fp){
		while (fgets(strLine, sizeof(strLine)-1, fp) != NULL)
		{
			strLine[strlen(strLine) - 1] = '\0';
			getTokens(strLine, "=", param);

			if ((NULL == strstr(param[0], "_ACCT_")) && (NULL == strstr(param[0], "_LINE_")))
			{
				strcpy(nodeName, VOIPSYSPARAM_COMMON_NODE);
				cfg_set_object_attr(nodeName, param[0], param[1]);
			}
			else
			{
				for (i = 0; i < voipLineNumber; i++)
				{
					memset(nodeName, 0x0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), VOIPSYSPARAM_ENTRY_NODE, i+1);
					if (cfg_obj_query_object(nodeName, NULL, NULL) <= 0)
						cfg_obj_create_object(nodeName);
					cfg_set_object_attr(nodeName, param[0], param[1]);
				}
			}
		}
		fclose(fp);
	}

	for (i = 0; i < voipLineNumber; i++)
	{
		memset(nodeName, 0x0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), VOIPTEST_ENTRY_NODE, i+1);
		cfg_set_object_attr(nodeName, "IsTestOnBusy", "0");
		cfg_set_object_attr(nodeName, "TestState", "None");
	}
	
	/*set boot2VoIPReady status,wan up to set wan ip action is need to be after boot2 VoIP Ready */ 		
	cfg_set_object_attr("root.InfoVoIP.Common", "boot2VoIPReady", "Yes");
	if(cfg_obj_get_object_attr("root.InfoVoIP.Common", "boot2delay", 0, tmp, sizeof(tmp)) >= 0){
		if(!strcmp(tmp, "Yes")){
			tcdbg_printf("boot2 is ready to send to voip service\n");
			cfg_obj_send_event(EVT_VOIP_INTERNAL, EVT_VOIP_BOOT2_READY, NULL, 0);
		}
	}

	return 0;
}

int svc_cfg_boot_voip_proto(void)
{
	char nodeName[64] = {0};
	char tmp[32] = {0};
	char protoval[32] = {0};

#if defined(TCSUPPORT_ECN_SIP) && !defined(TCSUPPORT_ECN_MEGACO)
	strncpy(protoval, "SIP", sizeof(protoval)-1);
#endif

#if !defined(TCSUPPORT_ECN_SIP) && defined(TCSUPPORT_ECN_MEGACO)
	strncpy(protoval, "H.248", sizeof(protoval)-1);
#endif

	strncpy(nodeName, VOIPBASIC_COMMON_NODE, sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "SIPProtocol", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	/* romfile is H.248, but upgrade to sip version ,so change to sip proto*/
	if(0 == strcmp(tmp,"H.248") && 0 == strcmp(protoval,"SIP"))
		cfg_set_object_attr(nodeName, "SIPProtocol", "SIP");
	else if (NULL != strstr(tmp,"SIP") && 0 == strcmp(protoval,"H.248"))
		cfg_set_object_attr(nodeName, "SIPProtocol", "H.248");

	tcdbg_printf("romfile prtoto:%s, upgrade proto:%s\n", tmp, protoval);

	return 0;
}

/*______________________________________________________________________________
**function :isVoIPAppDown
**
**description:
*    check sipclient or mgapp  crash or not
**parameters:
*    type: 0 - mean check sipclient
*              1 - mean check mgapp
**global:
*    none
**return: 
*    ret : 0 -voip app run ok 
*            1 - voip app crash
**call:
*    none
**revision:2.0
*  
**____________________________________________________________________________*/

int isVoIPAppDown(int type)
{
    FILE *fp = NULL;
    char cmdbuf[128] = {0};
    char voip_pid_tmp[32] = {0};
    char path[] = "/tmp/voipapp.pid";
    int  ret = 0;

    fp = fopen(path, "r");
    if(fp == NULL){
        /*get sipclient or mgapp process pid*/
        if (0 == type)
            snprintf(cmdbuf, sizeof(cmdbuf), "pidof %s > %s", "sipclient", path);
        else if (1 == type)
            snprintf(cmdbuf, sizeof(cmdbuf), "pidof %s > %s", "mgapp", path);
        else
            return ret;

        system_escape(cmdbuf);
        fp = fopen(path, "r");
    }
    if(fp == NULL){
        cfg_printf("open %s fail\n", path);
        return ret;
    }
    fgets(voip_pid_tmp, sizeof(voip_pid_tmp), fp);
    fclose(fp);
    unlink(path);
    if(atoi(voip_pid_tmp) == 0)
    {  /*if sip_pid_tmp == 0, that's mean need restart sipclient*/
        cfg_printf("\n%s crash!!\n", (type == 0?"sipclient":"mgapp"));
        ret = 1;
    }

    return ret;
}

int voip_update_protocol()
{
    char tmp[32] = {0};
    char node[32] = {0};
    int curSipStatus = 0;
    int prevSipStatus = 0;
    char tmpSip[16] = {0};
    char tmpH248[16] = {0};

    snprintf(node, sizeof(node), VOIPBASIC_COMMON_NODE);
    cfg_obj_get_object_attr(node, "prevSipStatus", 0, tmp, sizeof(tmp));
    prevSipStatus = atoi(tmp);

    memset(tmp, 0x0, sizeof(tmp));
    cfg_obj_get_object_attr(node, "SIPProtocol", 0, tmp, sizeof(tmp));
    printf("VoIPBasic_Common SIPProtocol is %s\n", tmp);
    if (0 == strcmp(tmp,"H.248"))
        curSipStatus = 1;
    else
        curSipStatus = 0;

    printf("prevSipStatus: %d, curSipStatus: %d\r\n", prevSipStatus, curSipStatus);
    if(prevSipStatus != curSipStatus) {
        cfg_set_object_attr(node, "prevSipStatus", itoa(curSipStatus));
        cfg_obj_get_object_attr(INFOVOIP_COMMON_NODE, "SC_START_STATUS", 0, tmpSip, sizeof(tmpSip));
        cfg_obj_get_object_attr(INFOVOIPH248_COMMON_NODE, "START_STATUS", 0, tmpH248, sizeof(tmpH248));

        if ((strlen(tmpSip) && !strcmp(tmpSip, "Yes")) || (strlen(tmpH248) && !strcmp(tmpH248, "Yes"))){
            printf("prev protocol is [sip:%s, H248:%s]\r\n", tmpSip, tmpH248);
            if(0 == curSipStatus) {
                system_escape("killall -9 mgapp");
                system_escape("killall -9 tcVoIPApiServer");
                system_escape("/userfs/bin/sipclient &");
            } else {
                system_escape("killall -9 sipclient");
                system_escape("/userfs/bin/tcVoIPApiServer &");
                system_escape("/userfs/bin/mgapp -id 0 &");
            }
        }
    }

    return curSipStatus;
}

void cfg_type_voip_send_evt(int type, int evt_id, int prevSipStatus, char* path)
{
	voip_evt_t voip_evt;
	char evt_type[EVT_TYPE_LENGTH];

	if((NULL == path) || (strlen(path) >= EVT_BUF_LENGTH))
	{
		printf("error path is NULL || path length is too long.\n");
		return ;
	}

	memset(&voip_evt, 0, sizeof(voip_evt));
	voip_evt.prevSipStatus = prevSipStatus;
	strncpy(voip_evt.buf, path, sizeof(voip_evt.buf)-1);
	memset(evt_type, 0, sizeof(evt_type));
	printf("voip_evt.buf: %s, type: %d, evt_id: 0x%08x\r\n", voip_evt.buf, type, evt_id);
	switch (type)
	{
		case EVT_VOIP_INTERNAL_TYPE :
		{
			strncpy(evt_type, EVT_VOIP_INTERNAL, sizeof(evt_type)-1);
			break;
		}
		case EVT_VOIP_EXTERNAL_TYPE:
		{
			strncpy(evt_type, EVT_VOIP_EXTERNAL, sizeof(evt_type)-1);
			break;
		}
		default:
		{
			return ;
		}
	}

	cfg_obj_send_event(evt_type, evt_id, (void*)&voip_evt, sizeof(voip_evt));

	return ;
}

int cfg_type_voipnormal_commit(char* path)
{
    int curSipStatus = voip_update_protocol();

    if(0 == curSipStatus)
        cfg_type_voip_send_evt(EVT_VOIP_INTERNAL_TYPE, EVT_CFG_VOIP_SIP_NORMAL_UPDATE, curSipStatus, path);
    else
        cfg_type_voip_send_evt(EVT_VOIP_INTERNAL_TYPE, EVT_CFG_VOIP_H248_NORMAL_UPDATE, curSipStatus, path);

    return 0;
}

