#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cfg_api.h>
#include "wan_link_list.h"
#include "dslite_info.h"
#include "port_binding.h"
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#include <lan_port/lan_port_info.h>
#include <lan_port/bind_list_map.h>
#include <lan_port/itf_policy_route_table.h>
#endif
#include "utility.h"
#include "global_def.h"

/*******************************************************/
/*******************************************************/
#define MAX_BIND_INIT_SIZE			(5)
#define MAX_BIND_STATUS_SIZE 		(1024)
unsigned long long bindrule_mark = 0;

#if defined(TCSUPPORT_CT_PPPOEPROXY) && !defined(TCSUPPORT_CT_PON_JS)
static void cmdPipe(char * command, char ** output);
#endif

/*******************************************************/
/*******************************************************/
#if defined(TCSUPPORT_CT_PPPOEPROXY) && !defined(TCSUPPORT_CT_PON_JS)
#define BUFSZ 4096
static void cmdPipe(char * command, char ** output)
{
    FILE *fp;
    char buf[BUFSZ];
    int len;

    *output = malloc(1);
    strncpy(*output, "", sizeof(*output));
    if((fp = popen(command, "r"))==NULL)
    {
        return;
    }
    while((fgets(buf, BUFSZ, fp)) != NULL)
    {
        len = strlen(*output) + strlen(buf);
        if((*output = realloc(*output, (sizeof(char)*(len+1))) )==NULL)
        {
            pclose(fp);
            return;
        }

        strcat(*output, buf);
    }

    pclose(fp);
    
    return ;
}
#endif

/**********************************************************************/
void dealwith_link_local_route(char* if_name, int if_index, int def_route)
{
    //isDefaultRoute[0:not default route, 1:add new default route; 2: default route switch]
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char net_mask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char sub_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
    struct in_addr gatewayIP;
    struct in_addr netmask;
    struct in_addr subIP;
    int pvc_index=0;
    int entry_index=0;
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    int is_internet = 0;

    pvc_index = if_index / SVC_WAN_MAX_ENTRY_NUM + 1 ;
    entry_index = if_index % SVC_WAN_MAX_ENTRY_NUM +1 ;
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
    if((obj = svc_wan_find_object(node)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }
    cfg = &obj->cfg;

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);
    memset(gate_way_v4, 0, sizeof(gate_way_v4));
    memset(net_mask_v4, 0, sizeof(net_mask_v4));
    if(cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0)
    {
        return ;
    }

    if(cfg_get_object_attr(node, "NetMask", net_mask_v4, sizeof(net_mask_v4)) < 0)
    {
        return ;
    }

    memset(sub_ip_v4, 0, sizeof(sub_ip_v4));
    if(SVC_WAN_IS_PPP(cfg))
    {
        memset(net_mask_v4, 0, sizeof(net_mask_v4));
        strcpy(sub_ip_v4, gate_way_v4);
        strcpy(net_mask_v4, "255.255.255.255");
    }
    else
    {
        inet_aton(gate_way_v4, &gatewayIP);
        inet_aton(net_mask_v4, &netmask);
        subIP.s_addr = (gatewayIP.s_addr & netmask.s_addr);
        strncpy(sub_ip_v4, inet_ntoa(subIP), sizeof(sub_ip_v4) - 1);
    }

    if ( SVC_WAN_SRV_INTERNET(cfg) )
        is_internet = 1;
    else
        is_internet = 0;

    //del link route if it is not default wan, and add link route when it is default route
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
        if ( 0 == is_internet 
#if defined(TCSUPPORT_CT_SDN)
            || cfg->sdn == 1
#endif
        )
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/sbin/route del -net %s netmask %s dev %s", sub_ip_v4, net_mask_v4, if_name);
            svc_wan_execute_cmd(cmd);
        }
    }

    return ;
}

/**********************************************************************/
int get_port_binding_interface_info(char* path, wan_entry_cfg_t* cfg, wan_entry_obj_t* obj)
{
    int i = 0;
    char tmp[64] = {0}, node_name[128] = {0};
    unsigned int  bindbit = 0;
    for(i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
        memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(path, bl_map_data[i].bindif, tmp, sizeof(tmp)) > 0)
        {
            if(0 == strcmp(tmp, "Yes"))
            {
                bindbit |= (1<<i);

            }
        }
    }



    return bindbit;
}

/**********************************************************************/
/**********************************************************************/
#if defined(TCSUPPORT_CT_JOYME2)

unsigned int other_wan_bind_bit = 0;
unsigned int other_wan_bind_info[64];
unsigned int other_wan_list0 = 0;
unsigned int other_wan_list1 = 0;

int get_other_wan_bind(wan_entry_obj_t *obj, int action)
{
	wan_entry_cfg_t* cfg = NULL;
	char buf[10] = {0};
	char buf1[10] = {0};
	char buf2[10] = {0};
	
	cfg = &obj->cfg;
	
	other_wan_bind_bit &= ~(other_wan_bind_info[obj->idx]);
	other_wan_bind_info[obj->idx] = 0;
	
	if (SVC_WAN_SRV_OTHER(cfg) && E_ADD_PORT_BIND_RULE == action) {
		other_wan_bind_info[obj->idx] = cfg->bindbit;
		other_wan_bind_bit |= other_wan_bind_info[obj->idx];
		if (obj->idx < 32)
			other_wan_list0 |= (1<<obj->idx);
		else 
			other_wan_list1 |= (1<<obj->idx - 32);
	}

	if (SVC_WAN_SRV_OTHER(cfg) && E_DEL_PORT_BIND_RULE == action) {
		if (obj->idx < 32)
				other_wan_list0 &= ~(1<<obj->idx);
			else 
				other_wan_list1 &= ~(1<<obj->idx - 32);
	}

	snprintf(buf1, sizeof(buf1), "%d", other_wan_list0);
	snprintf(buf2, sizeof(buf2), "%d", other_wan_list1);
	snprintf(buf, sizeof(buf), "%d", other_wan_bind_bit);

	cfg_set_object_attr("root.wan.common", "other_wan_list0", buf1);
	cfg_set_object_attr("root.wan.common", "other_wan_list1", buf2);
	cfg_set_object_attr("root.wan.common", "other_wan_bind_bit", buf);

	return 0;
}
#endif

int svc_wan_get_bind_info2(wan_entry_obj_t *obj, port_bind_info_t* info)
{
    wan_entry_cfg_t* cfg = NULL;
    int i =0;
    char bind_flag = 0;

    if ((NULL == obj ) || ( NULL == info))
    {
        return -1;
    }
    cfg = &obj->cfg;
    for(i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
        if (0 == (cfg->bindbit & (1<<i)))
        {
            continue;
        }

        if(0 == strlen(info->bind_ifName_list))
        {
            strncpy(info->bind_ifName_list, bl_map_data[i].realif, sizeof(info->bind_ifName_list) - 1);
            strcpy(info->bind_ifType_list, "O_O");
        }
        else
        {
            snprintf(info->bind_ifName_list + strlen(info->bind_ifName_list), 
				sizeof(info->bind_ifName_list) - strlen(info->bind_ifName_list), ",%s", bl_map_data[i].realif);
            strcat(info->bind_ifType_list, ",O_O");
        }
    }

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
    if(0 == strlen(info->bind_ifName_list))
    {
        snprintf(info->bind_lanif_list, sizeof(info->bind_lanif_list), NOTBINDSTRING);
    }
    else
    {
        snprintf(info->bind_lanif_list, sizeof(info->bind_lanif_list), info->bind_ifName_list);
    }
#endif

    if (0 < strlen(info->bind_ifName_list)) 
    {
        snprintf(info->bind_ifName_list + strlen(info->bind_ifName_list), 
			sizeof(info->bind_ifName_list) - strlen(info->bind_ifName_list), ",%s", obj->dev);
        bind_flag = 1;
    }
    else
    {
        strcpy(info->bind_ifName_list, obj->dev);
        bind_flag = 0;
    }

    if(SVC_WAN_SRV_E8C_INTERNET(cfg)
#if defined(TCSUPPORT_CT_VRWAN)
	|| SVC_WAN_SRV_VR(cfg)
#endif
#if defined(TCSUPPORT_NPTv6)
	|| (SVC_WAN_SRV_OTHER(cfg) && SVC_WAN_IS_ROUTE(cfg) && (cfg->npt == 1))
#endif
	)
    {
        if(1 == bind_flag)
        {
            if(SVC_WAN_IS_BRIDGE(cfg))
            {
                strcat(info->bind_ifType_list, ",I_B");
            }
            else
            {
                strcat(info->bind_ifType_list, ",I_R");
            }
        }
        else
        {
            if(SVC_WAN_IS_BRIDGE(cfg))
            {
                strcpy(info->bind_ifType_list, "I_B");
            }
            else
            {
                strcpy(info->bind_ifType_list, "I_R");
            }
        }
    }
    else
    {
        if(1 == bind_flag)
        {
            if(SVC_WAN_IS_BRIDGE(cfg))
            {
                strcat(info->bind_ifType_list, ",O_B");
            }
            else
            {
                strcat(info->bind_ifType_list, ",O_R");
            }
        }
        else
        {
            if(SVC_WAN_IS_BRIDGE(cfg))
            {
                strcpy(info->bind_ifType_list, "O_B");
            }
            else
            {
                strcpy(info->bind_ifType_list, "O_R");
            }
        }
    }
    
    SVC_WAN_ERROR_INFO("info->bind_ifType_list = %s, info->bind_ifName_list = %s, info->bind_lanif_list = %s\n", info->bind_ifType_list, info->bind_ifName_list, info->bind_lanif_list);
    info->if_index  = obj->idx;
    info->mode_flag = SVC_WAN_IS_BRIDGE(cfg);

    return 0;
}

/******************************************************************************************/
/******************************************************************************************/
void set_port_binding_info2(char *path, int action)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    port_bind_info_t bind_info;
    char cmd[SVC_WAN_BUF_256_LEN];
    int ret = 0;


    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }

    cfg = &obj->cfg;

    if((E_ADD_PORT_BIND_RULE == action) && (!SVC_WAN_IS_ACTIVE(cfg)))
    {
        return ;
    }

    memset(&bind_info, 0, sizeof(bind_info));
    ret = svc_wan_get_bind_info2(obj, &bind_info);
#if defined(TCSUPPORT_CT_JOYME2)
    get_other_wan_bind(obj, action);
#endif
    if(0 != ret)
    {
        return ;
    }

    switch (action) 
    {
        case E_ADD_PORT_BIND_RULE:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "portbindcmd addgroup %d %s %s", bind_info.if_index, bind_info.bind_ifName_list, bind_info.bind_ifType_list);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "portbindcmd enable");
            svc_wan_execute_cmd(cmd);

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/accesslimitcmd addbindinfo %d %d %s", bind_info.if_index, bind_info.mode_flag, bind_info.bind_lanif_list);
            svc_wan_execute_cmd(cmd);
#endif
            break;
        }
        case E_DEL_PORT_BIND_RULE:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "portbindcmd delgroup %d", bind_info.if_index);
            svc_wan_execute_cmd(cmd);	
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/usr/bin/accesslimitcmd deletebindinfo %d", bind_info.if_index);
            svc_wan_execute_cmd(cmd);	
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

/******************************************************************************************/
/******************************************************************************************/
/*
ip_type: 1 IPv4, 2 IPv6, 3 ALL
*/
void set_port_binding_info(wan_entry_obj_t* wan,int portbind_flag, int ip_type)
{
    int i=0;
    char cmd[SVC_WAN_BUF_256_LEN] = {0};
    char node[SVC_WAN_BUF_32_LEN] = {0};
    char lan_ip6[MAX_WAN_IPV6_ADDR_LEN] = {0};
    char lan_ip6_flag = 0;
    char lan_prefix6[SVC_WAN_BUF_16_LEN] = {0};
    char lan_prefix6_flag = 0;
    char get_lan_v6_info = 0;
    char def_route_index_v4[SVC_WAN_BUF_16_LEN] = {0};
    int i_def_route_index_v4 = -1;
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char dev_name[MAX_WAN_DEV_NAME_LEN] = {0};
    char ip_flag = 0;
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN] = {0};
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN] = {0};

#if defined(TCSUPPORT_CT_PPPOEPROXY)
    int flag[16];
    char *pt = NULL;
    char *output = NULL;
#endif

    unsigned char add_linkroute[SVC_MAX_WAN_INTF_NUMBER];
    memset(add_linkroute, 0, sizeof(add_linkroute));

	/* ip_type should be ALL when portbing flag process ALL. */
	if ( 1 == portbind_flag ) 
		ip_type = PORT_BIND_OP_ALL;

    /*first delete ip rule*/
    if(portbind_flag || (wan && wan->cfg.bindbit))
    {
	    for (i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
	    {
	    	if(1 == portbind_flag)
    		{
    			portbind_lib_del_route("all",bl_map_data[i].realif);
    		}
			else if((SVC_WAN_ATTR_VERSION_IPV4 == wan->cfg.version) 
				&& (wan->cfg.bindbit & (1<<i)))
			{
				portbind_lib_del_route("v4",bl_map_data[i].realif);
			}
			else if( (SVC_WAN_ATTR_VERSION_IPV6 == wan->cfg.version)
				&& (wan->cfg.bindbit & (1<<i)))
			{
				portbind_lib_del_route("v6",bl_map_data[i].realif);
			}
			else
			{
				if ( PORT_BIND_OP_V4 == ip_type )
					portbind_lib_del_route("v4", bl_map_data[i].realif);
				else if ( PORT_BIND_OP_V6 == ip_type )
					portbind_lib_del_route("v6", bl_map_data[i].realif);
				else 
					portbind_lib_del_route("all", bl_map_data[i].realif);
			}
	    }
	}

    /* get ipv6 lan info from node.*/
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", LAN_ENTRY0_NODE);

    if(cfg_get_object_attr(node, "IP6", lan_ip6, sizeof(lan_ip6)) > 0)
    {
        lan_ip6_flag = 1; 
    }

    if(cfg_get_object_attr(node, "PREFIX6", lan_prefix6, sizeof(lan_prefix6)) > 0)
    {
        lan_prefix6_flag = 1; 
    }

    if(lan_ip6_flag && lan_prefix6_flag)
    {
        get_lan_v6_info = 1;
    }

    //get default route for ipv4 
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_COMMON_NODE);
    memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
    if((cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) > 0) \
        && (strlen(def_route_index_v4) > 0) && (0 != strcmp(def_route_index_v4, "N/A")))
    {
        i_def_route_index_v4 = atoi(def_route_index_v4);
    }

    //add ip rule
    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        cfg = &obj->cfg;
        if(SVC_WAN_IS_BRIDGE(cfg))
        {
            continue;
        }

        add_linkroute[obj->idx] = 0;
        //convert  dslite to ipv6
        if(SVC_WAN_ATTR_VERSION_IPV4 == cfg->version)
        {
            ip_flag = SVC_WAN_ATTR_VERSION_IPV4;
        }
        else if(SVC_WAN_ATTR_VERSION_IPV6 == cfg->version)
        {
            ip_flag = SVC_WAN_ATTR_VERSION_IPV6;
        }
        else if(SVC_WAN_ATTR_VERSION_BOTH == cfg->version)
        {
            ip_flag = SVC_WAN_ATTR_VERSION_BOTH;
        }
        else
        {
            continue;
        }
#if defined(TCSUPPORT_CT_DSLITE)
        if(SVC_WAN_DSLITE_ENABLE(cfg) && SVC_WAN_SRV_INTERNET(cfg))
        {
            ip_flag = SVC_WAN_ATTR_VERSION_IPV6;
        }
#endif
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 
        if((SVC_WAN_ATTR_VERSION_IPV4 == ip_flag) || (SVC_WAN_ATTR_VERSION_BOTH == ip_flag))
        {
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
        	if ( 0 == (obj->flag & SVC_WAN_FLAG_UPV4) )
				set_portbinding_info_for_ondemand_wan(obj->idx/SVC_WAN_MAX_PVC_NUM, obj->idx%SVC_WAN_MAX_ENTRY_NUM, obj->dev);
			else
#endif
			{
            memset(gate_way_v4, 0, sizeof(gate_way_v4));
            cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4));
        }
        }

        if((SVC_WAN_ATTR_VERSION_IPV6 == ip_flag) || (SVC_WAN_ATTR_VERSION_BOTH == ip_flag))
        {
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
			if ( 0 == (obj->flag & SVC_WAN_FLAG_UPV6) )
				set_portbinding_info_for_ondemand_wan(obj->idx/SVC_WAN_MAX_PVC_NUM, obj->idx%SVC_WAN_MAX_ENTRY_NUM, obj->dev);
			else
#endif
			{
            memset(gate_way_v6, 0, sizeof(gate_way_v6));
            cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6));
        }
        }

        if ((0 == (obj->flag & SVC_WAN_FLAG_UPV4)) && (0 == (obj->flag & SVC_WAN_FLAG_UPV6)))
        {
            continue;
        }

        if((0 >= strlen(gate_way_v4)) && (0 >= strlen(gate_way_v6)))
        {
            continue;
        }

        memset(dev_name, 0, sizeof(dev_name));
        svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
		if(cfg->bindbit)
		{
        	for(i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
	        {
	            if (0 == (cfg->bindbit & (1<<i)))
	            {
	                continue;
	            }
#if defined(TCSUPPORT_CT_PPPOEPROXY) && !defined(TCSUPPORT_CT_PON_JS)
	            if ((flag[i] == 1) && (SVC_WAN_PROXY_ENABLE(cfg)))
	            {
	                break;
	            }
#endif

	            // add ip rule for every wan interface
	            if((obj->flag & SVC_WAN_FLAG_UPV4) && (strlen(gate_way_v4) > 0)
					&& ( ip_type & PORT_BIND_OP_V4 ) )
	            {
	                if((obj->idx != i_def_route_index_v4) &&  (0 == add_linkroute[obj->idx]))
	                {
	                    add_linkroute[obj->idx] = 1;
	                    dealwith_link_local_route(dev_name, obj->idx, E_DEFAULT_ROUTE_SWITCH);
	                }
	                /* for ipv4 */
	                portbind_lib_add_route("v4",bl_map_data[i].realif,dev_name,gate_way_v4,NULL);
	            }

	            if((obj->flag & SVC_WAN_FLAG_UPV6) && (strlen(gate_way_v6) > 0)
					&& ( ip_type & PORT_BIND_OP_V6 ) )
	            {
	                portbind_lib_add_route("v6",bl_map_data[i].realif,dev_name,NULL,gate_way_v6);
#if defined(TCSUPPORT_CT_DSLITE)
	                if(SVC_WAN_DSLITE_ENABLE(cfg) && SVC_WAN_SRV_INTERNET(cfg))
	                {
	                    /* record default for ds-lite tunnel, can't do portbindcmd because dslite interface maybe isn't up.*/
	                    obj->dslite_state |= DS_BINDSTATE_UP;
	                }
#endif
	            }

	            if (get_lan_v6_info)
	            {
	                memset(cmd, 0, sizeof(cmd));
	                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add %s/%s dev br0 table %s", \
	                    lan_ip6, lan_prefix6, bl_map_data[i].realif);
	                svc_wan_execute_cmd(cmd);
	            }
	            else
	            {
	                memset(cmd, 0, sizeof(cmd));
	                snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add fe80::1/64 dev br0 table %s", \
	                    bl_map_data[i].realif);
	                svc_wan_execute_cmd(cmd);
	            }
	        }
    	}
        if (1 == add_linkroute[obj->idx])
        {
            add_linkroute[obj->idx] = 0;
            dealwith_link_local_route(dev_name, obj->idx, E_NOT_DEFAULT_ROUTE);
        }
    }

#if defined(TCSUPPORT_CT_L2TP_VPN) && defined(TCSUPPORT_CMCCV2)
    char vpnUsrID[128];
    /* need insert VPN fwmark rule before LAN fwmark. */
    memset(vpnUsrID, 0, sizeof(vpnUsrID));
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), VPN_ENTRY0_NODE);
    if(cfg_get_object_attr(node, "userId", vpnUsrID, sizeof(vpnUsrID)) > 0 \
        && 0 != vpnUsrID[0])
    {
        svc_wan_execute_cmd("/usr/bin/ip rule del fwmark 0x6e0000/0x7f0000 table 300");
        svc_wan_execute_cmd("/usr/bin/ip rule add fwmark 0x6e0000/0x7f0000 table 300");
    }
#endif

    return ;
}

/***************************************************************************/
int find_if_in_pppoeproxymsg(char *realif, char *pvc)
{
    FILE *fp = NULL;
    char temp[64] = {0};
    int pid = 0;
    char pvcname[32] = {0};
    char lan_if[32] = {0};

    fp=fopen("/tmp/pppoeproxymsg","r+");
    if(fp == NULL)
    {
        /*wait for discuss*/
        return 0;
    }

    SVC_WAN_DEBUG_INFO("realif = %s, pvc = %s.\n", realif, pvc);

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        sscanf(temp,"%s %d %s",pvcname, &pid, lan_if);
        if((strcmp(lan_if, realif) == 0) && (strcmp(pvcname,pvc) != 0))
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void set_blackhole_route(char* path)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char node[SVC_WAN_BUF_32_LEN] = {0};
    char pvc_name[SVC_WAN_BUF_16_LEN] = {0};
    char gate_way_v6[MAX_WAN_IPV6_ADDR_LEN] = {0};
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN] = {0};
    char cmd[SVC_WAN_BUF_256_LEN] = {0};
    char dev_name[MAX_WAN_DEV_NAME_LEN] = {0};
    int i = 0;
    int j = 0;
    char lanMk[SVC_WAN_BUF_32_LEN] = {0};
    char lanTabId[SVC_WAN_BUF_16_LEN] = {0};
    if(NULL == path)
    {
        return ;
    }
    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }
    cfg = &obj->cfg;

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 
    if ((SVC_WAN_ATTR_VERSION_IPV4 == cfg->version) || (SVC_WAN_ATTR_VERSION_BOTH == cfg->version))
    {
        memset(gate_way_v4, 0, sizeof(gate_way_v4));
        cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4));
    }

    if((SVC_WAN_ATTR_VERSION_IPV6 == cfg->version) || (SVC_WAN_ATTR_VERSION_BOTH == cfg->version))
    {
        memset(gate_way_v6, 0, sizeof(gate_way_v6));
        cfg_get_object_attr(node, "GateWay6", gate_way_v6, sizeof(gate_way_v6));
    }

    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));

    memset(pvc_name, 0, sizeof(pvc_name));
    snprintf(pvc_name, sizeof(pvc_name), "PVC%d_Entry%d", (obj->idx / SVC_WAN_MAX_ENTRY_NUM + 1), (obj->idx % SVC_WAN_MAX_ENTRY_NUM + 1));
    for(i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
        if ((cfg->bindbit & (1<<i)) && (!(find_if_in_pppoeproxymsg(bl_map_data[i].realif, pvc_name)))) 
        {
            /* add ip rule for wan interface */
            if ('\0' != gate_way_v4[0]) 
            {
                /* for ipv4 */
				portbind_lib_del_route("all",bl_map_data[i].realif);

				portbind_lib_add_route("v4",bl_map_data[i].realif,dev_name, gate_way_v4,NULL);

                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route del default table %s", bl_map_data[i].realif);
                svc_wan_execute_cmd(cmd);

                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add blackhole default table %s", \
                    bl_map_data[i].realif);
                svc_wan_execute_cmd(cmd);
            }
            else if('\0' != gate_way_v6[0])
            {
                /* for ipv6 */
				portbind_lib_del_route("all",bl_map_data[i].realif);

				portbind_lib_add_route("v6",bl_map_data[i].realif,dev_name, NULL,gate_way_v6);

                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route del default table %s", bl_map_data[i].realif);
                svc_wan_execute_cmd(cmd);

                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add blackhole default table %s", bl_map_data[i].realif);
                svc_wan_execute_cmd(cmd);
            }
            else
            {
                for (j = 0; j < sizeof(ifpr) / sizeof(ifpr[0]); j++) 
                {
                    if (0 == strcmp(ifpr[j].ifName, bl_map_data[i].realif)) 
                    {
                        strncpy(lanMk, ifpr[j].mark, sizeof(lanMk) - 1);
                        strncpy(lanTabId, ifpr[j].tabid, sizeof(lanTabId) - 1);
                        break;
                    }
                }

                portbind_lib_del_route("all",bl_map_data[i].realif);

                if(SVC_WAN_ATTR_VERSION_IPV4 == cfg->version)
                {
                    /* 1. flush route table */
                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush table %s 2>/dev/null", lanTabId);
                    svc_wan_execute_cmd(cmd);

                    /* 2. flush rule */
                    memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark %s/0x%x table %s 2>/dev/null", \
						lanMk, LANIF_MASK, lanTabId);
                    svc_wan_execute_cmd(cmd);

                    memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd),"/usr/bin/ip rule add fwmark %s/0x%x table %s 2>/dev/null", \
						lanMk, LANIF_MASK, lanTabId);
                    svc_wan_execute_cmd(cmd);

                    /* 3. add route */
                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add 192.168.1.0/24 via 192.168.1.1 dev br0 table %s 2>/dev/null", lanTabId);
                    svc_wan_execute_cmd(cmd);

                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add blackhole default table %s 2>/dev/null", \
                        lanTabId);
                    svc_wan_execute_cmd(cmd);

                    /* ipv4 end */
                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush cache 2>/dev/null");
                    svc_wan_execute_cmd(cmd);
                }

                if((SVC_WAN_ATTR_VERSION_IPV6 == cfg->version) || (SVC_WAN_ATTR_VERSION_BOTH == cfg->version))
                {
                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route flush table %s 2>/dev/null", lanTabId);
                    svc_wan_execute_cmd(cmd);


                    memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule del fwmark %s/0x%x table %s 2>/dev/null",\
						lanMk, LANIF_MASK, lanTabId);
                    svc_wan_execute_cmd(cmd);

                    memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 rule add fwmark %s/0x%x table %s 2>/dev/null", \
						lanMk, LANIF_MASK, lanTabId);
                    svc_wan_execute_cmd(cmd);

                    memset(cmd, 0, sizeof(cmd));
                    snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route add blackhole default table %s 2>/dev/null", \
                        lanTabId);
                    svc_wan_execute_cmd(cmd);
                }
            }
        }
    }

    return ;
}

/***********************************************************/
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
void add_dhcprealy_port_binding_rule(wan_entry_obj_t* obj, char* RealyInputTabe, char* RealyForwardTabe)
#else
void add_dhcprealy_port_binding_rule(wan_entry_obj_t* obj, char* RealyForwardTabe)
#endif
{
    int i = 0; 
    char bind_sw[SVC_WAN_BUF_8_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char ebtablesCmd[SVC_WAN_BUF_128_LEN];
    wan_entry_cfg_t* cfg = NULL;

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
    if((NULL == RealyInputTabe) || (0 >= strlen(RealyInputTabe)))
    {
        SVC_WAN_ERROR_INFO("pointer can't NULL or length need > 0.\n");
        return ;
    }
#endif

    if((NULL == obj) || (NULL == RealyForwardTabe) || (0 >= strlen(RealyForwardTabe)))
    {
        SVC_WAN_ERROR_INFO("pointer can't NULL or length need > 0.\n");
        return ;
    }

    cfg = &obj->cfg;

    for (i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), VLANBIND_ENTRY_NODE, (i+1));	
        memset(bind_sw, 0, sizeof(bind_sw));
        if(cfg_get_object_attr(node, "Active", bind_sw, sizeof(bind_sw)) < 0)
        {
            strcpy(bind_sw, "No");
        }
        
        if(cfg->bindbit & (1<<i))
        {
            //DHCPRealy enable
            if(SVC_WAN_ATTR_DHCPRELAY_ENABLE == cfg->dhcp_realy)
            {   
#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
                    EBTABLES_FILTER, RealyInputTabe,  bl_map_data[i].realif);
                svc_wan_execute_cmd(ebtablesCmd);
#endif

                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                snprintf(ebtablesCmd, sizeof(ebtablesCmd),  "%s -A %s -i %s -o %s -p IPv4 --ip-proto 17 --ip-dport 67 -j ACCEPT", \
                    EBTABLES_FILTER, RealyForwardTabe,  bl_map_data[i].realif, obj->dev);                        
                svc_wan_execute_cmd(ebtablesCmd);

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
                    EBTABLES_FILTER, RealyInputTabe,  bl_map_data[i].realif);
                svc_wan_execute_cmd(ebtablesCmd);
#endif

                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -o %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j ACCEPT", \
                    EBTABLES_FILTER, RealyForwardTabe,  bl_map_data[i].realif, obj->dev);                      
                svc_wan_execute_cmd(ebtablesCmd);
            }
        }
        else
        {
            if((SVC_WAN_ATTR_DHCPRELAY_ENABLE == cfg->dhcp_realy) && (0 == strcasecmp(bind_sw, "Yes")))
            {
                /* dhcp relay enable */
                /* for IPv4 */
                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                
                snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -o %s -p IPv4 --ip-proto 17 --ip-dport 67 -j ACCEPT", \
                        EBTABLES_FILTER, RealyForwardTabe,  bl_map_data[i].realif, obj->dev);
                
                svc_wan_execute_cmd(ebtablesCmd);

                /* for IPv6 */
                memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
                
                snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -o %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j ACCEPT", \
                    EBTABLES_FILTER, RealyForwardTabe,   bl_map_data[i].realif, obj->dev);   
                
                svc_wan_execute_cmd(ebtablesCmd);
            }
        }
    }

    return ;
}

void add_dhcp_port_filter_rule(wan_entry_obj_t* obj, char* PortFilterInputTable, 
	char* PortFilterOutputTable)
{
    int i = 0;
    char bind_sw[SVC_WAN_BUF_8_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char ebtablesCmd[SVC_WAN_BUF_128_LEN];
    wan_entry_cfg_t* cfg = NULL;
	int pvc = 0;
	int entry = 0;
	int mark = 0;

    if((NULL == obj) || (NULL == PortFilterInputTable) || (0 >= strlen(PortFilterInputTable)))
    {
        SVC_WAN_ERROR_INFO("pointer can't NULL or length need > 0.\n");
        return ;
    }

	pvc = obj->idx / 8;
	entry = obj->idx % 8;

#if defined(TCSUPPORT_CMCC)
	mark = (pvc * 8 + entry + 1) << 16;
#else
	mark = (pvc * 8 + 1) << 16;
#endif

	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -M mark_m -A %s -p IPv4 --ip-proto udp --ip-dport 67 --mark 0x%x/0x7f0000 -j DROP\n",\
			EBTABLES_FILTER, PortFilterInputTable, mark);
	svc_wan_execute_cmd(ebtablesCmd);

	snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -M mark_m -A %s -p IPv6 --ip6-proto udp --ip6-dport 547 --mark 0x%x/0x7f0000 -j DROP\n",\
			EBTABLES_FILTER, PortFilterInputTable, mark);
	svc_wan_execute_cmd(ebtablesCmd);
	
    cfg = &obj->cfg;
    for (i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
#if defined(TCSUPPORT_CT_VLAN_BIND)
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), VLANBIND_ENTRY_NODE, (i+1));
        memset(bind_sw, 0, sizeof(bind_sw));
        if(cfg_get_object_attr(node, "Active", bind_sw, sizeof(bind_sw)) < 0)
        {
            snprintf(bind_sw, sizeof(bind_sw), "No");
        }
#endif
        if((cfg->bindbit & (1<<i)) && (0 == strcasecmp(bind_sw, "No")) )
        {
            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
                EBTABLES_FILTER, PortFilterInputTable, bl_map_data[i].realif);
            
            svc_wan_execute_cmd(ebtablesCmd);

            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
                EBTABLES_FILTER, PortFilterInputTable, bl_map_data[i].realif);
			svc_wan_execute_cmd(ebtablesCmd);
            
            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -i %s -p IPv6 --ip6-proto 58 --ip6-icmpv6type 133 -j DROP", \
                EBTABLES_FILTER, PortFilterInputTable, bl_map_data[i].realif);
            svc_wan_execute_cmd(ebtablesCmd);

            memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
            snprintf(ebtablesCmd, sizeof(ebtablesCmd), "%s -A %s -o %s -p IPv6 --ip6-proto 58 --ip6-icmpv6type 134 -j DROP", \
                EBTABLES_FILTER, PortFilterOutputTable, bl_map_data[i].realif);
            svc_wan_execute_cmd(ebtablesCmd);
        }
    }

    return ;
}

#if defined(TCSUPPORT_CT_DSLITE)
void set_portbinding_info_for_dslite(wan_entry_obj_t* obj)
{
    int i = 0;
    wan_entry_cfg_t* cfg = NULL;
    char cmd[SVC_WAN_BUF_128_LEN];
    char itf_name[MAX_WAN_DEV_NAME_LEN];

    if(NULL == obj)
    {
        SVC_WAN_DEBUG_INFO("pointer can't be NULL.\n");
        return ;
    }
    cfg = &obj->cfg;
    SVC_WAN_DEBUG_INFO("obj->path = %s.\n", obj->path);

    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj, itf_name, sizeof(itf_name));
    for(i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++)
    {
        if(cfg->bindbit & (1<<i))
        {
            // add default for ds-lite tunnel
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd,  sizeof(cmd), "portbindcmd addroute v4 %s ds.%s ''", bl_map_data[i].realif, itf_name);
            svc_wan_execute_cmd(cmd);
        }
    }

    return ;
}
#endif

int set_port_bind_mask(int index)
{
	bindrule_mark |= 1 << index;
	return 0;
}

int clear_port_bind_mask(int index)
{
	bindrule_mark &= ~(1 << index);
	return 0;
}

int check_port_bind_mask(int index)
{
	if( bindrule_mark & (1 << index) )
		return 1;
	else
		return 0;
}

int pre_check_lanport_bind( void )
{
	int i = 0, j = 0, k = 0;
	char path[32] = {0};
	char Active[8] = {0};
	char serviceList[32] = {0};
	char dhcp_enable[8] = {0};
	char wanmode[16] = {0};
	char chain_name[32] = {0};
	char output_chain_name[32] = {0};
	char tmp[16] = {0};
	char attr[16] = {0};
	char cmd[128] = {0};
	char node[32] = {0};
	char vlanbind_active[16] = {0};
	int mark = 0;
	
	for(i = 0; i < SVC_WAN_MAX_PVC_NUM; i++)
	{
		for( k = 0; k < SVC_WAN_MAX_ENTRY_NUM; k++ )
		{
			memset(Active, 0, sizeof(Active));
			memset(serviceList, 0, sizeof(serviceList));
			memset(dhcp_enable, 0, sizeof(dhcp_enable));
			memset(wanmode, 0, sizeof(wanmode));
			snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE, i + 1, k + 1);
			cfg_obj_get_object_attr(path, "Active", 0, Active, sizeof(Active));
			if( strcmp(Active, "Yes") )
				continue;

			cfg_obj_get_object_attr(path, "ServiceList", 0, serviceList, sizeof(serviceList));
			cfg_obj_get_object_attr(path, "DHCPEnable", 0, dhcp_enable, sizeof(dhcp_enable));
			cfg_obj_get_object_attr(path, "WanMode", 0, wanmode, sizeof(wanmode));
			if( !strcmp(dhcp_enable, "0") && !strcmp(wanmode, "Bridge")
				&& (!strcmp(serviceList, "INTERNET") || !strcmp(serviceList, "OTHER")
				|| !strcmp(serviceList, "IPTV") || !strcmp(serviceList, "OTT")))
			{
				memset(chain_name, 0, sizeof(chain_name));
				snprintf(chain_name, sizeof(chain_name), "DhcpFilterInputnas%d_%d", i, k);
				memset(output_chain_name, 0, sizeof(output_chain_name));
				snprintf(output_chain_name, sizeof(output_chain_name), "DhcpFilterOutputnas%d_%d", i, k);
				
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, chain_name);
				svc_wan_execute_cmd(cmd);
				
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -P %s RETURN", EBTABLES_FILTER, chain_name);
				svc_wan_execute_cmd(cmd);
				
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -A INPUT -j %s", EBTABLES_FILTER, chain_name);
				svc_wan_execute_cmd(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -N %s 2>/dev/null", EBTABLES_FILTER, output_chain_name);
				svc_wan_execute_cmd(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -P %s RETURN", EBTABLES_FILTER, output_chain_name);
				svc_wan_execute_cmd(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -A OUTPUT -j %s", EBTABLES_FILTER, output_chain_name);
				svc_wan_execute_cmd(cmd);
#if defined(TCSUPPORT_CMCCV2)
				mark = (i * 8 + k + 1) << 16;
#else
				mark = (i * 8 + 1) << 16;
#endif
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -M mark_m -A %s -p IPv4 --ip-proto udp --ip-dport 67 --mark 0x%x/0x7F0000 -j DROP\n",\
						EBTABLES_FILTER, chain_name, mark);
				svc_wan_execute_cmd(cmd);

				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "%s -M mark_m -A %s -p IPv6 --ip6-proto udp --ip6-dport 547 --mark 0x%x/0x7F0000 -j DROP\n",\
						EBTABLES_FILTER, chain_name, mark);
				svc_wan_execute_cmd(cmd);
				
				for(j = 0; j < sizeof(bl_map_data) / sizeof(bl_map_data[0]); j++)
				{
#if defined(TCSUPPORT_CT_VLAN_BIND)
					memset(node, 0, sizeof(node));
					snprintf(node, sizeof(node), VLANBIND_ENTRY_NODE, (j+1));
					memset(vlanbind_active, 0, sizeof(vlanbind_active));
					if(cfg_get_object_attr(node, "Active", vlanbind_active, sizeof(vlanbind_active)) < 0)
					{
						snprintf(vlanbind_active, sizeof(vlanbind_active), "No");
					}
#endif
					memset(tmp, 0, sizeof(tmp));
					cfg_obj_get_object_attr(path, bl_map_data[j].bindif, 0, tmp, sizeof(tmp));
					if( !strcmp(tmp, "Yes") && !strcmp(vlanbind_active, "No") )
					{
						
						memset(cmd, 0, sizeof(cmd));
	            		snprintf(cmd, sizeof(cmd), "%s -A %s -i %s -p IPv4 --ip-proto 17 --ip-dport 67 -j DROP", \
	                		EBTABLES_FILTER, chain_name, bl_map_data[j].realif);
			            svc_wan_execute_cmd(cmd);

			            memset(cmd, 0, sizeof(cmd));
			            
			            snprintf(cmd, sizeof(cmd), "%s -A %s -i %s -p IPv6 --ip6-proto 17 --ip6-dport 547 -j DROP", \
			                EBTABLES_FILTER,  chain_name, bl_map_data[j].realif);
			            svc_wan_execute_cmd(cmd);
						
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), "%s -A %s -i %s -p IPv6 --ip6-proto 58 --ip6-icmpv6type 133 -j DROP", \
			                EBTABLES_FILTER, chain_name, bl_map_data[j].realif);
			            svc_wan_execute_cmd(cmd);
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), "%s -A %s -o %s -p IPv6 --ip6-proto 58 --ip6-icmpv6type 134 -j DROP", \
							EBTABLES_FILTER, output_chain_name, bl_map_data[i].realif);
						svc_wan_execute_cmd(cmd);
					}
				}
				set_port_bind_mask(i * 8 + k);
			}
		}
	}
	return 0;
}


