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

#ifndef __LAN_HOST_MGR_H__
#define __LAN_HOST_MGR_H__
#include <common/ecnt_global_macro.h>

#define	NODE_NAME_LEN			32
#define CFG_NODE_BUF_LEN		128

#define	OFFLINE_TIME			86400

#define MAX_LAN_HOST_NUM	MAX_LANHOST2_ENTRY_NUM
#define LAN_PORT_NUM		4

#define DEV_MAC_ADDR_LEN		12
#define	MAC_ADDR_DOT_LEN	17
#define	HOSTNAME_LEN		63
#define DEVNAME_LEN			63
#define DEVTYPE_LEN			16
#define IP_ADDR_LEN			16
#define IPv6_ADDR_LEN		64
#define	BRAND_LEN			63
#define MODEL_LEN			63
#define OS_LEN				63
#define	TIME_LEN			32
#define	DUPLEX_LEN			4
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
#define LANHOST_AJUST_RETE  1
#endif
#define WIFI_ONLINING       2   /* wifi add but no get ip, then set middle stage */

#define ETH_PORT_STS    "/proc/tc3162/eth_port_status"

typedef struct lan_host_entry_s
{
	unsigned char valid;
#define 	IP_FLAG				(1<<0)
#define		HOST_NAME_FLAG		(1<<1)
#define 	IPV6_FLAG			(1<<2)
#define		IPV6_IDLE_FLAG		(1<<8)
#define		IPV6_IDLE_MAX_CNT	(2)
/*
** bit 8 is used to mark IPV6_IDLE_FLAG.
** bit 9 & 10 are user to set idle cnt.
*/
	unsigned int  update_flag;
	unsigned char exist;
#define		STATIC_MODE		1
#define		DHCP_MODE		2

	/*  come from dbus prop */
	char mac[DEV_MAC_ADDR_LEN + 1];
	char host_name[HOSTNAME_LEN + 1];
	char dev_name[DEVNAME_LEN + 1];
	unsigned char control_status; 		/* true; false */
	unsigned char internet_access; 		/* 0; 1;  2 */
	unsigned char storage_access;		/* true; false */
	unsigned int max_usbandwidth;
	unsigned int max_dsbandwidth;
	char dev_type[DEVTYPE_LEN + 1];
	char ip[IP_ADDR_LEN + 1];
	char ipv6[IPv6_ADDR_LEN + 1];
	unsigned char conn_type;
	unsigned char port;
	unsigned char port_bind;
	char brand[BRAND_LEN + 1];
	char model[MODEL_LEN + 1];
	char os[OS_LEN + 1];
	unsigned char active;
#define	ONLINE		1
#define	OFFLINE		0
	long online_time;
	char active_time[TIME_LEN];
	unsigned int offline_time;
	char inactive_time[TIME_LEN];
	unsigned int online_duration;
	unsigned long long rxbytes;
	unsigned long long txbytes;
	int power_level;
	unsigned int negorate;
    char duplex_mode[DUPLEX_LEN+1];
	unsigned char dev_online_notify;
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
	unsigned long upspeed;
	unsigned long downspeed;
#endif
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
#if !defined(TCSUPPORT_CT_UBUS)
	char ssid[32];
#endif
	int sleeptime;
	unsigned long long Lastrxbytes;
	unsigned long long Lasttxbytes;
#if !defined(TCSUPPORT_CT_UBUS)
	int wlanonline;
	int AvgRssi0;
	int AvgRssi1;
#endif
#endif
#if defined(TCSUPPORT_CFG_NG_UNION)
	char expireDay[10];
	char expireTime[10];
	int AddressSrc;              /*0:static; 1:DHCP*/
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#define DETECT_DONE		6
#define LOOP_END		5
#define MAX_DETECT_CNT  30   /* 10s per times, 30 --> total 300s (5 minutes) */
	unsigned char v6_detect; /* detect SLAAC address.  1-5 loop, 10 seconds per time, 6 done. */
	unsigned short detect_cnt;
	unsigned char other_br_dev; /* 1 - other br, lan device, 0 - not */
	long last_active_sec; /* last active monitor sec for other br lan device */
	unsigned char ip_type; /* 1 - ppp, 0 - dhcp */
	unsigned char ppp_lost_cnt; /* for update ppp or dhcp IP, update dhcp when ppp had lost up to 3 times */
#define LOCK_LOST_CNT	2
	unsigned char lock_lost; /* limit dev re-online in next 2 loopcheck when lan port down and wifi sta down at a moment */
#define IPV4_IPV6_MODE 0
#define IPV6_ONLY_MODE 1
#define IPV4_ONLY_MODE 2
	unsigned char ipmode; /* 0 - ipv4/ipv6, 1 - ipv6 only, 2 - ipv4 only  */
#endif

}lan_host_entry_t;

typedef struct lan_host_info_s
{
	unsigned char enable_stats;
	unsigned int lan_host_max_num;
	unsigned int lan_host_num;
	unsigned int ctrl_list_max_num;
	unsigned int ctrl_list_num;
	lan_host_entry_t lan_host_entry[MAX_LAN_HOST_NUM];
}lan_host_info_t;

#if defined(TCSUPPORT_CT_JOYME4)
typedef struct ipv6_qnode_s
{
	char *ipv6_addr;
	int ip_type;
#define IPV6_M_FLAG_OFF (1)
#define IPV6_M_FLAG_ON  (2)
	int detect_cnt;
#define IPV6_MAX_DETECT_CNT (5)
}ipv6_qnode_t;

/* set update_flag bit 9 and 10 for ipv6 idle cnt, value range(0 ~ 3) */
#define SET_IPV6_IDLE_CNT(x, y) do{ \
                                    (x) &= (~(0x3 << 9)); \
                                    (x) |= (((y) & 0x3) << 9); \
                                }while(0)

#define GET_IPV6_IDLE_CNT(x)    ( ((x) >> 9) & 0x3 )
#define CLR_IPV6_IDLE_CNT(x)    ( (x) &= (~(0x3 << 9)) )
#endif

#define G_LOCK(x) pthread_mutex_lock(x)
#define G_UNLOCK(x) pthread_mutex_unlock(x)

#define FREE_X(x) \
	if ( x ) \
	{ \
		free(x); \
		x = NULL; \
	}

#define FREE_Y(y) \
	free(y); \
	y = NULL;


int init_lan_port_status(void);
lan_host_info_t *get_lan_host_info(void);
int init_lan_host_entry(lan_host_entry_t *p_lan_host_entry, int entry_index);
int init_lan_host_info(lan_host_info_t *p_lan_host_info);
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
int check_lan_host_update(lan_host_info_t *p_lan_host_info , struct timeval curTime, struct timeval lastTime);
#else
int check_lan_host_update(lan_host_info_t *p_lan_host_info);
#if defined(TCSUPPORT_CT_JOYME4)
int get_default_wan_index(int *wan_index);
int update_wan_bytes();
int check_OB_lan_host_update(lan_host_info_t *p_lan_host_info);
#endif
#endif

#endif
