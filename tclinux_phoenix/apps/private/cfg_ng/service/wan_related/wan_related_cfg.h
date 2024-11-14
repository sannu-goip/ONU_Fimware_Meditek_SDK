
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

#ifndef __SVC_WAN_RELATED_CFG_H__
#define __SVC_WAN_RELATED_CFG_H__

#define FAIL -1
#define SUCCESS 0
#define APP_FILTER_SH	"/tmp/etc/AppFilter.sh"
#define APP_STOP_SH	"/usr/script/AppFilterStop.sh "
#define URL_FILTER_SH	"/tmp/etc/UrlFilter.sh"
#define URL_STOP_SH	"/usr/script/UrlFilterStop.sh "
#define URL_FILTER_BL_CLEAR	"/tmp/etc/UrlFilterClear.sh"
#define IGMPPROXY_CONF_PATH  "/etc/igmpproxy.conf"  
#define IGMPPROXY_DEA_PATH   "/userfs/bin/igmpproxy" 
#define DDNS_PATH "/etc/ddns.conf"
#define DDNS_PID_PATH "/var/log/ez-ipupdate.pid"
#if defined(TCSUPPORT_CT_JOYME)
#define ORAY_CONFIG_PATH "/usr/osgi/plugin-b/com.chinatelecom.econet.smartgateway.ddns/data/phlinux.conf"
#define ORAY_LOG_PATH "/usr/osgi/plugin-b/com.chinatelecom.econet.smartgateway.ddns/data/phddns.log"
#define ORAY_PATH "/usr/osgi/plugin-b/com.chinatelecom.econet.smartgateway.ddns/phddns"
#endif
#if defined(TCSUPPORT_CT_E8GUI)
#define MAX_DDNS_NUM 64
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
#define MAX_URL_RULE 33
#else
#define MAX_URL_RULE 100
#endif
#else
#define MAX_URL_RULE 16
#endif
#define LOCALTIME_FILE "/etc/localtime"
#define ZONEINFO_FILE "/usr/zoneinfo/"
#define TZ_FILE "/etc/TZ"
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
#define DIAG_TZ_FILE "/etc/DIAGTZ"
#endif

#define ACL_SH	"/tmp/etc/acl.sh"
#define ACL_STOP_SH "/usr/script/acl_stop.sh"
#define MAX_ACL_RULE 16

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
#define MAX_URLFILTER_NUM	33
#define MAX_URLLIST_NUM	99
#define MAX_DNSFILTER_NUM	32
#define MAX_DNSLIST_NUM	99
#define MAX_DURATION_PER_ENTRY	3

#define DURATION_START 		"StartTime"
#define DURATION_END 		"EndTime"
#define DURATION_REPEAT 	"RepeatDay"

typedef struct{
	unsigned int start[MAX_DURATION_PER_ENTRY];
	unsigned int end[MAX_DURATION_PER_ENTRY];
	unsigned char repeat_days;
	unsigned char matched_stat;
	unsigned char allday_flag;
}filter_duration_t;

typedef struct
{
	filter_duration_t duration;
}filter_setting_t;

extern filter_setting_t urlfilter_setting[MAX_URLFILTER_NUM];
extern filter_setting_t dnsfilter_setting[MAX_DNSFILTER_NUM];
#endif

int acl_check_filter(void);
int acl_get_active_interface(char interfaces[][8], const char* intf_tpye);
int svc_wan_related_acl_write(void);
int svc_wan_related_acl_execute(void);
int svc_wan_related_timezone_write(char* path, int ret);
int svc_wan_related_appfilter_write(void);
int app_check_filter(void);
int ipfilter_check_filter(void);
int url_check_filter(void);
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
int isStepInOutBoundary(int node_idx, filter_duration_t *duration, char *nodeName);
int initurlfilterduration(int idx);
int initdnsfilterduration(int idx);
int svc_wan_related_urlfilter_write(int idx);
#if defined(TCSUPPORT_CT_UBUS)
int svc_wan_related_dnsfilter_write();
#endif
#else
int svc_wan_related_urlfilter_write();
#endif
int svc_wan_related_urlfilter_boot(void);
int url_filter_deletewhiterules(int ipolicy);
int svc_wan_related_ipmacfilter_write(void);
int svc_wan_related_ipmacfilter_boot(void);
int svc_wan_related_igmpproxy_write(void);
int svc_wan_related_igmpproxy_execute(void);
int svc_wan_related_mldproxy_execute(void);
int svc_wan_related_ddns_write(void);
int svc_wan_related_ddns_execute(void);
void svc_wan_related_del_dmz_entry(int if_index);
void svc_wan_related_del_virserver_entry(int if_index);
void svc_wan_related_del_igmp_mld_proxy(char *if_name);
#if defined(TCSUPPORT_IGMP_PROXY)
void restart_igmpproxy(char *if_name);
#endif
#ifdef TCSUPPORT_MLD_PROXY
void restart_mldproxy(char *if_name, int if_index);
#endif
int svc_wan_handle_event_timezone_update(wan_related_cfg_update_t* param);
int svc_wan_handle_event_timezone_boot();
#if !defined(TCSUPPORT_CMCCV2)
void restart_dhcprelay(int if_index);
#endif

#if defined(TCSUPPORT_WLAN_8021X) 
void restart_rtdot1xd(void);
#endif
#if defined(TCSUPPORT_CMCCV2)
void restart_wantelnet();
#endif
#endif

