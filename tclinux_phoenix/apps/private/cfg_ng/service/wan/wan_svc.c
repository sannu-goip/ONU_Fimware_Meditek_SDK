
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
#include <svchost_api.h>
#include "wan_mgr.h"
#include "wan_polling.h"
#include "cfg_cli.h"
#include "utility.h"

static int svc_wan_internal_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_wan_evt p_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    SVC_WAN_CRITIC_INFO("receive wan internal msg evt = %d.\n", evt_id);
    switch (evt_id)
    {
	case EVT_CFG_WAN_ENTRY_BOOT :
        case EVT_CFG_WAN_ENTRY_DELETE :
        case EVT_CFG_WAN_ENTRY_UPDATE :
        case EVT_WAN_CONN_GETV4 :
        case EVT_WAN_CONN_LOSTV4 :
        case EVT_WAN_CONN_GETV6 :
        case EVT_WAN_CONN_LOSTV6 :
        case EVT_DUMP_WAN_SRV_ENTRY_DEV:  
        case EVT_DUMP_WAN_SRV_ENTRY_PATH:  
        case EVT_DUMP_WAN_SRV_ALL_ENTRY:  
        case EVT_WAN_SRV_READY_IPV6_GATEWAY:  
        case EVT_WAN_SRV_DEBUG_LEVEL:
        case EVT_XPON_UP:
        case EVT_XPON_DOWN:
        case EVT_CFG_WAN_VLANBIND_UPDATE:
	case EVT_WAN_UPDATE_DNSINFO:
        case EVT_CFG_WAN_VETHPAIR_UPDATE:
		case EVT_WAN_RELEASE6:
		case EVT_WAN_RENEW6:
		case EVT_CFG_WAN_ENTRY_BOOT2:
		case EVT_CFG_WAN_SDN_UPDATE:
 #if defined(TCSUPPORT_NP_CMCC)
		case EVT_WAN_AP_BRIDGE_GET4:
		case EVT_WAN_AP_BRIDGE_LOST4:
		case EVT_WAN_AP_STATUS_UPDATE:
		case EVT_WAN_AP_START_BRIDGE:
		case EVT_WAN_AP_START_CLIENT:
		case EVT_WAN_AP_MODE_UPDATE:
		case EVT_WAN_AP_LINK_UPDATE:
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		case EVT_CFG_WANCFGDATA_ENTRY_UPDATE:
		case EVT_CFG_WAN_BSSADDR_UPDATE:
#endif
#if defined(TCSUPPORT_CT_UBUS)
		case EVT_CFG_WAN_COMMON_UPDATE:
		case EVT_WAN_AP_BRIDGE_GET4:
		case EVT_WAN_AP_BRIDGE_LOST4:
#endif
        {
            p_evt_data = (pt_wan_evt)svchost_get_event_val(pbuf);
            if(NULL == p_evt_data)
            {
                ret = -1;
                break;
            }
            ret = svc_wan_mgr_handle_event(evt_id, p_evt_data->buf);
            break;
        }
        default:
        {
            break;
        }
    }

    return ret ;
}

#if 0
static int svc_wan_external_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_wan_evt p_evt_data = NULL;
    unsigned int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    SVC_WAN_CRITIC_INFO("receive wan external msg evt = %d.\n", evt_id);
    switch (evt_id)
    {
        case EVT_WAN_ENTRY_DELETE :
        case EVT_WAN_ENTRY_UPDATE :
        case EVT_WAN_IPV4_UP :
        case EVT_WAN_IPV4_DOWN :
        case EVT_WAN_IPV6_UP :
        case EVT_WAN_IPV6_DOWN :
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
            SVC_WAN_CRITIC_INFO("receive wan external msg evt = %d, buf = %s. \n", evt_id, p_evt_data->buf);
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

static int svc_pon_external_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_wan_evt p_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;
    char transMode[16] = {0};
	static int xpon_firstUp = 1;

    evt_id = svchost_get_event_id(pbuf);
    SVC_WAN_CRITIC_INFO("receive wan internal msg evt = %d.\n", evt_id);
    switch (evt_id)
    {
        case EVT_PON_TRAFFIC_STATUS :
        case EVT_ETHER_WAN_STATUS :
#if defined(TCSUPPORT_CT_DSL_EX)	
		case EVT_DSL_WAN_STATUS :
#endif
#if defined(TCSUPPORT_CT_UBUS)
		case EVT_APCLI_WAN_STATUS:
#endif 
#if defined(TCSUPPORT_ECNT_MAP)
		case EVT_MESH_BH_STATUS:
#endif 
        {
            if(evt_id == EVT_ETHER_WAN_STATUS)
            {
                if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode))
                    && 0 != strcmp(transMode, "Ethernet"))
                {
                    break;
                }
#if defined(TCSUPPORT_CT_UBUS)
				else if ( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", transMode,sizeof(transMode))
					&& 0 == strcmp(transMode, "repeater") )
				{
					break;
				}
#endif
            }
#if defined(TCSUPPORT_CT_DSL_EX)	 
			else if(evt_id == EVT_DSL_WAN_STATUS)
			{   
                if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
                    && (0 != strcmp(transMode, "ATM") && 0 != strcmp(transMode, "PTM")))
                {
                    break;
                }
            }
#endif
#if defined(TCSUPPORT_CT_UBUS)
			else if ( evt_id == EVT_APCLI_WAN_STATUS )
			{
				if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", transMode,sizeof(transMode))
					&& 0 != strcmp(transMode, "repeater") )
				{
					break;
				}
			}
#endif
#if defined(TCSUPPORT_ECNT_MAP)
			else if ( evt_id == EVT_MESH_BH_STATUS )
			{
#if !defined(TCSUPPORT_CT_UBUS) && !defined(TCSUPPORT_ANDLINK)
				break;
#endif
			}
#endif
            else
            {   
                if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode))
                    && 0 == strcmp(transMode, "Ethernet"))
                {
                    break;
                }
            }
            p_evt_data = (pt_wan_evt)svchost_get_event_val(pbuf);
            if(NULL == p_evt_data)
            {
                ret = -1;
                break;
            }
			if(strcmp(p_evt_data->buf, "up") == 0)
			{
				if(xpon_firstUp){
					svc_wan_mgr_handle_event(EVT_XPON_UP, "");
					xpon_firstUp = 0;
				}
				wan_mgr_update_status(E_WAN_PON_UP);
			}
			else
			{
				wan_mgr_update_status(E_WAN_PON_DOWN);
			}
            break;
        }
        default:
        {
            break;
        }
    }

    return ret ;
}

int cfg_external_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_CFG_LOAD_UPDATE :
        {
          	cfg_obj_set_root(shm_gettree());
            break;
        }
        default:
        {
            break;
        }
    }

    return ret ;
}

static struct host_service_event svc_wan_mgr_event[] = {	
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
    {EVT_WAN_INTERNAL, svc_wan_internal_evt_deal},
    /*{EVT_WAN_EXTERNAL, svc_wan_external_evt_deal},*/
    {EVT_PON_EXTERNAL, svc_pon_external_evt_deal},
};

static int svc_wan_start(struct host_service* svc)
{
    printf("svc_wan_start: ...... \n");

    svc_wan_start_all_wan_entry(1);
    svc_wan_start_all_wan_entry(0);
    return 0;
}

static int svc_wan_stop(struct host_service* svc)
{
    printf("svc_wan_stop: ............ \n");

    svc_wan_stop_all_wan_entry();

    return 0;
}


static int svc_wan_int(struct host_service* svc)
{
    int num = sizeof(svc_wan_mgr_event) / sizeof(struct host_service_event);

	wan_mgr_ponstatus_lock_init();
    if (svchost_reg_event(svc,svc_wan_mgr_event,num) != num)
    {
        printf("svc_wan_init: ...... reg event fail \n");
    }
    else
    {
	cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
        printf("svc_wan_init: ...... reg event ok \n");
    }
    
    creat_wan_mgr_polling_pthread();
    return 0;
}

static struct host_service_intf  svc_wan_mgr_intf = {
    .id = HOST_SERVICE_WANMGR_ID,
    .flag = 0,
    .start = svc_wan_start,
    .stop = svc_wan_stop,
    .init = svc_wan_int
};


struct host_service_intf* svc_wan_main(char* param);
struct host_service_intf* svc_wan_main(char* param)
{
    return &svc_wan_mgr_intf;
}



