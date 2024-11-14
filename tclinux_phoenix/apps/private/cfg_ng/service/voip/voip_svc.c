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

#include "voip_common.h"
#include "voip_cfg.h"
#include "voip_mgr.h"
#include "voip_polling.h"
#include "cfg_cli.h"

extern int voipProtocol;


static int svc_voip_internal_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_voip_evt p_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    SVC_VOIP_NOTICE_INFO("receive voip internal msg evt: 0x%08x\n", evt_id);

    switch(evt_id)
    {
        case EVT_CFG_VOIP_SIP_NORMAL_UPDATE:
        case EVT_CFG_VOIP_H248_NORMAL_UPDATE:
        case EVT_CFG_VOIP_SIP_SIMULATE_UPDATE:
        case EVT_CFG_VOIP_H248_SIMULATE_UPDATE:
        case EVT_VOIP_IAD_START:
        case EVT_VOIP_SRV_DEBUG_LEVEL:
        {
            p_evt_data = (pt_voip_evt)svchost_get_event_val(pbuf);
            if(NULL == p_evt_data)
            {
                ret = -1;
                SVC_VOIP_WARN_INFO("Null p_evt_data.\n");
                return ret;
            }
            ret = svc_voip_mgr_handle_event(evt_id, p_evt_data);
            break;
        }
	case EVT_VOIP_BOOT2_READY:
            svc_voip_mgr_handle_boot2_ready();
            break;
        default:
        {
            SVC_VOIP_WARN_INFO("unknown voip internal evt: 0x%08x\n", evt_id);
            break;
        }
    }

    return ret ;
}

static int svc_voip_external_evt_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_voip_evt voip_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    SVC_VOIP_NOTICE_INFO("receive voip external msg evt: 0x%x\n", evt_id);

    switch(evt_id)
    {
        case EVT_VOIP_LINE_STATE:
        case EVT_VOIP_REG_STATE_UNREGED:
        case EVT_VOIP_REG_STATE_REGED:
        case EVT_VOIP_IAD_COMPLETE:
        case EVT_VOIP_SIMULATE_COMPLETE:
        {
            voip_evt_data = (pt_voip_evt)svchost_get_event_val(pbuf);
            if(NULL == voip_evt_data)
            {
                ret = -1;
                SVC_VOIP_WARN_INFO("Null voip_evt_data.\n");
                return ret;
            }

            ret = svc_voip_mgr_handle_event(evt_id, (void*)voip_evt_data);
            break;
        }
        default:
        {
            SVC_VOIP_WARN_INFO("unknown voip external evt: 0x%08x\n", evt_id);
            break;
        }
    }

    return ret ;

}

static int svc_wan_external_evt_voip_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_wan_evt wan_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;
    char IP_Protocol[8] = {0};

    evt_id = svchost_get_event_id(pbuf);
    SVC_VOIP_NOTICE_INFO("receive wan external msg evt: 0x%x\n", evt_id);

    switch(evt_id)
    {
        case EVT_WAN_IPV4_UP :
        case EVT_WAN_IPV6_UP :
        case EVT_WAN_XPON_UP :
        case EVT_WAN_IPV4_DOWN :
        case EVT_WAN_IPV6_DOWN :
        case EVT_WAN_XPON_DOWN :
        {
#ifdef TCSUPPORT_IPV6
            if(cfg_get_object_attr("root.voipadvanced.common", "IPProtocol", IP_Protocol, sizeof(IP_Protocol)) < 0){
                strcpy(IP_Protocol, "IPV4");
            }
#endif
            if(((EVT_WAN_IPV4_DOWN == evt_id || EVT_WAN_IPV4_UP == evt_id) && strcmp(IP_Protocol, "IPV4"))
                || ((EVT_WAN_IPV6_DOWN == evt_id || EVT_WAN_IPV6_UP == evt_id) && strcmp(IP_Protocol, "IPV6"))){
                SVC_VOIP_WARN_INFO("Update info is not same with the Ip protocol used by VoIP, so update is break off.\n");
                return ret;
            }
            wan_evt_data = (pt_wan_evt)svchost_get_event_val(pbuf);
            if(NULL == wan_evt_data)
            {
                ret = -1;
                SVC_VOIP_WARN_INFO("Null wan_evt_data.\n");
                return ret;
            }
            ret = svc_voip_mgr_handle_wan_event(evt_id, (void*)wan_evt_data);
            break;
        }
        default:
       {
            SVC_VOIP_WARN_INFO("unknown wan external evt: 0x%08x\n", evt_id);
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

static struct host_service_event svc_voip_mgr_event[] = {
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
	{EVT_VOIP_INTERNAL, svc_voip_internal_evt_deal},
	{EVT_VOIP_EXTERNAL, svc_voip_external_evt_deal},
	{EVT_WAN_EXTERNAL,  svc_wan_external_evt_voip_deal},
};


static int svc_voip_start(struct host_service* svc)
{
	printf("svc_voip_start: ...... \n");
	/*TODO:realize this function*/


	return 0;
}

static int svc_voip_stop(struct host_service* svc)
{
	printf("svc_voip_stop: ............ \n");
	/*TODO:realize this function*/	


	return 0;
}


static int svc_voip_int(struct host_service* svc)
{
	int num = sizeof(svc_voip_mgr_event) / sizeof(struct host_service_event);
	printf("svc_voip_int: num = %d\n", num);
	if (svchost_reg_event(svc,svc_voip_mgr_event,num) != num)
		printf("svc_voip_init: ...... reg event fail \n");
	else
	{	
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
		printf("svc_voip_init: ...... reg event ok \n");
	}

	creat_voip_mgr_polling_pthread();
	return 0;
}

static struct host_service_intf  svc_voip_mgr_intf = {
	.id = HOST_SERVICE_VOIPMGR_ID,
	.flag = 0,
	.start = svc_voip_start,
	.stop = svc_voip_stop,
	.init = svc_voip_int
};

struct host_service_intf* svc_voip_main(char* param);
struct host_service_intf* svc_voip_main(char* param)
{
	return &svc_voip_mgr_intf;
}

