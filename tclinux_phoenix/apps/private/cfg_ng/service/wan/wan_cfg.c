

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
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_cfg.h"
#include "port_binding.h"
#include "utility.h"
#include "wan_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**********************************************************************/
/**********************************************************************/
static int svc_wan_check_ipv4_static(wan_entry_cfg_t* cfg)
{
    return  1;
}

static int svc_wan_check_ipv6_static(wan_entry_cfg_t* cfg)
{
    return 1;
}

static int svc_wan_check_ipv6_pd(wan_entry_cfg_t* cfg)
{
    return 1;
}

void svc_wan_set_intf_name(wan_entry_obj_t* obj)
{
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("obj can't be NULL.\n");
        return ;
    }
    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj, itf_name, sizeof(itf_name));	

    cfg_set_object_attr(obj->path, "NASName", obj->dev);
    cfg_set_object_attr(obj->path, "IFName", itf_name);

    return ;
}
int svc_wan_cfg_get_system_protocol_version(void)
{
    char node[SVC_WAN_BUF_32_LEN];
    char tmp[SVC_WAN_BUF_16_LEN];
    int  protocol_version = E_SYSTEM_IPV4_IPV6_ENABLE;

    memset(node,0, sizeof(node));
    snprintf(node, sizeof(node), "%s", SYS_ENTRY_NODE);
    memset(tmp, 0, sizeof(tmp));
    if(cfg_get_object_attr(node, "IPProtocolVersion", tmp, sizeof(tmp)) > 0)
    {
        protocol_version = atoi(tmp);
    }

    SVC_WAN_DEBUG_INFO("protocol_version = %d.\n", protocol_version);

    return protocol_version;
}


int svc_wan_check_cfg(wan_entry_cfg_t* cfg)
{
    if (cfg->active != SVC_WAN_ATTR_ACTIVE_ENABLE)
    {
        return 0;
    }

    if (cfg->wanmode != SVC_WAN_ATTR_WANMODE_ROUTE && cfg->wanmode !=  SVC_WAN_ATTR_WANMODE_BRIDGE)
    {
        return 0;
    }

    if (cfg->linkmode != SVC_WAN_ATTR_LINKMODE_IP && cfg->linkmode != SVC_WAN_ATTR_LINKMODE_PPP)
    {
        return 0;
    }

    if (cfg->vtag != SVC_WAN_ATTR_VTAG_UNTAG && cfg->vtag != SVC_WAN_ATTR_VTAG_TRANS \
            && cfg->vtag != SVC_WAN_ATTR_VTAG_TAG && cfg->vtag != SVC_WAN_ATTR_DOT1P_TAG_TRANS)
    {
        return 0;
    }

    if (cfg->vtag == SVC_WAN_ATTR_VTAG_TAG && cfg->vid >= 4096)
    {
        return 0;
    }

    if (cfg->version != SVC_WAN_ATTR_VERSION_IPV4 && cfg->version != SVC_WAN_ATTR_VERSION_IPV6 \
            && cfg->version != SVC_WAN_ATTR_VERSION_BOTH)
    {
        return 0;
    }

    if (SVC_WAN_IS_BRIDGE(cfg))
    {
        return 1;
    }

    if (SVC_WAN_IS_PPP(cfg))
    {
#if defined(TCSUPPORT_CT_PPP_MANUALLY)
		if ( SVC_WAN_IS_CONN_MANUALLY(cfg) && SVC_WAN_IS_PPP_MANUAL_DISCONN(cfg) )
		{
			return 0;
		}
#endif
        return 1;
    }
    
    if (SVC_WAN_IS_IPV4(cfg))
    {
        if (cfg->isp != SVC_WAN_ATTR_ISP_DHCP && cfg->isp != SVC_WAN_ATTR_ISP_STATIC)
        {
            return 0;
        }

        if (SVC_WAN_IS_IPV4_STATIC(cfg))
        {
            if (svc_wan_check_ipv4_static(cfg) <=0)
            {
                return 0;
            }
        }
    }

    if (SVC_WAN_IS_IPV6(cfg))
    {
        if (cfg->ipv6mode != SVC_WAN_ATTR_IPV6MODE_DHCP && cfg->ipv6mode != SVC_WAN_ATTR_IPV6MODE_SLAAC \
                && cfg->ipv6mode != SVC_WAN_ATTR_IPV6MODE_STATIC)
        {
            return 0;
        }

        if (SVC_WAN_IS_IPV6_STATIC(cfg))
        {
            if(svc_wan_check_ipv6_static(cfg) <=0)
            {
                return 0;
            }
        }

        if (cfg->dhcpv6pd == ENABLE && cfg->pdmode == SVC_WAN_ATTR_PDMODE_MANUAL)
        {
            if (svc_wan_check_ipv6_pd(cfg) <= 0)
            {
                return 0;
            }
        }
    }

    return 1;
}

int svc_wan_is_cfg_equal(wan_entry_cfg_t* dst, wan_entry_cfg_t* src)
{
    return memcmp(src,dst,sizeof(wan_entry_cfg_t)) == 0;
}

void dump_svc_wan_cfg(wan_entry_cfg_t* cfg, char is_bridge, char is_msg)
{
    if(is_msg)
    {
        goto log_print;
    }
    else if(g_svc_wan_level & E_DBG_INFO_LEVEL)
    {
        goto log_print;
    }
    else
    {
        return ;
    }


log_print:
    svc_wan_printf("cfg->active = %d\n",   cfg->active);
    svc_wan_printf("cfg->wanmode = %d\n",  cfg->wanmode);
    svc_wan_printf("cfg->linkmode = %d\n", cfg->linkmode);
    svc_wan_printf("cfg->vid = %d\n",      cfg->vid);
    svc_wan_printf("cfg->dot1p = %d\n",    cfg->dot1p);
    svc_wan_printf("cfg->vtag = %d\n",     cfg->vtag);
    svc_wan_printf("cfg->version = %d\n",  cfg->version);
    svc_wan_printf("cfg->isp = %d\n",      cfg->isp);
    svc_wan_printf("cfg->srv_type = %d\n", cfg->srv_type);
    svc_wan_printf("cfg->srv_list = %s\n", cfg->srv_list);
    svc_wan_printf("cfg->bindbit = %d\n",  cfg->bindbit);
    svc_wan_printf("cfg->pingresp = %d\n", cfg->pingresp);

    if(is_bridge)
    {
        return ;
    }

    if(SVC_WAN_IS_PPP(cfg))
    {
        svc_wan_printf("cfg->user = %s\n", cfg->user);
        svc_wan_printf("cfg->pwd = %s\n", cfg->pwd);
        svc_wan_printf("cfg->ppp_hybrid = %d\n", cfg->ppp_hybrid);
    }

    if (SVC_WAN_IS_IPV4_STATIC(cfg))
    {
        svc_wan_printf("cfg->addrv4 = %s\n", cfg->addrv4);
        svc_wan_printf("cfg->maskv4 = %s\n", cfg->maskv4);
        svc_wan_printf("cfg->gwv4 = %s\n", cfg->gwv4);
        svc_wan_printf("cfg->dns1v4 = %s\n", cfg->dns1v4);
        svc_wan_printf("cfg->dns2v4 = %s\n", cfg->dns2v4);
    }

    if (SVC_WAN_IS_IPV6(cfg))
    {
        svc_wan_printf("cfg->ipv6mode = %d\n", cfg->ipv6mode);
        svc_wan_printf("cfg->dhcpv6pd = %d\n", cfg->dhcpv6pd);
        svc_wan_printf("cfg->pd = %d\n", cfg->pd);

        if (SVC_WAN_IPV6_PD_ENABLE(cfg))
        {
            svc_wan_printf("cfg->pdmode = %d\n", cfg->pdmode);
        }

        if (SVC_WAN_IPV6_PD_MANUAL(cfg))
        {
            svc_wan_printf("cfg->pdaddr = %s\n", cfg->pdaddr);
            svc_wan_printf("cfg->pdtm1 = %d\n", cfg->pdtm1);
            svc_wan_printf("cfg->pdtm2 = %d\n", cfg->pdtm2);
        }

        if (SVC_WAN_IS_IPV6_STATIC(cfg))
        {
            svc_wan_printf("cfg->addrv6 = %s\n", cfg->addrv6);
            svc_wan_printf("cfg->gwv6 = %s\n", cfg->gwv6);
            svc_wan_printf("cfg->dns1v6 = %s\n", cfg->dns1v6);
            svc_wan_printf("cfg->dns2v6 = %s\n", cfg->dns2v6);
            svc_wan_printf("cfg->prefix6 = %d\n", cfg->prefix6);
        }

        svc_wan_printf("cfg->dslite = %d\n", cfg->dslite);
        svc_wan_printf("cfg->gwv6_op = %d\n", cfg->gwv6_op);
        svc_wan_printf("cfg->pdorigin = %s\n", cfg->pdorigin);
    }

    return ;
}

int svc_wan_load_cfg(char* path, wan_entry_cfg_t* cfg, wan_entry_obj_t* obj)
{
    char tmp[64] = {0};

    memset(cfg,0,sizeof(wan_entry_cfg_t));

    cfg_get_object_attr(path,SVC_WAN_ATTR_ACTIVE,tmp,sizeof(tmp));
    if(strcmp(tmp,"Yes") == 0)
    {
        cfg->active = SVC_WAN_ATTR_ACTIVE_ENABLE;
    }

    cfg_get_object_attr(path,SVC_WAN_ATTR_WANMODE,tmp,sizeof(tmp));
    if(strcmp(tmp,"Route") == 0)
    {
        cfg->wanmode = SVC_WAN_ATTR_WANMODE_ROUTE;
    }
    else if(strcmp(tmp,"Bridge") == 0)
    {
        cfg->wanmode = SVC_WAN_ATTR_WANMODE_BRIDGE;
    }

    cfg_get_object_attr(path,SVC_WAN_ATTR_LINKMODE,tmp,sizeof(tmp));
    if(strcmp(tmp,"linkIP") == 0)
    {
        cfg->linkmode = SVC_WAN_ATTR_LINKMODE_IP;
    }
    else if(strcmp(tmp,"linkPPP") == 0)
    {
        cfg->linkmode = SVC_WAN_ATTR_LINKMODE_PPP;
    }

    if (cfg_get_object_attr(path,SVC_WAN_ATTR_VID,tmp,sizeof(tmp)) > 0)
    {
        cfg->vid = atoi(tmp);
    }
    else
    {
        cfg->vid = -1;
    }

    if (cfg_get_object_attr(path,SVC_WAN_ATTR_8021P,tmp,sizeof(tmp)) > 0)
    {
        cfg->dot1p = atoi(tmp);
    }
    else
    {
        cfg->dot1p = 0;
    }
    
    cfg_get_object_attr(path,SVC_WAN_ATTR_VTAG,tmp,sizeof(tmp));
    if(strcmp(tmp,"UNTAG") == 0)
    {
        cfg->vtag = 1;
    }
    else if (strcmp(tmp,"TRANSPARENT") == 0)
    {
        cfg->vtag = 2;
    }
    else if (strcmp(tmp,"TAG") == 0)
    {
        cfg->vtag = 3;
		memset(tmp,0,sizeof(tmp));
		cfg_get_object_attr(path,SVC_WAN_ATTR_V8021P_ENABLE,tmp,sizeof(tmp));
		if(strcmp(tmp,"No") == 0)
		{
			cfg->vtag = 4;
		}
    }

    cfg_get_object_attr(path,SVC_WAN_ATTR_VERSION,tmp,sizeof(tmp));
    if(strcmp(tmp,"IPv4") == 0)
    {
        cfg->version = 1;
    }
    else if (strcmp(tmp,"IPv6") == 0)
    {
        cfg->version = 2;
    }
    else if (strcmp(tmp,"IPv4/IPv6") == 0)
    {
        cfg->version = 3;
    }

    if (cfg_get_object_attr(path,SVC_WAN_ATTR_ISP,tmp,sizeof(tmp)) > 0)
    {
        cfg->isp = atoi(tmp);
    }
    else
    {
        cfg->isp = -1;
    }
    
    memset(tmp, 0, sizeof(tmp));
    if (cfg_get_object_attr(path,SVC_WAN_ATTR_SRV_LIST,tmp,sizeof(tmp)) > 0)
    {
        strncpy(cfg->srv_list, tmp, sizeof(cfg->srv_list) - 1);
        if (strstr(tmp,"INTERNET") != NULL)
        {
            cfg->srv_type |= E_SRV_INTENET;
#if defined(TCSUPPORT_CT_SERVICELIST_E8C)
            cfg->srv_type |= E_SRV_E8C_INTENET;
#else
            if (0 == strcmp(tmp, "INTERNET") || 0 == strcmp(tmp, "TR069_INTERNET"))
            {
                cfg->srv_type |= E_SRV_E8C_INTENET;
            }
#endif
        }

        if (strstr(tmp,"OTHER") != NULL)
        {
            cfg->srv_type |= E_SRV_OTHER;
        }
#if defined(TCSUPPORT_CMCCV2)
		if (strstr(tmp,"IPTV") != NULL)
        {
            cfg->srv_type |= E_SRV_IPTV;
        }

		if (strstr(tmp,"OTT") != NULL)
        {
            cfg->srv_type |= E_SRV_OTT;
        }
#endif
        if (strstr(tmp,"TR069") != NULL)
        {
            cfg->srv_type |= E_SRV_TR069;
#if defined(TCSUPPORT_CT_SERVICELIST_E8C)
            cfg->srv_type |= E_SRV_E8C_TR069;
#else
            if (0 == strcmp(tmp, "TR069") || 0 == strcmp(tmp, "TR069_INTERNET"))
            {
                cfg->srv_type |= E_SRV_E8C_TR069;
            }
#endif
        }
        if ( NULL != strstr(tmp, "VOICE") )
        {
            cfg->srv_type |= E_SRV_VOICE;
        }
#if defined(TCSUPPORT_CT_VRWAN)
		if ( NULL != strstr(tmp, "VR") )
        {
            cfg->srv_type |= E_SRV_VR;
        }
#endif
       if ( NULL != strstr(tmp, "SPECIAL_SERVICE_") && NULL == strstr(tmp, "VR") )
        {
            cfg->srv_type |= E_SRV_SPE_SER;
        }
    }

    if (cfg_get_object_attr(path,SVC_WAN_ATTR_MTU,tmp,sizeof(tmp)) > 0)
    {
        cfg->mtu = atoi(tmp);
    }

#if defined(TCSUPPORT_CMCCV2)
	if ( SVC_WAN_IS_PPP(cfg) )
	{
		if ( 0 == cfg->mtu || cfg->mtu > MAX_PPPOE_MTU_SIZE )
			cfg->mtu = MAX_PPPOE_MTU_SIZE; /* hardware support max pppoe mtu size */
	}
	else
	{
		if ( 0 == cfg->mtu || cfg->mtu > MAX_IPOE_MTU_SIZE )
			cfg->mtu = MAX_IPOE_MTU_SIZE; /* hardware support max ipoe mtu size */
	}
#endif

    if (cfg_get_object_attr(path, SVC_WAN_ATTR_MVID,tmp,sizeof(tmp)) > 0)
    {
        cfg->mvid = atoi(tmp);
    }

    cfg->bindbit = get_port_binding_interface_info(path, cfg, obj);

    memset(tmp, 0, sizeof(tmp));
    if(cfg_get_object_attr(path, SVC_WAN_ATTR_PROXY_ENABLE, tmp, sizeof(tmp)) > 0)
    {
        cfg->proxyenable = atoi(tmp);
    }

    memset(tmp, 0, sizeof(tmp));
    if(cfg_get_object_attr(path, SVC_WAN_ATTR_DHCPRELAY, tmp, sizeof(tmp)) > 0)
    {
        if(0 == strcasecmp(tmp, "Yes"))
        {
            cfg->dhcp_realy = SVC_WAN_ATTR_DHCPRELAY_ENABLE;
        }
        else if(0 == strcasecmp(tmp, "No"))
        {
            cfg->dhcp_realy = SVC_WAN_ATTR_DHCPRELAY_DISABLE;
        }
        else
        {
            cfg->dhcp_realy = SVC_WAN_ATTR_DHCPRELAY_NOT_CONFIG;
        }
    }
    else
    {
        cfg->dhcp_realy = SVC_WAN_ATTR_DHCPRELAY_NOT_CONFIG;
    }


    memset(tmp, 0, sizeof(tmp));
    if(cfg_get_object_attr(path, SVC_WAN_ATTR_DHCPENABLE, tmp, sizeof(tmp)) > 0)
    {
        cfg->dhcp_enable = atoi(tmp);
    }
#if defined(TCSUPPORT_CT_SDN)
    memset(tmp, 0, sizeof(tmp));
    if(cfg_get_object_attr(path, SVC_WAN_ATTR_SDN, tmp, sizeof(tmp)) > 0)
    {
    	cfg->sdn = atoi(tmp);
    }
    else
    {
    	cfg->sdn = 0;
    }
#endif
    if (SVC_WAN_IS_BRIDGE(cfg))
    {
        dump_svc_wan_cfg(cfg, TRUE, FALSE);
        return 0;
    }
#if defined(TCSUPPORT_CT_JOYME4)
    cfg_get_object_attr(path,SVC_WAN_ATTR_PING_RESPONSE,tmp,sizeof(tmp));
    if (strcasecmp(tmp,"Yes") == 0)
        cfg->pingresp = ENABLE;
#endif

    if (SVC_WAN_IS_PPP(cfg))
    {
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_PPP_USER,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->user,tmp,sizeof(cfg->user) - 1);
        }
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_PPP_PWD,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->pwd,tmp,sizeof(cfg->pwd) - 1);
        }
		memset(tmp, 0, sizeof(tmp));
		if (cfg_get_object_attr(path,SVC_WAN_ATTR_CONNECTION,tmp,sizeof(tmp)) > 0)
		{
			if ( 0 == strcmp(tmp, "Connect_on_Demand") )
			{
				cfg->conn_mode = SVC_WAN_ATTR_CONNECT_ON_DEMAND;
			}
			else if ( 0 == strcmp(tmp, "Connect_Manually") )
			{
				cfg->conn_mode = SVC_WAN_ATTR_CONNECT_MANUALLY;
				memset(tmp, 0, sizeof(tmp));
				if ( cfg_get_object_attr(path,SVC_WAN_ATTR_PPP_MANUAL_STATUS,tmp,sizeof(tmp)) > 0 
					&& 0 == strcmp(tmp, "connect") )
				{
					cfg->ppp_manual_status = SVC_WAN_ATTR_PPP_MANUAL_STATUS_CONNECT;
				}
				else
				{
					cfg->ppp_manual_status = SVC_WAN_ATTR_PPP_MANUAL_STATUS_DISCONNECT;
				}
			}
			else
			{
				cfg->conn_mode = SVC_WAN_ATTR_CONNECT_KEEP_ALIVE;
			}
		}

        cfg_get_object_attr(path,SVC_WAN_ATTR_PPP_HYBRID,tmp,sizeof(tmp));
        if (strcasecmp(tmp,"Yes") == 0)
        {
            cfg->ppp_hybrid = ENABLE;
        }
    }
    
    if (SVC_WAN_IS_IPV4_STATIC(cfg))
    {
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_ADDRV4,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->addrv4,tmp,sizeof(cfg->addrv4) - 1);
        }
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_MASKV4,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->maskv4,tmp,sizeof(cfg->maskv4) - 1);
        }
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_GWV4,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->gwv4,tmp,sizeof(cfg->gwv4) - 1);
        }
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_DNS1V4,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->dns1v4,tmp,sizeof(cfg->dns1v4) - 1);
        }
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_DNS2V4,tmp,sizeof(tmp)) > 0)
        {
            strncpy(cfg->dns2v4,tmp,sizeof(cfg->dns2v4) - 1);
        }
    }

    if (SVC_WAN_IS_IPV6(cfg))
    {
        memset(tmp, 0, sizeof(tmp));
        cfg_get_object_attr(path, SVC_WAN_ATTR_DSLITE, tmp, sizeof(tmp));
        if(0 == strcasecmp(tmp, "Yes"))
        {
            cfg->dslite = ENABLE;
        }
        else
        {
            cfg->dslite = DISABLE;
        }
        
        if(SVC_WAN_DSLITE_ENABLE(cfg))
        {
            memset(tmp, 0, sizeof(tmp));
            if(cfg_get_object_attr(path, SVC_WAN_ATTR_DSLITE_MODE, tmp, sizeof(tmp)) > 0)
            {
                cfg->dslite_mode = atoi(tmp);
            }
            else
            {
                cfg->dslite_mode = SVC_WAN_ATTR_DSLITE_AUTO_MODE;
            }
        }
        /*web ipv6 get ip function*/
        cfg_get_object_attr(path,SVC_WAN_ATTR_IPV6MODE,tmp,sizeof(tmp));
        if (strcasecmp(tmp,"Yes") == 0)
        {
            cfg->ipv6mode = SVC_WAN_ATTR_IPV6MODE_DHCP;
        }
        else if (strcasecmp(tmp,"No") == 0)
        {
            cfg->ipv6mode = SVC_WAN_ATTR_IPV6MODE_SLAAC;
        }
        else if (strcasecmp(tmp,"N/A") == 0)
        {
            cfg->ipv6mode = SVC_WAN_ATTR_IPV6MODE_STATIC;
        }

        /*web pd  enable*/
        cfg_get_object_attr(path,SVC_WAN_ATTR_PDMODE,tmp,sizeof(tmp));
        if (strcasecmp(tmp,"Yes") == 0)
        {
            cfg->dhcpv6pd = ENABLE;
        }

        /*web hidden route, ipv6, internet or other*/
        cfg_get_object_attr(path,SVC_WAN_ATTR_PD,tmp,sizeof(tmp));
        if (strcasecmp(tmp,"Yes") == 0)
        {
            cfg->pd = ENABLE;
        }

	/*ipv6 npt*/
        cfg_get_object_attr(path,SVC_WAN_ATTR_NPT,tmp,sizeof(tmp));
        if (strcasecmp(tmp,"1") == 0)
        {
            cfg->npt = ENABLE;
        }
        else
        {
            cfg->npt = DISABLE;
        }
		
        /*web pd mode */
        if (SVC_WAN_IPV6_PD_ENABLE(cfg))
        {
            cfg_get_object_attr(path,SVC_WAN_ATTR_PDMODE,tmp,sizeof(tmp));
            if (strcasecmp(tmp,"Yes") == 0)
            {
                cfg->pdmode = SVC_WAN_ATTR_PDMODE_AUTO;
            }
            else if (strcasecmp(tmp,"No") == 0)
            {
                cfg->pdmode = SVC_WAN_ATTR_PDMODE_MANUAL;
            }
            else
            {
                cfg->pdmode = 0;
            }
        }

        /*pd addr  preferred life/ effect life*/
        if (SVC_WAN_IPV6_PD_MANUAL(cfg))
        {
            memset(tmp, 0, sizeof(tmp));
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_PDADDR,tmp,sizeof(tmp)) >0)
            {
                strncpy(cfg->pdaddr,tmp,sizeof(cfg->pdaddr));
            }

            memset(tmp, 0, sizeof(tmp));
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_PDTIME1,tmp,sizeof(tmp)) >0)
            {
                cfg->pdtm1 = atoi(tmp);
            }

            memset(tmp, 0, sizeof(tmp));
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_PDTIME2,tmp,sizeof(tmp)) >0)
            {
                cfg->pdtm2 = atoi(tmp);
            }
        }

        if (SVC_WAN_IS_IPV6_STATIC(cfg))
        {
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_ADDRV6,tmp,sizeof(tmp)) > 0)
            {
                strncpy(cfg->addrv6,tmp,sizeof(cfg->addrv6));
            }
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_GWV6,tmp,sizeof(tmp)) > 0)
            {
                strncpy(cfg->gwv6,tmp,sizeof(cfg->gwv6));
            }
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_DNS1V6,tmp,sizeof(tmp)) > 0)
            {
                strncpy(cfg->dns1v6,tmp,sizeof(cfg->dns1v6));
            }
            if (cfg_get_object_attr(path,SVC_WAN_ATTR_DNS2V6,tmp,sizeof(tmp)) > 0)
            {
                strncpy(cfg->dns2v6,tmp,sizeof(cfg->dns2v6));
            }
        }

        memset(tmp, 0, sizeof(tmp));
        if (cfg_get_object_attr(path,SVC_WAN_ATTR_GWV6_OP, tmp, sizeof(tmp)) > 0)
        {
            if (0 == strcasecmp(tmp,"Yes"))
            {
                cfg->gwv6_op = SVC_WAN_ATTR_GWV6_OP_MANUAL;
            }
        }

        memset(tmp, 0, sizeof(tmp));
        if (cfg_get_object_attr(path, SVC_WAN_ATTR_PREFIX6, tmp, sizeof(tmp)) > 0)
        {
            cfg->prefix6 = atoi(tmp);
        }

        memset(tmp, 0, sizeof(tmp));
        if (cfg_get_object_attr(path, SVC_WAN_ATTR_PD_ORIGIN, tmp, sizeof(tmp)) > 0)
        {
            strncpy(cfg->pdorigin, tmp, sizeof(cfg->pdorigin) - 1);
        }
    }

        dump_svc_wan_cfg(cfg, FALSE, FALSE);
        return 0;
}


int svc_wan_load_opt(wan_entry_obj_t* obj)
{

    char tmp[64] = {0};
    wan_entry_cfg_t* cfg = &obj->cfg;

    memset(tmp, 0, sizeof(tmp));
    if (SVC_WAN_IS_IPV4(cfg) && cfg_get_object_attr(obj->path,SVC_WAN_ATTR_NAT,tmp,sizeof(tmp)) > 0)
    {
        if (strstr(tmp,"Enable") != NULL)
        {
            obj->nat = 1;
        }
        else
        {
            obj->nat = 0;
        }
    }

    memset(tmp, 0, sizeof(tmp));
    if (cfg_get_object_attr(obj->path, SVC_WAN_ATTR_CHILD_PREFIX_BITS, tmp, sizeof(tmp)) > 0)
    {
        strncpy(obj->child_prefix_bits, tmp, sizeof(obj->child_prefix_bits) - 1);
    }

    return 0;
}

int svc_wan_set_ipv4_info(char* node,char* path)
{
    char info_string[2][MAX_INFO_LENGTH];
    char pathtmp[128] = {0};
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG)
    char cmdstr[256];
    char is_firewall_on[2];
#endif
    int num;

    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "ip");

    cfg_set_object_attr(node,"Status","up");

    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"IP", info_string[0]);
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG)
        memset(is_firewall_on, 0, sizeof(is_firewall_on));
        if((cfg_get_object_attr(FIREWALL_ENTRY_NODE, "firewall_enable", is_firewall_on, sizeof(is_firewall_on)) > 0) && (strlen(is_firewall_on) != 0))
        {
            /* When firewall is enabled. */
            if(strcmp(is_firewall_on,"1") == 0)
            {
                /* When get IPv4, set iptables LAND attack prevention and log rule. */
                memset(cmdstr, 0, sizeof(cmdstr));
                snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -A LAND -p tcp -s %s -d %s -m limit --limit 100/s --limit-burst 150 -j RETURN", info_string[0], info_string[0]);
                system(cmdstr);
                memset(cmdstr, 0, sizeof(cmdstr));
                snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -A LAND -p tcp -s %s -d %s -m limit --limit 1/m --limit-burst 1 -j LOG --log-prefix \"Firewall l:\" --log-level 7", info_string[0], info_string[0]);
                system(cmdstr);
                memset(cmdstr, 0, sizeof(cmdstr));
                snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -A LAND -p tcp -s %s -d %s -j DROP", info_string[0], info_string[0]);
                system(cmdstr);
            }
        }
#endif

#if defined(TCSUPPORT_NP_CMCC)		
		change_br0_ip(info_string[0]);
#endif

    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "netmask");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"NetMask", info_string[0]);
    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dns");
    num = svc_wan_get_file_string(pathtmp, info_string, 2);
    if (num > 0)
    {
        cfg_set_object_attr(node,"DNS", info_string[0]);
    }
    if (num > 1)
    {
        cfg_set_object_attr(node,"SecDNS", info_string[1]);
    }
    
    memset(pathtmp, 0, sizeof(pathtmp));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "gateway");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"GateWay", info_string[0]);
    }

    return 0;
}

int svc_wan_set_ipv6_info(char* node,char* path, wan_entry_obj_t* obj)
{
    char info_string[2][MAX_INFO_LENGTH];
    char pathtmp[128];
    char mac[20];
    int num;
    wan_entry_cfg_t* cfg = &obj->cfg;

    cfg_set_object_attr(node,"Status6","up");

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "ip6");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"IP6", info_string[0]);
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "prefix6");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"PrefixLen6", info_string[0]);
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp),"%s/%s", path, "pd6");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"PD6", info_string[0]);
#if defined(TCSUPPORT_CT_JOYME4)
	strncpy(cfg->pd6addr,info_string[0],sizeof(cfg->pd6addr)-1);
#endif
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "orgpd6");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"OrgPD6", info_string[0]);
#if defined(TCSUPPORT_CT_JOYME4)
	strncpy(cfg->orgpd6addr,info_string[0],sizeof(cfg->orgpd6addr)-1);
#endif
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "pd6_ptime");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"PD6ptime", info_string[0]);
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "pd6_vtime");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"PD6vtime", info_string[0]);
    }

    if (SVC_WAN_IPV6_PD_MANUAL(cfg) &&  (0 == strcmp("Static", cfg->pdorigin)))
    {
        cfg_set_object_attr(node, "PD6", cfg->pdaddr);
	cfg_set_object_attr(node, "OrgPD6", cfg->pdaddr);
#if defined(TCSUPPORT_CT_JOYME4)
		strncpy(cfg->pd6addr,cfg->pdaddr,sizeof(cfg->pd6addr)-1);
	strncpy(cfg->orgpd6addr,cfg->pdaddr,sizeof(cfg->orgpd6addr)-1);
#endif
        memset(info_string, 0, sizeof(info_string));
        snprintf(info_string[0], sizeof(info_string[0]), "%d", cfg->pdtm1);
        cfg_set_object_attr(node, "PD6ptime", info_string[0]);

        memset(info_string, 0, sizeof(info_string));
        snprintf(info_string[0], sizeof(info_string[0]), "%d", cfg->pdtm2);
        cfg_set_object_attr(node,"PD6vtime", info_string[0]);
    }

#if defined(TCSUPPORT_CT_DSLITE)
    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dsliteaddr");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"DsliteAddr", info_string[0]);
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dslitename");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"DsliteName", info_string[0]); 
    }
#endif

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dns6");
    num = svc_wan_get_file_string(pathtmp, info_string, 2);
    if (num > 0)
    {
        cfg_set_object_attr(node,"DNS6", info_string[0]);
    }
    if (num > 1)
    {
        cfg_set_object_attr(node,"SecDNS6", info_string[1]);
    }

    memset(pathtmp, 0, sizeof(pathtmp));
    memset(info_string, 0, sizeof(info_string));
    snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "gateway6");
    if(svc_wan_get_file_string(pathtmp, info_string, 1) > 0)
    {
        cfg_set_object_attr(node,"GateWay6", info_string[0]);
    }

    memset(mac, 0, sizeof(mac));
    cfg_set_object_attr(node, "hwaddr", getWanInfo(Mac_type, obj->dev, mac));

    return 0;
}

int svc_wan_clear_ipv6_info(char* node,char* path, wan_entry_obj_t* obj)
{
    char mac[20];
    cfg_set_object_attr(node,"Status6","down");
    cfg_set_object_attr(node,"IP6", "");
    cfg_set_object_attr(node,"PrefixLen6","");
    cfg_set_object_attr(node,"PD6", "");	
    cfg_set_object_attr(node,"OrgPD6","");
    cfg_set_object_attr(node,"PD6ptime", "");	
    cfg_set_object_attr(node,"PD6vtime","");
#if defined(TCSUPPORT_CT_DSLITE)
    cfg_set_object_attr(node,"DsliteAddr", "");
    cfg_set_object_attr(node,"DsliteName", ""); 	
#endif
    cfg_set_object_attr(node,"DNS6", "");
    cfg_set_object_attr(node,"SecDNS6", "");
    cfg_set_object_attr(node,"GateWay6","");
#if defined(TCSUPPORT_NP_CMCC)
    cfg_set_object_attr(node,"LinkLocalAddr","");
#endif
    memset(mac, 0, sizeof(mac));
    cfg_set_object_attr(node, "hwaddr", getWanInfo(Mac_type, obj->dev, mac));

    return 0;
}

int svc_wan_set_info(wan_entry_obj_t* obj,int stat)
{
    char fpath[32];
    char node[32];
    char mac[20];
    wan_entry_cfg_t* cfg = &obj->cfg;
    char* empty = "";
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG)
    char w_ip_v4[16];
    char cmdstr[256];
    char is_firewall_on[2];
#endif

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);

    if (cfg_query_object(node,NULL,NULL) <= 0)
    {
        cfg_create_object(node);
    }

    memset(fpath, 0, sizeof(fpath));
    if (SVC_WAN_IS_PPP(cfg))
    {
        snprintf(fpath, sizeof(fpath), "/var/run/ppp%d",obj->idx);
    }
    else
    {
        snprintf(fpath, sizeof(fpath), "/var/run/%s",obj->dev);
    }
    memset(mac, 0, sizeof(mac));
    switch(stat)
    {
        case EVT_WAN_CONN_GETV4:
        {
            svc_wan_set_ipv4_info(node,fpath);
            cfg_set_object_attr(node, "hwaddr", getWanInfo(Mac_type, obj->dev, mac));
            break;
        }
        case EVT_WAN_CONN_LOSTV4:
        {
            cfg_set_object_attr(node,"Status","down");
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG)
            memset(is_firewall_on, 0, sizeof(is_firewall_on));
            if((cfg_get_object_attr(FIREWALL_ENTRY_NODE, "firewall_enable", is_firewall_on, sizeof(is_firewall_on)) > 0) && (strlen(is_firewall_on) != 0))
            {
                /* When firewall is enabled. */
                if(strcmp(is_firewall_on,"1") == 0)
                {
                    memset(w_ip_v4, 0, sizeof(w_ip_v4));
                    /* When lost IPv4, delete iptables LAND attack prevention and log rule. */
                    if ((cfg_get_object_attr(node, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0) && (strlen(w_ip_v4) != 0))
                    {
                        memset(cmdstr, 0, sizeof(cmdstr));
                        snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -D LAND -p tcp -s %s -d %s -m limit --limit 100/s --limit-burst 150 -j RETURN", w_ip_v4, w_ip_v4);
                        system(cmdstr);
                        memset(cmdstr, 0, sizeof(cmdstr));
                        snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -D LAND -p tcp -s %s -d %s -m limit --limit 1/m --limit-burst 1 -j LOG --log-prefix \"Firewall l:\" --log-level 7", w_ip_v4, w_ip_v4);
                        system(cmdstr);
                        memset(cmdstr, 0, sizeof(cmdstr));
                        snprintf(cmdstr, sizeof(cmdstr), "iptables -t mangle -D LAND -p tcp -s %s -d %s -j DROP", w_ip_v4, w_ip_v4);
                        system(cmdstr);
                    }
                }
            }
#endif
            cfg_set_object_attr(node,"IP",empty);
            cfg_set_object_attr(node,"NetMask",empty);
            cfg_set_object_attr(node,"DNS",empty);
            cfg_set_object_attr(node,"SecDNS",empty);
            cfg_set_object_attr(node,"GateWay",empty);
            cfg_set_object_attr(node, "hwaddr", getWanInfo(Mac_type, obj->dev, mac));
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            svc_wan_set_ipv6_info(node, fpath, obj);
            break;
        }
        case EVT_WAN_CONN_LOSTV6:
        {
            svc_wan_clear_ipv6_info(node, fpath, obj);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

int is_link_up(void)
{
	int fd;
	char string[64] = {0};
	char EthernetState[8] = {0};
	int count = 0;

#if defined(TCSUPPORT_CT_DSL_EX)	 
	fd = open("/proc/tc3162/adsl_stats",O_RDONLY|O_NONBLOCK);
	if( fd < 0 )
	{
		return 0;
	}

	if((count = read(fd, string, sizeof(string) - 1)) <= 0 )
	{
		close(fd);
		return 0;
	}
	else
	{
		string[sizeof(string) - 1] = '\0';
		if(strstr(string, "up"))
		{
			/*xdsl link up*/
			close(fd);
			return 1;
		}
	}
	close(fd);
#else
	if(((cfg_get_object_attr(XPON_COMMON_NODE, "trafficStatus", string, sizeof(string)) >= 0)
		&& (strcmp(string, "up") == 0))
		|| ((cfg_get_object_attr(WANINFO_COMMON_NODE, "EthernetState", EthernetState, sizeof(EthernetState)) >= 0)
		&& (strcmp(EthernetState, "up") == 0)))
	{
		/*pon traffic up*/
		return 1;
	}
#endif

	return 0;

}

void svc_wan_cfg_update_attr(char* path, wan_entry_cfg_t* cfg, char active)
{
#if defined(TCSUPPORT_CT_PPP_MANUALLY)
	char ppp_status[32] = {0};
#endif
#if defined(TCSUPPORT_CMCCV2)
	char connError[64] = {0};
#endif
	
    if((NULL == path) || (NULL == cfg))
    {
        SVC_WAN_ERROR_INFO("pointer can't be NULL.\n");
        return ;
    }

    if(active)
    {
#if defined(TCSUPPORT_CT_PPPCONN_ERROR)
#if defined(TCSUPPORT_CT_PON_GD)
        if(SVC_WAN_IS_BRIDGE(cfg))
        {
            cfg_set_object_attr(path, "ConnectionError", "ERROR_NONE");
        }
#else
#if defined(TCSUPPORT_CMCCV2)
		cfg_get_object_attr(path, "ConnectionError", connError, sizeof(connError));
		cfg_set_object_attr(path, "LastConnectionError", connError);
#endif
	if(SVC_WAN_IS_BRIDGE(cfg))
        {
            cfg_set_object_attr(path, "ConnectionError", "ERROR_NONE");
        }
		else
		{
			if( SVC_WAN_IS_IP(cfg) )
			{
				cfg_set_object_attr(path, "ConnectionError", "ERROR_NONE");
			}
			else
			{
				if(1 == is_link_up())
				{
			    	cfg_set_object_attr(path,"ConnectionError", "ERROR_NO_ANSWER");
				}
				else
				{
			    	cfg_set_object_attr(path,"ConnectionError", "ERROR_NO_CARRIER");
				}
			}
		}
#if defined(TCSUPPORT_CT_PPP_MANUALLY)
		if ( cfg_get_object_attr(path, SVC_WAN_ATTR_PPP_MANUAL_STATUS, ppp_status, sizeof(ppp_status)) > 0 
			&& 0 == strcmp(ppp_status, "disconnect") )
			cfg_set_object_attr(path,"ConnectionError", "ERROR_USER_DISCONNECT");
#endif
#endif
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
        cfg_set_object_attr(path,"ConnectionErrorCode", "");
#endif
#endif
    }
    else
    {
        cfg_set_object_attr(path, "ConnectionError", "ERROR_NOT_ENABLED_FOR_INTERNET");
    }
    
    return ;
}

char is_commit_from_tr069(void)
{
    char value[8];
    int flag = 0;

    memset(value, 0, sizeof(value));
    if(cfg_get_object_attr(WEBCUSTOM_ENTRY_NODE, "tr69CommitWan", value, sizeof(value)) < 0)
    {
        return FALSE;
    }

    flag = atoi(value);
    if(!flag)
    {
        return FALSE;
    }

    cfg_set_object_attr(WEBCUSTOM_ENTRY_NODE, "tr69CommitWan", "0");

    return TRUE;
}





