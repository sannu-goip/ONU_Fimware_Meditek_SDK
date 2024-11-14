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

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/epoll.h>
#include <linux/version.h>
#include <linux/sysinfo.h>
#include <fcntl.h>
#include "lan_host_api.h"
#include "lan_host_msg.h"
#include <ecnt_event_global/ecnt_event_lanhost.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#if defined(TCSUPPORT_ANDLINK)
#include <ecnt_event_global/ecnt_event_alink.h>
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#include <common/ecnt_global_macro.h>
#include "blapi_traffic.h"
#endif
#if defined(TCSUPPORT_WLAN)
#include "libapi_lib_wifimgr.h"
#endif
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#else
#define MAX_DUALBAND_BSSID_NUM 8
#endif


lan_host_info_t lan_host_info;
int dhcp_update_flag[MAX_LAN_HOST_NUM];
int lan_port_status[LAN_PORT_NUM] = {0};
unsigned int lan_port_rate[LAN_PORT_NUM] = {0};
int check_flag[MAX_LAN_HOST_NUM];
int lanhost_chk_period = 2;
static pthread_t gLoopChkThread = 0;
char lan_port_duplex[LAN_PORT_NUM][DUPLEX_LEN+1] = {0};

#if defined(TCSUPPORT_ANDLINK)
time_t curTime;
time_t lastTime;
static mac_table_t gMacTable[MAX_DUALBAND_BSSID_NUM];
#endif

lan_host_info_t *get_lan_host_info(void)
{
	return &lan_host_info;
}

int init_lan_port_status(void)
{
	FILE *fp = NULL;
	char string[32] = "";
	int portNum = 0, i = 0;
	
	fp = fopen(ETH_PORT_STS, "r");
	if (fp != NULL) {
		memset(string, 0, sizeof(string));
		fgets(string, sizeof(string), fp);
		portNum = sscanf(string, "%d %d %d %d", lan_port_status, lan_port_status + 1, lan_port_status + 2, lan_port_status + 3);
		fclose(fp);
	}

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> lan port status=[%d %d %d %d]!\n", 
		__FUNCTION__, __LINE__, lan_port_status[0], lan_port_status[1], lan_port_status[2], lan_port_status[3]);


	for (i = 0; i < MAX_LAN_HOST_NUM; i++)
		check_flag[i] = 10;	
	
	return 0;
}

int init_lan_host_entry(lan_host_entry_t *p_lan_host_entry, int entry_index)
{
	char node_name[NODE_NAME_LEN] = {0};
	char buf[CFG_NODE_BUF_LEN] = {0};
	unsigned char uc = 0;
	unsigned int ui = 0;
	unsigned long ul = 0;
	int pl = 0;

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter==>!\n", __FUNCTION__, __LINE__);
	
	if (p_lan_host_entry == NULL) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	snprintf(node_name, sizeof(node_name), "LANHost2_Entry%d", entry_index);

	/* mac */
	memset(buf, 0, sizeof(buf));
	tcapi_get(node_name, "MAC", buf);
	strncpy(p_lan_host_entry->mac, buf, sizeof(p_lan_host_entry->mac) - 1);

	/* host name */
	memset(buf, 0, sizeof(buf));
	tcapi_get(node_name, "HostName", buf);
#if defined(TCSUPPORT_CHARSET_CHANGE) 
	strncpy(p_lan_host_entry->host_name, buf, sizeof(p_lan_host_entry->host_name) - 1);
#else
	basic64_decode(buf, sizeof(buf), p_lan_host_entry->host_name, sizeof(p_lan_host_entry->host_name));
#endif
	/* IP */
	memset(buf, 0, sizeof(buf));
	tcapi_get(node_name, "IP", buf);
	strncpy(p_lan_host_entry->ip, buf, sizeof(p_lan_host_entry->ip) - 1);

	/* connection type */
	memset(buf, 0, sizeof(buf));
	uc = 0;
	tcapi_get(node_name, "ConnectionType", buf);
	uc = atoi(buf);
	p_lan_host_entry->conn_type = uc;

	/* Port */
	memset(buf, 0, sizeof(buf));
	uc = 0;
	tcapi_get(node_name, "Port", buf);
	uc = atoi(buf);
	p_lan_host_entry->port = uc;

	/* Active */
	memset(buf, 0, sizeof(buf));
	uc = 0;
	tcapi_get(node_name, "Active", buf);
	uc = atoi(buf);
	p_lan_host_entry->active = uc;

	/* LatestActiveTime */
	memset(buf, 0, sizeof(buf));
	tcapi_get(node_name, "LatestActiveTime", buf);
	strncpy(p_lan_host_entry->active_time, buf, sizeof(p_lan_host_entry->active_time) - 1);

	/* LatestInActiveTime */
	memset(buf, 0, sizeof(buf));
	tcapi_get(node_name, "LatestInactiveTime", buf);
	strncpy(p_lan_host_entry->inactive_time, buf, sizeof(p_lan_host_entry->inactive_time) - 1);

	/* online time */
	memset(buf, 0, sizeof(buf));
	ui = 0;
	tcapi_get(node_name, "OnlineTime", buf);
	sscanf(buf, "%u", &ui);
	p_lan_host_entry->online_duration = ui;

	/* rxbytes */
	memset(buf, 0, sizeof(buf));
	ul = 0;
	tcapi_get(node_name, "RxBytes", buf);
	sscanf(buf, "%ul", &ul);
	p_lan_host_entry->rxbytes = ul;

	/* txbytes */
	memset(buf, 0, sizeof(buf));
	ul = 0;
	tcapi_get(node_name, "TxBytes", buf);
	sscanf(buf, "%ul", &ul);
	p_lan_host_entry->txbytes = ul;

	/* PowerLevel */
	memset(buf, 0, sizeof(buf));
	pl = 0;
	tcapi_get(node_name, "PowerLevel", buf);
	sscanf(buf, "%d", &pl);
	p_lan_host_entry->power_level = pl;

	/* Port */
	memset(buf, 0, sizeof(buf));
	uc = 0;
	tcapi_get(node_name, "DeviceOnlineNofication", buf);
	uc = atoi(buf);
	p_lan_host_entry->dev_online_notify = uc;

	/* set this entry valid */
	p_lan_host_entry->valid = 1;	

#if defined(TCSUPPORT_CT_JOYME4)
	/* for V6 detect. */
	p_lan_host_entry->v6_detect = 0;
	p_lan_host_entry->detect_cnt = 0;

	/* for OTHER Bridge devices */
	p_lan_host_entry->other_br_dev = 0;
#endif

	set_lan_host_offline(p_lan_host_entry, entry_index, 1);

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
	
	return 0;
	
}

int init_lan_host_info(lan_host_info_t *p_lan_host_info)
{
	char node_name[NODE_NAME_LEN] = {0};
	char mac[DEV_MAC_ADDR_LEN + 1] = {0};
	char buf[] = {0};
	int i = 0, ret = 0, entry_cnt = 0;

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> enter==>!\n", __FUNCTION__, __LINE__);
	
	if (p_lan_host_info == NULL) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	memset(p_lan_host_info, 0, sizeof(lan_host_info_t));
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		snprintf(node_name, sizeof(node_name), "LANHost2_Entry%d", i);
		if (tcapi_get(node_name, "MAC", mac) != 0)
			continue;

		entry_cnt++;
		init_lan_host_entry(&p_lan_host_info->lan_host_entry[i], i);
	}
	
	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit entry_cnt=[%d]==>!\n", __FUNCTION__, __LINE__, entry_cnt);
	
	return 0;
	
}

#if defined(TCSUPPORT_WLAN)
static int wifi_find_by_mactab(mac_table_t *pMacTable, lan_host_entry_t *p_lan_host_entry)
{
	int idx = 0;
	mac_entry_t *pCheckEntry = NULL;
	char new_mac[20] = {0};
	int is_found = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if ( NULL == pMacTable)
		return -1;

	for (idx = 0; idx < pMacTable->Num; idx++)
	{
		pCheckEntry = &(pMacTable->macEntry[idx]);
		if (NULL == pCheckEntry )
			continue;

		snprintf( new_mac, sizeof(new_mac), "%02X%02X%02X%02X%02X%02X", pCheckEntry->Addr[0],\
															pCheckEntry->Addr[1],\
															pCheckEntry->Addr[2],\
															pCheckEntry->Addr[3],\
															pCheckEntry->Addr[4],\
															pCheckEntry->Addr[5] );

//		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> checkmac:[%s]!\n", __FUNCTION__, __LINE__, new_mac);
	
		if (!strcasecmp(p_lan_host_entry->mac, new_mac)) 
		{
			is_found = 1;
			break;
		}
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> [is_found:%d]exit!\n", __FUNCTION__, __LINE__, is_found);
	
	return is_found;
}

int wifi_exsit_check(lan_host_entry_t *p_lan_host_entry)
{
	char nodeName[64] = {0};
	char buf[16] = {0};
	mac_table_t macTab;
	int i = 0;
	int is_found = 0;
	int maxNum = 0;
	char BSSIDNum[4] = {0};

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

#if defined(TCSUPPORT_CT_JOYME4)
	tcapi_get("wlan_common", "BssidNum", BSSIDNum);
	maxNum = atoi(BSSIDNum);
#endif
	/*check whether wifi mac table change*/
	for (i = 0; i < MAX_DUALBAND_BSSID_NUM; i++)
	{
		/* check if this ssid is enable or not */
		if (i < MAX_ECNT_WALN_PORT_NUM) 
			snprintf(nodeName, sizeof(nodeName), "WLan_Entry%d", i);
		else
			snprintf(nodeName, sizeof(nodeName), "WLan11ac_Entry%d", i - MAX_ECNT_WALN_PORT_NUM);
		
		if(tcapi_get(nodeName, "EnableSSID", buf) < 0)
			continue;
		if (!strcmp(buf, "0"))
			continue;

		/*now we only support 4 ssid*/
		memset(&macTab, 0, sizeof(macTab));
		getMacEntryByIndex(&macTab, i,maxNum);
		if (1 == wifi_find_by_mactab(&macTab, p_lan_host_entry)){
			is_found = 1;
			break;
		}
	}
	
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> [is_found:%d]exit!\n", __FUNCTION__, __LINE__, is_found);

	return is_found;
}
#endif

#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
/*counter the time*/
double getTimeRate(struct timeval curTime,struct timeval lastTime){
	double rate = 0;
	double delay = 0;		/*usec*/

	if(curTime.tv_usec > lastTime.tv_usec){
		delay = (curTime.tv_sec-lastTime.tv_sec) + (double)(curTime.tv_usec-lastTime.tv_usec)/1000000;
	}else{
		delay = (curTime.tv_sec - 1 -lastTime.tv_sec) + (double)(curTime.tv_usec + 1000000 -lastTime.tv_usec)/1000000;
	}

	rate = 1/delay;

	return rate;
}
#endif

#if defined(TCSUPPORT_ANDLINK)
static int notifyWLanHostMacs(mac_table_t *pcheckMacTable, int ssid_index)
{
	int idx = 0, dstidx = 0;
	mac_entry_t *pCheckEntry = NULL, *pDstEntry = NULL;
	int macTableChange = 0;
	int rssi_change = 0;
	static int update_rssi_cnt[MAX_DUALBAND_BSSID_NUM] = {0};
	char new_mac[20] = {0};
	lan_host_entry_t *p_lan_host_entry = NULL;
	int lan_host_entry = -1;
	unsigned int update_flag = 0;
	char nodeName[64] = {0};
	int isMark[MAX_LEN_OF_MAC_TABLE] = {0};
	int is_found = 0;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if ( NULL == pcheckMacTable)
		return -1;

	memset(isMark, 0, sizeof(isMark));
	for (idx = 0; idx < pcheckMacTable->Num; idx++)
	{
		is_found = 0;
		pCheckEntry = &(pcheckMacTable->macEntry[idx]);
		if (NULL == pCheckEntry )
			continue;

		snprintf( new_mac, sizeof(new_mac), "%02X%02X%02X%02X%02X%02X", pCheckEntry->Addr[0],\
															pCheckEntry->Addr[1],\
															pCheckEntry->Addr[2],\
															pCheckEntry->Addr[3],\
															pCheckEntry->Addr[4],\
															pCheckEntry->Addr[5] );
		
		p_lan_host_entry = find_lan_host_entry_by_mac(new_mac, &lan_host_entry);

		if (p_lan_host_entry) 
		{
			if(pCheckEntry->AvgRssi0 != p_lan_host_entry->AvgRssi0 || pCheckEntry->AvgRssi1 != p_lan_host_entry->AvgRssi1)
			{
				lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> wifimactable is changed\n", __FUNCTION__, __LINE__);
				/*update wifimactable info*/
				p_lan_host_entry->AvgRssi0 = pCheckEntry->AvgRssi0;
				p_lan_host_entry->AvgRssi1 = pCheckEntry->AvgRssi1;

				/*update the node of lanhost2*/
				p_lan_host_entry->power_level = (pCheckEntry->AvgRssi0 + pCheckEntry->AvgRssi1)/2;
				p_lan_host_entry->negorate = pCheckEntry->DataRate * 1000; /* kbps */
				if (p_lan_host_entry->port != ssid_index + 1) 
				{
					update_flag = SYNC_UPDATE_PORT;
					p_lan_host_entry->port = ssid_index + 1;
				}
				
				if (ssid_index < MAX_ECNT_WALN_PORT_NUM) 
					snprintf(nodeName, sizeof(nodeName), "WLan_Entry%d", ssid_index);
				else
					snprintf(nodeName, sizeof(nodeName), "WLan11ac_Entry%d", ssid_index - MAX_ECNT_WALN_PORT_NUM);
			
				tcapi_get(nodeName, "SSID", p_lan_host_entry->ssid);
			
				update_flag |= (SYNC_UPDATE_POWER_LEVEL | SYNC_UPDATE_NEGORATE);
				sync_lan_host_info(lan_host_entry, p_lan_host_entry, update_flag);
			}
		}
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
	
	return 0;
}

void wifiMacTab_update(void)
{
	char nodeName[64] = {0};
	char buf[16] = {0};
	mac_table_t macTab;
	int macTableChg = 0;
	int i = 0, notify_flag = 0;
	int maxNum = 0;
	char BSSIDNum[4] = {0};

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	tcapi_get("wlan_common", "BssidNum", BSSIDNum);
	maxNum = atoi(BSSIDNum);

	/*check whether wifi mac table change*/
	for (i = 0; i < MAX_DUALBAND_BSSID_NUM; i++)
	{
		/* check if this ssid is enable or not */
		if (i < MAX_ECNT_WALN_PORT_NUM) 
			snprintf(nodeName, sizeof(nodeName), "WLan_Entry%d", i);
		else
			snprintf(nodeName, sizeof(nodeName), "WLan11ac_Entry%d", i - MAX_ECNT_WALN_PORT_NUM);
		
		if(tcapi_get(nodeName, "EnableSSID", buf) < 0)
			continue;
		if (!strcmp(buf, "0"))
			continue;

		/*now we only support 4 ssid*/
		memset(&macTab, 0, sizeof(macTab));
		getMacEntryByIndex(&macTab, i,maxNum);

		notifyWLanHostMacs(&macTab, i);
	}

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);

	return ;
}
#endif

/* update lan host info, include info as below:
	1) online time;
	2) txbytes/rxbytes;
	3) check delete lan host entry if offline too long;
	4) check it ther new mac add into mac table or old mac disappear from mac table;
	    a) if there is new mac, should set it online;
	    b) if there is old mac disappear, should set it to offline;
*/
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
int check_lan_host_update(lan_host_info_t *p_lan_host_info , struct timeval curTime, struct timeval lastTime)
#else
int check_lan_host_update(lan_host_info_t *p_lan_host_info)
#endif
{
	lan_host_entry_t *p_lan_host_entry = NULL;
	FILE *fp = NULL;
	char buf[128] = {0}, mac[32] = {0};
	int port = 0, i = 0, empty_index = -1;
	unsigned long long rxbytes = 0, txbytes = 0;
	struct sysinfo info;
	long uptime;
	int sum = 0;
	unsigned int update_flag = 0;
	int lan_port = 0, ret = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	char lanhost2_node[32] = {0}, ip6buf[64] = {0}, lladdr[64] = {0};
	int ipv6_detect_cnt = 0;
#endif

#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
	char strAjustRate[32] = {0};
	double ajustRate = LANHOST_AJUST_RETE;
	double timeRate = getTimeRate(curTime, lastTime);

	/*for ajust rate*/
	ret = tcapi_get("Sys_Entry", "ajustRate", strAjustRate);
	if(ret == 0 && strlen(strAjustRate)!=0){
		sscanf(strAjustRate, "%f", &ajustRate);
	}
#endif

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if (p_lan_host_info == NULL)
		return -1;
	
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
	time(&curTime);	
#if !defined(TCSUPPORT_CT_UBUS)
	wifiMacTab_update();
#endif
#endif
	
		/*clear exist flag, will set later*/
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if (1 == p_lan_host_entry->exist){
			p_lan_host_entry->exist = 0;
		}			
	}
	
	fp = fopen("/proc/br_fdb_host/stb_list", "r");
	if (NULL == fp) {
		lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	bzero(buf, sizeof(buf));
	while (fgets(buf, sizeof(buf), fp))
	{
		sscanf(buf, "%d=%s", &port, mac);
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>>port=[%d], mac=[%s]!\n", __FUNCTION__, __LINE__, port, mac);*/
		if (check_mac_exist(p_lan_host_info, mac, &empty_index) == 0
#if defined(TCSUPPORT_CT_JOYME4)
			/* ip must exist, we will add new host and set online state */
			&& ( check_dhcpd_lease_exist(mac) || check_arp_exist(mac) )
#endif
		) {
#if !defined(TCSUPPORT_CT_CONGW_CH) 
			if (port >= 1 && port <= 4)
#endif
			{
				/* add new entry and set online state */
				if (empty_index != -1) {
					p_lan_host_entry = &p_lan_host_info->lan_host_entry[empty_index];
					add_lan_host(p_lan_host_entry, mac, empty_index, 0, port);
				}
			}
		} 		
	}
	fclose(fp);

	/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>>enter!\n", __FUNCTION__, __LINE__);*/
	
	for (i = 0; i < MAX_LAN_HOST_NUM; i++) {
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];		
		/* update online time and cnt */
		if (p_lan_host_entry->valid == 1 && p_lan_host_entry->active == 1) {
			if (get_lan_host_cnt_by_mac(p_lan_host_entry->mac, &rxbytes, &txbytes) == 0) {
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
				p_lan_host_entry->upspeed = timeRate*ajustRate*(txbytes - p_lan_host_entry->txbytes)*8/1024;	
				p_lan_host_entry->downspeed = timeRate*ajustRate*(rxbytes - p_lan_host_entry->rxbytes)*8/1024;
#endif
				p_lan_host_entry->rxbytes = rxbytes;
				p_lan_host_entry->txbytes = txbytes;
			}

			sysinfo(&info);
    		uptime = info.uptime;
			p_lan_host_entry->online_duration = uptime - p_lan_host_entry->online_time;

			update_flag = (SYNC_UPDATE_CNT | SYNC_UPDATE_ONLINE_TIME);
			sync_lan_host_info(i, p_lan_host_entry, update_flag);

#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
#if defined(TCSUPPORT_ANDLINK)
			p_lan_host_entry->sleeptime = curTime - lastTime; 	
#else
			p_lan_host_entry->sleeptime = curTime.tv_sec - lastTime.tv_sec;			
#endif		
			p_lan_host_entry->Lastrxbytes = rxbytes;
			p_lan_host_entry->Lasttxbytes = txbytes;
#endif
		}
		else if (check_flag[i] > 0) {
#if defined(TCSUPPORT_CT_JOYME4)
			/* set host online after arp idle completed */
			if ( check_ipv6_device_inform(p_lan_host_entry) )
#endif
			{
				lanhost_printf(DBG_MSG_INFO, "[%s][%d]>>check ip here!\n", __FUNCTION__, __LINE__);
				if (p_lan_host_entry->valid == 1 && p_lan_host_entry->conn_type == 0) {
					lan_port = get_lan_port_by_mac(p_lan_host_entry->mac);
#if !defined(TCSUPPORT_CT_CONGW_CH) 
					if (lan_port >= 1 && lan_port <= 4)
#endif
					{
						if ((lan_port_status[lan_port - 1] == 1) && ((set_lan_host_entry(p_lan_host_entry, i) == 0) || (p_lan_host_entry->port_bind != 0)
#if defined(TCSUPPORT_CT_JOYME4)
							|| check_ipv6_device_inform(p_lan_host_entry)
#endif
							)) {
							if ( lan_port != p_lan_host_entry->port )
							{
								update_flag = SYNC_UPDATE_PORT;
								p_lan_host_entry->port = lan_port;
								p_lan_host_entry->port_bind = get_lan_port_bind(port);
								sync_lan_host_info(i, p_lan_host_entry, update_flag);
							}
							set_lan_host_online(p_lan_host_entry, i, 1);
						
							check_flag[i] = 0;
						}
					}
				}
#if defined(TCSUPPORT_WLAN)
#if defined(TCSUPPORT_CT_JOYME4)
				/* only support ipv6 only device */
				else if ( p_lan_host_entry->valid == 1 
					&& wifi_exsit_check(p_lan_host_entry) 
					&& IPV6_ONLY_MODE == p_lan_host_entry->ipmode 
					&& (p_lan_host_entry->update_flag & IPV6_FLAG) )
				{
					update_flag = SYNC_UPDATE_IPv6;
					sync_lan_host_info(i, p_lan_host_entry, update_flag);
					set_lan_host_online(p_lan_host_entry, i, 0);
					check_flag[i] = 0;
				}
#endif
#endif
				check_flag[i]--;
			}
		}
		
		/* set offline state */
		if (p_lan_host_entry->valid == 1 && 
			p_lan_host_entry->active == 1 &&
			p_lan_host_entry->exist == 0) {		
#if defined(TCSUPPORT_WLAN)			
			if (0 == p_lan_host_entry->conn_type || 0 == wifi_exsit_check(p_lan_host_entry))
#else
			if (0 == p_lan_host_entry->conn_type)
#endif
			{
				lanhost_printf(DBG_MSG_INFO, "[%s][%d]>>conn_type[%d] set mac:%s offline !\n", 
					__FUNCTION__, __LINE__, p_lan_host_entry->conn_type, p_lan_host_entry->mac);
				set_lan_host_offline(p_lan_host_entry, i, 0);
			}
		}

#if defined(TCSUPPORT_ANDLINK)
		if(p_lan_host_entry->valid == 1 && p_lan_host_entry->active == 0)
		{
			/* need check arp table again */
			if((p_lan_host_entry->conn_type == 0) || (p_lan_host_entry->wlanonline == 1) || (p_lan_host_entry->wlanonline == 2))
			{
				if ( 0 == alink_set_lan_host_entry(p_lan_host_entry, i) )
				{
					lan_port = get_lan_port_by_mac(p_lan_host_entry->mac);
					if(p_lan_host_entry->exist == 1)
						set_lan_host_online(p_lan_host_entry, i, 1);
				}
			}
		}
#endif					

		/* check if offline too long, delete it ? */
		if (p_lan_host_entry->valid == 1 && p_lan_host_entry->active == 0) {
			sysinfo(&info);
    		uptime = info.uptime;
			if (uptime - p_lan_host_entry->offline_time > OFFLINE_TIME) {
				del_lan_host(p_lan_host_entry, i);
			}
		} 

#if defined(TCSUPPORT_CT_JOYME4)
		if ( ONLINE == p_lan_host_entry->active )
		{
			/* check global IPv6 first. */
			snprintf(lanhost2_node, sizeof(lanhost2_node), "LANHost2_Entry%d", i);
			if ( 0 != tcapi_get(lanhost2_node, "g_IPv6_1", ip6buf) )
				ip6buf[0] = 0;
			
			if ( 0 == ip6buf[0] )
			{
				/* V6 address Lost, re-detect it */
				if ( DETECT_DONE == p_lan_host_entry->v6_detect )
				{
					p_lan_host_entry->v6_detect = 0;
				}
			}
			else
			{
				/* no need check when GOT it. */
				p_lan_host_entry->v6_detect = DETECT_DONE;
				p_lan_host_entry->detect_cnt = 0;
			}

			if ( DETECT_DONE != p_lan_host_entry->v6_detect
				&& p_lan_host_entry->detect_cnt < MAX_DETECT_CNT ) /* max fail times:  300s. */
			{
				if ( LOOP_END == ++p_lan_host_entry->v6_detect ) /* wait 5 period times->10 seconds. */
				{
					p_lan_host_entry->v6_detect = 0; /* clear it, wait for next period. */
					tcapi_get(lanhost2_node, "l_IPv6", ip6buf);
					detect_slaac_address(p_lan_host_entry, i, ip6buf, 0, NULL, 0);
				}
			}

			/* check other bridge lan dev state */
			if ( p_lan_host_entry->other_br_dev )
			{
				sysinfo(&info);
				if ( info.uptime - p_lan_host_entry->last_active_sec >= 60 )
				{
					lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> OB DEV [%s] OFFLINE! \n", __FUNCTION__, __LINE__, p_lan_host_entry->mac);
					set_lan_host_offline(p_lan_host_entry, i, 0);
				}
			}

			/* check hostname update */
			if ( ( 0 == ( p_lan_host_entry->update_flag & HOST_NAME_FLAG ) ) 
				&& 0 != p_lan_host_entry->mac[0] 
				&& ( p_lan_host_entry->update_flag & IP_FLAG ) )
			{
				set_hostname_from_dhcp(p_lan_host_entry, i);
				p_lan_host_entry->update_flag |= HOST_NAME_FLAG;
			}
		}

		if ( p_lan_host_entry->update_flag & IPV6_IDLE_FLAG )
		{
			ipv6_detect_cnt = GET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag);
			if ( ipv6_detect_cnt > 0 )
			{
				ipv6_detect_cnt--;
				SET_IPV6_IDLE_CNT(p_lan_host_entry->update_flag, ipv6_detect_cnt);
			}
		}
#endif
	}

#if defined(TCSUPPORT_CT_JOYME4)
	/* pop out ipv6 queue */
	popout_ipv6_queue();
#endif

	/* save to flash */
	if (save_flag == 1) {
		/*lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);*/
		save_flag = 0;
		ret = ecnt_event_send(ECNT_EVENT_LANHOST,
						ECNT_EVENT_LANHOST_UPDATE,
						NULL,
						0);
#if defined(TCSUPPORT_ANDLINK)
		tcapi_save();
#else
		tcapi_save_raw();
#endif					
	}

#if defined(TCSUPPORT_ANDLINK)
	lastTime = curTime; 		
#endif					

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> exit!\n", __FUNCTION__, __LINE__);
	
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME4)
int get_default_wan_index(int *wan_index)
{
	char def_idx_buf[32] = {0};
	int def_idx = 0;

	if (wan_index == NULL)
		return -1;

	if (0 == tcapi_get("WanInfo_Common", "DefRouteIndexv4", def_idx_buf) && 
		0 != strcmp(def_idx_buf, "N/A")) {
		def_idx = atoi(def_idx_buf);
		
		*wan_index = def_idx;
		return 0;
	}

	if (0 == tcapi_get("WanInfo_Common", "DefRouteIndexv6", def_idx_buf) && 
		0 != strcmp(def_idx_buf, "N/A")) {
		def_idx = atoi(def_idx_buf);
		
		*wan_index = def_idx;
		return 0;
	}

	return -1;
}

int update_wan_bytes()
{
	char node_name[32] = {0}, txbytes_buf[32] = {0}, alltx_bytes_buf[32] = {0},
		rxbytes_buf[32] = {0}, allrx_bytes_buf[32] = {0};
	int wan_index = 0, wan_pvc = 0, wan_entry = 0;
	unsigned long long tx_bytes = 0, rx_bytes = 0;
	unsigned long long wan_txbytes = 0, wan_rxbytes = 0;
	wan_acnt_t ppeInfo;

	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> enter!\n", __FUNCTION__, __LINE__);

	if (get_default_wan_index(&wan_index) == -1)
		return -1;
	
	wan_pvc = wan_index / MAX_SMUX_NUM;
	wan_entry = wan_index % MAX_SMUX_NUM;

	/* cpu path count */
	snprintf(node_name, sizeof(node_name), "Info_nas%d.%d", wan_pvc, wan_entry);
	tcapi_get(node_name, "txbytes", txbytes_buf);
	sscanf(txbytes_buf, "%llu", &tx_bytes);
	wan_txbytes = tx_bytes;

	tcapi_get(node_name, "rxbytes", rxbytes_buf);
	sscanf(rxbytes_buf, "%llu", &rx_bytes);
	wan_rxbytes = rx_bytes;

	/* hardward path count */
	bzero(&ppeInfo, sizeof(ppeInfo));
	if ( 0 == blapi_traffic_get_wan_acnt(wan_index, &ppeInfo) )
	{
		wan_txbytes += ppeInfo.uniTxBytes;
		wan_txbytes += ppeInfo.mulTxBytes;

		wan_rxbytes += ppeInfo.uniRxBytes;
		wan_rxbytes += ppeInfo.mulRxBytes;
	}
	
	lanhost_printf(DBG_MSG_INFO, "[%s][%d]>> rxbytes=%llu, txbytes=%llu\n", 
				__FUNCTION__, __LINE__, wan_rxbytes, wan_txbytes);
		
	snprintf(txbytes_buf, sizeof(txbytes_buf), "%llu", wan_txbytes);
	snprintf(rxbytes_buf, sizeof(rxbytes_buf), "%llu", wan_rxbytes);

	snprintf(node_name, sizeof(node_name), "WanInfo_Entry%d", wan_index);
	tcapi_set(node_name, "TxBytes", txbytes_buf);
	tcapi_set(node_name, "RxBytes", rxbytes_buf);

	lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);

	return 0;
}

int check_OB_lan_host_update(lan_host_info_t *p_lan_host_info)
{
#define BRIDGE_UPDATE_PERIOD_TIME (5)
	static int period_sec = 0, flag = 0;
	int other_brwan_num = 0, wan_index[MAX_WAN_IF_INDEX] = {0};
	int i = 0;

	int devCnt = 0, data_len = 0, idx = 0;
	blapi_info_ioctl_OB_map_data data;
	blapi_ob_monitor_data *list = NULL;
	lan_host_entry_t *p_lan_host_entry = NULL;
	char devmac[20] = {0};
	char ipaddr[64] = {0};
	unsigned char port = 0, conn_type = 0, ip_type = D_TYPE_IP_IP6;
	unsigned int update_flag = 0;
	int lan_host_entry = -1;
	long uptime = 0;
	struct sysinfo info;
	struct in_addr in_s_v4addr = {0};
	struct in6_addr in_s_v6addr = {0};
	char ob_wanindex[256] = {0};

	/* check other bridge lan device every period */
	if ( ++period_sec < BRIDGE_UPDATE_PERIOD_TIME )
		return 0;
	period_sec = 0;

	/* get other bridge wan cnt and wan index */
	bzero(ob_wanindex, sizeof(ob_wanindex));
	tcapi_get("WanInfo_Common", "OB_WanIndex", ob_wanindex);
	if ( 0 == ob_wanindex[0] )
		return 0;
	other_brwan_num = parse_OB_WanIndex(ob_wanindex, wan_index, ",");
	if ( other_brwan_num < 1 )
		return 0;

	/* foreach other bridge wan, get and filter lan dev info */
	for ( i = 0; i < other_brwan_num; i++ )
	{
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OTHER WAN INDEX=[%d]\n", __FUNCTION__, __LINE__, wan_index[i]);
		/* get device info. */
		/*
			1st GET COUNT.
		*/
		bzero(&data, sizeof(data));
		data_len = sizeof(devCnt);
		data.wan_idx = wan_index[i];
		data.data_len = data_len;
		data.data = &devCnt;
		blapi_traffic_ob_info_get_cnt(&data);

		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OB DEV devCnt=[%d]\n", __FUNCTION__, __LINE__, devCnt);
		if ( devCnt )
		{
			/* 2nd GET IP list */
			data_len = sizeof(blapi_ob_monitor_data) * devCnt;
			list = malloc(data_len);
			if ( NULL == list )
				return;

			bzero(list, data_len);
			bzero(&data, sizeof(data));
			data.wan_idx = wan_index[i];
			data.data_len = data_len;
			data.data = list;
			blapi_traffic_ob_info_get_data(&data);
			
			for ( idx = 0; idx < devCnt; idx ++ )
			{
#if defined(TCSUPPORT_MULTI_USER_ITF)
				port = GET_LAN_ITF_MARK(list[idx].mark);
#else
				port = (list[idx].mark & 0xf0000000) >> 28;
#endif
				bzero(devmac, sizeof(devmac));
				snprintf(devmac, sizeof(devmac), "%02x%02x%02x%02x%02x%02x", list[idx].mac[0], list[idx].mac[1], list[idx].mac[2], list[idx].mac[3], list[idx].mac[4], list[idx].mac[5]);
				bzero(ipaddr, sizeof(ipaddr));
				snprintf(ipaddr, sizeof(ipaddr), "%s", list[idx].ipaddr);
				ip_type = list[idx].type;
				lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OB DEV port=[%d], ip=[%s], mac=[%s], type=[%s]\n", 
						__FUNCTION__, __LINE__, port, ipaddr, devmac, D_TYPE_PPP == ip_type ? "PPP" : "DHCP");

				/* 3rd GET & FILTER DEV INFO */
				if ( NULL != ( p_lan_host_entry = find_lan_host_OB_entry_by_mac(1, devmac, &lan_host_entry) ) 
					|| NULL != ( p_lan_host_entry = find_lan_host_OB_entry_by_mac(0, devmac, &lan_host_entry) ) )
				{
					/* skip loop when this lan port down or wifi sta down */
					if ( p_lan_host_entry->lock_lost )
						continue;

					/* set this lan dev as other bridge lan dev */
					p_lan_host_entry->other_br_dev = 1;
					
					sysinfo(&info);
					lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OB DEV active=[%d], system uptime=[%d], last_active_sec=[%d]\n", 
							__FUNCTION__, __LINE__, p_lan_host_entry->active, info.uptime, p_lan_host_entry->last_active_sec);

					/* set online */
					if ( OFFLINE == p_lan_host_entry->active )
					{
						set_lan_host_online(p_lan_host_entry, lan_host_entry, 0);
						p_lan_host_entry->active = ONLINE;
						lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OB DEV [%s] [ONLINE]\n", __FUNCTION__, __LINE__, devmac);
					}

					/* check and update port, ip, ip6, last active time */
					if ( p_lan_host_entry->port != port ) 
					{
						update_flag |= SYNC_UPDATE_PORT;
						if (port >= 1 && port <= 4)
						{
							p_lan_host_entry->conn_type = 0;
							p_lan_host_entry->port = port;
						}
						else
						{
							p_lan_host_entry->conn_type = 1;
							p_lan_host_entry->port = port - MAX_ECNT_ETHER_PORT_NUM;
						}

						update_flag |= SYNC_UPDATE_CONNTYPE;
					}

					bzero(&in_s_v6addr, sizeof(in_s_v6addr));
					bzero(&in_s_v4addr, sizeof(in_s_v4addr));
					/* 1. IPv6 addr ? */
					if ( 1 == inet_pton(AF_INET6, ipaddr, &in_s_v6addr) )
					{
						bzero(&ipaddr, sizeof(ipaddr));
						inet_ntop(AF_INET6, &in_s_v6addr.s6_addr, ipaddr, sizeof(ipaddr));
						lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> shrink ipv6 addr=[%s]\n", __FUNCTION__, __LINE__, ipaddr);
						/* ignore zero ipv6 addr */
						if ( 0 != ipaddr[0] )
						{
							update_flag |= SYNC_UPDATE_IPv6;
							strncpy(p_lan_host_entry->ipv6, ipaddr, sizeof(p_lan_host_entry->ipv6));
							p_lan_host_entry->update_flag |= IPV6_FLAG;
						}
					}
					/* 2. IPv4 addr ? */
					else if ( 1 == inet_pton(AF_INET, ipaddr, &in_s_v4addr) )
					{
						lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> curr ip_type=[%s], old ip_type=[%s]\n", 
							__FUNCTION__, __LINE__, ip_type == D_TYPE_PPP ? "PPP" : "DHCP", p_lan_host_entry->ip_type == D_TYPE_PPP ? "PPP" : "DHCP");
						/* record ppp ip first. update dhcp ip when ppp ip had lost up to 3 times */
						if ( D_TYPE_PPP != ip_type && D_TYPE_PPP == p_lan_host_entry->ip_type  )
						{
							if ( ++p_lan_host_entry->ppp_lost_cnt >= 3 )
							{
								update_flag |= SYNC_UPDATE_IP;
								strncpy(p_lan_host_entry->ip, ipaddr, sizeof(p_lan_host_entry->ip));
								p_lan_host_entry->ip_type = ip_type;
								p_lan_host_entry->ppp_lost_cnt = 0;
								p_lan_host_entry->update_flag |= IP_FLAG;
							}
						}
						else
						{
							/* IP is PPP, or Type is not changed, check & update directly */
							p_lan_host_entry->ppp_lost_cnt = 0;
							if ( 0 != strcmp(p_lan_host_entry->ip, ipaddr) )
							{
								update_flag |= SYNC_UPDATE_IP;
								strncpy(p_lan_host_entry->ip, ipaddr, sizeof(p_lan_host_entry->ip));
								p_lan_host_entry->ip_type = ip_type;
								p_lan_host_entry->update_flag |= IP_FLAG;
							}
						}
					}

					/* record last active sec for dev aging cycle */
					p_lan_host_entry->last_active_sec = info.uptime;
					p_lan_host_entry->online_duration = p_lan_host_entry->last_active_sec - p_lan_host_entry->online_time;
					update_flag |= ( SYNC_UPDATE_STATE | SYNC_UPDATE_ONLINE_TIME );
					sync_lan_host_info(lan_host_entry, p_lan_host_entry, update_flag);				
				}
				else 
				{
					/* add new lan dev */
					if ( port >= 1 && port <= 4 )
						conn_type = 0;
					else
						conn_type = 1;
					p_lan_host_entry = find_empty_lan_host_entry(&lan_host_entry);
					if ( p_lan_host_entry )
					{
						p_lan_host_entry->other_br_dev = 1;
						add_lan_host(p_lan_host_entry, devmac, lan_host_entry, conn_type, port);
					}
				}
			}

			free(list);
			list = NULL;
		}
	}

	/* reduce lock lost for lan port down and wifi sta down */
	for ( i = 0; i < MAX_LAN_HOST_NUM; i++ )
	{
		p_lan_host_entry = &p_lan_host_info->lan_host_entry[i];
		if ( p_lan_host_entry->lock_lost )
		{
			p_lan_host_entry->lock_lost -= 1;
			if ( p_lan_host_entry->other_br_dev && ONLINE == p_lan_host_entry->active )
			{
				lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> OB DEV [%s] [OFFLINE] in lock lost!\n", __FUNCTION__, __LINE__, p_lan_host_entry->mac);
				set_lan_host_offline(p_lan_host_entry, lan_host_entry, 0);
			}
		}
	}
	return 0;
}
#endif

void *start_loop_checkupdate()
{
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
	struct timeval curTime;
	struct timeval lastTime;

	memset(&lastTime, 0, sizeof(struct timeval));
#endif
	while(1)
	{
#if defined(TCSUPPORT_CUC) || defined(TCSUPPORT_CT_UBUS)
		gettimeofday(&curTime, NULL);
		check_lan_host_update(&lan_host_info, curTime, lastTime);
		lastTime = curTime;
#else
		check_lan_host_update(&lan_host_info);
#if defined(TCSUPPORT_CT_JOYME4)
		update_wan_bytes();

		check_OB_lan_host_update(&lan_host_info);
#endif
#endif
		sleep(lanhost_chk_period);
	}
}

/* for starteupgrade task. */
int start_update_thread()
{
	pthread_attr_t thread_attr;
	int ret = 0;
	
	/* create thread */
	ret = pthread_attr_init(&thread_attr);
	if ( 0 != ret )
	{
		lanhost_printf(DBG_MSG_ERR, "%s pthread_attr_init fail.\n", __FUNCTION__);
		return -1;
	}

	ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	if ( 0 != ret )
	{
		lanhost_printf(DBG_MSG_ERR, "%s pthread_attr_setdetachstate fail.\n", __FUNCTION__);
		return -1;
	}
	
	ret = pthread_create(&gLoopChkThread,
						&thread_attr,
						(void *)start_loop_checkupdate,
						NULL);
	if ( 0 != ret )
	{
		lanhost_printf(DBG_MSG_ERR, "%s pthread_create fail.\n", __FUNCTION__);
		return -1;
	}

	pthread_attr_destroy(&thread_attr);

	return 0;
}

int main (int argc, char* argv[])
{
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
	struct sockaddr_nl nl_daddr;
	struct epoll_event epoll_ev[4];
	char buffer[NLMSG_SPACE(MAX_MSGSIZE)] = {0};
	char period[10] = {0};
	int ret = 0, ecnt_event_sock =0, arp_event_sock = 0, fd_epoll =0, fd_ret = 0, i = 0;

	/* get lanhost check period */
	if (0 == tcapi_get("Sys_Entry", "lanhost_chk_period", period)
		&& atoi(period) > 0 && atoi(period) <= 20)
		lanhost_chk_period = atoi(period);

#if defined(TCSUPPORT_CT_JOYME4)
	/* init ipv6 detect queue */
	if ( 0 != init_ipv6_detect_queue() )
		return -1;
#endif

	/* step 0: init lan port status */
	init_lan_port_status();

	/* step 1: init lan_host_info from cfg node */
	if (init_lan_host_info(&lan_host_info) == -1) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);		
		return -1;
	}

	/* step 2: init msg socket */
	if ((fd_epoll = epoll_create(6)) < 0) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ecnt_event_sock = create_ecnt_event_sock(fd_epoll);
	if (ecnt_event_sock == -1) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	arp_event_sock = create_arp_event_sock(fd_epoll);
	if (arp_event_sock == -1) {
		lanhost_printf(DBG_MSG_INFO, "\n[%s][%d]>> exit==>!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	nlh = (struct nlmsghdr *)buffer;
	memset(nlh, 0, sizeof(buffer));	
	/*iov structure */
	iov.iov_base = (void *)nlh;
	iov.iov_len = sizeof(buffer);
	/* msg */
	memset(&msg, 0, sizeof(msg));
	memset(&nl_daddr, 0, sizeof(struct sockaddr_nl));
	msg.msg_name = (void *)&nl_daddr;
	msg.msg_namelen = sizeof(nl_daddr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	/* step 3: start msg loop and update lan host info period */
	tcapi_set("wifiMacTab_Common", "lanhostdone", "1");
	/* clean arp for init online device */
	system("/usr/bin/ip neigh flush dev br0 2>/dev/null");
	/* create thred check_lan_host_update*/
	start_update_thread();
	do {
		fd_ret = epoll_wait(fd_epoll, epoll_ev, 4, 5000);
		if (fd_ret <= 0) {
			continue;
		}
		for (i = 0; i < fd_ret; ++i) {
			if (epoll_ev[i].data.fd == ecnt_event_sock) {
				ecnt_event_handler(ecnt_event_sock, &msg, nlh);
			}
			else if (epoll_ev[i].data.fd == arp_event_sock) {
				arp_event_handler(arp_event_sock, &msg, nlh);
			}
		}		
	} while (1);

#if defined(TCSUPPORT_CT_JOYME4)
	destroy_ipv6_detect_queue();
#endif

	return 0;
}

