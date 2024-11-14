#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cfg_api.h>
#include "wan_common.h"
#include "wan_link_list.h"
#include "utility.h"
#include "blapi_traffic.h"
/**********************************************************************************************/
static int  svc_wan_get_all_wan_index(int indexbuf[]);
static int create_br0_v6addr(char *pd, int pd_len, char *newV6addr);
/**********************************************************************************************/
#if defined(TCSUPPORT_CT_JOYME4)
void lan_icmpv6_update(wan_entry_obj_t* obj ,int evt)
{
	char cmd[SVC_WAN_BUF_128_LEN];
	wan_entry_cfg_t* cfg = &obj->cfg;

	if(evt == EVT_WAN_CONN_GETV6)
	{
		if(cfg->orgpd6addr[0] != '\0')
		{
			memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "ip6tables -I LAN_DROP_ICMPV6 -s %s -i br0 -p icmpv6 -j ACCEPT", cfg->orgpd6addr);
	        system(cmd);
	        
		}
	}
	else if(evt == EVT_WAN_CONN_LOSTV6)
	{
		if(cfg->orgpd6addr[0] != '\0')
		{
			memset(cmd, 0, sizeof(cmd));
	        snprintf(cmd, sizeof(cmd), "ip6tables -D LAN_DROP_ICMPV6 -s %s -i br0 -p icmpv6 -j ACCEPT", cfg->orgpd6addr);
	        system(cmd);
	        
		}
	}

}
#endif

int check_pd_enable(int ifindex)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char other_pd[8];
    char internet_pd[8];
    char pd_index[6];
    char node[32];
    char nodeName[32];
    int pvc_index = 0;
    int entry_index = 8;
    char def_route_index_v6[SVC_WAN_BUF_16_LEN];

    pvc_index   = (ifindex / SVC_WAN_MAX_ENTRY_NUM) + 1;
    entry_index = (ifindex % SVC_WAN_MAX_ENTRY_NUM) + 1;
    
    memset(nodeName, 0, sizeof(nodeName));
    snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
    if((obj = svc_wan_find_object(nodeName)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not find object\n");
        return -1;
    }
    
    cfg = &obj->cfg;

#if defined(TCSUPPORT_CT_JOYME4)
	/*
		1. JOYME4 add some iptables rules after update default route and it will cause some time.
		2. Another INTERNET will do check_pd_enable() faster then default route, because it won't do default route and lan iptables rules.
		3. PD will use NON default route internet wan as step 1-2.
		4. Add code for checking default route update done.
	*/
    strcpy(node, WANINFO_COMMON_NODE);
    bzero(def_route_index_v6, sizeof(def_route_index_v6));
    if ( cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6)) > 0
		&& 0 != strcmp(def_route_index_v6, "N/A") )
    {
        if ( obj->idx != atoi(def_route_index_v6) )
        {
			return 0;
        }
    }
#endif

    if(SVC_WAN_IS_ROUTE(cfg) && SVC_WAN_IPV6_PD_ENABLE(cfg))
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WANINFO_COMMON_NODE);

        memset(internet_pd, 0, sizeof(internet_pd));
        if(cfg_get_object_attr(node, "PDINTERNETIFIdx", internet_pd, sizeof(internet_pd)) < 0)
        {
            snprintf(internet_pd, sizeof(internet_pd), "N/A");
        }

        memset(other_pd, 0, sizeof(other_pd));
        if(cfg_get_object_attr(node, "PDOTHERIFIdx", other_pd, sizeof(other_pd)) < 0)
        {
            snprintf(other_pd, sizeof(other_pd), "N/A");
        }

        if(SVC_WAN_SRV_INTERNET(cfg) && (0 == strcmp(internet_pd, "N/A")))
        {
            memset(pd_index, 0, sizeof(pd_index));
            snprintf(pd_index, sizeof(pd_index), "%d", obj->idx);
            cfg_set_object_attr(node,"PDINTERNETIFIdx", pd_index);
            cfg_set_object_attr(node,"PDRUNIFIdx",      pd_index);

            return 1;
        }
        else if((SVC_WAN_SRV_OTHER(cfg)
#if defined(TCSUPPORT_CMCCV2)
				|| SVC_WAN_SRV_IPTV(cfg) || SVC_WAN_SRV_OTT(cfg)
#endif
				) && (0 == strcmp(other_pd, "N/A")) && (0 == strcmp(internet_pd, "N/A")))
        {
#ifndef TCSUPPORT_CT_MULTI_LAN_PD
            memset(pd_index, 0, sizeof(pd_index));
            snprintf(pd_index, sizeof(pd_index), "%d", obj->idx);
            cfg_set_object_attr(node,"PDINTERNETIFIdx", pd_index);
            cfg_set_object_attr(node,"PDRUNIFIdx",      pd_index);
#endif
            return 1;
        }
    }

    return 0;
}

int get_dhcp6s_dns_wan_interface(wan_entry_obj_t* obj)
{
    char val_buf[8] = {0};
    char node[32] = {0};
	int pvc_idx = 0, entry_idx = 0;
	char wan_node[64] = {0}, tmpBuf[8] = {0};
	int radvd_idx = 0, dhcp6s_idx = 0, isinternet = 0;
	int isvrwan = 0;

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
#if defined(TCSUPPORT_CT_VRWAN)
	isvrwan = check_vr_wan(obj);
#endif

	wan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;

	if( SVC_WAN_SRV_INTERNET(cfg) )	
		isinternet = 1;

    memset(node, 0, sizeof(node));

	if( 0 == isinternet )
		snprintf(node, sizeof(node), DHCP6S_ENTRY_NODE, obj->idx+1);
	else
		snprintf(node, sizeof(node), DHCP6S_COMMON_NODE);
#else
    snprintf(node, sizeof(node), DHCP6S_ENTRY_NODE);
#endif
    memset(val_buf, 0, sizeof(val_buf));
    if(cfg_get_object_attr(node, "DNSType", val_buf, sizeof(val_buf)) < 0)
    {
        return -1;
    }

    if(0 == atoi(val_buf)
#if defined(TCSUPPORT_CT_VRWAN)
	|| 1 == isvrwan 
#endif
	)
    {
        memset(val_buf, 0, sizeof(val_buf));
        if(cfg_get_object_attr(node, "DNSWANConnection", val_buf, sizeof(val_buf)) < 0)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return atoi(val_buf);
}

static int  svc_wan_get_all_wan_index(int indexbuf[])
{
    wan_entry_obj_t* obj = NULL;

    int wan_entry_num = 0;
    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        *indexbuf = obj->idx;
        indexbuf++;
        wan_entry_num++;
    }

    return wan_entry_num;	
}

int get_run_pd_index(void)
{
    char pd_index[6];
    char nodeName[32];

    memset(pd_index, 0, sizeof(pd_index));
    memset(nodeName, 0, sizeof(nodeName));


    snprintf(nodeName, sizeof(nodeName), WANINFO_COMMON_NODE);

    if(cfg_get_object_attr(nodeName, "PDRUNIFIdx", pd_index, sizeof(pd_index)) < 0)
    {
        return -1;
    }

    if( 0 == strcmp(pd_index, "N/A") )
    {
        return -2;
    }

    return atoi(pd_index);
}

static int get_pd_index(int ifindex)
{
    int valid_if_num = 0;
    int valid_if[SVC_MAX_WAN_INTF_NUMBER];
    int i = 0;
    int internet_if = -1;
    int other_if = -1;
#if defined(TCSUPPORT_CMCCV2)
	int iptv_if = -1;
	int ott_if = -1;
#endif
    char nodeName[32];
    int pvc_index = 0;
    int entry_index = 8;
    char str_status6[8];
    char str_pd6[64];
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char itf_flag = 0;
    valid_if_num = svc_wan_get_all_wan_index(valid_if);
    for(i = 0; i < valid_if_num; i++)
    {
        if (ifindex == valid_if[i])
        {
            continue;
        }
        if ( -1 == internet_if )
        {
            memset(nodeName, 0, sizeof(nodeName));
            pvc_index   = (valid_if[i] / SVC_WAN_MAX_ENTRY_NUM) + 1;
            entry_index = (valid_if[i] % SVC_WAN_MAX_ENTRY_NUM) + 1;
            snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

            obj = NULL;
            cfg = NULL;
            if((obj = svc_wan_find_object(nodeName)) == NULL)
            {
                SVC_WAN_ERROR_INFO("not find object\n");
                return -1;
            }

            cfg = &obj->cfg;
            if(SVC_WAN_IS_ACTIVE(cfg) && SVC_WAN_IS_ROUTE(cfg) && SVC_WAN_IPV6_PD_ENABLE(cfg))
            {
                memset(nodeName, 0, sizeof(nodeName));
                snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE,obj->idx + 1);
                memset(str_status6, 0, sizeof(str_status6));
                if(cfg_get_object_attr(nodeName, "Status6", str_status6, sizeof(str_status6)) < 0)
                {
                    strcpy(str_status6, "N/A");
                }
                memset(str_pd6, 0, sizeof(str_pd6));
                if(cfg_get_object_attr(nodeName, "PD6", str_pd6, sizeof(str_pd6)) < 0)
                {
                    strcpy(str_pd6, "N/A");
                }

                if(('\0' != str_pd6[0] && 0 != strcmp(str_pd6, "N/A")) && 0 == strcmp(str_status6, "up"))
                {
                    itf_flag = 1;
                }

                if(SVC_WAN_SRV_INTERNET(cfg) && (1 == itf_flag))
                {
                    internet_if = valid_if[i];
                }
                else if ((-1 == other_if) && SVC_WAN_SRV_OTHER(cfg) && (1 == itf_flag))
                {
                    other_if = valid_if[i];
                }
#if defined(TCSUPPORT_CMCCV2)
				else if ((-1 == iptv_if) && SVC_WAN_SRV_IPTV(cfg) && (1 == itf_flag))
				{
					iptv_if = valid_if[i];
				}
				else if ((-1 == ott_if) && SVC_WAN_SRV_OTT(cfg) && (1 == itf_flag))
				{
					ott_if = valid_if[i];
				}
#endif
            }
        }
    }

	if ( -1 == internet_if )
	{
#if defined(TCSUPPORT_CMCCV2)
		if( -1 == other_if )
		{
			if( -1 == iptv_if )
				internet_if = ott_if;
			else
				internet_if = iptv_if;
		}
		else	
#endif
			internet_if = other_if;
	}

    return ( -1 != internet_if ) ? internet_if : -1;
}

void update_run_pd_index(int ifindex)
{
    char nodeName[32];
    int pd_ifindex = get_pd_index(ifindex);
    memset(nodeName, 0, sizeof(nodeName));
    snprintf(nodeName, sizeof(nodeName), WANINFO_COMMON_NODE);

    cfg_set_object_attr(nodeName, "PDINTERNETIFIdx", "N/A");
    cfg_set_object_attr(nodeName, "PDOTHERIFIdx", 	 "N/A");
    cfg_set_object_attr(nodeName, "PDRUNIFIdx", 	 "N/A");

    if( -1 == pd_ifindex )
    {
        SVC_WAN_ERROR_INFO("pd_ifindex = %d\n", pd_ifindex);
        return;
    }

    check_pd_enable(pd_ifindex);
    
    return ;
}

int get_ra_wan_pd_interface(wan_entry_obj_t* obj)
{
    char nodeName[32];
    char val_buf[8];
    char active[8];
    int pdIdx = 0;
    int pvc_idx = 0, entry_idx = 0;
	char wan_node[64] = {0}, tmpBuf[8] = {0};
	int radvd_idx = 0, dhcp6s_idx = 0, isvrwan = 0, isinternet = 0;
	
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
#if defined(TCSUPPORT_CT_VRWAN)
	isvrwan = check_vr_wan(obj);
#endif
	wan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;

	if( SVC_WAN_SRV_INTERNET(cfg) )	
		isinternet = 1;

    memset(nodeName, 0, sizeof(nodeName));
	if( 0 == isinternet )
		snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE, obj->idx+1);
	else
		snprintf(nodeName, sizeof(nodeName), RADVD_COMMON_NODE);
#else
    snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE);
#endif

    memset(val_buf, 0, sizeof(val_buf));
    if(cfg_get_object_attr(nodeName, "AutoPrefix", val_buf, sizeof(val_buf)) < 0)
    {
        return get_run_pd_index();
    }
    
    if ( 0 == atoi(val_buf) )
    {
        memset(val_buf, 0, sizeof(val_buf));
        if(cfg_get_object_attr(nodeName, "DelegatedWanConnection", val_buf, sizeof(val_buf)) < 0)
        {
            return get_run_pd_index();
        }
        else
        {
            if ( '\0' != val_buf[0] )
            {
#if defined(TCSUPPORT_CT_VRWAN)
            	if( 1 != isvrwan )
            	{
#endif
                pdIdx = atoi(val_buf);
                pvc_idx = pdIdx / SVC_WAN_MAX_ENTRY_NUM + 1;
                entry_idx = pdIdx % SVC_WAN_MAX_ENTRY_NUM + 1;

                memset(nodeName, 0, sizeof(nodeName));
                snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_idx,entry_idx);
                memset(active, 0, sizeof(active));
                if(cfg_get_object_attr(nodeName, "Active", active, sizeof(active)) < 0)
                {
                    memset(nodeName, 0, sizeof(nodeName));
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
                    if( 0 == isinternet )
			            snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE, obj->idx+1);
                    else
                        snprintf(nodeName, sizeof(nodeName), RADVD_COMMON_NODE);
#else
                    snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE);
#endif
                    cfg_set_object_attr(nodeName, "DelegatedWanConnection", "");
                    return get_run_pd_index();
                }
#if defined(TCSUPPORT_CT_VRWAN)
            	}
#endif
            }
            else
            {
                return get_run_pd_index();
            }
        }
    }
    else
    {
        return -1;
    }

    return atoi(val_buf);
}

void restart_pd_function(int type, wan_entry_obj_t* obj)
{
    char radvd_node[32] = {0}, dhcp6s_node[32] = {0};
    char en[5];
    char mode[5];

    int radvd_idx = 0, dhcp6s_idx = 0, isdefault = 0;
    wan_entry_cfg_t* cfg = NULL;
    cfg = &obj->cfg;
    if( SVC_WAN_SRV_INTERNET(cfg) )
        isdefault = 1;
	
	
#if defined(TCSUPPORT_CT_VRWAN)

	if( 0 == isdefault )
	{
		snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, obj->idx+1);
		snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE, obj->idx+1);
	}
	else
	{
		snprintf(radvd_node, sizeof(radvd_node), RADVD_COMMON_NODE);
		snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_COMMON_NODE);
	}
#else
	snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE);
	snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE);
#endif

    if ( 0 == type || 2 == type )
    {
        memset(en, 0, sizeof(en));
        memset(mode, 0, sizeof(mode));
        if( (cfg_get_object_attr(dhcp6s_node, "Enable", en, sizeof(en)) > 0) \
            && (cfg_get_object_attr(dhcp6s_node, "Mode", mode, sizeof(mode)) > 0))
        {
            /*Dhcp6s is enabled and auto mode*/
            if( (( '1' == en[0] ) && ( '0' == mode[0] || '2' == mode[0])))
            {
                cfg_commit_object(dhcp6s_node);
            }
        }
    }

    if ( 1 == type || 2 == type )
    {
        /*Set Radvd prefix value*/
        memset(en, 0, sizeof(en));
        memset(mode, 0, sizeof(mode));
        if((cfg_get_object_attr(radvd_node, "Enable", en, sizeof(en)) > 0) \
            && (cfg_get_object_attr(radvd_node, "Mode", mode, sizeof(mode)) > 0))
        {
            /*Radvd is enabled and auto mode*/
            if( (( '1' == en[0] ) && ( '0' == mode[0])))
            {
                cfg_commit_object(radvd_node);
            }
        }
    }

    return;
}

static int create_br0_v6addr(char *pd, int pd_len, char *newV6addr)
{
    int i = 0;
    unsigned char ifid[16] = {0};
    char tmp_str[64] = {0};
    struct in6_addr in_s_v6addr;

    if ( NULL == pd || NULL == newV6addr )
    {
        return -1;
    }

    if ( pd_len < 16 )
    {
        SVC_WAN_ERROR_INFO("error:create_br0_v6addr() pd length is too short! \n");
        return -1;
    }

    memset(&in_s_v6addr, 0, sizeof(in_s_v6addr));
	get_br0_v6addr(ifid);
    if ( 1 != inet_pton(AF_INET6, pd, &in_s_v6addr) )
    {
        return -1;
    }


    for (i = 15; i >= pd_len / 8; i--)
    {
        in_s_v6addr.s6_addr[i] = ifid[i];
    }

    inet_ntop(AF_INET6, &in_s_v6addr.s6_addr, tmp_str, sizeof(tmp_str));
    strcpy(newV6addr, tmp_str);
    
    return 0;
}

int update_br0_pdaddress(int if_index, int evt)
{
    char nodeName[32];
    char cmd[128];
    char ipv6_br0[64]= {0};
    char dhcpv6_wanpd_prefix[64] = {0}, dhcpv6_wanpd_prefix_len[12] = {0};
    char v6addr_str[64] = {0};
    char *pos = NULL;
    int pvc_index = 0, entry_index = 0;
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;

    pvc_index = if_index / SVC_WAN_MAX_ENTRY_NUM + 1;
    entry_index = if_index % SVC_WAN_MAX_ENTRY_NUM + 1;

    memset(nodeName, 0, sizeof(nodeName));
    snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

    if((obj = svc_wan_find_object(nodeName)) == NULL)
    {
        SVC_WAN_ERROR_INFO("not found object.\n");
        return 0;
    }

    cfg = &obj->cfg;
    if((cfg_get_object_attr(nodeName, "Br0Addr", ipv6_br0, sizeof(ipv6_br0)) > 0) \
        && (0 != strcmp(ipv6_br0, "N/A")))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 del %s", ipv6_br0);
        svc_wan_execute_cmd(cmd);

        cfg_set_object_attr(nodeName, "Br0Addr", "N/A");
    }

    if(!SVC_WAN_IS_ACTIVE(cfg))
    {
        return 0;
    }

    if(SVC_WAN_IS_ROUTE(cfg) && SVC_WAN_IS_IPV6(cfg) && !SVC_WAN_IPV6_DHCPV6PD_ENABLE(cfg) \
        && (0 == strcmp(cfg->pdorigin, "Static")) && (SVC_WAN_SRV_INTERNET(cfg) || SVC_WAN_SRV_OTHER(cfg) 
#if defined(TCSUPPORT_CMCCV2)
		|| SVC_WAN_SRV_IPTV(cfg) || SVC_WAN_SRV_OTT(cfg)
#endif
		))
    {
        if ( EVT_WAN_CONN_GETV6 == evt)
        {
            if(strlen(cfg->pdaddr) <= 0)
            {
                SVC_WAN_ERROR_INFO("pdaddr length is 0.\n");
                return 0;
            }

            pos = strtok(cfg->pdaddr, "/");
            if ( pos )
            {
                memset(dhcpv6_wanpd_prefix, 0, sizeof(dhcpv6_wanpd_prefix));
                snprintf(dhcpv6_wanpd_prefix, sizeof(dhcpv6_wanpd_prefix) - 1, "%s", pos);
                pos = strtok(NULL, "/");
                if ( pos )
                {
                    memset(dhcpv6_wanpd_prefix_len, 0, sizeof(dhcpv6_wanpd_prefix_len));
                    snprintf(dhcpv6_wanpd_prefix_len, sizeof(dhcpv6_wanpd_prefix_len) - 1, "%s", pos);
                }
            }
            
            if ( '\0' != dhcpv6_wanpd_prefix[0] && '\0' != dhcpv6_wanpd_prefix_len[0] )
            {
                if ( 0 != create_br0_v6addr(dhcpv6_wanpd_prefix, atoi(dhcpv6_wanpd_prefix_len), v6addr_str) )
                {
                    SVC_WAN_ERROR_INFO("creat br0 v6 addr faile.\n");
                    return 0;
                }

                memset(ipv6_br0, 0, sizeof(ipv6_br0));
                snprintf(ipv6_br0, sizeof(ipv6_br0) - 1, "%s/%s", v6addr_str, dhcpv6_wanpd_prefix_len);
                snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 %s", ipv6_br0);
                svc_wan_execute_cmd(cmd);
                // save br0 address
                cfg_set_object_attr(nodeName, "Br0Addr", ipv6_br0);
            }
        }
    }
    return 0;
}

#if defined(TCSUPPORT_CT_VRWAN)
int send_update_evt_to_ra_dhcp(wan_entry_obj_t* obj, char *value)
{
	char nodeName[64] = {0}, radvd_node[64] = {0};
	char vrwan_idx[8] = {0};
	int index = 0;
	wan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;

	if( SVC_WAN_SRV_INTERNET(cfg) )
	{
		index = get_vr_Entry_index("VR");
		if(index >= 0)
			snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, index+1);
		else
			return 0;
		if( cfg_get_object_attr(radvd_node, "DelegatedWanConnection", vrwan_idx, sizeof(vrwan_idx)) <= 0
			|| 0 == atoi(vrwan_idx) )
		{
			return 0;
		}

		snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE, index+1);
		cfg_commit_object(nodeName);
		
		snprintf(nodeName, sizeof(nodeName), DHCP6S_ENTRY_NODE, index+1);
		cfg_commit_object(nodeName);
	}
	return 0;
}

int setVRNPTRule(int action, wan_entry_obj_t* obj)
{
	char waninfo_node[64] = {0},PD6[128] = {0}, radvd_node[64] = {0};
	char vrwan_index[8] = {0}, wan_node[64] = {0},rule_action[8] = {0};
	char cmd[256] = {0}, ifname[16] = {0}, tmpbuf[16] = {0};
	char prefix[128] = {0}, prefixlen[8] = {0}, defrouteidx[8] = {0}, defPD6[128] = {0};
	int pvc_index = 0, entry_index = 0, i = 0;
	char vrwan_idx[8] = {0}, linkmode[8] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;
	int index = -1;

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -D POSTROUTING -j VR_POST_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -D PREROUTING -j VR_PRE_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -F VR_POST_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -F VR_PRE_CHAIN -w");
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -X VR_POST_CHAIN -w");
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -X VR_PRE_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -D FORWARD -j VRCHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -F VRCHAIN -w");
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -X VRCHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -N VR_POST_CHAIN -w");
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -N VR_PRE_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -I POSTROUTING -j VR_POST_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -I PREROUTING -j VR_PRE_CHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -N VRCHAIN -w");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ip6tables -t filter -I FORWARD -j VRCHAIN -w");
	system(cmd);
	
	blapi_traffic_del_nptv6_prefix_all();
	if( cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", defrouteidx, sizeof(defrouteidx)) > 0 )
	{
		memset(waninfo_node, 0, sizeof(waninfo_node));
		snprintf(waninfo_node, sizeof(waninfo_node), WANINFO_ENTRY_NODE, atoi(defrouteidx) + 1);
		if(cfg_get_object_attr(waninfo_node, "PD6", defPD6, sizeof(defPD6)) <= 0)
		{
			snprintf(defPD6, sizeof(defPD6), "fd00::/64");
		}
	}
	else
	{
		snprintf(defPD6, sizeof(defPD6), "fd00::/64");
	}
	
	pvc_index	= (atoi(defrouteidx) / SVC_WAN_MAX_ENTRY_NUM);
	entry_index = (atoi(defrouteidx) % SVC_WAN_MAX_ENTRY_NUM);

	snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);
	if(cfg_get_object_attr(wan_node, "Active", tmpbuf, sizeof(tmpbuf)) <= 0 || strcmp(tmpbuf, "Yes"))
	{
		snprintf(defPD6, sizeof(defPD6), "fd00::/64");
	}

	for( i = 0; i < 2; i++ )
	{
		memset(vrwan_idx, 0, sizeof(vrwan_idx));
		memset(radvd_node, 0, sizeof(radvd_node));
		if( 0 == i )
		{
			index = get_vr_Entry_index("VR");
			if( index >= 0 )
				snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, index + 1);
			else
				continue;

			cfg_get_object_attr(radvd_node, "vr_index", vrwan_idx, sizeof(vrwan_idx));
		}
		else
		{
			index = get_vr_Entry_index("OTHER");
			if( index >= 0 )
				snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, index + 1);
			else
				continue;

			cfg_get_object_attr(radvd_node, "other_index", vrwan_idx, sizeof(vrwan_idx));
		}
		
		if( 0 == vrwan_idx[0] )
			continue;
		
		pvc_index	= (atoi(vrwan_idx) / SVC_WAN_MAX_ENTRY_NUM);
		entry_index = (atoi(vrwan_idx) % SVC_WAN_MAX_ENTRY_NUM);

		snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);
		if(cfg_get_object_attr(wan_node, "NPT", tmpbuf, sizeof(tmpbuf)) <= 0 || strcmp(tmpbuf, "1"))
		{
			continue;
		}
		
		snprintf(waninfo_node, sizeof(waninfo_node), WANINFO_ENTRY_NODE, atoi(vrwan_idx) + 1);

		if(cfg_get_object_attr(waninfo_node, "PD6", PD6, sizeof(PD6)) <= 0)
		{
			continue;
		}
		
		if(cfg_get_object_attr(wan_node, "IFName", ifname, sizeof(ifname)) <= 0)
		{
			continue;
		}
		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ip6tables -t filter -I VRCHAIN -i %s -d %s -j ACCEPT -w", ifname, defPD6);
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ip6tables -t filter -I VRCHAIN -i br0 -o %s -p tcp -j ACCEPT -w", ifname);
		system(cmd);
		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -I VR_POST_CHAIN -s %s -o %s -j SNPT --src-pfx %s --dst-pfx %s -w",
									defPD6, ifname, defPD6, PD6);
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ip6tables -t mangle -I VR_PRE_CHAIN -i %s -d %s -j DNPT --src-pfx %s --dst-pfx %s -w", 
									ifname, PD6, PD6, defPD6);
		system(cmd);

		if (0 == strncmp(defPD6,"fd00::/64",sizeof(defPD6)-1) && 0 == create_br0_v6addr("fd00::", 64, prefix) )
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 %s/64", prefix);
			system(cmd);
		}

		blapi_traffic_add_nptv6_prefix(ifname,PD6,defPD6);
	}

	return 0;
}

int set_ipv6_wan_index(wan_entry_obj_t* obj, int action)
{
	char nodeName[64] = {0}, buf[16] = {0}, linkmode[8] = {0};
	int radvd_idx = 0, dhcp6s_idx = 0;
	char radvd_node[64] = {0}, dhcp6s_node[64] = {0};
	char attrname[32] = {0};
	wan_entry_cfg_t* cfg = &obj->cfg;

	if( SVC_WAN_SRV_OTHER(cfg) )
	{
		snprintf(attrname, sizeof(attrname), "other_index");
	}
	else if( SVC_WAN_SRV_VR(cfg) )
	{
		snprintf(attrname, sizeof(attrname), "vr_index");
	}
	else
	{
		return 0;
	}
	
	if( 0 == action )
	{
		snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, obj->idx+1);
		snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE, obj->idx+1);
		cfg_set_object_attr(dhcp6s_node, "DNSWANConnection", "");
		cfg_set_object_attr(radvd_node, attrname, "");
		cfg_set_object_attr(radvd_node, "DelegatedWanConnection", "");

		if( SVC_WAN_SRV_OTHER(cfg) ){
			cfg_set_object_attr(radvd_node, "Enable", "0");
			cfg_set_object_attr(dhcp6s_node, "Enable", "0");

			cfg_commit_object(radvd_node);
			cfg_commit_object(dhcp6s_node);
		}

	}
	else
	{
		snprintf(buf, sizeof(buf), "%d", obj->idx);		
		snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_ENTRY_NODE, obj->idx+1);
		snprintf(radvd_node, sizeof(radvd_node), RADVD_ENTRY_NODE, obj->idx+1);
		cfg_set_object_attr(dhcp6s_node, "DNSWANConnection", buf);
		cfg_set_object_attr(radvd_node, attrname, buf);
		cfg_set_object_attr(radvd_node, "DelegatedWanConnection", buf);

		if (cfg_get_object_attr(obj->path,"PDEnable",buf,sizeof(buf)) > 0){
			if (strcasecmp(buf,"Yes") == 0)
			{
				if( SVC_WAN_SRV_OTHER(cfg) ){
					cfg_set_object_attr(radvd_node, "Enable", "1");
					cfg_set_object_attr(dhcp6s_node, "Enable", "1");
				}
			}	
		}
	}

	setVRNPTRule(action, obj);
	return 0;
}

int check_default_wan(wan_entry_obj_t* obj)
{
	wan_entry_cfg_t* cfg = NULL;
	char defrouteidx[8] = {0};
	
	cfg = &obj->cfg;

	if( cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", defrouteidx, sizeof(defrouteidx)) <= 0 
		|| 0 == defrouteidx[0] )
	{
		return 0;
	}
	
	if( obj->idx == atoi(defrouteidx) && SVC_WAN_SRV_INTERNET(cfg) )
		return 1;
	else
		return 0;
}

int get_vr_Entry_index(char *servicelist)
{
	int i = 0;
	char type[16] = {0};
	char nodeName[64] = {0};
	for(i=0; i<CFG_TYPE_MAX_RADVD_NUM; i++){
		snprintf(nodeName, sizeof(nodeName), RADVD_ENTRY_NODE, i+1);
		if( cfg_get_object_attr(nodeName, "Type", type, sizeof(type)) > 0 && strstr(type, servicelist) )
		{
			return i;
		}
	}
	return -1;
}


int check_vr_wan(wan_entry_obj_t* obj)
{
	wan_entry_cfg_t* cfg = NULL;
	
	cfg = &obj->cfg;

	if( SVC_WAN_SRV_VR(cfg) || (SVC_WAN_SRV_OTHER(cfg) && SVC_WAN_IS_ROUTE(cfg)))
		return 1;
	else
		return 0;
}

int set_radvd_dhcp6s_info(wan_entry_obj_t* obj, int action)
{
	char waninfoNode[64];
	char tmpNode[64];
	int mask = 0;
	int skb_mark = 0;
	int IFIdx = 0;
	char buf[64] = {0};
	unsigned int bindbit = 0;

	if( check_vr_wan(obj) )
	{
		if( action )
		{
			set_ipv6_wan_index(obj, 1);
		}
		else
		{
			set_ipv6_wan_index(obj, 0);
		}
		
	}
}
#endif

#if defined(TCSUPPORT_NPTv6)
int setNPTRule(unsigned int nptwan_mask_pre, unsigned int nptwan_mask,int wan_index, int action)
{
	char waninfo_node[64] = {0},PD6[128] = {0}, wan_node[64] = {0};
	char cmd[256] = {0}, ifname[16] = {0}, tmpbuf[16] = {0};
	char defrouteidx[8] = {0}, defPD6[128] = {0};
	int pvc_index = 0, entry_index = 0, default_lost = 0;
	int idx_max = SVC_MAX_WAN_INTF_NUMBER - 1, idx_min = 0, i = 0;

	if(wan_index == -1) /*default route wan*/
	{
		if(nptwan_mask == 0)
			return 0; /*no wan enable NPT*/
	}
	if(wan_index >=0)
	{
		if((nptwan_mask&(1<<wan_index)) == 0 && (nptwan_mask_pre&(1<<wan_index)) == 0)
			return 0; /*this index wan have no change*/
		idx_max = idx_min = wan_index;
	}

	if( cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", defrouteidx, sizeof(defrouteidx)) > 0 )
	{
		memset(waninfo_node, 0, sizeof(waninfo_node));
		snprintf(waninfo_node, sizeof(waninfo_node), WANINFO_ENTRY_NODE, atoi(defrouteidx) + 1);
		if(cfg_get_object_attr(waninfo_node, "PD6", defPD6, sizeof(defPD6)) <= 0)
			default_lost= 1;
	}
	else
		default_lost= 1;

	pvc_index	= (atoi(defrouteidx) / SVC_WAN_MAX_ENTRY_NUM);
	entry_index = (atoi(defrouteidx) % SVC_WAN_MAX_ENTRY_NUM);

	snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);
	if(cfg_get_object_attr(wan_node, "Active", tmpbuf, sizeof(tmpbuf)) <= 0 || 0 == strcmp(tmpbuf, "No"))
		default_lost= 1;

	for( i = idx_min; i <= idx_max; i++ )
	{
		memset(PD6, 0, sizeof(PD6));
		memset(ifname, 0, sizeof(ifname));

		if( (0 == (nptwan_mask & (1 << i))) && (0 == (nptwan_mask_pre & (1 << i)))) /*this wan not enable NPT*/
		{
			continue;
		}

		if( (0 == (nptwan_mask & (1 << i))) || (0 == action) || (1 == default_lost) )
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -D POSTROUTING -j NPT_POST_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -D PREROUTING -j NPT_PRE_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -F NPT_POST_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -F NPT_PRE_CHAIN%d -w", i);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -X NPT_POST_CHAIN%d -w", i);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -X NPT_PRE_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -D FORWARD -j NPT_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -F NPT_CHAIN%d -w", i);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -X NPT_CHAIN%d -w", i);
			system(cmd);
			
			blapi_traffic_del_nptv6_prefix_all();
		}
		else
		{
			pvc_index	= (i / SVC_WAN_MAX_ENTRY_NUM);
			entry_index = (i % SVC_WAN_MAX_ENTRY_NUM);

			snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);

			snprintf(waninfo_node, sizeof(waninfo_node), WANINFO_ENTRY_NODE, i + 1);

			cfg_get_object_attr(waninfo_node, "PD6", PD6, sizeof(PD6));
			cfg_get_object_attr(wan_node, "IFName", ifname, sizeof(ifname));

			if( 0 == PD6[0] || 0 == ifname[0] )
			{
				continue;
			}

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -N NPT_POST_CHAIN%d -w", i);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -N NPT_PRE_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -I POSTROUTING -j NPT_POST_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -I PREROUTING -j NPT_PRE_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -N NPT_CHAIN%d -w", i);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -I FORWARD -j NPT_CHAIN%d -w", i);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -I NPT_CHAIN%d -i %s -d %s -j ACCEPT -w", i, ifname, defPD6);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -I NPT_CHAIN%d -i br0 -o %s -p tcp -j ACCEPT -w", i, ifname);
			system(cmd);
			
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -I NPT_POST_CHAIN%d -s %s -o %s -j SNPT --src-pfx %s --dst-pfx %s -w",
										i, defPD6, ifname, defPD6, PD6);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t mangle -I NPT_PRE_CHAIN%d -i %s -d %s -j DNPT --src-pfx %s --dst-pfx %s -w", 
										i, ifname, PD6, PD6, defPD6);
			system(cmd);
			
			blapi_traffic_add_nptv6_prefix(ifname,PD6,defPD6);
		}
	}
	
	return 0;
}

unsigned int update_npt_index(int action, unsigned int nptwanmask, wan_entry_obj_t* obj)
{
	char nptwanmask_s[32] = {0};
	wan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
		
	if( 1 == action && SVC_WAN_NPT_ENABLE(cfg))
		nptwanmask |= 1 << obj->idx;
	else
		nptwanmask &= ~(1 << obj->idx);

 	snprintf(nptwanmask_s, sizeof(nptwanmask_s), "%x", nptwanmask);
	cfg_set_object_attr(RADVD_ENTRY_NODE, "npt_wan_idx", nptwanmask_s);
	
	return nptwanmask;
}

int get_wan_type(wan_entry_obj_t* obj)
{
	wan_entry_cfg_t* cfg = NULL;
	char defrouteidx[8] = {0};
	char buf[8] = {0};
	
	cfg = &obj->cfg;

	cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", defrouteidx, sizeof(defrouteidx));
	
	if( obj->idx == atoi(defrouteidx) && SVC_WAN_SRV_INTERNET(cfg) )
	{
		return 1;
	}
	else if( SVC_WAN_SRV_OTHER(cfg) && SVC_WAN_IS_ROUTE(cfg) )
	{
		return 2;
	}
	else
		return 0;
}

int update_npt_related_wan_rule(wan_entry_obj_t* obj, int action)
{
	static unsigned int nptwanmask_pre = 0;
	unsigned int nptwanmask = 0;
	int wan_type = get_wan_type(obj);
	int ret = 0;

	if(wan_type == 1)
	{
		nptwanmask = nptwanmask_pre;
		ret = setNPTRule(nptwanmask_pre, nptwanmask, -1, action);
		return ret;
	}
	else if(wan_type == 2)
	{
		nptwanmask = update_npt_index(action, nptwanmask_pre, obj);
		ret = setNPTRule(nptwanmask_pre, nptwanmask, obj->idx, action);
		nptwanmask_pre = nptwanmask;
		return ret;
	}
	else
		return 0;

}

#endif

void update_pd_info(wan_entry_obj_t* obj, int evt)
{
	int dhcp6s_idx = 0, radvd_idx = 0;
	char tmp[64] = {0};
	char radvd_node[32] = {0}, dhcp6s_node[32] = {0};
    
    if(NULL == obj)
    {
        return ;
    }

    switch (evt)
    {
        case EVT_WAN_CONN_GETV6:
        {
#if defined(TCSUPPORT_CT_VRWAN)
			if( check_default_wan(obj) )
			{
				setVRNPTRule(1, obj);
			}
			set_radvd_dhcp6s_info(obj, 1);
#endif			
#if defined(TCSUPPORT_NPTv6)
			update_npt_related_wan_rule(obj, 1);
#endif
            if(check_pd_enable(obj->idx) ||  obj->idx == get_dhcp6s_dns_wan_interface(obj))
            {
#if 0/*defined(TCSUPPORT_CT_MULTI_LAN_PD)*/
                if(check_default_wan(obj)){
                    snprintf(dhcp6s_node, sizeof(dhcp6s_node), DHCP6S_COMMON_NODE);
                    if(cfg_get_object_attr(dhcp6s_node, "DNSWANConnection", tmp, sizeof(tmp)) >= 0){
                        if('\0' ==  tmp[0]){
                            snprintf(tmp,sizeof(tmp),"%d",obj->idx);
                            cfg_set_object_attr(dhcp6s_node, "DNSWANConnection", tmp);
                        }
            }
                }
#endif
                restart_pd_function(0, obj);
            }
            if(obj->idx == get_ra_wan_pd_interface(obj))
            {
#if 0/*defined(TCSUPPORT_CT_MULTI_LAN_PD)*/
                if(check_default_wan(obj)){
                    snprintf(radvd_node, sizeof(radvd_node), RADVD_COMMON_NODE);
                    if(cfg_get_object_attr(radvd_node, "DelegatedWanConnection", tmp, sizeof(tmp)) >= 0){
                        if('\0' ==  tmp[0]){
                            snprintf(tmp,sizeof(tmp),"%d",obj->idx);
                            cfg_set_object_attr(radvd_node, "DelegatedWanConnection", tmp);
                        }
                    }
                }
#endif	
                restart_pd_function(1, obj);
            }
            
            update_br0_pdaddress(obj->idx, EVT_WAN_CONN_GETV6);
#if defined(TCSUPPORT_CT_JOYME4)
            lan_icmpv6_update(obj,EVT_WAN_CONN_GETV6);
#endif
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
			dhcp6s_idx = get_dhcp6s_dns_wan_interface(obj);
			radvd_idx = get_ra_wan_pd_interface(obj);
#if defined(TCSUPPORT_CT_VRWAN)
			set_radvd_dhcp6s_info(obj, 0);
#endif
            if(obj->idx == get_run_pd_index())
            {
                update_run_pd_index(obj->idx);
                restart_pd_function(0, obj);
            }
            else if(obj->idx == dhcp6s_idx)
            {
                restart_pd_function(0, obj);
            }
            if(obj->idx == radvd_idx)
            {
                restart_pd_function(1, obj);
            }
            update_br0_pdaddress(obj->idx, EVT_WAN_CONN_LOSTV6);
#if defined(TCSUPPORT_CT_JOYME4)
            lan_icmpv6_update(obj,EVT_WAN_CONN_LOSTV6);
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



