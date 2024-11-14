
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


#ifndef __SVC_WAN_MGR_H__
#define __SVC_WAN_MGR_H__

#include "wan_common.h"

enum _wan_link_op_
{
    E_WAN_LINK_DISABLE_2_ENABLE = 1, 
    E_WAN_LINK_ENABLE_2_DISABLE = 2, 
    E_WAN_LINK_ENABLE_2_ENABLE  = 3, 
};

#if defined(TCSUPPORT_CT_UBUS)
#define NP_LANHOST_MGR_KO		"np_lanhost_mgr.ko"
#define NP_LANHOST_MGR_PROC		"/proc/tc3162/np_lanhost_mgr"

#define NP_BRIDGE_IP_DEF		"192.168.10.0"
#define NP_BRIDGE_MASK_DEF		"255.255.255.0"
#define NP_BRIDGE_GW_DEF		"192.168.10.1"
#define NP_ROUTE_PARAM_DEF		"0.0.0.0"
#endif


int svc_wan_mgr_handle_event(int event,char* buf);

int svc_wan_update_opt_cfg(wan_entry_obj_t* obj);

int svc_wan_set_info(wan_entry_obj_t* obj,int stat);

int svc_wan_start_all_wan_entry(int isInternet);

int svc_wan_handle_conn_evt(wan_entry_obj_t* obj,int evt);


int svc_wan_stop_all_wan_entry(void);
#if defined(TCSUPPORT_WAN_ETHER)
void check_ether_wan_state(void);
#endif
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
void check_Active_ether_wan_state(void);
#endif
void svc_wan_handle_event_conn_lostv4_deal(wan_entry_obj_t* obj);
void svc_wan_handle_event_conn_lostv6_deal(wan_entry_obj_t* obj);

#if defined(TCSUPPORT_CT_UBUS)
int np_wan_update_proc(char *ip, char *mask, char *gateway, char *proc);
#endif

#endif

