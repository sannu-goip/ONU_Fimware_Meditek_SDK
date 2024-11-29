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
#include "debug.h"
#include "lan_host_msg.h"
#include "ecnt_event_global/ecnt_event_alink.h"
#if defined(TCSUPPORT_ANDLINK)
#include <libapi_lib_wifimgr.h>
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#include "queue_api.h"
#endif

extern int lan_port_status[LAN_PORT_NUM];
extern unsigned int lan_port_rate[LAN_PORT_NUM];
extern char lan_port_duplex[LAN_PORT_NUM][DUPLEX_LEN+1];
extern int check_flag[MAX_LAN_HOST_NUM];

#if defined(TCSUPPORT_CT_JOYME4)
pthread_mutex_t gIPv6QueueMutex = PTHREAD_MUTEX_INITIALIZER;
Queue *g_ipv6_queue = NULL;
#endif

int sock_add_epoll(int sock, int epoll)
{
	struct epoll_event event;
	int ret = 0;

	if( sock < 0 ){
		return -1;
	}
	ret = fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
	
	memset(&event, 0,  sizeof(struct epoll_event));
	event.data.fd = sock;		
	event.events = EPOLLIN | EPOLLET;	
	
	epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &event);

	return 0;
}

/* ecnt event hander, there are many events should be handle:
1) wifi mac table add event:
2) wifi mac table del event:
3) dhcp reinit event: update lan hostname/ip etc.
4) LAN Down event: should set all macs upder this lan port to offline state;
*/
int ecnt_event_handler(int sock, struct msghdr *pmsg, struct nlmsghdr *pnlh)
{
	int ret = 0;

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter==>!\n", __FUNCTION__, __LINE__);

	while(1)
	{
		ret = recvmsg(sock, pmsg, 0);
		if(ret < 0) {
			lanhost_printf(DBG_MSG_INFO, "%s>> sock=%d recv end[%s]!\n", __FUNCTION__, sock, strerror(errno));
			return -1;
		} 

		lanhost_printf(DBG_MSG_INFO, "%s>> sock=%d type[%d]!\n", __FUNCTION__, sock, pnlh->nlmsg_type);

		if (pnlh->nlmsg_type == ((ECNT_EVENT_WIFI << 8) | ECNT_EVENT_WIFI_UPDATESTAINFO)) {
			wifi_update_handler(pnlh);
		}
#if defined(TCSUPPORT_CT_JOYME4)
		else if (pnlh->nlmsg_type == ((ECNT_EVENT_LANHOST << 8) | ECNT_EVENT_LANHOST_DHCP_UPDATE)) {
			dhcp_update_hander(pnlh);
		}
		else if (pnlh->nlmsg_type == ((ECNT_EVENT_LANHOST << 8) | ECNT_EVENT_LANHOST_IPV6_CHECK)) {
			ipv6_check_hander(pnlh);
		}
#else
		else if (pnlh->nlmsg_type == ((ECNT_EVENT_LANHOST << 8) | ECNT_EVENT_LANHOST_DHCP_REINIT)) {
			dhcp_reinit_hander(pnlh);
		}
#endif
		else if (pnlh->nlmsg_type == ((ECNT_EVENT_ETH << 8) | ECNT_EVENT_ETH_UP)) {
			lan_up_handler(pnlh);
		}
		else if (pnlh->nlmsg_type == ((ECNT_EVENT_ETH << 8) | ECNT_EVENT_ETH_DOWN)) {
			lan_down_handler(pnlh);
		}
		else if ( pnlh->nlmsg_type == ((ECNT_EVENT_ALINK << 8) | ECNT_EVENT_ALINK_BRIDGE_DEV) )
		{
			alink_bridge_dev_handler(pnlh);
		}
	}
	
	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
	
	return 0;	
}

int wifi_update_handler(struct nlmsghdr *nlh)
{
    char *data = NULL;
	struct ecnt_event_data *event_data = NULL;
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char cmd[16] = {0}, ssid_idx_buf[16] = {0}, entry_buf[16] = {0}, rssi_buf[16] = {0}, data_buf[16] = {0};
	char *temp = NULL;
	int old_ssid_idx = 0, sta_index = 0, lan_host_entry = -1, flag = 0;
	unsigned int update_flag = 0;
	char node_name[32] = {0}, mac_addr[32] = {0}, new_mac[32] = {0};
#if defined(TCSUPPORT_ANDLINK)
	ecnt_alink_inform_data event_data1;
#endif
		
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	data = (char*)event_data;
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter data=[%s]==>!\n", __FUNCTION__, data);

	temp = strtok(data, ";");
	while (temp) {
		if (flag == 0)
			strncpy(cmd, temp, sizeof(cmd) - 1);
		else if (flag == 1)
			strncpy(ssid_idx_buf, temp, sizeof(ssid_idx_buf) - 1);
		else if (flag == 2)
			strncpy(entry_buf, temp, sizeof(entry_buf) - 1);
		else if (flag == 3)
			strncpy(mac_addr, temp, sizeof(mac_addr) - 1);
		else if (flag == 4)
			strncpy(rssi_buf, temp, sizeof(rssi_buf) - 1);
		else if (flag == 5)
			strncpy(data_buf, temp, sizeof(data_buf) - 1);
		temp = strtok(NULL, ";");
		flag++;
	}
	
	sscanf(ssid_idx_buf, "%d", &old_ssid_idx);
	sscanf(entry_buf, "%d", &sta_index);
	
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter cmd=[%s], ssid_idx_buf=[%s], entry_buf=[%s], mac_addr=[%s], old_ssid_idx=[%d], entry_index=[%d]==>!\n", 
					__FUNCTION__, cmd, ssid_idx_buf, entry_buf, mac_addr, old_ssid_idx, sta_index);

	mac_rm_dot(mac_addr, new_mac);

	if (!strcmp(cmd, "add")) {
		/* the code should be enhance */
		p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &lan_host_entry);
		if (p_lan_host_entry) {
			p_lan_host_entry->conn_type = 1;
			set_lan_host_online(p_lan_host_entry, lan_host_entry, 1);
			if (p_lan_host_entry->port != old_ssid_idx + 1) {
				update_flag = SYNC_UPDATE_PORT;
				lanhost_printf(DBG_MSG_INFO, "\n%s>> port:%d->%d ==>!\n", __FUNCTION__,
					p_lan_host_entry->port, (old_ssid_idx + 1));
				p_lan_host_entry->port = old_ssid_idx + 1;
				sync_lan_host_info(lan_host_entry, p_lan_host_entry, update_flag);				
			}
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->power_level = atoi(rssi_buf);
			p_lan_host_entry->port = old_ssid_idx + 1;
			if(old_ssid_idx < MAX_ECNT_WALN_PORT_NUM)
				snprintf(node_name, sizeof(node_name), "WLan_Entry%d", old_ssid_idx);
			else
				snprintf(node_name, sizeof(node_name), "WLan11ac_Entry%d", old_ssid_idx - MAX_ECNT_WALN_PORT_NUM);

			tcapi_get(node_name, "SSID", p_lan_host_entry->ssid);
#endif

			set_lan_host_online(p_lan_host_entry, lan_host_entry, 1);
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->wlanonline = 2;
#endif

		}
		else {
			p_lan_host_entry = find_empty_lan_host_entry(&lan_host_entry);
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->power_level = atoi(rssi_buf);

			if(old_ssid_idx < MAX_ECNT_WALN_PORT_NUM)
				snprintf(node_name, sizeof(node_name), "WLan_Entry%d", old_ssid_idx);
			else
				snprintf(node_name, sizeof(node_name), "WLan11ac_Entry%d", old_ssid_idx - MAX_ECNT_WALN_PORT_NUM);

			tcapi_get(node_name, "SSID", p_lan_host_entry->ssid);
#endif
			
			add_lan_host(p_lan_host_entry, new_mac, lan_host_entry, 1, old_ssid_idx + 1);
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->wlanonline = 1;
#endif

		}
		
		if (p_lan_host_entry == NULL) {
			lanhost_printf(DBG_MSG_ERR, "\n%s>> exit for no empty entry ==>!\n", __FUNCTION__);
			return -1;
		}
		if (0 == (p_lan_host_entry->update_flag & IP_FLAG) ||
			!strcasecmp(p_lan_host_entry->ip,"")){
			p_lan_host_entry->exist = WIFI_ONLINING;
			lanhost_printf(DBG_MSG_INFO, "\n%s>> wifi no ip set exist to 2  ==>!\n", __FUNCTION__);
		}
	}
	else if (!strcmp(cmd, "del")) {
		p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &lan_host_entry);
		lanhost_printf(DBG_MSG_INFO, "\n%s>> del and set wifi offline!\n", __FUNCTION__);
		if (p_lan_host_entry && (p_lan_host_entry->port == old_ssid_idx + 1))
		{
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->wlanonline = 0;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			if ( p_lan_host_entry->other_br_dev )
				p_lan_host_entry->lock_lost = LOCK_LOST_CNT;
#endif
			set_lan_host_offline(p_lan_host_entry, lan_host_entry, 0);
		}
	}
	else if (!strcmp(cmd, "update")) {
		p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &lan_host_entry);
		if (p_lan_host_entry) {
			p_lan_host_entry->power_level = atoi(rssi_buf);
			p_lan_host_entry->negorate = atoi(data_buf) * 1000; /* kbps */
			if (p_lan_host_entry->port != old_ssid_idx + 1) {
				update_flag = SYNC_UPDATE_PORT;
				p_lan_host_entry->port = old_ssid_idx + 1;
			}

#if defined(TCSUPPORT_ANDLINK)			
			if(old_ssid_idx < MAX_ECNT_WALN_PORT_NUM)
				snprintf(node_name, sizeof(node_name), "WLan_Entry%d", old_ssid_idx);
			else
				snprintf(node_name, sizeof(node_name), "WLan11ac_Entry%d", old_ssid_idx - MAX_ECNT_WALN_PORT_NUM);

			tcapi_get(node_name, "SSID", p_lan_host_entry->ssid);
#endif
			update_flag |= (SYNC_UPDATE_POWER_LEVEL | SYNC_UPDATE_NEGORATE | SYNC_UPDATE_DUPLEXMODE);
			sync_lan_host_info(lan_host_entry, p_lan_host_entry, update_flag);
		}
	}

	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit normal ==>!\n", __FUNCTION__);
	
	return 0;
}

int dhcp_reinit_hander(struct nlmsghdr *nlh)
{
    char *data = NULL;
	struct ecnt_event_data *event_data = NULL;
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;	
	char ip[32] = {0}, host_name[32] = {0};
	int i = 0, update_flag = 0;
		
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	data = (char *)event_data;
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>!\n", __FUNCTION__);

	/* update lan hostname  and other dhcp parameter */
	p_lan_host_info = get_lan_host_info();
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		set_ip_from_dhcp(p_lan_host_entry, i);
	}

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME4)
int ipv6_check_hander(struct nlmsghdr *nlh)
{
	ecnt_ip6_info_dev_data *info_ptr = NULL;
	ipv6_qnode_t *nodeinfo = NULL;
	char *pipv6addr = NULL;
	int ret = 0;

	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==>!\n", __FUNCTION__);

	info_ptr = (ecnt_ip6_info_dev_data *)(NLMSG_DATA(nlh));
	if ( NULL == info_ptr )
		return -1;

	pipv6addr = (char *)malloc(IPv6_ADDR_LEN + 1);
	if ( NULL == pipv6addr )
		goto set_err;
	memset(pipv6addr, 0, IPv6_ADDR_LEN + 1);
	snprintf(pipv6addr, IPv6_ADDR_LEN, "%s", info_ptr->ip6addr);

	/* push into ipv6 queue */
	nodeinfo = (ipv6_qnode_t *)malloc(sizeof(ipv6_qnode_t));
	if ( NULL == nodeinfo )
		goto set_err;
	nodeinfo->ipv6_addr = pipv6addr;
	nodeinfo->ip_type = info_ptr->type;
	nodeinfo->detect_cnt = 0;
	G_LOCK(&gIPv6QueueMutex);
	ret = QueuePushTail(g_ipv6_queue, nodeinfo);
	G_UNLOCK(&gIPv6QueueMutex);
	if ( 0 != ret )
	{
		if ( nodeinfo )
			free(nodeinfo);
		printf("%s QueuePushTail [%s] failed.\n", __FUNCTION__, pipv6addr);
		goto set_err;
	}
#if 0	
	sleep(1); /* need wait for neigh udpate. */
	bzero(host_mac, sizeof(host_mac));
	get_mac_addr_via_neigh(info_ptr->ip6addr, host_mac, sizeof(host_mac));
	if ( 0 == host_mac[0] )
		return -2;

	bzero(host_v6_addr, sizeof(host_v6_addr));
	bzero(s_lladdr, sizeof(s_lladdr));

	/* link local addr, maybe radvd without M flag. */
	if ( 2 == info_ptr->type )
	{
		snprintf(s_lladdr, sizeof(s_lladdr), "%s", info_ptr->ip6addr);

		bzero(&entry_tmp, sizeof(entry_tmp));
		detect_slaac_address(&entry_tmp, 0, s_lladdr, 1, host_v6_addr, sizeof(host_v6_addr));

		if ( 0 != host_v6_addr[0] )
		{
			/* add to new entry. */
			lan_host_ipv6only_update(host_mac, host_v6_addr);
		}
	}
	/* global addr. */
	else if ( 1 == info_ptr->type )
	{
		snprintf(host_v6_addr, sizeof(host_v6_addr), "%s", info_ptr->ip6addr);
		if ( 0 != host_v6_addr[0] )
		{
			/* add to new entry. */
			lan_host_ipv6only_update(host_mac, host_v6_addr);
		}
	}
#endif

	return 0;

set_err:
	if ( pipv6addr )
		free(pipv6addr);

	printf("%s set_err.\n", __FUNCTION__);
	return -1;
}

int dhcp_update_hander(struct nlmsghdr *nlh)
{
    char *data = NULL;
	struct ecnt_event_data *event_data = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;	
	int entry_index = 0;
	char type[8] = {0}, macaddr[20] = {0}, new_mac[20] = {0};
	char cmd[128] = {0};
		
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	data = (char *)event_data;
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter==> data=[%s]!\n", __FUNCTION__, data);
	sscanf(data, "%[^;];%s", type, macaddr);

	mac_rm_dot(macaddr, new_mac);
	if ( 'd' == type[0] )
	{
		if ( NULL != ( p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &entry_index) ) )
		{
			/* set arp msg to death */
			if ( 0 != p_lan_host_entry->ip[0] )
			{
				bzero(cmd, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/sbin/arp -d %s", p_lan_host_entry->ip);
				system(cmd);
			}
			set_lan_host_offline(p_lan_host_entry, entry_index, 0);
		}
	}
	else if ( 'a' == type[0] )
	{
		if ( NULL != ( p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &entry_index) ) )
		{
			if ( 1 == set_ip_from_dhcp(p_lan_host_entry, entry_index) )
				set_lan_host_online(p_lan_host_entry, entry_index, 0);
		}
	}

	return 0;
}
#endif

int lan_up_handler(struct nlmsghdr *nlh)
{
    struct eth_event_data *data = NULL;
	struct ecnt_event_data *event_data = NULL;
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	int i;
		
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	data = (struct eth_event_data*)event_data;
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter data->lan_port=[%d]==>!\n", __FUNCTION__, data->lan_port);

	/* update lan port status */
	lan_port_status[data->lan_port] = 1;

	/* update lan port rate */
	if (data->link_speed == SPEED_10M)
		lan_port_rate[data->lan_port] = 10000;
	else if (data->link_speed == SPEED_100M)
		lan_port_rate[data->lan_port] = 100000;
	else if (data->link_speed == SPEED_1000M)
		lan_port_rate[data->lan_port] = 1000000;
	
	/* update lan port duplex */
	if (data->link_duplex == HALF_DUPLEX)
		strncpy(lan_port_duplex[data->lan_port] , "Half", DUPLEX_LEN);
	else if (data->link_duplex == FULL_DUPLEX)
		strncpy(lan_port_duplex[data->lan_port] , "Full", DUPLEX_LEN);
	else 
		strncpy(lan_port_duplex[data->lan_port] , "Auto", DUPLEX_LEN);
	lan_port_duplex[data->lan_port][DUPLEX_LEN] = '\0';	

	for (i = 0; i < MAX_LAN_HOST_NUM; i++)
		check_flag[i] = 3;

	/* flush arp */
	system("/usr/bin/ip neigh flush dev br0 2>/dev/null");

	return 0;
}

int lan_down_handler(struct nlmsghdr *nlh)
{
    struct eth_event_data *data = NULL;
	struct ecnt_event_data *event_data = NULL;
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	int i;
		
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	data = (struct eth_event_data*)event_data;
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter, port=[%d]==>!\n", __FUNCTION__, data->lan_port);

	/* update lan port status */
	lan_port_status[data->lan_port] = 0;

	p_lan_host_info = get_lan_host_info();
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if (p_lan_host_entry->valid && 
			p_lan_host_entry->active && 
			(p_lan_host_entry->conn_type == 0) && 
			p_lan_host_entry->port == data->lan_port + 1) {
#if defined(TCSUPPORT_CT_JOYME4)
			if ( p_lan_host_entry->other_br_dev )
				p_lan_host_entry->lock_lost = LOCK_LOST_CNT;
#endif
			set_lan_host_offline(p_lan_host_entry, i, 0);
		}
	}

	/* flush arp */
	system("/usr/bin/ip neigh flush dev br0 2>/dev/null");
	
	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit==>!\n", __FUNCTION__);
	
	return 0;
	
}

int alink_bridge_dev_handler(struct nlmsghdr *nlh)
{
	struct ecnt_event_data *event_data = NULL;
	ecnt_alink_bridge_dev_data *dev_data;
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char mac_addr[32] = {0};
	int i = 0, lan_port = 0;
	unsigned int update_flag = 0;

	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));
	dev_data = (ecnt_alink_bridge_dev_data *)event_data;
	
	lanhost_printf(DBG_MSG_INFO, "\n%s>> enter, ip=[%s], mac=[%s]==>!\n"
				, __FUNCTION__, dev_data->ipaddr, dev_data->macaddr);

	/* only for V4 now. */
	p_lan_host_info = get_lan_host_info();
	if ( NULL == p_lan_host_info )
	{
		lanhost_printf(DBG_MSG_ERR, "[%s][%d] >> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	mac_rm_dot(dev_data->macaddr, mac_addr);
	for ( i = 0; i < MAX_LAN_HOST_NUM; i++ )
	{
		p_lan_host_entry = &(p_lan_host_info->lan_host_entry[i]);
		if ( 0 == strcasecmp(mac_addr, p_lan_host_entry->mac)
			&& 1 == p_lan_host_entry->valid )
		{
			lan_port = get_lan_port_by_mac(p_lan_host_entry->mac);
			lanhost_printf(DBG_MSG_INFO, "\n%s>> lan_port=[%d], mac=[%s]==>!\n"
				, __FUNCTION__, lan_port, p_lan_host_entry->mac);
			if ( lan_port > 0 )
			{
				if ( (OFFLINE == p_lan_host_entry->active || 0 == p_lan_host_entry->ip[0]) 
					|| (ONLINE == p_lan_host_entry->active && strcasecmp(dev_data->ipaddr,p_lan_host_entry->ip) != 0) )
				{
					snprintf(p_lan_host_entry->ip, sizeof(p_lan_host_entry->ip)
							, "%s", dev_data->ipaddr);
					p_lan_host_entry->update_flag |= IP_FLAG;
					update_flag = SYNC_UPDATE_IP;
					if ( 0 == p_lan_host_entry->conn_type
						&& lan_port <= 4 )
					{
						update_flag |= SYNC_UPDATE_PORT;
						p_lan_host_entry->port = lan_port;
					}
					sync_lan_host_info(i, p_lan_host_entry, update_flag);
					set_lan_host_online(p_lan_host_entry, i, 0);
				}
			}
		}
	}
	
	lanhost_printf(DBG_MSG_INFO, "\n%s>> exit==>!\n", __FUNCTION__);

	return 0;
	
}

int create_ecnt_event_sock(int epoll)
{
	struct sockaddr_nl nl_saddr;
	int sock_fd = 0;
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));	
	sock_fd = ecnt_netlink_create(&nl_saddr, getpid());

	if (sock_fd < 0) {
		return -1;
	}
	
	if (sock_add_epoll(sock_fd, epoll) == -1) {
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}

/* arp event hander, there are two event
1) arp add event, if this event happen, it should update ip to lan host entry;
2) arp del event, it this event happen, it should set lan host entry to offload state.
*/
int arp_event_handler(int sock, struct msghdr *pmsg, struct nlmsghdr *pnlh)
{
	struct nlmsghdr *h;
	struct rtattr * tb[NDA_MAX+1];
	struct ndmsg *r;
	char abuf[256] = {0};
	char lladdr[64] = {0};
	int ret = 0, i=0, index = -1,type = 0;
	char portidx[32] = {0};
	int ip_type = -1;

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter==>!\n", __FUNCTION__, __LINE__);

	while (1) {
		memset(pnlh, 0, NLMSG_SPACE(MAX_MSGSIZE));
		ret = recvmsg(sock, pmsg, 0);
		if (ret <= 0) {
			fprintf(stderr, "EOF on netlink\n");
			lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> EOF on netlink==>!\n", __FUNCTION__, __LINE__);
			return -1;
		}

		for (h = pnlh; NLMSG_OK(h, ret); h = NLMSG_NEXT(h, ret)) {
			lanhost_printf(DBG_MSG_INFO, "%s>> type=[%d]!\n", __FUNCTION__, h->nlmsg_type);
			r = NLMSG_DATA(h);
			parse_rtattr(tb, NDA_MAX, NDA_RTA(r), h->nlmsg_len - NLMSG_LENGTH(sizeof(*r)));
			if (tb[NDA_LLADDR]) {	
				/*get Mac*/
				ll_addr_n2a(RTA_DATA(tb[NDA_LLADDR]),
			      RTA_PAYLOAD(tb[NDA_LLADDR]),lladdr, sizeof(lladdr));

				/*get IP*/
				if (NULL == inet_ntop(r->ndm_family, RTA_DATA(tb[NDA_DST]), abuf, sizeof(abuf)))
				{
					continue;
				}

				if (AF_INET == r->ndm_family)
				{
#if defined(TCSUPPORT_CT_JOYME4)
					/* ignore arp not in the same subnet with lan */
					if ( 1 != check_lan_same_subnet(abuf) )
					{
						lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> ignore not in the same subnet, Invalid arp=[%s]!\n", __FUNCTION__, __LINE__, abuf);
						continue;
					}
#endif
					ip_type = 0;
				}
				else if (AF_INET6 == r->ndm_family)
				{
					ip_type = 1;
				}
				lanhost_printf(DBG_MSG_INFO, "%s>> mac lladdr=%s, ip=%s!\n", __FUNCTION__, lladdr, abuf);
#if 0
				if (inet_addr(abuf) == INADDR_NONE)
					continue;		
#endif											
			}
			
			switch (h->nlmsg_type) {
				case RTM_NEWNEIGH:	
					arp_add_handler(lladdr, abuf, ip_type);	
					break;
				case RTM_DELNEIGH:
					arp_del_handler(lladdr, abuf, ip_type);
					break;
				default:
					lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit nlmsg_type=[%d]==>!\n", __FUNCTION__, __LINE__, h->nlmsg_type);
					break;
			}
		}
	}

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);

	return 0;
}

/* find this entry in lanhost group, 
update ip info to lanhost entry */
int arp_add_handler(char *lladdr, char *abuf, int ip_type) 
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char mac_addr[32] = {0};
	int i, found = 0, lan_port = 0, lan_port_st = 0;
	unsigned int update_flag = 0;
	char ip[32];

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if (lladdr == NULL || abuf == NULL)
		return -1;

	p_lan_host_info = get_lan_host_info();
	if (p_lan_host_info == NULL) {
		lanhost_printf(DBG_MSG_ERR, "[%s][%d] >> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	mac_rm_dot(lladdr, mac_addr);
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &(p_lan_host_info->lan_host_entry[i]);

		if (strcasecmp(mac_addr, p_lan_host_entry->mac) == 0) {
			if (p_lan_host_entry->valid == 1) {
				lan_port = get_lan_port_by_mac(p_lan_host_entry->mac);
#if !defined(TCSUPPORT_CT_CONGW_CH) 
				if (lan_port >= 1 && lan_port <= 4)
#endif
					lan_port_st = lan_port_status[lan_port - 1];
				else
					lan_port_st = 0;
				
				lanhost_printf(DBG_MSG_INFO, "[%s][%d] >> active=%d, i=%d, lan_port=[%d],lan_port_st=[%d]\n", __FUNCTION__, __LINE__, p_lan_host_entry->active, i, lan_port, lan_port_st);				
				if (0 == ip_type)
				{
					/*static ip changed*/
					if(strcasecmp(p_lan_host_entry->ip, abuf) != 0)
						p_lan_host_entry->update_flag &= ~IP_FLAG;

					lanhost_printf(DBG_MSG_INFO, "[%s][%d] >> exist:%d, conn_type:%d\n", __FUNCTION__, __LINE__, p_lan_host_entry->exist,p_lan_host_entry->conn_type);
					
					if (p_lan_host_entry->active == OFFLINE || (p_lan_host_entry->update_flag & IP_FLAG) == 0) {
						/* if lan port, this lan port should up */
						if ((p_lan_host_entry->conn_type == 1) || (p_lan_host_entry->conn_type == 0 && lan_port_st == 1)) {
							/* update IP first */
							if ((p_lan_host_entry->update_flag & IP_FLAG) == 0) {
								/*if IP not exist, need send add signal*/
								if (!strcasecmp(p_lan_host_entry->ip,""))
									update_flag = SYNC_ADD_IP;
								else
									update_flag = SYNC_UPDATE_IP;
								
								strncpy(p_lan_host_entry->ip, abuf, sizeof(p_lan_host_entry->ip) - 1);
								p_lan_host_entry->update_flag |= IP_FLAG;

								if (p_lan_host_entry->conn_type == 0) {
									update_flag |= SYNC_UPDATE_PORT;
									p_lan_host_entry->port = lan_port;
									p_lan_host_entry->port_bind = get_lan_port_bind(lan_port);
								}
								sync_lan_host_info(i, p_lan_host_entry, update_flag);
							}

						/* set on line state */
#if !defined(TCSUPPORT_ANDLINK)
						if ((p_lan_host_entry->conn_type == 0) || (WIFI_ONLINING == p_lan_host_entry->exist))
						{
#endif
								set_lan_host_online(p_lan_host_entry, i, 0);
#if !defined(TCSUPPORT_ANDLINK)
						}
#endif
						}
					}else /*active is online*/
					{
						strncpy(ip, abuf, sizeof(p_lan_host_entry->ip) - 1);
						if(strcasecmp(ip,p_lan_host_entry->ip) != 0)
						{
							update_flag = SYNC_UPDATE_IP; 
							memcpy(p_lan_host_entry->ip, ip, sizeof(p_lan_host_entry->ip) - 1);
							sync_lan_host_info(i, p_lan_host_entry, update_flag);
						}
					}
				}
				else if (1 == ip_type)
				{
					if((p_lan_host_entry->update_flag & IPV6_FLAG) == 0)
					{
						p_lan_host_entry->update_flag |= IPV6_FLAG;
						update_flag = SYNC_UPDATE_IPv6;	
						
						sync_lan_host_info(i, p_lan_host_entry, update_flag);	
					}
				}
				found = 1;
			}
			
			break;
		}
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);

	return found;
}

/* set lanhost entry to offload state and clean lanhost info */
int arp_del_handler(char *lladdr, char *abuf, int ip_type) 
{
	lan_host_info_t *p_lan_host_info = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char mac_addr[32] = {0};
	int i, found = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);
	
	if (lladdr == NULL || abuf == NULL)
		return -1;

	p_lan_host_info = get_lan_host_info();
	if (p_lan_host_info == NULL)
		return -1;

	mac_rm_dot(lladdr, mac_addr);
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &(p_lan_host_info->lan_host_entry[i]);
		if (strcasecmp(mac_addr, p_lan_host_entry->mac) == 0 && strcasecmp(abuf, p_lan_host_entry->ip) == 0) {
			found = 1;
			break;
		}
	}

	/* set to offline state */
	if (found == 1) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>>set offline? no! mac_addr=[%s], abuf=[%s]\n", __FUNCTION__, __LINE__, mac_addr, abuf);
		//set_lan_host_offline(p_lan_host_entry, i);
	}
	
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]exit!\n", __FUNCTION__, __LINE__);
	
	return found;
}

int create_arp_event_sock(int epoll)
{
	struct sockaddr_nl nl_saddr;
	int sock_fd = 0;
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));	
	sock_fd = nlksock_create_2(NETLINK_ROUTE,  getpid(), RTMGRP_NEIGH);
	if (sock_fd < 0) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if (sock_add_epoll(sock_fd, epoll) == -1) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}

