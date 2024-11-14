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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <linux/netlink.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <linux/netlink.h>
#include <sys/epoll.h>
#include <time.h>
#include "lan_host_api.h"
#include <ecnt_event_global/ecnt_event_lanhost.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include "ecnt_utility.h"
#if defined(TCSUPPORT_ANDLINK)
#include "ecnt_event_global/ecnt_event_alink.h"
#endif
#include "blapi_traffic.h"
#include <notify/notify.h>
#if defined(TCSUPPORT_CT_JOYME4)
#include <common/ecnt_global_macro.h>
#include "queue_api.h"
#endif

int save_flag = 0;
extern unsigned int lan_port_rate[LAN_PORT_NUM];
extern char lan_port_duplex[LAN_PORT_NUM][DUPLEX_LEN+1];

#if defined(TCSUPPORT_CFG_NG_UNION)
struct timespec gLastGetTime;
int gTheFirstGet = 0;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
extern pthread_mutex_t gIPv6QueueMutex;
extern Queue *g_ipv6_queue;
extern int check_flag[MAX_LAN_HOST_NUM];
#endif

static const char *ALPHA_BASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *basic64_encode(const char *buf, const long size, char *base64Char) 
{
	int a = 0;
	int i = 0;
	int int63 = 0x3F; 	/*  00111111 */
	int int255 = 0xFF; /* 11111111 */
	char b0 = 0;
	char b1 = 0;
	char b2 = 0;
	
	while (i < size) 
	{
		b0 = buf[i++];
		b1 = (i < size) ? buf[i++] : 0;
		b2 = (i < size) ? buf[i++] : 0;
		base64Char[a++] = ALPHA_BASE[(b0 >> 2) & int63];
		base64Char[a++] = ALPHA_BASE[((b0 << 4) | ((b1 & int255) >> 4)) & int63];
		base64Char[a++] = ALPHA_BASE[((b1 << 2) | ((b2 & int255) >> 6)) & int63];
		base64Char[a++] = ALPHA_BASE[b2 & int63];
	}
	
	switch (size % 3)
	{
		case 1:
		    base64Char[--a] = '=';
		case 2:
		    base64Char[--a] = '=';
	}
	
	return base64Char;
}
 
char *basic64_decode(const char *base64Char, const long base64CharSize, char *originChar, long originCharSize)
{
	int toInt[128] = {-1};
	int int255 = 0xFF;
	int index = 0;
	int i = 0;
	int c0 = 0;
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;

	for (i = 0; i < 64; i++) 
	{
		toInt[ALPHA_BASE[i]] = i;
	}
	
	for (i = 0; i < base64CharSize; i += 4)
	{
		c0 = toInt[base64Char[i]];
		c1 = toInt[base64Char[i + 1]];
		originChar[index++] = (((c0 << 2) | (c1 >> 4)) & int255);
		if (index >= originCharSize) 
		{
		    return originChar;
		}
		c2 = toInt[base64Char[i + 2]];
		originChar[index++] = (((c1 << 4) | (c2 >> 2)) & int255);
		
		if (index >= originCharSize) 
		{
		    return originChar;
		}
		
		c3 = toInt[base64Char[i + 3]];
		originChar[index++] = (((c2 << 6) | c3) & int255);
	}
	
	return originChar;
}

#define DOWN_STREAM 0
#define UP_STREAM 1


int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max)
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len)
		fprintf(stderr, "!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	return 0;
}

char *ll_addr_n2a(unsigned char *addr, int alen, char *buf, int blen)
{
	int i;
	int l;

	if (alen == 4) {
		return inet_ntop(AF_INET, addr, buf, blen);
	}
	if (alen == 16) {
		return inet_ntop(AF_INET6, addr, buf, blen);
	}
	l = 0;
	for (i=0; i<alen; i++) {
		if (i==0) {
			snprintf(buf+l, blen, "%02x", addr[i]);
			blen -= 2;
			l += 2;
		} else {
			snprintf(buf+l, blen, ":%02x", addr[i]);
			blen -= 3;
			l += 3;
		}
	}
	return buf;
}

int get_lan_port_by_mac(char *dev_mac)
{
	FILE *fp = NULL;
	int found = 0, port = 0;
	char buf[128] = {0}, mac[32] = {0}, new_mac[32] = {0};

	if (dev_mac == NULL)
		return -1;
	
	fp = fopen("/proc/br_fdb_host/stb_list", "r");
	if (NULL == fp) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	mac_add_dot(dev_mac, new_mac);
	bzero(buf, sizeof(buf));
	while (fgets(buf, sizeof(buf), fp))
	{
		sscanf(buf, "%d=%s", &port, mac);
		if (!strcasecmp(mac, new_mac)) {
			found = 1;
			break;
		}
	}
	fclose(fp);

	if (found == 1)
		return port;

	return -1;
}

void get_current_time(char *time_buf, int len)
{
	time_t cur_ltime;
	struct tm nowTime;
	
	time(&cur_ltime);
	localtime_r(&cur_ltime, &nowTime);

	snprintf(time_buf, len, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday,
			nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);

	return;
}

int mac_add_dot(char *old_mac, char *new_mac)
{
	int i = 0, j = 0;

	if (old_mac == NULL || new_mac == NULL)
		return -1;

	for (i = 0; i < DEV_MAC_ADDR_LEN; i += 2) {
		new_mac[j] = old_mac[i];
		new_mac[j + 1] = old_mac[i + 1];
		if (j + 2 < MAC_ADDR_DOT_LEN)
			new_mac[j + 2] = ':';
		j += 3;
	}

	return 0;
}

int mac_rm_dot(char *old_mac, char *new_mac)
{
	int i = 0, j = 0, mac_len = 17;

	if (old_mac == NULL || new_mac == NULL)
		return -1;
	
	for (i = 0; i < mac_len; i++) {
		if (old_mac[i] != ':')
			new_mac[j++] = old_mac[i];
	}

	return 0;
}

int set_ipv6_from_dhcp_arp(lan_host_entry_t *p_lan_host_entry, int entry_index)
{	
	char cmd[128] = {0};
	char new_mac[128] = {0};
	char ipv6[64]={0},ipv6_lladdr[15]={0},ipv6_mac[20]={0},ipv6_stale[15]={0};
	int ret = 0;
	FILE *ipv6_arp_fp = NULL;
	int found_ipv6 = 0;
	unsigned int update_flag = 0;
	char lanhost2_node[64] = {0};
	char attr_ipv6[64] = {0};
	int i = 0;
	unsigned int size = 0;
	char *buffer = NULL;

	snprintf(lanhost2_node, sizeof(lanhost2_node), "LANHost2_Entry%d", entry_index);

#if defined(TCSUPPORT_CT_JOYME4)
	/* set ipv6 addr for OTHER BRIDGE devices */
	if ( p_lan_host_entry->other_br_dev )
	{
		memset(ipv6, 0, sizeof(ipv6));
		tcapi_get(lanhost2_node, "l_IPv6", ipv6);
		if ( 0 == strncmp(p_lan_host_entry->ipv6, "fe80:", 5) )
		{
			if ( 0 != strcmp(p_lan_host_entry->ipv6, ipv6) )
				tcapi_set(lanhost2_node, "l_IPv6", ipv6);
		}
		else
		{
			tcapi_set(lanhost2_node, "g_IPv6_1", p_lan_host_entry->ipv6);
			tcapi_set(lanhost2_node, "g_IPv6_2", "");
			tcapi_set(lanhost2_node, "g_IPv6_3", "");
		}
		return 0;
	}
#endif

	tcapi_set(lanhost2_node, "g_IPv6_1", "");
	tcapi_set(lanhost2_node, "g_IPv6_2", "");
	tcapi_set(lanhost2_node, "g_IPv6_3", "");

	if (mac_add_dot(p_lan_host_entry->mac, new_mac) == -1) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	snprintf(cmd, sizeof(cmd),"/usr/bin/ip -6 neigh show dev br0 > /tmp/ipv6_arp");
	system(cmd);

	ipv6_arp_fp = fopen("/tmp/ipv6_arp", "r");
	
	if (ipv6_arp_fp == NULL)
	{
		lanhost_printf(DBG_MSG_INFO, "%s:%d========>\n", __FUNCTION__,__LINE__);
		return -1;
	}
	while(((ret = getline(&buffer, &size, ipv6_arp_fp)) != -1)
			&& (i < 3))
	{
		ret = sscanf(buffer, "%s %s %s %s", ipv6, ipv6_lladdr, ipv6_mac, ipv6_stale);
		
		if ((4 == ret))
		{
			if ((strcasecmp(ipv6_mac, new_mac) == 0)
				&& ((strcasecmp(ipv6_stale, "REACHABLE") == 0)
				/* for wifi phone device, will use IP4 first, IPv6 addr will changed to be STALE. */
				|| (0 != p_lan_host_entry->ip[0] && 0 == strcasecmp(ipv6_stale, "STALE") )
				)) 
			{
				if (0 == strncmp(ipv6, "fe80:", 5))
				{
					tcapi_set(lanhost2_node, "l_IPv6", ipv6);
				}
				else
				{
					i++;
					snprintf(attr_ipv6, sizeof(attr_ipv6), "g_IPv6_%d", i);
					tcapi_set(lanhost2_node, attr_ipv6, ipv6);					
				}	
			}		
		}
	}
	
	if (buffer)
	{
		free(buffer);
	}
	
	if (ipv6_arp_fp)
	{
		fclose(ipv6_arp_fp);
	}
	
	unlink("/tmp/ipv6_arp");

	return 0;
}

#if defined(TCSUPPORT_CFG_NG_UNION)
void check_expire_time(lan_host_entry_t *p_lan_host_entry, int lease_time)
{
	struct timespec cur;
	int timeLeft = 0, days = 0, hours = 0, minutes = 0, seconds = 0;
	time_t curTime;

	/*Add a time check*/
	if(gTheFirstGet == 0) 
	{
		clock_gettime(CLOCK_MONOTONIC,&gLastGetTime);
		gTheFirstGet = 1;
	}
	else 
	{
		clock_gettime(CLOCK_MONOTONIC,&cur);
		if(cur.tv_sec - gLastGetTime.tv_sec < 3) 
		{
			return 0;
		}
		else 
		{
			gLastGetTime.tv_sec = cur.tv_sec;
			gLastGetTime.tv_nsec = cur.tv_nsec;
		}
	}
	
	curTime = gLastGetTime.tv_sec;
	
	timeLeft = atoi(lease_time) - curTime;
	
	days = timeLeft / (60*60*24);
	timeLeft %= 60*60*24;
	hours = timeLeft / (60*60);
	timeLeft %= 60*60;
	minutes = timeLeft / 60;
	seconds = timeLeft % 60;
	snprintf(p_lan_host_entry->expireDay, sizeof(p_lan_host_entry->expireDay), "%d", days);
	snprintf(p_lan_host_entry->expireTime, sizeof(p_lan_host_entry->expireTime), "%d:%d:%d", hours, minutes, seconds);

	return;

}
#endif

#if defined(TCSUPPORT_CT_JOYME4)
/*
SLAAC MODE
when device ll address fe80::xxxyyy
it will create global address pd::xxxyyy
*/
int detect_slaac_address(lan_host_entry_t *p_lan_host_entry, int index, char *lladdr
, int only_calc, char *out_global, int out_len)
{
	char LANPORT[16] = {0};
	int wlan_cnt = 8, i = 0, j = 0, bind_flag = 0, wan_idx = 0;
	char wan_node[32] = {0}, wan_if_val[20] = {0}, lan_if_val[12] = {0};
	char ipver[16] = {0}, pdenable[16] = {0}, wan_pd[64] = {0};
	char cmdbuf[256] = {0}, def_route_index_v6[16] = {0};
	char *pd_p = NULL, *ll_p = NULL;

	if ( 0 == p_lan_host_entry->conn_type )
		snprintf(LANPORT, sizeof(LANPORT), "LAN%d", p_lan_host_entry->port);
	else
	{
		if ( p_lan_host_entry->port <= wlan_cnt )
			snprintf(LANPORT, sizeof(LANPORT), "SSID%d", p_lan_host_entry->port);
		else
			snprintf(LANPORT, sizeof(LANPORT), "SSIDAC%d", p_lan_host_entry->port - wlan_cnt);
	}

	/* check VLAN BIND:  not support for VLAN ping6.  RESERVED. */


	/* check PORT BIND. */
	bzero(wan_if_val, sizeof(wan_if_val));
	for ( i = 0; i < PVC_NUM; i++ )
	{
		for ( j = 0; j < MAX_SMUX_NUM; j++ ) {
			sprintf(wan_node,  "Wan_PVC%d_Entry%d", i, j);
			if ( tcapi_get(wan_node, LANPORT, lan_if_val) < 0 )
				continue;

			if ( !strcmp(lan_if_val, "Yes") )
			{
				tcapi_get(wan_node, "IPVERSION", ipver);
				if ( strstr(ipver, "IPv6") )
				{
					tcapi_get(wan_node, "PDEnable", pdenable);
					bind_flag = 1;
					wan_idx = i * MAX_SMUX_NUM + j;
					break;
				}
				else
					continue;
			}
		}
	}

	if ( !bind_flag )
	{
		/* USE default V6 wan. */
		if ( 0 == tcapi_get("WanInfo_Common", "DefRouteIndexv6", def_route_index_v6)
			&& 0 != strcmp(def_route_index_v6, "N/A") )
		{
			wan_idx = atoi(def_route_index_v6);
			sprintf(wan_node,  "Wan_PVC%d_Entry%d"
						, wan_idx / MAX_SMUX_NUM , wan_idx % MAX_SMUX_NUM);
			tcapi_get(wan_node, "IPVERSION", ipver);
			tcapi_get(wan_node, "PDEnable", pdenable);

		}
	}

	if ( !strcmp(pdenable, "Yes")
		&& strstr(ipver, "IPv6") )
	{
		sprintf(wan_node,  "WanInfo_Entry%d", wan_idx);
		if ( 0 == tcapi_get(wan_node, "PD6", wan_pd)
			&& ( pd_p = strstr(wan_pd, "::/"))
			&& ( ll_p = strstr(lladdr, "::")) )
		{
			*pd_p = 0; /* remove ' ::/xx' */

			if ( only_calc && out_global )
			{
				snprintf(out_global, out_len, "%s:%s", wan_pd, ll_p + 2);
				return 0;
			}

			/* try to detect once.	*/
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/bin/ping6 -q -c 1 -W 1 -I br0 %s:%s"
							, wan_pd
							, ll_p + 2); /* jump :: */
			system(cmdbuf);

			p_lan_host_entry->detect_cnt ++;
		}
	}

	return 0;
}

int set_hostname_from_dhcp(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	FILE *fp = NULL;
	char buf[128] = {0}, new_mac[128] = {0};
	char mac[32], ip[32], lease_time[32], host_name[128], tmphostName[128] = {0}, strbuf[128] = {0};
	char nodeName[64] = {0};
	int found = 0, inlen = 0, outlen = 0;;
	unsigned int update_flag = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if ( mac_add_dot(p_lan_host_entry->mac, new_mac) == -1 )
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	fp = fopen("/etc/udhcp_lease", "r");
	if ( NULL == fp )
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> open udhcp_lease failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	while ( fgets(buf, 100, fp) != NULL )
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> buf=[%s]!\n", __FUNCTION__, __LINE__, buf);

		sscanf(buf, "%s %s %s %s", mac, ip, lease_time, host_name);
		if (strcasecmp(mac, new_mac) == 0)
		{
			found = 1;
			break;
		}		
	}
	fclose(fp);

	if ( found )
	{
		if ( ( p_lan_host_entry->update_flag & HOST_NAME_FLAG ) == 0 )
		{
			memset(nodeName, 0, sizeof(nodeName));
			memset(tmphostName, 0, sizeof(tmphostName));
			memset(strbuf, 0, sizeof(strbuf));
			snprintf(nodeName, sizeof(nodeName), "LANHost2_Entry%d", entry_index);
			tcapi_get(nodeName, "HostName", tmphostName);
			if(tmphostName[0] != '\0')
			{
				inlen = strlen(tmphostName);
				outlen = sizeof(strbuf);
				if ( 0 == charsetconv("utf-8", "gb2312", &inlen, &outlen, tmphostName, strbuf) )
					strncpy(p_lan_host_entry->host_name, strbuf, sizeof(p_lan_host_entry->host_name) - 1);
			}
			else
				strncpy(p_lan_host_entry->host_name, host_name, sizeof(p_lan_host_entry->host_name) - 1);

			p_lan_host_entry->update_flag |= HOST_NAME_FLAG;
			update_flag = SYNC_UPDATE_HOSTNAME;
			sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);
		}
	}
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> found=[%d]exit!\n", __FUNCTION__, __LINE__, found);

	return found;
}

int parse_OB_WanIndex(char *str, int *wan_index, const char* delim)
{
	int cnt = 0;
	char *ptr = NULL;

	if ( NULL == str )
		return 0;

	ptr = strtok(str, delim);
	if ( NULL == ptr )
		return 0;

	while ( ptr )
	{
		wan_index[cnt] = atoi(ptr);
		cnt++;
		ptr = strtok(NULL, delim);
	}

	return cnt;
}

lan_host_entry_t * find_lan_host_OB_entry_by_mac(int is_OB_Dev, char *mac, int *lan_host_entry)
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	int i = 0;

	p_lan_host_info = get_lan_host_info();
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) 
	{
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if ( is_OB_Dev == p_lan_host_entry->other_br_dev
			&& !strcasecmp(p_lan_host_entry->mac, mac) ) 
		{
			*lan_host_entry = i;
			return p_lan_host_entry;
		}
	}

	return NULL;
}

int check_lan_same_subnet(char *targetIP)
{
	char lanip[16] = {0}, lanmask[16] = {0};
	struct in_addr in_s_lanip;
	struct in_addr in_s_lanmask;
	struct in_addr in_s_targetip;

	if ( NULL == targetIP )
		return 0;

	bzero(&in_s_targetip, sizeof(in_s_targetip));
	if ( 1 != inet_pton(AF_INET, targetIP, &in_s_targetip) )
		return 0;

	tcapi_get("Lan_Entry0", "IP", lanip);
	if ( 0 == lanip[0] )
		return 0;
	bzero(&in_s_lanip, sizeof(in_s_lanip));
	if ( 1 != inet_pton(AF_INET, lanip, &in_s_lanip) )
		return 0;

	tcapi_get("Lan_Entry0", "netmask", lanmask);
	if ( 0 == lanmask[0] )
		return 0;
	bzero(&in_s_lanmask, sizeof(in_s_lanmask));
	if ( 1 != inet_pton(AF_INET, lanmask, &in_s_lanmask) )
		return 0;
	
	if ( (in_s_lanip.s_addr & in_s_lanmask.s_addr) == (in_s_targetip.s_addr & in_s_lanmask.s_addr) )
		return 1;

	return 0;
}

int check_dhcpd_lease_exist(char *mac)
{
	FILE *fp = NULL;
	char buf[256] = {0};
	char ip[32], hw_addr[32], lease_time[32], host_name[128];
	int line = 0, found = 0;

	if ( NULL == mac )
		return 0;

	fp = fopen("/etc/udhcp_lease", "r");
	if ( !fp )
		return 0;

	while (fgets(buf, 100, fp) != NULL) 
	{
		sscanf(buf, "%s %s %s %s", hw_addr, ip, lease_time, host_name);
		if (strcasecmp(mac, hw_addr) == 0) {
			found = 1;
			break;
		}		
	}
	fclose(fp);

	return found;
}

int check_arp_exist(char *mac)
{
	FILE *fp = NULL;
	char buf[256] = {0};
	char ip[32], hw_type[32], Flags[32], hw_addr[32], mask[32], dev[32];
	int line = 0, found = 0;

	if ( NULL == mac )
		return 0;

	fp = fopen("/proc/net/arp", "r");
	if ( !fp )
		return 0;

	while (fgets(buf, sizeof(buf), fp) != NULL) 
	{
		/* skip first line */
		if (line == 0) {
			line++;
			continue;
		}
		
		sscanf(buf, "%s %s %s %s %s %s", ip, hw_type, Flags, hw_addr, mask, dev);
		
		/* ignore dead arp */
		if ( 0 != strcmp(Flags, "0x2") )
		{
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> error, ignore dead arp=[%s]!\n", __FUNCTION__, __LINE__, buf);
			continue;
		}

		/* ignore arp not in the same subnet with lan */
		if ( 1 != check_lan_same_subnet(ip) )
		{
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> ignore not in the same subnet, Invalid arp=[%s]!\n", __FUNCTION__, __LINE__, ip);
			continue;
		}

		if (strlen(ip) <= 0) {
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> error, buf=[%s]!\n", __FUNCTION__, __LINE__, buf);
			continue;
		}
		if (strcasecmp(hw_addr, mac) == 0) {
			found = 1;
			break;
		}
		line++;
	}
	fclose(fp);

	return found;
}

int lan_host_ipv6only_update(char *mac, char *ipv6addr)
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char cmd[16] = {0}, new_mac[32] = {0};
	int lan_host_entry = -1, lan_host_port = -1;
	int conn_type = 0;
	unsigned int update_flag = 0;
	
	mac_rm_dot(mac, new_mac);
	lan_host_port = get_lan_port_by_mac(new_mac);

	if( lan_host_port < 0 )
	{
		lanhost_printf(DBG_MSG_ERR, "\n%s>> lan host port error ==>!\n", __FUNCTION__);
		return -1;
	}

	/* get lan port and conn_type */
	if( lan_host_port >= 1 && lan_host_port <= 4 )
		conn_type = 0;
	else
		conn_type = 1;
	
	p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &lan_host_entry);
	if (p_lan_host_entry) 
	{
		if ( strcmp(p_lan_host_entry->ipv6, ipv6addr) ) 
		{
			update_flag = SYNC_UPDATE_IPv6;
			p_lan_host_entry->update_flag |= IPV6_FLAG;
			if ( 0 == ( p_lan_host_entry->update_flag & IP_FLAG ) )
				p_lan_host_entry->ipmode = IPV6_ONLY_MODE; /* ipv6 only */
			else
				p_lan_host_entry->ipmode = IPV4_IPV6_MODE;

			if ( 0 == (p_lan_host_entry->update_flag & IPV6_IDLE_FLAG) )
			{
				p_lan_host_entry->update_flag |= IPV6_IDLE_FLAG;
				SET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag, IPV6_IDLE_MAX_CNT);
				check_flag[lan_host_entry] = 5;
			}
			sync_lan_host_info(lan_host_entry, p_lan_host_entry, update_flag);				
		}
		if( OFFLINE == p_lan_host_entry->active )
		{
			p_lan_host_entry->update_flag |= IPV6_FLAG;
			if ( 0 == ( p_lan_host_entry->update_flag & IP_FLAG ) )
				p_lan_host_entry->ipmode = IPV6_ONLY_MODE; /* ipv6 only */
			else
				p_lan_host_entry->ipmode = IPV4_IPV6_MODE;

			if ( 0 == (p_lan_host_entry->update_flag & IPV6_IDLE_FLAG) )
			{
				p_lan_host_entry->update_flag |= IPV6_IDLE_FLAG;
				SET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag, IPV6_IDLE_MAX_CNT);
				check_flag[lan_host_entry] = 5;
			}
			set_lan_host_online(p_lan_host_entry, lan_host_entry, 0);
		}
	}
	else 
	{
		p_lan_host_entry = find_empty_lan_host_entry(&lan_host_entry);
		if ( p_lan_host_entry )
		{
			p_lan_host_entry->update_flag |= IPV6_FLAG;
			if ( 0 == ( p_lan_host_entry->update_flag & IP_FLAG ) )
				p_lan_host_entry->ipmode = IPV6_ONLY_MODE; /* ipv6 only */
			else
				p_lan_host_entry->ipmode = IPV4_IPV6_MODE;

			if ( 0 == (p_lan_host_entry->update_flag & IPV6_IDLE_FLAG) )
			{
				p_lan_host_entry->update_flag |= IPV6_IDLE_FLAG;
				SET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag, IPV6_IDLE_MAX_CNT);
				check_flag[lan_host_entry] = 5;
			}
			add_lan_host(p_lan_host_entry, new_mac, lan_host_entry, conn_type, lan_host_port);
		}
	}
	
	if (p_lan_host_entry == NULL)
	{
		lanhost_printf(DBG_MSG_ERR, "\n%s>> exit for no empty entry ==>!\n", __FUNCTION__);
		return -1;
	}		
	
	return 0;
}

int check_ipv6_mac_exist(char *mac_buf)
{
	lan_host_entry_t *p_lan_host_entry = NULL;
	lan_host_info_t *p_lan_host_info = NULL;
	int idx = 0;
	char new_mac[128] = {0};

	if ( NULL == mac_buf )
		return -1;

	p_lan_host_info = get_lan_host_info();
	for ( idx = 0; idx < MAX_LAN_HOST_NUM; idx ++ )
	{
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[idx];
		if ( -1 == mac_add_dot(p_lan_host_entry->mac, new_mac) )
			continue;

		if ( 0 == strcmp(new_mac, mac_buf) )
			return 1;
	}

	return 0;
}

int get_mac_addr_via_neigh(char *v6_addr, char *out_mac, int buf_len)
{
#define TMP_NEIGH_V6_FILE "/tmp/chk_1_ipv6_neigh"
	char ipv6[64]={0}, s_lladdr[15]={0}, ipv6_mac[20]={0}, ipv6_stale[15]={0};
	unsigned int size = 0;
	FILE *fp = NULL;
	int ret = 0;
	char *buffer = NULL;

	if ( NULL == v6_addr || NULL == out_mac )
		return -1;

	/* get MAC from neighbor list. */
	system("/usr/bin/ip -6 neigh show dev br0 > "TMP_NEIGH_V6_FILE);
	
	fp = fopen(TMP_NEIGH_V6_FILE, "r");
	
	if ( !fp )
		return -2;
	
	while( (ret = getline(&buffer, &size, fp)) != -1 )
	{
		ret = sscanf(buffer, "%s %s %s %s", ipv6, s_lladdr, ipv6_mac, ipv6_stale);
		if ( 4 == ret )
		{
			if ( 0 == strcasecmp(ipv6, v6_addr) )
			{
				snprintf(out_mac, buf_len, "%s", ipv6_mac);
				break;
			}
		}
	}
	
	if ( buffer )
		free(buffer);
	if ( fp )
		fclose(fp);
	unlink(fp);

	return 0;
}

int popout_ipv6_queue(void)
{
	int i = 0, ret = 0, queue_length = 0;
	ipv6_qnode_t *q_node = NULL;
	lan_host_entry_t entry_tmp;
	char host_v6_addr[IPv6_ADDR_LEN + 1] = {0}, s_lladdr[IPv6_ADDR_LEN + 1] = {0};
	char host_mac[24] = {0};

	/* 
	** we must get queue length before current detect action,
	** because we may repush q_node to tail which will update queue length,
	** anyway, we should not pop out the repushed q_node at this turn.
	*/
	G_LOCK(&gIPv6QueueMutex);
	queue_length = g_ipv6_queue->length;
	G_UNLOCK(&gIPv6QueueMutex);

	/* detect all ipv6 addr */
	for ( i = 0; i < queue_length; i++ )
	{
		G_LOCK(&gIPv6QueueMutex);
		q_node = (ipv6_qnode_t *)QueuePopHead(g_ipv6_queue);
		G_UNLOCK(&gIPv6QueueMutex);
		/* stop popout queue when queue is empty */
		if ( NULL == q_node )
			break;

		q_node->detect_cnt++;
		/* get mac from ipv6 neighbor. */
		bzero(host_mac, sizeof(host_mac));
		get_mac_addr_via_neigh(q_node->ipv6_addr, host_mac, sizeof(host_mac));
		/* check to push tail for next detect when less than IPV6_MAX_DETECT_CNT times */
		if ( 0 == host_mac[0] )
		{
			if ( q_node->detect_cnt < IPV6_MAX_DETECT_CNT )
			{
				G_LOCK(&gIPv6QueueMutex);
				ret = QueuePushTail(g_ipv6_queue, q_node);
				G_UNLOCK(&gIPv6QueueMutex);
				if ( 0 != ret )
				{
					FREE_X(q_node->ipv6_addr);
					FREE_X(q_node);
				}
			}
			/* 
			** we have check this ipv6 addr for more than IPV6_MAX_DETECT_CNT times, 
			** no need to check it any more, free q_node 
			*/
			else
			{
				FREE_X(q_node->ipv6_addr);
				FREE_X(q_node);
			}
		}
		else
		{
			bzero(host_v6_addr, sizeof(host_v6_addr));
			bzero(s_lladdr, sizeof(s_lladdr));
		
			/* link local addr, maybe radvd without M flag. */
			if ( IPV6_M_FLAG_OFF == q_node->ip_type )
			{
				snprintf(s_lladdr, sizeof(s_lladdr), "%s", q_node->ipv6_addr);
		
				bzero(&entry_tmp, sizeof(entry_tmp));
				detect_slaac_address(&entry_tmp, 0, s_lladdr, 1, host_v6_addr, sizeof(host_v6_addr));
		
				if ( 0 != host_v6_addr[0] )
				{
					/* add to new entry. */
					lan_host_ipv6only_update(host_mac, host_v6_addr);
				}
			}
			/* global addr. */
			else if ( IPV6_M_FLAG_ON == q_node->ip_type )
			{
				snprintf(host_v6_addr, sizeof(host_v6_addr), "%s", q_node->ipv6_addr);
				if ( 0 != host_v6_addr[0] )
				{
					/* add to new entry. */
					lan_host_ipv6only_update(host_mac, host_v6_addr);
				}
			}
			/* free q_node after we had detect this ipv6 addr */
			FREE_X(q_node->ipv6_addr);
			FREE_X(q_node);
		}
	}

	return 0;
}

int check_ipv6_device_inform(lan_host_entry_t *p_lan_host_entry)
{
	int idle_cnt = 0;

	if ( NULL == p_lan_host_entry )
		return 0;

	/* get ipv4 addr, clear IPV6_IDLE_FLAG and return true */
	if ( p_lan_host_entry->update_flag & IP_FLAG )
	{
		p_lan_host_entry->update_flag &= ~IPV6_IDLE_FLAG;
		CLR_IPV6_IDLE_CNT(p_lan_host_entry->update_flag);
		return 1;
	}
	if ( IPV6_ONLY_MODE == p_lan_host_entry->ipmode && (p_lan_host_entry->update_flag & IPV6_FLAG) )
	{
		if ( p_lan_host_entry->update_flag & IPV6_IDLE_FLAG )
		{
			idle_cnt = GET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag);
			if ( idle_cnt > 0 )
				return 0;
			else
				return 1;
		}
		else
			return 1;
	}

	return 0;
}
#endif

int set_ip_from_dhcp(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	FILE *fp = NULL;
	char buf[128] = {0}, new_mac[128] = {0};
	char mac[32], ip[32], lease_time[32], host_name[128], tmphostName[128] = {0}, strbuf[128] = {0};
	char nodeName[64] = {0};
	int found = 0, inlen = 0, outlen = 0;;
	unsigned int update_flag = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

#if 0
	if (p_lan_host_entry->update_flag & HOST_NAME_FLAG) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> no need to update!\n", __FUNCTION__, __LINE__);
		return found;
	}
#endif

	if (mac_add_dot(p_lan_host_entry->mac, new_mac) == -1) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	
	fp = fopen("/etc/udhcp_lease", "r");
	if (NULL == fp) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	while (fgets(buf, 100, fp) != NULL) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> buf=[%s]!\n", __FUNCTION__, __LINE__, buf);
		
		sscanf(buf, "%s %s %s %s", mac, ip, lease_time, host_name);
		if (strcasecmp(mac, new_mac) == 0) {
			found = 1;
			break;
		}		
	}
	fclose(fp);

	if (found) {
		if ((p_lan_host_entry->update_flag & HOST_NAME_FLAG) == 0 || 0 != strcasecmp(p_lan_host_entry->ip, ip)
#if defined(TCSUPPORT_CT_JOYME4)
			|| 0 == (p_lan_host_entry->update_flag & IP_FLAG)
#endif
		) {
#if defined(TCSUPPORT_CHARSET_CHANGE) 
			memset(nodeName, 0, sizeof(nodeName));
			memset(tmphostName, 0, sizeof(tmphostName));
			memset(strbuf, 0, sizeof(strbuf));
			snprintf(nodeName, sizeof(nodeName), "LANHost2_Entry%d", entry_index);
			tcapi_get(nodeName, "HostName", tmphostName);
			if(tmphostName[0] != '\0')
			{
				inlen = strlen(tmphostName);
				outlen = sizeof(strbuf);
				if ( 0 == charsetconv("utf-8", "gb2312", &inlen, &outlen, tmphostName, strbuf) )
					strncpy(p_lan_host_entry->host_name, strbuf, sizeof(p_lan_host_entry->host_name) - 1);
			}
			else
#endif
			strncpy(p_lan_host_entry->host_name, host_name, sizeof(p_lan_host_entry->host_name) - 1);
			
			strncpy(p_lan_host_entry->ip, ip, sizeof(p_lan_host_entry->ip) - 1);
			p_lan_host_entry->update_flag |= (HOST_NAME_FLAG | IP_FLAG);

#if defined(TCSUPPORT_CFG_NG_UNION)
			check_expire_time(p_lan_host_entry, lease_time);
			p_lan_host_entry->AddressSrc = 1;				 /*dhcp*/
#endif

			update_flag = (SYNC_UPDATE_IP | SYNC_UPDATE_HOSTNAME);
			sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);
		}
	}
#if 0
	else if (found == 0 && (p_lan_host_entry->update_flag & HOST_NAME_FLAG)) {
		bzero(p_lan_host_entry->ip, sizeof(p_lan_host_entry->ip));
		p_lan_host_entry->update_flag &= ~(IP_FLAG);
		
		update_flag = SYNC_UPDATE_IP;
		sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);
	}
#endif
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> found=[%d]exit!\n", __FUNCTION__, __LINE__, found);
	
	return found;
}

int set_ip_from_arp(lan_host_entry_t *p_lan_host_entry, int entry_index, int flag)
{
	FILE *fp = NULL;
	char buf[256] = {0}, new_mac[MAC_ADDR_DOT_LEN + 1] = {0};
	char ip[32], hw_type[32], Flags[32], hw_addr[32], mask[32], dev[32];
	int line = 0, found = 0;
	unsigned update_flag = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if ((p_lan_host_entry->update_flag & IP_FLAG) &&strlen(p_lan_host_entry->ip)){	
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> no need to update IP!\n", __FUNCTION__, __LINE__);

		return 1;
	}
	
	if (mac_add_dot(p_lan_host_entry->mac, new_mac) == -1)
		return 0;

	fp = fopen("/proc/net/arp", "r");
	if(!fp)
		return 0;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		/* skip first line */
		if (line == 0) {
			line++;
			continue;
		}
		
		sscanf(buf, "%s %s %s %s %s %s", ip, hw_type, Flags, hw_addr, mask, dev);

#if defined(TCSUPPORT_CT_JOYME4)
		/* ignore dead arp */
		if ( 0 != strcmp(Flags, "0x2") )
		{
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> error, ignore dead arp=[%s]!\n", __FUNCTION__, __LINE__, buf);
			continue;
		}

		/* ignore arp not in the same subnet with lan */
		if ( 1 != check_lan_same_subnet(ip) )
		{
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> ignore not in the same subnet, Invalid arp=[%s]!\n", __FUNCTION__, __LINE__, ip);
			continue;
		}
#endif
		if (strlen(ip) <= 0) {
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> error, buf=[%s]!\n", __FUNCTION__, __LINE__, buf);
			continue;
		}
		
		if (strcasecmp(dev, "br0") == 0) 
		{
			if (strcasecmp(hw_addr, new_mac) == 0) 
			{
#if defined(TCSUPPORT_ANDLINK)
				if((strcasecmp(Flags, "0x2") == 0) && flag == 1)
#endif
					found = 1;
				break;
			}
		}
		line++;
	}
	fclose(fp);

	if (found) {
		strncpy(p_lan_host_entry->ip, ip, sizeof(p_lan_host_entry->ip) - 1);
		p_lan_host_entry->update_flag |= IP_FLAG;

#if defined(TCSUPPORT_CFG_NG_UNION)
		p_lan_host_entry->AddressSrc = 0;                  /*static*/
#endif

		update_flag = SYNC_UPDATE_IP;
		sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> found=[%d]exit!\n", __FUNCTION__, __LINE__, found);
	
	return found;
}

int set_lan_host_entry(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	char mac_addr[32]= {0}, host_name[32] = {0};

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
	
	if (p_lan_host_entry == NULL) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (set_ip_from_dhcp(p_lan_host_entry, entry_index) == 1) {
		//lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit from dhcp!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (set_ip_from_arp(p_lan_host_entry, entry_index, 0) == 1) {
		//lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit from arp!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);

	return -1;
}

int  get_lan_host_cnt_by_mac(char *mac, unsigned long long *rxbytes, unsigned long long *txbytes)
{
	int ret = 0;
	char tmp[8] = {0};
	
	if (mac == NULL || rxbytes == NULL || txbytes == NULL)
		return -1;

	tcapi_get("LANHost2_Common", "EnableStats", tmp);

	ret = blapi_traffic_get_dev_acnt_by_mac(atoi(tmp),mac, rxbytes, txbytes);
	
	return 0;
}

int check_mac_exist(lan_host_info_t *p_lan_host_info, char *mac, int *empty_index)
{
	lan_host_entry_t *p_lan_host_entry = NULL;
	int i, found = 0, found_empty_entry = 0;
	char nodename[32] = {0}, control_status[4] = {0}, new_mac[32] = {0};
	char s_lan_host_num[8] = {0}, s_lan_host_max_num[8] = {0};
	int lan_host_num = 0, lan_host_max_num = 0;
	int latest_inactive_index = -1;
	unsigned int offline_time = OFFLINE_TIME;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d] >> enter mac = %s!\n", __FUNCTION__, __LINE__, mac);
	
	if (p_lan_host_info == NULL || mac == NULL || empty_index == NULL)
		return -1;

	tcapi_get("LANHost2_Common", "LANHostNumber", s_lan_host_num);
	if (s_lan_host_num[0] != '\0')
	{
		lan_host_num = atoi(s_lan_host_num);
	}
	
	tcapi_get("LANHost2_Common", "LANHostMaxNumber", s_lan_host_max_num);
	if (s_lan_host_max_num[0] != '\0')
	{
		lan_host_max_num = atoi(s_lan_host_max_num);
		if ( lan_host_max_num > MAX_LAN_HOST_NUM )
			lan_host_max_num = MAX_LAN_HOST_NUM;
	}

	for (i = 0; i < lan_host_max_num; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if ((p_lan_host_entry->active == 0  &&  p_lan_host_entry->offline_time > 0)
			&& (offline_time > p_lan_host_entry->offline_time))
		{
			bzero(nodename, sizeof(nodename));
			snprintf(nodename, sizeof(nodename), "LanHost2_Entry%d", i);
			bzero(control_status, sizeof(control_status));
			tcapi_get(nodename, "ControlStatus", control_status);
			/* controlstatus must be false */
			if ( 0 != strcmp(control_status, "1") )
			{
				offline_time = p_lan_host_entry->offline_time;
				latest_inactive_index = i;
				lanhost_printf(DBG_MSG_INFO, "[%s][%d] >> p_lan_host_entry[%d]->active=%d,offline_time=%d===============>\n", __FUNCTION__, __LINE__,i,p_lan_host_entry->active,p_lan_host_entry->offline_time);
			}
		}
		if (p_lan_host_entry->valid == 1) {
			if (mac_rm_dot(mac, new_mac) == -1)
				return -1;
			
			lanhost_printf(DBG_MSG_INFO, "[%s][%d] >> enter p_lan_host_entry->mac = %s, new_mac = %s!\n", __FUNCTION__, __LINE__, p_lan_host_entry->mac, new_mac);
			if (!strcasecmp(p_lan_host_entry->mac, new_mac)) {
				/*add no ip entry of route mode one more time*/
				if(!strcasecmp(p_lan_host_entry->ip,"") && p_lan_host_entry->port_bind == 0 
#if defined(TCSUPPORT_CT_JOYME4)
					&& 0 == (p_lan_host_entry->update_flag & IPV6_FLAG)
#endif
					)
				{
					*empty_index = i;
					return 0;
				}
				found = 1;
				if (0 == p_lan_host_entry->exist)
					p_lan_host_entry->exist = 1;
				
				break;
			}
		}
		else if (found_empty_entry == 0) {
			*empty_index = i;
			found_empty_entry = 1;
		}
	}

	if (lan_host_max_num == lan_host_num)
	{
		*empty_index = latest_inactive_index;	
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]: found = %d>> exit!\n", __FUNCTION__, __LINE__, found);
	
	return found;
}

lan_host_entry_t * find_empty_lan_host_entry(int *found)
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	lan_host_entry_t *p_tmp_lan_host_entry = NULL;
	int i = 0;

	char s_lan_host_num[8] = {0}, s_lan_host_max_num[8] = {0};
	int lan_host_num = 0, lan_host_max_num = 0;
	int latest_inactive_index = -1;
	unsigned int offline_time = OFFLINE_TIME;
	char nodename[32] = {0}, control_status[4] = {0};

	tcapi_get("LANHost2_Common", "LANHostNumber", s_lan_host_num);
	if (s_lan_host_num[0] != '\0')
	{
		lan_host_num = atoi(s_lan_host_num);
	}
	tcapi_get("LANHost2_Common", "LANHostMaxNumber", s_lan_host_max_num);
	if (s_lan_host_max_num[0] != '\0')
	{
		lan_host_max_num = atoi(s_lan_host_max_num);
		if ( lan_host_max_num > MAX_LAN_HOST_NUM )
			lan_host_max_num = MAX_LAN_HOST_NUM;
	}

	p_lan_host_info = get_lan_host_info();
	for (i = 0; i < lan_host_max_num; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		
		if ((p_lan_host_entry->active == 0 && p_lan_host_entry->offline_time > 0)
			&& (offline_time > p_lan_host_entry->offline_time)) {
			
			bzero(nodename, sizeof(nodename));
			snprintf(nodename, sizeof(nodename), "LanHost2_Entry%d", i);
			bzero(control_status, sizeof(control_status));
			tcapi_get(nodename, "ControlStatus", control_status);
			/* controlstatus must be false */
			if ( 0 != strcmp(control_status, "1") )
			{
				offline_time = p_lan_host_entry->offline_time;
				p_tmp_lan_host_entry = p_lan_host_entry;
				latest_inactive_index = i;
			}
		}
		
		if (p_lan_host_entry->valid == 0) {
			*found = i;
			return p_lan_host_entry;
		}
	}
	
	if (lan_host_max_num == lan_host_num)
	{
		*found = latest_inactive_index;
		return p_tmp_lan_host_entry;	
	}

	return NULL;
}

lan_host_entry_t * find_lan_host_entry_by_mac(char *mac, int *lan_host_entry)
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	int i = 0;

	p_lan_host_info = get_lan_host_info();
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if (!strcasecmp(p_lan_host_entry->mac, mac)) {
			*lan_host_entry = i;
			return p_lan_host_entry;
		}
	}

	return NULL;
}

void laninfo_devicestatus_change_inform(int entry_idx)
{
	char node[64] = {0};
	char mac[24] = {0};
	char val[5] = {0};
	int active = 0;
	laninfo_msg_t msgdata;
	memset(node, 0, sizeof(node));
	memset(mac, 0, sizeof(mac));
	memset(&msgdata, 0, sizeof(laninfo_msg_t));

    snprintf(node, sizeof(node),"root.lanhost2.entry.%d",entry_idx+1);
    cfg_get_object_attr(node, "MAC", mac, sizeof(mac));
	cfg_get_object_attr(node, "Active", val, sizeof(val));
	active = atoi(val);
	memcpy(msgdata.mac, mac, sizeof(mac));
	msgdata.active = active;
	ctc_notify2ctcapd(NOTIFY_LANINFO_DEVICESTATUS_CHANGE, &msgdata, sizeof(laninfo_msg_t));
}
int active_old[64] = {0};
int sync_lan_host_info(int entry_index, lan_host_entry_t *p_lan_host_entry, unsigned int update_flag)
{
	char node_name[32] = {0}, data[64] = {0};
	char buf1[32] = {0}, buf2[32] = {0};
	char base64HostName[128] = {0};
	char utfValue[HOSTNAME_LEN] = {0};
	int inlen = 0;
	int outlen = HOSTNAME_LEN;
	int ret = 0;
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
	char accesslimit_mac[32] = {0}, nodeName[32] = {0};
	int i = 0;
	lan_notify_info_t Info = {0};
#endif
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
	char RxRate_rt[32] = {0};
	char TxRate_rt[32] = {0};
	char lastTxByte[32] = {0};
	char lastRxByte[32] = {0};
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	int ip_status = 0;
#endif
		
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if (update_flag & (SYNC_NEW_ENTRY | 
					   SYNC_DEL_ENTRY |
					   SYNC_UPDATE_STATE |
					   SYNC_UPDATE_MAC |
					   SYNC_UPDATE_HOSTNAME |
					   SYNC_UPDATE_ACTIVE_TIME |
					   SYNC_UPDATE_INACTIVE_TIME |
					   SYNC_UPDATE_PORT)) {
		save_flag = 1;
	}
	
	if (p_lan_host_entry == NULL || update_flag == 0) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	snprintf(node_name, sizeof(node_name), "LANHost2_Entry%d", entry_index);
	if (update_flag & SYNC_DEL_ENTRY) {
		tcapi_unset(node_name);
#if defined(TCSUPPORT_CT_UBUS)
		active_old[entry_index] = 0;
#endif
		snprintf(node_name, sizeof(node_name), "LANHost2_Common");
		tcapi_commit(node_name);
		return 0;
	}

	/* this node delete by dbus or other app */
	if ((update_flag & SYNC_NEW_ENTRY) == 0) {
		if (tcapi_get(node_name, "MAC", buf1) != 0) {
			bzero(p_lan_host_entry, sizeof(lan_host_entry_t));
			lanhost_printf(DBG_MSG_ERR, "[%s][%d]>> no this entry!\n", __FUNCTION__, __LINE__);
			return 0;
		}
	}

	if (update_flag & SYNC_UPDATE_STATE) {
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->active);
		tcapi_set(node_name, "Active", buf1);
#if defined(TCSUPPORT_CT_JOYME4)
		/* mark this dev is OTHER BRIDGE dev or not ? */
		if ( p_lan_host_entry->other_br_dev )
			tcapi_set(node_name, "O_B_Dev", buf1);
		else
			tcapi_set(node_name, "O_B_Dev", "0");
#endif
#if defined(TCSUPPORT_CT_UBUS)
		if(active_old[entry_index] != atoi(buf1))
		{
			active_old[entry_index] = atoi(buf1);
			laninfo_devicestatus_change_inform(entry_index);
		}
#endif
#if defined(TCSUPPORT_CUC)
		bzero(&Info, sizeof(Info));
		snprintf(Info.DevName, sizeof(Info.DevName), "%s", p_lan_host_entry->dev_name);
		snprintf(Info.DevType, sizeof(Info.DevType), "%s", p_lan_host_entry->dev_type);
		snprintf(Info.DevMAC, sizeof(Info.DevMAC), "%s", p_lan_host_entry->mac);
		snprintf(Info.DevIP, sizeof(Info.DevIP), "%s", p_lan_host_entry->ip);
		snprintf(Info.Brand, sizeof(Info.Brand), "%s", p_lan_host_entry->brand);
		snprintf(Info.OS, sizeof(Info.OS), "%s", p_lan_host_entry->os);
		Info.ConnectType = p_lan_host_entry->conn_type;
		if(p_lan_host_entry->conn_type)
			Info.Port = 0;
		else
			Info.Port = p_lan_host_entry->port;
		if(p_lan_host_entry->active)
			snprintf(Info.Action, sizeof(Info.Action), "%s", "Online");
		else
			snprintf(Info.Action, sizeof(Info.Action), "%s", "Offline");
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		ret = ecnt_event_send(ECNT_EVENT_DBUS,
						  ECNT_EVENT_DBUS_NOTIFYLANDEVICEACCESS,
						  &Info,
						  sizeof(Info));
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
		/* for block */
		for(i = 0; i < MAX_ACCESSCONTROLBLACKLIST_NUM; i++)
		{
			bzero(nodeName, sizeof(nodeName));
			bzero(accesslimit_mac, sizeof(accesslimit_mac));
			snprintf(nodeName, sizeof(nodeName), "AccessControl_Entry%d", i);
			if(0 != tcapi_get(nodeName, "MAC", accesslimit_mac)
				|| '\0' == accesslimit_mac[0])
				continue;
			if(0 == strcasecmp(p_lan_host_entry->mac, accesslimit_mac))
			{
				tcapi_set(node_name, "Active", "2");
			}
		}
#endif
	}

	if (update_flag & SYNC_UPDATE_MAC) {
		tcapi_set(node_name, "MAC", p_lan_host_entry->mac);
		/*llanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}
	
	if (update_flag & SYNC_UPDATE_IP) {
		if(p_lan_host_entry->port_bind != 0){ /*no ip in case of bridged LAN port*/
#if defined(TCSUPPORT_CT_JOYME4)
			/* update ip for O_B_Dev */
			if ( p_lan_host_entry->other_br_dev )
				tcapi_set(node_name, "IP", p_lan_host_entry->ip);
#else
			tcapi_set(node_name, "IP", "");
#endif
		}
		else{
			tcapi_set(node_name, "IP", p_lan_host_entry->ip);
		}
		
		/*llanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
#if defined(TCSUPPORT_CFG_NG_UNION)
		if(p_lan_host_entry->AddressSrc == 1)
		{
			tcapi_set(node_name, "AddressSrc", "DHCP");
			tcapi_set(node_name, "ExpireDay", p_lan_host_entry->expireDay);
			tcapi_set(node_name, "ExpireTime", p_lan_host_entry->expireTime);
		}
		else
		{
			tcapi_set(node_name, "AddressSrc", "Static");
			tcapi_set(node_name, "ExpireDay", "0");
			tcapi_set(node_name, "ExpireTime", "0");
		}
#endif
		
	}

	if (update_flag & SYNC_ADD_IP) {
		tcapi_set(node_name, "IP", p_lan_host_entry->ip);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> SYNC_ADD_IP!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_IPv6) {
		set_ipv6_from_dhcp_arp(p_lan_host_entry, entry_index);
	}

	if (update_flag & SYNC_UPDATE_CNT) {
		snprintf(buf1, sizeof(buf1), "%llu", p_lan_host_entry->rxbytes);
		snprintf(buf2, sizeof(buf2), "%llu", p_lan_host_entry->txbytes);
		tcapi_set(node_name, "RxBytes", buf1);
		tcapi_set(node_name, "TxBytes", buf2);
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
		snprintf(lastRxByte, sizeof(lastRxByte), "%llu", p_lan_host_entry->Lastrxbytes);
		snprintf(lastTxByte, sizeof(lastTxByte), "%llu", p_lan_host_entry->Lasttxbytes);
		if(p_lan_host_entry->sleeptime > 0)
		{
			snprintf(RxRate_rt, sizeof(RxRate_rt), "%d", (atoi(buf1) - atoi(lastRxByte))/p_lan_host_entry->sleeptime);
			snprintf(TxRate_rt, sizeof(TxRate_rt), "%d", (atoi(buf2) - atoi(lastTxByte))/p_lan_host_entry->sleeptime);
			tcapi_set(node_name, "RxRate_rt", RxRate_rt);
			tcapi_set(node_name, "TxRate_rt", TxRate_rt);
		}
#endif
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS) 
		snprintf(buf1, sizeof(buf1), "%lu", p_lan_host_entry->upspeed);
		snprintf(buf2, sizeof(buf2), "%lu", p_lan_host_entry->downspeed);
		tcapi_set(node_name, "UpSpeed", buf1);
		tcapi_set(node_name, "DownSpeed", buf2);
#endif
	}

	if (update_flag & SYNC_UPDATE_HOSTNAME) {
#if defined(TCSUPPORT_CHARSET_CHANGE) 
		inlen = strlen(p_lan_host_entry->host_name);
		if ( 0 == charsetconv("gb2312", "utf-8", &inlen, &outlen, p_lan_host_entry->host_name, utfValue) )
			tcapi_set(node_name, "HostName", utfValue);
		else	
			tcapi_set(node_name, "HostName", "unknown");	
#else
		memset(base64HostName, 0, sizeof(base64HostName));		
		basic64_encode(p_lan_host_entry->host_name, sizeof(p_lan_host_entry->host_name), base64HostName); 	
		tcapi_set(node_name, "HostName", base64HostName);
#endif		
		/*llanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}		

	if (update_flag & SYNC_UPDATE_ONLINE_TIME) {
		snprintf(buf1, sizeof(buf1), "%u", p_lan_host_entry->online_duration);
		tcapi_set(node_name, "OnlineTime", buf1);
		/*llanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_ACTIVE_TIME) {
		tcapi_set(node_name, "LatestActiveTime", p_lan_host_entry->active_time);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_INACTIVE_TIME) {
		tcapi_set(node_name, "LatestInactiveTime", p_lan_host_entry->inactive_time);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_PORT) {
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->port);
		tcapi_set(node_name, "Port", buf1);
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->port_bind);
		tcapi_set(node_name, "PortBind", buf1);	
		/*llanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_CONNTYPE) {
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->conn_type);
		tcapi_set(node_name, "ConnectionType", buf1);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
	}

	if (update_flag & SYNC_UPDATE_POWER_LEVEL) {
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->power_level);
		tcapi_set(node_name, "PowerLevel", buf1);
#if defined(TCSUPPORT_ANDLINK)
		snprintf(buf1, sizeof(buf1), "%s", p_lan_host_entry->ssid);
		tcapi_set(node_name, "SSID", buf1);
#endif
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter power_leven = %d, buf1=%s!\n", __FUNCTION__, __LINE__, p_lan_host_entry->power_level, buf1);*/
	}

	if (update_flag & SYNC_UPDATE_NEGORATE) {
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->negorate);
		tcapi_set(node_name, "NegoRate", buf1);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter power_leven = %d, buf1=%s!\n", __FUNCTION__, __LINE__, p_lan_host_entry->power_level, buf1);*/
	}

	if (update_flag & SYNC_UPDATE_DUPLEXMODE) {
		tcapi_set(node_name, "DuplexMode", p_lan_host_entry->duplex_mode);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter power_leven = %d, buf1=%s!\n", __FUNCTION__, __LINE__, p_lan_host_entry->power_level, buf1);*/
	}

#if defined(TCSUPPORT_CT_JOYME4)
	ip_status = check_ipv6_device_inform(p_lan_host_entry);
	if ( update_flag & ( SYNC_UPDATE_STATE | SYNC_ADD_IP | SYNC_UPDATE_IP | SYNC_UPDATE_IPv6) )
	{
		if ( p_lan_host_entry->update_flag & IP_FLAG )
		{
			if ( p_lan_host_entry->update_flag & IPV6_FLAG )
				p_lan_host_entry->ipmode = IPV4_IPV6_MODE; /* ipv4/ipv6 */
			else
				p_lan_host_entry->ipmode = IPV4_ONLY_MODE; /* device is online and is ipv4 only */
		}
		else if ( p_lan_host_entry->update_flag & IPV6_FLAG )
		{
			p_lan_host_entry->ipmode = IPV6_ONLY_MODE; /* ipv6 only */
		}
		else
			p_lan_host_entry->ipmode = IPV4_IPV6_MODE; /* ipv4/ipv6 */

		bzero(buf1, sizeof(buf1));
		snprintf(buf1, sizeof(buf1), "%d", p_lan_host_entry->ipmode);
		tcapi_set(node_name, "IPMode", buf1);
	}
#endif

	if (update_flag & SYNC_NEW_ENTRY) {
		snprintf(node_name, sizeof(node_name), "LANHost2_Common");
		tcapi_commit(node_name);
	}
	
	if (update_flag & SYNC_ADD_IP) {
#if defined(TCSUPPORT_CT_JOYME4)
		if ((p_lan_host_entry->update_flag & IP_FLAG) || ip_status)
#endif
		{
			/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> SYNC_ADD_IP-->>send signal!\n", __FUNCTION__, __LINE__);*/
			snprintf(data, sizeof(data), "add;%d", entry_index);
			ret = ecnt_event_send(ECNT_EVENT_LANHOST,
					ECNT_EVENT_LANHOST_UPDATE,
					data,
					strlen(data) + 1);
		}
	}
	else {
#if defined(TCSUPPORT_CT_JOYME4)
		if ((p_lan_host_entry->update_flag & IP_FLAG) || ip_status)
#endif
		{
			snprintf(data, sizeof(data), "update;%d", entry_index);
			if (update_flag & (	SYNC_UPDATE_IP | SYNC_UPDATE_IPv6 | SYNC_UPDATE_PORT )) {			   
				ret = ecnt_event_send(ECNT_EVENT_LANHOST,
								ECNT_EVENT_LANHOST_UPDATE,
								data,
								strlen(data) + 1);
			}
		}
	}


	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);

	return 0;
}

int get_lan_port_bind(int port)
{
	char bind_status[32] = {0};
	char wanNodeName[32] = {0};
	char serviceListName[32] = {0};
	char wanModeName[32] = {0};
	char lanName[32] = {0};
	int i,j,tmp = 0;

	snprintf(lanName, sizeof(lanName), "LAN%d", port);
	
	for(i=0; i<8; i++){
		for(j=0; j<8; j++){
			snprintf(wanNodeName, sizeof(wanNodeName), "Wan_PVC%d_Entry%d", i, j);
			tcapi_get(wanNodeName, "ServiceList", serviceListName);
			tcapi_get(wanNodeName, "WanMode", wanModeName);
			if((strcmp(serviceListName,"OTHER") == 0) && (strcmp(wanModeName,"Bridge") == 0)){
				tcapi_get(wanNodeName, lanName, bind_status);
				tmp = 1;
			    break;
			}		
		}
		if(tmp){
			break;
		}	
	}
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>! i=%d,j=%d,port=%d bind_status:%s\n", __FUNCTION__,i,j,port,bind_status);	
	if(strcmp(bind_status,"Yes") == 0){
		return 1;
	}else{
		return 0;
	}
}

#if defined(TCSUPPORT_ANDLINK)
void alink_send_onlineinfo(int flag)
{
	ecnt_alink_inform_data event_data1;
	char heartBeatTime[16] = {0};
	char step[4] = {0};
	char if6step[4] = {0};

	tcapi_get("alinkmgr_Entry", "heartBeatTime", heartBeatTime);
	tcapi_get("alinkmgr_Entry", "step", step);
	tcapi_get("alinkmgr_Entry", "IF6step", if6step);
	if(((strcmp(step, "8") == 0) && (heartBeatTime[0] != '\0')) || (strcmp(if6step, "3") == 0))
	{
		if(flag == 1)
		{
			memset(&event_data1, 0, sizeof(event_data1));
			strncpy(event_data1.eventType, "DEVICE_REONLINE", sizeof(event_data1.eventType));
			event_data1.respCode = 2001;
			strncpy(event_data1.respCont, "Device reonline success", sizeof(event_data1.eventType));
			ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_INFORM, &event_data1, sizeof(event_data1));
		}
		else
		{
			memset(&event_data1, 0, sizeof(event_data1));
			strncpy(event_data1.eventType, "DEVICE_ONLINE", sizeof(event_data1.eventType));
			event_data1.respCode = 2001;
			strncpy(event_data1.respCont, "Device online success", sizeof(event_data1.eventType));
			ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_INFORM, &event_data1, sizeof(event_data1));
		}
	}
}

int alink_set_lan_host_entry(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	char mac_addr[32]= {0}, host_name[32] = {0};

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
	
	if (p_lan_host_entry == NULL) 
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (set_ip_from_arp(p_lan_host_entry, entry_index, 1) == 1) 
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);

	return -1;
}

#endif

int add_lan_host(lan_host_entry_t *p_lan_host_entry, char *mac, int entry_index, int conn_type, int port)
{
	struct sysinfo info;
	char new_mac[32] = {0};
	char data[64] = {0};
	unsigned int update_flag = 0;
	int ret = 0;
	char *pduplexMode = NULL;
	int ip_ret = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	int ip_status = 0;
#endif

	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>!\n", __FUNCTION__);	
	if (p_lan_host_entry == NULL) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	mac_rm_dot(mac, new_mac);
        /*try to get ip first*/
	ip_ret = set_lan_host_entry(p_lan_host_entry, entry_index);
        /*entry already exist but still not get ip*/
	if(strcmp(p_lan_host_entry->mac,new_mac) == 0 && 0 != ip_ret
#if defined(TCSUPPORT_CT_JOYME4)
		&& 0 == check_ipv6_device_inform(p_lan_host_entry)
#endif
		)
		return -1;
	
	p_lan_host_entry->valid = 1;

	if (conn_type == 0){
		p_lan_host_entry->exist = 1;
#if defined(TCSUPPORT_CT_JOYME4)
		/* online wait for getting ipv4 addr or ipv6 idle finished */
		if ( check_ipv6_device_inform(p_lan_host_entry) )
#endif
		p_lan_host_entry->active = 1;
	}

	/* update mac */
	strncpy(p_lan_host_entry->mac, new_mac, sizeof(p_lan_host_entry->mac) - 1);

	/* get online time */
	get_current_time(p_lan_host_entry->active_time, sizeof(p_lan_host_entry->active_time));
	
	sysinfo(&info);
	p_lan_host_entry->online_time = info.uptime;

	if (p_lan_host_entry->port != port) {
	        p_lan_host_entry->port = port;
                update_flag |= SYNC_UPDATE_PORT;
	}

	if (conn_type == 0){
		p_lan_host_entry->port_bind = get_lan_port_bind(port);
	}
	
	p_lan_host_entry->conn_type = conn_type;
#if defined(TCSUPPORT_ANDLINK)
	if((p_lan_host_entry->conn_type == 0) || (p_lan_host_entry->wlanonline == 1))
	{
		p_lan_host_entry->active = 1;
	}
#else
	if((p_lan_host_entry->conn_type == 0) || 0 == ip_ret){/* set ip successly  */
		p_lan_host_entry->active = 1;
		p_lan_host_entry->exist = 1;
	}
#endif

	/* update negorate */
	if (conn_type == 0) {
		p_lan_host_entry->negorate = get_lan_port_negorate(port);
		update_flag |= SYNC_UPDATE_NEGORATE;
		
		pduplexMode = get_lan_port_duplexmode(port);
		if(NULL == pduplexMode){
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		strncpy(p_lan_host_entry->duplex_mode , pduplexMode, sizeof(p_lan_host_entry->duplex_mode ) - 1);
		p_lan_host_entry->duplex_mode[DUPLEX_LEN] = '\0';
		update_flag |= SYNC_UPDATE_DUPLEXMODE;
	}

#if defined(TCSUPPORT_CT_JOYME4)
	/* try to get ip by new mac from dhcp or arp */
	ip_ret = set_lan_host_entry(p_lan_host_entry, entry_index);

	/* set ip from dhcp or arp when online */
	if(0 == ip_ret){/* set ip successly  */
		p_lan_host_entry->exist = 1;
		p_lan_host_entry->active = 1;
	} else if (1 == p_lan_host_entry->conn_type){
		p_lan_host_entry->exist = WIFI_ONLINING;
		p_lan_host_entry->active = 0;
	}
	
	ip_status = check_ipv6_device_inform(p_lan_host_entry);
	if ( ip_status )
	{
		p_lan_host_entry->exist = 1;
		p_lan_host_entry->active = 1;
		update_flag |= SYNC_UPDATE_IPv6;
	}
#endif
	
	update_flag |= (SYNC_NEW_ENTRY | SYNC_UPDATE_STATE | SYNC_UPDATE_MAC | SYNC_UPDATE_CONNTYPE | SYNC_UPDATE_ACTIVE_TIME);
	sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);

	/* set ip from dhcp or arp when online */
#if defined(TCSUPPORT_ANDLINK)
	if(p_lan_host_entry->conn_type == 0)
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	if ((p_lan_host_entry->update_flag & IP_FLAG) || ip_status)
#endif
	{
		snprintf(data, sizeof(data), "add;%d", entry_index);
		ret = ecnt_event_send(ECNT_EVENT_LANHOST,
						ECNT_EVENT_LANHOST_UPDATE,
						data,
						strlen(data) + 1);
	}
	
#if defined(TCSUPPORT_ANDLINK)
	if(p_lan_host_entry->conn_type == 0)
		alink_send_onlineinfo(0);
#endif
	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit ==>!\n", __FUNCTION__);

	return 0;
	
}

int del_lan_host(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	unsigned int update_flag = 0;
	char data[64] = {0};
	int ret = 0;

	if (p_lan_host_entry == NULL)
		return -1;
	
	bzero(p_lan_host_entry, sizeof(lan_host_entry_t));
	update_flag = SYNC_DEL_ENTRY;
	sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);

	snprintf(data, sizeof(data), "del;%d", entry_index);
	ret = ecnt_event_send(ECNT_EVENT_LANHOST,
					ECNT_EVENT_LANHOST_UPDATE,
					data,
					strlen(data) + 1);
	return 0;
}

/* used for update online state */
int set_lan_host_online(lan_host_entry_t *p_lan_host_entry, int entry_index, int ip_flag)
{
	struct sysinfo info;
	char new_mac[32] = {0};
	unsigned int update_flag = 0;
	char msg[128] = {0}, node_name[32] = {0}, notify_val[32] = {0};
	int ret = 0;
	char *pduplexMode = NULL;
	int ip_ret = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	int ip_status = 0;
#endif

	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>!\n", __FUNCTION__);
	
	if (p_lan_host_entry == NULL) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (p_lan_host_entry->valid == 1 && p_lan_host_entry->active == 1) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> skip this update!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
#if defined(TCSUPPORT_CT_JOYME4)
	ip_status = check_ipv6_device_inform(p_lan_host_entry);
	if ( 0 == ip_status ) 
	{
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> update_flag=[%x] exit!\n", __FUNCTION__, __LINE__, p_lan_host_entry->update_flag);
		return -1;
	}
#endif

	p_lan_host_entry->valid = 1;
	/*If need to update IP again*/
	if(ip_flag)
	{
		/* set ip from dhcp or arp when online */
		p_lan_host_entry->update_flag &= ~IP_FLAG;
		 /*try to get ip first*/
		ip_ret = set_lan_host_entry(p_lan_host_entry, entry_index);
	}
	else
	{
		ip_ret = 0;
	}
	
#if defined(TCSUPPORT_ANDLINK)
	if((p_lan_host_entry->conn_type == 0) || (p_lan_host_entry->wlanonline == 1) || (p_lan_host_entry->wlanonline == 2))
	{
		p_lan_host_entry->active = 1;
	}
#else
	if((p_lan_host_entry->conn_type == 0) || (0 == ip_ret)){
		p_lan_host_entry->active = 1;
	}
#endif
	
#if defined(TCSUPPORT_CT_JOYME4)
	if ( ip_status )
	{
		lanhost_printf(DBG_MSG_INFO, "\n%s>> update IPV6_FLAG online==>!\n", __FUNCTION__);
		p_lan_host_entry->active = 1;
		p_lan_host_entry->exist = 1;
		update_flag |= SYNC_UPDATE_IPv6;
	}
#endif

	/* get online time */
	get_current_time(p_lan_host_entry->active_time, sizeof(p_lan_host_entry->active_time));
	
	sysinfo(&info);
	p_lan_host_entry->online_time = info.uptime;

	/* update negorate */
	if (p_lan_host_entry->conn_type == 0) {
		p_lan_host_entry->negorate = get_lan_port_negorate(p_lan_host_entry->port);
		update_flag |= SYNC_UPDATE_NEGORATE;
		pduplexMode = get_lan_port_duplexmode(p_lan_host_entry->port);
		if(NULL == pduplexMode){
			lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		strncpy(p_lan_host_entry->duplex_mode , pduplexMode, sizeof(p_lan_host_entry->duplex_mode ) - 1);
		p_lan_host_entry->duplex_mode[DUPLEX_LEN] = '\0';
		update_flag |= SYNC_UPDATE_DUPLEXMODE;
	}

#if defined(TCSUPPORT_ANDLINK)
	if(p_lan_host_entry->conn_type == 1)
	{
		update_flag |= SYNC_UPDATE_PORT;
	}
#endif

	update_flag |= (SYNC_UPDATE_STATE | SYNC_UPDATE_ACTIVE_TIME | SYNC_UPDATE_CONNTYPE);
	sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);

#if 0
	/* set ip from dhcp or arp when online */
#if defined(TCSUPPORT_ANDLINK)
	if(p_lan_host_entry->conn_type == 0)
#else 
	if(set_lan_host_entry(p_lan_host_entry, entry_index) != 0 && p_lan_host_entry->port_bind == 0)	{
		return -1;
	}
#endif
		set_lan_host_entry(p_lan_host_entry, entry_index);		
#endif
	/* send online time */
	snprintf(node_name, sizeof(node_name), "LANHost2_Entry%d", entry_index);
	tcapi_get(node_name, "DeviceOnlineNofication", notify_val);
	
#if !defined(TCSUPPORT_ANDLINK)
	if (atoi(notify_val) == 1) {
		snprintf(msg, sizeof(msg), "/com/ctc/igd1/Config/LANHosts/%d;com.ctc.igd1.LANHost;Active:b:%d;LatestActiveTime:s:%s", 
							entry_index + 1, p_lan_host_entry->active, p_lan_host_entry->active_time);
		ret = ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
	}
#else
	if((p_lan_host_entry->conn_type == 0) || (p_lan_host_entry->wlanonline == 2))
	{
		alink_send_onlineinfo(1);
	}
	else if(p_lan_host_entry->wlanonline == 1)
	{
		alink_send_onlineinfo(0);
	}
#endif
	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit ==>!\n", __FUNCTION__);

	return 0;
	
}


int set_lan_host_offline(lan_host_entry_t *p_lan_host_entry, int entry_index, int initflag)
{
	struct sysinfo info;
	unsigned int update_flag = 0;
	struct tm offTime;
	time_t realtime = 0;
	
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>!\n", __FUNCTION__);
	memset(&offTime, 0, sizeof(offTime));
	if (p_lan_host_entry == NULL)
		return -1;

	p_lan_host_entry->active = OFFLINE;

#if !defined(TCSUPPORT_CT_JOYME4)
	bzero(p_lan_host_entry->ip, sizeof(p_lan_host_entry->ip));
#endif
	p_lan_host_entry->update_flag &= ~IP_FLAG;
#if defined(TCSUPPORT_ANDLINK)
	p_lan_host_entry->update_flag &= ~IPV6_FLAG;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	p_lan_host_entry->update_flag &= ~IPV6_FLAG;
	/* we can clear ipv6 addr, it will be update at next sync */
	bzero(p_lan_host_entry->ipv6, sizeof(p_lan_host_entry->ipv6));
	update_flag |= SYNC_UPDATE_IPv6;
#endif

	/* offline time */
	if (initflag != 1 || p_lan_host_entry->inactive_time[0] == '\0'
		|| strstr(p_lan_host_entry->inactive_time, "1970-"))
	{
		if(!strstr(p_lan_host_entry->active_time, "1970-"))
		{
			strptime(p_lan_host_entry->active_time, "%Y-%m-%d %H:%M:%S", &offTime);
			realtime = mktime(&offTime);

			realtime += p_lan_host_entry->online_duration;
			memset(&offTime, 0, sizeof(offTime));
			localtime_r(&realtime, &offTime);

			strftime(p_lan_host_entry->inactive_time, TIME_LEN, "%F %T", &offTime);
		}
		else
			get_current_time(p_lan_host_entry->inactive_time, sizeof(p_lan_host_entry->inactive_time));

	}

	sysinfo(&info);
	p_lan_host_entry->offline_time = info.uptime;

	bzero(p_lan_host_entry->host_name, sizeof(p_lan_host_entry->host_name));
	p_lan_host_entry->update_flag &= ~HOST_NAME_FLAG;

	update_flag = (SYNC_UPDATE_STATE | SYNC_UPDATE_IP | SYNC_UPDATE_INACTIVE_TIME);
#if defined(TCSUPPORT_CT_JOYME4)
	if ( p_lan_host_entry->other_br_dev )
	{
		/* clean ip_type and ipv6 addr */
		p_lan_host_entry->ip_type = D_TYPE_IP_IP6;
		bzero(p_lan_host_entry->ipv6, sizeof(p_lan_host_entry->ipv6));
		update_flag |= SYNC_UPDATE_IPv6;
		p_lan_host_entry->other_br_dev = 0;
	}

	/* clear IPV6_IDLE_FLAG and detect cnt */
	p_lan_host_entry->update_flag &= ~IPV6_IDLE_FLAG;
	CLR_IPV6_IDLE_CNT(p_lan_host_entry->update_flag);
#endif
	sync_lan_host_info(entry_index, p_lan_host_entry, update_flag);

	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit==>!\n", __FUNCTION__);
	
	return 0;
}

#define PORTMAP_PATH		"/proc/tc3162/eth_portmap"
#define MAXSIZE	160
unsigned int get_lan_port_negorate(int port)
{
	FILE *fp = NULL;
	char buf[128] = {0};
	char port_buf[64] = {0}, link_rate[64] = {0}, real_rate[64] = {0};
	int port_line = 0, line = 0;

	int ret = 0;
	int lanMapStart = 0;
	int switch_port[7] = {-1, -1, -1, -1, -1, -1, -1};

	char pmbuf[MAXSIZE] = {0};
	int portmapl = -1;
	int portmapr = -1;

	FILE *pmfp = NULL;

	pmfp = fopen(PORTMAP_PATH, "r");
	if ( NULL == pmfp )
	{
		lanhost_printf(DBG_MSG_ERR, "\n[%s][%d]>> open %s fail!\n", __FUNCTION__, __LINE__, PORTMAP_PATH);
		return 0;
	}

	while ( fgets(pmbuf, MAXSIZE, pmfp) )
	{
		if(strstr(pmbuf, "switch_port_map"))
			break;

		if(strstr(pmbuf, "lan_port_map"))
		{
			lanMapStart = 1;
			continue;
		}
		else if(!lanMapStart)
			continue;

		ret = sscanf(pmbuf, "%d %d", &portmapl, &portmapr);
		if ( 2 != ret )
		{
			portmapl = -1;
			portmapr = -1;
			memset(pmbuf, 0, sizeof(pmbuf));
			continue;
		}

		if(portmapl>=0 && portmapl<=6)
		{
			switch_port[portmapl] = portmapr;
		}
		portmapl = -1;
		portmapr = -1;
		memset(pmbuf, 0, sizeof(pmbuf));
	}
	if(pmfp)
		fclose(pmfp);

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> switch port:%d %d %d %d==>!\n", \
		__FUNCTION__, __LINE__, switch_port[0], switch_port[1], switch_port[2], switch_port[3]);

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter, port=[%d]==>!\n", __FUNCTION__, __LINE__, port);
	
	if (lan_port_rate[port - 1] > 0)
		return lan_port_rate[port - 1];

	fp = fopen("/proc/tc3162/gsw_link_st", "r");
	if (fp == NULL)
		return 0;

	bzero(buf, sizeof(buf));
	while (fgets(buf, sizeof(buf), fp))
	{
		if (strstr(buf, "Port") == NULL)
			continue;
			
		sscanf(buf, "%s : %s", port_buf, link_rate);
		if (switch_port[port - 1] == port_line)
			break;
		port_line++;
	}
	if(fp)
		fclose(fp);

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter link_rate = [%s]==>!\n", __FUNCTION__, __LINE__, link_rate);

	sscanf(link_rate, "%[^/]", real_rate);
	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter real_rate = [%s]==>!\n", __FUNCTION__, __LINE__, real_rate);
	
	if (!strcmp(real_rate, "10M"))
		lan_port_rate[port - 1] = 10000;
	else if (!strcmp(real_rate, "100M")) 
		lan_port_rate[port - 1] = 100000;
	else if (!strcmp(real_rate, "1000M"))
		lan_port_rate[port - 1] = 1000000;

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter link_rate = [%d]==>!\n", __FUNCTION__, __LINE__, lan_port_rate[port - 1]);
		
	return lan_port_rate[port - 1];
}

char *get_lan_port_duplexmode(int port)
{
	FILE *fp = NULL;
	char buf[128] = {0};
	char port_buf[64] = {0}, link_rate[64] = {0}, real_duplex[64] = {0};
	int port_line = 0, line = 0;

	lanhost_printf(DBG_MSG_ERR, "\n[%s][%d]>> enter, port=[%d]==>!\n", __FUNCTION__, __LINE__, port);
	if (port > 4 || port < 1)
		return NULL;
	
	fp = fopen("/proc/tc3162/gsw_link_st", "r");
	if (fp == NULL)
		return NULL;

	bzero(buf, sizeof(buf));
	while (fgets(buf, sizeof(buf), fp))
	{
		if (strstr(buf, "Port") == NULL)
			continue;
			
		sscanf(buf, "%s : %s", port_buf, link_rate);
		if (port == port_line)
			break;
		port_line++;
	}
	fclose(fp);
	if(port != port_line){
		strncpy(lan_port_duplex[port - 1] , "Auto", DUPLEX_LEN);
		lan_port_duplex[port - 1][DUPLEX_LEN] = '\0';
		return lan_port_duplex[port - 1];
	}		
	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter link_rate = [%s]==>!\n", __FUNCTION__, __LINE__, link_rate);

	sscanf(link_rate, "%*[^/]/%[^ ]", real_duplex);
	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter real_duplex = [%s]==>!\n", __FUNCTION__, __LINE__, real_duplex);
	
	if (!strcmp(real_duplex, "Half"))
		strncpy(lan_port_duplex[port - 1] , "Half", DUPLEX_LEN);
	else if (!strcmp(real_duplex, "Full")) 
		strncpy(lan_port_duplex[port - 1] , "Full", DUPLEX_LEN);
	else
		strncpy(lan_port_duplex[port - 1] , "Auto", DUPLEX_LEN);
	lan_port_duplex[port - 1][DUPLEX_LEN] = '\0';	
	return lan_port_duplex[port - 1];
}

int get_mapping_idx(int old_idx)
{
	char node_name[32] = {0}, buf[32] = {0};
#if defined(TCSUPPORT_CUC)
	int index = 0;
#endif

	snprintf(node_name, sizeof(node_name), "WLanMapping_Entry%d", old_idx);
	
#if defined(TCSUPPORT_CUC)
	tcapi_get(node_name, "SSIDAlias", buf);
	if(strstr(buf, "2.4G"))
	{
		sscanf(buf, "2.4G-%d", &index);
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter  index=[%d]==>!\n", __FUNCTION__, __LINE__, index);
		return index - 1;
	}
	else
	{
		sscanf(buf, "5G-%d", &index);
				lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter  index=[%d]==>!\n", __FUNCTION__, __LINE__, index);
		return index + MAX_ECNT_WALN_USE_PORT_NUM - 1;
	}	
#else
	tcapi_get(node_name, "reverseidx", buf);

	return atoi(buf);
#endif
}

#if defined(TCSUPPORT_CT_JOYME4)
int init_ipv6_detect_queue(void)
{
	if ( 0 != pthread_mutex_init(&gIPv6QueueMutex, NULL) )
	{
		printf("%s pthread_mutex_init failed.\n", __FUNCTION__);
		return -1;
	}

	g_ipv6_queue = QueueInit();
	if ( NULL == g_ipv6_queue )
	{
		printf("%s g_queue_new failed.\n", __FUNCTION__);
		return -2;
	}

	return 0;
}


int destroy_ipv6_detect_queue(void)
{
	pthread_mutex_destroy(&gIPv6QueueMutex);

	if ( g_ipv6_queue )
		QueueFree(g_ipv6_queue, 1);

	return 0;
}
#endif

