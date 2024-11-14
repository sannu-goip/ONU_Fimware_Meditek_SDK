

/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_cfg.h"
#include "wan_interface.h"
#include "wan_mgr.h"
#include "wan_nat.h"
#include "port_binding.h"
#include "full_route.h"
#include "dslite_info.h"
#include "dhcp_relay_info.h"
#include "dhcp_port_filter_info.h"
#include "vlan_binding.h"
#include "misc_srv.h"
#include "utility.h"
#include "atmcmdd.h"
#include <sys/socket.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/un.h>
#if defined(TCSUPPORT_CMCCV2)
#include <blapi_traffic.h>
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
#include <signal.h>
#endif

/***********************************************************/
#define PPP_START_SH "/usr/script/pppd_start.sh"

/***********************************************************/
static int svc_wan_create_conf(wan_entry_obj_t* obj);
static int svc_wan_delete_conf(wan_entry_obj_t* obj);
static int create_dhcp6c_conf(char *dev_name, int PD, int NA, int DSLite);
static int prepare_start_dhcp6c(wan_entry_obj_t* obj);
static void svc_wan_mtu_conf(wan_entry_obj_t* obj, int action);
static void svc_wan_config_intf_attr(wan_entry_obj_t* obj, int protocol_version);
static void svc_wan_do_something_creat_intf(wan_entry_obj_t* obj, int protocol_version);
static void svc_wan_do_something_del_intf(wan_entry_obj_t* obj);
static int start_dhcp6c(wan_entry_obj_t* obj);
static int stop_dhcp6c(wan_entry_obj_t* obj);

/***********************************************************/
static int svc_wan_create_device(wan_entry_obj_t* obj);
static int svc_wan_delete_device(char* name);
static int svc_wan_start_device(wan_entry_obj_t* obj);
static int svc_wan_stop_device(wan_entry_obj_t* obj);
static int svc_wan_start_ipv4_dhcp(wan_entry_obj_t* obj);
static int svc_wan_stop_ipv4_dhcp(wan_entry_obj_t* obj);
static int svc_wan_start_ipv4_static(wan_entry_obj_t* obj);
static int svc_wan_stop_ipv4_static(wan_entry_obj_t* obj);
static int svc_wan_start_ppp(wan_entry_obj_t* obj, int protocol_flag);
static int svc_wan_stop_ppp(wan_entry_obj_t* obj);
static int svc_wan_start_bridge(wan_entry_obj_t* obj);
static int svc_wan_stop_bridge(wan_entry_obj_t* obj);
int svc_wan_start_ipv6_dhcp(wan_entry_obj_t* obj);
int svc_wan_stop_ipv6_dhcp(wan_entry_obj_t* obj);
static int svc_wan_start_ipv6_slaac(wan_entry_obj_t* obj);
static int svc_wan_stop_ipv6_slaac(wan_entry_obj_t* obj);
static int svc_wan_start_ipv6_static(wan_entry_obj_t* obj);
static int svc_wan_stop_ipv6_static(wan_entry_obj_t* obj);
/***********************************************************/
#if defined(TCSUPPORT_CFG_NG_UNION) && !defined(TCSUPPORT_CMCC)
static int svc_save_protect_wan_info(wan_entry_obj_t* obj)
{
	char mac[20] = {0}, cmdbuf[128] = {0};
	const char *attack_file = "/proc/tc3162/attackprotector";
	wan_entry_cfg_t* cfg = &obj->cfg;

	bzero(mac, sizeof(mac));
	bzero(cmdbuf, sizeof(cmdbuf));
	if ( SVC_WAN_SRV_TR069(cfg) )
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "T %s", getWanInfo(Mac_type, obj->dev, mac));
		doValPut(attack_file, cmdbuf);
	}

	if ( SVC_WAN_SRV_VOICE(cfg) )
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "V %s", getWanInfo(Mac_type, obj->dev, mac));
		doValPut(attack_file, cmdbuf);

		snprintf(cmdbuf, sizeof(cmdbuf), "D %d", 5060);
		doValPut(attack_file, cmdbuf);
	}

    return 0;
}
#endif

static int svc_wan_create_device(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    char* str[4] = {"bridge", "pppoe", "ipoe", "pppoe_bi"};
    unsigned char mode;
    char dev[8];
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
    char transMode[16];
#endif
#if defined(TCSUPPORT_PON_ROSTELECOM)
	int defaultGW_flag = 0;
#endif
    wan_entry_cfg_t* cfg = &obj->cfg;

    if (SVC_WAN_IS_BRIDGE(cfg))
    {
        mode = 0;
    }
    else if (SVC_WAN_IS_PPP_HYBRID(cfg))
    {
        mode = 3;
    }
    else if (SVC_WAN_IS_PPP(cfg))
    {
        mode = 1;
    }
    else if (SVC_WAN_IS_IP(cfg))
    {
        mode = 2;
    }
    else 
    {
        return -1;
    }
    memset(dev,0,sizeof(dev));
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
    memset(transMode,0,sizeof(transMode));
	cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode));
    if( 0 == strcmp(transMode, "Ethernet")) /*Ethernet Mode*/
    {
        snprintf(dev,sizeof(dev),"nas10");
    }
	else if( 0 == strcmp(transMode, "ActiveEtherWan"))
	{
		snprintf(dev,sizeof(dev),"ae_wan");
	}
    else
#endif
#if defined(TCSUPPORT_CT_DSL_EX)	
	atm_create_device(obj, dev, sizeof(dev));
#else
    {
        snprintf(dev,sizeof(dev),"pon");
    }
#endif

    memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_PON_ROSTELECOM)
	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl add %s %s %s %d %d %d %d %d",str[mode],dev,obj->dev,defaultGW_flag,cfg->vtag,cfg->vid,cfg->dot1p,cfg->mvid);
#else
    snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl add %s %s %s %d %d %d %d",str[mode],dev,obj->dev,cfg->vtag,cfg->vid,cfg->dot1p,cfg->mvid);
#endif
    svc_wan_execute_cmd(cmd);

    if(SVC_WAN_IS_BRIDGE(cfg))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl ipversion %s %d",obj->dev,cfg->version == SVC_WAN_ATTR_VERSION_BOTH  ? 0: cfg->version);
        svc_wan_execute_cmd(cmd);
    }

#if defined(TCSUPPORT_CFG_NG_UNION) && !defined(TCSUPPORT_CMCC)
	svc_save_protect_wan_info(obj);
#endif

    return 0;
}

static int svc_wan_delete_device(char* name)
{
    char cmd[SVC_WAN_BUF_64_LEN];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl rem %s ",name);

    svc_wan_execute_cmd(cmd);

    return 0;
}

static int svc_wan_start_device(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_64_LEN];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s up",obj->dev);

    svc_wan_execute_cmd(cmd);

    return 0;
}


static int svc_wan_stop_device(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_64_LEN];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s down",obj->dev);

    svc_wan_execute_cmd(cmd);
	svc_wan_execute_cmd("/userfs/bin/hw_nat -!");

    return 0;
}



static int svc_wan_start_ipv4_dhcp(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/udhcpc -i %s -s /usr/script/udhcpc_nodef.sh -p /var/run/%s/udhcpc.pid &", obj->dev,obj->dev);
	
    snprintf(obj->pid4, sizeof(obj->pid4), "/var/run/%s/udhcpc.pid",obj->dev);

    svc_wan_execute_cmd(cmd);

    return 0;
}

static int svc_wan_stop_ipv4_dhcp(wan_entry_obj_t* obj)
{
    svc_wan_kill_process(obj->pid4, 1);

    svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV4,obj->dev);

    return 0;
}

static int svc_wan_start_ipv4_static(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_64_LEN];
    char fpath[32];

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/status",obj->dev);
    snprintf(cmd, sizeof(cmd),"/bin/echo up > %s",fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath),"/var/run/%s/ip",obj->dev);
    snprintf(cmd, sizeof(cmd),"/bin/echo %s > %s",obj->cfg.addrv4,fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/netmask",obj->dev);
    snprintf(cmd, sizeof(cmd), "/bin/echo %s > %s",obj->cfg.maskv4,fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/gateway",obj->dev);
    snprintf(cmd, sizeof(cmd), "/bin/echo %s > %s",obj->cfg.gwv4,fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/dns",obj->dev);
    snprintf(cmd, sizeof(cmd),"/bin/echo %s > %s",obj->cfg.dns1v4,fpath);
    svc_wan_execute_cmd(cmd);
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo %s >> %s",obj->cfg.dns2v4,fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s %s netmask %s",obj->dev,obj->cfg.addrv4,obj->cfg.maskv4);
    svc_wan_execute_cmd(cmd);

    svc_wan_mgr_handle_event(EVT_WAN_CONN_GETV4,obj->dev);

    return 0;
}


static int svc_wan_stop_ipv4_static(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_64_LEN];
    char fpath[32];

    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/status", obj->dev);
    snprintf(cmd, sizeof(cmd), "/bin/echo down > %s",fpath);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s 0.0.0.0",obj->dev);
    svc_wan_execute_cmd(cmd);

 	svc_wan_handle_event_conn_lostv4_deal(obj);

    return 0;
}


static int svc_wan_start_ppp(wan_entry_obj_t* obj, int protocol_flag)
{
    int pid;
    char cmd[SVC_WAN_BUF_512_LEN];
    char sh_exe_cmd[SVC_WAN_BUF_512_LEN];
    char tmp[SVC_WAN_BUF_512_LEN];
    char parm[SVC_WAN_BUF_512_LEN];
    char path[SVC_WAN_BUF_32_LEN];
	char escap_para1[SVC_WAN_BUF_256_LEN], escap_para2[SVC_WAN_BUF_256_LEN];

    wan_entry_cfg_t* cfg = &obj->cfg;
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	char idletime[32] = {0};
#endif

    memset(parm, 0, sizeof(parm));
    snprintf(parm, sizeof(parm), "%s", "nodetach holdoff 4 maxfail 0 usepeerdns lcp-echo-interval 10 lcp-echo-failure 6");

    memset(tmp, 0, sizeof(tmp));
	memset(escap_para1, 0, sizeof(escap_para1));
	memset(escap_para2, 0, sizeof(escap_para2));

#if defined(TCSUPPORT_SDN_OVS) || defined(TCSUPPORT_CT_VPN_PPTP)
    snprintf(tmp, sizeof(tmp), " plugin /lib/libpppoe.so %s", obj->dev);
#else
    snprintf(tmp, sizeof(tmp), " plugin pppoe %s", obj->dev);
#endif
    strcat(parm, tmp);
    strcat(parm, " noipdefault");

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	if ( SVC_WAN_IS_CONN_ON_DEMAND((&obj->cfg)) )
	{
		if ( cfg_get_object_attr(obj->path, "CLOSEIFIDLE", idletime, sizeof(idletime)) <= 0 )
			strncpy(idletime, "1800", sizeof(idletime)- 1);/* default idle time to 30min*/
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), " demand idle %s", idletime);
		strcat(parm, tmp);
	}
	else
#endif
    strcat(parm, " persist");
    if(0 != cfg->mtu)
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), " mtu %d mru %d", cfg->mtu, cfg->mtu);
        strcat(parm, tmp);
    }

    if(E_SYSTEM_IPV4_IPV6_ENABLE == protocol_flag)
    {
        if(SVC_WAN_ATTR_VERSION_BOTH == cfg->version && (!SVC_WAN_DSLITE_ENABLE(cfg)))
        {
            strcat(parm, " ipv6 , ");
            if(SVC_WAN_GWV6_IS_MANUAL(cfg))
            {
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
                svc_wan_execute_cmd(tmp);
            }
        }
        else if(SVC_WAN_IS_IPV6(cfg))
        {
            strcat(parm, " ipv6 , noip");
            if(SVC_WAN_GWV6_IS_MANUAL(cfg))
            {
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
                svc_wan_execute_cmd(tmp);
            }
        }
        else if(SVC_WAN_IS_IPV4(cfg))
        {
            SVC_WAN_NOTICE_INFO("nothing.\n");
        }
    }
    else if(E_SYSTEM_ONLY_IPV6_ENABLE == protocol_flag)
    {
        if(SVC_WAN_IS_IPV6(cfg))
        {
            strcat(parm, " ipv6 , noip");
            if(SVC_WAN_GWV6_IS_MANUAL(cfg))
            {
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
                svc_wan_execute_cmd(tmp);
            }
        }
        else
        {
            strcat(parm, " noip");
        }
    }
    else if(E_SYSTEM_ONLY_IPV4_ENABLE == protocol_flag)
    {
        if ((SVC_WAN_ATTR_VERSION_BOTH == cfg->version && (SVC_WAN_DSLITE_ENABLE(cfg))) \
            || (SVC_WAN_ATTR_VERSION_IPV6 == cfg->version))
        {
            strcat(parm, " noip");
            if(SVC_WAN_GWV6_IS_MANUAL(cfg))
            {
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
                svc_wan_execute_cmd(tmp);
            }
        }
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/bin/pppd unit %d user %s password %s %s"
										, obj->idx
										, ESCAPE_VAL(obj->cfg.user, escap_para1, sizeof(escap_para1))
										, ESCAPE_VAL(obj->cfg.pwd, escap_para2, sizeof(escap_para2))
										, parm);

    snprintf(obj->pid4, sizeof(obj->pid4), "/var/run/ppp%d/ppp%d.pid", obj->idx, obj->idx);
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, cmd, sizeof(tmp) - 1);

    memset(sh_exe_cmd, 0, sizeof(sh_exe_cmd));
    snprintf(sh_exe_cmd, sizeof(sh_exe_cmd), "%s \"%s\" ppp%d", PPP_START_SH, cmd, obj->idx);
    svc_wan_execute_cmd_normal(sh_exe_cmd);


    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo \"%s &\" > /var/tmp/ppp%d.conf", tmp, obj->idx);
    svc_wan_execute_cmd_normal(cmd);

    if(SVC_WAN_IS_IPV6_SLAAC(cfg))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/bin/echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf", obj->idx);
        svc_wan_execute_cmd(cmd);
    }

    if(SVC_WAN_IS_PPP_HYBRID(cfg))
    {
#if defined(TCSUPPORT_CT_JOYME4)
    	obj->flag |= SVC_WAN_FLAG_PPP_RELAY_CHECK;
#else
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/userfs/bin/pppoe-relay -C br0 -S %s &", obj->dev);
        svc_wan_execute_cmd(cmd);
#if 0
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/ip link set br0 promisc on");
		svc_wan_execute_cmd(cmd);
#endif
        //Redesignate process priority
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "/var/run/pppoe-relay_%s.pid", obj->dev);
        pid = svc_wan_get_pid(path);
        if(pid > 0)
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "renice -10 %d", pid);
            svc_wan_execute_cmd(cmd);
        }
#endif
    }

    return 0;
}

int stop_pppoe_relay(wan_entry_obj_t* obj)
{
	char path[SVC_WAN_BUF_128_LEN];

	bzero(path, sizeof(path));
	snprintf(path, sizeof(path), "/var/run/pppoe-relay_%s.pid", obj->dev);
	svc_wan_kill_process(path, 1);
	unlink(path);

#if defined(TCSUPPORT_CT_JOYME4)
	obj->flag |= SVC_WAN_FLAG_PPP_RELAY_CHECK;
#endif

	return 0;
}

static int svc_wan_stop_ppp(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_64_LEN];
    char dev[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t* cfg = &obj->cfg;
    char path[SVC_WAN_BUF_32_LEN];

    /* del  start process cmd */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/rm -f /var/tmp/ppp%d.conf", obj->idx);
    svc_wan_execute_cmd(cmd);

    //kill pppoe-relay process
    if(SVC_WAN_IS_PPP_HYBRID(cfg))
    {
		stop_pppoe_relay(obj);

#if !defined(TCSUPPORT_CT_JOYME4)
        SVC_WAN_ERROR_INFO("ip link set br0 promisc off\n");

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/usr/bin/ip link set br0 promisc off");
        svc_wan_execute_cmd(cmd);
#endif
    }
    
    //kill pppd process  
    svc_wan_kill_process(obj->pid4, 1);

    snprintf(dev, sizeof(dev), "ppp%d",obj->idx);
    if(SVC_WAN_IS_IPV4(cfg) && SVC_WAN_IS_IPV6(cfg) && (!SVC_WAN_DSLITE_ENABLE(cfg)))
    {
        svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV6, dev);
        svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV4, dev);
    }
    else if(SVC_WAN_IS_IPV6(cfg))
    {
        svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV6, dev);
    }
    else if(SVC_WAN_IS_IPV4(cfg))
    {
        svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV4, dev);
    }
    else
    {
        SVC_WAN_ERROR_INFO("stop ppp error.\n");
    }

    return 0;
}

static int svc_wan_start_bridge(wan_entry_obj_t* obj)
{
	char cmd[SVC_WAN_BUF_64_LEN];
	char proxy_if[32] = {0}, ppp_if[32] = {0};
	int need_save = 0;

	snprintf(ppp_if, sizeof(ppp_if), "ppp%d", obj->idx);
	bzero(proxy_if, sizeof(proxy_if));
	cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", proxy_if, sizeof(proxy_if));
	if ( '\0' != proxy_if[0] && 
		( 0 == strcmp(obj->dev, proxy_if) || 0 == strcmp(ppp_if, proxy_if) ) ) {
		cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "UpstreamIF", "");
		cfg_set_object_attr(IGMPPROXY_ENTRY_NODE, "Active", "No");
		need_save = 1;

		/*kill igmpproxy process*/
		system("killall igmpproxy &");
	}

#ifdef IPV6
	bzero(proxy_if, sizeof(proxy_if));
	cfg_get_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", proxy_if, sizeof(proxy_if));
	if (  '\0' != proxy_if[0] && 
		( 0 == strcmp(obj->dev, proxy_if) || 0 == strcmp(ppp_if, proxy_if) ) ) {
		cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "UpstreamIF", "");
		cfg_set_object_attr(MLDPROXY_ENTRY_NODE, "Active", "No");
		need_save = 1;

		/*kill mld proxy process*/
		system("killall ecmh &");
	}
#endif
	if ( need_save )
		cfg_evt_write_romfile_to_flash();

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/bin/echo up > /var/run/%s/status",obj->dev);
	svc_wan_execute_cmd(cmd);
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/brctl addif br0 %s",obj->dev);
	svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "echo 2 > /sys/class/net/br0/brif/%s/multicast_router", obj->dev);
    svc_wan_execute_cmd(cmd);
#endif

	svc_wan_handle_conn_evt(obj, EVT_WAN_BRIDGE_UP);
#if defined(TCSUPPORT_CT_JOYME4)	
	cfg_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ARP_RULE_CHG, NULL, 0);
#endif	
	return 0;
}

static int svc_wan_stop_bridge(wan_entry_obj_t* obj)
{
	char cmd[SVC_WAN_BUF_64_LEN];

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/bin/echo down > /var/run/%s/status",obj->dev);
	svc_wan_execute_cmd(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/brctl delif br0 %s",obj->dev);
	svc_wan_execute_cmd(cmd);

	svc_wan_handle_conn_evt(obj, EVT_WAN_BRIDGE_DOWN);
#if defined(TCSUPPORT_CT_JOYME4)
	cfg_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ARP_RULE_CHG, NULL, 0);
#endif
	return 0;
}

static int create_dhcp6c_conf(char *dev_name, int PD, int NA, int DSLite)
{
    FILE *fp=NULL;
    char fpath[64];
    char buf[64];

    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/dhcp6c.conf", dev_name);
    fp = fopen(fpath,"w");
    if(NULL != fp)
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "interface %s {\n", dev_name);
        fputs(buf, fp);
        if(PD)
        {
            fputs("\tsend ia-pd 213;\n", fp);
        }
        if(NA)
        {
            fputs("\tsend ia-na 210;\n", fp);
        }
#if defined(TCSUPPORT_CT_DHCP6_OPTION)
        fputs("\trequest domain-name;\n\trequest domain-name-servers;\n\tscript \"/usr/script/dhcp6c_script\";\n\trequest vendor_opts;\n", fp);
#else
        fputs("\trequest domain-name;\n\trequest domain-name-servers;\n\tscript \"/usr/script/dhcp6c_script\";\n", fp);
#endif

#if defined(TCSUPPORT_CT_DSLITE)
        if( DSLite )
        {
            fputs("\trequest dslite-name;\n", fp);
        }
#endif
        if(PD == DISABLE && NA == DISABLE)
        {
            fputs("\tinformation-only;\n", fp);
        }

        fputs("};\n", fp);
        if(PD)
        {
            fputs("id-assoc pd 213 {\n\tprefix-interface br0\n\t{\n\t\tsla-len 0;\n\t};\n};\n", fp);
        }
        if(NA)
        {
            fputs("id-assoc na 210 { };\n", fp);
        }
        fclose(fp);
    }
    else
    {
        return -1;
    }

    return 0;
}

static int prepare_start_dhcp6c(wan_entry_obj_t* obj)
{
    int ret = 0;
    unsigned char     pd = DISABLE;
    unsigned char     na = DISABLE;
    unsigned char dslite = DISABLE;
    wan_entry_cfg_t* cfg = &obj->cfg;
    char dev_name[MAX_WAN_DEV_NAME_LEN];

    if(!SVC_WAN_IS_IPV6(cfg) || SVC_WAN_IS_IPV6_STATIC(cfg))
    {
        SVC_WAN_NOTICE_INFO("not need creat dhcp6c conf\n");
        return 0;
    }

    if(SVC_WAN_IPV6_DHCPV6PD_ENABLE(cfg))
    {
        pd = ENABLE;
    }

    if(SVC_WAN_IS_IPV6_DHCP(cfg))
    {
        na = ENABLE;
    }

#if defined(TCSUPPORT_CT_DSLITE)
    if((!SVC_WAN_IS_BRIDGE(cfg)) && SVC_WAN_SRV_INTERNET(cfg) && SVC_WAN_DSLITE_ENABLE(cfg))
    {
        dslite = ENABLE;
    }
#endif

    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
    ret = create_dhcp6c_conf(dev_name, pd, na, dslite);
    if(0 != ret)
    {
        SVC_WAN_ERROR_INFO("create dhcp6c conf faile.\n ");
    }

    return ret;
}

static int svc_wan_get_dhcp6c(char* file, char *buf)
{
	FILE* fp = NULL;
	char *tmp_buf = NULL;
	size_t len = 0;
	
	if ( NULL == (fp = fopen(file, "r")) )
	{
		return 0;
	}

	if ( -1 != getline(&tmp_buf, &len, fp) )
	{	
		memcpy(buf, tmp_buf, strlen(tmp_buf)-1);
	}
	
	fclose(fp);
		
	if ( tmp_buf )
	{
		free(tmp_buf);
	}
	
    return 0;
}

static int delete_itf_inet6addr(wan_entry_obj_t* obj)
{
	char cmd[SVC_WAN_BUF_256_LEN] = {0};
	char file[SVC_WAN_BUF_256_LEN] = {0};
	char addrv6[SVC_WAN_BUF_64_LEN] = {0};
	char prefix6[SVC_WAN_BUF_8_LEN] = {0};

	wan_entry_cfg_t* cfg = &obj->cfg;

    snprintf(file, sizeof(file), "/var/run/%s/ip6", obj->dev);
	svc_wan_get_dhcp6c(file, addrv6);
	
	snprintf(file, sizeof(file), "/var/run/%s/prefix6", obj->dev);
	svc_wan_get_dhcp6c(file, prefix6);
	
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s del %s/%s", obj->dev, addrv6, prefix6);
	svc_wan_execute_cmd(cmd);
	
	return 0;	
}

static int start_dhcp6c(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    wan_entry_cfg_t* cfg = &obj->cfg;

    if(SVC_WAN_IS_IPV6_SLAAC(cfg))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/bin/echo 1 > /proc/sys/net/ipv6/conf/%s/autoconf", obj->dev);
        svc_wan_execute_cmd(cmd);
    }

    memset(cmd, 0, sizeof(cmd));
    if(!SVC_WAN_IPV6_PD_ENABLE(cfg))
    {
        snprintf(cmd, sizeof(cmd), "/userfs/bin/dhcp6c -c /var/run/%s/dhcp6c.conf -p /var/run/%s/dhcp6c.pid %s &",obj->dev,obj->dev,obj->dev);
    }
    else
    {
		memset(cmd, 0, sizeof(cmd));
    	snprintf(cmd, sizeof(cmd), "/userfs/bin/dhcp6c -c /var/run/%s/dhcp6c.conf -p /var/run/%s/dhcp6c.pid -x /var/run/%s/pd6 -X /var/run/%s/orgpd6 -u /var/run/%s/pd6_ptime -v /var/run/%s/pd6_vtime %s &",obj->dev,obj->dev, \
    	obj->dev, obj->dev, obj->dev, obj->dev, obj->dev);
    }

    memset(obj->pid6, 0, sizeof(obj->pid6));
    snprintf(obj->pid6, sizeof(obj->pid6), "/var/run/%s/dhcp6c.pid", obj->dev);
    svc_wan_execute_cmd(cmd);

    if(SVC_WAN_GWV6_IS_MANUAL(cfg))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
        svc_wan_execute_cmd(cmd);
    }

    return 0;
}

static int stop_dhcp6c(wan_entry_obj_t* obj)
{
#if defined(TCSUPPORT_NP)
	delete_itf_inet6addr(obj);
#endif
    svc_wan_kill_process(obj->pid6, 1);

    svc_wan_mgr_handle_event(EVT_WAN_CONN_LOSTV6, obj->dev);

    return 0;
}

int svc_wan_start_ipv6_dhcp(wan_entry_obj_t* obj)
{
    return start_dhcp6c(obj);
}

int svc_wan_stop_ipv6_dhcp(wan_entry_obj_t* obj)
{
    return stop_dhcp6c(obj);
}

static int svc_wan_start_ipv6_slaac(wan_entry_obj_t* obj)
{
    return start_dhcp6c(obj);
}

static int svc_wan_stop_ipv6_slaac(wan_entry_obj_t* obj)
{
    return stop_dhcp6c(obj);
}

static int svc_wan_start_ipv6_static(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_256_LEN];

    wan_entry_cfg_t* cfg = &obj->cfg;

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s %s/%d",obj->dev, cfg->addrv6, cfg->prefix6);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo %s > /var/run/%s/ip6",  cfg->addrv6, obj->dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo %d > /var/run/%s/prefix6",  cfg->prefix6, obj->dev);
    svc_wan_execute_cmd(cmd);

    if(SVC_WAN_GWV6_IS_MANUAL(cfg))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd),"/bin/echo %s > /var/run/%s/gateway6", cfg->gwv6, obj->dev);
        svc_wan_execute_cmd(cmd);
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo %s > /var/run/%s/dns6",   cfg->dns1v6, obj->dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo %s >> /var/run/%s/dns6",  cfg->dns2v6, obj->dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/echo up > /var/run/%s/status6", obj->dev);
    svc_wan_execute_cmd(cmd);

    svc_wan_mgr_handle_event(EVT_WAN_CONN_GETV6,obj->dev);

    return 0;
}

static int svc_wan_stop_ipv6_static(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_256_LEN];
    char fpath[32];
   
#if defined(TCSUPPORT_NP)
	wan_entry_cfg_t* cfg = &obj->cfg;

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s del %s/%d", obj->dev, cfg->addrv6, cfg->prefix6);
	svc_wan_execute_cmd(cmd);
#endif
    memset(cmd, 0, sizeof(cmd));
    memset(fpath, 0, sizeof(fpath));
    snprintf(fpath, sizeof(fpath), "/var/run/%s/status6", obj->dev);
    snprintf(cmd, sizeof(cmd), "/bin/echo down > %s",fpath);
    svc_wan_execute_cmd(cmd);

	svc_wan_handle_event_conn_lostv6_deal(obj);
    return 0;
}

static int svc_wan_create_conf(wan_entry_obj_t* obj)
{
    char dir[32],node[32],tmp[32];
    wan_entry_cfg_t* cfg = &obj->cfg;
    FILE* fp;
    int ret = 0, res = 0;
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	char idletime[32] = {0};
#endif

	if(obj->dev[0] == 0){
		return -1;
	}
	
    memset(dir, 0, sizeof(dir));
    if(SVC_WAN_IS_BRIDGE(cfg))
    {
	snprintf(dir, sizeof(dir), "/var/run/%s",obj->dev);
    }
    else if (SVC_WAN_IS_PPP(cfg))
    {
        snprintf(dir, sizeof(dir), "/var/run/ppp%d",obj->idx);
    }
    else
    {
        snprintf(dir, sizeof(dir), "/var/run/%s",obj->dev);
    }

    res = mkdir(dir, 0777);
    if(res){
		/**do nothing,just for compile*/
	}
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%s/pvc.conf",dir);
	
    if ((fp = fopen(tmp,"w")) == NULL)
    {
        return -1;
    }
	fclose(fp);
    
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WAN_PVC_NODE,(obj->idx / 8) + 1);

#if defined(TCSUPPORT_CT_DSL_EX)	
	atm_create_interfae(node, obj);
#endif

    cfg_save_attrs_to_file(node,tmp,NO_QMARKS);
    
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	if ( cfg_get_object_attr(obj->path, "CLOSEIFIDLE", idletime, sizeof(idletime)) <= 0 )
		cfg_set_object_attr(obj->path, "CLOSEIFIDLE", "1800");
#endif

    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%s/interface.conf",dir);

    cfg_save_attrs_to_file(obj->path,tmp,NO_QMARKS);

    ret = prepare_start_dhcp6c(obj);


    return ret;
}

static int svc_wan_delete_conf(wan_entry_obj_t* obj)
{
    char dir[32];
    char cmd[32];
    wan_entry_cfg_t* cfg = &obj->cfg;

    if (obj->dev[0] == 0)
    {
        return -1;
    }

    memset(dir, 0, sizeof(dir));
    if (SVC_WAN_IS_BRIDGE(cfg))
    {
	 snprintf(dir, sizeof(dir), "/var/run/%s",obj->dev);
    }
    else if (SVC_WAN_IS_PPP(cfg))
    {
        snprintf(dir, sizeof(dir), "/var/run/ppp%d",obj->idx);
    }
    else
    {
        snprintf(dir, sizeof(dir), "/var/run/%s",obj->dev);
    }
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/bin/rm -rf %s", dir);

    svc_wan_execute_cmd(cmd);

#if defined(TCSUPPORT_CT_DSL_EX)
    memset(dir, 0, sizeof(dir));
	snprintf(dir, sizeof(dir), "/var/run/nas%d.pid",(obj->idx / 8));
	kill_process(dir);
#endif

    return 0;
}

#if defined(TCSUPPORT_CMCCV2)
static void svc_wan_switch_ppe_max_mtu(wan_entry_obj_t* in_obj, int action)
{
	wan_entry_obj_t *obj = NULL;
	wan_entry_cfg_t *cfg = NULL;
	unsigned int max_mtu_size = 0, old_max_mtu_size = 0, real_mtu_size = 0;
	char buf[8] = {0};

	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(WAN_COMMON_NODE, "MaxMTUSize", buf, sizeof(buf));
	if ( 0 != buf[0] )
		sscanf(buf, "%u", &old_max_mtu_size);
	
	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		if ( 0 == strcmp(obj->path, in_obj->path)
			&& E_DEL_WAN_DEVICE == action )
			continue;
		
		cfg = &obj->cfg;
		if ( !SVC_WAN_IS_ACTIVE(cfg) )
			continue;

		if ( SVC_WAN_IS_BRIDGE(cfg) )
			continue;
		
		if ( SVC_WAN_IS_PPP(cfg) )
			real_mtu_size = cfg->mtu + PPPOE_MTU_INCREMENT;
		else
			real_mtu_size = cfg->mtu + IPOE_MTU_INCREMENT;
		
		if ( real_mtu_size > max_mtu_size )
			max_mtu_size = real_mtu_size;
	}

	if ( 0 != max_mtu_size && old_max_mtu_size != max_mtu_size )
	{
		blapi_traffic_set_mtu(max_mtu_size);
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%u", max_mtu_size);
		cfg_set_object_attr(WAN_COMMON_NODE, "MaxMTUSize", buf);
	}
	
	return 0;
}
#endif

static void svc_wan_mtu_conf(wan_entry_obj_t* obj, int action)
{
    char cmd[SVC_WAN_BUF_256_LEN] = {0};
    char cmd6[SVC_WAN_BUF_256_LEN] = {0};
    char dev_name[MAX_WAN_DEV_NAME_LEN] = {0};
    wan_entry_cfg_t* cfg = &obj->cfg;

    if(SVC_WAN_IS_BRIDGE(cfg))
    {
        return ;
    }

#if defined(TCSUPPORT_CMCCV2)
    svc_wan_switch_ppe_max_mtu(obj, action);
#endif
	
    if(E_CREAT_WAN_DEVICE == action)
    {
        if(0 != cfg->mtu)
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s mtu %d", obj->dev, cfg->mtu);
            svc_wan_execute_cmd(cmd);
        }
        else
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd),"/sbin/ifconfig %s mtu 1500", obj->dev);
            svc_wan_execute_cmd(cmd);
        }
    }

    memset(dev_name, 0, sizeof(dev_name));
    svc_wan_intf_name(obj, dev_name, sizeof(dev_name));
    memset(cmd, 0, sizeof(cmd));
    if(E_CREAT_WAN_DEVICE == action)
    {
    		memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -A FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", dev_name);

		memset(cmd6, 0, sizeof(cmd6));
		snprintf(cmd6, sizeof(cmd6), "/usr/bin/ip6tables -I FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", dev_name);		
    }
    else if (E_DEL_WAN_DEVICE == action)
    {
    		memset(cmd, 0, sizeof(cmd));
        snprintf(cmd,  sizeof(cmd),"/usr/bin/iptables -D FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", dev_name);
		
		memset(cmd6, 0, sizeof(cmd6));
		snprintf(cmd6, sizeof(cmd6), "/usr/bin/ip6tables -D FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", dev_name);
    }

    svc_wan_execute_cmd(cmd);
	svc_wan_execute_cmd(cmd6);

    return ;
}

static void svc_wan_config_intf_attr(wan_entry_obj_t* obj, int protocol_version)
{
    char cmd[SVC_WAN_BUF_128_LEN];
    wan_entry_cfg_t* cfg = NULL;
#if defined(TCSUPPORT_NP_CMCC)
	char buf[4] = {0};
	char path[SVC_WAN_BUF_128_LEN] = {0};
#endif

    cfg = &obj->cfg;
#if defined(TCSUPPORT_NP_CMCC)
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/disable_ipv6", obj->dev);
	memset(buf, 0, sizeof(buf));
	if ( E_SYSTEM_ONLY_IPV4_ENABLE == protocol_version )
	{
		strncpy(buf, "1", sizeof(buf) - 1);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%d", SVC_WAN_IS_IPV6(cfg) ? 0 : 1 );
	}
	doValPut(path, buf);
#else
    if((E_SYSTEM_ONLY_IPV4_ENABLE != protocol_version) && !SVC_WAN_IS_IPV6(cfg) )
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6", obj->dev);
        svc_wan_execute_cmd(cmd);
    }
#endif

    return ;
}
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
static int svc_wan_create_radvd_dhcp6s(wan_entry_obj_t* obj){
    char tmp[64] = {0};
    char radvd_node[32] = {0};
    char dhcp6s_node[32] = {0};
    char *pos = NULL;
    char nodeName[64] = {0};
    char configFile[64] = {0}, pidPath[64] = {0};
    char pid[8]={0};
	int other_NPT = 1;
	int pvc_index = 0, entry_index = 0;
	char wan_node[64] = {0};
	
	wan_entry_cfg_t* cfg = NULL;
	if(!obj)
		return 0;
	cfg = &obj->cfg;


	if ((SVC_WAN_SRV_OTHER(cfg)||SVC_WAN_SRV_VR(cfg)||SVC_WAN_SRV_SPE_SER(cfg)) && SVC_WAN_IS_IPV6(cfg))
	{
		snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, obj->idx+1);
		snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE, obj->idx+1);
		if (cfg_query_object(radvd_node,NULL,NULL) <= 0)
		{
			cfg_create_object(radvd_node);
		}
		if (cfg_query_object(dhcp6s_node,NULL,NULL) <= 0)
		{
			cfg_create_object(dhcp6s_node);
		}

		if(SVC_WAN_SRV_VR(cfg)){
			cfg_set_object_attr(radvd_node, "Type", "VR");
			cfg_set_object_attr(dhcp6s_node, "Type", "VR");
		}else if(SVC_WAN_SRV_OTHER(cfg) && SVC_WAN_IS_ROUTE(cfg) && 1 == cfg->npt){ 			
			cfg_set_object_attr(radvd_node, "Type", "OTHER");				
			cfg_set_object_attr(dhcp6s_node, "Type", "OTHER");			
		}else{
			cfg_set_object_attr(radvd_node, "Type", "");				
			cfg_set_object_attr(dhcp6s_node, "Type", "");
		}

		snprintf(tmp,sizeof(tmp),"%d",obj->idx);
		cfg_set_object_attr(radvd_node, "DelegatedWanConnection", tmp);
		cfg_set_object_attr(dhcp6s_node, "DNSWANConnection", tmp);

		if (cfg_get_object_attr(obj->path,SVC_WAN_ATTR_PD,tmp,sizeof(tmp)) > 0){
			if (strcasecmp(tmp,"Yes") == 0)
			{
				cfg_set_object_attr(radvd_node, "Enable", "1");
				cfg_set_object_attr(dhcp6s_node, "Enable", "1");
			}else{
				cfg_set_object_attr(radvd_node, "Enable", "0");
				cfg_set_object_attr(dhcp6s_node, "Enable", "0");
			}		
		}

		if (cfg_get_object_attr(obj->path,SVC_WAN_ATTR_PD_ORIGIN,tmp,sizeof(tmp)) > 0){
			if (strcasecmp(tmp,"PrefixDelegation") == 0)
			{
				cfg_set_object_attr(radvd_node, "Mode", "0");
				cfg_set_object_attr(dhcp6s_node, "Mode", "0");
				cfg_set_object_attr(radvd_node, "AutoPrefix", "0");
				cfg_set_object_attr(dhcp6s_node, "DNSType", "0");
				
			}else{
				cfg_set_object_attr(radvd_node, "Mode", "1");
				cfg_set_object_attr(dhcp6s_node, "Mode", "1");
				if (cfg_get_object_attr(obj->path,SVC_WAN_ATTR_PDADDR,tmp,sizeof(tmp)) > 0){
					pos = strtok(tmp, "/");
					if(pos)
					{
						cfg_set_object_attr(radvd_node, "PrefixIPv6", pos);
						cfg_set_object_attr(dhcp6s_node, "PrefixIPv6", pos);
						pos = strtok(NULL, "/");
						if (pos)
						{
							cfg_set_object_attr(radvd_node, "Prefixv6Len", pos);
							cfg_set_object_attr(dhcp6s_node, "Prefixv6Len", pos);
						}
					}
				}

				if (cfg_get_object_attr(obj->path,SVC_WAN_ATTR_PDTIME1,tmp,sizeof(tmp)) > 0){
					cfg_set_object_attr(radvd_node, "PreferredLifetime", tmp);
					cfg_set_object_attr(dhcp6s_node, "PreferredLifetime", tmp);
				}
				if (cfg_get_object_attr(obj->path,SVC_WAN_ATTR_PDTIME2,tmp,sizeof(tmp)) > 0){
					cfg_set_object_attr(radvd_node, "ValidLifetime", tmp);
					cfg_set_object_attr(dhcp6s_node, "ValidLifetime", tmp);
				}
			}
		}
		cfg_commit_object(radvd_node);
		cfg_commit_object(dhcp6s_node);
	}
}
#endif

static void svc_wan_do_something_creat_intf(wan_entry_obj_t* obj, int protocol_version)
{
    wan_entry_cfg_t* cfg = NULL;

    cfg = &obj->cfg;
    svc_wan_set_intf_name(obj);

    /**************port bind rule conf*************************/
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK) && !defined(TCSUPPORT_NP_CMCC)
#if defined(TCSUPPORT_CT_FULL_ROUTE)
    if(0 == full_route_execute(obj))
#endif
    {
        set_port_binding_info2(obj->path, E_DEL_PORT_BIND_RULE);
        set_port_binding_info2(obj->path, E_ADD_PORT_BIND_RULE);
    }
#endif

    /*********************************************/
    if(SVC_WAN_IS_BRIDGE(cfg))
    {
        set_DHCPRelay_info(obj, E_ADD_DHCPRELAY_RULE);
    }

#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
    set_DHCP_PortFilter_info(obj, E_ADD_DHCP_PORT_FILTER_RULE);
#endif

    check_nat_enable();

    svc_wan_mtu_conf(obj, E_CREAT_WAN_DEVICE);

    svc_wan_config_intf_attr(obj, protocol_version);

#if !defined(TCSUPPORT_NP_CMCC)
    svc_wan_vlanbind_execute(obj,0);
#endif

#if defined(TCSUPPORT_CT_L2TP_VPN)
    restartVPN(obj);
#endif
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
    svc_wan_create_radvd_dhcp6s(obj);
#endif

    return ;
}
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
static int svc_wan_delete_radvd_dhcp6s(wan_entry_obj_t* obj){
    int id[CFG_MAX_NODE_LEVEL];
    unsigned int pvc,entry;
    char configFile[64] = {0}, pidPath[64] = {0};
    char pid[8]={0};
    char radvd_node[32] = {0};
    char dhcp6s_node[32] = {0};

	snprintf(pidPath, sizeof(pidPath), "/var/run/radvd_%d.pid", obj->idx);
	snprintf(configFile, sizeof(configFile), "/etc/radvd_%d.conf", obj->idx);
	snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, obj->idx+1);

	fileRead(pidPath, pid, sizeof(pid));

	if(strlen(pid) > 0){
		kill(atoi(pid), SIGTERM);
		unlink(pidPath);
	}
	unlink(configFile);	

	if (cfg_query_object(radvd_node,NULL,NULL) > 0)
	{
		cfg_delete_object(radvd_node);
	}

	snprintf(pidPath, sizeof(pidPath), "/var/run/dhcp6s_%d.pid", obj->idx);
	snprintf(configFile, sizeof(configFile), "/etc/dhcp6s_%d.conf", obj->idx);
	snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE, obj->idx+1);

	fileRead(pidPath, pid, sizeof(pid));

	if(strlen(pid) > 0){
		kill(atoi(pid), SIGTERM);
		unlink(pidPath);
	}
	unlink(configFile);

	if (cfg_query_object(dhcp6s_node,NULL,NULL) > 0)
	{
		cfg_delete_object(dhcp6s_node);
	} 
}
#endif


#if defined(TCSUPPORT_CT_IPOE_DETECT)
void svc_wan_stop_ipoe_diag(wan_entry_obj_t* obj)
{
	int i = -1, pid = -1;
	char nodeName[64] = {0}, wannode[64] = {0}, tmp[32] = {0}, filepath[32] = {0};
	struct stat fstat;
	wan_entry_cfg_t* cfg = NULL;

	if ( NULL == obj )
		return;

	cfg = &obj->cfg;
	if ( SVC_WAN_SRV_VR(cfg) )
	{
		memset(wannode, 0, sizeof(wannode));
		memset(tmp, 0, sizeof(tmp));
		snprintf(wannode, sizeof(wannode), WAN_PVC_ENTRY_NODE, obj->idx / SVC_WAN_MAX_PVC_NUM + 1, obj->idx % SVC_WAN_MAX_ENTRY_NUM + 1);
		cfg_get_object_attr(wannode, "ipoediag_index", tmp, sizeof(tmp));
		if ( 0 != tmp[0] && 0 != strcmp(tmp, "-1") )
		{
			i = atoi(tmp);

			/* double check ipoe & wan index mapping */
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), IPOEDIAG_ENTRY_NODE, i + 1);
			memset(tmp, 0, sizeof(tmp));
			cfg_get_object_attr(nodeName, "wan_index", tmp, sizeof(tmp));
			if ( 0 == tmp[0] || obj->idx != atoi(tmp) )
				i = -1;
		}

		if ( -1 == i )
		{
			for ( i < 0; i < MAX_IPOE_DIAG_NUM; i++ )
			{
				memset(nodeName, 0, sizeof(nodeName));
				memset(tmp, 0, sizeof(tmp));
				snprintf(nodeName, sizeof(nodeName), IPOEDIAG_ENTRY_NODE, i + 1);
				cfg_get_object_attr(nodeName, "wan_index", tmp, sizeof(tmp));
				if ( 0 != tmp[0] && obj->idx == atoi(tmp) )
					break;
			}
			if ( MAX_IPOE_DIAG_NUM == i )
				return;

			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", i);
			cfg_set_object_attr(wannode, "ipoediag_index", tmp);
		}

		snprintf(filepath, sizeof(filepath), "/var/run/ipoediag_%d", i);
		if( 0 == stat(filepath, &fstat) )
		{
			pid = svc_wan_get_pid(filepath);
			kill(pid, SIGTERM);
			unlink(filepath);
		}
	}
}
#endif

static void svc_wan_do_something_del_intf(wan_entry_obj_t* obj)
{
    svc_wan_mtu_conf(obj, E_DEL_WAN_DEVICE);

#if defined(TCSUPPORT_CT_FULL_ROUTE)
    close_table_by_ifIndex(obj->idx);
#endif
#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
    set_port_binding_info2(obj->path, E_DEL_PORT_BIND_RULE);
#endif

#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
    set_DHCP_PortFilter_info(obj, E_DEL_DHCP_PORT_FILTER_RULE);
#endif

    set_DHCPRelay_info(obj, E_DEL_DHCPRELAY_RULE);

#if defined(TCSUPPORT_CT_DSLITE)
    stop_dslite(obj);
#endif
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
    svc_wan_delete_radvd_dhcp6s(obj);
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
	svc_wan_stop_ipoe_diag(obj);
#endif
    return ;
}

int svc_wan_do_start_intf(wan_entry_obj_t* obj)
{
    int protocol_version = E_SYSTEM_IPV4_IPV6_ENABLE;
    wan_entry_cfg_t* cfg = &obj->cfg;

    protocol_version = svc_wan_cfg_get_system_protocol_version();

    if (svc_wan_create_conf(obj) < 0)
    {
        return -1;
    }
#if !defined(TCSUPPORT_NP_CMCC)
    if (svc_wan_create_device(obj) < 0)
    {
        return -1;
    }
 #endif
   svc_wan_do_something_creat_intf(obj, protocol_version);
    
#if !defined(TCSUPPORT_NP_CMCC)
    if (svc_wan_start_device(obj) < 0)
    {
        goto error;
    }
#endif

    if (SVC_WAN_IS_BRIDGE(cfg))
    {
        svc_wan_start_bridge(obj);
    }
    else if (SVC_WAN_IS_PPP(cfg))
    {
        svc_wan_start_ppp(obj, protocol_version);
    }
    else if (SVC_WAN_IS_IP(cfg))
    {
        if(E_SYSTEM_ONLY_IPV6_ENABLE != protocol_version)
        {
            if (SVC_WAN_IS_IPV4(cfg))
            {
                if (SVC_WAN_IS_IPV4_DHCP(cfg) && (!SVC_WAN_DSLITE_ENABLE(cfg)))
                {
                    svc_wan_start_ipv4_dhcp(obj);
                }
                else if (SVC_WAN_IS_IPV4_STATIC(cfg))
                {
                    svc_wan_start_ipv4_static(obj);
                }
                else if(SVC_WAN_IS_IPV6(cfg))
                {
                    /****do nothing, bug not go to error. double stack****/
                }
                else
                {
                    goto error;
                }
            }
        }

        if(E_SYSTEM_ONLY_IPV4_ENABLE != protocol_version)
        {
            if (SVC_WAN_IS_IPV6(cfg))
            {
                if (SVC_WAN_IS_IPV6_DHCP(cfg))
                {
                    svc_wan_start_ipv6_dhcp(obj);
                }
                else if (SVC_WAN_IS_IPV6_STATIC(cfg))
                {
                    svc_wan_start_ipv6_static(obj);
                }
                else if (SVC_WAN_IS_IPV6_SLAAC(cfg))
                {
                    svc_wan_start_ipv6_slaac(obj);
                }
                else
                {
                    goto error;
                }
            }
        }
    }
    else 
    {
        goto  error;
    }

    return 0;

error:
    svc_wan_do_something_del_intf(obj);
#if !defined(TCSUPPORT_NP_CMCC)
    svc_wan_delete_device(obj->dev);
#endif
    svc_wan_delete_conf(obj);
    SVC_WAN_ERROR_INFO("svc_wan_do_start_intf: version[%d] isp[%d] linkmode[%d] mode[%d]",cfg->version,cfg->isp,cfg->linkmode,cfg->wanmode);	
    return -1;
}

int svc_wan_do_stop_intf(wan_entry_obj_t* obj)
{
    wan_entry_cfg_t* cfg = &obj->cfg;

    if (SVC_WAN_IS_BRIDGE(cfg))
    {
        svc_wan_stop_bridge(obj);
    }
    else if (SVC_WAN_IS_PPP(cfg))
    {
        svc_wan_stop_ppp(obj);
    }
    else if (SVC_WAN_IS_IP(cfg))
    {
        if (SVC_WAN_IS_IPV4(cfg))
        {
            if (SVC_WAN_IS_IPV4_DHCP(cfg))
            {
                svc_wan_stop_ipv4_dhcp(obj);
            }
            else if (SVC_WAN_IS_IPV4_STATIC(cfg))
            {
                svc_wan_stop_ipv4_static(obj);
            }
            else if(SVC_WAN_IS_IPV6(cfg))
            {
                /****do nothing, bug not go to error. double stack****/
            }
            else
            {
                goto  error;
            }
        }

        if (SVC_WAN_IS_IPV6(cfg))
        {
            if (SVC_WAN_IS_IPV6_DHCP(cfg))
            {
                svc_wan_stop_ipv6_dhcp(obj);
            }
            else if (SVC_WAN_IS_IPV6_STATIC(cfg))
            {
                svc_wan_stop_ipv6_static(obj);
            }
            else if (SVC_WAN_IS_IPV6_SLAAC(cfg))
            {
                svc_wan_stop_ipv6_slaac(obj);
            }
            else
            {
                goto  error;
            }
        }
    }
    else 
    {
        goto  error;
    }

    svc_wan_do_something_del_intf(obj);

#if !defined(TCSUPPORT_NP_CMCC)
    svc_wan_stop_device(obj);

    svc_wan_delete_device(obj->dev);
#endif

#if defined(TCSUPPORT_CT_L2TP_VPN)
    restartVPN(obj);
#endif

    svc_wan_delete_conf(obj);

    return 0;

error:
    SVC_WAN_ERROR_INFO("svc_wan_do_start_intf: version[%d] isp[%d] linkmode[%d] mode[%d]",cfg->version,cfg->isp,cfg->linkmode,cfg->wanmode);	
    return -1;	
}

