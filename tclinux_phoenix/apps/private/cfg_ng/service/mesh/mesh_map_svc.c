/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <svchost_api.h>
#include "mesh_map_mgr.h"
#include "mesh_map_common.h"
#include "mesh_action.h"

static int svc_mesh_internal_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	unsigned int evt_id = 0;
	int ret = 0;
	evt_id = svchost_get_event_id(pbuf);

	SVC_MESH_DEBUG_INFO("evt_id = 0x%04x\n", evt_id);
	switch(evt_id)
	{
		case EVT_CFG_MESH_BOOT:
		{
			ret = mesh_boot();
			break;
		}
		case EVT_CFG_UPDATE_DAT:
		{
			ret = update_dat_evt_handle();
			break;
		}
		case EVT_CFG_UPDATE_COMMON:
		case EVT_CFG_UPDATE_MAP:
		{
			ret = trigger_restart_map();
			break;
		}
		case EVT_CFG_UPDATE_MAPD:
		{
			ret = set_rssi_thresh();
			break;
		} 
		case EVT_CFG_UPDATE_STEER:
		{
			ret = set_channel_utilization_threshold();
			break;
		}
		case EVT_CFG_UPDATE_RADIO:
		{
			ret = mesh_renew_bssinfo(pbuf);
			break;
		}
		case EVT_CFG_ACTION:
		{
			ret = action_evt_handle();
			break;
		}
		case EVT_MESH_SRV_DEBUG_LEVEL:
		{
			ret = mesh_debug(pbuf);
			break;
		}
		default:
		{
			break;
		}
	}

	SVC_MESH_DEBUG_INFO("evt_id = 0x%04x, ret = %d\n", evt_id, ret);
	return ret;
}

int mesh_cc_ext_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
	SVC_MESH_DEBUG_INFO("evt_id = 0x%04x, ret = %d\n", evt_id, ret);
    switch (evt_id)
    {
		case EVT_CC_TRIGGER_MESH_REINIT :
		{
			restart_map();
			break;
		}
        default:
        {
            break;
        }
    }

    return ret ;
}

static struct host_service_event svc_mesh_mgr_evt[] = 
{
	{EVT_MESH_INTERNAL, svc_mesh_internal_evt_handle},
	{EVT_CENTRAL_CTRL_EXTERNAL,  mesh_cc_ext_evt_handle},
};

static int svc_mesh_start(struct host_service *svc)
{
	return 0;
}

static int svc_mesh_stop(struct host_service *svc)
{
	return 0;
}

static int svc_mesh_int(struct host_service *svc)
{
	int num = sizeof(svc_mesh_mgr_evt) / sizeof(struct host_service_event);
	if(num != svchost_reg_event(svc, svc_mesh_mgr_evt, num))
	{
		printf("svc_mesh reg event fail\n");
	}
	else
	{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
		printf("svc_mesh reg event ok\n");
	}
	
	mesh_pthread();

	return 0;
}

static struct host_service_intf svc_mesh_mgr_intf = 
{
	.id = HOST_SERVICE_MESH_MGR_ID,
	.flag = 0,
	.start = svc_mesh_start,
	.stop = svc_mesh_stop,
	.init = svc_mesh_int,
};

struct host_service_intf* svc_mesh_main(char* param);
struct host_service_intf* svc_mesh_main(char* param)
{
	return &svc_mesh_mgr_intf;
}

