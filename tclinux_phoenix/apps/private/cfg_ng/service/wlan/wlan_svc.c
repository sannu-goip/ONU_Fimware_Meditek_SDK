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
#include <stdlib.h>
#include <pthread.h>
#include <svchost_api.h>
#if defined(TCSUPPORT_CT_JOYME2)
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#endif
#include "wlan_mgr.h"
#if defined(TCSUPPORT_ECNT_MAP)
#include "wlan_mesh.h"
#endif
static int svc_wlan_internal_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	int evt_id = 0;
	pt_wlan_evt param = NULL;
	int ret = 0;

	evt_id = svchost_get_event_id(pbuf);
	param = (pt_wlan_evt)svchost_get_event_val(pbuf);
	
	ret = svc_wlan_mgr_internal_handle_event(evt_id, param);
	
	return ret ;
}

#if 0
static int svc_wlan_external_deal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{

	unsigned int evt_id = 0;
	pt_wlan_evt param = NULL;
	int ret = 0;


	evt_id = svchost_get_event_id(pbuf);
	param = (pt_wlan_evt)svchost_get_event_val(pbuf);

	ret = svc_wlan_mgr_external_handle_event(evt_id, param);
	
	return ret;

}	
#endif

#if defined(TCSUPPORT_CT_JOYME2)
static void signal_handler(int signum)
{
	char msg[256] = {0}, cur_time[64] = {0};
	char reason[128] = {0}, logs[64] = {0};
	FILE *fp = NULL;
	int ret = 0;

	fp = fopen(WIFI_SEND_FLAG, "w");
	if (fp) fclose(fp);

	get_current_time(cur_time, sizeof(cur_time));
	switch (signum) {
		case SIGBUS:
		case SIGSEGV:
		case SIGHUP:
		case SIGABRT:
		case SIGTERM:
		case SIGFPE:
		default:
			snprintf(reason, sizeof(reason), "receive error signal %d.", signum);
			break;
	}
	strncpy(logs, "no error logs", sizeof(logs) - 1);
	snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"%s\",\"Logs\":\"%s\"}",
								CTC_GW_SERVICE_NAME_WIFI, cur_time, reason, logs);
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_PROC_STAT,
					msg,
					strlen(msg)+1);
	
	switch (signum) {
		case SIGBUS:
		case SIGSEGV:
		case SIGHUP:
		case SIGABRT:
		case SIGTERM:
		case SIGFPE:
			exit(1);
		default:
			break;
	}

	return;
}

static int init_signal_handler(void)
{	
	signal(SIGHUP, signal_handler); 	/* quit */
	signal(SIGABRT, signal_handler); 	/* quit */
	//signal(SIGINT, signal_handler); 	/* not quit */
	signal(SIGTERM, signal_handler); 	/* quit */
	//signal(SIGQUIT, signal_handler); 	/* no quit */
	signal(SIGBUS, signal_handler); 	/* quit */
	signal(SIGSEGV, signal_handler); 	/* quit */
	signal(SIGFPE, signal_handler); 	/* quit */

	return 0;
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

#if defined(TCSUPPORT_ECNT_MAP)
static int svc_wlan_mesh_external_evt_handle(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	unsigned int evt_id = 0;
	int ret = 0;

	evt_id = svchost_get_event_id(pbuf);
	switch (evt_id)
	{
		case EVT_UPDATE_DAT :
		{
			ret = wlan_mesh_update_dat();
			break;
		}
		case EVT_RENEW_BSSINFO :
		{
			ret = wlan_mesh_renew_bssinfo(pbuf);
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

static struct host_service_event svc_wlan_mgr_event[] = {
	{EVT_CFG_EXTERNAL, cfg_external_evt_handle},
	{EVT_WLAN_INTERNAL, svc_wlan_internal_deal},
	/*{EVT_WLAN_EXTERNAL, svc_wlan_external_deal},*/
#if defined(TCSUPPORT_ECNT_MAP)
	{EVT_MESH_EXTERNAL, svc_wlan_mesh_external_evt_handle},
#endif	
};

static int 
svc_wlan_start(struct host_service* svc)
{
	printf("svc_wlan_start: ........ \n");
	if(svc_wlan_boot() != 0)
		printf("svc_wlan_start failed.\n");

	return 0;
}

static int 
svc_wlan_stop(struct host_service* svc)
{
	printf("svc_wlan_stop is called. \n");
	return 0;
}

static int 
svc_wlan_init(struct host_service* svc)
{
	int nevt, res = 0;
	pthread_t thread_id,thread_id2;
	pthread_attr_t thread_attr,timerthread_attr;
	pthread_t timerthread_id,Rssithread_id;
	pthread_attr_t Rssidthread_attr;
	
#if defined(TCSUPPORT_CT_JOYME2)
	init_signal_handler();
#endif
	nevt = sizeof(svc_wlan_mgr_event) / sizeof(struct host_service_event); 	
	if (svchost_reg_event(svc, svc_wlan_mgr_event, nevt) != nevt)
		printf("svc_wlan_init: ...... reg event fail !\n");
	else
	{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
		printf("svc_wlan_init: ...... reg event ok \n");
	}

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED); 
	if(pthread_create(&thread_id, &thread_attr, (void *)wlan_cc, NULL) != 0){
		fprintf(stderr, "pthread_create wlan_cc error!!\n");
		exit(0);
	}
#ifdef TCSUPPORT_CT_WLAN_JOYME3
	pthread_attr_init(&Rssidthread_attr);
	pthread_attr_setdetachstate(&Rssidthread_attr, PTHREAD_CREATE_DETACHED); 
	if(pthread_create(&Rssithread_id, &Rssidthread_attr, (void *)wlan_MonitorTask, NULL) != 0){
		fprintf(stderr, "pthread_create wlan_MonitorTask error!!\n");
		exit(0);
	}

	if(pthread_create(&thread_id2,NULL,(void*)wlan_driver_event_user,NULL)!=0){
		fprintf(stderr, "pthread_create wlan_driver_event_user error!!\n");
		exit(0);
	}
#endif

#ifdef TCSUPPORT_WLAN_VENDIE
	pthread_attr_init(&timerthread_attr);
	pthread_attr_setdetachstate(&timerthread_attr, PTHREAD_CREATE_DETACHED); 
	if(pthread_create(&timerthread_id, &timerthread_attr, (void *)timer_tool, NULL) != 0){
		fprintf(stderr, "pthread_create timer_tool error!!\n");
		exit(0);
	}
#endif	
	return 0;
}

static struct host_service_intf svc_wlan_intf = {
	.id = HOST_SERVICE_WLANMGR_ID,
	.flag = 0,
	.start = svc_wlan_start,
	.stop = svc_wlan_stop,
	.init = svc_wlan_init
};

struct host_service_intf* svc_wlan_main(char* param);

struct host_service_intf* svc_wlan_main(char* param)
{
	return &svc_wlan_intf;
}



