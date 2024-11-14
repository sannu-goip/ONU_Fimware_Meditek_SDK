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
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/msg.h>
#include <unistd.h>

#include <linux/version.h>

#include <svchost_api.h> 
#include <cfg_api.h>
#include <regex.h>

#include "voip_common.h"
#include "voip_mgr.h"
#include "utility.h"


int g_svc_voip_level = E_NO_INFO_LEVEL;

int svc_voip_execute_cmd(char* cmd)
{
	SVC_VOIP_DEBUG_INFO("voip cmd:[%s]\n", cmd);
	system_escape(cmd);
	
	return 0;
}

void svc_voip_debug_level(int level)
{
	g_svc_voip_level = level;
	tcdbg_printf("g_svc_voip_level = 0x%08x\n", g_svc_voip_level);
	return;
}

int svc_voip_update_line_num()
{
	char tmp[32] = {0};
	char node[32] = {0};
	int  voipLineNumber = 0;

	snprintf(node, sizeof(node), VOIPBASIC_COMMON_NODE);
	if (cfg_get_object_attr(node, "VoIPLineNumber", tmp, sizeof(tmp)) < 0)
		voipLineNumber = 2;
	else
		voipLineNumber = atoi(tmp);

	return voipLineNumber;

}

#if defined(TCSUPPORT_SDN_OVS)
void voip_netns_start(char *cmd)
{
	char nodeName[64] = {0};
	char tmp[32] = {0};
	char cmdbuf[128]= {0};

	strncpy(nodeName, VOIPADVANCED_COMMON_NODE, sizeof(nodeName) - 1);
	if((cfg_get_object_attr(nodeName, "netns", tmp, sizeof(tmp)) >= 0) && strlen(tmp))
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/ip netns exec %s %s", tmp, cmd);
		system_escape(cmdbuf);
		svc_voip_execute_cmd("pidof sipclient > /proc/ksocket_ns");  
	}
	else{
		svc_voip_execute_cmd(cmd);
	}
	
	SVC_VOIP_DEBUG_INFO("voip cmd:%s %s!!\r\n", cmdbuf, cmd);
}
#endif

