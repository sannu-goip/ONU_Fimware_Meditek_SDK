#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_common.h"
#include "cfg_msg.h"
#include "wan_link_list.h"
#include "msg_notify.h"
#include "utility.h"
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include <notify/notify.h>

void svc_wan_mgr_send_evt(int type, int evt_id, char* buf)
{
    wan_evt_t wan_evt;
    memset(&wan_evt, 0, sizeof(wan_evt));
    char evt_type[EVT_TYPE_LENGTH];

    if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
    {
        printf("error path is NULL || path length is too long.\n");
        return ;
    }

    strcpy(wan_evt.buf, buf);
    memset(evt_type, 0, sizeof(evt_type));
    switch (type)
    {
        case EVT_WAN_INTERNAL_TYPE :
        {
            strcpy(evt_type, EVT_WAN_INTERNAL);
            break;
        }
        case EVT_WAN_EXTERNAL_TYPE:
        {
            strcpy(evt_type, EVT_WAN_EXTERNAL);
            break;
        }
        case EVT_WAN_RELATED_INTERNAL_TYPE:
        {
            strcpy(evt_type, EVT_WAN_RELATED_INTERNAL);
            break;
        }
        default:
        {
            return ;
        }
    }

    cfg_send_event(evt_type, evt_id, (void*)&wan_evt, sizeof(wan_evt));

    return ;
}

void sendfifo2mobile(char input)
{
    int fd = -1, ret = 0;

    fd = open(WANCHANGEFIFO, O_WRONLY | O_NONBLOCK);
    if(fd < 0) 
    {
        SVC_WAN_ERROR_INFO("sendfifo2mobile:open fifo err!\n");
        return;
    }

    ret = write(fd, &input,sizeof(char));
    if(ret < 0 )
    {
        SVC_WAN_ERROR_INFO("sendfifo2mobile:write fifo err!\n");
    }

    close(fd);

    return ;
}

void sendMsgtoCwmp(int cwmpMsgType, int if_state, int if_index)
{
    cwmp_msg_t message;
    long type = 1;              //tr69 must be 1
    int msgFlag = IPC_NOWAIT;   //0;

    memset(&message,0,sizeof(cwmp_msg_t));
    
#if defined(TCSUPPORT_CT_MIDWARE)
    message.text.reserved[0] = if_state;
    message.text.reserved[1] = if_index;
#endif

    message.cwmptype = cwmpMsgType; 

    if(sendmegq(type, &message, msgFlag) < 0)
    {
        SVC_WAN_ERROR_INFO("\r\n send message to cwmp error!!");
    }

    return ;
}

int send_msg2dbus(WanIpEvent *p_wan_ip, int evt)
{
	char msg[128] = {0};
#if defined(TCSUPPORT_CT_UBUS)
	waninfo_msg_t msgdata;
#endif
	if (p_wan_ip == NULL)
		return -1;

#if 1
	switch (evt) {
		case EVT_WAN_CONN_LOSTV4:
		case EVT_WAN_CONN_LOSTV6:
		case EVT_WAN_CONN_GETV4:
		case EVT_WAN_CONN_GETV6:
#if defined(TCSUPPORT_CT_JOYME2)
			ctc_notify2dbus(NOTIFY_TCAPI_SAVE_CALLED, NULL, NULL);
#endif
#if defined(TCSUPPORT_CT_UBUS)
						memset(&msgdata, 0, sizeof(waninfo_msg_t));
						strncpy(msgdata.ipaddr, p_wan_ip->wanIpAddr, sizeof(msgdata.ipaddr) - 1);
						strncpy(msgdata.status, p_wan_ip->status, sizeof(msgdata.status) - 1);
						ctc_notify2ctcapd(NOTIFY_WANINFO_IP_CHANGE, &msgdata, sizeof(waninfo_msg_t));
#endif

			break;
		default:
			break;
	}
#else
	switch (evt) {
		case EVT_WAN_CONN_LOSTV4:
			snprintf(msg, sizeof(msg), "/com/ctc/igd1/Info/Network;com.ctc.igd1.NetworkInfo;WANConnectionStatus:s:Disconnected;WANIPAddr:s:%s", 
						p_wan_ip->wanIpAddr);
			ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
			break;
		case EVT_WAN_CONN_LOSTV6:
			snprintf(msg, sizeof(msg), "/com/ctc/igd1/Info/Network;com.ctc.igd1.NetworkInfo;IPv6_WANConnectionStatus:s:Disconnected;WANIPv6Addr:s:%s", 
						p_wan_ip->ipv6WanAddr);
			ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
			break;
		case EVT_WAN_CONN_GETV4:
			snprintf(msg, sizeof(msg), "/com/ctc/igd1/Info/Network;com.ctc.igd1.NetworkInfo;WANConnectionStatus:s:Connected;WANIPAddr:s:%s", 
						p_wan_ip->wanIpAddr);
			ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
			break;
		case EVT_WAN_CONN_GETV6:
			snprintf(msg, sizeof(msg), "/com/ctc/igd1/Info/Network;com.ctc.igd1.NetworkInfo;IPv6_WANConnectionStatus:s:Connected;WANIPv6Addr:s:%s", 
						p_wan_ip->ipv6WanAddr);
			ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
			break;
	}
#endif
	return 0;
}

void wan_ip_change_inform(char* path, int evt)
{
    int ipv4_change = 0;
    int ipv6_change = 0;
    int Cindex = -1;
    int len=0;
    char buf[SVC_WAN_BUF_64_LEN];
    char node[SVC_WAN_BUF_32_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char def_route_index_v6[SVC_WAN_BUF_16_LEN];
    WanIpEvent wanIpCfg;
    wan_entry_obj_t* obj = NULL;

    obj = svc_wan_find_object(path);
    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("not find object.\n");
        return ;
    }

    memset(node, 0, sizeof(node));
    memset(&wanIpCfg, 0, sizeof(wanIpCfg));

    snprintf(node, sizeof(node), "%s", WANINFO_COMMON_NODE);
    switch(evt)
    {
        case EVT_WAN_CONN_LOSTV4:
        {
            memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
            cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4));
            if(0 == strcmp(def_route_index_v4, "N/A"))
            {
                strcpy(wanIpCfg.wanIpAddr,"0.0.0.0");
                strcpy(wanIpCfg.wanSubnetMask,"0.0.0.0");
                ipv4_change=1;
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
#if defined(TCSUPPORT_CT_UBUS)
				strcpy(wanIpCfg.status, "down");
#endif
				send_msg2dbus(&wanIpCfg, evt);
#endif
            }
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
            cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6));
            if(0 == strcmp(def_route_index_v6, "N/A"))
            {
                strcpy(wanIpCfg.ipv6WanAddr,"0:0:0:0:0:0:0:0");
                wanIpCfg.wanPrefixLen = 0;
                ipv6_change=1;
#if defined(TCSUPPORT_CT_JOYME2)
				send_msg2dbus(&wanIpCfg, evt);
#endif
            }
            break;
        }
        case EVT_WAN_CONN_GETV4:
        {
            memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
            cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4));
            Cindex = atoi(def_route_index_v4);
            if(Cindex == obj->idx)
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 
                memset(buf, 0, sizeof(buf));
                cfg_get_object_attr(node, "IP", buf, sizeof(buf));
                len = sizeof(wanIpCfg.wanIpAddr) - 1;
                strncpy(wanIpCfg.wanIpAddr, buf, len);	

                memset(buf, 0, sizeof(buf));
                cfg_get_object_attr(node, "NetMask", buf, sizeof(buf));
                len = sizeof(wanIpCfg.wanSubnetMask) - 1;
                strncpy(wanIpCfg.wanSubnetMask, buf, len);
                ipv4_change=1;
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
#if defined(TCSUPPORT_CT_UBUS)
				strcpy(wanIpCfg.status, "up");
#endif
				send_msg2dbus(&wanIpCfg, evt);
#endif
            }
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
            cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6));
            Cindex = atoi(def_route_index_v6);
            if(Cindex == obj->idx)
            {
                memset(node, 0, sizeof(node));
                snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1); 

                memset(buf, 0, sizeof(buf));
                cfg_get_object_attr(node, "IP6", buf, sizeof(buf));
                len = sizeof(wanIpCfg.ipv6WanAddr) - 1;
                strncpy(wanIpCfg.ipv6WanAddr, buf, len);	

                memset(buf, 0, sizeof(buf));
                cfg_get_object_attr(node, "PrefixLen6", buf, sizeof(buf));
                wanIpCfg.wanPrefixLen= atoi(buf);
                ipv6_change=1;
#if defined(TCSUPPORT_CT_JOYME2)
				send_msg2dbus(&wanIpCfg, evt);
#endif
            }
            break;
        }
        default:
        {
            break;
        }
    }
	
#if !defined(TCSUPPORT_CT_JOYME2)
    if(ipv4_change ||  ipv6_change )
    {
        nlk_msg_t nklmsg;
        int ret = 0;
        memset(&nklmsg, 0, sizeof(nlk_msg_t));
        nklmsg.nlmhdr.nlmsg_len = sizeof(nlk_msg_t);
        nklmsg.nlmhdr.nlmsg_pid = getpid();  /* self pid */
        nklmsg.nlmhdr.nlmsg_flags = 0;

        nklmsg.eventType = MSG_MUlTICAST_EVENT;

        snprintf(nklmsg.data.payload, sizeof(nklmsg.data.payload), "{\"Event\":\"WAN_IP_CHANGE\",\"IpAddr\":\"%s\",\"SubnetMask\":\"%s\",\"IpV6Addr\":\"%s\",\"PrefixLen\":\"%d\"}", \
            wanIpCfg.wanIpAddr,wanIpCfg.wanSubnetMask,wanIpCfg.ipv6WanAddr,wanIpCfg.wanPrefixLen);

        ret = nlkmsg_send(NLKMSG_GRP_MULTI, sizeof(nklmsg), &nklmsg);
        if(ret <0) 
        {
            SVC_WAN_ERROR_INFO("wanchange send err:%s!\n", strerror(errno));
        }
    }
#endif
    return ;
}

#if defined(TCSUPPORT_CT_UBUS)
void br_wan_ip_change_inform(char *dev, int evt)
{
    char nodeName[SVC_WAN_BUF_32_LEN];
    char buf[SVC_WAN_BUF_64_LEN];
    WanIpEvent wanIpCfg;

	if ( NULL == dev || 0 != strcmp(dev, WAN_ITF_BR0_1) )
		return;

	memset(&wanIpCfg, 0, sizeof(wanIpCfg));
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WANINFO_BRENTRY_NODE);

	switch(evt)
	{
		case EVT_WAN_CONN_LOSTV4:
		{
			strcpy(wanIpCfg.wanIpAddr,"0.0.0.0");
			strcpy(wanIpCfg.wanSubnetMask,"0.0.0.0");
			strcpy(wanIpCfg.status, "down");
			send_msg2dbus(&wanIpCfg, evt);
			break;
		}
		case EVT_WAN_CONN_GETV4:
		{
			memset(buf, 0, sizeof(buf));
			cfg_get_object_attr(nodeName, "IP", buf, sizeof(buf));
			strncpy(wanIpCfg.wanIpAddr, buf, sizeof(wanIpCfg.wanIpAddr) - 1);	

			memset(buf, 0, sizeof(buf));
			cfg_get_object_attr(nodeName, "NetMask", buf, sizeof(buf));
			strncpy(wanIpCfg.wanSubnetMask, buf, sizeof(wanIpCfg.wanSubnetMask) - 1);
			strcpy(wanIpCfg.status, "up");
			send_msg2dbus(&wanIpCfg, evt);
			break;
		}
		default:
			break;
	}
	return;
}
#endif


