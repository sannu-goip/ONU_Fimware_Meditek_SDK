#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_link_list.h"
#include "msg_notify.h"
#include "misc_srv.h"
#include "utility.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include <ecnt_event_global/ecnt_event_global.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#endif
/***********************************************************************/
#if defined(TCSUPPORT_CT_PMINFORM)
static int update_Cwmp_Info(char *wanip, int evt);
#endif
static void del_if_from_conf(char *if_name);
static void update_cwmp_route_node(char *devname, char *path, int evt, int cwmp_flag);
static void set_tr069_nat(char *path);
static void update_cwmp_route(char *path, int evt);
/***********************************************************************/
void update_wan_up_time(wan_entry_obj_t* obj, int evt)
{
    struct timespec cur_time;
    char tmpSec[SVC_WAN_BUF_32_LEN];
    char tmpNSec[SVC_WAN_BUF_32_LEN];

    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    memset(tmpSec, 0, sizeof(tmpSec));
    memset(tmpNSec, 0, sizeof(tmpNSec));
    snprintf(tmpSec, sizeof(tmpSec), "%ld",  cur_time.tv_sec);
    snprintf(tmpNSec, sizeof(tmpSec), "%ld",  cur_time.tv_nsec);

    switch (evt)
    {
        case EVT_WAN_BRIDGE_UP:
        {
            cfg_set_object_attr(obj->path, "uptime_sec",    tmpSec);
            cfg_set_object_attr(obj->path, "uptime_nsec",   tmpNSec);
            cfg_set_object_attr(obj->path, "uptime6_sec",   tmpSec);
            cfg_set_object_attr(obj->path, "uptime6_nsec",  tmpNSec);
            break;
        }
        case EVT_WAN_BRIDGE_DOWN:
        {
            if (cfg_query_object(obj->path,NULL,NULL) <= 0)
            {
                break;
            }
            cfg_set_object_attr(obj->path, "uptime_sec",    "-1");
            cfg_set_object_attr(obj->path, "uptime_nsec",   "-1");
            cfg_set_object_attr(obj->path, "uptime6_sec",   "-1");
            cfg_set_object_attr(obj->path, "uptime6_nsec",  "-1");
            break;
        }
        case EVT_WAN_CONN_GETV4:
        {
            cfg_set_object_attr(obj->path, "uptime_sec",    tmpSec);
            cfg_set_object_attr(obj->path, "uptime_nsec",   tmpNSec);
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
            if (cfg_query_object(obj->path,NULL,NULL) <= 0)
            {
                break;
            }
            cfg_set_object_attr(obj->path, "uptime_sec",    "-1");
            cfg_set_object_attr(obj->path, "uptime_nsec",   "-1");
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            cfg_set_object_attr(obj->path, "uptime6_sec",   tmpSec);
            cfg_set_object_attr(obj->path, "uptime6_nsec",  tmpNSec);
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            if (cfg_query_object(obj->path,NULL,NULL) <= 0)
            {
                break;
            }
            cfg_set_object_attr(obj->path, "uptime6_sec",   "-1");
            cfg_set_object_attr(obj->path, "uptime6_nsec",  "-1");
            break;
        }
        default:
        {
            break;
        }
    }

    return ;
}

#if defined(TCSUPPORT_CT_UBUS)
void update_br_wan_up_time(int evt)
{
    struct timespec cur_time;
    char tmpSec[SVC_WAN_BUF_32_LEN];
    char tmpNSec[SVC_WAN_BUF_32_LEN];

    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    memset(tmpSec, 0, sizeof(tmpSec));
    memset(tmpNSec, 0, sizeof(tmpNSec));
    snprintf(tmpSec, sizeof(tmpSec), "%ld",  cur_time.tv_sec);
    snprintf(tmpNSec, sizeof(tmpSec), "%ld",  cur_time.tv_nsec);

    switch (evt)
    {
        case EVT_WAN_AP_BRIDGE_GET4:
        {
            cfg_set_object_attr(WANINFO_BRENTRY_NODE, "uptime_sec",    tmpSec);
            cfg_set_object_attr(WANINFO_BRENTRY_NODE, "uptime_nsec",   tmpNSec);
            break;
        }
        case EVT_WAN_AP_BRIDGE_LOST4:
        {
            if (cfg_query_object(WANINFO_BRENTRY_NODE,NULL,NULL) <= 0)
            {
                break;
            }
            cfg_set_object_attr(WANINFO_BRENTRY_NODE, "uptime_sec",    "-1");
            cfg_set_object_attr(WANINFO_BRENTRY_NODE, "uptime_nsec",   "-1");
            break;
        }
        default:
        {
            break;
        }
    }

    return ;
}
#endif

#if defined(TCSUPPORT_CT_PMINFORM)
static int update_Cwmp_Info(char *wanip, int evt)
{
    char node[SVC_WAN_BUF_32_LEN];

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", PMINFORM_ENTRY_NODE);

    cfg_set_object_attr(node, "CwmpWanIP",	(EVT_WAN_CONN_GETV4 == evt)?  wanip : "0.0.0.0");

    return 0;
}
#endif

static void del_if_from_conf(char *if_name)
{
    FILE *fp = NULL, *fp2 = NULL;
    char line[64];
	int res = 0;

    memset(line, 0, sizeof(line));
    fp = fopen(TRO69_IF_CONF, "r");
    if (NULL == fp) 
    {
        return;
    }

    fp2 = fopen(TRO69_IF_CONF2, "w");
    if (NULL == fp2) 
    {
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp)) 
    {
        if (NULL == strstr(line, if_name)) 
        {
            fputs(line, fp2);
        }
    }

    fclose(fp);
    fclose(fp2);
    unlink(TRO69_IF_CONF);
    res = rename(TRO69_IF_CONF2, TRO69_IF_CONF);
	if(res){
		/*do nothing,just for compile*/
	}
    return;
}

static void update_cwmp_route_node(char *devname, char *path, int evt, int cwmp_flag)
{
    int i = 0;
    char node[SVC_WAN_BUF_32_LEN], node_waninfo[SVC_WAN_BUF_32_LEN];
    char line[SVC_WAN_BUF_64_LEN];
    char if_name[SVC_WAN_BUF_32_LEN];
    char if_gateway[SVC_WAN_BUF_32_LEN];
    char if_name_attr[SVC_WAN_BUF_32_LEN];
    char if_gateway_attr[SVC_WAN_BUF_32_LEN];
    char if_status_attr[SVC_WAN_BUF_32_LEN];
    char route_num[SVC_WAN_BUF_16_LEN];	
    FILE *fp = NULL;	
    wan_entry_obj_t* obj = NULL;
    char w_ip_v4[MAX_WAN_IPV4_ADDR_LEN];
    char w_ip_v6[MAX_WAN_IPV6_ADDR_LEN];
	char wan_idx[32] = {0};
    int ret = 0;

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object in cwmp route note.\n");
        return ;
    }

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", CWMPROUTE_ENTRY_NODE);

    if (cfg_query_object(node,NULL,NULL) > 0)
    {
        cfg_delete_object(node);
    }

    if (cfg_query_object(node,NULL,NULL) <= 0)
    {
        cfg_create_object(node);
    }

	if ( 1 == cwmp_flag )
	{
	    bzero(node_waninfo, sizeof(node_waninfo));
	    snprintf(node_waninfo, sizeof(node_waninfo), WANINFO_ENTRY_NODE, obj->idx + 1); 
	    cfg_set_object_attr(node, "IFName", devname);
	    if ( EVT_WAN_CONN_GETV6 == evt )
	    {
	       cfg_set_object_attr(node, "ifName0", devname);
	       cfg_set_object_attr(node, "Status6", "Up");
		   if ( cfg_get_object_attr(node_waninfo, "IP6", w_ip_v6, sizeof(w_ip_v6)) > 0
		   		&& 0 != w_ip_v6[0] )
		   		cfg_set_object_attr(node, "IP6", w_ip_v6);
	    }
	    else if ( EVT_WAN_CONN_LOSTV6 == evt )
	    {
	       cfg_set_object_attr(node, "Status6", "Down");
	       cfg_set_object_attr(node, "IP6", "");
	    }
	    else if ( EVT_WAN_CONN_GETV4 == evt )
	    {
	       cfg_set_object_attr(node, "Status", "Up");
	       if ( cfg_get_object_attr(node_waninfo, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0
		   		&& 0 != w_ip_v4[0] )
		   		cfg_set_object_attr(node, "IP", w_ip_v4);
#if defined(TCSUPPORT_CT_JOYME2)
			ret = send_cwmp_wan_status("Up", w_ip_v4);
			snprintf(wan_idx, sizeof(wan_idx), "%d", obj->idx + 1);
			cfg_set_object_attr(node, "WanIdx", wan_idx);
#endif
	    }
	    else if ( EVT_WAN_CONN_LOSTV4 == evt )
	    {
	       cfg_set_object_attr(node, "Status", "Down");
		   cfg_set_object_attr(node, "IP", "");
#if defined(TCSUPPORT_CT_JOYME2)
		   ret = send_cwmp_wan_status("Down", "");
		   cfg_set_object_attr(node, "WanIdx", "");
#endif
	    }
	}

    fp = fopen(TRO69_IF_CONF, "r");
    if (NULL == fp) 
    {
        return;
    }

    cfg_set_object_attr(node, "RouteNum", "0");
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), fp))
    {
        if(strlen(line) > 0)
        {
            sscanf(line, "%s %s", if_name, if_gateway);
            memset(if_name_attr, 0, sizeof(if_name_attr));
            memset(if_gateway_attr, 0, sizeof(if_gateway_attr));
            memset(if_status_attr, 0, sizeof(if_status_attr));
            snprintf(if_name_attr, sizeof(if_name_attr), "ifName%d", i);
            snprintf(if_gateway_attr, sizeof(if_gateway_attr), "ifGateway%d", i);
            snprintf(if_status_attr, sizeof(if_status_attr), "ifStatus%d", i);

            cfg_set_object_attr(node, if_name_attr,  if_name);
            cfg_set_object_attr(node, if_gateway_attr, if_gateway);
            cfg_set_object_attr(node, if_status_attr,  "new");

            i++;
            memset(line, 0, sizeof(line));
            memset(if_name, 0, sizeof(if_name));
            memset(if_gateway, 0, sizeof(if_gateway));
        }
    }

    /* update route num */
    snprintf(route_num, sizeof(route_num), "%d", i);
    cfg_set_object_attr(node, "RouteNum", route_num);
    fclose(fp);

    return;
}


static void set_tr069_nat(char *path)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char cmd[SVC_WAN_BUF_256_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];
    
    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }

    cfg = &obj->cfg;
    /* if the interface is TR069 type, remove nat rule */
    if(0 == strcmp(cfg->srv_list, "TR069"))
    {
        memset(dev_name, 0, sizeof(dev_name));
        svc_wan_intf_name(obj, dev_name, sizeof(dev_name));

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -D POSTROUTING -o %s -j MASQUERADE 2>/dev/null", IPTABLES_NAT, dev_name);
        svc_wan_execute_cmd(cmd);
    }

    return;
}


static void update_cwmp_route(char *path, int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char node[SVC_WAN_BUF_32_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];
	char ifname_attr[SVC_WAN_BUF_16_LEN] = {0};
    int iscwmp_wan = 0;

#if defined(TCSUPPORT_CT_PMINFORM)
    char wanip[MAX_WAN_IPV4_ADDR_LEN];
#endif

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }

    cfg = &obj->cfg;

    /*prepare info*/
    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
    if(EVT_WAN_CONN_GETV4 == evt)
    {
        memset(node, 0, sizeof(node));
        memset(gate_way_v4, 0, sizeof(gate_way_v4));
        snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);	
        if(cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) > 0)
        {
            if(0 >= strlen(gate_way_v4))
            {
                return ;
            }
        }
#if defined(TCSUPPORT_CT_PMINFORM)	
        memset(wanip, 0, sizeof(wanip));
        cfg_get_object_attr(node, "wanip", wanip, sizeof(wanip));
#endif
    }

    memset(cmd, 0, sizeof(cmd));
    if(SVC_WAN_SRV_E8C_TR069(cfg))
    {
    	iscwmp_wan = 1;
        if(EVT_WAN_CONN_GETV4 == evt)
        {
            snprintf(cmd, sizeof(cmd), "/bin/echo \"%s %s\" >> %s", dev_name, gate_way_v4, TRO69_IF_CONF);
            svc_wan_execute_cmd(cmd);
        }
#if defined(TCSUPPORT_CT_PMINFORM)
        update_Cwmp_Info(wanip, evt);
#endif
    }

    if(EVT_WAN_CONN_LOSTV4 == evt)
    {
        del_if_from_conf(dev_name);

		if(SVC_WAN_SRV_E8C_TR069(cfg))
		{
			cfg_set_object_attr(CWMPROUTE_ENTRY_NODE, "ifName0", "N/A");
		}
    }

    update_cwmp_route_node(dev_name, path, evt, iscwmp_wan);

    return;
}

/**********************************************************************/
static void set_pre_srv_rule(char* path, int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;	
    char dev_name[MAX_WAN_DEV_NAME_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char connreq_port[SVC_WAN_BUF_16_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }

    cfg = &obj->cfg;
    if(!SVC_WAN_SRV_E8C_TR069(cfg))
    {
        return ;
    }

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", CWMP_ENTRY_NODE);
    if(cfg_get_object_attr(node, "conReqPort", connreq_port, sizeof(connreq_port)) < 0)
    {
        strcpy(connreq_port, "7547");
    }

    if(0 >= strlen(connreq_port))
    {
        strcpy(connreq_port, "7547");
    }

    /*prepare info*/
    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
    switch(evt)
    {
        case EVT_WAN_CONN_GETV4:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "%s -D PRE_SERVICE -i %s -p tcp --dport %s -j ACCEPT 2>/dev/null", \
                IPTABLES_NAT, dev_name, connreq_port);
            svc_wan_execute_cmd(cmd);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "%s -A PRE_SERVICE -i %s -p tcp --dport %s -j ACCEPT", \
                IPTABLES_NAT, dev_name, connreq_port);
            svc_wan_execute_cmd(cmd);
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "%s -D PRE_SERVICE -i %s -p tcp --dport %s -j ACCEPT 2>/dev/null", \
                IPTABLES_NAT, dev_name, connreq_port);
            svc_wan_execute_cmd(cmd);
            break;
        }
        default:
        {
            break;
        }
    }

#ifdef CWMP
    svc_wan_mgr_send_evt(EVT_WAN_EXTERNAL_TYPE, EVT_WAN_UPDATE_CONNREQ_PORT, connreq_port);
#endif

    return ;
}

void misc_srv_manage(char* path, int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;	
    char node[SVC_WAN_BUF_32_LEN];
    char buf[SVC_WAN_BUF_32_LEN];
    char dev_name[MAX_WAN_DEV_NAME_LEN];

    if((obj = svc_wan_find_object(path)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return ;
    }
    cfg = &obj->cfg;
    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));

    switch (evt)
    {
        case EVT_WAN_CONN_GETV4:
        {
#ifdef CWMP
            update_cwmp_route(obj->path, EVT_WAN_CONN_GETV4);
            set_tr069_nat(obj->path);
            sendMsgtoCwmp(7, IF_STATE_UP, obj->idx);    //7 is wan config change
#endif
            set_pre_srv_rule(path, EVT_WAN_CONN_GETV4);
#if defined(TCSUPPORT_CT_INFORM_NODE)
            if(SVC_WAN_SRV_TR069(cfg))
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), "%s", CWMP_ENTRY_NODE);
                memset(buf, 0, sizeof(buf));
                if(cfg_get_object_attr(node, "account_change_flag", buf, sizeof(buf)) > 0)
                {
                    if(0 == strcmp(buf,"1"))
                    {
                        cfg_commit_object(node);
                    }
                }
#if defined(TCSUPPORT_CT_WAN_PTM) || defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_E8B_ADSL)
                sendMsgtoCwmp(2, IF_STATE_UP, obj->idx);
#endif
            }
#endif

#if defined(TCSUPPORT_CT_PPPINFORM)
            if((SVC_WAN_SRV_INTERNET(cfg)) && (NULL != strstr(dev_name, "ppp")) )
            {
                if(0 < strlen(cfg->user))
                {
                    sendMsgtoCwmp(18, IF_STATE_UP, obj->idx);
                }
            }
            else if(SVC_WAN_SRV_TR069((cfg)))
            {
                sendMsgtoCwmp(18, IF_STATE_UP, obj->idx);
            }
#endif
#if defined(TCSUPPORT_CMCCV2)
            if(SVC_WAN_SRV_INTERNET(cfg))
            {
                cfg_commit_object(TRAFFICMONITOR_NODE);
                cfg_commit_object(TRAFFICMIRROR_NODE);
                cfg_commit_object(TRAFFICDETAILPROCESS_NODE);
            }
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			cfg_commit_object(DOWNLINKQOS_NODE);
#endif
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
#ifdef CWMP
            sendMsgtoCwmp(7,IF_STATE_DOWN, obj->idx);
            update_cwmp_route(obj->path, EVT_WAN_CONN_LOSTV4);
#endif
            set_pre_srv_rule(path, EVT_WAN_CONN_LOSTV4);
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
#ifdef CWMP
            update_cwmp_route(obj->path, EVT_WAN_CONN_GETV6);
            sendMsgtoCwmp(7, IF_STATE_UP6, obj->idx);
#endif
#if defined(TCSUPPORT_IPV6_CWMP)
            if( SVC_WAN_SRV_TR069((cfg)) )
            {
                sendMsgtoCwmp(26, IF_STATE_UP6, obj->idx);
            }
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			cfg_commit_object(DOWNLINKQOS_NODE);
#endif
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
#ifdef CWMP
            update_cwmp_route(obj->path, EVT_WAN_CONN_LOSTV6);
            sendMsgtoCwmp(7, IF_STATE_DOWN6, obj->idx);
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

#if defined(TCSUPPORT_CT_L2TP_VPN)

#if defined(TCSUPPORT_CMCCV2)
#define VPN_INSTANCE_NUM        1
#else
#define VPN_INSTANCE_NUM        16
#endif
#define VPN_TUNNEL_NAME_PRE     "vpn_tunnel_"

void restartVPN(wan_entry_obj_t* obj)
{
    char nodeName[32];
    wan_entry_cfg_t* cfg = NULL;

    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("obj can't be NULL.\n");
        return ;
    }

    cfg = &obj->cfg;
    if(SVC_WAN_SRV_INTERNET(cfg))
    {
        bzero(nodeName, sizeof(nodeName));
        snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
        cfg_set_object_attr(nodeName, "vpnaction", "wan_restart");
        cfg_commit_object(nodeName);
    }

    return;
}

/*
open all vpn ppp tunnel
*/
int open_VPN_PPP_tunnel(void)
{
    char nodeName[32];
    char userIdBuf[128] = {0};
    char lns_addr[64] = {0};
    char vpnstatus[32] = {0};
    char cmdbuf[256] = {0};
    int idx = 0;

    bzero(nodeName, sizeof(nodeName));
    for ( idx = 0; idx < VPN_INSTANCE_NUM; idx ++ )
    {
        snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, (idx+1));
        bzero(userIdBuf, sizeof(userIdBuf));
        bzero(lns_addr, sizeof(lns_addr));
        bzero(vpnstatus, sizeof(vpnstatus));
        if ( 0 < cfg_get_object_attr(nodeName, "userId", userIdBuf, sizeof(userIdBuf))
            && 0 != userIdBuf[0]
            && 0 < cfg_get_object_attr(nodeName, "serverIpAddr", lns_addr, sizeof(lns_addr))
            && 0 != lns_addr[0]
            && 0 < cfg_get_object_attr(nodeName, "vpnstatus", vpnstatus, sizeof(vpnstatus))
#if defined(TCSUPPORT_CMCCV2)
            && 0 != strcmp(vpnstatus, "2")
#else
            && 0 != strcmp(vpnstatus, "0")
#endif
        )
        {
            bzero(cmdbuf, sizeof(cmdbuf));
            snprintf(cmdbuf, sizeof(cmdbuf) - 1
            , "echo 'c "VPN_TUNNEL_NAME_PRE"%d' "
            ">/var/run/xl2tpd/l2tp-control"
            , idx);

            svc_wan_execute_cmd(cmdbuf);
        }
    }

    return 0;
}

#endif

#if defined(TCSUPPORT_CT_JOYME2)
int send_cwmp_wan_status(char* status, char *ip)
{
	char msg[256] = {0}, cur_time[64] = {0}, inform_time[64] = {0};
	char reason[128] = {0};
	int ret =0;

	memset(cur_time,0,sizeof(cur_time));
	memset(inform_time,0,sizeof(inform_time));
	get_current_time(cur_time, sizeof(cur_time));
	get_inform_time(inform_time, sizeof(inform_time));
	snprintf(reason, sizeof(reason), "{TR069WANStatus:%s,TR069WANIP:%s,LastInformTime:%s}", status, ip, inform_time);
	snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"%s\",\"Logs\":\"\"}",
							CTC_GW_SERVICE_NAME_TR069C, cur_time, reason);
	
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
				ECNT_EVENT_DBUS_PROC_STAT,
				msg,
				strlen(msg)+1);
	return ret;
}
#endif
