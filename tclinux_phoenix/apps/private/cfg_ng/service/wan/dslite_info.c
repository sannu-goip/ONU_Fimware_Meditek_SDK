#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "port_binding.h"
#include "wan_link_list.h"
#include "default_route_handle.h"
#include "dslite_info.h"
#include "utility.h"

/*****************************************************/
static char global_dslite_addr[SVC_WAN_BUF_128_LEN];

/*****************************************************/
#if defined(TCSUPPORT_CT_PON_CY_JX)
static sigjmp_buf jmpbuf;

static void alarm_func(int sign)
{
    siglongjmp(jmpbuf,1);
}

/*This function only for ping/traceroute diagnose, and it is Not reentrant*/
static int tc_getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result, int timeout)
{
    int reval = -1;

    signal(SIGALRM, alarm_func);

    if(sigsetjmp(jmpbuf, 1) != 0)
    {
        /***timeout***/
        alarm(0);
        signal(SIGALRM, SIG_IGN);
        return reval;
    }

    alarm(timeout);
    reval = getaddrinfo(hostname, service, hints, result);
    signal(SIGALRM, SIG_IGN);

    return reval;
}
#endif
/********************************************************************/
void init_dslite_global_variable(void)
{
    memset(global_dslite_addr, 0, sizeof(global_dslite_addr));
}

void stop_dslite(wan_entry_obj_t* obj)
{
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    char cmd[SVC_WAN_BUF_128_LEN];
    wan_entry_cfg_t* cfg = NULL;

    if(NULL == obj)
    {
        SVC_WAN_DEBUG_INFO("pointer can't be NULL.\n");
        return ;
    }
    cfg = &obj->cfg;
    
    if(!SVC_WAN_DSLITE_ENABLE(cfg))
    {
        SVC_WAN_DEBUG_INFO("dslite not enable, not need execute stop dslite script.\n");
        return ;
    }

    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj, itf_name, sizeof(itf_name));

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), STOP_DSLITE, itf_name);

    svc_wan_execute_cmd(cmd);

    /***********config dslite state*************/
    obj->dslite_state &= ~DS_ACTIVE;

    return ;
}

void notify_dslite_state(void)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char node[SVC_WAN_BUF_32_LEN];
    char wan_addr[SVC_WAN_BUF_64_LEN];
    char dslite_addr[SVC_WAN_BUF_128_LEN];
    char dslite_addr_buf[SVC_WAN_BUF_64_LEN];
    char dslite_domain[SVC_WAN_BUF_128_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char cmd[SVC_WAN_BUF_128_LEN];
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    char dslite_ifname[MAX_WAN_DEV_NAME_LEN];
    struct addrinfo hints;
    struct in6_addr in_dslite_v6addr;
    struct addrinfo *res = NULL;
    struct sockaddr_in6 *p_dslite_v6addr = NULL;
    int ret1 = 0,ret2 = 0,ret3 = 0;

    for( obj = svc_wan_get_obj_list(); NULL != obj; obj=obj->next)
    {
        cfg = &obj->cfg;
        if(!SVC_WAN_IS_ACTIVE(cfg) || !SVC_WAN_IS_IPV6(cfg) || !SVC_WAN_SRV_INTERNET(cfg))
        {
            continue;
        }

        if(SVC_WAN_DSLITE_ENABLE(cfg) && (obj->dslite_state & DS_ACTIVE))
        {
            if(obj->dslite_state & DS_BINDSTATE_UP)
            {
                set_portbinding_info_for_dslite(obj);
                obj->dslite_state &= ~DS_BINDSTATE_UP;
            }
            continue;
        }
        else if(!SVC_WAN_DSLITE_ENABLE(cfg) && (0 == (obj->dslite_state & DS_ACTIVE)))
        {
            continue;
        }

        memset(node, 0, sizeof(node));
        strcpy(node,WANINFO_COMMON_NODE); 
        memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
        if(cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) < 0) 
        {
            strcpy(def_route_index_v4, "N/A");
        }

        memset(itf_name,0, sizeof(itf_name));
        svc_wan_intf_name(obj, itf_name, sizeof(itf_name));

        if(SVC_WAN_DSLITE_ENABLE(cfg))
        {
            if (SVC_WAN_DSLITE_AUTO_MODE(cfg)) // auto
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
			
                memset(dslite_addr, 0, sizeof(dslite_addr));
			ret1 = cfg_get_object_attr(node, "DsliteAddr", dslite_addr, sizeof(dslite_addr));
			
                memset(dslite_domain, 0, sizeof(dslite_domain));
			ret2 = cfg_get_object_attr(node, "DsliteName", dslite_domain, sizeof(dslite_domain));

                memset(wan_addr, 0, sizeof(wan_addr));
			ret3  = cfg_get_object_attr(node, "IP6", wan_addr, sizeof(wan_addr)) ;

			if((ret1 < 0 && ret2 < 0) || (0 >= strlen(dslite_addr) && 0 >= strlen(dslite_domain)) || ret3 < 0)
                {
                    continue;
                }
            }
            else if(SVC_WAN_DSLITE_MANUAL_MODE(cfg)) // manual
            {

                memset(dslite_addr, 0, sizeof(dslite_addr));
                if(cfg_get_object_attr(obj->path, "DsliteAddr", dslite_addr, sizeof(dslite_addr)) < 0) 
                {
                    continue;
                }
                
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
                memset(wan_addr, 0, sizeof(wan_addr));
                if(cfg_get_object_attr(node, "IP6", wan_addr, sizeof(wan_addr)) < 0)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        
            // check dslite address
            if ( 1 != inet_pton(AF_INET6, dslite_addr, &in_dslite_v6addr) )
            {
                if (SVC_WAN_DSLITE_AUTO_MODE(cfg))
                {
                    strcpy(dslite_addr, dslite_domain);
                }
                //resolve the domain,  DNS is UDP packet
                memset( &hints, 0 , sizeof(hints));
                hints.ai_family   = AF_INET6;
                hints.ai_socktype = SOCK_DGRAM; 
                hints.ai_protocol = IPPROTO_UDP;
#if defined(TCSUPPORT_CT_PON_CY_JX)
                if (0 != tc_getaddrinfo(dslite_addr, NULL, &hints, &res, 5) )
                {
                    strcpy(dslite_addr, global_dslite_addr);
                }
#else
                if(0 != getaddrinfo(dslite_addr, NULL, &hints, &res))
                {
                    continue;
                }
#endif

#if defined(TCSUPPORT_CT_PON_CY_JX)
                if ( NULL == res || AF_INET6 != res->ai_family )
                {
                    strcpy(dslite_addr, global_dslite_addr);
                }
#else
                if ( NULL == res || AF_INET6 != res->ai_family )
                {
                	if(res)
						freeaddrinfo(res);
                    continue;
                }
#endif
                if (NULL != res) 
                {
                    p_dslite_v6addr = (struct sockaddr_in6 *)res->ai_addr;
                    memset(dslite_addr_buf, 0, sizeof(dslite_addr_buf));
                    inet_ntop(AF_INET6, &p_dslite_v6addr->sin6_addr, dslite_addr_buf, INET6_ADDRSTRLEN);
                    strcpy(dslite_addr, dslite_addr_buf);
					freeaddrinfo(res);
                }
            }
#if defined(TCSUPPORT_CT_PON_CY_JX)
            if (0 == strcmp(dslite_addr, "::")) 
            {
                strcpy(dslite_addr, "240e:cc:1000::1");
            }
            strcpy(global_dslite_addr, dslite_addr);
#endif
            if( strlen(dslite_addr) > 0 && strlen(wan_addr) > 0 )
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), START_DSLITE, wan_addr, dslite_addr, itf_name);
                svc_wan_execute_cmd(cmd);

                /* update if_index to wanInfo node */
                if ( 0 == strcmp(def_route_index_v4, "N/A") || obj->idx == atoi(def_route_index_v4))
                {
                    memset(dslite_ifname, 0, sizeof(dslite_ifname));
                    snprintf(dslite_ifname, sizeof(dslite_ifname), "ds.%s", itf_name);

                    svc_wan_execute_cmd("/sbin/route del default");

                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/sbin/route add default dev %s", dslite_ifname);
                    svc_wan_execute_cmd(cmd);

                    snprintf(def_route_index_v4, sizeof(def_route_index_v4), "%d", obj->idx);
                    memset(node, 0, sizeof(node));
                    snprintf(node, sizeof(node), WANINFO_COMMON_NODE); 
                    cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
                }

                obj->dslite_state |= DS_ACTIVE;
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
                cfg_set_object_attr(node, "DsliteUP", "Yes");
#endif
            }
        }
        else
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd,  sizeof(cmd), STOP_DSLITE, itf_name);
            svc_wan_execute_cmd(cmd);

            obj->dslite_state &= ~DS_ACTIVE;

#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE, obj->idx + 1);
            cfg_set_object_attr(node, "DsliteUP", "No");
#endif

            /* default route for dslite interface is down, find another V4 INTERNET interface to replace it. */
            if ((0 != strcmp(def_route_index_v4, "N/A")) && (obj->idx == atoi(def_route_index_v4)))
            {
                default_route_switch(def_route_index_v4, sizeof(def_route_index_v4), EVT_WAN_CONN_LOSTV4);
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_COMMON_NODE); 
                cfg_set_object_attr(node, "DefRouteIndexv4", def_route_index_v4);
            }
        }
    }

    return ;
}


