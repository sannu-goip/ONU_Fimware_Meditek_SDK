
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
#include <pthread.h>

#include "cfg_cli.h"
#include "cfg_romfile.h"
#include "type/cfg_types.h"
#include "type/cfg_type_voip.h"
#if defined(TCSUPPORT_CWMP_TR181)
#include "type/tr181/cfg_tr181_global.h" 
#endif

typedef int (*cfg_boot_func)(void);

static cfg_boot_func cfg_boot2[] = 
{
#if defined(TCSUPPORT_CT_PMINFORM) || defined(TCSUPPORT_CT_JOYME2)
	svc_cfg_boot_pminform,
#endif
	svc_cfg_boot_timezone,
#if defined(TCSUPPORT_CT_IPV4_RADIO)
	svc_cfg_boot_sys,
#endif
	svc_cfg_boot_dhcprelay,
#if defined(TCSUPPORT_VOIP)
	svc_cfg_boot_voip,
#endif
#ifdef IPV6
	svc_cfg_boot_radvd,
	svc_cfg_boot_dhcp6s,
#endif
#ifdef CWMP
	svc_cfg_boot_switchpara,
#endif
#ifdef TCSUPPORT_WLAN_VENDIE
	svc_cfg_boot_proberxvsie,
	svc_cfg_boot_proberespvsie,
#endif
#if defined(TCSUPPORT_CT_L2TP_VPN) || defined(TCSUPPORT_CT_VPN_PPTP)
	svc_cfg_vpnlist_boot,
#endif
#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
	svc_cfg_boot_infovoippoorql,
#endif
	svc_cfg_boot_firewall,
#ifdef IPV6
	svc_cfg_boot_route6,
#endif
	svc_cfg_boot_dmz,
	svc_cfg_boot_virServ,
#if defined(TCSUPPORT_CT_E8GUI)	
	svc_cfg_boot_algswitch,
#endif
#if defined(TCSUPPORT_PORT_TRIGGER)
	svc_cfg_porttriggering_boot,
#endif
	svc_cfg_boot_snmpd,
	svc_cfg_boot_upnpd,
#if defined(TCSUPPORT_CT_E8DDNS)
	svc_cfg_boot_ddns,
#endif
#if defined(TCSUPPORT_CWMP_TR181)
	svc_cfg_boot_tr181,
#endif
#ifdef CWMP
	svc_cfg_boot_cwmp,
#endif

#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT) || defined(TCSUPPORT_TRUE_LANDING_PAGE)
	svc_cfg_boot_portal,
#endif
#if defined(CT_COM_DEVICEREG)
	svc_cfg_boot_deviceaccount,
#endif
#ifdef TCSUPPORT_QOS
	svc_cfg_boot_qos,
#endif
#ifdef TCSUPPORT_SSH
	svc_cfg_boot_ssh,
#endif
	svc_cfg_boot_acl,
	svc_cfg_boot_ipmacfilter,
	svc_cfg_boot_urlfilter,
#if defined(TCSUPPORT_CT_UBUS)
	svc_cfg_boot_dnsfilter,
#endif
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
	svc_cfg_boot_ctdiagnostic,
#endif
#ifdef TCSUPPORT_DMS
	svc_cfg_boot_dms,
#endif
#ifdef TCSUPPORT_SYSLOG
	svc_cfg_boot_syslog,
#endif
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
	svc_cfg_boot_accesslimit,
#endif

#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
	svc_cfg_boot_appftp,
#endif
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
	svc_cfg_boot_samba,
#endif
#if defined(TCSUPPORT_CT_LOOPDETECT)
	svc_cfg_boot_loopdetect,
#endif
#if defined(TCSUPPORT_CT_DS_LIMIT)
	svc_cfg_boot_dataspeedlimit,  
#endif
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
	svc_cfg_boot_mobile,
#endif
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
	svc_cfg_boot_logicid,
#endif
#endif
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
	svc_cfg_boot_pppoeemulator,
#endif
#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
	svc_cfg_boot_ipoe_emulator,
#endif
#if defined(TCSUPPORT_CT_JOYME)
	svc_cfg_boot_osgiupgrade,    
#endif
#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
	svc_cfg_boot_bandwidth,         	
#endif
#if defined(TCSUPPORT_CMCCV2)
	svc_cfg_boot_voipmonitor,
	svc_cfg_boot_wlanshare,
	svc_cfg_boot_apipermission,
	svc_cfg_cmcc_info_boot,
	svc_cfg_vpndestlist_boot,
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
	svc_cfg_boot_trafficmonitor,
	svc_cfg_boot_trafficforward,
	svc_cfg_boot_trafficmirror,
	svc_cfg_boot_trafficqosservice,
	svc_cfg_boot_trafficdetailprocess,
	svc_cfg_boot_guestssidinfo,
#endif
#if defined(TCSUPPORT_CUC)
	svc_cfg_boot_storageaccessright,
	svc_cfg_boot_accesscontrol,
#endif
	svc_cfg_boot_lanhost2,
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
	svc_cfg_timer_boot,
#if defined(TCSUPPORT_CT_JOYME2)
	svc_cfg_boot_incomingfilter,
#endif
#if defined(TCSUPPORT_CT_SDN)
	svc_cfg_boot_sdn,
#endif
#endif
#if defined(TCSUPPORT_VXLAN)
	svc_cfg_boot_vxlan,
#endif
	svc_cfg_boot_webcustom,
	svc_cfg_boot_autoexec,
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
	svc_cfg_boot_parental,
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	svc_cfg_boot_bridgedata_acl,
	svc_cfg_boot_downlinkqos,
#endif
#if defined(TCSUPPORT_STUN)
	svc_cfg_boot_stun,
#endif
	NULL
};

static cfg_boot_func cfg_boot[] = 
{
#if defined(TCSUPPORT_CT_DSL_EX)
	svc_cfg_boot_adsl,
#endif
#if defined(TCSUPPORT_ANDLINK)
	svc_cfg_boot_alinkmgr,
#endif
	svc_cfg_boot_wan,
	NULL
};

static cfg_boot_func cfg_boot1[] = 
{
	svc_cfg_boot_lan,
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	svc_cfg_boot_dhcpd,
#endif
#if defined(TCSUPPORT_ECNT_MAP)
	svc_cfg_boot_mesh,
#endif
#ifdef TCSUPPORT_WLAN
	svc_cfg_boot_wlan,
#ifdef TCSUPPORT_WLAN_AC
	svc_cfg_boot_wlan11ac,
#endif
#endif
	svc_cfg_boot_account,
#if defined(TCSUPPORT_CT_JOYME2)
#ifdef TCSUPPORT_WLAN
	svc_cfg_wlanmapping_boot,
#endif
#endif
	svc_cfg_boot_system,
#if defined(TCSUPPORT_VOIP)
	svc_cfg_boot_voip_proto,
#endif
	NULL
};

static int svc_cfg_call_boot2(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{

	int i,n = sizeof(cfg_boot2) /  sizeof(cfg_boot_func);

	cfg_obj_send_event(EVT_CFG_EXTERNAL, EVT_CFG_LOAD_UPDATE, NULL, 0);
	for(i= 0 ; i < n; i++)
	{
		if (cfg_boot2[i])
			cfg_boot2[i]();
	}
	return 0;
}

static int svc_cfg_call_boot(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{
	int i = 0;
	int n = sizeof(cfg_boot) /  sizeof(cfg_boot_func);

	cfg_obj_send_event(EVT_CFG_EXTERNAL, EVT_CFG_LOAD_UPDATE, NULL, 0);
	for(i= 0 ; i < n; i++)
	{
		if (cfg_boot[i])
			cfg_boot[i]();
	}	

	sleep(3);

	n = sizeof(cfg_boot1) /  sizeof(cfg_boot_func);
	for(i= 0 ; i < n; i++)
	{
		if (cfg_boot1[i])
			cfg_boot1[i]();
	}
	unlink("/tmp/call_boot_ok");
	tcdbg_printf("call_boot call boot success \n");

	return 0;
}

static int msg_cnt = 0;
static int svc_cfg_evt_internal(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{	
	int evt_id = 0;
	pt_cfg_evt param;
	
	param = (pt_cfg_evt)svchost_get_event_val(pbuf);
    evt_id = svchost_get_event_id(pbuf);
    switch (evt_id)
    {
        case EVT_CFG_TCAPI_SAVE :
        {
			if(!access("/tmp/rebootflag", 0))
			{
				tcdbg_printf("restore to default, no need to save.\n"); 		
				break;
			}
          	cfg_write_romfile_to_flash();
            break;
        }
		case EVT_CFG_UPGRADE_FIRMWARE :
		{
			char* value = param->buf;
			sys_upgrade_firmware(value);
			break;
		}
		case EVT_CFG_BOOT :
		{
			char* value = param->buf;
			sys_boot_effect(value);
			break;
		}
		case EVT_CFG_SVC_REG:
		{			
			msg_cnt++;
			break;
		}
        default:
        {
            break;
        }
    }
	

	return 0;
}

static int cfg_evt_hook = 1;

static  int svc_cfg_evt_log(struct host_service_call* pcall, struct host_service_pbuf* pbuf)
{

	FILE* fp = NULL;
	char param[64] = {0}, *ptr = NULL;
	unsigned int len = 0;
	
	ptr = svchost_get_param(pbuf,1,&len);
	
	if (ptr == NULL)
		sprintf(param,"NULL");
	else
		strncpy(param,ptr,sizeof(param) - 1);
	
	param[63] = 0;

/*	printf("svc_cfg_evt_log: EVT[%s] Param[%s] \n",pcall->meth,param); */
	
	if ((fp = fopen(CFG_EVT_LOG_PATH,"a")) == NULL)
		return 0;
	
	fprintf(fp,"EVT[%s] Param[%s] \n",pcall->meth,param);

	fclose(fp);
	
	return 0;
}


static struct host_service_method svc_cfg_meth[] = {
	{"boot",svc_cfg_call_boot},
	{"boot2",svc_cfg_call_boot2},	
};

static char  cfg_svc_all_evt[] = "*";

static struct host_service_event svc_cfg_event[] = {
	{.name = EVT_CFG_INTERNAL,.event = svc_cfg_evt_internal},
/*		
	{.name = "asyncsave",.event = svc_cfg_evt_save},
	{.name = cfg_svc_all_evt ,.event = svc_cfg_evt_log}
*/
};


static cfg_node_obj_t* svc_cfg_load_object(void)
{
	cfg_node_obj_t* node = NULL;

	if ((node = cfg_obj_load_from_type()) == NULL)
	{
		printf("svc_cfg_load_object: create obj from type fail \n");
		return NULL;
	}

	printf("Load cfg from file [%s] ..... \n",CFG_OBJ_FILE_PATH);
	
	shm_settree(node);

	cfg_obj_set_root(node);

	cfg_load_romfile();	

	return node;
}

void call_boot()
{
	char svc_num[8] = {0};

	while(1)
	{
		memset(svc_num, 0, sizeof(svc_num));
		fileRead("/tmp/svc_num", svc_num, sizeof(svc_num));

		if(svc_num[0] == '\0')
		{
			sleep(1);
			continue;
		}
		
		if(atoi(svc_num) > msg_cnt)
		{
			tcdbg_printf("call_boot wait svc start, msg_cnt=[%d] svc_num=[%s] \n", msg_cnt, svc_num);
			sleep(1);
		}
		else
		{
			system("echo 1 > /tmp/call_boot_ok");
			unlink("/tmp/svc_num");
			break;
		}

	}
	return;
}

static int svc_cfg_start(struct host_service* svc)
{
	cfg_node_obj_t* node = NULL;
	
	printf("svc_cfg_start: ........ \n");

	if ((node = (cfg_node_obj_t*)shm_gettree()) != NULL)
	{
		printf("svc_cfg_start: root not null \n");
	}

	if (node == NULL)
	{
		if ((node = svc_cfg_load_object()) == NULL)
		{
			printf("svc_cfg_start: load cfg object fail \n");
			return -1;
		}

		cfg_load_web_type_node();
		cfg_set_object_attr(CFG_LOAD_STATUS_FLAG_PATH,CFG_LOAD_STATUS_FLAG,CFG_LOAD_DONE);
		system("echo 1 > /tmp/cfg_load");
	}
	
	printf("svc_cfg_start: ........ ok [%08x]! \n",(unsigned int)node);

	return 0;
}

static int svc_cfg_stop(struct host_service* svc)
{
	printf("svc_cfg_stop is called \n");

	return 0;
}

static int svc_cfg_init(struct host_service* svc)
{
	int napi,nevt;
	pthread_t thread_id;
	pthread_attr_t thread_attr;

	napi = sizeof(svc_cfg_meth) / sizeof(struct host_service_method);

	nevt = sizeof(svc_cfg_event) / sizeof(struct host_service_event); 
	
	if (svchost_reg_method(svc,svc_cfg_meth,napi) != napi)
		printf("svc_cfg_start: reg api fail !\n");
	
	if (svchost_reg_event(svc,svc_cfg_event,nevt) != nevt)
		printf("svc_cfg_start: reg event fail !\n");
	else{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_SVC_REG, NULL, 0);
	}

	
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED); 
	if(pthread_create(&thread_id, &thread_attr, (void *)call_boot, NULL) != 0){
		fprintf(stderr, "pthread_create call_boot error!!\n");
		exit(0);
	}

	return 0;
}


static struct host_service_intf  svc_cfg_intf = {
	.id = HOST_SERVICE_CFGMGR_ID,
	.flag = 1,
	.start = svc_cfg_start,
	.stop = svc_cfg_stop,
	.init = svc_cfg_init
};

struct host_service_intf* svc_cfg_main(char* param);

struct host_service_intf* svc_cfg_main(char* param)
{
	if (strstr(param,"hook"))
		cfg_evt_hook = 1;

	if (strstr(param,"unhook"))
		cfg_evt_hook = 0;

	return &svc_cfg_intf;
}



