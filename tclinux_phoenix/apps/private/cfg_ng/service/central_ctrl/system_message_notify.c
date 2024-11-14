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

/************************************************************************
*                  I N C L U D E S
*************************************************************************/
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_system.h>
#include "system_message_notify.h"
#include "utility.h"
#ifdef TCSUPPORT_SYSLOG_ENHANCE
#include <syslog.h>
#endif
#include <arpa/inet.h>
#include <svchost_evt.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************/

/************************************************************************
*                  M A C R O S
*************************************************************************/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************/
void ctc_system_snmp_update(struct ecnt_event_data *event_data)
{
	FILE *fp = NULL;
	int i_max = 10;

	/*tcdbg_printf("\n [%s] \n", (char*)event_data);*/

	/* warmstrat trap */
	cfg_set_object_attr(SNMPD_ENTRY_NODE, "coldTrapFlag", "1");
	cfg_evt_write_romfile_to_flash();

	/* check romfile save done. */
	while ( i_max )
	{
		fp = fopen("/tmp/tcapi_save_status", "r");
		if ( NULL == fp )
			break;

		fclose(fp);
		sleep(1);
		i_max --;
	}

	doValPut("/var/tmp/signal_reboot", "1");

	return;
}

void ctc_plugin_info_update(struct ecnt_event_data *event_data)
{
	char tempbuf[120] = {0};

	cfg_get_object_attr(PLUGIN_COMMON_NODE, "PluginName", tempbuf, sizeof(tempbuf));

	return 0;
}

void ctc_web_login_handle(struct ecnt_event_data *event_data)
{	
	struct web_event_data *data = (struct web_event_data*)event_data;
	
#ifdef TCSUPPORT_SYSLOG_ENHANCE
	char log[128] = {0};
	char username[64] = {0};
	char hostIP[64] = {0};
	char buf[64] = {0};
	struct sockaddr_in6 *ipv6addr = NULL;
	struct in_addr *addr = NULL;
	
	memset(username, 0, sizeof(username));
	memset(hostIP, 0, sizeof(hostIP));
	cfg_get_object_attr(ACCOUNT_ENTRY2_NODE, "username", username, sizeof(username));
	if(data)
	{
		if(data->ipver== 6){
			ipv6addr = (struct sockaddr_in6 *)(data->ip);
			inet_ntop(AF_INET6,ipv6addr, buf, INET6_ADDRSTRLEN);
			shrink_ipv6addr(buf, hostIP, sizeof(hostIP));
		}
		else{
			addr = (struct in_addr*)(data->ip);
			inet_ntop(AF_INET, addr, hostIP, sizeof(hostIP));
		}
		
		memset(log, 0, sizeof(log));
		if(data->flag == 1){
			snprintf(log, sizeof(log), "DBUS WEB user <%s> login,IP:%s .\n", username, hostIP);
		}
		else{
			snprintf(log, sizeof(log), "DBUS WEB user <%s> logout.\n", username);
		}
		openlog("TCSysLog DBUS", 0, LOG_LOCAL2);
		syslog(LOG_INFO, log);
		closelog();
	}
	
#endif
	
    	return;
}


/*
0: match or no global addr now
( nlk message sent before iface v6 address is deleted.
v6 address maybe exist in ifconfig. )
other: exist more than 2 global address.
*/
static int check_wan_global_addr_match(char *dev_name, char *wan_v6addr)
{
	char cmd[128];
	FILE *fp = NULL;
	char *buf = NULL, *p_s = NULL;
	char *k1 = "inet6 addr:";
	int size = 0, v6_cnt = 0;

	bzero(cmd, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s > %s", dev_name, TMP_WAN_IF_INFO);
	system(cmd);

	fp = fopen(TMP_WAN_IF_INFO, "r");
	if ( !fp )
		return -1;

	while ( -1 != getline(&buf, &size, fp) )
	{
		if( buf
			&& (p_s = strstr(buf, k1))
			&& strstr(buf, "Scope:Global") )
		{
			if ( NULL == strstr(buf, wan_v6addr) ) 
				v6_cnt ++;
		}
	}
	unlink(TMP_WAN_IF_INFO);

	fclose(fp);
	if ( buf )
		free(buf);

	return v6_cnt;
}


void ctc_ipv6_addr_info_handle(struct ecnt_event_data *event_data)
{
	struct ecnt_ipv6_addrinfo_data *data = (struct ecnt_ipv6_addrinfo_data*)event_data;
	char path[128], n2p_v6_addr[64], cmd[128], node_name[32];
	char svr_list[64];
	int i_pvc = 0, i_entry = 0, i_index = 0;

	if ( strstr(data->dev_name, "nas") )
	{
		sscanf(data->dev_name, "nas%d_%d", &i_pvc, &i_entry);
		i_index = i_pvc * MAX_PVC_ENTRY + i_entry;
	}
	else if ( strstr(data->dev_name, "ppp") )
	{
		sscanf(data->dev_name, "ppp%d", &i_index);
		i_pvc = i_index / MAX_PVC_ENTRY;
		i_entry = i_index % MAX_PVC_ENTRY;
	}
	else
		return;

	if ( i_index < 0 || i_index >= (MAX_PVC_NUM * MAX_PVC_ENTRY) )
		return;

	/* check INTERNET WAN. */
	bzero(node_name, sizeof(node_name));		
	snprintf(node_name, sizeof(node_name), WAN_PVC_ENTRY_NODE, i_pvc + 1, i_entry + 1);
	bzero(svr_list, sizeof(svr_list));
	cfg_get_object_attr(node_name, "ServiceList", svr_list, sizeof(svr_list));

	if ( NULL == strstr(svr_list, "INTERNET") )
		return;

	/* check wan interface global v6 addr match. */
	inet_ntop(AF_INET6, &data->e_s6_addr, n2p_v6_addr, sizeof(n2p_v6_addr));
	if ( 0 == check_wan_global_addr_match(data->dev_name, n2p_v6_addr) )
	{
		/* clear gateway6 */
		bzero(path, sizeof(path));
		snprintf(path, sizeof(path), "/proc/sys/net/ipv6/neigh/%s/default_route", data->dev_name);
		doValPut(path, "\n");

		cfg_type_wan_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_WAN_CONN_LOSTV6, data->dev_name);
	}

	return;
}


