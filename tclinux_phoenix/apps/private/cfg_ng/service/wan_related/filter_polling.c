#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "filter_polling.h"
#include "cfg_api.h"
#include "utility.h"
#include "cfg_msg.h"
#include "wan_related_mgr.h"
#include "wan_related_cfg.h"


void checkfilterDuration(void)
{
	int node_idx = 0;
	char url_nodeName[32] = {0};
	char dns_nodeName[32] = {0};
	int dns_flag = 0;

	/* UrlFilter_Entry(i) */
	for (node_idx = 0; node_idx < MAX_URLFILTER_NUM; node_idx++)
	{
		memset(url_nodeName, 0, sizeof(url_nodeName));
		memset(dns_nodeName, 0, sizeof(dns_nodeName));
		snprintf(url_nodeName, sizeof(url_nodeName), URLFILTER_ENTRY_N_NODE, node_idx + 1);
		snprintf(dns_nodeName, sizeof(dns_nodeName), DNSFILTER_ENTRY_N_NODE, node_idx + 1);
		/* UrlFilter_Entry(I) ->StartTime(i)  */
		if(isStepInOutBoundary(node_idx, &urlfilter_setting[node_idx].duration, url_nodeName))
		{
			svc_wan_related_urlfilter_write(node_idx + 1);
		}

		if(isStepInOutBoundary(node_idx, &dnsfilter_setting[node_idx].duration, dns_nodeName))
		{
			dns_flag = 1;
		}
	}

	if(dns_flag)
	{
#if defined(TCSUPPORT_CT_UBUS)
		svc_wan_related_dnsfilter_write();
#endif
		system("/usr/bin/killall -9 dnsmasq");
		system("/userfs/bin/dnsmasq &");
	}
	
}

static void filter_polling_pthread(void)
{
	char load_status[16] = {0};
	char nodeName[64] = {0};
	struct tm *cur_time = NULL;
	int i = 0;
	static int flag = 0;
	time_t t;
	
    while(1)
    {
   		sleep(1);
    	cfg_get_object_attr(CFG_LOAD_STATUS_FLAG_PATH,CFG_LOAD_STATUS_FLAG,load_status, sizeof(load_status));
		if( 0 != strncmp(load_status,CFG_LOAD_DONE,sizeof(load_status)-1))
		{
			continue;
		}
		time(&t);
		cur_time = localtime(&t);
		if(cur_time->tm_year != 70) /* nptserver sync ok */
		{
			flag = 0;
			checkfilterDuration();
		}
		else
		{
			if(flag == 1)
				continue;
				
			for(i = 0; i < MAX_URLFILTER_NUM; i++)
			{
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), URLFILTER_ENTRY_N_NODE, i + 1);
				cfg_set_object_attr(nodeName, "induration", "0");

				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), DNSFILTER_ENTRY_N_NODE, i + 1);
				cfg_set_object_attr(nodeName, "induration", "0");
			}
			flag = 1;
		}
		
    }
    
	pthread_exit(0);
}

int creat_filter_polling_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)filter_polling_pthread, NULL) < 0)
    {
        printf("filter_polling_pthread: create thread fail \n");
        return -1;
    }

    return 0;
}

