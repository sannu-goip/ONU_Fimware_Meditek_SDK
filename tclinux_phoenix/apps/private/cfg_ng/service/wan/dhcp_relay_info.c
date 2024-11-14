#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "port_binding.h"
#include "dhcp_relay_info.h"

void set_DHCPRelay_info(wan_entry_obj_t* obj, int op)
{
    char ebtablesCmd[SVC_WAN_BUF_128_LEN];
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
    char RealyInputTabe[SVC_WAN_BUF_64_LEN];
#endif
    char RealyForwardTabe[SVC_WAN_BUF_64_LEN];
    char isInternetTrans = 0;
    wan_entry_cfg_t* cfg = NULL;
    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("pointer can' t NULL.\n");
        return ;
    }

    cfg = &obj->cfg;
    if(SVC_WAN_ATTR_DHCPRELAY_NOT_CONFIG == cfg->dhcp_realy)
    {
        SVC_WAN_NOTICE_INFO("cfg->dhcp_realy  not config.\n");
        return ;
    }

    if((SVC_WAN_ATTR_DHCPRELAY_ENABLE  == cfg->dhcp_realy) &&  SVC_WAN_SRV_INTERNET(cfg))
    {
        SVC_WAN_NOTICE_INFO("dhcp_realy enable.\n");
        isInternetTrans = 1;
    }

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
    memset(RealyInputTabe, 0, sizeof(RealyInputTabe));
    snprintf(RealyInputTabe, sizeof(RealyInputTabe), "RealyInput%s", obj->dev);
#endif
    memset(RealyForwardTabe, 0, sizeof(RealyForwardTabe));
    snprintf(RealyForwardTabe, sizeof(RealyForwardTabe), "RealyForward%s", obj->dev);

    if(SVC_WAN_IS_BRIDGE(cfg) &&  E_ADD_DHCPRELAY_RULE == op)
    {
        //new input chain for DHCP realy
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -P %s RETURN",  EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A INPUT -j %s", EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);
#endif

        //new forward chain for DHCP realy
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -P %s RETURN",  EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A FORWARD -j %s", EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);
#if !defined(TCSUPPORT_NP_CMCC)
#ifdef TCSUPPORT_PORTBIND
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
        add_dhcprealy_port_binding_rule(obj, RealyInputTabe, RealyForwardTabe);
#else
        add_dhcprealy_port_binding_rule(obj, RealyForwardTabe);
#endif
#endif
#endif
        if ( 0 == isInternetTrans )
        {
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -o %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
                EBTABLES_FILTER, RealyForwardTabe, obj->dev);
            
            svc_wan_execute_cmd(ebtablesCmd);

            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -o %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
                EBTABLES_FILTER, RealyForwardTabe, obj->dev);
            
            svc_wan_execute_cmd(ebtablesCmd);
#endif
        }
    }
    else if(E_DEL_DHCPRELAY_RULE == op)
    {
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -F %s 2>/dev/null", EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);
#endif

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -F %s 2>/dev/null", EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -D INPUT -j %s", EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);
#endif

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -D FORWARD -j %s", EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -X %s 2>/dev/null", EBTABLES_FILTER, RealyInputTabe);
        svc_wan_execute_cmd(ebtablesCmd);
#endif

        memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
        snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -X %s 2>/dev/null", EBTABLES_FILTER, RealyForwardTabe);
        svc_wan_execute_cmd(ebtablesCmd);
    }

    return;
}





