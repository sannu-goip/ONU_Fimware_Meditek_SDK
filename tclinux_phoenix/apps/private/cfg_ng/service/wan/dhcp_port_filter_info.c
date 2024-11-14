#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "port_binding.h"
#include "dhcp_port_filter_info.h"
#include "utility.h"

#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
int setSingleBridgeWanPortFilter(wan_entry_cfg_t* cfg, char* PortFilterInputTable)
{
	char ebtablesCmd[SVC_WAN_BUF_128_LEN] = {0};
	char path[SVC_WAN_BUF_32_LEN] = {0};
	char active[SVC_WAN_BUF_16_LEN] = {0};
	int i = 0, j = 0, num = 0;

	for (i = 0; i < SVC_WAN_MAX_PVC_NUM; i++)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), WAN_PVC_NODE,(i+1));
		if(cfg_query_object(path, NULL, NULL) <= 0)
		{
			continue;
		}

		for(j = 0; j < SVC_WAN_MAX_ENTRY_NUM; j++)
		{
		    memset(path, 0, sizeof(path));
		    snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE,(i+1), (j+1));
			if(cfg_obj_get_object_attr(path, "Active", 0, active, sizeof(active)) > 0 && !strcmp(active, "Yes"))
			{
				num++;
			}
		}
	}

	if(num == 1 
		&& cfg->wanmode == SVC_WAN_ATTR_WANMODE_BRIDGE 
		&& !strcmp(cfg->srv_list, "INTERNET")
		&& cfg->dhcp_realy == SVC_WAN_ATTR_DHCPRELAY_ENABLE)
	{		
		/* ipv4 */
		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
		
		snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i eth0.+ -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
			EBTABLES_FILTER, PortFilterInputTable);

		svc_wan_execute_cmd(ebtablesCmd);

		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
		
		snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i ra+ -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
			EBTABLES_FILTER, PortFilterInputTable);

		svc_wan_execute_cmd(ebtablesCmd);

		/* ipv6 */
		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
		
		snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i eth0.+ -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
			EBTABLES_FILTER, PortFilterInputTable);

		svc_wan_execute_cmd(ebtablesCmd);

		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
		
		snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i ra+ -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
			EBTABLES_FILTER, PortFilterInputTable);

		svc_wan_execute_cmd(ebtablesCmd);
		
		return 1;
	}
	
	return 0;
}

void set_DHCP_PortFilter_info(wan_entry_obj_t* obj, int op)
{
    wan_entry_cfg_t* cfg = NULL;
    char PortFilterInputTable[SVC_WAN_BUF_64_LEN] = {0};
	char PortFilterOutputTable[SVC_WAN_BUF_64_LEN] = {0};
    char ebtablesCmd[SVC_WAN_BUF_128_LEN] = {0};
	int ret = -1;

    if(NULL == obj)
    {
        return ;
    }
    cfg = &obj->cfg;

    memset(PortFilterInputTable, 0, sizeof(PortFilterInputTable));
    snprintf(PortFilterInputTable, sizeof(PortFilterInputTable), "DhcpFilterInput%s", obj->dev);

	memset(PortFilterOutputTable, 0, sizeof(PortFilterOutputTable));
    snprintf(PortFilterOutputTable, sizeof(PortFilterOutputTable), "DhcpFilterOutput%s", obj->dev);

    if(E_ADD_DHCP_PORT_FILTER_RULE == op)
    {
		if( check_port_bind_mask(obj->idx) )
					return;
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -P %s RETURN", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A INPUT -j %s", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -P %s RETURN", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A OUTPUT -j %s", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

		ret = setSingleBridgeWanPortFilter(cfg, PortFilterInputTable);
		if(ret != 1){		
#ifdef TCSUPPORT_PORTBIND
	 	       if( (DISABLE == cfg->dhcp_enable) && SVC_WAN_IS_ACTIVE(cfg) \
		            && (SVC_WAN_SRV_INTERNET(cfg) || SVC_WAN_SRV_OTHER(cfg)
#if defined(TCSUPPORT_CMCCV2)
				|| SVC_WAN_SRV_IPTV(cfg) || SVC_WAN_SRV_OTT(cfg)
#endif
				) )
 			       {
#if defined(TCSUPPORT_CMCCV2)
				if ( SVC_WAN_SRV_INTERNET(cfg) && SVC_WAN_IS_ROUTE(cfg) )
					return; /* NO dhcp filter for INTERNET ROUTE. */
#endif
 				           add_dhcp_port_filter_rule(obj, PortFilterInputTable, PortFilterOutputTable);
			        }
#endif
 		   }
			set_port_bind_mask(obj->idx);
    }
    else if(E_DEL_DHCP_PORT_FILTER_RULE == op)
    {
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -F %s 2>/dev/null", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -D INPUT -j %s", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -X %s 2>/dev/null", EBTABLES_FILTER, PortFilterInputTable);
        svc_wan_execute_cmd(ebtablesCmd);

		memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -F %s 2>/dev/null", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -D OUTPUT -j %s", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -X %s 2>/dev/null", EBTABLES_FILTER, PortFilterOutputTable);
        svc_wan_execute_cmd(ebtablesCmd);

		clear_port_bind_mask(obj->idx);
    }

    return ;
}

#endif

