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
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "wan_link_list.h"
#include "port_binding.h"
#include "wan_cfg.h"
#include "msg_notify.h"
#include "default_route_handle.h"
#include "pd_info.h"
#include "dns_info.h"
#include "static_route.h"
#include "policy_route.h"
#include "wan_nat.h"
#include "misc_srv.h"
#include "wan_mgr.h"
#include "wan_polling.h"
#include "wan_protocol_handle.h"
#include "update_child_prefix.h"
#include "update_ipv6_gateway.h"
#include "wan_interface.h"
#include "vlan_binding.h"
#if defined(TCSUPPORT_NP_CMCC)
#include "np_wan_handle.h"
#endif
#include "utility.h"
#include "vlan_eth_pair.h"
#include "blapi_traffic.h"
#include "blapi_perform.h"
#if defined(TCSUPPORT_CT_SDN)
#include "sdn_ovs.h"
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#endif
#include <sys/stat.h>
/************************************************************************/
static int svc_wan_stop_intf(wan_entry_obj_t* obj);
static int svc_wan_start_intf(wan_entry_obj_t* obj);
static int svc_wan_update_object(wan_entry_obj_t* obj);
static void del_waninfo_entry(wan_entry_obj_t* obj);
static void del_object_do_something(wan_entry_obj_t* obj);
static void update_wan_common_LatestIFIdex(void);
static int svc_wan_delete_object(wan_entry_obj_t* obj);
static int svc_wan_handle_event_delete(char* path);
static int svc_wan_handle_event_update(char* path);
static int svc_wan_handle_event_conn_getv4(char* dev);
static int svc_wan_handle_event_conn_getv6(char* dev);
static int svc_wan_handle_event_conn_lostv4(char* dev);
static int svc_wan_handle_event_conn_lostv6(char* dev);
static int bridge_up_down(wan_entry_obj_t* obj, int evt);
static void dump_svc_wan_entry_cfg(wan_entry_obj_t* obj);
static void msg_dump_svc_wan_cfg(char*buf, int evt);
static char check_ipv6_info_ready(wan_entry_obj_t* obj);

/***********************************************************************/
#if defined(TCSUPPORT_CT_JOYME4)
int svc_wan_set_ratelimit(wan_entry_obj_t* obj, int action)
{
	char uprate[16] = {0}, downrate[16] = {0};
	char wan_name[16] = {0};
	char wanMode[16] = {0}, mtuValue[16] = {0};
	unsigned char wan_type = 0; /*0: bridge, 1:route*/
	int up_rate = 0, dn_rate = 0, ret = -1;
	int isppp = -1, meter_mode = -1;
	wan_entry_cfg_t* cfg = NULL;
	char nodeName[64] = {0};
	char wanRateThr[32] = {0};
	
	cfg = &obj->cfg;

	snprintf(nodeName, sizeof(nodeName), WAN_COMMON_NODE);
	if(cfg_get_object_attr(nodeName, "WanRateUsePpsThreshold", wanRateThr, sizeof(wanRateThr)) >= 0)
	{
		blapi_traffic_set_wan_rate_threshold(atoi(wanRateThr));
	}
	
	snprintf(wan_name, sizeof(wan_name), "nas%d_%d", obj->idx/8, obj->idx%8);

	cfg_get_object_attr(obj->path, "WanMode", wanMode, sizeof(wanMode));
	cfg_get_object_attr(obj->path, "MTU", mtuValue, sizeof(mtuValue));
	if(0 == strcmp(wanMode, "Route"))
		wan_type = 1;
	else
		wan_type = 0;

	if( 0 == action )
	{
		blapi_traffic_set_wan_mtu_val(wan_type, atoi(mtuValue));
		/* when meter mdoe is NOT WAN_MODE, no need to clear it if rate is ZERO.*/
		ret = blapi_traffic_get_ratelimit_mode(UP_DIR, &meter_mode);
		if ( RATELIMIT_MODE_WAN == meter_mode )
			ret = blapi_traffic_set_wan_ratelimit(wan_name, obj->idx, 0, 0);
	}
	else
	{
		cfg_get_object_attr(obj->path,"UpRate", uprate, sizeof(uprate));
		cfg_get_object_attr(obj->path,"DownRate", downrate, sizeof(downrate));
		if( '\0' == uprate[0] )
			up_rate = 0;
		else
			up_rate = atoi(uprate) * 512;

		if( '\0' == downrate[0] )
			dn_rate = 0;
		else
			dn_rate = atoi(downrate) * 512;

		blapi_traffic_set_wan_mtu_val(wan_type, atoi(mtuValue));
		/* when meter mdoe is NOT WAN_MODE, no need to clear it if rate is ZERO.*/
		ret = blapi_traffic_get_ratelimit_mode(UP_DIR, &meter_mode);
		if ( (up_rate || dn_rate) && (RATELIMIT_MODE_WAN == meter_mode) )
		{
			ret = blapi_traffic_set_ratelimit_mode(RATELIMIT_MODE_WAN);
			ret = blapi_traffic_set_wan_ratelimit(wan_name, obj->idx, up_rate, dn_rate);
		}
	}

	return ret;
}

int clear_icmp_wan_chain(wan_entry_obj_t* obj)
{
	char cmdbuf[256] = {0}, ifname[64] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;

	if ( SVC_WAN_IS_PPP(cfg) )
		snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
	else
		snprintf(ifname, sizeof(ifname), "%s", obj->dev);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -F ICMP_WAN_LIMIT_%s",
						ifname);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -F ICMP_WAN_LIMIT_%s",
						ifname);
	system(cmdbuf);

	return 0;
	
}
int svc_update_icmp_wan_chain(wan_entry_obj_t* obj, int is_add)
{
	char cmdbuf[256] = {0}, ifname[64] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;

	if ( SVC_WAN_IS_PPP(cfg) )
		snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
	else
		snprintf(ifname, sizeof(ifname), "%s", obj->dev);

	/* create ICMP chain for each wan interfaces. */
	if ( is_add )
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -N ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -A ICMP_WAN_LIMIT -j ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -F ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -Z ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -N ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -A ICMP_WAN_LIMIT -j ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -F ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -Z ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);
	}
	else
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -D ICMP_WAN_LIMIT -j ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/iptables -t filter -X ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -D ICMP_WAN_LIMIT -j ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), 
						"/usr/bin/ip6tables -t filter -X ICMP_WAN_LIMIT_%s",
						ifname);
		system(cmdbuf);
	}

	return 0;
}

int svc_update_icmp_drop_rule(wan_entry_obj_t* obj, int is_start)
{
	char cmdbuf[256] = {0}, ifname[64] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;

	if ( SVC_WAN_IS_PPP(cfg) )
		snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
	else
		snprintf(ifname, sizeof(ifname), "%s", obj->dev);
	
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), 
	"/usr/bin/iptables -t filter %s ICMP_WAN_LIMIT_%s -p icmp --icmp-type echo-request -i %s -j DROP",
	(is_start ? "-A" : "-D"), ifname, ifname);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf),
	"/usr/bin/ip6tables -t filter %s ICMP_WAN_LIMIT_%s -p icmpv6 --icmpv6-type echo-request -i %s -j DROP",
	(is_start ? "-A" : "-D"), ifname, ifname);
	system(cmdbuf);

	return 0;
}

int svc_ping_state_setup(wan_entry_obj_t* obj, int is_start)
{
	char cmdbuf[256] = {0}, ifname[64] = {0}, node_wan[32] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;
	char *token = NULL, *safep = NULL;
	int wan_pvc_idx = 0, wan_entry_idx = 0;
	char start_addr[80] = {0}, end_addr[80] = {0};
	static char whiteList[1200];
	char ping_resp_active[8] = {0};
	
	if ( SVC_WAN_IS_ROUTE(cfg) )
	{
		wan_pvc_idx = obj->idx / 8;
		wan_entry_idx = obj->idx % 8;
		snprintf(node_wan, sizeof(node_wan), WAN_PVC_ENTRY_NODE, wan_pvc_idx + 1, wan_entry_idx + 1);
		
		cfg_get_object_attr(node_wan, "PingResponse", ping_resp_active, sizeof(ping_resp_active));
		if ( !strcmp(ping_resp_active, "Yes") )
		{
			/* check white list. */
			bzero(whiteList, sizeof(whiteList));
			
			if ( cfg_get_object_attr(node_wan, "PingResponseWhiteList", whiteList, sizeof(whiteList)) > 0 )
			{
				svc_update_icmp_wan_chain(obj, is_start);

				if ( SVC_WAN_IS_PPP(cfg) )
					snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
				else
					snprintf(ifname, sizeof(ifname), "%s", obj->dev);

				token = strtok_r(whiteList, ",", &safep);
				while ( token )
				{
					if ( NULL == strstr(token, "-") ) /* single ip */
					{
						if ( strstr(token, ":") ) /* IPv6 */
						{
							bzero(cmdbuf, sizeof(cmdbuf));
							snprintf(cmdbuf, sizeof(cmdbuf),
							"/usr/bin/ip6tables -t filter %s ICMP_WAN_LIMIT_%s -p icmpv6 --icmpv6-type echo-request -i %s -s %s -j RETURN",
							(is_start ? "-A" : "-D"), ifname, ifname, token);
							system(cmdbuf);
						}
						else
						{
							/* IPv4 */
							bzero(cmdbuf, sizeof(cmdbuf));
							snprintf(cmdbuf, sizeof(cmdbuf), 
							"/usr/bin/iptables -t filter %s ICMP_WAN_LIMIT_%s -p icmp --icmp-type echo-request -i %s -s %s -j RETURN",
							(is_start ? "-A" : "-D"), ifname, ifname, token);
							system(cmdbuf);
						}
					}
					else /* ip range */ 
					{
						if ( strstr(token, ":") ) /* IPv6 */
						{
							bzero(cmdbuf, sizeof(cmdbuf));
							snprintf(cmdbuf, sizeof(cmdbuf),
							"/usr/bin/ip6tables -t filter %s ICMP_WAN_LIMIT_%s -p icmpv6 --icmpv6-type echo-request -i %s -m iprange --src-range %s -j RETURN",
							(is_start ? "-A" : "-D"), ifname, ifname, token);
							system(cmdbuf);
						}
						else
						{
							/* IPv4 */
							bzero(cmdbuf, sizeof(cmdbuf));
							snprintf(cmdbuf, sizeof(cmdbuf), 
							"/usr/bin/iptables -t filter %s ICMP_WAN_LIMIT_%s -p icmp --icmp-type echo-request -i %s -m iprange --src-range %s -j RETURN",
							(is_start ? "-A" : "-D"), ifname, ifname, token);
							system(cmdbuf);
						}
					}
					token = strtok_r(NULL, ",", &safep);
				}

				/* add all drop rule. */
				svc_update_icmp_drop_rule(obj, is_start);
			}
			else
			{
				clear_icmp_wan_chain(obj);
			}
			
		}
		else
		{
			svc_update_icmp_wan_chain(obj, is_start);
			svc_update_icmp_drop_rule(obj, is_start);
		}
	}

	return 0;
}

static int svc_wan_datacfg_event_update(char* path)
{
	wan_entry_obj_t* obj = NULL;
	char wan_path[32] = {0};
	int entry_idx = 0, wan_pvc_idx = 0, wan_entry_idx = 0, i_action = 0;
#if defined(TCSUPPORT_CT_FULL_ROUTE)
	int full_route_mode = 0;
#endif
	wan_entry_cfg_t* cfg = NULL;
	
	if( get_entry_number_cfg2(path, "entry.", &entry_idx) != 0 )
		return -1;

	entry_idx -= 1;

	wan_pvc_idx = entry_idx / 8;
	wan_entry_idx = entry_idx % 8;

	bzero(wan_path, sizeof(wan_path));
	snprintf(wan_path, sizeof(wan_path), WAN_PVC_ENTRY_NODE, wan_pvc_idx + 1, wan_entry_idx + 1);
	
	if( cfg_query_object(wan_path,NULL,NULL) <= 0 )
	{
		SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: cfg wan_path [%s] fail  \n",wan_path);
		return -1;
	}

	if((obj = svc_wan_find_object(wan_path)) == NULL)
		return -1;

	cfg = &obj->cfg;

	if(cfg->active == SVC_WAN_ATTR_ACTIVE_ENABLE)
	{
		/* Speedlimit  */
		svc_wan_set_ratelimit(obj, 0);
		svc_wan_set_ratelimit(obj, 1);

#if defined(TCSUPPORT_CT_FULL_ROUTE)
		/* IPForwardList  */
		if((full_route_mode = is_full_route_enable(obj)) != 0)
		{
			if(0 != get_table_status(obj->idx))
			{
				update_IPForwardList(obj, full_route_mode);
			}
		}
#endif

		svc_wan_execute_cmd("/userfs/bin/hw_nat -!");
		/* update pingresponse rule */
		svc_ping_state_setup(obj, 1);
	}
	
	return 0;
}

static int svc_wan_update_bss_capability_vip(int url_type)
{
	char ip_addr[MAX_WAN_IPV4_ADDR_LEN];
	struct in_addr in_s_v4addr = {0};
	static unsigned int ip_hex_last[E_ADDR_MAX_NUM] = {0};
	static unsigned short port_last[E_ADDR_MAX_NUM] = {0};
	ifc_param_t ifc_data;
	char sourceaddr[SVC_WAN_BUF_64_LEN] = {0}, hostname[SVC_WAN_BUF_64_LEN] = {0};
	unsigned short port = 0;
	char nodename[64] = {0}, addrattr[16] = {0};
	char retrytimes[4] = {0};
	int i_retrytimes = 0;

	if ( E_BSSADDR == url_type )
	{
		strncpy(nodename, MOBILE_ENTRY_NODE, sizeof(nodename) - 1);
		strncpy(addrattr, "BSSAddr", sizeof(addrattr) - 1);
	}
	else if ( E_CAPABILITYADDR == url_type )
	{
		strncpy(nodename, PLUGIN_COMMON_NODE, sizeof(nodename) - 1);
		strncpy(addrattr, "CapabilityAddr", sizeof(addrattr) - 1);
	}
	else
		return -1;

	/* get addr */
	cfg_get_object_attr(nodename, addrattr, sourceaddr, sizeof(sourceaddr));
	if ( 0 == sourceaddr[0] )
		return -1;

	if ( 0 != parseUrlHostInfo(sourceaddr, hostname, sizeof(hostname) - 1, &port) )
		return -1;

	/* resolve hostname, only for ipv4 now, set timeout = 5s */
	if ( -1 == ecnt_dns_resolve(hostname, ip_addr, sizeof(ip_addr), AF_INET, 5) )
	{
		/* return failed when we have retry 3 times */
		memset(retrytimes, 0, sizeof(retrytimes));
		cfg_get_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", retrytimes, sizeof(retrytimes));
		i_retrytimes = atoi(retrytimes);
		i_retrytimes++;
		/*
		if ( i_retrytimes >= 3 )
		{
			cfg_set_object_attr(GLOBALSTATE_NODE, "bss_retrytimes", "0");
			return -1;
		}
		*/

		/* retry 3 times for dns resolve */
		memset(retrytimes, 0, sizeof(retrytimes));
		snprintf(retrytimes, sizeof(retrytimes), "%d", i_retrytimes);
		cfg_set_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", retrytimes);
		cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", "1");
		cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", "0");
		return -1;
	}
	else
	{
		/* clear dns retry times */
		cfg_set_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", "0");
	}

	if ( 1 != inet_pton(AF_INET, ip_addr, &in_s_v4addr) )
		return -1;

	/* delete last ifc rules */
	if ( ip_hex_last[url_type] )
	{
		if ( ip_hex_last[url_type] == htonl(in_s_v4addr.s_addr) && port_last[url_type] == port )
		{
			return 0;
		}
		memset(&ifc_data, 0, sizeof(ifc_data));
		ifc_data.dip = ip_hex_last[url_type];
		ifc_data.dport = port_last[url_type];
		ifc_data.ifc_type = DIP_DPORT_CHG;
		blapi_traffic_update_ifc_vip_params(IFC_DEL, &ifc_data);

		memset(&ifc_data, 0, sizeof(ifc_data));
		ifc_data.sip = ip_hex_last[url_type];
		ifc_data.sport = port_last[url_type];
		ifc_data.ifc_type = SIP_SPORT_CHG;
		blapi_traffic_update_ifc_vip_params(IFC_DEL, &ifc_data);
	}

	/* add new bss ifc rules */
	memset(&ifc_data, 0, sizeof(ifc_data));
	ifc_data.dip = htonl(in_s_v4addr.s_addr);
	ifc_data.dport = port;
	ifc_data.ifc_type = DIP_DPORT_CHG;
	blapi_traffic_update_ifc_vip_params(IFC_ADD, &ifc_data);

	memset(&ifc_data, 0, sizeof(ifc_data));
	ifc_data.sip = htonl(in_s_v4addr.s_addr);
	ifc_data.sport = port;
	ifc_data.ifc_type = SIP_SPORT_CHG;
	blapi_traffic_update_ifc_vip_params(IFC_ADD, &ifc_data);
	
	ip_hex_last[url_type] = htonl(in_s_v4addr.s_addr);
	port_last[url_type] = port;

	return 0;
}

static int svc_wan_update_bssaddr_vip()
{
	char def_route_idx[4] = {0};

	/* find default route */
	cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", def_route_idx, sizeof(def_route_idx));
	if ( 0 == def_route_idx[0] )
		return -1;

	if ( 0 != svc_wan_update_bss_capability_vip(E_BSSADDR) )
		return -1;

	if ( 0 != svc_wan_update_bss_capability_vip(E_CAPABILITYADDR) )
		return -1;

	return 0;
}

static int checkIsDefRouteIPv4Wan(int if_idx)
{
	char def_route_idx[4] = {0};

	if ( if_idx < 0 || if_idx >= SVC_MAX_WAN_INTF_NUMBER )
		return 0;

	cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", def_route_idx, sizeof(def_route_idx));
	if ( 0 == def_route_idx[0] || 0 == strcmp(def_route_idx, "N/A") )
		return 0;

	if ( if_idx == atoi(def_route_idx) )
		return 1;

	return 0;
}

void svc_wan_ob_info_restart(wan_entry_obj_t* obj)
{
	blapi_info_ioctl_OB_map_data data;
	wan_entry_cfg_t* cfg = NULL;

	if ( NULL == obj )
		return;
	cfg = &obj->cfg;

	if ( SVC_WAN_IS_BRIDGE(cfg) && SVC_WAN_SRV_OTHER(cfg) )
	{
		bzero(&data, sizeof(data));
		data.wan_idx = obj->idx;
		blapi_traffic_ob_info_restart(&data);
	}
	return;
}

static void svc_wan_update_other_bridge_index()
{
	char pvcnode[32] = {0}, nodeName[32] = {0};
	char servicelist[32] = {0}, wanmode[8] = {0};
	char buf[256] = {0};
	int i = 0, j = 0, len = 0;

	memset(buf, 0, sizeof(buf));
	for ( i = 0; i < SVC_WAN_MAX_PVC_NUM; i++ )
	{
		memset(pvcnode, 0, sizeof(pvcnode));
		snprintf(pvcnode, sizeof(pvcnode), WAN_PVC_NODE, i + 1);
		
		if ( cfg_obj_query_object(pvcnode, NULL, NULL) <= 0 )
			continue;
	
		for ( j = 0; j < SVC_WAN_MAX_ENTRY_NUM; j++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, i + 1, j + 1);
			if ( cfg_obj_query_object(nodeName, NULL, NULL) <= 0 )
				continue;

			memset(servicelist, 0, sizeof(servicelist));
			memset(wanmode, 0, sizeof(wanmode));
			if ( cfg_get_object_attr(nodeName, "ServiceList", servicelist, sizeof(servicelist)) <= 0 )
				continue;
			if ( cfg_get_object_attr(nodeName, "WanMode", wanmode, sizeof(wanmode)) <= 0 )
				continue;
			if ( strstr(servicelist, "OTHER") && 0 == strcmp(wanmode, "Bridge") )
			{
				len += snprintf(buf + len, sizeof(buf) - len, "%d,", i * SVC_WAN_MAX_PVC_NUM + j);
			}
		}
	}
	if ( len > 0 )
		buf[len - 1] = '\0';

	cfg_set_object_attr(WANINFO_COMMON_NODE, "OB_WanIndex", buf);
	return;
}

#endif

static void dump_svc_wan_entry_cfg(wan_entry_obj_t* obj)
{
    char if_name[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t* cfg = NULL;

    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));

    cfg = &obj->cfg;
    svc_wan_printf("obj->path= %s\n",    obj->path);
    svc_wan_printf("obj->idx = %d\n",    obj->idx);
    svc_wan_printf("obj->dev = %s\n",    obj->dev);
    svc_wan_printf("if_name = %s\n",     if_name);
    svc_wan_printf("obj->flag = %d\n",   obj->flag);
    svc_wan_printf("obj->nat = %d\n",    obj->nat);
    if(strlen(obj->pid4))
    {
        svc_wan_printf("obj->pid4 = %s\n", obj->pid4);
    }
    if(strlen(obj->pid6))
    {
        svc_wan_printf("obj->pid6 = %s\n", obj->pid6);
    }

    dump_svc_wan_cfg(cfg, SVC_WAN_IS_BRIDGE(cfg), TRUE);

    return ;
}
/***********************************************************************/

static int svc_wan_stop_intf(wan_entry_obj_t* obj)
{
    if ((obj->flag & SVC_WAN_FLAG_STARTED) == 0)
    {
        return 0;
    }
    svc_wan_do_stop_intf(obj);

    obj->flag &= (~SVC_WAN_FLAG_STARTED);
#if defined(TCSUPPORT_CT_JOYME4)
	/* clear wan ratelimit */
	svc_wan_set_ratelimit(obj, 0);
    svc_ping_state_setup(obj, 0);
	svc_wan_ob_info_restart(obj);
#endif

    return 0;
}

static int svc_wan_start_intf(wan_entry_obj_t* obj)
{
    int ret = 0;
    
    if (obj->flag & SVC_WAN_FLAG_STARTED)
    {
        return 0;
    }

    if (svc_wan_check_xpon_up() <= 0)
    {
        return 0;
    }

    if (svc_wan_check_cfg(&obj->cfg) <= 0)
    {
        return 0;
    }

    ret = svc_wan_do_start_intf(obj);
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	if ( SVC_WAN_IS_CONN_ON_DEMAND((&obj->cfg)) )
	{
		set_ondemand_route_state(obj->idx, 1);
	}
	ClearOndemandWanHisAddr(obj->idx);
#endif

    if(0 == ret)
    {
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
        set_childprefx_file(obj);
#endif
    }

    obj->flag |= SVC_WAN_FLAG_STARTED;
    
#if defined(TCSUPPORT_CT_JOYME4)
	/* set wan ratelimit */
	svc_wan_set_ratelimit(obj, 1);
    svc_ping_state_setup(obj, 1);
	svc_wan_ob_info_restart(obj);
#endif
    
    return 0;
}

static int svc_wan_update_object(wan_entry_obj_t* obj)
{

    wan_entry_cfg_t tmp;
    int action = 0;
    char effect = TRUE;
    wan_entry_cfg_t* cfg = NULL, *cfg_tmp = &tmp;
	static int is_avalanche_on = 0;
	char value[16] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
	char str_work_timer[8] = {0};
#endif

    if (svc_wan_load_cfg(obj->path,&tmp, obj) < 0)
    {
        SVC_WAN_ERROR_INFO("svc_wan_update_object: load cfg fail \n");
        return 0;
    }

	/* It work fail when there 2 inrenet connection  and it will calltrace when reboot always.
	   Move it to wan polling thread.
	*/
#if !defined(TCSUPPORT_CT_JOYME4)
	if(cfg_get_object_attr(SYSTEM_ENTRY_NODE,"avalActive",value,sizeof(value)) >= 0 && (0 == strcmp(value, "1")))
	{
		if ( (SVC_WAN_SRV_INTERNET(cfg_tmp)) )
		{
			if ( (SVC_WAN_IS_ROUTE(cfg_tmp))
			&& (SVC_WAN_ATTR_ISP_STATIC == cfg_tmp->isp)
			&& (900 == cfg_tmp->vid) )
			{
				if ( 0 == is_avalanche_on )
				{
					is_avalanche_on = 1;
					cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "is_avalanche_start", "Yes");
					system_escape("/usr/script/avalanche_start.sh &");
				}
			}
			else
			{
				if ( 1 == is_avalanche_on )
				{
					is_avalanche_on = 0;
					cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "is_avalanche_start", "");
					system_escape("/usr/script/avalanche_stop.sh &");
				}
			}
		}
	}
#endif
#if 0
    if (svc_wan_is_cfg_equal(&tmp,&obj->cfg) == 1)
    {
        if(obj->flag & SVC_WAN_FLAG_STARTED)
        {
            svc_wan_update_opt_cfg(obj);
        }

        return 0;
    }
#endif

    /*tmp :current,        obj: mgr save*/
    if (obj->cfg.active == SVC_WAN_ATTR_ACTIVE_DISABLE && tmp.active == SVC_WAN_ATTR_ACTIVE_ENABLE)
    {
        action = E_WAN_LINK_DISABLE_2_ENABLE;
    }
    else if(obj->cfg.active == SVC_WAN_ATTR_ACTIVE_ENABLE && tmp.active == SVC_WAN_ATTR_ACTIVE_DISABLE)
    {
        action = E_WAN_LINK_ENABLE_2_DISABLE;
    }
    else if (obj->cfg.active == SVC_WAN_ATTR_ACTIVE_ENABLE && tmp.active == SVC_WAN_ATTR_ACTIVE_ENABLE)
    {
        action = E_WAN_LINK_ENABLE_2_ENABLE;
    }
    else 
    {
        return 0;
    }

    cfg = &obj->cfg;
    if(SVC_WAN_SRV_TR069(cfg) && is_commit_from_tr069()
#if defined(TCSUPPORT_CT_JOYME4)
		 && cfg_obj_get_object_attr(CWMP_ENTRY_NODE, "cwmp_work_timer", 0, str_work_timer, sizeof(str_work_timer)) <= 0
#endif
	)
    {
        effect = FALSE;
    }

    if (E_WAN_LINK_DISABLE_2_ENABLE == action)
    {
        memcpy(&obj->cfg,&tmp,sizeof(wan_entry_cfg_t));
        svc_wan_cfg_update_attr(obj->path, &obj->cfg, TRUE);
        svc_wan_load_opt(obj);
        if(effect)
        {
            svc_wan_start_intf(obj);
        }
    }
    else if(E_WAN_LINK_ENABLE_2_DISABLE == action)
    {
        svc_wan_cfg_update_attr(obj->path, &obj->cfg, FALSE);
        if(effect)
        {
            svc_wan_stop_intf(obj);
        }
        memcpy(&obj->cfg,&tmp,sizeof(wan_entry_cfg_t));
    }
    else if (E_WAN_LINK_ENABLE_2_ENABLE == action)
    {
        if(effect)
        {
            svc_wan_stop_intf(obj);
        }
        memcpy(&obj->cfg,&tmp,sizeof(wan_entry_cfg_t));
        svc_wan_cfg_update_attr(obj->path, &obj->cfg, TRUE);
        svc_wan_load_opt(obj);
        if(effect)
        {
            svc_wan_start_intf(obj);
        }
    }

    return 0;
}

static void del_waninfo_entry(wan_entry_obj_t* obj)
{
    char node[SVC_WAN_BUF_32_LEN];
    
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE, obj->idx + 1);
    cfg_delete_object(node);

    return ;
}

static void del_object_do_something(wan_entry_obj_t* obj)
{

    del_waninfo_entry(obj);

    del_static_route(obj);

    return ;
}

static void update_wan_common_LatestIFIdex(void)
{
#if 0
    char node[SVC_WAN_BUF_32_LEN];
    wan_entry_obj_t* obj = NULL;

    obj = svc_wan_get_obj_list();
    if(NULL == obj)
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), "%s", WAN_COMMON_NODE);
        cfg_set_object_attr(node, "LatestIFIdx", "0");
    }
#endif

    return ;
}


static int svc_wan_delete_object(wan_entry_obj_t* obj)
{

    svc_wan_stop_intf(obj);

    del_object_do_something(obj);

    svc_wan_del_from_list(obj);

    update_wan_common_LatestIFIdex();
	
    svc_wan_vlanbind_execute(obj,1);

	restart_dnsmasq();
	
    return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
#define SIP_PID_FILE "/var/run/voip_sip.pid"
int checkVoipWan()
{
	int i = 0, j = 0, voipFlag = 0, pid = 0;
	char wannode[64] = {0}, svcList[32]= {0}, pid_buf[32] = "0", action[32] = {0};
	FILE *fp = NULL;
	
	if(access(SIP_PID_FILE, 0))
		return 0;
	
	for (i = 0; i < SVC_WAN_MAX_PVC_NUM; i++)
	{
		for(j = 0; j < SVC_WAN_MAX_ENTRY_NUM; j++)
		{
			snprintf(wannode, sizeof(wannode), WAN_PVC_ENTRY_NODE, i+1, j+1);
			if(cfg_get_object_attr(wannode, "ServiceList", svcList, sizeof(svcList)) <= 0
				|| NULL == strstr(svcList, "VOICE"))
			{
				continue;
			}
			voipFlag = 1;
			break;
		}
		if(voipFlag)
			break;
	}
	
	if(voipFlag == 0)
	{
		cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "voipload_flag", "0");
		system("/usr/script/voip_unload.sh &");
	}
	
	return 0;
}
#endif

#if defined(TCSUPPORT_CT_JOYME4)
void switch_proxy_wanif(char *if_name, int if_index, int ipversion)
{
	wan_entry_obj_t* obj = NULL;
	wan_entry_cfg_t* cfg = NULL;

	if ( NULL == if_name )
		return;

	/* firstly find other route wan */
	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		if ( obj->idx == if_index )
			continue;
		
		cfg = &obj->cfg;
		if ( !SVC_WAN_IS_ROUTE(cfg) )
			continue;
		
		if ( !SVC_WAN_SRV_OTHER(cfg) )
			continue;

		if ( !cfg->bindbit )
			continue;
		
		if ( !( cfg->version & ipversion ) )
			continue;

		if ( SVC_WAN_IS_PPP(cfg) )
			sprintf(if_name, "ppp%d", obj->idx);
		else
			sprintf(if_name, "%s", obj->dev);
		
		return;
	}

	/* secondly find internet route wan */
	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		if ( obj->idx == if_index )
			continue;
		
		cfg = &obj->cfg;
		if ( !SVC_WAN_IS_ROUTE(cfg) )
			continue;
		
		if ( !SVC_WAN_SRV_INTERNET(cfg) )
			continue;
		
		if ( !( cfg->version & ipversion ) )
			continue;

		if ( SVC_WAN_IS_PPP(cfg) )
			sprintf(if_name, "ppp%d", obj->idx);
		else
			sprintf(if_name, "%s", obj->dev);
		
		return;
	}
}
void checkIgmpMldProxy(wan_entry_obj_t* obj)
{
	wan_entry_cfg_t* cfg = NULL;
	char ifname[8] = {0}, ifname_new[8] = {0};
	char upstreamif_set[16] = {0};

	if ( NULL == obj )
		return;

	cfg = &obj->cfg;
	if ( SVC_WAN_IS_ROUTE(cfg) )
	{
		if ( SVC_WAN_IS_PPP(cfg) )
			snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
		else
			snprintf(ifname, sizeof(ifname), "%s", obj->dev);

		if ( SVC_WAN_IS_IPV4(cfg) )
		{
			memset(upstreamif_set, 0, sizeof(upstreamif_set));
			cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set, sizeof(upstreamif_set));
			if ( 0 == strcmp(upstreamif_set, ifname) )
			{
				memset(ifname_new, 0, sizeof(ifname_new));
				switch_proxy_wanif(ifname_new, obj->idx, SVC_WAN_ATTR_VERSION_IPV4);
				if ( 0 != ifname_new[0] )
				{
					cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", ifname_new);
					cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "Yes");
					cfg_commit_object(IGMPPROXY_ENTRY_NODE);
				}
				else
				{
					cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", "");
					/*kill igmpproxy process*/
					system("killall igmpproxy");
				}
			}
		}
		
		if ( SVC_WAN_IS_IPV6(cfg) )
		{
			memset(upstreamif_set, 0, sizeof(upstreamif_set));
			cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set, sizeof(upstreamif_set));
			if ( 0 == strcmp(upstreamif_set, ifname) )
			{
				memset(ifname_new, 0, sizeof(ifname_new));
				switch_proxy_wanif(ifname_new, obj->idx, SVC_WAN_ATTR_VERSION_IPV6);
				if ( 0 != ifname_new[0] )
				{
					cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", ifname_new);
					cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "Yes");
					cfg_commit_object(MLDPROXY_ENTRY_NODE);
				}
				else
				{
					cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", "");
					/*kill mld proxy process*/
					system("killall ecmh");
				}
			}
		}
	}
	
	return;
}
#endif

static int svc_wan_handle_event_delete(char* path)
{
    wan_entry_obj_t* obj;

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_TRACE_INFO("svc_wan_handle_event_delete: [%s] no exist \n",path);
        return -1;
    }

#if defined(TCSUPPORT_CT_JOYME4)
	checkIgmpMldProxy(obj);
#endif

    if (svc_wan_delete_object(obj) < 0)
    {
        SVC_WAN_TRACE_INFO("svc_wan_handle_event_delete: [%s] delete fail \n",path);
        return -1;
    }
#if defined(TCSUPPORT_CT_JOYME2)
	checkVoipWan();
#endif
    return 0;
}

static void msg_dump_svc_wan_cfg(char*buf, int evt)
{
    wan_entry_obj_t* obj = NULL;

    switch (evt)
    {
        case EVT_DUMP_WAN_SRV_ENTRY_DEV:
        {
            obj = svc_wan_find_object_by_dev(buf);
            break;
        }
        case EVT_DUMP_WAN_SRV_ENTRY_PATH:
        {
            obj = svc_wan_find_object(buf);
            break;
        }
        case EVT_DUMP_WAN_SRV_ALL_ENTRY:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    if(NULL != obj)
    {
        dump_svc_wan_entry_cfg(obj);
        return ;
    }
    else if(EVT_DUMP_WAN_SRV_ALL_ENTRY == evt)
    {
        for(obj = svc_wan_get_obj_list(); NULL != obj; obj = obj->next)
        {
            dump_svc_wan_entry_cfg(obj);
        }
    }

    return ;
}

static int svc_wan_handle_event_update(char* path)
{
    wan_entry_obj_t* obj;
    int id[CFG_MAX_NODE_LEVEL];
    unsigned int pvc,entry;
    char upstreamif[16] = {0};
	char defaultwan_v4[16] = {0};	
	char defaultwan_v6[16] = {0};
    char upstreamif_set[16] = {0};
    wan_entry_cfg_t* cfg = NULL;
#if defined(TCSUPPORT_NP_CMCC)
	char WanHWAddr[20] = {0};
	char IFName[12] = {0};
	char cmd[128] = {0};
#endif

    if (cfg_query_object(path,NULL,NULL) <= 0)
    {
        SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: cfg query [%s] fail  \n",path);
        return 0;
    }

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        if (cfg_path_to_oid_array(path,id) != 4)
        {
            SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: parse [%s] fail \n",path);
            return -1;
        }

        pvc = (id[2] & CFG_OBJ_MASK )- 1;
        entry = (id[3] & CFG_OBJ_MASK) - 1;

        if (pvc >= SVC_WAN_MAX_PVC_NUM || entry >= SVC_WAN_MAX_ENTRY_NUM )
        {
            SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: [%s] pvc[%d] entry[%d] error \n",path,id[1],id[2]);
            return -1;
        }

        if ((obj = svc_wan_create_object(path)) == NULL)
        {
            SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: [%s] create fail \n",path);
            return -1;
        }

        strncpy(obj->path,path, sizeof(obj->path) - 1);
        obj->idx = pvc * SVC_WAN_MAX_ENTRY_NUM + entry;
        snprintf(obj->dev, sizeof(obj->dev), "nas%d_%d", pvc, entry);

#if defined(TCSUPPORT_CT_PON_GD) || defined(TCSUPPORT_CT_PON_CQ) || defined(TCSUPPORT_CMCCV2)
        obj->dhcp_release = TRUE;
#else
        obj->dhcp_release = FALSE;
#endif
    }

    if (svc_wan_update_object(obj) < 0)
    {
        SVC_WAN_ERROR_INFO("svc_wan_handle_event_update: [%s] update fail \n",path);
        return -1;
    }
#if defined(TCSUPPORT_CMCCV2)
    cfg = &obj->cfg;
#if defined(TCSUPPORT_CT_JOYME4)
    if(SVC_WAN_IS_ROUTE(cfg) && (SVC_WAN_SRV_INTERNET(cfg) || SVC_WAN_SRV_OTHER(cfg)))
#else
    if(SVC_WAN_IS_ROUTE(cfg) && SVC_WAN_SRV_OTHER(cfg))
#endif
    {
        if (SVC_WAN_IS_PPP(cfg))
        {
            snprintf(upstreamif_set, sizeof(upstreamif_set), "ppp%d", obj->idx);
        }
        else
        {
            snprintf(upstreamif_set, sizeof(upstreamif_set), "%s", obj->dev);
        }
#if defined(TCSUPPORT_CT_JOYME4)
		if(SVC_WAN_SRV_INTERNET(cfg))
		{	
			if(SVC_WAN_IS_IPV4(cfg)){
				if ( cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "DefaultWAN", defaultwan_v4, sizeof(defaultwan_v4)) <= 0
					|| obj->idx == get_wanindex_by_name(defaultwan_v4) )
					cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "DefaultWAN", upstreamif_set);
			}
			if(SVC_WAN_IS_IPV6(cfg)){
				if ( cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "DefaultWAN", defaultwan_v6, sizeof(defaultwan_v6)) <= 0
					|| obj->idx == get_wanindex_by_name(defaultwan_v6) )
					cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "DefaultWAN", upstreamif_set);
			}
		}
		if(cfg->bindbit && SVC_WAN_SRV_OTHER(cfg)) 
		{
			if(SVC_WAN_IS_IPV4(cfg))
	        {
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set);
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(IGMPPROXY_ENTRY_NODE);
	        }
	        if(SVC_WAN_IS_IPV6(cfg))
	        {
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set);
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(MLDPROXY_ENTRY_NODE);
	        }
		}
		else if((cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "DefaultWAN", defaultwan_v4, sizeof(defaultwan_v4)) > 0) ||
			(cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "DefaultWAN", defaultwan_v6, sizeof(defaultwan_v6)) >0 ))
		{
			if(SVC_WAN_IS_IPV4(cfg))
	        {	        	
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", defaultwan_v4);
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(IGMPPROXY_ENTRY_NODE);
	        }
	        if(SVC_WAN_IS_IPV6(cfg))
	        {
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", defaultwan_v6);
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(MLDPROXY_ENTRY_NODE);
	        }
		}
		else
#endif
		{
	        if(SVC_WAN_IS_IPV4(cfg))
	        {
	            if(cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", upstreamif, sizeof(upstreamif)) <= 0)
	            {
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set);
	                cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(IGMPPROXY_ENTRY_NODE);
	            }
	        }
	        if(SVC_WAN_IS_IPV6(cfg))
	        {
	            if(cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", upstreamif, sizeof(upstreamif)) <= 0)
	            {
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", upstreamif_set);
	                cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "Yes");
	                cfg_commit_object(MLDPROXY_ENTRY_NODE);
	            }
	        }
    	}
    }
    
#endif

#if defined(TCSUPPORT_NP_CMCC)
	if((cfg_obj_get_object_attr(path, "WanHWAddr", 0, WanHWAddr, sizeof(WanHWAddr)) >= 0)
		 && (WanHWAddr[0] != '\0'))
	{
		cfg_obj_get_object_attr(path, "IFName", 0, IFName, sizeof(IFName));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s hw ether %s", IFName, WanHWAddr);
		system(cmd);
	}
#endif

    return 0;
}

static int svc_wan_handle_xpon_event(int evt)
{
	static int pre_evt = 0;

	if(pre_evt == evt)
	{
		SVC_WAN_DEBUG_INFO("evt=%d \n", evt);
		return 0;
	}

    if(EVT_XPON_UP == evt)
    {
        svc_wan_execute_cmd("echo up > /tmp/xpon_stats");

#if defined(TCSUPPORT_CT_DSL_EX)	 			
		svc_wan_start_all_wan_entry(1);
		svc_wan_start_all_wan_entry(0);
#else
		send_signal_wan(EVT_XPON_UP, 1);
		send_signal_wan(EVT_XPON_UP, 0);
#endif

#if defined(TCSUPPORT_CT_PON)
        sendMsgtoCwmp(2, 1, 0);
#endif

	 /*cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "StatusChanging","1");*/
    }
    else if(EVT_XPON_DOWN == evt)
    {
        svc_wan_execute_cmd("echo down > /tmp/xpon_stats");
#if defined(TCSUPPORT_CT_DSL_EX)	 			
		svc_wan_stop_all_wan_entry();
#else	
		send_signal_wan(EVT_XPON_DOWN, 1);
		send_signal_wan(EVT_XPON_DOWN, 0);
#endif
	 /*cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "StatusChanging","0");*/
    }
    else
    {
        SVC_WAN_ERROR_INFO("xpon evt id = %d.\n", evt);
    }

	pre_evt = evt;
    return 0;
}

static int svc_wan_handle_event_conn_getv4(char* dev)
{
    wan_entry_obj_t* obj;
#if defined(TCSUPPORT_NP_CMCC)
	static int ap_boot = 0;
#endif

    if ((obj = svc_wan_find_object_by_dev(dev)) == NULL)
    {
        SVC_WAN_TRACE_INFO("svc_wan_handle_event_conn_getv4: find [%s] fail \n",dev);
        return -1;
    }

    if (obj->flag & SVC_WAN_FLAG_UPV4)
    {
        return 0;
    }

    obj->flag |= SVC_WAN_FLAG_UPV4;

    svc_wan_set_info(obj,EVT_WAN_CONN_GETV4);

    svc_wan_handle_conn_evt(obj,EVT_WAN_CONN_GETV4);

    svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_UP, obj->path);

#if defined(TCSUPPORT_NP_CMCC)
	/* ntp sync once time when boot */
	if ( 0 == ap_boot )
	{
		ap_boot = 1;
		np_wan_ntp_sync();
	}

	/* IF6 start */
	np_send_if6_request(EVT_WAN_CONN_GETV4);
#endif
    return 0;
}

static char check_ipv6_info_ready(wan_entry_obj_t* obj)
{
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    char path[SVC_WAN_BUF_128_LEN];
    char info_string[2][MAX_INFO_LENGTH];

    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("pointer can't be NULL.\n");
        return FALSE;
    }

    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj, itf_name, sizeof(itf_name));

    memset(path, 0, sizeof(path));
    memset(info_string, 0, sizeof(info_string));
    snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "ip6");
    if(0 >= svc_wan_get_file_string(path, info_string, 1))
    {
        return FALSE;
    }

    memset(path, 0, sizeof(path));
    memset(info_string, 0, sizeof(info_string));
    snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "prefix6");
    if(0 >= svc_wan_get_file_string(path, info_string, 1))
    {
        return FALSE;
    }
    if(obj->cfg.dhcpv6pd == ENABLE)
    {
        memset(path, 0, sizeof(path));
        memset(info_string, 0, sizeof(info_string));
        snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "pd6");
        if(0 >= svc_wan_get_file_string(path, info_string, 1))
        {
	    tcdbg_printf("%s:%d no pd6 get!!\n",__FUNCTION__,__LINE__);
        }
    }
    memset(path, 0, sizeof(path));
    memset(info_string, 0, sizeof(info_string));
    snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "nodns6");
    if(0 >= svc_wan_get_file_string(path, info_string, 1))
    {
    memset(path, 0, sizeof(path));
    memset(info_string, 0, sizeof(info_string));
    snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "dns6");
    if(0 >= svc_wan_get_file_string(path, info_string, 1))
    {
        return FALSE;
    }
    }

    memset(path, 0, sizeof(path));
    memset(info_string, 0, sizeof(info_string));
    snprintf(path, sizeof(path), "/var/run/%s/%s", itf_name, "gateway6");
    if(0 >= svc_wan_get_file_string(path, info_string, 1))
    {
        return FALSE;
    }

    return TRUE;
}

static int svc_wan_handle_event_conn_getv6(char* dev)
{
    wan_entry_obj_t* obj;
    wan_entry_cfg_t* cfg = NULL;
    
    if ((obj = svc_wan_find_object_by_dev(dev)) == NULL)
    {
        SVC_WAN_TRACE_INFO("find [%s] fail \n",dev);
        return -1;
    }
    cfg = &obj->cfg;

    if ( obj->flag & SVC_WAN_FLAG_UPV6 && !SVC_WAN_IS_IPV6_SLAAC(cfg) )
    {
        return 0;
    }

    if(!check_ipv6_info_ready(obj))
    {
        return 0;
    }
    
    obj->flag |= SVC_WAN_FLAG_UPV6;
    svc_wan_set_info(obj, EVT_WAN_CONN_GETV6);

    svc_wan_handle_conn_evt(obj, EVT_WAN_CONN_GETV6);
    svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV6_UP, obj->path);
    
    return 0;
}

 void svc_wan_handle_event_conn_lostv4_deal(wan_entry_obj_t* obj)
 {
    obj->flag &= (~SVC_WAN_FLAG_UPV4);

    svc_wan_set_info(obj, EVT_WAN_CONN_LOSTV4);

    svc_wan_handle_conn_evt(obj, EVT_WAN_CONN_LOSTV4);

    svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_DOWN, obj->path);
 }

void svc_wan_handle_event_conn_lostv6_deal(wan_entry_obj_t* obj)
{
	char node[32] = {0};
	
	obj->flag &= (~SVC_WAN_FLAG_UPV6);

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
    cfg_set_object_attr(node,"Status6","down");

#if defined(TCSUPPORT_CT_VRWAN)
	if( check_default_wan(obj) )
		setVRNPTRule(0, obj);
#endif
#if defined(TCSUPPORT_NPTv6)
	update_npt_related_wan_rule(obj, 0);
#endif

    svc_wan_set_info(obj, EVT_WAN_CONN_LOSTV6);

    svc_wan_handle_conn_evt(obj, EVT_WAN_CONN_LOSTV6);

    svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV6_DOWN, obj->path);

	return;
}

static int svc_wan_handle_event_conn_lostv4(char* dev)
{
    wan_entry_obj_t* obj;
    wan_entry_cfg_t* cfg = NULL;
#if defined(TCSUPPORT_NP_CMCC)
	char action[8] = {0};
#endif

    if ((obj = svc_wan_find_object_by_dev(dev)) == NULL)
    {
        SVC_WAN_TRACE_INFO("find [%s] fail \n",dev);
        return -1;
    }

    if ((obj->flag & SVC_WAN_FLAG_UPV4) == 0)
    {
        return 0;
    }
	cfg = &obj->cfg;

	if(SVC_WAN_IS_IPV4_STATIC(cfg))
	{
		return 0;
	}	
	svc_wan_handle_event_conn_lostv4_deal(obj);

#if defined(TCSUPPORT_NP_CMCC)
	/* if ap mode is changed, alink-mgr will be restarted. so we should not send stop msg to alink-mgr */
	cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "APAction", 0, action, sizeof(action));
	if ( 0 != strcmp(action, "Modify") )
		np_send_if6_request(EVT_WAN_CONN_LOSTV4);
	else
		cfg_set_object_attr(WANINFO_COMMON_NODE, "APAction", "");
#endif
    return 0;
}

static int svc_wan_handle_event_conn_lostv6(char* dev)
{
    wan_entry_obj_t* obj;
    char node[32];
	wan_entry_cfg_t* cfg = NULL;
	
    if ((obj = svc_wan_find_object_by_dev(dev)) == NULL)
    {
        SVC_WAN_TRACE_INFO("find [%s] fail \n", dev);
        return -1;
    }

    if ((obj->flag & SVC_WAN_FLAG_UPV6) == 0)
    {
        return 0;
    }

	cfg = &obj->cfg;

	if(SVC_WAN_IS_IPV6_STATIC(cfg))
	{
		return 0;
	}

	svc_wan_handle_event_conn_lostv6_deal(obj);
    return 0;
}

#if defined(TCSUPPORT_CT_UBUS)
int svc_wan_handle_check_wlanload()
{
	int ret = 0;
	char wlanload[4] = {0};
	char workmode[16] = {0};
	
	cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, workmode, sizeof(workmode));
	if ( 0 == workmode[0] )
		return 0;
	else if ( 0 == strcmp(workmode, "repeater") )
	{
		cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", 0, wlanload, sizeof(wlanload));
		if ( 0 != wlanload[0] && 0 == strcmp(wlanload, "1") )
			return 1;
		else
			return 0;
	}
	else
		return 1;
}


enum {
	OPTION_ADD,
	OPTION_DEL,
	OPTION_END
};
int svc_wan_handle_change_br_interface(int opt, char *itf_name)
{
	char cmd[128] = {0};
	
	if ( NULL == itf_name )
		return -1;

	if ( OPTION_ADD == opt )
	{
		/* add br interface */
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/brctl addif br0 %s", itf_name);
		svc_wan_execute_cmd(cmd);
	}
	else if ( OPTION_DEL == opt )
	{
		/* add br interface */
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/brctl delif br0 %s", itf_name);
		svc_wan_execute_cmd(cmd);
	}
	else
		return -1;
}

int svc_wan_handle_switch_multiwan(int opt)
{
	char cmd[SVC_WAN_BUF_128_LEN];
	
	if ( OPTION_ADD == opt )
	{
		/* insmode multiwan*/
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/insmod /lib/modules/multiwan.ko");
		svc_wan_execute_cmd(cmd);
	}
	else if ( OPTION_DEL == opt )
	{
		/* rmmode multiwan*/
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/rmmod /lib/modules/multiwan.ko");
		svc_wan_execute_cmd(cmd);
	}
	else
		return -1;

	return 0;
}

int svc_wan_handle_config_dev_dir(int opt, char *itf_name)
{
	char dir[SVC_WAN_BUF_32_LEN];
	int res = 0;

	if ( NULL == itf_name )
		return -1;

	if ( OPTION_ADD == opt )
	{
		/* create interface folder */
		memset(dir, 0, sizeof(dir));
		snprintf(dir, sizeof(dir), "/var/run/%s", itf_name);

		res = mkdir(dir, 0777); 
		if( res ); /**do nothing,just for compile*/
	}
	else if ( OPTION_DEL == opt )
	{
		;/* do nothing at now */
	}
	else
		return -1;

	return 0;
}

int svc_wan_handle_br_bridge_dhcpc(int opt, char *dev)
{
	char path[SVC_WAN_BUF_32_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char cmd[SVC_WAN_BUF_256_LEN];

	if ( NULL == dev )
		return -1;

	if ( OPTION_ADD == opt )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/udhcpc -i %s -s /usr/script/udhcpc_nodef.sh -p /var/run/%s/udhcpc.pid &", dev, dev);
		svc_wan_execute_cmd(cmd);
	}
	else if ( OPTION_DEL == opt )
	{
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
	}
	else
		return -1;
	
	return 0;
}

int svc_wan_handle_restart_dnsmasq_for_bridge()
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


int svc_wan_handle_switch_default_route(int opt, char *dev)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	char path[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];

	if ( NULL == dev )
		return -1;

	if ( OPTION_ADD == opt )
	{
		/* add default route */
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "/var/run/%s/gateway", dev);
		if( svc_wan_get_file_string(path, info_string, 1) <= 0 )
		{
			return -1;
		}
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s", info_string[0], dev);
		svc_wan_execute_cmd(cmd);
	}
	else if ( OPTION_DEL == opt )
	{
		/* delete default route */
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/ip route delete default");
		svc_wan_execute_cmd(cmd);
	}
	else
		return -1;
	
	return 0;
}

int svc_wan_handle_reconfig_dnsmasq(char *dns)
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

int svc_wan_handle_event_common_update()
{
	char workmode[16] = {0}, currRadio[16] = {0};
	char cmd[128] = {0}, nodeName[32] = {0}, apcli_itf[8] = {0};

	/* stop all configrations on bridge/repeater workmode */
	svc_wan_handle_br_bridge_dhcpc(OPTION_DEL, WAN_ITF_BR0_1);
	svc_wan_handle_config_dev_dir(OPTION_DEL, WAN_ITF_BR0_1);
	svc_wan_handle_change_br_interface(OPTION_DEL, ETHERNET_ITF);
#if 0
	svc_wan_handle_change_br_interface(OPTION_DEL, APCLI_ITF_24G);
	svc_wan_handle_change_br_interface(OPTION_DEL, APCLI_ITF_5G);
#endif
	/* stop apclient link */
	cfg_send_event(EVT_WLAN_INTERNAL, EVT_WLAN_AP_STOP_CLIENT, NULL, 0);

	/* set up apclient interfaces */
	cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, workmode, sizeof(workmode));
	if ( 0 == strcmp(workmode, "repeater") || 0 == strcmp(workmode, "bridge") )
	{
		svc_wan_handle_switch_multiwan(OPTION_DEL);
		if ( 0 == strcmp(workmode, "repeater") )
		{
			cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currRadio, sizeof(currRadio));
			memset(cmd, 0, sizeof(cmd));
			if ( 0 == currRadio[0] || 0 == strcmp(currRadio, "0") )
				snprintf(apcli_itf, sizeof(apcli_itf), "apcli0");
			else if ( 0 == strcmp(currRadio, "1") )
				snprintf(apcli_itf, sizeof(apcli_itf), "apclii0");
			else
				return -1;

			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s up", apcli_itf);
			system(cmd);
			cfg_commit_object(APCLI_COMMON_NODE);
			svc_wan_handle_change_br_interface(OPTION_ADD, apcli_itf);
		}
		else
		{
			svc_wan_handle_change_br_interface(OPTION_ADD, ETHERNET_ITF);
		}

		/* start udhcpc */
		svc_wan_handle_config_dev_dir(OPTION_ADD, WAN_ITF_BR0_1);
		svc_wan_handle_br_bridge_dhcpc(OPTION_ADD, WAN_ITF_BR0_1);
		svc_wan_handle_restart_dnsmasq_for_bridge();
	}
	else
	{
		np_wan_update_proc(NP_BRIDGE_IP_DEF, NP_BRIDGE_MASK_DEF, NP_BRIDGE_GW_DEF, NP_LANHOST_MGR_PROC);
		/* workmode ethernet */
		svc_wan_handle_switch_multiwan(OPTION_ADD);
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 1, 1);
		cfg_commit_object(nodeName);
		/* restart dhcpd, dnsmasq */
		cfg_commit_object(DHCPD_COMMON_NODE);
		cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1");
	}

	return 0;
}

int svc_wan_handle_event_br_bridge_get4(char *dev)
{
	int ret = 0, np_status = 0;
	char wan_state[8] = {0};
	
	char fpath[SVC_WAN_BUF_32_LEN] = {0}, buff[SVC_WAN_BUF_128_LEN] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char dns[16] = {0}, ip[16] = {0}, mask[16] = {0}, gw[16] = {0};
	static int ap_boot = 0;
	
	if ( NULL == dev || 0 != strcmp(dev, WAN_ITF_BR0_1) )
		return -1;

	memset(wan_state, 0, sizeof(wan_state));
	cfg_obj_get_object_attr(WANINFO_BRENTRY_NODE, "Status", 0, wan_state, sizeof(wan_state));
	if ( 0 == strcmp(wan_state, "up") )
	{
		return 0;
	}

	snprintf(fpath, sizeof(fpath), "/var/run/%s", dev);
	svc_wan_set_ipv4_info(WANINFO_BRENTRY_NODE, fpath);
	svc_wan_handle_switch_default_route(OPTION_ADD, dev);
	br_wan_ip_change_inform(dev, EVT_WAN_CONN_GETV4);
	update_br_wan_up_time(EVT_WAN_AP_BRIDGE_GET4);

	/* restart dnsmasq */
	svc_wan_handle_restart_dnsmasq_for_bridge();
	cfg_get_object_attr(WANINFO_BRENTRY_NODE, "DNS", dns, sizeof(dns));
	if ( 0 == dns[0] )
		return -1;
	svc_wan_handle_reconfig_dnsmasq(dns);
	
	/* update lanhostmgr proc */
	cfg_get_object_attr(WANINFO_BRENTRY_NODE, "IP", ip, sizeof(ip));
	if ( 0 == ip[0] )
		strncpy(ip, NP_BRIDGE_IP_DEF, sizeof(ip) - 1);
	cfg_get_object_attr(WANINFO_BRENTRY_NODE, "NetMask", mask, sizeof(mask));
	if ( 0 == mask[0] )
		strncpy(mask, NP_BRIDGE_MASK_DEF, sizeof(mask) - 1);
	cfg_get_object_attr(WANINFO_BRENTRY_NODE, "GateWay", gw, sizeof(gw));
	if ( 0 == gw[0] )
		strncpy(gw, NP_BRIDGE_GW_DEF, sizeof(gw) - 1);
	np_wan_update_proc(ip, mask, gw, NP_LANHOST_MGR_PROC);
	
	svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_UP, dev);
	return 0;
}

int svc_wan_handle_event_br_bridge_lost4(char *dev)
{
	char wan_state[8] = {0};
	
	if ( NULL == dev || 0 != strcmp(dev, WAN_ITF_BR0_1) )
		return -1;

	memset(wan_state, 0, sizeof(wan_state));
	cfg_obj_get_object_attr(WANINFO_BRENTRY_NODE, "Status", 0, wan_state, sizeof(wan_state));
	if ( 0 == strcmp(wan_state, "down") )
	{
		return 0;
	}

	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "Status", "down");
	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "IP", "");
	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "NetMask", "");
	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "DNS", "");
	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "SecDNS", "");
	cfg_set_object_attr(WANINFO_BRENTRY_NODE, "GateWay", "");

	svc_wan_handle_switch_default_route(OPTION_DEL, dev);
	br_wan_ip_change_inform(dev, EVT_WAN_CONN_LOSTV4);
	update_br_wan_up_time(EVT_WAN_AP_BRIDGE_LOST4);
	
	svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_IPV4_DOWN, dev);	
	return 0;
}


int np_wan_update_proc(char *ip, char *mask, char *gateway, char *proc)
{
	char buf[SVC_WAN_BUF_128_LEN] = {0};
	
	if ( NULL == ip || NULL == mask || NULL == gateway || NULL == proc )
		return -1;

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%s %s %s", ip, mask, gateway);
	doValPut(proc, buf);
	return 0;
}

int svc_wan_ubus_wan_boot()
{
	char wlanload[4] = {0}, currRadio[4] = {0};
	char workmode[16] = {0};
	
	cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, workmode, sizeof(workmode));
	if ( 0 == workmode[0] )
		return -1;

	/* insmod np_lanhost_mgr.ko and init proc */
	svc_wan_execute_cmd("/sbin/insmod /lib/modules/"NP_LANHOST_MGR_KO);
	np_wan_update_proc(NP_BRIDGE_IP_DEF, NP_BRIDGE_MASK_DEF, NP_BRIDGE_GW_DEF, NP_LANHOST_MGR_PROC);

	if ( 0 == strcmp(workmode, "repeater") )
	{
		cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", 0, wlanload, sizeof(wlanload));
		if ( 0 != wlanload[0] && 0 == strcmp(wlanload, "1") )
		{
			svc_wan_handle_switch_multiwan(OPTION_DEL);
			cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currRadio, sizeof(currRadio));
			if ( 0 == strcmp(currRadio, "1") )
				svc_wan_handle_change_br_interface(OPTION_ADD, APCLI_ITF_5G);
			else
				svc_wan_handle_change_br_interface(OPTION_ADD, APCLI_ITF_24G);
			svc_wan_handle_config_dev_dir(OPTION_ADD, WAN_ITF_BR0_1);
			svc_wan_handle_br_bridge_dhcpc(OPTION_ADD, WAN_ITF_BR0_1);
			svc_wan_handle_restart_dnsmasq_for_bridge();
		}
		else
			return -1;
	}
	else if ( 0 == strcmp(workmode, "bridge") )
	{
		svc_wan_handle_switch_multiwan(OPTION_DEL);
		svc_wan_handle_change_br_interface(OPTION_ADD, ETHERNET_ITF);
		svc_wan_handle_config_dev_dir(OPTION_ADD, WAN_ITF_BR0_1);
		svc_wan_handle_br_bridge_dhcpc(OPTION_ADD, WAN_ITF_BR0_1);
		svc_wan_handle_restart_dnsmasq_for_bridge();
	}
	else
		svc_wan_start_all_wan_entry(1);

	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", "1");
	return 0;
}



#endif

static void svc_wan_confirm_delete_node(char *buf)
{
	char tmp[16] = {0}, path[64] = {0}, *pos = NULL;

	if ( !buf )
		return;

	/* delete wan node again */
	if ( cfg_obj_query_object(buf, NULL, NULL) > 0 )
	{
		if ( cfg_get_object_attr(buf, "Active", tmp, sizeof(tmp)) < 0 )
		{
			cfg_obj_delete_object(buf);

			bzero(path, sizeof(path));
			snprintf(path, sizeof(path), "%s", buf);
			if ( NULL != (pos = strstr(path, ".entry.")) )
				*pos = 0;
			if ( cfg_obj_query_object(path, "entry", NULL) <= 0 )
				cfg_obj_delete_object(path);
		}
	}
	return;
}
int svc_wan_mgr_handle_event(int event, char* buf)
{
    int ret = 0;
    int debug_level = E_NO_INFO_LEVEL;
    wan_entry_obj_t* obj;
    SVC_WAN_CRITIC_INFO("svc_wan_mgr_handle_event: event [%d] [%s]  \n", event,buf);	

    switch (event)
    {
   	case EVT_CFG_WAN_ENTRY_BOOT:
        {
			pre_check_lanport_bind();
#if defined(TCSUPPORT_WAN_ETHER)
            check_ether_wan_state();
#endif	
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
			check_Active_ether_wan_state();
#endif	
#if defined(TCSUPPORT_NP_CMCC)
		np_wan_ap_boot();
#else		
#if defined(TCSUPPORT_CT_UBUS)
            svc_wan_ubus_wan_boot();
#else
	    svc_wan_start_all_wan_entry(1);
#if defined(TCSUPPORT_CT_DSL_EX)	
            /*start all wan interface for dsl */
		svc_wan_start_all_wan_entry(0);
#endif
           cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", "1");
#endif
            break;
        }
   	 case EVT_CFG_WAN_ENTRY_BOOT2:
	{	
#if !defined(TCSUPPORT_CT_DSL_EX)	
	    svc_wan_start_all_wan_entry(0);
#endif
#endif
            break;
        }
        case EVT_CFG_WAN_ENTRY_DELETE:
        {
            svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_ENTRY_DELETE, buf);
            ret = svc_wan_handle_event_delete(buf);
            svc_wan_confirm_delete_node(buf);
#if defined(TCSUPPORT_CT_JOYME4)
            svc_wan_update_other_bridge_index();
#endif

            break;
        }
        case EVT_CFG_WAN_ENTRY_UPDATE:
        {
            svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_ENTRY_UPDATE, buf);
            ret = svc_wan_handle_event_update(buf);
#if defined(TCSUPPORT_CT_JOYME4)
            svc_wan_update_other_bridge_index();
#endif
            break;
        }
#if defined(TCSUPPORT_CT_JOYME4)
        case EVT_CFG_WANCFGDATA_ENTRY_UPDATE:
        {
            ret = svc_wan_datacfg_event_update(buf);
            break;
        }
        case EVT_CFG_WAN_BSSADDR_UPDATE:
        {
			ret = svc_wan_update_bssaddr_vip();
            break;
        }
#endif
        case EVT_XPON_UP:
        {
#if defined(TCSUPPORT_NP_CMCC)
			if ( np_wan_pre_handle_xpon_update(EVT_XPON_UP, buf) < 0 )
			{
				break;
			}
#endif
			svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_XPON_UP, buf);
            ret = svc_wan_handle_xpon_event(EVT_XPON_UP);
            break;
        }
        case EVT_XPON_DOWN:
        {
#if defined(TCSUPPORT_NP_CMCC)
			if ( np_wan_pre_handle_xpon_update(EVT_XPON_DOWN, buf) < 0 )
			{
				break;
			}
#endif
			svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_XPON_DOWN, buf);
            ret = svc_wan_handle_xpon_event(EVT_XPON_DOWN);
            break;
        }
        case EVT_WAN_CONN_GETV4:
        {
            ret = svc_wan_handle_event_conn_getv4(buf);
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            ret = svc_wan_handle_event_conn_getv6(buf);
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
            ret = svc_wan_handle_event_conn_lostv4(buf);
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            ret = svc_wan_handle_event_conn_lostv6(buf);
            break;
        }
        case EVT_DUMP_WAN_SRV_ENTRY_DEV:
        case EVT_DUMP_WAN_SRV_ENTRY_PATH:
        case EVT_DUMP_WAN_SRV_ALL_ENTRY:
        {
            msg_dump_svc_wan_cfg(buf, event);
            break;
        }
        case EVT_WAN_SRV_READY_IPV6_GATEWAY:
        {
            update_ipv6_slaac_addr_and_gateway();
            break;
        }
        case EVT_CFG_WAN_VLANBIND_UPDATE:
        {
			obj = svc_wan_find_object_by_dev(buf);
            svc_wan_vlanbind_execute(obj,1);
            break;
        }
        case EVT_CFG_WAN_VETHPAIR_UPDATE:
        {
            svc_wan_vethpair_execute(buf);
            break;
        }
        case EVT_WAN_SRV_DEBUG_LEVEL:
        {
            debug_level = atoi(buf);
            svc_wan_debug_level(debug_level);
            break;
        }
		case EVT_WAN_UPDATE_DNSINFO:
		{
			restart_dnsmasq();
            break;
		}
		case EVT_WAN_RENEW6:
		{
			obj = svc_wan_find_object_by_dev(buf);
			if(obj == NULL){				
				break;
			}
			
			svc_wan_start_ipv6_dhcp(obj);
            break;
		}
		case EVT_WAN_RELEASE6:
		{
			obj = svc_wan_find_object_by_dev(buf);	
			if(obj == NULL){
				break;
			}

			svc_wan_stop_ipv6_dhcp(obj);
            break;
		}
#if defined(TCSUPPORT_NP_CMCC)
		case EVT_WAN_AP_BRIDGE_GET4:
		{
			ret = np_wan_ap_br_alink_get4(buf);
			break;
		}
		case EVT_WAN_AP_BRIDGE_LOST4:
		{
			ret = np_wan_ap_br_alink_lost4(buf);
			break;
		}
		case EVT_WAN_AP_STATUS_UPDATE:
		{
			ret = np_event_handle(atoi(buf));
			break;
		}
		case EVT_WAN_AP_START_BRIDGE:
		{
			ret = np_wan_start_ap_bridge();
			break;
		}
		case EVT_WAN_AP_START_CLIENT:
		{
			ret = np_wan_start_ap_client();
			break;
		}
		case EVT_WAN_AP_MODE_UPDATE:
		{
			ret = np_wan_ap_mode_update(buf);
			break;
		}
		case EVT_WAN_AP_LINK_UPDATE:
		{
			ret = np_wan_ap_link_update(atoi(buf));
			break;
		}
#endif
#if defined(TCSUPPORT_CT_SDN)		
		case EVT_CFG_WAN_SDN_UPDATE:
		{
			ret = svc_wan_sdn_update();
			break;
		}
#endif
#if defined(TCSUPPORT_CT_UBUS)
		case EVT_CFG_WAN_COMMON_UPDATE:
		{
			ret = svc_wan_handle_event_common_update();
			break;
		}
		case EVT_WAN_AP_BRIDGE_GET4:
		{
			ret = svc_wan_handle_event_br_bridge_get4(buf);
			break;
		}
		case EVT_WAN_AP_BRIDGE_LOST4:
		{
			ret = svc_wan_handle_event_br_bridge_lost4(buf);
			break;
		}
#endif

        default:
        {
            SVC_WAN_ERROR_INFO("svc_wan_mgr_handle_event: buf[%s]  event [%d]  \n", buf, event);
            break;
        }
    }

    return ret;
}

int svc_wan_start_all_wan_entry(int isInternet)
{
    int pvc[SVC_WAN_MAX_PVC_NUM],entry[SVC_WAN_MAX_ENTRY_NUM];
    char node[32];
    int i,j,pvcnum,entrynum;
    char tmp[64] = {0};
    char internet_not_first[12] = {0};
    int internetnotfirst = 0;

    if(cfg_get_object_attr(WAN_COMMON_NODE, "InternetNotFirst", internet_not_first, sizeof(internet_not_first)) > 0)
        internetnotfirst = atoi(internet_not_first);

    if(internetnotfirst && !isInternet)
        return 0;

    memset(tmp, 0, sizeof(tmp));

    pvcnum = cfg_query_object(WAN_NODE,"pvc",pvc);

    for(i=0; i< pvcnum; i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WAN_PVC_NODE,pvc[i]);

        entrynum = cfg_query_object(node,"entry",entry);


        for(j=0;j < entrynum; j++)
        {
            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WAN_PVC_ENTRY_NODE,pvc[i],entry[j]);
            /*update event*/
			
            if(internetnotfirst)
            {
		svc_wan_mgr_handle_event(EVT_CFG_WAN_ENTRY_UPDATE, node);
            }
            else
            {
		cfg_get_object_attr(node,SVC_WAN_ATTR_SRV_LIST,tmp,sizeof(tmp));
			
	        if(isInternet && strstr(tmp,"INTERNET"))
	        {
                    svc_wan_mgr_handle_event(EVT_CFG_WAN_ENTRY_UPDATE, node);
	        }
		else if(!isInternet && !strstr(tmp,"INTERNET"))
	        {
		    svc_wan_mgr_handle_event(EVT_CFG_WAN_ENTRY_UPDATE, node);
	        }
            }
        }
    }

    return 0;
}

int svc_wan_stop_all_wan_entry(void)
{
    wan_entry_obj_t* tmp = NULL;
    wan_entry_obj_t* obj = NULL;
	
	obj = svc_wan_get_obj_list();
	
    while(obj != NULL)
    {
        tmp = obj;
        obj = obj->next;
        svc_wan_stop_intf(tmp);
		svc_wan_del_from_list(tmp);
    }

    svc_wan_init_obj_list();

    return 0;
}
/**************************************************************************/
int svc_wan_update_opt_cfg(wan_entry_obj_t* obj)
{

    char tmp[SVC_WAN_BUF_64_LEN];
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t* cfg = &obj->cfg;
    int val;

    memset(tmp, 0, sizeof(tmp));
    if (SVC_WAN_IS_IPV4(cfg) && cfg_get_object_attr(obj->path,SVC_WAN_ATTR_NAT, tmp, sizeof(tmp)) > 0)
    {
        if (strstr(tmp,"Enable") != NULL)
        {
            val = 1;
        }
        else
        {
            val = 0;
        }

        if (obj->nat != val)
        {
            memset(itf_name, 0, sizeof(itf_name));
            svc_wan_intf_name(obj, itf_name, sizeof(itf_name));
            if (obj->nat)
            {
                svc_wan_stop_intf_nat(itf_name);
            }
            else
            {
                svc_wan_start_intf_nat(itf_name);
            }
        }
        
        obj->nat = val;
    }

    return 0;
}

static int bridge_up_down(wan_entry_obj_t* obj, int evt)
{
	char cmd[SVC_WAN_BUF_256_LEN];
	int pvcIndex = 0;
	int entryIndex = 0;
	char nodePath[32] = {0};
	if(get_entry_number_cfg2(obj->path, "pvc.", &pvcIndex) == 0  && get_entry_number_cfg2(obj->path, "entry.", &entryIndex) == 0)
	{	
		snprintf(nodePath, sizeof(nodePath), WANINFO_ENTRY_NODE,(pvcIndex - 1)*MAX_WAN_ENTRY_NUMBER + entryIndex);
		if (cfg_query_object(nodePath,NULL,NULL) <= 0)
		{
			cfg_create_object(nodePath);
		}
	}

    if(EVT_WAN_BRIDGE_UP == evt)
    {
	  cfg_set_object_attr(nodePath, "Status", "up");
		
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -A INPUT -i %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", EBTABLES_FILTER, obj->dev);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -A INPUT -i %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", EBTABLES_FILTER, obj->dev);
        svc_wan_execute_cmd(cmd);

        svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_BRIDGE_UP, obj->path);
    }
    else if(EVT_WAN_BRIDGE_DOWN == evt)
    {
    	  cfg_set_object_attr(nodePath, "Status", "down");
		  
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D INPUT -i %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP 2>/dev/null", EBTABLES_FILTER, obj->dev);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D INPUT -i %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP 2>/dev/null", EBTABLES_FILTER, obj->dev);
        svc_wan_execute_cmd(cmd);

        svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_BRIDGE_DOWN, obj->path);
    }

    return 0;
}

static int svc_wan_update_tr69_vip(wan_entry_obj_t* obj, int evt)
{
		wan_entry_cfg_t* cfg = NULL;
		char node[SVC_WAN_BUF_32_LEN];
		char ip_addr[MAX_WAN_IPV4_ADDR_LEN];
		struct in_addr in_s_v4addr = {0};
		static unsigned int ip_hex_last = 0;
		int ret = 0;
		ifc_param_t ifc_data;
		char conReqPort[8] = {0};
		unsigned short iconReqPort = 0;
	
		cfg = &obj->cfg;
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);

		if((cfg->srv_type == E_SRV_TR069) 
			|| (cfg->srv_type == E_SRV_E8C_TR069) 
			|| (cfg->srv_type == (E_SRV_TR069|E_SRV_E8C_TR069)))
		{
			memset(conReqPort, 0, sizeof(conReqPort));
			cfg_get_object_attr(CWMP_ENTRY_NODE, "conReqPort", conReqPort, sizeof(conReqPort));
			if ( 0 == conReqPort[0] )
				strcpy(conReqPort, "7547");
			iconReqPort = strtoul(conReqPort, NULL, 10);

			if(EVT_WAN_CONN_GETV4 == evt)
			{
				memset(ip_addr, 0, sizeof(ip_addr));
				if( (cfg_get_object_attr(node, "IP", ip_addr, sizeof(ip_addr)) < 0)  || (0 >= strlen(ip_addr)) )
					return 0;

				inet_pton(AF_INET, ip_addr, &in_s_v4addr);
				
				ip_hex_last = htonl(in_s_v4addr.s_addr);

				memset(&ifc_data, 0, sizeof(ifc_data));
				ifc_data.dip = htonl(in_s_v4addr.s_addr);
				ifc_data.dport = iconReqPort;
				ifc_data.ifc_type = DIP_DPORT_CHG;
				blapi_traffic_update_ifc_vip_params(IFC_ADD, &ifc_data);

				memset(&ifc_data, 0, sizeof(ifc_data));
				ifc_data.sip = htonl(in_s_v4addr.s_addr);
				ifc_data.sport = iconReqPort;
				ifc_data.ifc_type = SIP_SPORT_CHG;
				blapi_traffic_update_ifc_vip_params(IFC_ADD, &ifc_data);
			}
			else if(EVT_WAN_CONN_LOSTV4 == evt)
			{
				if(ip_hex_last)
				{
					memset(&ifc_data, 0, sizeof(ifc_data));
					ifc_data.dip = ip_hex_last;
					ifc_data.dport = iconReqPort;
					ifc_data.ifc_type = DIP_DPORT_CHG;
					blapi_traffic_update_ifc_vip_params(IFC_DEL, &ifc_data);

					memset(&ifc_data, 0, sizeof(ifc_data));
					ifc_data.sip = ip_hex_last;
					ifc_data.sport = iconReqPort;
					ifc_data.ifc_type = SIP_SPORT_CHG;
					blapi_traffic_update_ifc_vip_params(IFC_DEL, &ifc_data);
				}
			}
		}

		return 0;
}

int svc_wan_handle_conn_evt(wan_entry_obj_t* obj,int evt)
{
    char itf_name[16];
#if defined(TCSUPPORT_CT_SDN)
	char rdata[128] = {0},nodeName[64] = {0},guiname[64] = {0};
	int portid = -1;
#endif
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_IPOE_DETECT)
	wan_entry_cfg_t* cfg = &obj->cfg;
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
	int index = -1, ipoediag_start= -1;
	char tmp[32] = {0}, filepath[32] = {0};
	struct stat fstat;
#endif


#if defined(TCSUPPORT_CT_JOYME4)
	int is_def_route_wan = 0;
#endif

#if defined(TCSUPPORT_CT_IPOE_DETECT)
	if( SVC_WAN_SRV_VR(cfg) )
	{
		cfg_get_object_attr(obj->path, "ipoediag_index", tmp, sizeof(tmp));
		index = atoi(tmp);
		snprintf(filepath, sizeof(filepath), "/var/run/ipoediag_%d", index);
		if( 0 == stat(filepath,&fstat) )
			ipoediag_start = 1;
	}
#endif
    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj,itf_name, sizeof(itf_name));

#if defined(TCSUPPORT_CT_DNSBIND)
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
    set_policy_route(obj, evt);
#endif
#endif

    /*update wan time*/
    update_wan_up_time(obj, evt);

    switch (evt)
    {
        case EVT_WAN_BRIDGE_UP:
        {
            bridge_up_down(obj, EVT_WAN_BRIDGE_UP);
            break;
        }
        case EVT_WAN_BRIDGE_DOWN:
        {
            bridge_up_down(obj, EVT_WAN_BRIDGE_DOWN);
            break;
        }
        case EVT_WAN_CONN_GETV4:
        {
            if(obj->nat)
            {
                svc_wan_start_intf_nat(itf_name);
            }
#ifdef TCSUPPORT_PORTBIND
#if defined(TCSUPPORT_CT_SDN)
            if(obj->cfg.sdn != 1)
            {
#endif
         	set_port_binding_info(obj, 0, PORT_BIND_OP_V4);
#if defined(TCSUPPORT_CT_SDN)
            }
#endif
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
			delRoute4ViaOndemand((obj->idx)/SVC_WAN_MAX_PVC_NUM, (obj->idx)%SVC_WAN_MAX_ENTRY_NUM, itf_name);
#endif
            update_default_route(obj->path, EVT_WAN_CONN_GETV4);
            check_route(obj->path);

#if defined(TCSUPPORT_CT_PHONEAPP)
            sendfifo2mobile(obj->idx);
#endif

            misc_srv_manage(obj->path, EVT_WAN_CONN_GETV4);

#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
            wan_ip_change_inform(obj->path, EVT_WAN_CONN_GETV4);
#endif
            restart_dnsmasq();

		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "WanStatus", "1");
	
		svc_wan_update_tr69_vip(obj,evt);
#if defined(TCSUPPORT_CT_IPOE_DETECT)
			if( SVC_WAN_SRV_VR(cfg) && index >= 0 && ipoediag_start <= 0)
				svc_other_handle_event_ipoe_update(index + 1);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			if ( checkIsDefRouteIPv4Wan(obj->idx) )
			{
				cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", "1");
				cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", "0");
				cfg_set_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", "0");
			}
#endif
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {		
#if defined(TCSUPPORT_CT_JOYME4)
			is_def_route_wan = checkIsDefRouteIPv4Wan(obj->idx);
#endif		
            if(obj->nat)
            {
                svc_wan_stop_intf_nat(itf_name);
            }
#ifdef TCSUPPORT_PORTBIND
            set_port_binding_info(obj, 0, PORT_BIND_OP_V4);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			if ( SVC_WAN_IS_PPP_HYBRID(cfg) )
				stop_pppoe_relay(obj);
#endif
            update_default_route(obj->path, EVT_WAN_CONN_LOSTV4);
            misc_srv_manage(obj->path, EVT_WAN_CONN_LOSTV4);
#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
            wan_ip_change_inform(obj->path, EVT_WAN_CONN_LOSTV4);
#endif

		svc_wan_update_tr69_vip(obj,evt);

#if defined(TCSUPPORT_CT_JOYME4)
			if ( is_def_route_wan )
			{
				cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", "1");
				cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", "0");
				cfg_set_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", "0");
			}
#endif
#if defined(TCSUPPORT_CUC)
			restart_dnsmasq();
#endif
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            check_static_route6(obj->path);

#ifdef TCSUPPORT_PORTBIND
            set_port_binding_info(obj, 0, PORT_BIND_OP_V6);
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
			delRoute6ViaOndemand((obj->idx)/SVC_WAN_MAX_PVC_NUM, (obj->idx)%SVC_WAN_MAX_ENTRY_NUM, itf_name);
#endif
            update_default_route(obj->path, EVT_WAN_CONN_GETV6);

            update_pd_info(obj, EVT_WAN_CONN_GETV6);

            misc_srv_manage(obj->path, EVT_WAN_CONN_GETV6);

#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2)
            wan_ip_change_inform(obj->path, EVT_WAN_CONN_GETV6);
#endif

            restart_dnsmasq();

		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "WanStatus", "1");

#if defined(TCSUPPORT_CT_IPOE_DETECT)
			if( SVC_WAN_SRV_VR(cfg) && index >= 0 && ipoediag_start <= 0)
				svc_other_handle_event_ipoe_update(index + 1);
#endif

            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
#ifdef TCSUPPORT_PORTBIND
            set_port_binding_info(obj, 0, PORT_BIND_OP_V6);
#endif	
#if defined(TCSUPPORT_CT_JOYME4)
			if ( SVC_WAN_IS_PPP_HYBRID(cfg) )
				stop_pppoe_relay(obj);
#endif
            update_default_route(obj->path, EVT_WAN_CONN_LOSTV6);

            update_pd_info(obj, EVT_WAN_CONN_LOSTV6);

            misc_srv_manage(obj->path, EVT_WAN_CONN_LOSTV6);
			
#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2)
            wan_ip_change_inform(obj->path, EVT_WAN_CONN_LOSTV6);
#endif
#if defined(TCSUPPORT_CT_DSLITE)
			stop_dslite(obj);
#endif
#if defined(TCSUPPORT_CUC)
			restart_dnsmasq();
#endif
            break;
        }
        
        default:
        {
            break;
        }
    }

    return 0;
}

#if defined(TCSUPPORT_WAN_ETHER)
void check_ether_wan_state(void)
{
	int adsl_status = 0;
	char transMode[16] = {0};
	char ethernetState[8] = {0};
	char eth_portmap[32] = {0};
	char eth_portvlan[32] = {0};
	int ret = 0;
	
	memset(transMode, 0, sizeof(transMode));
	if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode))
			&& 0 == strcmp(transMode, "Ethernet"))/*Ethernet Mode*/
	{
		/*These two operations of set node should be removed*/
		cfg_set_object_attr(DEVICEACCOUNT_ENTRY_NODE, "registerStatus", "0");
		cfg_set_object_attr(DEVICEACCOUNT_ENTRY_NODE, "registerResult", "1");
		cfg_commit_object(DEVICEACCOUNT_ENTRY_NODE);

		cfg_obj_get_object_attr(SYS_ENTRY_NODE, "eth_port_vlan", 0, eth_portvlan, sizeof(eth_portvlan));
		cfg_obj_get_object_attr(SYS_ENTRY_NODE, "eth_portmap", 0, eth_portmap, sizeof(eth_portmap));
		if(strlen(eth_portmap) > 0)
			ret = blapi_traffic_set_switch_portmap(eth_portmap, eth_portvlan);
		else
			ret = blapi_traffic_set_default_switch_portmap();
		adsl_status = is_ethernet_link_up();
		if(adsl_status == 1)
		{
			wan_mgr_update_status(E_WAN_PON_UP);
		}
	}

	return;
}
#endif

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
void check_Active_ether_wan_state(void)
{
	int adsl_status = 0;
	char transMode[16] = {0};
	int ret = 0;

	memset(transMode, 0, sizeof(transMode));
	if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode))
			&&  0 == strcmp(transMode, "ActiveEtherWan") )  /*Acvive Ether Wan Mode*/
	{
		/*These two operations of set node should be removed*/
		cfg_set_object_attr(DEVICEACCOUNT_ENTRY_NODE, "registerStatus", "0");
		cfg_set_object_attr(DEVICEACCOUNT_ENTRY_NODE, "registerResult", "1");
		cfg_commit_object(DEVICEACCOUNT_ENTRY_NODE);
 
		adsl_status = is_ActiveEtherWan_link_up();
		if(adsl_status == 1)
		{
			wan_mgr_update_status(E_WAN_PON_UP);
		}
	}

	return;
}
#endif


