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

#ifndef __LAN_HOST_MSG_H__
#define __LAN_HOST_MSG_H__
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_lanhost.h>
#include <ecnt_event_global/ecnt_event_wifi.h>
#include <ecnt_event_global/ecnt_event_eth.h>
#include "lan_host_api.h"

#define NDA_RTA(r) 	((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#define RTA_LENGTH(len)	(RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))

int sock_add_epoll(int sock, int epoll);
int ecnt_event_handler(int sock, struct msghdr *pmsg, struct nlmsghdr *pnlh);
int wifi_update_handler(struct nlmsghdr *nlh);
int dhcp_reinit_hander(struct nlmsghdr *nlh);
int lan_up_handler(struct nlmsghdr *nlh);
int lan_down_handler(struct nlmsghdr *nlh);
int alink_bridge_dev_handler(struct nlmsghdr *nlh);
int np_bridge_dev_handler(struct nlmsghdr *nlh);
#if defined(TCSUPPORT_CT_JOYME4)
int dhcp_update_hander(struct nlmsghdr *nlh);
int ipv6_check_hander(struct nlmsghdr *nlh);
#endif

int create_ecnt_event_sock(int epoll);
int arp_event_handler(int sock, struct msghdr *pmsg, struct nlmsghdr *pnlh);
int arp_add_handler(char *lladdr, char *abuf, int ip_type);
int arp_del_handler(char *lladdr, char *abuf, int ip_type);
int create_arp_event_sock(int epoll);

#endif /* __LAN_HOST_MSG_H__ */
