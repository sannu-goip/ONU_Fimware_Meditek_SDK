#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "ping_diagnostic.h"
#include "utility.h"

void recheckPingDiagnostic(int if_index)
{
	char nodeName[32];
	int i = 0;
	char diagnostic[20] = {0},pingtotal[15] = {0},pingnum[15] = {0}, active[8] = {0};
	char recheck_flag[16];
	char *temp = NULL;
	char Interfacebuf[32] = {0};
	char cmdbuf[128] = {0};
	int interfaceindex = 0;
	FILE *fp = NULL;
	int totalnum = 0,currentnum = 0;
	int ping_temp = 0;

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), GLOBALSTATE_PINGDIAGNOSTIC_NODE);
	memset(recheck_flag, 0, sizeof(recheck_flag));
	if(cfg_get_object_attr(nodeName, "recheck_flag", recheck_flag, sizeof(recheck_flag)) <= 0)
	{
		ping_temp = 0;
	}
	else
	{
		ping_temp = atoi(recheck_flag);
	}

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), CTDIAGNOSTIC_COMMON_NODE);	
	memset(active,0,sizeof(active));
	if(cfg_get_object_attr(nodeName, "Active", active, sizeof(active)) <= 0)
	{
		return ;
	}
	else
	{
		if(0 == strcmp(active, "No"))
		{
			return;
		}
	}

	for(i=0; i<CTCOM_MAX_IPPINGDIAGNOSTIC_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), CTDIAGNOSTIC_ENTRY_NODE, (i+1));
		memset(diagnostic, 0, sizeof(diagnostic));
		if(cfg_get_object_attr(nodeName, "DiagnosticsState", diagnostic, sizeof(diagnostic)) <= 0)
		{
			continue;
		}

		memset(pingtotal, 0, sizeof(pingtotal));
		if(cfg_get_object_attr(nodeName, "NumberOfRepetitions", pingtotal, sizeof(pingtotal)) <= 0)
		{
			continue;
		}

		memset(Interfacebuf, 0, sizeof(Interfacebuf));
		if(cfg_get_object_attr(nodeName, "Interface", Interfacebuf, sizeof(Interfacebuf)) <= 0)
		{
			continue;
		}

		memset(pingnum, 0, sizeof(pingnum));
		if(cfg_get_object_attr(nodeName, "PingNum", pingnum, sizeof(pingnum)) <= 0)
		{
			continue;
		}

		totalnum   = atoi(pingtotal);
		currentnum = atoi(pingnum);
		if(strcmp(diagnostic, "None") && ((0 == atoi(pingtotal)) ||(totalnum -currentnum) > 0))//only "None" mean not need to do ping operation
		{
			//if up interface is not the diagnostic interface
			if((temp = strstr(Interfacebuf,"smux")) != NULL)
			{
				temp += strlen("smux");
				interfaceindex = atoi(temp);
				if(if_index != interfaceindex)
				{
					continue;
				}
			}
			else if((temp = strstr(Interfacebuf, "br0")) != NULL)
			{
				continue;
			}
			//	sprintf(buf,IPPINGDIAGNOSTIC_ENTRY,i);
#if 0
			//need to stop ping first wether the ping application is exist or not(not permit to ping more times)
			stopCwmpPing(i);
#endif
			//we use whether pid file is exist or not as judgement first
			sprintf(cmdbuf,CTCOM_PING_PID_PATH,i);
			fp = fopen(cmdbuf, "r");
			if(fp)
			{
				fclose(fp);
				continue;
			}
			//and then commit the node
			//bit X is 1 meas CtDiagnostic_EntryX need to committed by start_cc
			//recheckPingDiagnostic_flag will be checked in start_cc			
			ping_temp |= 0x1<<i;
		}
	}

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), GLOBALSTATE_PINGDIAGNOSTIC_NODE);
	memset(recheck_flag, 0, sizeof(recheck_flag));
	snprintf(recheck_flag, sizeof(recheck_flag), "%d", ping_temp);
	cfg_set_object_attr(nodeName, "recheck_flag", recheck_flag);

	return; 
}

