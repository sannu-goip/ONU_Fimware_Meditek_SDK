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
#include <time.h>
#include <unistd.h>
#include <cfg_cli.h> 
#include <svchost_evt.h>

#include "cfg_types.h"
#include "cfg_type_voip.h"
#if defined(TCSUPPORT_CMCCV2)
#include "cfg_msg.h"
#endif
#include "utility.h"


#define VOICE_IAD_ERROR                "1"
#define VOICE_ROUTE_ERROR              "2"
#define VOICE_RESPONSE_ERROR           "3"
#define VOICE_USER_PW_ERROR            "4"
#define VOICE_UNKNOWN_ERROR            "5"
#define VOICE_NO_WAN_ERROR             "6"

#if defined(TCSUPPORT_CMCCV2)
#define VOIPMONITOR "VoIPMonitor"
#define BUNDLENAME  "BundleName"
#define VOIPEVENT   "event"
#define MAX_EVENTNUM_RULE					4
#endif


int cfg_type_infovoip_common_get(char* path,char* attr,char* val,int len)
{
	char tmp[32] = {0};
	int prevSipStatus = 0;
	
	if(cfg_obj_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	
	prevSipStatus = atoi(tmp);

    if(0 == prevSipStatus)
    {
        cfg_set_object_attr(path, "EntryID0", "1");
        cfg_set_object_attr(path,"Codec0", "G.711MuLaw");
        cfg_set_object_attr(path,"BitRate0", "16");
        cfg_set_object_attr(path,"PacketizationPeriod0", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression0", "1");

        cfg_set_object_attr(path,"EntryID1", "2");
        cfg_set_object_attr(path,"Codec1", "G.711ALaw");
        cfg_set_object_attr(path,"BitRate1", "16");
        cfg_set_object_attr(path,"PacketizationPeriod1", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression1", "1");

        cfg_set_object_attr(path,"EntryID2", "3");
        cfg_set_object_attr(path,"Codec2", "G.729");
        cfg_set_object_attr(path,"BitRate2", "16");
        cfg_set_object_attr(path,"PacketizationPeriod2", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression2", "1");

        cfg_set_object_attr(path,"EntryID3", "4");
        cfg_set_object_attr(path,"Codec3", "G.726");
        cfg_set_object_attr(path,"BitRate3", "16,24,32");
        cfg_set_object_attr(path,"PacketizationPeriod3", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression3", "0");

        cfg_set_object_attr(path,"EntryID4", "5");
        cfg_set_object_attr(path,"Codec4", "G.723.1");
        cfg_set_object_attr(path,"BitRate4", "16");
        cfg_set_object_attr(path,"PacketizationPeriod4", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression4", "0");

        cfg_set_object_attr(path,"EntryID5", "6");
        cfg_set_object_attr(path,"Codec5", "G.722");
        cfg_set_object_attr(path,"BitRate5", "16");
        cfg_set_object_attr(path,"PacketizationPeriod5", "10,20,30");
        cfg_set_object_attr(path,"SilenceSuppression5", "0");

    }

	cfg_obj_get_object_attr(path, attr, 0, val, len);

	return 0;
}


int cfg_type_infovoip_entry_get(char* path,char* attr,char* val,int len)
{
	char tmp[64] = {0};
	char voipPrevCurTime[64] = {0};
	char voipPrevRegFlag[8] = {0};
	time_t now={0};
	long currentTime = time(&now);
	int voipDownTime = 0;
	int lineIdx = 0;

	if(strstr(path, "entry"))
		sscanf(path, INFOVOIP_ENTRY_NODE, &lineIdx);
	else if(strstr(path, "Entry"))
		sscanf(path, "root.InfoVoIP.Entry.%d", &lineIdx);

	if(lineIdx <= 0)
	{
		printf("cfg_type_infovoip_entry_get: invalid lineIdx(%d).\n", lineIdx);
		return -1;
	}

	if (cfg_obj_query_object(path, NULL, NULL) <= 0)
		cfg_obj_create_object(path);

    if (!strcmp(attr, "RegFailReason") && isVoIPAppDown(0))
    {   
        /* 1. update InfoVoIP_Entry%d Status to be Error
           	2. update InfoVoIP_Entry%d RegFailReason to be 1(IAD error)*/
        cfg_set_object_attr(path, "Status", "Error");
        cfg_set_object_attr(path, "RegFailReason", VOICE_IAD_ERROR);
    }
    else if (!strcmp(attr, "ServerDownTime"))/* update voip line status */  
    {
    	cfg_obj_get_object_attr(path, "VoipPrevRegFlag", 0, voipPrevRegFlag, sizeof(voipPrevRegFlag));
    	if(!strcmp(voipPrevRegFlag, "0"))
    	{
			cfg_obj_get_object_attr(path, "ServerDownTime", 0, tmp, sizeof(tmp));
			cfg_obj_get_object_attr(path, "VoipPrevCurTime", 0, voipPrevCurTime, sizeof(voipPrevCurTime));
			voipDownTime = atoi(tmp) + (currentTime - atoi(voipPrevCurTime));
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", voipDownTime);
			cfg_set_object_attr(path, "ServerDownTime", tmp);
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%ld", currentTime);
			cfg_set_object_attr(path, "VoipPrevCurTime", tmp);			
			printf("voipDownTime = %d, currentTime = %ld, atoi(voipPrevCurTime) = %d\n", voipDownTime, currentTime, atoi(voipPrevCurTime));
		}

	}

	cfg_obj_get_object_attr(path, attr, 0, val, len);

	return 0;
}


#if defined(TCSUPPORT_CMCCV2)
int VoIPmonitorMess_send(void)
{
	nlk_msg_t nklmsg;
	traffic_msg_t *nfmsg = NULL;
	int ret = 0, i = 0, k = 0, isSuc = 0, callDur = 0, potsidx = 0 ;
	char nodeName[64] ={'\0'}, BundleName[128] ={'\0'}, event[128] ={'\0'};
	char CallDirect[8] ={'\0'}, IsSucces[8] ={'\0'}, CallDuration[32] ={'\0'}, 
		CallNumber[32] ={'\0'}, POTSindex[32] ={'\0'};
	
	memset(&nklmsg, 0, sizeof(nlk_msg_t));	
	nklmsg.nlmhdr.nlmsg_len = NLK_MSG_LEN(traffic_msg_t);/*NLMSG_SPACE(NLK_LOCAL_PAYLOAD);*/
	nklmsg.nlmhdr.nlmsg_pid = getpid();  /* self pid */
	nklmsg.nlmhdr.nlmsg_flags = 0;
	nklmsg.eventType = MSG_NOTIFY2MOBILE;
	
	nfmsg = &nklmsg.data.fmsg;

	for(i = 0; i < 8; i++ ){
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VOIPMONITOR_PVC_NODE, i+1);
		memset(BundleName, 0, sizeof(BundleName));
		ret = cfg_obj_get_object_attr(nodeName, BUNDLENAME, 0, BundleName, sizeof(BundleName));
		
		if( ret < 0 || '\0' == BundleName[0] )
			continue;
		snprintf(nfmsg->bundlename, sizeof(nfmsg->bundlename), "%s", BundleName);
		
		for(k =0; k < MAX_EVENTNUM_RULE; k++){
			memset(nodeName,0,sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VOIPMONITOR_PVC_ENTRY_NODE, i+1, k+1);
			ret = cfg_obj_get_object_attr(nodeName, VOIPEVENT, 0, event, sizeof(event));

			if( ret >= 0 && '\0' != event[0]){
				memset(nodeName,0,sizeof(nodeName));
				strcpy(nodeName, INFOVOIP_COMMON_NODE);
				
				cfg_obj_get_object_attr(nodeName, "CallDirect", 0, CallDirect, sizeof(CallDirect));
				cfg_obj_get_object_attr(nodeName, "IsSuccess", 0, IsSucces, sizeof(IsSucces));
				cfg_obj_get_object_attr(nodeName, "CallDuration", 0, CallDuration, sizeof(CallDuration));
				cfg_obj_get_object_attr(nodeName, "CallNumber", 0, CallNumber, sizeof(CallNumber));
				cfg_obj_get_object_attr(nodeName, "POTSindex", 0, POTSindex, sizeof(POTSindex));
				isSuc = atoi(IsSucces);
				callDur = atoi(CallDuration);
				potsidx = atoi(POTSindex);
				
				snprintf(nfmsg->buf, sizeof(nfmsg->buf), "{\"Event\":\"%s\",\"CallDirect\":\"%s\",\"IsSuccess\":%d,\"CallDuration\":%d,\"CallNumber\":\"%s\",\"POTSindex\":%d}",
					event, CallDirect, isSuc, callDur, CallNumber, potsidx);
				
				/* printf("nklmsg.data.payload=%s,nfmsg->bundlename=[%s]\n", nfmsg->buf, nfmsg->bundlename); */
		
				ret = nlkmsg_send(NLKMSG_GRP_MOBILE, NLK_MSG_LEN(traffic_msg_t), &nklmsg);
				if(ret < 0) {
					printf("send message err!\n");
					return ret;
				}
			}			
		}		
	}
	return 0;
}

int cfg_type_infovoip_commit(char* path)
{
	return VoIPmonitorMess_send();
}
#endif


static cfg_node_ops_t cfg_type_infovoip_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query,
#if defined(TCSUPPORT_CMCCV2)
	 .commit = cfg_type_infovoip_commit,
#else
	 .commit = cfg_type_default_func_commit,
#endif
}; 

static cfg_node_ops_t cfg_type_infovoip_common_ops  = { 
	 .get = cfg_type_infovoip_common_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
#if defined(TCSUPPORT_CMCCV2)
	 .commit = cfg_type_infovoip_commit,
#else
	 .commit = cfg_type_default_func_commit,
#endif
}; 

static cfg_node_ops_t cfg_type_infovoip_entry_ops  = { 
	 .get = cfg_type_infovoip_entry_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
#if defined(TCSUPPORT_CMCCV2)
	 .commit = cfg_type_infovoip_commit,
#else
	 .commit = cfg_type_default_func_commit,
#endif
}; 

cfg_node_type_t cfg_type_infovoip_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE, 
	 .parent = &cfg_type_infovoip, 
	 .ops = &cfg_type_infovoip_common_ops, 
}; 

cfg_node_type_t cfg_type_infovoip_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE|CFG_TYPE_FLAG_MULTIPLE|CFG_TYPE_VOIP_INFO_NUM,
	 .parent = &cfg_type_infovoip, 
	 .ops = &cfg_type_infovoip_entry_ops, 
}; 


static cfg_node_type_t* cfg_type_infovoip_child[] = { 
	 &cfg_type_infovoip_common, 
	 &cfg_type_infovoip_entry, 
	 NULL 
};


cfg_node_type_t cfg_type_infovoip= { 
	 .name = "InfoVoIP", 
	 .flag = CFG_TYPE_FLAG_MEMORY|CFG_TYPE_FLAG_UPDATE|1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_infovoip_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_infovoip_child, 
	 .ops = &cfg_type_infovoip_ops, 
}; 

