
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
#include "cfg_cli.h"

#include "wan_related_mgr.h"
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
#include "filter_polling.h"
#endif
#if defined(TCSUPPORT_VXLAN)
#include "wan_related_vxlan_cfg.h"
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#include "wan_related_qos_cfg.h"
#endif

static int svc_wan_related_internal_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	int evt_id = 0;
	wan_related_cfg_update_t *param = NULL;
	int ret = 0;
	
	evt_id = svchost_get_event_id(pbuf);
	param = (wan_related_cfg_update_t *)svchost_get_event_val(pbuf);
	
	ret = svc_wan_related_mgr_handle_event(evt_id, param);
	
	return ret ;
}

#if 0
static int svc_wan_related_external_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{

	unsigned int evt_id = 0;
	/*wan_related_cfg_update_t *param = NULL;*/
	int ret = 0;


	evt_id = svchost_get_event_id(pbuf);
	/*
	param = (wan_related_cfg_update_t *)svchost_get_event_val(pbuf);
	*/	
	switch (evt_id)
	{

		default :
		{
			break;
		}
	}
	
	return ret;

}
#endif

static int svc_wan_related_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	int evt_id = 0;
	wan_related_cfg_update_t *param = NULL;

	evt_id = svchost_get_event_id(pbuf);
	param = (wan_related_cfg_update_t *)svchost_get_event_val(pbuf);

	switch (evt_id)
	{
		case EVT_WAN_ENTRY_DELETE:
			if(param != NULL){
				svc_wan_related_entry_delete(param);
			}		
			break;
		case EVT_WAN_ENTRY_UPDATE: 
			if(param != NULL){
				svc_wan_related_get_wan_conf(param->path);
			}
			break;
		case EVT_WAN_IPV4_UP: 
			if(param != NULL){
				svc_wan_related_ipv4_up(param);
			}
			break;
		case EVT_WAN_IPV4_DOWN:
			if(param != NULL){
				svc_wan_related_ipv4_down(param);
			}
			break;
		case EVT_WAN_IPV6_UP: 
			if(param != NULL){
				svc_wan_related_ipv6_up(param);
			}
			break;
		case EVT_WAN_IPV6_DOWN:
			break;
		case EVT_WAN_BRIDGE_UP:	
			break;
		case EVT_WAN_BRIDGE_DOWN:
			break;
		case EVT_WAN_UPDATE_CONNREQ_PORT: 
			break;
#if defined(TCSUPPORT_VXLAN)
		case EVT_WAN_XPON_UP:
			svc_wan_related_vxlan_xpon_handle(E_VXLAN_XPON_UP);
			break;
		case EVT_WAN_XPON_DOWN:
			svc_wan_related_vxlan_xpon_handle(E_VXLAN_XPON_DOWN);
			break;
#endif
		default :
		{
			printf("svc_wan_external_deal: path[%s]  event [%d]  \n", param->path, evt_id);	
			break;
		}
	}
	
#if defined(TCSUPPORT_CT_JOYME4)
	switch ( evt_id )
	{
		case EVT_WAN_IPV4_DOWN:
		case EVT_WAN_IPV4_UP:
		case EVT_WAN_IPV6_DOWN:
		case EVT_WAN_IPV6_UP:
		case EVT_WAN_BRIDGE_UP:
		case EVT_WAN_BRIDGE_DOWN:
			update_qos_template_for_other_special_service();
			break;
	}
#endif

	return 0;
}

static int svc_wan_related_handle_pon_external_evt(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	wan_related_cfg_update_t *param = NULL;
	int evt_id = 0;
	int ret = 0;

	evt_id = svchost_get_event_id(pbuf);
	param = (wan_related_cfg_update_t*)svchost_get_event_val(pbuf);
	
	switch (evt_id)
	{
	    case EVT_PON_TRAFFIC_STATUS :
			if(param != NULL)
			{	
				if(strcmp(param->path, "up") == 0)
				{
#if defined(TCSUPPORT_CUC_LANDING_PAGE)
					system("iptables -t nat -F PRE_PONSTATE");
					system("iptables -t nat -Z PRE_PONSTATE");
					system("iptables -t nat -A PRE_PONSTATE -j RETURN");
#endif
				}
				else
				{	
#if defined(TCSUPPORT_CUC_LANDING_PAGE)
					system("iptables -t nat -F PRE_PONSTATE");
					system("iptables -t nat -Z PRE_PONSTATE");
					system("iptables -t nat -A PRE_PONSTATE -i br0 -p tcp --dport 80 -j REDIRECT");
#endif
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


static struct host_service_event svc_wan_related_mgr_event[] = {
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
	{EVT_WAN_RELATED_INTERNAL, svc_wan_related_internal_deal},
	/*{EVT_WAN_RELATED_EXTERNAL, svc_wan_related_external_deal},*/
	{EVT_WAN_EXTERNAL, 	svc_wan_related_handle},
	{EVT_PON_EXTERNAL, svc_wan_related_handle_pon_external_evt},
};


static int svc_wan_related_start(struct host_service* svc)
{
	printf("svc_wan_start: ...... \n");
	svc_wan_related_mgr_start();
	
	return 0;
}

static int svc_wan_related_stop(struct host_service* svc)
{
	printf("svc_wan_stop: ............ \n");

	svc_wan_related_free_all_wan_conf();
	svc_wan_related_mgr_stop();
#if defined(TCSUPPORT_VXLAN)
	svc_wan_related_stop_all_vxlan();
#endif

	return 0;
}


static int svc_wan_related_init(struct host_service* svc)
{
	int num = sizeof(svc_wan_related_mgr_event) / sizeof(struct host_service_event);
	
	if (svchost_reg_event(svc,svc_wan_related_mgr_event,num) != num)
		printf("svc_wan_related_init: ...... reg event fail \n");
	else
	{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
		printf("svc_wan_related_init: ...... reg event ok \n");
	}
#if defined(TCSUPPORT_CT_JOYME4)  || defined(TCSUPPORT_CT_UBUS)
	creat_filter_polling_pthread();
#endif

	return 0;
}

static struct host_service_intf  svc_wan_related_mgr_intf = {
	.id = HOST_SERVICE_WANRELATEDMGR_ID,
	.flag = 0,
	.start = svc_wan_related_start,
	.stop = svc_wan_related_stop,
	.init = svc_wan_related_init
};


struct host_service_intf* svc_wan_related_main(char* param);
struct host_service_intf* svc_wan_related_main(char* param)
{
	return &svc_wan_related_mgr_intf;
}



