#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include "wan_link_list.h"
#include "static_route.h"
#include "utility.h"
/********************************************************************************************/
static int proc_linklocal_route(char *path, char *if_name, int action, char* out_subip,
    int len_subip, char *out_netmask, int len_netmask);

static int get_subnet_info(char *path, char *out_subip, int len_subip, char *out_netmask,
    int len_netmask, char *out_gateway, int len_gateway);

static void route6_cmd(int action, char route6_para[][SVC_WAN_BUF_64_LEN]);
/********************************************************************************************/
static int get_subnet_info(char *path, char *out_subip, int len_subip, char *out_netmask, 
    int len_netmask, char *out_gateway, int len_gateway)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char node[SVC_WAN_BUF_32_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char net_mask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char sub_ip_v4[MAX_WAN_IPV4_ADDR_LEN];	
    struct in_addr gateway_ip;
    struct in_addr netmask;
    struct in_addr subip;

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return -1;
    }

    cfg = &obj->cfg;
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
    memset(gate_way_v4, 0, sizeof(gate_way_v4));
    if(cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0)
    {
        return -1;
    }

    if(0 >= strlen(gate_way_v4))
    {
        return -1;
    }
    memset(net_mask_v4, 0, sizeof(net_mask_v4));
    if(cfg_get_object_attr(node, "NetMask", net_mask_v4, sizeof(net_mask_v4)) < 0)
    {
        return -1;
    }
    if(0 >= strlen(net_mask_v4))
    {
        return -1;
    }

    if(SVC_WAN_IS_PPP(cfg))
    {
        memset(sub_ip_v4, 0, sizeof(sub_ip_v4));
        snprintf(sub_ip_v4, sizeof(sub_ip_v4), "%s", gate_way_v4);

        memset(net_mask_v4, 0, sizeof(net_mask_v4));
        snprintf(net_mask_v4, sizeof(net_mask_v4), "%s", "255.255.255.255");
    }
    else
    {
        inet_aton(gate_way_v4, &gateway_ip);
        inet_aton(net_mask_v4, &netmask);
        subip.s_addr = (gateway_ip.s_addr & netmask.s_addr);
        snprintf(sub_ip_v4, sizeof(sub_ip_v4), "%s", inet_ntoa(subip));
    }

    snprintf(out_subip,   len_subip,   "%s", sub_ip_v4);
    snprintf(out_netmask, len_netmask, "%s", net_mask_v4);
    snprintf(out_gateway, len_gateway, "%s", gate_way_v4);
    
    return 0;
}

static int proc_linklocal_route(char *path, char *if_name, int action, char* out_subip,
    int len_subip, char *out_netmask, int len_netmask)
{

    char gate_way[32] ;
    char net_mask[32] ;
    char sub_ip[32] ;
    char route_cmd[128] ;

    if ( NULL == path)
    {
        return -1;
    }

    memset(gate_way, 0, sizeof(gate_way));
    memset(net_mask, 0, sizeof(net_mask));
    memset(sub_ip, 0, sizeof(sub_ip));
    if ( 0 != get_subnet_info(path, sub_ip, sizeof(sub_ip), net_mask, sizeof(net_mask), \
        gate_way, sizeof(gate_way)) )
    {
        return -1;
    }

    memset(route_cmd, 0, sizeof(route_cmd));
    if ( E_ADD_RULE == action )
    {
        snprintf(route_cmd, sizeof(route_cmd), "/sbin/route add -net %s netmask %s dev %s", \
            sub_ip, net_mask, if_name);
    }
    else if(E_DEL_RULE == action)
    {
        snprintf(route_cmd, sizeof(route_cmd), "/sbin/route del -net %s netmask %s dev %s", \
            sub_ip, net_mask, if_name);
    }
    else
    {
        return -1;
    }

    snprintf(out_subip, len_subip, "%s", sub_ip);
    snprintf(out_netmask, len_netmask, "%s", net_mask);

    svc_wan_execute_cmd(route_cmd);
    return 0;
}


void check_route(char *path)
{
    wan_entry_obj_t* obj = NULL;
    char cmd[SVC_WAN_BUF_256_LEN];
    char sub_cmd[SVC_WAN_BUF_128_LEN];
    char route_tmp[SVC_WAN_BUF_16_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    int i_def_route_index_v4 = -1;
    int is_add_linkroute = 0;
    char dev_name[MAX_WAN_DEV_NAME_LEN];
    char if_name[MAX_WAN_DEV_NAME_LEN];
    char route_active[SVC_WAN_BUF_8_LEN];
    char dst_ip[MAX_WAN_IPV4_ADDR_LEN];
    char netmask[MAX_WAN_IPV4_ADDR_LEN];
    char dst_ip_s[MAX_WAN_IPV4_ADDR_LEN];
    char submask_s[MAX_WAN_IPV4_ADDR_LEN];
    int i_is_same_subnet = 0;
    int i = 0;

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }

    memset(node, 0, sizeof(node));
    strcpy(node,WANINFO_COMMON_NODE);
    memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
    if((cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) > 0) \
         && (strlen(def_route_index_v4) > 0) && (0 != strcmp(def_route_index_v4, "N/A")))
    {
        i_def_route_index_v4 = atoi(def_route_index_v4);
    }

    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));

    for(i=0; i<MAX_STATIC_ROUTE_NUM; i++)
    {
        memset(node, 0, sizeof(node));
        memset(dev_name, 0, sizeof(dev_name));
        snprintf(node, sizeof(node), ROUTE_ENTRY_NODE, (i+1));
        cfg_get_object_attr(node, "Device", dev_name, sizeof(dev_name));
        if(0 != strcmp(dev_name, if_name))
        {
            continue;
        }

        memset(cmd, 0, sizeof(cmd));
        strcpy(cmd, "/sbin/route add ");
        memset(route_active, 0, sizeof(route_active));
        cfg_get_object_attr(node, "Active", route_active, sizeof(route_active));
        if (0 != strcmp(route_active, "Yes"))
        {
            strcpy(cmd, "#/sbin/route add ");
        }
        else
        {
            if (i_def_route_index_v4 != obj->idx)
            {
                if ( 0 == is_add_linkroute)
                {
                    memset(dst_ip, 0, sizeof(dst_ip));
                    memset(netmask, 0, sizeof(netmask));
                    
                    proc_linklocal_route(obj->path, if_name, E_ADD_RULE, dst_ip, sizeof(dst_ip),
                        netmask, sizeof(netmask));

                    is_add_linkroute = 1;
                }
            }
        }
        memset(route_tmp, 0, sizeof(route_tmp));
        if(cfg_get_object_attr(node, "DST_IP", route_tmp, sizeof(route_tmp)) < 0)
        {
            continue;
        }
        memset(sub_cmd, 0, sizeof(sub_cmd));
        snprintf(sub_cmd, sizeof(sub_cmd), "-net %s ",route_tmp);
        strcat(cmd, sub_cmd);
        memset(dst_ip_s, 0, sizeof(dst_ip_s));
        strcpy(dst_ip_s, route_tmp);

        memset(route_tmp, 0, sizeof(route_tmp));
        if(cfg_get_object_attr(node, "Sub_mask", route_tmp, sizeof(route_tmp)) > 0)
        {
            memset(sub_cmd, 0, sizeof(sub_cmd));
            snprintf(sub_cmd, sizeof(sub_cmd), "netmask %s ",route_tmp);
            strcat(cmd, sub_cmd);
            memset(submask_s, 0, sizeof(submask_s));
            strcpy(submask_s, route_tmp);
        }

        memset(route_tmp, 0, sizeof(route_tmp));
        if(cfg_get_object_attr(node, "Device", route_tmp, sizeof(route_tmp)) > 0)
        {
            memset(sub_cmd, 0, sizeof(sub_cmd));
            snprintf(sub_cmd, sizeof(sub_cmd), "dev %s ",route_tmp);
            strcat(cmd, sub_cmd);
        }

        memset(route_tmp, 0, sizeof(route_tmp));
        if(cfg_get_object_attr(node, "Gateway", route_tmp, sizeof(route_tmp)) > 0)
        {
            if(0 != strcmp(route_tmp,"0.0.0.0"))
            {
                memset(sub_cmd, 0, sizeof(sub_cmd));
                snprintf(sub_cmd, sizeof(sub_cmd), "gw %s ",route_tmp);
                strcat(cmd, sub_cmd);
            }
        }

        memset(route_tmp, 0, sizeof(route_tmp));
        if(cfg_get_object_attr(node, "metric", route_tmp, sizeof(route_tmp)) > 0)
        {
            if(atoi(route_tmp) > 0)
            {
                memset(sub_cmd, 0, sizeof(sub_cmd));
                snprintf(sub_cmd, sizeof(sub_cmd), "metric %s ",route_tmp);
                strcat(cmd, sub_cmd);
            }
        }

        svc_wan_execute_cmd(cmd);


        if ( 1 == is_add_linkroute && 0 == i_is_same_subnet \
            && 0 == strcmp(route_active, "Yes") )
        {
            if (0 == strcmp(dst_ip, dst_ip_s) && 0 == strcmp(netmask, submask_s))
            {
                i_is_same_subnet = 1;
            }
        }
    }

    if ( is_add_linkroute && 0 == i_is_same_subnet )
    {
        proc_linklocal_route(obj->path, if_name, E_DEL_RULE, dst_ip, sizeof(dst_ip), netmask, sizeof(netmask));
    }

    return ;
}

static void route6_cmd(int action, char route6_para[][SVC_WAN_BUF_64_LEN])
{
    char cmd[130];
    char tmp[45];

    memset(cmd, 0, sizeof(cmd));
    switch(action)
    {
        case ROUTE6_ADD:
        {
            snprintf(cmd, sizeof(cmd), "/sbin/route -A inet6 add ");
            break;
        }
        case ROUTE6_DEL:
        {
            snprintf(cmd, sizeof(cmd), "/sbin/route -A inet6 del ");
            break;
        }
        default:
        {
            return ;
        }
    }

    if(strlen(route6_para[DST_IP_TYPE]) == 0 || strlen(route6_para[PREFIX_LEN_TYPE]) == 0)
    {
        SVC_WAN_ERROR_INFO("\n %s:no des ip or prefix length\n", __func__);
        return ;
    }
    
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp,  sizeof(tmp), "%s/%s ", route6_para[DST_IP_TYPE], route6_para[PREFIX_LEN_TYPE]);
    strcat(cmd, tmp);

    if(strlen(route6_para[GATEWAY_TYPE]) == 0 && strlen(route6_para[DEVICE_TYPE]) == 0)
    {
        SVC_WAN_ERROR_INFO("\n %s:no gateway or device\n", __func__);
        return ;
    }

    /*Add gateway*/
    if(strcmp(route6_para[GATEWAY_TYPE], "*") && strcmp(route6_para[GATEWAY_TYPE], "") \
        && strcmp(route6_para[GATEWAY_TYPE], "::"))
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), "gw %s ", route6_para[GATEWAY_TYPE]);
        strcat(cmd, tmp);
    }

    /*Add dev*/
    if(strcmp(route6_para[DEVICE_TYPE], "*") && strcmp(route6_para[DEVICE_TYPE], ""))
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), "dev %s ", route6_para[DEVICE_TYPE]);
        strcat(cmd, tmp);
    }

    svc_wan_execute_cmd(cmd);
    
    return ;

}


void check_static_route6(char* path)
{
    char node[SVC_WAN_BUF_32_LEN];
    char route6_para[4][SVC_WAN_BUF_64_LEN];
    int i = 0;
    char device[MAX_WAN_DEV_NAME_LEN];
    char if_name[MAX_WAN_DEV_NAME_LEN];

    wan_entry_obj_t* obj = NULL;
    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }
    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));

    for(i = 0; i < MAX_STATIC_ROUTE6_NUM; i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node,  sizeof(node), ROUTE6_ENTRY_NODE, (i+1));
        if(cfg_get_object_attr(node, "Device", device, sizeof(device)) > 0)
        {
            if(0 != strcmp(if_name, device))
            {
                continue ;
            }
            memset(route6_para, 0, sizeof(route6_para));
            cfg_get_object_attr(node, "DST_IP", route6_para[DST_IP_TYPE], sizeof(route6_para[DST_IP_TYPE]));
            cfg_get_object_attr(node, "Prefix_len", route6_para[PREFIX_LEN_TYPE], sizeof(route6_para[PREFIX_LEN_TYPE]));
            cfg_get_object_attr(node, "Gateway", route6_para[GATEWAY_TYPE], sizeof(route6_para[GATEWAY_TYPE]));
            cfg_get_object_attr(node, "Device", route6_para[DEVICE_TYPE], sizeof(route6_para[DEVICE_TYPE]));

            route6_cmd(ROUTE6_ADD, route6_para);
        }
    }

    return;
}

void del_static_route(wan_entry_obj_t* obj)
{
    int i = 0;
    char node[SVC_WAN_BUF_32_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];
    char attr_dev_name[MAX_WAN_DEV_NAME_LEN];

    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));

    for(i = 0; i < MAX_STATIC_ROUTE_NUM; i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), ROUTE_ENTRY_NODE, (i+1));
        memset(attr_dev_name, 0, sizeof(attr_dev_name));
        cfg_get_object_attr(node, "Device", attr_dev_name, sizeof(attr_dev_name));
        if ((0 < strlen(attr_dev_name)) && (0 == strcmp(dev_name, attr_dev_name)))
        {
            cfg_delete_object(node);
        }
    }

    for(i = 0; i < MAX_STATIC_ROUTE6_NUM; i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node,  sizeof(node), ROUTE6_ENTRY_NODE, (i+1));
        memset(attr_dev_name, 0, sizeof(attr_dev_name));
        cfg_get_object_attr(node, "Device", attr_dev_name, sizeof(attr_dev_name));
        if ((0 < strlen(attr_dev_name)) && (0 == strcmp(dev_name, attr_dev_name)))
        {
            cfg_delete_object(node);
        }
    }

    return ;
}

