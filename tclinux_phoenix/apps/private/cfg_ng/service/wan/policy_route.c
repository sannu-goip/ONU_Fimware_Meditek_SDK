#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <cfg_api.h>
#include <svchost_api.h> 
#include "wan_link_list.h"
#include "port_binding.h"
#include "policy_route.h"
#include "utility.h"
#if defined(TCSUPPORT_CT_SDN)
#include "sdn_ovs.h"
#endif

#if 0
static char is_default_route(int index)
{
    char node[SVC_WAN_BUF_32_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    int  def_route_index = -1;
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "root.waninfo.common");
    memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
    if(cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) < 0)
    {
        return FALSE;
    }

    if(0 != strcmp(def_route_index_v4, "N/A"))
    {
        def_route_index = atoi(def_route_index_v4);
    }

    return index == def_route_index;
}
#endif


void set_policy_route(wan_entry_obj_t* obj, int evt)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char if_name[MAX_WAN_DEV_NAME_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char w_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char mask_dec[SVC_WAN_BUF_8_LEN];
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN];
    char w_ip_v6[MAX_WAN_IPV6_ADDR_LEN];
	char ip_pd6[SVC_WAN_BUF_64_LEN];
	char temp[SVC_WAN_BUF_8_LEN] = {0};
	char PrefixLen6[SVC_WAN_BUF_8_LEN] = {0};
    unsigned int mark;
    struct in_addr ip_v4_addr;
    struct in_addr netmask_v4_addr;
    wan_entry_cfg_t* cfg = NULL;

    if(NULL == obj)
    {
        return ;
    }

    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));

    cfg = &obj->cfg;
    mark = (obj->idx + 1) << 16; 
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
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
            if(cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) > 0)
            {
                dealwith_link_local_route(if_name, obj->idx, E_DEFAULT_ROUTE_SWITCH);
                memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_SDN)
                if(cfg->sdn == 1)
                {
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s table %d", \
	                gate_way_v4, if_name, SDN_POLICY_TABLE + obj->idx);
                }
                else
#endif
                {
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s table %d", \
	                gate_way_v4, if_name, 100 + obj->idx);
                }				
				cfg_set_object_attr(obj->path, "GW_last", gate_way_v4) ;
                svc_wan_execute_cmd(cmd);
                dealwith_link_local_route(if_name, obj->idx, E_NOT_DEFAULT_ROUTE);
            }

            if(SVC_WAN_ATTR_LINKMODE_IP == cfg->linkmode)
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);

                memset(ip_v4, 0, sizeof(ip_v4));
                memset(netmask_v4, 0, sizeof(netmask_v4));
                if((cfg_get_object_attr(node, "IP", ip_v4, sizeof(ip_v4)) > 0) \
                    && (cfg_get_object_attr(node, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
                {
                    if ((strlen(netmask_v4) > 0) && (strlen(ip_v4) > 0))
                    {
                        if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
                        {
                            return ;
                        }
                        inet_aton(ip_v4, &ip_v4_addr);
                        inet_aton(netmask_v4, &netmask_v4_addr);
                        ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);

                        memset(ip_netmask_v4, 0, sizeof(ip_netmask_v4));
                        snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));

                        memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_SDN)
                        if(cfg->sdn == 1)
                        {
                            snprintf(cmd,  sizeof(cmd), "/usr/bin/ip route add %s/%s dev %s table %d", \
                                ip_netmask_v4, mask_dec, if_name, SDN_POLICY_TABLE + obj->idx);
                        }
                        else
#endif
                        {
                        snprintf(cmd,  sizeof(cmd), "/usr/bin/ip route add %s/%s dev %s table %d", \
                            ip_netmask_v4, mask_dec, if_name, 100 + obj->idx);
                        }
                        svc_wan_execute_cmd(cmd);
                    }
                }
            }

            if(SVC_WAN_IS_ROUTE(cfg))
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
                memset(w_ip_v4, 0, sizeof(w_ip_v4));
                if(cfg_get_object_attr(node, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0)
                {
                    memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_SDN)
                    if(cfg->sdn == 1)
                    {
                        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule add iif ovs_wan%d table %d",obj->idx, SDN_POLICY_TABLE + obj->idx);
                    }
                    else
#endif
                    {
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add from %s/32 table %d", w_ip_v4, 100 + obj->idx);
                    }
                    svc_wan_execute_cmd(cmd);
                    memset(node, 0, sizeof(node));
                    snprintf(node, sizeof(node), "%s", LAN_ENTRY0_NODE);

                    memset(ip_v4, 0, sizeof(ip_v4));
                    memset(netmask_v4, 0, sizeof(netmask_v4));
                    if((cfg_get_object_attr(node, "IP", ip_v4, sizeof(ip_v4)) > 0) \
                        && (cfg_get_object_attr(node, "netmask", netmask_v4, sizeof(netmask_v4)) > 0))
                    {
                        if ((strlen(ip_v4) > 0) && (strlen(netmask_v4) > 0))
                        {
                            if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
                            {
                                snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
                            }
                            inet_aton(ip_v4, &ip_v4_addr);
                            inet_aton(netmask_v4, &netmask_v4_addr);
                            ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
                            snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
                            memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_SDN)
                            if(cfg->sdn == 1)
                            {
                                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev ovs_wan%d table %d", \
                                    ip_netmask_v4, mask_dec, ip_v4, obj->idx,SDN_POLICY_TABLE + obj->idx);
                            }
                            else							
#endif
                            {
                                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev br0 table %d", \
                                    ip_netmask_v4, mask_dec, ip_v4, 100 + obj->idx);
                            }
                        }
                    }
                    else
                    {
                        memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_SDN)
                        if(cfg->sdn == 1)
                        {
                            snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add 192.168.1.0/24 via 192.168.1.1 dev ovs_wan%d table %d", \
                            obj->idx,SDN_POLICY_TABLE + obj->idx);
                        }
                        else
#endif
                        {
                        snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add 192.168.1.0/24 via 192.168.1.1 dev br0 table %d", \
                            100 + obj->idx);
                        }
                    }

                    svc_wan_execute_cmd(cmd);
                    cfg_set_object_attr(obj->path, "IP_last", w_ip_v4);
                    memcpy(obj->ip_last, w_ip_v4, sizeof(obj->ip_last));
					
					cfg_get_object_attr(WAN_COMMON_NODE, "dhcp_release", temp, sizeof(temp)); 
					if(!strcmp(temp, "1")){
						cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_TCAPI_SAVE, NULL, 0);
					}
                }
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
            memcpy(ip_v4, obj->ip_last, sizeof(ip_v4));
            if(strlen(ip_v4) > 0)
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del from %s/32 table %d", ip_v4, 100 + obj->idx);
                svc_wan_execute_cmd(cmd);
            }
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush table %d", 100 + obj->idx);
            svc_wan_execute_cmd(cmd);
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule add fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
            memset(gate_way_v6, 0, sizeof(gate_way_v6));
            if(cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6)) > 0)
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add default via %s dev %s table %d", \
                    gate_way_v6, if_name, 100 + obj->idx);
                svc_wan_execute_cmd(cmd);
            }
			
			 memset(ip_pd6, 0, sizeof(ip_pd6));
            if ( cfg_get_object_attr(node, "PD6", ip_pd6, sizeof(ip_pd6)) > 0
				&& '\0' != ip_pd6[0] )
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add %s dev br0 table %d",
                           ip_pd6, 100 + obj->idx);
                svc_wan_execute_cmd(cmd);
            }
			
            bzero(w_ip_v6, sizeof(w_ip_v6));
            if ( cfg_get_object_attr(node, "IP6", w_ip_v6, sizeof(w_ip_v6)) > 0
				&&  '\0' != w_ip_v6[0] )
            {

				/*add ipv6 link route*/
				bzero(PrefixLen6, sizeof(PrefixLen6));
				if(!SVC_WAN_IS_PPP(cfg) && (cfg_get_object_attr(node, "PrefixLen6", PrefixLen6, sizeof(PrefixLen6))> 0
					&& '\0' != PrefixLen6[0]))
				{
					bzero(cmd, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add %s/%s dev %s table %d",
								w_ip_v6, PrefixLen6, if_name, 100 + obj->idx);
					svc_wan_execute_cmd(cmd);
				}
			

				if(0 == obj->ip6_last[0])
				{
	                bzero(cmd, sizeof(cmd));
	                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule add from %s/128 table %d"
	                          , w_ip_v6, 100 + obj->idx);
	                svc_wan_execute_cmd(cmd);
	                cfg_set_object_attr(obj->path, "IP6_last", w_ip_v6);
	                memcpy(obj->ip6_last, w_ip_v6, sizeof(obj->ip6_last));
				}
            }
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route flush table %d", 100 + obj->idx);
            svc_wan_execute_cmd(cmd);

            bzero(w_ip_v6, sizeof(w_ip_v6));
            memcpy(w_ip_v6, obj->ip6_last, sizeof(w_ip_v6));
            if ( 0 != w_ip_v6[0] )
            {
                bzero(cmd, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del from %s/128 table %d"
					, w_ip_v6, 100 + obj->idx);
                svc_wan_execute_cmd(cmd);
				bzero(obj->ip6_last, sizeof(obj->ip6_last));
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return;
}

void set_policy_route_vlan_mode(wan_entry_obj_t* obj, int evt)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char if_name[MAX_WAN_DEV_NAME_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char w_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char mask_dec[SVC_WAN_BUF_8_LEN];
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN];
    char w_ip_v6[MAX_WAN_IPV6_ADDR_LEN];
	char ip_pd6[SVC_WAN_BUF_64_LEN];

    unsigned int mark;
    struct in_addr ip_v4_addr;
    struct in_addr netmask_v4_addr;
    wan_entry_cfg_t* cfg = NULL;

    if(NULL == obj)
    {
        return ;
    }
    if(NULL == strstr(obj->cfg.srv_list,"TR069") && NULL == strstr(obj->cfg.srv_list,"VOICE"))
    {
        return;
    }
    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));

    cfg = &obj->cfg;
    mark = (obj->idx + 1) << 16; 
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
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
            if(cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) > 0)
            {
                dealwith_link_local_route(if_name, obj->idx, E_DEFAULT_ROUTE_SWITCH);
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s table %d", \
                gate_way_v4, if_name, VLAN_TABLE_BASE + obj->idx);
                svc_wan_execute_cmd(cmd);
                dealwith_link_local_route(if_name, obj->idx, E_NOT_DEFAULT_ROUTE);
            }

            if(SVC_WAN_ATTR_LINKMODE_IP == cfg->linkmode)
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);

                memset(ip_v4, 0, sizeof(ip_v4));
                memset(netmask_v4, 0, sizeof(netmask_v4));
                if((cfg_get_object_attr(node, "IP", ip_v4, sizeof(ip_v4)) > 0) \
                    && (cfg_get_object_attr(node, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
                {
                    if ((strlen(netmask_v4) > 0) && (strlen(ip_v4) > 0))
                    {
                        if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
                        {
                            return ;
                        }
                        inet_aton(ip_v4, &ip_v4_addr);
                        inet_aton(netmask_v4, &netmask_v4_addr);
                        ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);

                        memset(ip_netmask_v4, 0, sizeof(ip_netmask_v4));
                        snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));

                        memset(cmd, 0, sizeof(cmd));
                        snprintf(cmd,  sizeof(cmd), "/usr/bin/ip route add %s/%s dev %s table %d", \
                            ip_netmask_v4, mask_dec, if_name, VLAN_TABLE_BASE+ obj->idx);
                        svc_wan_execute_cmd(cmd);
                    }
                }
            }

            if(SVC_WAN_IS_ROUTE(cfg))
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
                memset(w_ip_v4, 0, sizeof(w_ip_v4));
                if(cfg_get_object_attr(node, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0)
                {
                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add from %s/32 table %d", w_ip_v4, VLAN_TABLE_BASE + obj->idx);
                    svc_wan_execute_cmd(cmd);

                    cfg_set_object_attr(obj->path, "IP_last", w_ip_v4);
                    memcpy(obj->ip_last, w_ip_v4, sizeof(obj->ip_last));
                }
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
            memcpy(ip_v4, obj->ip_last, sizeof(ip_v4));
            if(strlen(ip_v4) > 0)
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del from %s/32 table %d", ip_v4, VLAN_TABLE_BASE + obj->idx);
                svc_wan_execute_cmd(cmd);
            }
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush table %d", VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule add fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
            memset(gate_way_v6, 0, sizeof(gate_way_v6));
            if(cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6)) > 0)
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add default via %s dev %s table %d", \
                    gate_way_v6, if_name, VLAN_TABLE_BASE + obj->idx);
                svc_wan_execute_cmd(cmd);
            }
			
			
            bzero(w_ip_v6, sizeof(w_ip_v6));
            if ( cfg_get_object_attr(node, "IP6", w_ip_v6, sizeof(w_ip_v6)) > 0
				&& 0 == obj->ip6_last[0] )
            {
                bzero(cmd, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule add from %s/128 table %d"
                          , w_ip_v6, VLAN_TABLE_BASE + obj->idx);
                svc_wan_execute_cmd(cmd);
                cfg_set_object_attr(obj->path, "IP6_last", w_ip_v6);
                memcpy(obj->ip6_last, w_ip_v6, sizeof(obj->ip6_last));
            }
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del fwmark 0x%x/0x%x table %d", mark, WAN_IF_MARK, VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route flush table %d", VLAN_TABLE_BASE + obj->idx);
            svc_wan_execute_cmd(cmd);

            bzero(w_ip_v6, sizeof(w_ip_v6));
            memcpy(w_ip_v6, obj->ip6_last, sizeof(w_ip_v6));
            if ( 0 != w_ip_v6[0] )
            {
                bzero(cmd, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del from %s/128 table %d"
					, w_ip_v6, VLAN_TABLE_BASE + obj->idx);
                svc_wan_execute_cmd(cmd);
				bzero(obj->ip6_last, sizeof(obj->ip6_last));
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return;
}

