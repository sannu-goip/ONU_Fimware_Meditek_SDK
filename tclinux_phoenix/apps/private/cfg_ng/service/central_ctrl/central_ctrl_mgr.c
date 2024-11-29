#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "central_ctrl_common.h"
#include "central_ctrl_mgr.h"

int cc_mgr_debug_level(struct host_service_pbuf* pbuf)
{
	int level = 0;
	pt_central_ctrl_evt p_evt_data = NULL;

	p_evt_data = (pt_central_ctrl_evt)svchost_get_event_val(pbuf);
	if(NULL == p_evt_data)
	{
		return -1;
	}
	level = atoi(p_evt_data->buf);

	svc_cc_debug_level(level);

	return 0;
}


int central_ctrl_mgr_deal_wan_evt(unsigned int evt, char* buf)
{
    SVC_CENTRAL_CTRL_DEBUG_INFO("evt = %d, buf = %s.\n", evt, buf);

    switch (evt)
    {
        case EVT_WAN_IPV4_UP:
        {
            break;
        }
        case EVT_WAN_IPV4_DOWN:
        {
            break;
        }
        case EVT_WAN_IPV6_UP:
        {
            break;
        }
        case EVT_WAN_IPV6_DOWN:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    
    return 0;
}


