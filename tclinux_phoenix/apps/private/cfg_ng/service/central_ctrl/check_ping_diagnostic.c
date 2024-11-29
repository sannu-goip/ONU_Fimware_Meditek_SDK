#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "central_ctrl_common.h"
#include "check_ping_diagnostic.h"
#include "utility.h"

void check_ctIppingDiagnosatic(void)
{
	int num = 0;
	char nodeName[32];
	char buf[32] = {0};
	char delay_char[32]={0};
	static int delay = -1;
	char recheck_flag[16];
	int flag = 0;

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), GLOBALSTATE_PINGDIAGNOSTIC_NODE);
	memset(recheck_flag, 0, sizeof(recheck_flag));
	if(cfg_get_object_attr(nodeName, "recheck_flag", recheck_flag, sizeof(recheck_flag)) <= 0)
	{
		flag = 0;
	}
	else
	{
		flag = atoi(recheck_flag);
	}

	if(!flag)
	{
		return;
	}

	if(delay < 0)
	{
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), GLOBALSTATE_COMMON_NODE);
		memset(delay_char, 0, sizeof(delay_char));
		if(cfg_get_object_attr(nodeName, "Delay", delay_char, sizeof(delay_char)) <= 0)
		{
			delay = 5;
		}
		else
		{
			delay = atoi(delay_char);
		}
	}

	delay--;
	if(delay > 0)
	{
		return;
	}

	for(num = 0; num < CTCOM_MAX_IPPINGDIAGNOSTIC_NUM; num++)
	{
		if(flag&(0x1 << num))
		{
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), CTDIAGNOSTIC_ENTRY_NODE, (num+1));
			if(!cfg_commit_object(buf))
			{
				flag &= ~(0x1 << num);
			}
		}
	}

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), GLOBALSTATE_PINGDIAGNOSTIC_NODE);
	memset(recheck_flag, 0, sizeof(recheck_flag));
	snprintf(recheck_flag, sizeof(recheck_flag), "%d", flag);
	cfg_set_object_attr(nodeName, "recheck_flag", recheck_flag);

	return ;
}		

