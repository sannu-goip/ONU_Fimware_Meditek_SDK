
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


#ifndef __SVC_WAN_RELATED_MGR_H__
#define __SVC_WAN_RELATED_MGR_H__

#define WAN_RELATED_EVENT_FIREWALL_UPDATE 	1
#define WAN_RELATED_EVENT_ACL_UPDATE 		2
#define WAN_RELATED_EVENT_TIMEZONE_UPDATE	3
#define WAN_RELATED_EVENT_APPFILTER_UPDATE	4

#define MAX_VIRSERV_RULE 10

#define MACFILTER_SH	"/tmp/etc/macfilter.sh"
#define IPUPFILTER_SH	"/tmp/etc/ipupfilter.sh"
#define IPDOWNFILTER_SH	"/tmp/etc/ipdownfilter.sh"
#define INCOMINGFILTER_SH "/tmp/etc/incomingfilter.sh"

#define POLICY_ACCEPT	1
#define POLICY_DROP		0

#define VIRSERV_SH		"/usr/script/vserver.sh"
#define VIRSERV_PATH 	"/var/run/%s/vserver%s" 

#define DEVACCOUNT_STATUS    "registerStatus"
#define DEVACCOUNT_RESULT  "registerResult"
#define DEVACCOUNT_RETRYLIMIT  "retryLimit"
#define DEVACCOUNT_RETRYTIMES    "retryTimes"
#define DEVACCOUNT_USERNAME    "userName"
#define DEVACCOUNT_USERPASSWD    "userPasswordDEV"
#define DEVACCOUNT_WEBPAGELOCK  "webpageLock"
#define DEVACCOUNT_FAILUREWARNING  "failureWarning"
#define RESETRETRY	180

typedef struct wan_related_cfg_update_event {
    char path[64];
}wan_related_cfg_update_t;

enum if_state
{
	IF_STATE_DOWN = 0,
	IF_STATE_UP
};
int svc_wan_related_igmpproxy_update(void);
int svc_wan_related_mldproxy_update(void);
int svc_wan_related_mgr_start(void);
int svc_wan_related_mgr_stop(void);
int svc_wan_related_mgr_handle_event(int event,wan_related_cfg_update_t* path);
char *string_tolower(char *string);
char *escape_special_character(char *inp, char *outp, int len);
int system_escape(char *cmd);
void fputs_escape(char *cmd, FILE *fp);
void fileRead(char *path, char *buf, int size);
void check_and_set_l7filter(unsigned int new_filter_state);
int isValidDnsIp(char * dnsIP);
#if defined(TCSUPPORT_CT_E8GUI)
int ipfilter_check_switch(void);
#endif
int ipfilter_check_active();
int svc_wan_related_entry_delete(wan_related_cfg_update_t* param);
int svc_wan_related_ipv4_up(wan_related_cfg_update_t* param);
int svc_wan_related_ipv4_down(wan_related_cfg_update_t* param);
int svc_wan_related_ipv6_up(wan_related_cfg_update_t* param);
int svc_wan_related_get_wan_conf(char* path);
void svc_wan_related_free_all_wan_conf(void);
int resyncNTPServer(char *if_name, int if_index, int state);
#if defined(TCSUPPORT_CT_JOYME4)
int creat_boot_wan_ntp_check_pthread(void);
#endif

#endif

