#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include "msg_netlink.h"
#include "central_ctrl_mgr.h"
#include "central_ctrl_polling.h"
#include "active_notify.h"
#include "cfg_cli.h"
#include "cfg_msg.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include "dbus_timer.h"
#endif
#include "wlan_link.h"
#include "central_ctrl_wan_evt.h"


#if 0
static int svc_central_ctrl_internal_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    unsigned int evt_id = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        default:
        {
            tcdbg_printf("central ctrl internal evt_id = %d.\n", evt_id);
            break;
        }
    }

    return 0 ;
}

static int svc_central_ctrl_external_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    unsigned int evt_id = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        default:
        {
            tcdbg_printf("central ctrl external evt_id = %d.\n", evt_id);
            break;
        }
    }

    return 0 ;
}

static int svc_central_ctrl_deal_wan_evt(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int               ret = 0;
    unsigned int   evt_id = 0;
    pt_wan_evt p_evt_data = NULL;
    
    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_WAN_ENTRY_DELETE:
        case EVT_WAN_ENTRY_UPDATE:
        case EVT_WAN_IPV4_UP:
        case EVT_WAN_IPV4_DOWN:
        case EVT_WAN_IPV6_UP:
        case EVT_WAN_IPV6_DOWN:
        case EVT_WAN_BRIDGE_UP:
        case EVT_WAN_BRIDGE_DOWN:
        case EVT_WAN_UPDATE_CONNREQ_PORT:
        {
            p_evt_data = (pt_wan_evt)svchost_get_event_val(pbuf);
            if(NULL == p_evt_data)
            {
                ret = -1;
                break;
            }
            ret = central_ctrl_mgr_deal_wan_evt(evt_id, p_evt_data->buf);
            break;
        }
        default:
        {
            tcdbg_printf("central ctrl external evt_id = %d.\n", evt_id);
            break;
        }
    }

    return ret;
}

static struct host_service_event svc_central_ctrl_event[] = 
{
    {EVT_CENTRAL_CTRL_INTERNAL, svc_central_ctrl_internal_evt_deal},
    {EVT_CENTRAL_CTRL_EXTERNAL, svc_central_ctrl_external_evt_deal},
    {EVT_WAN_EXTERNAL,          svc_central_ctrl_deal_wan_evt},
};
#endif




int cfg_external_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;
#if defined(TCSUPPORT_CT_UBUS) || defined(TCSUPPORT_CT_JOYME2)
	char* path = 0;
	path = (char*)svchost_get_event_val(pbuf);
#endif
    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_CFG_LOAD_UPDATE :
        {
          	cfg_obj_set_root(shm_gettree());
            break;
        }
#if defined(TCSUPPORT_CT_UBUS) || defined(TCSUPPORT_CT_JOYME2)
		case EVT_CFG_TIMER_BOOT:
			cfg_type_timer_data_boot(path);
			break;
		case EVT_CFG_TIMER_UPDATE:
			cfg_type_timer_data_update(1, path);
			break;
#endif
        default:
        {
            break;
        }
    }

    return ret ;
}

#if defined(TCSUPPORT_ECNT_MAP)
int cc_wlan_ext_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_WLAN_TRIGGER_MESH_UPDATE :
        {
			prepare_trigger_mesh();
			break;
        }
        default:
        {
            break;
        }
    }

    return ret ;
}

static int cc_mesh_ext_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
		case EVT_TRIGGER_MESH_UPDATE :
		{
			prepare_trigger_mesh();
			break;
		}
        default:
        {
            break;
        }
    }

    return ret ;
}
#endif

static int svc_cc_internal_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int ret = 0;
    int evt_id = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
		case EVT_CC_SRV_DEBUG_LEVEL :
		{
			ret = cc_mgr_debug_level(pbuf);
			break;
		}
        default:
        {
            tcdbg_printf("central ctrl internal evt_id = %d.\n", evt_id);
            break;
        }
    }

    return 0 ;
}

int cc_wan_ext_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_WAN_IPV4_UP :
        {
			cc_handle_wan_v4_up(pbuf);
			break;
        }
        default:
        {
            break;
        }
    }

    return ret ;
}

static struct host_service_event svc_central_ctrl_event[] = 
{
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
#if defined(TCSUPPORT_ECNT_MAP)
	{EVT_WLAN_EXTERNAL, cc_wlan_ext_evt_handle},
	{EVT_MESH_EXTERNAL, cc_mesh_ext_evt_handle},
#endif
	{EVT_WAN_EXTERNAL,  cc_wan_ext_evt_handle},
	{EVT_CENTRAL_CTRL_INTERNAL, svc_cc_internal_evt_handle},
};

static int svc_centrl_ctrl_init(struct host_service* svc)
{
    int num = sizeof(svc_central_ctrl_event) / sizeof(struct host_service_event);
	int res = 0;

    if (svchost_reg_event(svc, svc_central_ctrl_event, num) != num)
    {
        tcdbg_printf("svc_centrl_ctrl_init: ...... reg event fail \n");
    }
    else
    {
    	cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
        tcdbg_printf("svc_centrl_ctrl_init: ...... reg event ok \n");
    }

	msg_netlink_register();

    creat_centrlal_mgr_polling_pthread();

	res = notify_service_init();

#if defined(TCSUPPORT_CT_UBUS) || defined(TCSUPPORT_CT_JOYME2)
	res = ctc_dbus_timer_init();
#endif
	
	
    return 0;
}

static int svc_central_ctrl_start(struct host_service* svc)
{
    tcdbg_printf("central ctrl start.\n");
    return 0;
}

static int svc_central_ctrl_stop(struct host_service* svc)
{
    tcdbg_printf("central ctrl stop.\n");
	msg_netlink_unregister();
	
#if defined(TCSUPPORT_CT_JOYME2)
	ctc_dbus_timer_data_free();
#endif
    return 0;
}



static struct host_service_intf  svc_centrl_ctrl_mgr_intf = 
{
    .id     = HOST_SERVICE_CENTRAL_CTRL_MGR_ID,
    .flag   = 0,
    .init   = svc_centrl_ctrl_init,
    .start  = svc_central_ctrl_start,
    .stop   = svc_central_ctrl_stop,
};


struct host_service_intf* svc_central_ctrl_main(char* param);
struct host_service_intf* svc_central_ctrl_main(char* param)
{
    return &svc_centrl_ctrl_mgr_intf;
}





