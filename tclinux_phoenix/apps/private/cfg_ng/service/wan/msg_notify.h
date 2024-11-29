
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

#ifndef __SVC_WAN_MSG_NOTIFY_H__
#define __SVC_WAN_MSG_NOTIFY_H__

/*compatibility with the previous version*/
enum if_state
{
    IF_STATE_DOWN = 0,
    IF_STATE_UP,
    IF_STATE_GETIP,
    IF_STATE_GETGATEWAY,
    IF_STATE_GETDNS,             /*DNS server address*/
    IF_STATE_DOWN6,
    IF_STATE_UP6,
    IF_STATE_GETIP6,
    IF_STATE_GETGATEWAY6,
    IF_STATE_GETDNS6,             /*DNS server ipv6 address*/
    IF_STATE_GETPD,               /*Get prefix delegation for IPv6 DHCPv6-PD mode*/
    IF_STATE_MODIFY,
    BRIDGE_IF_STATE_DOWN,
    BRIDGE_IF_STATE_UP,
};

void sendMsgtoCwmp(int cwmpMsgType, int if_state, int if_index);

void svc_wan_mgr_send_evt(int type, int evt_id, char* buf);

#define WANCHANGEFIFO  "/tmp/fifo_wanchange"
void sendfifo2mobile(char input);


typedef struct
{
    char wanIpAddr[16];
    char wanSubnetMask[16];
    char ipv6WanAddr[64];
    int wanPrefixLen;
#if defined(TCSUPPORT_CT_UBUS)
	char status[8];
#endif
}WanIpEvent;

void wan_ip_change_inform(char* path, int evt);

#if defined(TCSUPPORT_CT_UBUS)
void br_wan_ip_change_inform(char *dev, int evt);
#endif

#endif



