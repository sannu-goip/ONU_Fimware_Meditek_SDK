#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <svchost_api.h>
#include <cfg_api.h>
#include "wan_common.h"
#include "wan_link_list.h"
#include "port_binding.h"
#include "misc_srv.h"
#include "default_route_handle.h"
#include "utility.h"

/*******************************************************************/
/*******************************************************************/
void default_route_switch(char* def_route_index, int index_size, int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];

    if (NULL == def_route_index)
    {
        return;
    }

    if(0 != strcmp(def_route_index, "N/A"))
    {
        if(EVT_WAN_CONN_LOSTV4 == evt)
        {
            svc_wan_delete_default_ipv4_route();
        }
        else if ( EVT_WAN_CONN_LOSTV6 == evt ) 
            svc_wan_delete_default_ipv6_route();
    }

    strcpy(def_route_index, "N/A");
    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        cfg = &obj->cfg;

        if(0 == SVC_WAN_SRV_E8C_INTERNET(cfg) 
#if defined(TCSUPPORT_CT_SDN)
		|| (cfg->sdn == 1)
#endif
			)
        {
            continue;
        }

        if(EVT_WAN_CONN_LOSTV6 == evt)
        {
            if (0 == (obj->flag & SVC_WAN_FLAG_UPV6))
            {
                continue;
            }

            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 
            memset(gate_way_v6, 0, sizeof(gate_way_v6));
            if( (cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6)) < 0) \
                || (0 >= strlen(gate_way_v6)) )
            {
                continue;
            }
            
            memset(dev_name, 0, sizeof(dev_name));
            svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
            svc_wan_add_default_ipv6_route(gate_way_v6, dev_name);
            snprintf(def_route_index, index_size, "%d", obj->idx);
            break;
        }
        else if(EVT_WAN_CONN_LOSTV4 == evt)
        {
#if defined(TCSUPPORT_CT_DSLITE)
            if(SVC_WAN_DSLITE_ENABLE(cfg) && SVC_WAN_SRV_INTERNET(cfg))
            {
                if(0 == (obj->flag & SVC_WAN_FLAG_UPV6))
                {
                    continue;
                }

                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 
                memset(gate_way_v6, 0, sizeof(gate_way_v6));
                if((cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6)) < 0) \
                    || (0 >= strlen(gate_way_v6)))
                {
                    continue;
                }
                memset(dev_name, 0, sizeof(dev_name));
                svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
                snprintf(cmd, sizeof(cmd), "/sbin/route add default dev ds.%s", dev_name);
                svc_wan_execute_cmd(cmd);
            }
            else
#endif
            {
                if(0 == (obj->flag & SVC_WAN_FLAG_UPV4))
                {
                    continue;
                }
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
                memset(gate_way_v4, 0, sizeof(gate_way_v4));
                if((cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0) \
                    ||(0 >= strlen(gate_way_v4)))
                {
                    continue;
                }
                memset(dev_name, 0, sizeof(dev_name));
                svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
                /*link local route*/
                dealwith_link_local_route(dev_name, obj->idx, E_DEFAULT_ROUTE_SWITCH);
                /**default route**/
                svc_wan_add_default_ipv4_route(gate_way_v4, dev_name);
            }
            snprintf(def_route_index, index_size, "%d", obj->idx);
            break;
        }
        else
        {
            break;
        }
    }

    return ;
}

#if defined(TCSUPPORT_CT_JOYME2)
void svc_wan_restart_nat(char* ifname)
{
	char cmd[SVC_WAN_BUF_256_LEN] = {0}, vs_chain[16] = {0};
	
	if ( NULL == ifname )
		return;

	/* flush nat */
	memset(vs_chain, 0, sizeof(vs_chain));
	memset(cmd, 0, sizeof(cmd));
	snprintf(vs_chain, sizeof(vs_chain), "VS_%s", ifname);
	snprintf(cmd, sizeof(cmd), "iptables -t nat -F %s", vs_chain);
	svc_wan_execute_cmd(cmd);

	/* restart nat */
	cfg_commit_object(VIRSERVER_NODE);
	return;
}

void svc_wan_restart_ddns(void)
{
	cfg_commit_object(DDNS_COMMON_NODE);
	return;
}
#else
void svc_wan_restart_ddns_by_oray_Flag(void)
{
	char useOray[SVC_WAN_BUF_16_LEN] = {0};
	if(cfg_get_object_attr(DDNS_COMMON_NODE, "useOray", useOray, sizeof(useOray)) > 0 && !strcmp("Yes", useOray))
	{
		cfg_commit_object(DDNS_COMMON_NODE);
	}
	
	return;
}
#endif
#if defined(TCSUPPORT_CT_SDN)
void change_default_route(int default_idx)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;

    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        cfg = &obj->cfg;
        if(!SVC_WAN_IS_IPV4(cfg) || !SVC_WAN_IS_ACTIVE(cfg) 
			|| !SVC_WAN_SRV_E8C_INTERNET(cfg) || (obj->idx == default_idx) 
			|| !SVC_WAN_IS_ROUTE(cfg) 
			|| (cfg->sdn == 1))
        {
            continue;
        }

        SVC_WAN_DEBUG_INFO("obj->path = %s.\n", obj->path);

        cfg_commit_object(obj->path);
		return;
    }
}
#endif
#if defined(TCSUPPORT_CT_JOYME4)
void lan_icmpv6_default_update(int idx)
{
#if defined(TCSUPPORT_CT_VRWAN)
	if(idx < 0)
	{
		system("ip6tables -I LAN_DROP_ICMPV6 -s fd00::/64 -i br0 -p icmpv6 -j ACCEPT");
	}
	else
	{
		system("ip6tables -D LAN_DROP_ICMPV6 -s fd00::/64 -i br0 -p icmpv6 -j ACCEPT");
	}
#endif

}
#endif

void update_default_route(char* path, int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char node[SVC_WAN_BUF_32_LEN];
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char def_route_index_v6[SVC_WAN_BUF_16_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];
#if defined(TCSUPPORT_CT_DSLITE)
    int dslite = 0;
#endif

    obj = svc_wan_find_object(path);
    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("not find object");
        return ;
    }

    cfg = &obj->cfg;
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
    if(EVT_WAN_CONN_GETV4 == evt)
    {
        if(0 == (obj->flag & SVC_WAN_FLAG_UPV4))
        {
            return ;
        }
        
        memset(gate_way_v4, 0, sizeof(gate_way_v4));
        if( (cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0) \
            || (0 >= strlen(gate_way_v4)) )
        {
            return ;
        }
    }
    else if(EVT_WAN_CONN_GETV6 == evt)
    {
        if ((0 == (obj->flag & SVC_WAN_FLAG_UPV6)) || (0 == SVC_WAN_SRV_E8C_INTERNET(cfg)))
        {
            return ;
        }

        memset(gate_way_v6, 0, sizeof(gate_way_v6));
        if( (cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6)) < 0) \
            || (0 >= strlen(gate_way_v6)) )
        {
            return ;
        }
    }
    memset(node, 0, sizeof(node));
    strcpy(node,WANINFO_COMMON_NODE);

    memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
    if(cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) < 0)
    {
        strcpy(def_route_index_v4, "N/A");
        cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
    }
#if defined(TCSUPPORT_CT_SDN)
    else
    {
        if((atoi(def_route_index_v4) == obj->idx) && (cfg->sdn == 1))
        {
            strcpy(def_route_index_v4, "N/A");
            cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
            change_default_route(obj->idx);
        }
    }
#endif
    memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
    if(cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6)) < 0)
    {
        strcpy(def_route_index_v6, "N/A");
        cfg_set_object_attr(node, "DefRouteIndexv6", def_route_index_v6);
    }

    switch (evt)
    {
        case EVT_WAN_CONN_GETV4:
        {
            if(SVC_WAN_SRV_E8C_INTERNET(cfg))
            {
#if defined(TCSUPPORT_CT_L2TP_VPN)
                open_VPN_PPP_tunnel();
#endif
            }
            memset(dev_name, 0, sizeof(dev_name));
            svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
            if((SVC_WAN_SRV_E8C_INTERNET(cfg) && (0 == strcmp(def_route_index_v4, "N/A"))) 
#if defined(TCSUPPORT_CT_SDN)				
				&& (cfg->sdn != 1)
#endif
				)
            {
                dealwith_link_local_route(dev_name, obj->idx, E_DEFAULT_ROUTE_SWITCH);
                svc_wan_add_default_ipv4_route(gate_way_v4, dev_name);
                dealwith_link_local_route(dev_name, obj->idx, E_NOT_DEFAULT_ROUTE);
                snprintf(def_route_index_v4, sizeof(def_route_index_v4), "%d", obj->idx);
                cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
                dealwith_link_local_route(dev_name, obj->idx, E_DEFAULT_ROUTE);
#if defined(TCSUPPORT_CT_JOYME2)
				/* flush nat and restart nat */
				svc_wan_restart_nat(dev_name);
				svc_wan_restart_ddns();
#else
				svc_wan_restart_ddns_by_oray_Flag();
#endif
            }
            else
            {
                dealwith_link_local_route(dev_name, obj->idx, E_NOT_DEFAULT_ROUTE);
            }
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
            if (0 != strcmp(def_route_index_v4, "N/A")) 
            {
                if (obj->idx == atoi(def_route_index_v4)) 
                {
                    default_route_switch(def_route_index_v4, sizeof(def_route_index_v4), EVT_WAN_CONN_LOSTV4);
                }
                cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
#if defined(TCSUPPORT_CT_JOYME2)
				/* flush nat and restart nat */
				svc_wan_restart_nat(dev_name);
#endif
            }
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            if (0 == strcmp(def_route_index_v6, "N/A"))
            {
                memset(dev_name, 0, sizeof(dev_name));
                svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
                svc_wan_add_default_ipv6_route(gate_way_v6, dev_name);

                snprintf(def_route_index_v6, sizeof(def_route_index_v6), "%d", obj->idx);
                cfg_set_object_attr(node, "DefRouteIndexv6", def_route_index_v6);
#if defined(TCSUPPORT_CT_JOYME4)
				lan_icmpv6_default_update(obj->idx);
#endif
            }
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            if (0 != strcmp(def_route_index_v6, "N/A")) 
            {
                if (obj->idx == atoi(def_route_index_v6)) 
                {
                    default_route_switch(def_route_index_v6, sizeof(def_route_index_v6), EVT_WAN_CONN_LOSTV6);
#if defined(TCSUPPORT_CT_JOYME4)
                    if(0 != strcmp(def_route_index_v6, "N/A"))
                    {
                        lan_icmpv6_default_update(atoi(def_route_index_v6));
                    }
                    else
                    {
                        lan_icmpv6_default_update(-1);
                    }
					
#endif
                }
                cfg_set_object_attr(node, "DefRouteIndexv6", def_route_index_v6);
            }
#if defined(TCSUPPORT_CT_DSLITE)
            if((!SVC_WAN_IS_BRIDGE(cfg)) && SVC_WAN_SRV_INTERNET(cfg) && SVC_WAN_DSLITE_ENABLE(cfg))
            {
                dslite = ENABLE;
            }

            if(dslite && 0 != strcmp(def_route_index_v4, "N/A"))
            {
                if(obj->idx == atoi(def_route_index_v4))
                {
                    default_route_switch(def_route_index_v4, sizeof(def_route_index_v4),  EVT_WAN_CONN_LOSTV4);
                    cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
                }
            }
#endif
            break;
        }
        default:
        {
            break;
        }
    }

    return ;
}

