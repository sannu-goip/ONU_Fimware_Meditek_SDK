
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
#include "other_mgr.h"
#include "other_mgr_polling.h"
#include "cfg_cli.h"


static int svc_other_evt_internal_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	int evt_id = 0;
	other_evt_t *param = NULL;
	int ret = 0;
	
	if((evt_id = svchost_get_event_id(pbuf)) < 0)
	{
		printf("svc_other_evt_internal_deal: evt_id get fail.");
		return -1;
	}
	if((param = (other_evt_t *)svchost_get_event_val(pbuf)) == NULL)
	{
		printf("svc_other_evt_internal_deal: param get fail.");
		/*return 0;*/
	}
	
	ret = svc_other_mgr_handle_event(evt_id, param);
	
	return ret ;
}

static int svc_other_evt_wan_external_deal
(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
    pt_wan_evt wan_evt_data = NULL;
    int evt_id = 0;
    int ret = 0;

    evt_id = svchost_get_event_id(pbuf);
    switch ( evt_id )
    {
        case EVT_WAN_XPON_UP:
			svc_other_mgr_external_handle_event(EVT_CFG_OTHER_SNMP_SIGNAL);
			break;
        case EVT_WAN_XPON_DOWN:
			svc_other_mgr_external_handle_event(EVT_CFG_OTHER_SNMP_SIGNAL);
			break;
        default:
			break;
    }

	return ret;
}

#if 0
static int svc_other_evt_external_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{

	unsigned int evt_id = 0;
	other_evt_t *param = NULL;
	int ret = 0;


	if((evt_id = svchost_get_event_id(pbuf)) < 0)
	{
		printf("svc_other_evt_internal_deal: evt_id get fail.");
		return -1;
	}
	if((param = (other_evt_t *)svchost_get_event_val(pbuf)) < 0)
	{
		printf("svc_other_evt_internal_deal: param get fail.");
		return -1;
	}
	
	switch (evt_id)
	{

		default :
		{

			printf("svc_other_evt_external_deal: path[%s]  event [%d]  \n", param->buf, evt_id);	
			break;
		}
	}
	
	return ret;

}
#endif

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

static struct host_service_event svc_other_mgr_event[] = {
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
	{EVT_OTHER_INTERNAL, svc_other_evt_internal_deal},
	/*{EVT_OTHER_EXTERNAL, svc_other_evt_external_deal}*/
	{EVT_WAN_EXTERNAL, svc_other_evt_wan_external_deal},
};


static int svc_other_start(struct host_service* svc)
{
	printf("svc_other_start: ...... \n");
#if 0	
	svc_other_start_lan();
#endif	
	return 0;
}

static int svc_other_stop(struct host_service* svc)
{
	printf("svc_other_stop: ............ \n");
#if 0	
	svc_other_stop_lan();
#endif
	return 0;
}


static int svc_other_init(struct host_service* svc)
{
	int num = sizeof(svc_other_mgr_event) / sizeof(struct host_service_event);
	
	if (svchost_reg_event(svc,svc_other_mgr_event,num) != num)
		printf("svc_other_init: ...... reg event fail \n");
	else
	{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
		printf("svc_other_init: ...... reg event ok \n");
	}

	creat_other_mgr_polling_pthread();
        checkOtherSvcBoot();
	return 0;
}

static struct host_service_intf  svc_other_mgr_intf = {
	.id = HOST_SERVICE_OTHERMGR_ID,
	.flag = 0,
	.start = svc_other_start,
	.stop = svc_other_stop,
	.init = svc_other_init
};


struct host_service_intf* svc_other_main(char* param);
struct host_service_intf* svc_other_main(char* param)
{
	return &svc_other_mgr_intf;
}



