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

#ifndef __LAN_HOST_API_H__
#define __LAN_HOST_API_H__
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <pthread.h>
#include <string.h>
#include <linux/version.h>
#include <linux/sysinfo.h>
#include "debug.h"
#include "lan_host_mgr.h"
#if defined(TCSUPPORT_CUC)
#include <lan_port/lan_port_info.h>
#endif

#define		SYNC_NEW_ENTRY				(1<<0)
#define		SYNC_DEL_ENTRY				(1<<1)
#define		SYNC_UPDATE_STATE			(1<<2)
#define		SYNC_UPDATE_MAC				(1<<3)
#define		SYNC_UPDATE_IP				(1<<4) /* no ned save to flash */
#define		SYNC_UPDATE_CNT				(1<<5)
#define		SYNC_UPDATE_HOSTNAME		(1<<6)
#define		SYNC_UPDATE_ONLINE_TIME		(1<<7)
#define		SYNC_UPDATE_ACTIVE_TIME		(1<<8)
#define		SYNC_UPDATE_INACTIVE_TIME	(1<<9)
#define		SYNC_UPDATE_PORT			(1<<10)
#define		SYNC_UPDATE_POWER_LEVEL		(1<<11) /* no ned save to flash */
#define		SYNC_UPDATE_CONNTYPE		(1<<12)
#define		SYNC_UPDATE_NEGORATE		(1<<13)
#define		SYNC_UPDATE_IPv6			(1<<14) /* no ned save to flash */
#define		SYNC_UPDATE_DUPLEXMODE		(1<<15)
#define		SYNC_ADD_IP					(1<<16) /* no ned save to flash */

#define MAX_ACCESSCONTROLBLACKLIST_NUM 	(64)

extern int save_flag;

int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);
char *ll_addr_n2a(unsigned char *addr, int alen, char *buf, int blen);

int get_lan_port_by_mac(char *dev_mac);
void get_current_time(char *time, int len);
int mac_add_dot(char *old_mac, char *new_mac);
int mac_rm_dot(char *old_mac, char *new_mac);
int get_lan_host_cnt_by_mac(char *mac, unsigned long long *rxbytes, unsigned long long *txbytes);
int set_ipv6_from_dhcp_arp(lan_host_entry_t *p_lan_host_entry, int entry_index);
int set_ip_from_dhcp(lan_host_entry_t *p_lan_host_entry, int entry_index);
int set_ip_from_arp(lan_host_entry_t *p_lan_host_entry, int entry_index, int flag);
int set_lan_host_entry(lan_host_entry_t *p_lan_host_entry, int entry_index);
int check_mac_exist(lan_host_info_t *p_lan_host_info, char *mac, int *empty_index);

lan_host_entry_t * find_empty_lan_host_entry(int *found);
lan_host_entry_t * find_lan_host_entry_by_mac(char *mac, int *lan_host_entry);


int sync_lan_host_info(int entry_index, lan_host_entry_t *p_lan_host_entry, unsigned int update_flag);
int set_lan_host_online(lan_host_entry_t *p_lan_host_entry, int entry_index, int ip_flag);
int add_lan_host(lan_host_entry_t *p_lan_host_entry, char *mac, int entry_index, int conn_type, int port);
int del_lan_host(lan_host_entry_t *p_lan_host_entry, int entry_index);
int set_lan_host_offline(lan_host_entry_t *p_lan_host_entry, int entry_index, int initflag);
unsigned int get_lan_port_negorate(int port);
char *get_lan_port_duplexmode(int port);
int get_mapping_idx(int old_idx);

int get_lan_port_bind(int port);
#if defined(TCSUPPORT_CT_JOYME4)
int detect_slaac_address(lan_host_entry_t *p_lan_host_entry, int index, char *lladdr, int only_calc, char *out_global, int out_len);
int set_hostname_from_dhcp(lan_host_entry_t *p_lan_host_entry, int entry_index);
int parse_OB_WanIndex(char *str, int *wan_index, const char* delim);
lan_host_entry_t * find_lan_host_OB_entry_by_mac(int is_OB_Dev, char *mac, int *lan_host_entry);
int check_lan_same_subnet(char *targetIP);
int check_dhcpd_lease_exist(char *mac);
int check_arp_exist(char *mac);
int lan_host_ipv6only_update(char *mac, char *ipv6addr);
int init_ipv6_detect_queue(void);
int destroy_ipv6_detect_queue(void);
int check_ipv6_mac_exist(char *mac_buf);
int get_mac_addr_via_neigh(char *v6_addr, char *out_mac, int buf_len);
int popout_ipv6_queue(void);
#endif

#endif /* __LAN_HOST_API_H__  */
