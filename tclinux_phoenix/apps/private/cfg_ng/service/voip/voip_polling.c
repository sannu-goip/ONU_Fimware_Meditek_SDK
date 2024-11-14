#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "cfg_cli.h"
#include "cfg_msg.h"

#include "voip_cfg.h"
#include "utility.h"

static void voip_mgr_polling_thread(void)
{
	int voipProtocol = 0;
	char tmp[8] = {0};
	char lanEnable[8] = {0};
	char scStatus[8] = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	char loadFlag[4] = {0};
#endif

	tcdbg_printf("voip_mgr_polling_pthread entered.\n");

	while(1)
	{
		sleep(1);

		if(checkCFGload() == 0){
			continue;
		}

		if((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "LanBindEnable", lanEnable, sizeof(lanEnable)) < 0)
			||((cfg_get_object_attr(VOIPADVANCED_COMMON_NODE, "LanBindEnable", lanEnable, sizeof(lanEnable)) >= 0) && strcmp(lanEnable, "1")))
		{
			if(!is_xpon_traffic_up())
			{
				continue;
			}
		}
			
		cfg_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", tmp, sizeof(tmp));
		voipProtocol = atoi(tmp);

#if defined(TCSUPPORT_CT_JOYME2)
		cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", loadFlag, sizeof(loadFlag));
#endif

		if(0 == voipProtocol)
		{
			if(
#if defined(TCSUPPORT_CT_JOYME2)
				!strcmp(loadFlag, "1") && 
#endif
				((0 == strcmp(lanEnable, ""))||strcmp(lanEnable, "1")))
			{
				if((cfg_get_object_attr(INFOVOIP_COMMON_NODE,"SC_START_STATUS",scStatus, sizeof(scStatus)) >= 0) && (0 == strcmp(scStatus, "Yes")))
				{
					restart_sip_when_sip_down();
#if !defined(TCSUPPORT_CT_PON_SC)
					restart_sip_when_line_unreg();
#endif
				}
			}
			voipStatUpgrade();
		}
#if defined(TCSUPPORT_ECN_MEGACO)
		else
		{
			if(
#if defined(TCSUPPORT_CT_JOYME2)
				!strcmp(loadFlag, "1") && 
#endif
				((0 == strcmp(lanEnable, ""))||strcmp(lanEnable, "1")))
			{
				if((cfg_get_object_attr(INFOVOIPH248_COMMON_NODE, "START_STATUS", scStatus, sizeof(scStatus)) >=0)&& (0 == strcmp(scStatus, "Yes")))
				{
					restart_mgapp_when_down();
					check_mgw_active_state();
				}
			}
		}
#endif

	}

	return;
}

int creat_voip_mgr_polling_pthread(void)
{
	pthread_t thread_id;

	tcdbg_printf("voip_mgr_polling_pthread created.\n");
	if(pthread_create(&thread_id, NULL, (void *)voip_mgr_polling_thread, NULL) < 0 )
	{
		tcdbg_printf("creat_voip_mgr_polling_pthread: create thread fail!!\n");
		return -1;
	}

	return 0;
}

