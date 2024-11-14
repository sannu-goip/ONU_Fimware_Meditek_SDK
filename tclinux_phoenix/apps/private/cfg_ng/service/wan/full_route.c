#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cfg_api.h>
#include "full_route.h"
#include "wan_common.h"
#include "wan_link_list.h"
#include "full_route.h"
#include "../cfg/utility.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

#define MAXLEN_TCAPI_MSG        (1024)
#define MAXLEN_IP_FWD_LENGTH        (MAXLEN_TCAPI_MSG + 80)

#if defined(TCSUPPORT_CT_FULL_ROUTE)
unsigned int get_default_pvc_DNSOrder(void)
{
    char node[SVC_WAN_BUF_32_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char def_route_index_v6[SVC_WAN_BUF_16_LEN];
    unsigned int defaut_route_id = 0xFFFFFFFF;
    int tempId = -1;

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", WANINFO_COMMON_NODE);

    memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
    if(cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) < 0)
    {
        goto checkv6;
    }

    if( (0 == strlen(def_route_index_v4)) || (0 == strncmp(def_route_index_v4, "N/A", 3)) ) 
    {
        goto checkv6;
    }

    defaut_route_id &= ~0xFFFF;
    defaut_route_id |= atoi(def_route_index_v4);


checkv6:
    memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
    if(cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6)) < 0)
    {
        goto out;
    }
    if( (0 == strlen(def_route_index_v6)) || (0 == strncmp(def_route_index_v6, "N/A", 3)) ) 
    {
        goto out;
    }

    tempId = atoi(def_route_index_v6);
    defaut_route_id &= ~0xFFFF0000;
    defaut_route_id |= tempId << 16;

    out:	
    return defaut_route_id;
}

#ifdef TCSUPPORT_IPV6_ADVANCE
/*
        return:
        0:  success.
        -1:error
        1:  may be process next time.
*/
int ipv6_rule_exec(char *ip6Addr, int if_index)
{
    struct in6_addr in_s_v6addr;
    struct in6_addr in_e_v6addr;
    char full_route_cmd[SVC_WAN_BUF_128_LEN];
    char start_addr[80] = {0};
    char end_addr[80] = {0};
    unsigned int mark = 0;

    mark = (if_index + 1) << 16;
    if ( NULL == strstr(ip6Addr, "-") ) // single ip
    {
        if ( strlen(ip6Addr) > 39)
        {
            return -1;
        }
        if ( 1 != inet_pton(AF_INET6, ip6Addr, &in_s_v6addr) )
        {
            return 1;
        }

        memset(full_route_cmd, 0, sizeof(full_route_cmd));
        
        snprintf(full_route_cmd, sizeof(full_route_cmd), "ip6tables -A full_route_%d -t mangle -d %s -j MARK --set-mark 0x%x/0x7f0000\n", \
            if_index, ip6Addr, mark);
        
        svc_wan_execute_cmd(full_route_cmd);
    }
    else // ip range
    {
        sscanf(ip6Addr, "%[^-]-%s", start_addr, end_addr);
        if ( 1 != inet_pton(AF_INET6, start_addr, &in_s_v6addr))
        {
            return 1;
        }

        if ( 1 != inet_pton(AF_INET6, end_addr, &in_e_v6addr))
        {
            return 1;
        }
        
        if ( memcmp(&in_s_v6addr, &in_e_v6addr, sizeof(struct in6_addr)) > 0 )
        {
            return -1;
        }

        memset(full_route_cmd, 0, sizeof(full_route_cmd));
        
        snprintf(full_route_cmd, sizeof(full_route_cmd), "ip6tables -A full_route_%d -t mangle -m iprange --dst-range %s -j MARK --set-mark 0x%x/0x7f0000\n", \
            if_index, ip6Addr, mark);
        
        svc_wan_execute_cmd(full_route_cmd);
    }
    
    return 0;
}
#endif

/*
        return:
        0:      success.
        -1:     error
        1:      may be process next time.
*/
int ipv4_rule_exec(char *ip4Addr, int if_index)
{
    struct in_addr in_s_v4addr = {0};
    struct in_addr in_e_v4addr = {0};
    char full_route_cmd[SVC_WAN_BUF_128_LEN];
    char start_addr[80] = {0};
    char end_addr[80] = {0};
    unsigned int mark = 0;

    mark = (if_index + 1) << 16;
    if (NULL == strstr(ip4Addr, "-"))
    {
        if ( strlen(ip4Addr) > 15 )
        {
            return -1;
        }

        if ( 1 != inet_pton(AF_INET, ip4Addr, &in_s_v4addr) )
        {
            return 1;
        }
        memset(full_route_cmd, 0, sizeof(full_route_cmd));
        
        snprintf(full_route_cmd, sizeof(full_route_cmd), "iptables -A full_route_%d -t mangle -d %s -j MARK --set-mark 0x%x/0x7f0000\n", \
            if_index, ip4Addr, mark);
        
        svc_wan_execute_cmd(full_route_cmd);
    }
    else
    {
        sscanf(ip4Addr, "%[^-]-%s", start_addr, end_addr);
        if ( 1 != inet_pton(AF_INET, start_addr, &in_s_v4addr) )
        {
            return 1;
        }
        if ( 1 != inet_pton(AF_INET, end_addr, &in_e_v4addr) )
        {
            return 1;
        }
        if ( in_s_v4addr.s_addr > in_e_v4addr.s_addr )
        {
            return -1;
        }

        memset(full_route_cmd, 0, sizeof(full_route_cmd));
        snprintf(full_route_cmd, sizeof(full_route_cmd), "iptables -A full_route_%d -t mangle -m iprange --dst-range %s -j MARK --set-mark 0x%x/0x7f0000\n", \
            if_index, ip4Addr, mark);
        svc_wan_execute_cmd(full_route_cmd);
    }
    
    return 0;
}

// if  wan interface i's table is created, set full_route_table[i] to 1.
int full_route_table[SVC_MAX_WAN_INTF_NUMBER] = {0};
int onOff[2] = {0};
int insmod_ip6_tables_ko_flag = 0;

inline int get_table_status(int if_index)
{
    return full_route_table[if_index];
}
inline void set_table_status(int if_index, int value)
{
    full_route_table[if_index] = value;
    return;
}

/*
    return:
            returnPtr:point the rest of  "ipStr",
            because ipStr may be "....,192.168", then returnPtr will be "192.168"

    ipv4Str format: x.x.x.x,x.x.x.a-x.x.x.b
    ipv6Str format: x:x:x:x,x:x:x:a-x:x:x:b
*/

char *set_forward_rule(char *ipStr, int if_index, char flag)
{
    char *tempStr = NULL;
    char *tempP = ipStr;
    char *returnPtr = NULL;
    int ret = 0;

    while( NULL != (tempStr = strsep(&tempP, ",")) )
    {
        returnPtr = tempStr;
        if(tempP ==NULL)
        {
            //the last part
            if(flag != ',')// the rest of ipStr is not NULL, so return for next time to process.
            {
                break;
            }
            returnPtr = NULL;//flag==',' mean the rest of ipStr is NULL
        }
        if(NULL != strstr(tempStr, "."))
        {
            ret = ipv4_rule_exec(tempStr, if_index);
        }
        else if(NULL != strstr(tempStr, ":"))
        {
#ifdef TCSUPPORT_IPV6_ADVANCE
            ret = ipv6_rule_exec(tempStr, if_index);
#endif
        }
        
        if(ret == 0)
        {
            returnPtr = NULL;
        }
    }

    return returnPtr;
}

void close_table_by_ifIndex(int index)
{
    char cmd[SVC_WAN_BUF_128_LEN];

    if(index<0 || index > SVC_MAX_WAN_INTF_NUMBER-1)
    {
        return;
    }
    
    if(1 == get_table_status(index))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D PREROUTING -j full_route_%d\n", IPTABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -F full_route_%d\n", IPTABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -X full_route_%d\n", IPTABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);
#ifdef TCSUPPORT_IPV6_ADVANCE
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D PREROUTING -j full_route_%d\n", IP6TABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -F full_route_%d\n", IP6TABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -X full_route_%d\n", IP6TABLES_MANGLE, index);
        svc_wan_execute_cmd(cmd);
#endif
        set_table_status(index, 0);
    }

    return ;
}

void full_route_on(void)
{
    char node[SVC_WAN_BUF_32_LEN];
    char type[SVC_WAN_BUF_8_LEN];
#ifdef TCSUPPORT_IPV6_ADVANCE
    char vername[SVC_WAN_BUF_16_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
#endif
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node),"%s", LAN_DHCP_NODE);

    memset(type, 0, sizeof(type));
    if( (cfg_get_object_attr(node, "type", type, sizeof(type)) > 0) && (0 == strncmp(type, "0", 1)) )
    {
        cfg_set_object_attr(node, "type", "1");
        cfg_commit_object(DHCPD_COMMON_NODE);
    }

    if(onOff[0] == 1)
    {
        return;
    }

#ifdef TCSUPPORT_IPV6_ADVANCE
    if(0 == insmod_ip6_tables_ko_flag)
    {
        memset(vername,0,sizeof(vername));
        decideModulePath(vername, SVC_WAN_BUF_16_LEN);
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "insmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6_tables.ko", vername);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "insmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6table_filter.ko", vername);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "insmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6table_mangle.ko", vername);
        svc_wan_execute_cmd(cmd);
        insmod_ip6_tables_ko_flag = 1;
    }
#endif

    onOff[0] = 1;
    onOff[1] = 0;

    return ;
}

void remove_all_tables(void)
{
    wan_entry_obj_t* obj = NULL;
    char node[SVC_WAN_BUF_32_LEN];
#if defined(TCSUPPORT_CT_VRWAN)
	wan_entry_cfg_t* cfg = NULL;
#endif
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", WAN_COMMON_NODE);	
    cfg_set_object_attr(node, "IPForwardModeEnable", "No");

#if defined(TCSUPPORT_CT_VLAN_BIND)
    /*reset vlan bind entry & open portbind function*/
    cfg_commit_object(VLANBIND_NODE);
#endif

#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
#ifdef TCSUPPORT_PORTBIND
    svc_wan_execute_cmd("portbindcmd enable");
#endif
#endif

    for(obj = svc_wan_get_obj_list(); NULL != obj; obj = obj->next)
    {
#if defined(TCSUPPORT_CT_VRWAN)
		cfg = &obj->cfg;
		if(!SVC_WAN_SRV_VR(cfg) && !SVC_WAN_SRV_OTHER(cfg))
#endif
        close_table_by_ifIndex(obj->idx);
    }

    return ;
}

void full_route_off(void)
{
#ifdef TCSUPPORT_IPV6_ADVANCE
    char vername[SVC_WAN_BUF_16_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
#endif
    if(onOff[1] == 1)
    {
        return;
    }

#if defined(TCSUPPORT_IPV6_ADVANCE) && !defined(TCSUPPORT_CT_VRWAN)
    if(insmod_ip6_tables_ko_flag)
    {
        memset(vername,0,sizeof(vername));
        decideModulePath(vername, SVC_WAN_BUF_16_LEN);
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "rmmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6_tables.ko", vername);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "rmmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6table_filter.ko", vername);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "rmmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6table_mangle.ko", vername);
        svc_wan_execute_cmd(cmd);
        insmod_ip6_tables_ko_flag = 0;
    }
#endif
    if(onOff[0] == 1){
	tcdbg_printf("[%s:%d]full_route_off is on should be off\n",__FUNCTION__,__LINE__);
        remove_all_tables();
    }
    onOff[0] = 0;
    onOff[1] = 1;
    
    return ;
}

/*
**  function name: is_full_route_enable
**  descriptions:
**      To check full route function is open or not.

**  return,
**       1: full route function is open.
**      0:full route function is closed.
*/
int is_full_route_enable(wan_entry_obj_t* obj)
{
    char node[SVC_WAN_BUF_32_LEN];
    char fr_enable[SVC_WAN_BUF_8_LEN];
#if defined(TCSUPPORT_CT_VRWAN)
	wan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
#endif
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", WAN_COMMON_NODE);	

    memset(fr_enable, 0, sizeof(fr_enable));
   	cfg_get_object_attr(node, "IPForwardModeEnable", fr_enable, sizeof(fr_enable));

    if(0 == strncmp(fr_enable, "Yes", 3))
    {
        full_route_on();
        return 1;
    }
#if defined(TCSUPPORT_CT_VRWAN)
    else
    {
        if(SVC_WAN_SRV_VR(cfg) || (SVC_WAN_SRV_OTHER(cfg) && SVC_WAN_IS_ROUTE(cfg)))
        {
            full_route_on();
            return 2;
        }
    }
#endif

    full_route_off();

    return 0;
}

int update_IPForwardList(wan_entry_obj_t* obj, int full_route_mode)
{
	char cmd[256] = {0}, ipforwardVal[MAXLEN_IP_FWD_LENGTH];
	char ipforwardStr[SVC_WAN_BUF_32_LEN], ipforwardTemp[MAXLEN_IP_FWD_LENGTH];
	char value_buf[80];
	char *part_str = NULL;
	int total_attr = 0, i = 0, len = 0;

	memset(cmd, 0, sizeof(cmd));
	/* flush this nas's chain */
	snprintf(cmd, sizeof(cmd), "%s -F full_route_%d\n", IPTABLES_MANGLE, obj->idx);
	svc_wan_execute_cmd(cmd);

#ifdef TCSUPPORT_IPV6_ADVANCE
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "%s -F full_route_%d\n", IP6TABLES_MANGLE, obj->idx);
	svc_wan_execute_cmd(cmd);
#endif

    /*add rule to the chain*/
    // max ipv6 addr pair is 80, total 1024, so we need 80*1024/MAXLEN_TCAPI_MSG +1 attr to restore this value.
    total_attr = (80 * 1024 / MAXLEN_TCAPI_MSG) + 1;
    memset(ipforwardVal, 0, sizeof(ipforwardVal));
    if( (cfg_get_object_attr(obj->path, "IPForwardList0", ipforwardVal, sizeof(ipforwardVal)) < 0) \
        ||  (0 >= strlen(ipforwardVal)) )
    {
        goto out;
    }

    if(strlen(ipforwardVal) < MAXLEN_TCAPI_MSG - 1)
    {
        goto normal;
    }

    for(i=1; i<total_attr; i++)
    {
        len = 0;
        memset(ipforwardStr, 0, sizeof(ipforwardStr));
        snprintf(ipforwardStr, sizeof(ipforwardStr), "IPForwardList%d", i);
        memset(ipforwardTemp, 0, sizeof(ipforwardTemp));
        if( (cfg_get_object_attr(obj->path, ipforwardStr, ipforwardTemp, sizeof(ipforwardTemp)) < 0) \
            || (0 >= strlen(ipforwardTemp)) )
        {
            ipforwardTemp[0] = ',';
        }

        part_str = set_forward_rule(ipforwardVal, obj->idx, ipforwardTemp[0]);
        if((part_str!=NULL) && (0 != (len = strlen(part_str))))
        {
            strncpy(value_buf, part_str, len);
        }
        memset(ipforwardVal, 0, sizeof(ipforwardVal));
        if(len)
        {
            strncpy(ipforwardVal, value_buf, len);
        }
        strncpy(ipforwardVal+len, ipforwardTemp, strlen(ipforwardTemp));
        if(strlen(ipforwardTemp) < MAXLEN_TCAPI_MSG - 1)
        {
            break;
        }
    }

normal:
    set_forward_rule(ipforwardVal, obj->idx, ',');

out:
	if(full_route_mode == 2)
		return 0;
	else
    	return 1;
	
}


/*_____________________________________________________________________________
**  function name: full_route_execute
**          descriptions:
**                  to do full route function.
**
**          parameters:
**                  pvc_index:	pvc num.
**                  entry_index:	entry num.
**          global:
**              None
**
**          return:
**              0:    not open this function, do others.
**              1:	set full route bind rule.
**              call:
**
**          revision:
**              1. Sofree.meng 20130828
**____________________________________________________________________________
*/
int full_route_execute(wan_entry_obj_t* obj)
{
    int i = 0;
    int total_attr = 0;
    int len = 0;
    char cmd[SVC_WAN_BUF_256_LEN];
    int full_route_mode = 0;
    wan_entry_cfg_t* cfg = NULL;

    if((full_route_mode = is_full_route_enable(obj)) == 0)
    {
        return 0;
    }

    cfg = &obj->cfg;
    if(SVC_WAN_IS_BRIDGE(cfg))
    {
        remove_all_tables();
        return 1;
    }
#if defined(TCSUPPORT_CT_VRWAN)
    if(full_route_mode != 2)
    {
#endif

#if defined(TCSUPPORT_CT_VLAN_BIND)
    /*clean all vlan bind's mark & close vlan bind and port bind*/
    for(i=0; i<MAX_LAN_PORT_NUM; i++) 
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D BROUTING -j vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -X vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
        svc_wan_execute_cmd(cmd);
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/tc3162/vbind_active");
    svc_wan_execute_cmd(cmd);

#endif
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
#ifdef TCSUPPORT_PORTBIND
    svc_wan_execute_cmd("portbindcmd disable");
#endif
#endif
#if defined(TCSUPPORT_CT_VRWAN)
    }
#endif

    /*create chain first if it is not exist..*/
    if(0 == get_table_status(obj->idx))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -N full_route_%d\n", IPTABLES_MANGLE, obj->idx);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -I PREROUTING 1 -j full_route_%d\n", IPTABLES_MANGLE, obj->idx);
        svc_wan_execute_cmd(cmd);
#ifdef TCSUPPORT_IPV6_ADVANCE
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -N full_route_%d\n", IP6TABLES_MANGLE, obj->idx);
        svc_wan_execute_cmd(cmd);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -I PREROUTING 1 -j full_route_%d\n", IP6TABLES_MANGLE, obj->idx);
        svc_wan_execute_cmd(cmd);
#endif
        set_table_status(obj->idx, 1);
    }

	return update_IPForwardList(obj, full_route_mode);
}

#endif

