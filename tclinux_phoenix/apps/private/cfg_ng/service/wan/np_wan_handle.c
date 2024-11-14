
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
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_common.h"
#include "np_wan_handle.h"
#include "ecnt_event_global/ecnt_event_global.h"
#include "ecnt_event_global/ecnt_event_alink.h"
#include "utility.h"
#include "libecntevent.h"

/**********************************************************************/
/**********************************************************************/
/* global definitions */
struct timespec g_pretime, g_pretime_ethdown;
int g_switch_mode = NP_WAN_AP_NONE, g_ap_status = NP_WAIT_TIMEOUT;

void getPreTime()
{
	memset(&g_pretime, 0, sizeof(g_pretime));
	clock_gettime(CLOCK_MONOTONIC, &g_pretime);
	return;
}

void getPreTimeEthDown()
{
	memset(&g_pretime_ethdown, 0, sizeof(g_pretime_ethdown));
	clock_gettime(CLOCK_MONOTONIC, &g_pretime_ethdown);
	return;
}

int getSwitchMode()
{
	return g_switch_mode;
}

int setSwitchMode(int mode)
{
	g_switch_mode = mode;
	return 0;
}

int getAPStatus()
{
	return g_ap_status;
}

int setAPStatus(int status)
{
	g_ap_status = status;
	return 0;
}

/* this array has the same order as the type _np_wan_status_ */
static const char *ap_wan_status[] = {
  "WAIT_TIMEOUT", "WAIT WAN UP", "BR WAN GET IP", "BR WAN GET IP TIMEOUT", "BR WAN LOST IP", "IF3 REQ", 
  "IF3 REQ TIMEOUT", "WAIT APCLIENT UP", "APCLIENT UP", 
};

void np_wan_send_evt(int evt_id, int ap_status)
{
	wan_evt_t wan_evt;
	memset(&wan_evt, 0, sizeof(wan_evt));

	switch (ap_status)
	{
		case NP_WAIT_TIMEOUT:
		case NP_BR_ALINK_IP_GET:
		case NP_BR_ALINK_IP_LOST:
		case NP_BR_ALINK_IP_GET_TIMEOUT:
		case NP_FOUND_GATEWAY_REQ:
		case NP_APCLI_UP:
		{
			snprintf(wan_evt.buf, sizeof(wan_evt.buf), "%d", ap_status);
			break;
		}
		default:
		{
			return;
		}
	}

	cfg_send_event(EVT_WAN_INTERNAL, evt_id, (void*)&wan_evt, sizeof(wan_evt));

	return;
}

int np_wan_pre_handle_xpon_update(int evt, char *source)
{
	char fixmode[16] = {0}, curmode[16] = {0};

	if ( NULL == source )
		return 0;

#if defined(TCSUPPORT_ECNT_MAP)
	/* mesh agent ignore this event */
	if(1 == check_mesh_enable() && 2 == check_mesh_role())
		return 0;
#endif
	
	cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", 0, fixmode, sizeof(fixmode));
	cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "CurAPMode", 0, curmode, sizeof(curmode));
	/* check current link mode and AP up state */
	if ( 0 == strcmp(source, "APClient") )
	{
		/* apclient up/down: do nothing if ap mode is not equal to apclient */
		if ( 0 == strcmp(fixmode, "APClient") || 0 == strcmp(curmode, "APClient") )
			return 0;
		else
			return -1;
	}
	else
	{
		if ( EVT_XPON_DOWN == evt )
			getPreTimeEthDown();
		/* etherwan up/down: do nothing if ap mode is not equal to bridge/route */
		if ( 0 == strcmp(fixmode, "APClient") || 0 == strcmp(curmode, "APClient") )
			return -1;
		else
			return 0;
	}
}


int np_check_wan_boot()
{
	int wan_boot = 0;
	char boot_val[8] = {0};

	cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanBoot", boot_val, sizeof(boot_val));
	if ( 0 != boot_val[0] )
		wan_boot = atoi(boot_val);
	else
		wan_boot = 0;
	return wan_boot;
}

int np_add_br_interface(char *itf_name)
{
	char cmd[SVC_WAN_BUF_256_LEN];

	if ( NULL == itf_name )
		return -1;
	
	/* add br interface */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/brctl addif %s %s", NP_BR0_IF, itf_name);
	svc_wan_execute_cmd(cmd);
	return 0;
}

int np_del_br_interface(char *itf_name)
{
	char cmd[SVC_WAN_BUF_256_LEN];

	if ( NULL == itf_name )
		return -1;
	
	/* add br interface */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/brctl delif %s %s", NP_BR0_IF, itf_name);
	svc_wan_execute_cmd(cmd);
	return 0;
}

int np_create_dev_interface(char *itf_name, char *pvc_name)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char dir[SVC_WAN_BUF_32_LEN];
	int res = 0;
	char itf_mac[20] = {0};

	if ( NULL == itf_name || NULL == pvc_name )
		return -1;
	
	/* create interface folder */
	memset(dir, 0, sizeof(dir));
	snprintf(dir, sizeof(dir), "/var/run/%s", itf_name);

	res = mkdir(dir, 0777); 
	if( res ); /**do nothing,just for compile*/

#if SMUX_ENABLE
	/* create interface */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl add bridge %s %s 1 0 0 0", pvc_name, itf_name);
	svc_wan_execute_cmd(cmd);

#if 0
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl ipversion %s 1", itf_name); /* 1:IPV4, 2:IPV6, 3:ALL */
	svc_wan_execute_cmd(cmd);
#endif

	/* ifconfig up */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s up", itf_name);
	svc_wan_execute_cmd(cmd);
#endif

	return 0;
}

int np_del_dev_interface(char *itf_name)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char dir[32] = {0};

	if ( NULL == itf_name )
		return -1;
	
	memset(dir, 0, sizeof(dir));
	snprintf(dir, sizeof(dir), "/var/run/%s", itf_name);
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/bin/rm -rf %s", dir);
	svc_wan_execute_cmd(cmd);

	/* stop interface */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s down", itf_name);
	svc_wan_execute_cmd(cmd);

	/* delete interface */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl rem %s", itf_name);
	svc_wan_execute_cmd(cmd);

	return 0;
}

int np_restart_dnsmasq_for_bridge()
{
	FILE* fp = NULL;
	FILE* fpInfo = NULL;
	char buf[64] = {0};
	
	/* step1: down dnsmasq process */
	system("/usr/bin/killall -9 dnsmasq");

	/* step2: write dnsmasq.conf and dnsInfo.conf, only for AP Bridge */
	fp = fopen("/etc/dnsmasq.conf", "w");
	if ( NULL == fp )
	{
		tcdbg_printf("open dnsmasq.conf failed");
		return -1;
	}

	fpInfo = fopen("/etc/dnsInfo.conf", "w");
	if(fpInfo == NULL)
	{
		fclose(fp);
		tcdbg_printf("open dnsInfo.conf failed");
		return -1;
	}

	/* only for AP Bridge!!!
	**dnsmasq.conf format is:
	** strict-order
	** no-resolv
	** server=

	** dnsInfo.conf will be empty.
	*/

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "no-resolv\n");
	fwrite(buf, sizeof(char), strlen(buf), fp);

	snprintf(buf, sizeof(buf), "server=\n");
	fwrite(buf, sizeof(char), strlen(buf), fp);

	fclose(fp);
	fclose(fpInfo);
	
	/* step3: start dnsmasq process */
	system("/userfs/bin/dnsmasq &");
	return 0;
}

int np_wan_pre_conf()
{
	char cmd[SVC_WAN_BUF_256_LEN];

	/* enable nas10 ipv6 */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/bin/echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6", NP_WAN_IF);
	svc_wan_execute_cmd(cmd);

	/* init AP Bridge IP */
	np_wan_set_bridge_info(EVT_WAN_AP_BRIDGE_LOST4, NP_AP_B_WAN_IF);
	return 0;
}

int np_wan_pre_get_timeout(int *eth_up_timeout, int *ap_wan_timeout)
{
	char buff[16] = {0};

	if ( NULL != eth_up_timeout )
	{
		memset(buff, 0, sizeof(buff));
		cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "EtherUpTimeout", 0, buff, sizeof(buff));
		if ( 0 == buff[0] )
			*eth_up_timeout = 0;
		else
			*eth_up_timeout = atoi(buff);
	}

	if ( NULL != ap_wan_timeout )
	{
		memset(buff, 0, sizeof(buff));
		cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "APWanTimeout", 0, buff, sizeof(buff));
		if ( 0 == buff[0] )
			*ap_wan_timeout = 0;
		else
			*ap_wan_timeout = atoi(buff);
	}

	return 0;
}

int np_reconfig_dnsmasq(char *dns)
{
	char buf[64] = {0};
	FILE *fp = NULL;

	if ( NULL == dns )
		return -1;

	/* write dns to /etc/resolv.conf */
	fp = fopen("/etc/resolv.conf", "w");
	if ( NULL == fp )
		return -1;

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "nameserver %s", dns);
	fputs(buf, fp);
	fclose(fp);

	return 0;
}

int np_restart_alinkmgr()
{
	cfg_commit_object(ALINKMGR_ENTRY_NODE);
	usleep(100);
	return 0;
}

int np_start_dhcp_server()
{
	/* start dhcpd */
	cfg_set_object_attr(LAN_DHCP_NODE, "type", "1");
	cfg_delete_object(DHCPRELAY_ENTRY_NODE);
	cfg_commit_object(DHCPD_COMMON_NODE);
	cfg_commit_object(LAN_DHCP_NODE);

	/* start dhcp6s & radvd */
	cfg_set_object_attr(DHCP6S_ENTRY_NODE, "Enable", "1");
	cfg_commit_object(DHCP6S_ENTRY_NODE);
	cfg_set_object_attr(RADVD_ENTRY_NODE, "Enable", "1");
	cfg_commit_object(RADVD_ENTRY_NODE);
	cfg_evt_write_romfile_to_flash();
	return 0;
}

int np_stop_dhcp_server()
{
	/* stop dhcpd */
	cfg_set_object_attr(LAN_DHCP_NODE, "type", "0");
	cfg_delete_object(DHCPRELAY_ENTRY_NODE);
	cfg_commit_object(DHCPD_COMMON_NODE);
	cfg_commit_object(LAN_DHCP_NODE);
	
	/* stop dhcp6s & radvd */
	cfg_set_object_attr(DHCP6S_ENTRY_NODE, "Enable", "0");
	cfg_commit_object(DHCP6S_ENTRY_NODE);
	cfg_set_object_attr(RADVD_ENTRY_NODE, "Enable", "0");
	cfg_commit_object(RADVD_ENTRY_NODE);
	cfg_evt_write_romfile_to_flash();
	return 0;
}

void np_get_hostname(char *host, int len)
{
	char hwaddr[32] = {0};
	char ssid[32] = {0};
	
	if ( NULL == host|| len <= 0 )
		return;

	cfg_get_object_attr(DEVICEINFO_DEVPARADYNAMIC_NODE, "ssid", ssid, sizeof(ssid));

	memset(hwaddr, 0, sizeof(hwaddr));
	np_get_itf_mac(NP_BR0_IF, hwaddr);
	
	if ( 0 == hwaddr[0] )
	{
		snprintf(host, len, "%s", ssid);
		return;
	}
	
	snprintf(host, len, "%s-%s", ssid, &hwaddr[6]);
	return;
	}

int np_wan_start_br_alink_dhcpc(char *dev)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char host[64] = {0};

	if ( NULL == dev )
		return -1;
	
	np_get_hostname(host, sizeof(host));

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/udhcpc -i %s -h %s -s /usr/script/udhcpc_nodef.sh -p /var/run/%s/udhcpc.pid &", dev, host, dev);
	svc_wan_execute_cmd(cmd);
	return 0;
}

int np_wan_stop_br_alink_dhcpc(char *dev)
{
	char path[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char cmd[SVC_WAN_BUF_256_LEN];

	if ( NULL == dev )
		return -1;

	/* kill pid */
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "/var/run/%s/udhcpc.pid", dev);
	if( svc_wan_get_file_string(path, info_string, 1) <= 0 )
	{
		return -1;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/bin/kill -15 %s", info_string[0]);
	svc_wan_execute_cmd(cmd);

	return 0;
}

int np_wan_start_ap_bridge()
{
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "Bridge");
	/* stop dhcpd & dhcp6s & radvd */
	np_stop_dhcp_server();

#if SMUX_ENABLE
	/* create nas_br interface */
	np_create_dev_interface(NP_BR_ALINK_IF, NP_WAN_IF);
	np_add_br_interface(NP_BR_ALINK_IF);
#else
	np_add_br_interface(NP_WAN_IF);
#endif
	
	/* create br_alink interface */
	np_create_dev_interface(NP_AP_B_WAN_IF, NP_WAN_IF);

	/* start dhcpc */
	getPreTime();
	np_wan_start_br_alink_dhcpc(NP_AP_B_WAN_IF);
	
	/* restart dnsmasq */
	np_restart_dnsmasq_for_bridge();

	return 0;
}

int np_wan_stop_ap_bridge()
{
	/* stop dhcpc */
	np_wan_stop_br_alink_dhcpc(NP_AP_B_WAN_IF);
#if SMUX_ENABLE
	np_del_dev_interface(NP_AP_B_WAN_IF);
#endif
	
	/* clear ap mode */
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "");

#if SMUX_ENABLE
	/* remove br_alink */
	np_del_br_interface(NP_BR_ALINK_IF);
	np_del_dev_interface(NP_BR_ALINK_IF);
#else
	np_del_br_interface(NP_WAN_IF);
#endif
	
	/* reset dns resolv.conf */
	np_reconfig_dnsmasq("127.0.0.1");
	return 0;
}

int np_wan_start_ap_route()
{
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "Route");
	np_wan_update_proc(NP_ROUTE_PARAM_DEF, NP_ROUTE_PARAM_DEF, NP_ROUTE_PARAM_DEF, NP_LANHOST_MGR_PROC, LL_OP_ROUTE);
	cfg_set_object_attr(NP_ROUTE_WAN_PATH, "Active", "Yes");
	cfg_commit_object(NP_ROUTE_WAN_PATH);
	np_start_dhcp_server();
	cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1") ;
	return 0;
}

int np_wan_stop_ap_route()
{
	/* clear ap mode */
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "");

	cfg_set_object_attr(NP_ROUTE_WAN_PATH, "Active", "No");
	cfg_commit_object(NP_ROUTE_WAN_PATH);
	np_stop_dhcp_server();

	/* stop dnsmasq */
	svc_wan_execute_cmd("/usr/bin/killall -9 dnsmasq");
	
	return 0;
}

int np_wan_start_ap_client()
{
	char radio[4] = {0};
	char apcli_itf[8] = {0};
	
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "APClient");
	/* stop dhcpd & dhcp6s & radvd */
	np_stop_dhcp_server();

	/* br itf will be added when apcli0 or apclii0 up */
	cfg_get_object_attr(APCLI_COMMON_NODE, "currentRadio", radio, sizeof(radio));
	if ( 0 == radio[0] || 0 == strcmp(radio, "0") )
		strncpy(apcli_itf, NP_WLAN_IF, sizeof(apcli_itf) - 1);
	else if ( 0 == strcmp(radio, "1") )
		strncpy(apcli_itf, NP_WLANAC_IF, sizeof(apcli_itf) - 1);
	else
		return -1;

#if SMUX_ENABLE
	/* create nas_br interface */
	np_create_dev_interface(NP_BR_ALINK_IF, apcli_itf);
	np_add_br_interface(NP_BR_ALINK_IF);
#else
	np_add_br_interface(apcli_itf);
#endif
	
	/* create br_alink interface */
	np_create_dev_interface(NP_AP_B_WAN_IF, apcli_itf);

	/* start dhcpc */
	getPreTime();
	np_wan_start_br_alink_dhcpc(NP_AP_B_WAN_IF);
	
	/* restart dnsmasq */
	np_restart_dnsmasq_for_bridge();

	return 0;
}

int np_wan_stop_ap_client()
{
	char radio[4] = {0};
	char apcli_itf[8] = {0};
	
	np_wan_stop_br_alink_dhcpc(NP_AP_B_WAN_IF);
#if SMUX_ENABLE
	np_del_dev_interface(NP_AP_B_WAN_IF);
#endif
	
	/* clear ap mode */
	cfg_set_object_attr(WANINFO_COMMON_NODE, "CurAPMode", "");
	cfg_set_object_attr(WANINFO_COMMON_NODE, "need_apstart", "0");
	cfg_set_object_attr(APCLI_COMMON_NODE, "StartConn", "0");
	cfg_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", "1");

#if SMUX_ENABLE
	/* remove br_alink */
	np_del_br_interface(NP_BR_ALINK_IF);
	np_del_dev_interface(NP_BR_ALINK_IF);
#else
	/* br itf will be deleted when apcli0 or apclii0 down */
	np_del_br_interface(NP_WLAN_IF);
	np_del_br_interface(NP_WLANAC_IF);
	cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_STOP_CLIENT, NULL, 0);
#endif

	/* reset dns resolv.conf */
	np_reconfig_dnsmasq("127.0.0.1");
	return 0;
}

int np_wan_stop_current_ap_interface()
{
	int curmode;
	
	curmode = check_ap_wan_fixed_mode(NP_WAN_UP);
	np_wan_stop_by_fixed_mode(curmode);
	return 0;
}

int np_wan_ap_mode_update(char *path)
{
	int fixed_mode = NP_WAN_AP_NONE, up_mode = NP_WAN_AP_NONE;
	int switch_mode = NP_WAN_AP_NONE;

	fixed_mode = check_ap_wan_fixed_mode(NP_WAN_FIXED);
	up_mode = check_ap_wan_fixed_mode(NP_WAN_UP);
	switch_mode = getSwitchMode();
	getPreTimeEthDown();/*init time spec*/
	
	if ( up_mode != switch_mode )
	{
		/* stop switch ap mode */
		np_wan_stop_by_fixed_mode(switch_mode);
	}
	
	cfg_set_object_attr(WANINFO_COMMON_NODE, "APAction", "");
	if ( NP_WAN_AP_NONE == up_mode )
	{
		/* init current ap status */
		setAPStatus(NP_WAIT_TIMEOUT);
		tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
		if ( NP_WAN_AP_NONE != fixed_mode )
		{
			setSwitchMode(fixed_mode);
			np_wan_start_by_fixed_mode(fixed_mode);
		}
		else
		{
			cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_STOP_CLIENT, NULL, 0);
			/*init time spec*/
			getPreTime();
			/* start auto link up */
			setSwitchMode(fixed_mode);
		}
	}
	else if ( fixed_mode != up_mode )
	{
		cfg_set_object_attr(WANINFO_COMMON_NODE, "APAction", "Modify");
		/* stop current ap mode */
		np_wan_stop_by_fixed_mode(up_mode);
		
		/* restart alink-mgr */
		np_restart_alinkmgr();

		/* init current ap status */
		setAPStatus(NP_WAIT_TIMEOUT);
		tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
		if ( NP_WAN_AP_NONE != fixed_mode )
		{
			setSwitchMode(fixed_mode);
			np_wan_start_by_fixed_mode(fixed_mode);
		}
		else
		{
			cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_STOP_CLIENT, NULL, 0);
			/*init time spec*/
			getPreTime();
			/* start auto link up */
			setSwitchMode(fixed_mode);
		}
	}
	else if ( NP_WAN_AP_ROUTE == fixed_mode )
	{
		setSwitchMode(fixed_mode);
		cfg_commit_object(NP_ROUTE_WAN_PATH);
	}
	else
	{
		cfg_set_object_attr(WANINFO_COMMON_NODE, "APAction", "Modify");
		np_wan_stop_by_fixed_mode(fixed_mode);
		
		/* restart alink-mgr */
		np_restart_alinkmgr();
		
		setSwitchMode(fixed_mode);
		np_wan_start_by_fixed_mode(fixed_mode);
	}
	return 0;
}

int np_wan_ap_link_update(int sync_flag)
{
	char tmpbuf[20] = {0};
	
	if( ECNT_APCLI_SYNC_ALINK == sync_flag )
	{
		np_send_if3_request(sync_flag);
	}
	else
	{
		cfg_get_object_attr(APCLI_COMMON_NODE, "currentRadio", tmpbuf, sizeof(tmpbuf));
		if( 0 == atoi(tmpbuf) )
		{
			np_del_br_interface("apclii0");
			np_add_br_interface("apcli0");
		}
		else
		{
			np_del_br_interface("apcli0");
			np_add_br_interface("apclii0");
		}
	}	
	return 0;
}

int np_wan_set_ap_default_route()
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char path[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];

	/* add default route */
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "/var/run/%s/gateway", NP_AP_B_WAN_IF);
	if( svc_wan_get_file_string(path, info_string, 1) <= 0 )
	{
		return -1;
	}
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s", info_string[0], NP_AP_B_WAN_IF);
	svc_wan_execute_cmd(cmd);
	
	return 0;
}

int np_wan_del_ap_default_route()
{
	char cmd[SVC_WAN_BUF_256_LEN];

	/* delete default route */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip route delete default");
	svc_wan_execute_cmd(cmd);
	
	return 0;
}

int np_wan_add_static_route(char *dev)
{
	char dns_v4[MAX_WAN_IPV4_ADDR_LEN] = {0};
	char cmd[SVC_WAN_BUF_256_LEN] = {0};

	if ( NULL == dev )
		return -1;

	if ( cfg_get_object_attr(APWANINFO_ENTRY_NODE, "DNS", dns_v4, sizeof(dns_v4)) <= 0 )
		return -1;

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s dev %s", dns_v4, dev);
	svc_wan_execute_cmd(cmd);
	return 0;
}

void np_dealwith_link_local_route(char* if_name, int if_index, int def_route)
{
	/*isDefaultRoute[0:not default route, 1:add new default route; 2: default route switch]*/
	char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
	char net_mask_v4[MAX_WAN_IPV4_ADDR_LEN];
	char sub_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
	char cmd[SVC_WAN_BUF_256_LEN];
	struct in_addr gatewayIP;
	struct in_addr netmask;
	struct in_addr subIP;
	int is_internet = 0;

	memset(gate_way_v4, 0, sizeof(gate_way_v4));
	memset(net_mask_v4, 0, sizeof(net_mask_v4));
	if(cfg_get_object_attr(APWANINFO_ENTRY_NODE, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0)
	{
	    return ;
	}

	if(cfg_get_object_attr(APWANINFO_ENTRY_NODE, "NetMask", net_mask_v4, sizeof(net_mask_v4)) < 0)
	{
	    return ;
	}

	memset(sub_ip_v4, 0, sizeof(sub_ip_v4));
	inet_aton(gate_way_v4, &gatewayIP);
	inet_aton(net_mask_v4, &netmask);
	subIP.s_addr = (gatewayIP.s_addr & netmask.s_addr);
	strncpy(sub_ip_v4, inet_ntoa(subIP), sizeof(sub_ip_v4) - 1);

	is_internet = 0;
	/*del link route if it is not default wan, and add link route when it is default route*/
	if(def_route)
	{
	    if(def_route == E_DEFAULT_ROUTE_SWITCH && 0 == is_internet)
	    {
	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/sbin/route add -net %s netmask %s dev %s", sub_ip_v4, net_mask_v4, if_name);
	        svc_wan_execute_cmd(cmd);
	    }
	}
	else
	{
		if ( 0 == is_internet )
		{
	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/sbin/route del -net %s netmask %s dev %s", sub_ip_v4, net_mask_v4, if_name);
	        svc_wan_execute_cmd(cmd);
		}
	}

	return ;
}

void np_wan_set_policy_route(int evt)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char if_name[MAX_WAN_DEV_NAME_LEN];
	char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
	char ip_v4[MAX_WAN_IPV4_ADDR_LEN];
	char w_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
	char ip_netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
	char netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
	char mask_dec[SVC_WAN_BUF_8_LEN];

	unsigned int mark;
	struct in_addr ip_v4_addr;
	struct in_addr netmask_v4_addr;
	int idx  = NP_AP_B_WAN_IDX;

	memset(if_name, 0, sizeof(if_name));
	strcpy(if_name, NP_AP_B_WAN_IF);

	mark = (NP_AP_B_WAN_IDX + 1) << 16; 
	switch (evt) 
	{
	    case EVT_WAN_CONN_GETV4:
	    {
	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "%s -D OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", IPTABLES_MANGLE, if_name, mark);
	        svc_wan_execute_cmd(cmd);

	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "%s -A OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", IPTABLES_MANGLE, if_name, mark);
	        svc_wan_execute_cmd(cmd);

	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + idx);
	        svc_wan_execute_cmd(cmd);

	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + idx);
	        svc_wan_execute_cmd(cmd);

	        if(cfg_get_object_attr(APWANINFO_ENTRY_NODE, "GateWay", gate_way_v4, sizeof(gate_way_v4)) > 0)
	        {
	            np_dealwith_link_local_route(if_name, idx, E_DEFAULT_ROUTE_SWITCH);
	            memset(cmd, 0, sizeof(cmd));
	            snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s table %d", 
	            gate_way_v4, if_name, 100 + idx);
	            svc_wan_execute_cmd(cmd);
	            np_dealwith_link_local_route(if_name, idx, E_NOT_DEFAULT_ROUTE);
	        }

			memset(ip_v4, 0, sizeof(ip_v4));
			memset(netmask_v4, 0, sizeof(netmask_v4));
			if((cfg_get_object_attr(APWANINFO_ENTRY_NODE, "IP", ip_v4, sizeof(ip_v4)) > 0) 
			    && (cfg_get_object_attr(APWANINFO_ENTRY_NODE, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
			{
			    if ((strlen(netmask_v4) > 0) && (strlen(ip_v4) > 0))
			    {
			        if ( !check_mask_format(netmask_v4, mask_dec, sizeof(mask_dec)) )
			        {
			            return ;
			        }
			        inet_aton(ip_v4, &ip_v4_addr);
			        inet_aton(netmask_v4, &netmask_v4_addr);
			        ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);

			        memset(ip_netmask_v4, 0, sizeof(ip_netmask_v4));
			        snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));

			        memset(cmd, 0, sizeof(cmd));
			        snprintf(cmd,  sizeof(cmd), "/usr/bin/ip route add %s/%s dev %s table %d", 
			            ip_netmask_v4, mask_dec, if_name, 100 + idx);
			        svc_wan_execute_cmd(cmd);
			    }
			}

			memset(w_ip_v4, 0, sizeof(w_ip_v4));
			if(cfg_get_object_attr(APWANINFO_ENTRY_NODE, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0)
			{
			    memset(cmd, 0, sizeof(cmd));
			    snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add from %s/32 table %d", w_ip_v4, 100 + idx);
			    svc_wan_execute_cmd(cmd);
				
			    memset(ip_v4, 0, sizeof(ip_v4));
			    memset(netmask_v4, 0, sizeof(netmask_v4));
			    if((cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", ip_v4, sizeof(ip_v4)) > 0) 
			        && (cfg_get_object_attr(LAN_ENTRY0_NODE, "netmask", netmask_v4, sizeof(netmask_v4)) > 0))
			    {
			        if ((strlen(ip_v4) > 0) && (strlen(netmask_v4) > 0))
			        {
			            if ( !check_mask_format(netmask_v4, mask_dec, sizeof(mask_dec)) )
			            {
			                snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
			            }
			            inet_aton(ip_v4, &ip_v4_addr);
			            inet_aton(netmask_v4, &netmask_v4_addr);
			            ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
			            snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
			            memset(cmd, 0, sizeof(cmd));
			            snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev br0 table %d", 
			                ip_netmask_v4, mask_dec, ip_v4, 100 + idx);
			        }
			    }
			    else
			    {
			        memset(cmd, 0, sizeof(cmd));
			        snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add 192.168.10.0/24 via 192.168.10.1 dev br0 table %d", 
			            100 + idx);
			    }

	            svc_wan_execute_cmd(cmd);
	        }

	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush cache 2>/dev/null");
	        svc_wan_execute_cmd(cmd);
	        break;
	    }
	    case EVT_WAN_CONN_LOSTV4:
	    {
	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "%s -D OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", IPTABLES_MANGLE, if_name, mark);
	        svc_wan_execute_cmd(cmd);

	        memset(ip_v4, 0, sizeof(ip_v4));
			cfg_get_object_attr(APWANINFO_ENTRY_NODE, "IPLast", ip_v4, sizeof(ip_v4));
	        if(strlen(ip_v4) > 0)
	        {
	            memset(cmd, 0, sizeof(cmd));
	            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del from %s/32 table %d", ip_v4, 100 + idx);
	            svc_wan_execute_cmd(cmd);
	        }
	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + idx);
	        svc_wan_execute_cmd(cmd);

	        memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush table %d", 100 + idx);
	        svc_wan_execute_cmd(cmd);
	        break;
	    }
	    default:
	    {
	        break;
	    }
	}

	return;
}

#if defined(TCSUPPORT_NP_CMCC)
void change_br0_ip(char* wan_ip)
{
	char lanIp[20] = {0};
	int ip[4] = {0};
	int laniptmp[4] = {0};
	other_evt_t param;
	char option60[20] = {0};
	int option60tmp[4] = {0};

	cfg_obj_get_object_attr(LAN_ENTRY_NODE, "IP", 0, lanIp, sizeof(lanIp));
	sscanf(lanIp, "%d.%d.%d.%d", &laniptmp[0], &laniptmp[1], &laniptmp[2], &laniptmp[3]);
	sscanf(wan_ip, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);

	if((laniptmp[0] == ip[0]) && (laniptmp[1] == ip[1]) && (laniptmp[2] == ip[2]))
	{
		laniptmp[2] = laniptmp[2] + 1;
		memset(lanIp, 0, sizeof(lanIp));
		snprintf(lanIp, sizeof(lanIp), "%d.%d.%d.%d", laniptmp[0], laniptmp[1], laniptmp[2], laniptmp[3]);
		cfg_set_object_attr(LAN_ENTRY_NODE, "IP", lanIp);
		
		memset(&param, 0, sizeof(param));
		strncpy(param.buf, LAN_ENTRY_NODE, sizeof(param)-1);
		cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_LAN_UPDATE, (void*)&param, sizeof(param));

		cfg_set_object_attr(DHCPD_COMMON_NODE, "router", lanIp);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "router", lanIp);

		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "start", 0, option60, sizeof(option60));
		sscanf(option60, "%d.%d.%d.%d", &option60tmp[0], &option60tmp[1], &option60tmp[2], &option60tmp[3]);
		
		memset(option60, 0, sizeof(option60));
		snprintf(option60, sizeof(option60), "%d.%d.%d.%d", option60tmp[0], option60tmp[1], laniptmp[2], option60tmp[3]);

		cfg_set_object_attr(DHCPD_COMMON_NODE, "start", option60);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "start", option60);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "startSTB", option60);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "startPhone", option60);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "startCamera", option60);
		cfg_set_object_attr(DHCPD_OPTION60_NODE, "startHGW", option60);
		
		memset(&param, 0, sizeof(param));
		strncpy(param.buf, DHCPD_NODE, sizeof(param)-1);
		cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_DHCPD_UPDATE, (void*)&param, sizeof(param));
	}
	
	return;
}
#endif

int np_wan_set_ipv4_info(char* node, char* path)
{
    char info_string[2][MAX_INFO_LENGTH];
    char pathtmp[128] = {0};
    int num;

    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "ip");

    cfg_set_object_attr(node,"Status","up");

    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"IP", info_string[0]);
        cfg_set_object_attr(node,"IPLast", info_string[0]);

#if defined(TCSUPPORT_NP_CMCC)
		change_br0_ip(info_string[0]);
#endif
    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "netmask");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"NetMask", info_string[0]);
    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dns");
    num = svc_wan_get_file_string(pathtmp, info_string, 2);
    if (num > 0)
    {
        cfg_set_object_attr(node,"DNS", info_string[0]);
    }
    if (num > 1)
    {
        cfg_set_object_attr(node,"SecDNS", info_string[1]);
    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "gateway");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"GateWay", info_string[0]);
    }

    return 0;
}

int np_wan_set_bridge_info(int stat, char *dev)
{
    char fpath[32];
    char* empty = "";

	if ( NULL == dev )
		return -1;

    if (cfg_query_object(APWANINFO_ENTRY_NODE,NULL,NULL) <= 0)
    {
        cfg_create_object(APWANINFO_ENTRY_NODE);
    }

    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s", dev);
    switch(stat)
    {
        case EVT_WAN_AP_BRIDGE_GET4:
        {
            np_wan_set_ipv4_info(APWANINFO_ENTRY_NODE, fpath);
            break;
        }
        case EVT_WAN_AP_BRIDGE_LOST4:
        {
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "Status","down");
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "IP", empty);
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "NetMask", empty);
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "DNS", empty);
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "SecDNS", empty);
            cfg_set_object_attr(APWANINFO_ENTRY_NODE, "GateWay", empty);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}


int np_check_manageIP()
{
	char path[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
    char info_string[2][MAX_INFO_LENGTH];

	memset(buff, 0, sizeof(buff));
	if ( cfg_obj_get_object_attr(APWANINFO_ENTRY_NODE, "Status", 0, buff, sizeof(buff)) <= 0 
		|| 0 != strcmp(buff, "up") )
	{
		return -1;
	}

	memset(buff, 0, sizeof(buff));
	if ( cfg_obj_get_object_attr(APWANINFO_ENTRY_NODE, "IP", 0, buff, sizeof(buff)) <= 0 )
	{
		return -1;
	}
	
	return 0;
}


int np_get_itf_mac(char *dev, char *mac)
{
	char buff[20] = {0};

	if ( NULL == dev || NULL == mac )
		return -1;

	getWanInfo(Mac_type, dev, buff);
	if ( 0 == buff[0] )
		return -1;
	
	mac_rm_dot(buff, mac);
	return 0;
}
int np_send_if3_request(int sync_flag)
{
	ecnt_alink_if_3_6_notify_data event_data;
	char buff[16] = {0}, itf_mac[16] = {0};
	int ap_mode = NP_WAN_AP_NONE;
	char radio[4] = {0}, apcli_itf[8] = {0};
	
	/* IF3 START */
	memset(&event_data, 0, sizeof(event_data));
	memset(itf_mac, 0, sizeof(itf_mac));
	event_data.wanMode = ECNT_EVENT_WANMODE_BRIDGE;
	
	ap_mode = check_ap_wan_fixed_mode(NP_WAN_UP);
	if ( NP_WAN_AP_BRIDGE == ap_mode )
	{
		if ( np_get_itf_mac(NP_AP_B_WAN_IF, itf_mac) < 0 )
			return -1;
		strncpy(event_data.deviceMAC, itf_mac, sizeof(event_data.deviceMAC));
		
		memset(buff, 0, sizeof(buff));
		if ( cfg_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", buff, sizeof(buff)) <= 0 )
			event_data.apAutoMode = ECNT_IF3_EVENT_TYPE_AUTO;
		else
			event_data.apAutoMode = ECNT_IF3_EVENT_TYPE_FIXED;
		event_data.eventType = ECNT_EVENT_IF3NOTIFY_START_BRIDGE;
		strncpy(event_data.apUplinkType, "Ethernet", sizeof(event_data.apUplinkType) - 1);
	}
	else if ( NP_WAN_AP_CLIENT == ap_mode )
	{
		memset(radio, 0, sizeof(radio));
		memset(apcli_itf, 0, sizeof(apcli_itf));
		cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, radio, sizeof(radio));
		if ( 0 == strcmp(radio, "0") )
			strncpy(apcli_itf, NP_WLAN_IF, sizeof(apcli_itf) - 1);
		else if ( 0 == strcmp(radio, "1") )
			strncpy(apcli_itf, NP_WLANAC_IF, sizeof(apcli_itf) - 1);
		else
			return -1;
			
		if ( np_get_itf_mac(apcli_itf, itf_mac) < 0 )
			return -1;

#if 0
		if ( 0 == strcmp(radio, "0") ) /* 2.4G */
		{
			strncpy(event_data.deviceMAC, itf_mac, sizeof(event_data.deviceMAC) - 1);
		}
		else if ( 0 == strcmp(radio, "1") ) /* 5G */
		{
			strncpy(event_data.deviceMAC, itf_mac, 6);

			memset(itf_mac, 0, sizeof(itf_mac));
			if ( np_get_itf_mac(NP_BR0_IF, itf_mac) < 0 )
				return -1;
			strncpy(event_data.deviceMAC + 6, itf_mac + 6, 6);
		}
#else
		strncpy(event_data.deviceMAC, itf_mac, 6);

		memset(itf_mac, 0, sizeof(itf_mac));
		if ( np_get_itf_mac(NP_BR0_IF, itf_mac) < 0 )
			return -1;
		strncpy(event_data.deviceMAC + 6, itf_mac + 6, 6);
#endif

		tcdbg_printf("\n%s():%d==> deviceMAC=[%s]\n", __FUNCTION__, __LINE__, event_data.deviceMAC);

		event_data.apAutoMode = ECNT_IF3_EVENT_TYPE_FIXED;
		/* check ap qlink assigned succeed */
		if ( ECNT_APCLI_SYNC_ALINK == sync_flag )
			event_data.eventType = ECNT_EVENT_IF3NOTIFY_START_APCLI_ASSIGN_SUCCESS;
		else
			event_data.eventType = ECNT_EVENT_IF3NOTIFY_START_APCLI;

		memset(buff, 0, sizeof(buff));
		cfg_get_object_attr(APCLI_COMMON_NODE, "syncFlag", buff, sizeof(buff));
		if ( 0 == strcmp(buff, "2") )
			event_data.syncSource = ECNT_APCLI_SYNC_USER;
		else
			event_data.syncSource = sync_flag;
		strncpy(event_data.apUplinkType, "Wireless", sizeof(event_data.apUplinkType) - 1);
	}
	else
		return -1;
	ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_IF3NOTIFY, &event_data, sizeof(event_data));
	return 0;
}

int np_send_if6_request(int evt)
{
	ecnt_alink_if_3_6_notify_data event_data;
	char currMode[8] = {0};
	
	/* IF6 START */
	memset(&event_data, 0, sizeof(event_data));
	if ( EVT_WAN_CONN_GETV4 == evt )
	{
		event_data.eventType = ECNT_EVENT_IF6NOTIFY_START;
	}
	else if ( EVT_WAN_CONN_LOSTV4 == evt )
	{
		event_data.eventType = ECNT_EVENT_IF6NOTIFY_STOP;
	}
	else
		return -1;
		
	event_data.wanMode = ECNT_EVENT_WANMODE_ROUTE;

	cfg_get_object_attr(ALINKMGR_ENTRY_NODE, "currMode", currMode, sizeof(currMode));
	if(strcmp(currMode, "IF3Mode") == 0)
		ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_IF3NOTIFY, &event_data, sizeof(event_data));
	else
		ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_IF6NOTIFY, &event_data, sizeof(event_data));
	
	return 0;
}

int np_check_search_response()
{
	/* get response of search gateway */
	return 0;
}

int np_wan_if3_start()
{
	np_send_if3_request(ECNT_APCLI_SYNC_NONE);
	return 0;
}

int np_wan_if3_stop()
{
	ecnt_alink_if_3_6_notify_data event_data;
	
	/* IF3 STOP */
	memset(&event_data, 0, sizeof(event_data));
	event_data.eventType = ECNT_EVENT_IF3NOTIFY_STOP;
	event_data.wanMode = ECNT_EVENT_WANMODE_BRIDGE;
	ecnt_event_send(ECNT_EVENT_ALINK, ECNT_EVENT_ALINK_IF3NOTIFY, &event_data, sizeof(event_data));
	return 0;
}

int np_wan_ntp_sync()
{
	char server[64] = {0};
	char cmd[SVC_WAN_BUF_256_LEN] = {0};

	cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SERVER", server, sizeof(server));
	snprintf(cmd, sizeof(cmd),"/userfs/bin/ntpclient -s -c 1 -l -h %s", strcmp(server,"0.0.0.0") ? server : "ntp1.cs.wisc.edu");
	system_escape(cmd);
	return 0;
}

int np_wan_update_proc(char *ip, char *mask, char *gateway, char *proc, int is_bridge)
{
	char buf[SVC_WAN_BUF_128_LEN] = {0};
	char ipv6_addr[64] = {0};
	
	if ( NULL == ip || NULL == mask || NULL == gateway || NULL == proc )
		return -1;

	update_br0_linklocal_address(is_bridge);
	cfg_get_object_attr(APWANINFO_ENTRY_NODE, ATTR_EUI_LLADDR2KO, ipv6_addr, sizeof(ipv6_addr));

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%s %s %s %s", ip, mask, gateway, ipv6_addr);
	doValPut(proc, buf);
	return 0;
}

/* add eui64 link local address for br0 when bridge mode. 
ap_bridge_st ==> 1 already bridge   2 already route.
*/
void update_br0_linklocal_address(int is_bridge)
{
	char ap_bst[12] = {0}, cmdbuf[256] = {0}, s_ll_addr[64] = {0};
	unsigned char u_eui[8] = {0}, u_ll_addr[16] = {0};
	int i_ap_bst = 0;

	if ( LL_OP_ROUTE == is_bridge ) /* when route mode, fe80::1 will be used. */
	{
		/* 1. do nothing when already route ST. */
		if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ap_bridge_st", ap_bst, sizeof(ap_bst)) > 0 )
			i_ap_bst = atoi(ap_bst);

		if ( 2 == i_ap_bst )
			return;

		/* 2. add fe80::1 and del eui64. */
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig br0 add %s/64", "fe80::1");
		system(cmdbuf);

		bzero(s_ll_addr, sizeof(s_ll_addr));
		cfg_get_object_attr(APWANINFO_ENTRY_NODE, ATTR_EUI_LLADDR, s_ll_addr, sizeof(s_ll_addr));
		if ( 0 != s_ll_addr[0] )
		{
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig br0 del %s/64", s_ll_addr);
			system(cmdbuf);
		}

		cfg_set_object_attr(GLOBALSTATE_NODE, "ap_bridge_st", "2");

	}
	else if ( LL_OP_BRIDGE == is_bridge ) /* when bridge mode, eui64 ll address will be used. */
	{
		/* 1. do nothing when already bridge ST. */
		if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ap_bridge_st", ap_bst, sizeof(ap_bst)) > 0 )
			i_ap_bst = atoi(ap_bst);

		if ( 1 == i_ap_bst )
			return;

		/* 2. add eui64 and del fe80::1. */
		bzero(s_ll_addr, sizeof(s_ll_addr));
		cfg_get_object_attr(APWANINFO_ENTRY_NODE, ATTR_EUI_LLADDR, s_ll_addr, sizeof(s_ll_addr));
		if ( 0 != s_ll_addr[0] )
		{
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig br0 add %s/64", s_ll_addr);
			system(cmdbuf);
		}

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig br0 del %s/64", "fe80::1");
		system(cmdbuf);

		cfg_set_object_attr(GLOBALSTATE_NODE, "ap_bridge_st", "1");
	}

	return;
}

int np_wan_ap_br_alink_get4(char *dev)
{
	int ret = 0, np_status = 0;
	int ap_mode = NP_WAN_AP_NONE;
	char wan_state[8] = {0};
	
	char path[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char dns[16] = {0}, ip[16] = {0}, mask[16] = {0}, gw[16] = {0};
	static int ap_boot = 0;
	
	if ( NULL == dev || 0 != strcmp(dev, NP_AP_B_WAN_IF) )
		return -1;

	memset(wan_state, 0, sizeof(wan_state));
	cfg_obj_get_object_attr(APWANINFO_ENTRY_NODE, "Status", 0, wan_state, sizeof(wan_state));
	if ( 0 == strcmp(wan_state, "up") )
	{
		return 0;
	}

	np_wan_set_bridge_info(EVT_WAN_AP_BRIDGE_GET4, dev);
#if SMUX_ENABLE
	np_wan_set_policy_route(EVT_WAN_CONN_GETV4);
	np_wan_add_static_route(NP_AP_B_WAN_IF);
	np_dealwith_link_local_route(NP_AP_B_WAN_IF, NP_AP_B_WAN_IDX, E_NOT_DEFAULT_ROUTE);
#else
	np_wan_set_ap_default_route();
#endif
	/* restart dnsmasq */
	cfg_get_object_attr(APWANINFO_ENTRY_NODE, "DNS", dns, sizeof(dns));
	if ( 0 == dns[0] )
		return -1;
	np_reconfig_dnsmasq(dns);

	/* ntp sync once time when boot */
	if ( 0 == ap_boot )
	{
		ap_boot = 1;
		np_wan_ntp_sync();
	}

	/* update lanhostmgr proc */
	cfg_get_object_attr(APWANINFO_ENTRY_NODE, "IP", ip, sizeof(ip));
	if ( 0 == ip[0] )
		strncpy(ip, NP_BRIDGE_IP_DEF, sizeof(ip) - 1);
	cfg_get_object_attr(APWANINFO_ENTRY_NODE, "NetMask", mask, sizeof(mask));
	if ( 0 == mask[0] )
		strncpy(mask, NP_BRIDGE_MASK_DEF, sizeof(mask) - 1);
	cfg_get_object_attr(APWANINFO_ENTRY_NODE, "GateWay", gw, sizeof(gw));
	if ( 0 == gw[0] )
		strncpy(gw, NP_BRIDGE_GW_DEF, sizeof(gw) - 1);
	np_wan_update_proc(ip, mask, gw, NP_LANHOST_MGR_PROC, LL_OP_BRIDGE);

	/* change ap state to get ip */
	setAPStatus(NP_BR_ALINK_IP_GET);
	tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);

	svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_UP, NP_AP_B_WAN_IF);
	return 0;
}

int np_wan_ap_br_alink_lost4(char *dev)
{
	char wan_state[8] = {0};
	char action[8] = {0};
	
	if ( NULL == dev || 0 != strcmp(dev, NP_AP_B_WAN_IF) )
		return -1;

	memset(wan_state, 0, sizeof(wan_state));
	cfg_obj_get_object_attr(APWANINFO_ENTRY_NODE, "Status", 0, wan_state, sizeof(wan_state));
	if ( 0 == strcmp(wan_state, "down") )
	{
		return 0;
	}

	np_wan_set_bridge_info(EVT_WAN_AP_BRIDGE_LOST4, dev);
#if SMUX_ENABLE
	np_wan_set_policy_route(EVT_WAN_CONN_LOSTV4);
#else
	np_wan_del_ap_default_route();
#endif
	
	/* if ap mode is changed, alink-mgr will be restarted. so we should not send stop msg to alink-mgr */
	tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[NP_BR_ALINK_IP_LOST]);
	cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "APAction", 0, action, sizeof(action));
	if ( 0 != strcmp(action, "Modify") )
		np_wan_if3_stop();
	else
		cfg_set_object_attr(WANINFO_COMMON_NODE, "APAction", "");

	svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_DOWN, NP_AP_B_WAN_IF);
	return 0;
}

int np_wan_start_by_fixed_mode(int mode)
{
	if ( mode < NP_WAN_AP_NONE )
		return -1;

	switch(mode)
	{
		case NP_WAN_AP_BRIDGE:
			setAPStatus(NP_WAIT_BR_ALINK_UP);
			tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
			np_wan_start_ap_bridge();
			break;
		case NP_WAN_AP_ROUTE:
			np_wan_start_ap_route();
			break;
		case NP_WAN_AP_CLIENT:
			setAPStatus(NP_WAIT_APCLI_UP);
			tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
			getPreTime();
			cfg_set_object_attr(WANINFO_COMMON_NODE, "need_apstart", "1");
			cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_START_CLIENT, NULL, 0);
			break;
		default:
			tcdbg_printf("\n%s:np wan mode is out of range!\n", __FUNCTION__);
			return -1;
	}
	return 0;
}

int np_wan_stop_by_fixed_mode(int mode)
{
	if ( mode <= NP_WAN_AP_NONE )
		return -1;

	switch(mode)
	{
		case NP_WAN_AP_BRIDGE:
			np_wan_stop_ap_bridge();
			break;
		case NP_WAN_AP_ROUTE:
			np_wan_stop_ap_route();
			break;
		case NP_WAN_AP_CLIENT:
			np_wan_stop_ap_client();
			break;
		default:
			tcdbg_printf("\n%s:ap wan has not been up!\n", __FUNCTION__);
			return -1;
	}
	return 0;
}
int check_ap_wan_fixed_mode(int flag)
{
	int mode = NP_WAN_AP_NONE;
	char ap_mode[16] = {0};

	memset(ap_mode, 0, sizeof(ap_mode));
	if ( NP_WAN_UP == flag )
		cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "CurAPMode", 0, ap_mode, sizeof(ap_mode));
	else
		cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", 0, ap_mode, sizeof(ap_mode));
	
	if ( 0 == strcmp(ap_mode, "Bridge") )
		mode = NP_WAN_AP_BRIDGE;
	else if ( 0 == strcmp(ap_mode, "Route") )
		mode = NP_WAN_AP_ROUTE;
	else if ( 0 == strcmp(ap_mode, "APClient") )
		mode = NP_WAN_AP_CLIENT;
	else
		mode = NP_WAN_AP_NONE;
	return mode;
}

int np_event_handle(int ap_status)
{
	switch(ap_status)
	{
		case NP_BR_ALINK_IP_GET:
			/*np_search_gateway();*/
			break;
		case NP_BR_ALINK_IP_GET_TIMEOUT:
			np_wan_stop_current_ap_interface();
			np_wan_start_ap_route(); /* ap route mode will started by andlink */
			break;
		case NP_BR_ALINK_IP_LOST:
			np_wan_if3_stop();
			break;
		case NP_FOUND_GATEWAY_REQ:
			np_wan_if3_start();
			break;
		case NP_APCLI_UP:
			np_wan_stop_current_ap_interface();
			np_wan_start_ap_client(); /* ap route mode will started by andlink */
			break;
		default:
			tcdbg_printf("\n%s:np wan status wait timeout!\n", __FUNCTION__);
			break;
	}

	return 0;
}

int create_ll_eui64_addres(void)
{
	char mac_buf[24] = {0}, s_ll_addr[64] = {0};
	unsigned char u_eui[8] = {0}, u_ll_addr[16] = {0};
	int i_tmp_mac[6] = {0};

	/* 1. get BR0 mac. */
	bzero(mac_buf, sizeof(mac_buf));
	bzero(i_tmp_mac, sizeof(i_tmp_mac));
	getWanInfo(Mac_type, "br0", mac_buf);
	sscanf(mac_buf, "%02X:%02X:%02X:%02X:%02X:%02X"
			, &i_tmp_mac[0], &i_tmp_mac[1], &i_tmp_mac[2]
			, &i_tmp_mac[3], &i_tmp_mac[4], &i_tmp_mac[5]);

	/* 2. create link local address. */
	u_ll_addr[0] = 0xfe;
	u_ll_addr[1] = 0x80;
	u_ll_addr[15] = 0x01;

	/* 3. create eui64. */
	u_eui[0] = i_tmp_mac[0];
	u_eui[0] ^= 0x02;
	u_eui[1] = i_tmp_mac[1];
	u_eui[2] = i_tmp_mac[2];
	u_eui[3] = 0xFF;
	u_eui[4] = 0xFE;
	u_eui[5] = i_tmp_mac[3];
	u_eui[6] = i_tmp_mac[4];
	u_eui[7] = i_tmp_mac[5];

	memcpy(&u_ll_addr[8], u_eui, 8);

	/* 4. update to br0 interface. */
	bzero(s_ll_addr, sizeof(s_ll_addr));
	snprintf(s_ll_addr, sizeof(s_ll_addr)
		, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x"
		, u_ll_addr[0], u_ll_addr[1], u_ll_addr[2], u_ll_addr[3]
		, u_ll_addr[4], u_ll_addr[5], u_ll_addr[6], u_ll_addr[7]
		, u_ll_addr[8], u_ll_addr[9], u_ll_addr[10], u_ll_addr[11]
		, u_ll_addr[12], u_ll_addr[13], u_ll_addr[14], u_ll_addr[15]);
	/* 5. save it. */
	cfg_set_object_attr(APWANINFO_ENTRY_NODE, ATTR_EUI_LLADDR, s_ll_addr);

	bzero(s_ll_addr, sizeof(s_ll_addr));
	snprintf(s_ll_addr, s_ll_addr
		, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
		, u_ll_addr[0], u_ll_addr[1], u_ll_addr[2], u_ll_addr[3]
		, u_ll_addr[4], u_ll_addr[5], u_ll_addr[6], u_ll_addr[7]
		, u_ll_addr[8], u_ll_addr[9], u_ll_addr[10], u_ll_addr[11]
		, u_ll_addr[12], u_ll_addr[13], u_ll_addr[14], u_ll_addr[15]);
	cfg_set_object_attr(APWANINFO_ENTRY_NODE, ATTR_EUI_LLADDR2KO, s_ll_addr);

	return 0;
}

int np_wan_ap_boot()
{
	int switch_mode = NP_WAN_AP_NONE;

	/* init start time and wan boot flag */
	getPreTime();

	/* prepare configuration */
	np_wan_pre_conf();

	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wanBoot", "1");
	cfg_set_object_attr(APCLI_COMMON_NODE, "WPSMode", "0");

	create_ll_eui64_addres();

	/* insmod np_lanhost_mgr.ko and init proc */
	svc_wan_execute_cmd("/sbin/insmod /lib/modules/"NP_LANHOST_MGR_KO);
	np_wan_update_proc(NP_BRIDGE_IP_DEF, NP_BRIDGE_MASK_DEF, NP_BRIDGE_GW_DEF, NP_LANHOST_MGR_PROC, LL_OP_BRIDGE);
	/* fixed mode loading */
	switch_mode = check_ap_wan_fixed_mode(NP_WAN_FIXED);
	if ( NP_WAN_AP_NONE != switch_mode )
	{
		setSwitchMode(switch_mode);
		np_wan_start_by_fixed_mode(switch_mode);
	}

	return 0;
}

int np_wan_start_process()
{
	char buff[16] = {0};
	int eth_up_timeout = 0, ap_wan_timeout = 0, found_gw_timeout = 0;
	int ret = 0, is_fixed = 0, mode = NP_WAN_AP_NONE, apcli_connstatus = NP_APCLI_DISCONNECTED;
	struct timespec curtime;
	char alink_status[4] = {0}, alink_mode[8] = {0}, apcli_start[4] = {0};
	static int wan_boot = 0;
#if defined(TCSUPPORT_ECNT_MAP)
	static int role_dected = 0;
#endif

	if ( 0 == wan_boot )
		wan_boot = np_check_wan_boot();
	
	if ( wan_boot )
	{
		/* wait until wlan load complete */
		memset(buff, 0, sizeof(buff));
		if( cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", buff, sizeof(buff)) < 0
			|| 0 != strcmp(buff, "1") )
		{
			return -1;
		}
		
#if defined(TCSUPPORT_ECNT_MAP)
		if(1 == check_mesh_enable() && 2 == check_mesh_role())
		{
			/*had fix AP bridge mode*/

			if(NP_WAN_AP_BRIDGE != getSwitchMode())
			{
                np_wan_stop_by_fixed_mode(getSwitchMode());
				np_wan_send_evt(EVT_WAN_AP_START_BRIDGE, 0);
				setSwitchMode(NP_WAN_AP_BRIDGE);
				setAPStatus(NP_WAIT_BR_ALINK_UP);			
				tcdbg_printf("\n%s:%d==>Agent role switch to bridge[%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
			}	
			if ( NP_BR_ALINK_IP_GET == getAPStatus() )
			{
				setAPStatus(NP_FOUND_GATEWAY_REQ);
				tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
				np_wan_send_evt(EVT_WAN_AP_STATUS_UPDATE, getAPStatus());
			}

			return 0;
				
		}
#endif
		
		if ( NP_WAN_AP_NONE != (mode = check_ap_wan_fixed_mode(NP_WAN_FIXED)) )
			is_fixed = 1;
		
		np_wan_pre_get_timeout(&eth_up_timeout, &ap_wan_timeout);
		if ( 0 == is_fixed && NP_WAIT_TIMEOUT == getAPStatus() )
		{
			if ( NP_WAN_AP_NONE == getSwitchMode() )
			{
				if ( E_WAN_PON_UP != wan_mgr_get_status() )
				{
					/* ether wan up is timeout, switch ap mode to ap client */
					clock_gettime(CLOCK_MONOTONIC, &curtime);
					if ( curtime.tv_sec - g_pretime.tv_sec > eth_up_timeout 
#if defined(TCSUPPORT_ECNT_MAP)
						&& 0 == check_mesh_enable()
#endif
						)
					{
						setSwitchMode(NP_WAN_AP_CLIENT);
						setAPStatus(NP_WAIT_APCLI_UP);
						tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
						cfg_set_object_attr(WANINFO_COMMON_NODE, "need_apstart", "1");
						if ( NP_APCLI_DISCONNECTED == (apcli_connstatus = np_wlan_apcli_connstatus()) )
							cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_START_CLIENT, NULL, 0);
					}
				}
				else
				{
					/* ether wan up, switch ap mode to ap bridge first */
					np_wan_send_evt(EVT_WAN_AP_START_BRIDGE, 0);
					setSwitchMode(NP_WAN_AP_BRIDGE);
					setAPStatus(NP_WAIT_BR_ALINK_UP);
					tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
					getPreTime();
				}
			}
		}

		if ( NP_WAN_AP_CLIENT == mode 
			|| NP_WAN_AP_CLIENT == getSwitchMode() )
		{
			if ( NP_WAIT_APCLI_UP == getAPStatus() )
			{
				memset(buff, 0, sizeof(buff));
				if( cfg_get_object_attr(WANINFO_COMMON_NODE, "need_apstart", buff, sizeof(buff)) > 0
					&& 0 == strcmp(buff, "1")
					&& NP_APCLI_DISCONNECTED != (apcli_connstatus = np_wlan_apcli_connstatus()))
				{
					cfg_set_object_attr(WANINFO_COMMON_NODE, "need_apstart", "0");
					setAPStatus(NP_APCLI_UP);
					tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
					if( NP_APCLI_24G_CONNECTED == apcli_connstatus)
					{
						cfg_set_object_attr(WLAN_COMMON_NODE, "ChannelDisable", "1");
						cfg_set_object_attr(WLAN11AC_COMMON_NODE, "ChannelDisable", "0");
					}
					else
					{
						cfg_set_object_attr(WLAN_COMMON_NODE, "ChannelDisable", "0");
						cfg_set_object_attr(WLAN11AC_COMMON_NODE, "ChannelDisable", "1");
					}
					np_wan_send_evt(EVT_WAN_AP_STATUS_UPDATE, getAPStatus());
					getPreTime();
				}
			}
		}
	
		if ( ( ( NP_WAN_AP_BRIDGE == mode 
			|| NP_WAN_AP_BRIDGE == getSwitchMode() )
			 && NP_WAIT_BR_ALINK_UP == getAPStatus() ) /* bridge mode */
			 || ( (NP_WAN_AP_CLIENT == mode 
			|| NP_WAN_AP_CLIENT == getSwitchMode() )
			&& NP_APCLI_UP == getAPStatus() ) /* apclient mode */ )
		{
			ret = np_check_manageIP();
			if ( 0 != ret
				&& 0 == is_fixed 
				&& ( NP_WAN_AP_BRIDGE == mode 
					|| NP_WAN_AP_BRIDGE == getSwitchMode() ) )
			{
				clock_gettime(CLOCK_MONOTONIC, &curtime);
				if ( curtime.tv_sec - g_pretime.tv_sec > ap_wan_timeout )
				{
					setAPStatus(NP_BR_ALINK_IP_GET_TIMEOUT);
					tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
					np_wan_send_evt(EVT_WAN_AP_STATUS_UPDATE, getAPStatus());
					setSwitchMode(NP_WAN_AP_ROUTE);
				}
			}
		}
	
		if ( NP_BR_ALINK_IP_GET == getAPStatus() )
		{
			setAPStatus(NP_FOUND_GATEWAY_REQ);
			tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
			np_wan_send_evt(EVT_WAN_AP_STATUS_UPDATE, getAPStatus());
		}

		if ( is_fixed )
			return 0;

		if ( E_WAN_PON_UP == wan_mgr_get_status() )
		{
			clock_gettime(CLOCK_MONOTONIC, &curtime);
			if ( curtime.tv_sec - g_pretime.tv_sec > eth_up_timeout )
			{
				memset(apcli_start, 0, sizeof(apcli_start));
				cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "need_apstart", 0, apcli_start, sizeof(apcli_start));
				if ( ( 0 == strcmp(apcli_start, "1")
						&& NP_APCLI_DISCONNECTED == np_wlan_apcli_connstatus() ) /* apcli does not link up */
					|| 
					( ( NP_WAN_AP_CLIENT == mode || NP_WAN_AP_CLIENT == getSwitchMode() ) /* ap mode is apclient */
						&& cfg_obj_get_object_attr(ALINKMGR_ENTRY_NODE, "step", 0, alink_status, sizeof(alink_status)) > 0 
						&& 0 == strcmp(alink_status, "1") ) /* register fail */ )
				{
					/* 
						apcli does not  link up or has been failed, 
						AP should change ap mode to auto and retry up with bridge mode 
					 */
					np_wan_stop_ap_client();
					cfg_set_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", "");
					setSwitchMode(NP_WAN_AP_NONE);
					setAPStatus(NP_WAIT_TIMEOUT);
					tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
				}
			}
		}
		else if ( NP_WAIT_TIMEOUT != getAPStatus()
			&& NP_WAN_AP_NONE != getSwitchMode() 
			&& NP_WAN_AP_CLIENT != getSwitchMode() 
			&& NP_WAN_AP_CLIENT != mode )
		{
			/* 
				ether wan down beyond eth_up_timeout, 
				AP should change mode to auto and retry up.
				
				if current work mode is apcli, do nothing.
			*/
			memset(apcli_start, 0, sizeof(apcli_start));
			cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "need_apstart", 0, apcli_start, sizeof(apcli_start));
			if ( 0 != strcmp(apcli_start, "1") )
			{
				clock_gettime(CLOCK_MONOTONIC, &curtime);
				if ( curtime.tv_sec - g_pretime_ethdown.tv_sec > eth_up_timeout )
				{
					np_wan_stop_by_fixed_mode(getSwitchMode());
					cfg_set_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", "");
					setSwitchMode(NP_WAN_AP_NONE);
					setAPStatus(NP_WAIT_TIMEOUT);
					tcdbg_printf("\n%s:%d==>AP Wan Status [%s]\n", __FUNCTION__, __LINE__, ap_wan_status[getAPStatus()]);
					getPreTime();
				}
			}
		}
	}
	
	return 0;
}


