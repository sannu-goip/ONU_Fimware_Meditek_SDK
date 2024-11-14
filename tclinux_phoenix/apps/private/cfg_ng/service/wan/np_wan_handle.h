
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

#ifndef __NP_WAN_HANDLE_H__
#define __NP_WAN_HANDLE_H__


/*************************************************************************************************/
/*************************************************************************************************/
#define NP_TIMEOUT (5)

#define NP_BR0_NO_RESET (0)
#define NP_BR0_RESET (1)

#define NP_TYPE_DHCP_DISABLE (0)
#define NP_TYPE_DHCP_ENABLE (1)
#define NP_TYPE_DHCPRELAY_ENABLE (2)

#define NP_WAN_UP (0)
#define NP_WAN_FIXED (1)

#define NP_APCLI_2_4_G (0)
#define NP_APCLI_5_G (1)

#define NP_AP_B_WAN_IDX (0)
#define WAN_IF_MARK (0x7f0000) 

#define NP_LANHOST_MGR_KO		"np_lanhost_mgr.ko"
#define NP_LANHOST_MGR_PROC		"/proc/tc3162/np_lanhost_mgr"

#define NP_BRIDGE_IP_DEF		"192.168.1.0"
#define NP_BRIDGE_MASK_DEF		"255.255.255.0"
#define NP_BRIDGE_GW_DEF		"192.168.1.1"
#define NP_ROUTE_PARAM_DEF		"0.0.0.0"

enum _np_wan_status_
{
	NP_WAIT_TIMEOUT = 0,
	NP_WAIT_BR_ALINK_UP,
	NP_BR_ALINK_IP_GET,
	NP_BR_ALINK_IP_GET_TIMEOUT,
	NP_BR_ALINK_IP_LOST,
	NP_FOUND_GATEWAY_REQ,
	NP_FOUND_GATEWAY_TIMEOUT,
	NP_WAIT_APCLI_UP,
	NP_APCLI_UP,
};

enum _np_wan_mode_
{
	NP_WAN_AP_NONE = 0,
	NP_WAN_AP_BRIDGE,
	NP_WAN_AP_ROUTE,
	NP_WAN_AP_CLIENT,
};

void getPreTime();
int np_wan_ntp_sync();
int np_wan_update_proc(char *ip, char *mask, char *gateway, char *proc, int is_bridge);
int np_wan_pre_handle_xpon_update(int evt, char *source);
int np_check_wan_boot();
int np_wan_pre_conf();
int np_wan_pre_get_timeout(int *eth_up_timeout, int *ap_wan_timeout);
int np_wan_start_br_alink_dhcpc(char *dev);
int np_wan_stop_br_alink_dhcpc(char *dev);
int np_wan_stop_ap_bridge();
int np_wan_start_ap_route();
int np_wan_stop_ap_route();
int np_wan_start_ap_client();
int np_wan_stop_ap_client();
int np_wan_ap_mode_update(char *path);
int np_wan_set_ap_default_route();
int np_wan_del_ap_default_route();
int np_wan_set_bridge_info(int stat, char *dev);
int np_check_manageIP();
int np_send_if3_request(int sync_flag);
int np_check_search_response();
int np_wan_if3_start();
/*int np_wan_switch_dhcp_mode(int dhcp_type, char *serverIP);*/
int np_wan_ap_br_alink_get4(char *dev);
int np_wan_ap_br_alink_lost4(char *dev);
int np_wan_start_by_fixed_mode(int mode);
int np_wan_stop_by_fixed_mode(int mode);
int check_ap_wan_fixed_mode(int flag);
int np_event_handle(int ap_status);
int np_wan_ap_boot();
int np_wan_start_process();

/* add eui64 link local address for br0 when bridge mode. */
#define ATTR_EUI_LLADDR			"BR0_EUI64_LLADDR"
#define ATTR_EUI_LLADDR2KO		"BR0_EUI64_LLADDR2KO"

#define	LL_OP_BRIDGE		1
#define	LL_OP_ROUTE			0
void update_br0_linklocal_address(int is_bridge);

#endif
