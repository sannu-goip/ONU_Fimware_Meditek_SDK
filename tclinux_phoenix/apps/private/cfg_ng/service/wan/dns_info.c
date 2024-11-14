#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "wan_link_list.h"
#include "full_route.h"
#include "dns_info.h"
#include "utility.h"

void restart_dnsmasq(void)
{
    FILE* fp = NULL;
    FILE* fp_info = NULL;
    char cmd[SVC_WAN_BUF_256_LEN];
    char buf[SVC_WAN_BUF_256_LEN];
    char node[SVC_WAN_BUF_32_LEN];

#if defined(TCSUPPORT_CT_FULL_ROUTE)
    unsigned int default_pvc = 0;
    unsigned short tempV4 = 0;
    unsigned short tempV6 = 0;
    int flag = 0;
    int fullroutesw = 0;
    char ip_forward_mode[SVC_WAN_BUF_8_LEN];
    int i = 0 ;
#endif

    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char dns[SVC_WAN_BUF_32_LEN];
    char secdns[SVC_WAN_BUF_32_LEN];
    char if_name[MAX_WAN_DEV_NAME_LEN];
    char dns6[SVC_WAN_BUF_64_LEN];
    char secdns6[SVC_WAN_BUF_64_LEN];
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
    char query_delay[SVC_WAN_BUF_64_LEN];
#endif
#if defined(TCSUPPORT_CT_L2TP_VPN) || defined(TCSUPPORT_CT_VPN_PPTP)
	FILE* vpn_fp_info = NULL;
	char vpn_dns_path[128] = {0};
	char dns_buf[MAXSIZE] = {0};
	FILE* vpn_fp = NULL;
	int j = 0;
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	char ppphisaddr[64] = {0};
#endif

    fp = fopen("/etc/dnsmasq.conf", "w");
    if(fp == NULL)
    {
        SVC_WAN_ERROR_INFO("open dnsmasq.conf failed");
        return;
    }

    fp_info = fopen("/etc/dnsInfo.conf", "w");
    if(fp_info == NULL)
    {
        fclose(fp);
        SVC_WAN_ERROR_INFO("open dnsInfo.conf failed");
        return;
    }

    memset(buf,0, sizeof(buf));
    snprintf(buf, sizeof(buf), "no-resolv\n");
    fwrite(buf, sizeof(char), strlen(buf), fp);
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", WAN_COMMON_NODE);
    memset(ip_forward_mode, 0, sizeof(ip_forward_mode));
    if(cfg_get_object_attr(node, "IPForwardModeEnable", ip_forward_mode, sizeof(ip_forward_mode)) < 0)
    {
        fclose(fp);
        fclose(fp_info);
        SVC_WAN_ERROR_INFO("get IPForwardModeEnable  failed.\n");
        return ;
    }

    if(0 == strncmp(ip_forward_mode, "Yes", 3))
    {
        fullroutesw = 1;
        default_pvc = get_default_pvc_DNSOrder();
        tempV4 = (unsigned short)default_pvc;
        tempV6 = (unsigned short)(default_pvc >> 16);
    }
#endif

    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
        //set default dns first, if flag==2, this means all dns was written finished.
        if(fullroutesw == 1)
        {
            if(flag == 0)
            {
                //dns v4
                if(tempV4 !=0xFFFF)
                {
                    i = tempV4;
                }
                else
                {
                    flag = 1; //v4 pass
                }
                if(tempV4 == tempV6)
                {
                    flag = 1;
                }
            }

            if(flag == 1)
            {
                //dns v6
                if(tempV6 !=0xFFFF)
                {
                    i = tempV6;
                }
                else
                {
                    flag = 2; //v6 pass
                    i = -1;
                    continue;
                }
            }
        }
#endif

        cfg = &obj->cfg;
        if(SVC_WAN_IS_BRIDGE(cfg))
        {
            continue;
        }
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
        if(fullroutesw == 1)
        {
            if(flag == 2 && i == tempV4)
            {
                //next time,if it's default DNS no need to write again
                goto v6;
            }

            if(flag == 1 && tempV4 != tempV6)//v6 dns set , if v4==v6, we need write v4&v6 dns to file one time.
            {
                goto v6;
            }
        }
#endif
        memset(if_name, 0, sizeof(if_name));
        svc_wan_intf_name(obj, if_name, sizeof(if_name));
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,obj->idx + 1);

        if(obj->flag & SVC_WAN_FLAG_UPV4)
        {
            memset(dns, 0, sizeof(dns));
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
            if((tempV4 == obj->idx && cfg_get_object_attr(DNSSERVER_SERVERV4_NODE, "Server1", dns, sizeof(dns)) > 0 
#else
            if((cfg_get_object_attr(DNSSERVER_SERVERV4_NODE, "Server1", dns, sizeof(dns)) > 0 
#endif
				&& 0 < strlen(dns) && isValidDnsIp(dns)) 
				|| (cfg_get_object_attr(node, "DNS", dns, sizeof(dns)) > 0 &&  0 < strlen(dns)
                && isValidDnsIp(dns)))
            {
                memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
                snprintf(buf, sizeof(buf), "server=%s@%s%s\n", dns, if_name, ((SVC_WAN_ATTR_VERSION_BOTH == cfg->version) ? "$ds" : ""));
#else
                snprintf(buf, sizeof(buf), "server=%s@%s\n", dns, if_name);
#endif
                fwrite(buf, sizeof(char), strlen(buf), fp);

                memset(buf, 0, sizeof(buf));
                if(strlen(cfg->srv_list) > 0)
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", dns, if_name, cfg->srv_list);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", dns, if_name, "OTHER");
                }
                fwrite(buf, sizeof(char), strlen(buf), fp_info);
            }

            memset(secdns, 0, sizeof(secdns));
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
            if((tempV4 == obj->idx && cfg_get_object_attr(DNSSERVER_SERVERV4_NODE, "Server2", secdns, sizeof(secdns)) > 0 
#else
            if((cfg_get_object_attr(DNSSERVER_SERVERV4_NODE, "Server2", secdns, sizeof(secdns)) > 0 
#endif
				&&  0 < strlen(secdns) && isValidDnsIp(secdns))
				|| (cfg_get_object_attr(node, "SecDNS", secdns, sizeof(secdns)) > 0 &&  0 < strlen(secdns) 
                && isValidDnsIp(secdns)))
            {
                memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
                snprintf(buf, sizeof(buf), "server=%s@%s%s\n", secdns, if_name, ((SVC_WAN_ATTR_VERSION_BOTH == cfg->version) ? "$ds" : ""));
#else
                snprintf(buf, sizeof(buf), "server=%s@%s\n", secdns, if_name);
#endif
                fwrite(buf, sizeof(char), strlen(buf), fp);

                memset(buf, 0, sizeof(buf));
                if(strlen(cfg->srv_list) > 0)
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", secdns, if_name, cfg->srv_list);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", secdns, if_name, "OTHER");
                }
                fwrite(buf, sizeof(char), strlen(buf), fp_info);
            }
        }
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
		else if ( SVC_WAN_IS_CONN_ON_DEMAND(cfg) )
		{
			if ( cfg_get_object_attr(node, "OndemandHisAddr", ppphisaddr, sizeof(ppphisaddr))
				&& '\0' != ppphisaddr[0]
				&& 0 != strcmp(ppphisaddr, "N/A") )
			{
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "server=%s@%s\n", ppphisaddr, if_name); 		
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s %s %s\n", ppphisaddr, if_name, 0 != cfg->srv_list[0] ? cfg->srv_list : "OTHER");
				fwrite(buf, sizeof(char), strlen(buf), fp_info);
			}
		}
#endif

#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
v6:
        if(fullroutesw == 1)
        {
            if(flag == 0)//v4 dns set finish
            {
                goto next;
            }
            if(flag == 2 && i == tempV6)//next time,if it's default DNS no need to write again
            {
                goto next;
            }
        }
#endif

        if(obj->flag & SVC_WAN_FLAG_UPV6)
        {
            memset(dns6, 0, sizeof(dns6));
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
            if((tempV6 == obj->idx && cfg_get_object_attr(DNSSERVER_SERVERV6_NODE, "Server1", dns, sizeof(dns)) > 0 &&  0 < strlen(dns6))
#else
            if((cfg_get_object_attr(DNSSERVER_SERVERV6_NODE, "Server1", dns, sizeof(dns)) > 0 &&  0 < strlen(dns6))
#endif
				|| (cfg_get_object_attr(node, "DNS6", dns6, sizeof(dns6)) > 0 &&  0 < strlen(dns6)))
            {
                memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
                snprintf(buf, sizeof(buf), "server=%s@%s%s\n", dns6, if_name, ((SVC_WAN_ATTR_VERSION_BOTH == cfg->version) ? "$ds" : ""));
#else
                snprintf(buf, sizeof(buf), "server=%s@%s\n", dns6, if_name);
#endif
                fwrite(buf, sizeof(char), strlen(buf), fp);

                memset(buf, 0, sizeof(buf));
                if(strlen(cfg->srv_list) > 0)
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", dns6, if_name, cfg->srv_list);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", dns6, if_name, "OTHER");
                }
                
                fwrite(buf, sizeof(char), strlen(buf), fp_info);
            }

            memset(secdns6, 0, sizeof(secdns6));
#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
            if((tempV6 == obj->idx && cfg_get_object_attr(DNSSERVER_SERVERV6_NODE, "Server2", dns, sizeof(dns)) > 0 &&  0 < strlen(secdns6))
#else
            if((cfg_get_object_attr(DNSSERVER_SERVERV6_NODE, "Server2", dns, sizeof(dns)) > 0 &&  0 < strlen(secdns6))
#endif
				|| (cfg_get_object_attr(node, "SecDNS6", secdns6, sizeof(secdns6)) > 0 &&  0 < strlen(secdns6)))
            {
                memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
                snprintf(buf, sizeof(buf), "server=%s@%s%s\n", secdns6, if_name, ((SVC_WAN_ATTR_VERSION_BOTH == cfg->version) ? "$ds" : ""));
#else
                snprintf(buf, sizeof(buf), "server=%s@%s\n", secdns6, if_name);
#endif
                fwrite(buf, sizeof(char), strlen(buf), fp);

                memset(buf, 0, sizeof(buf));
                if(strlen(cfg->srv_list) > 0)
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", secdns6, if_name, cfg->srv_list);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "%s %s %s\n", secdns6, if_name, "OTHER");
                }
                fwrite(buf, sizeof(char), strlen(buf), fp_info);
            }
        }
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
			else if ( ((SVC_WAN_ATTR_VERSION_IPV6 == cfg->version) || (SVC_WAN_ATTR_VERSION_BOTH == cfg->version))
					&& SVC_WAN_IS_CONN_ON_DEMAND(cfg) )
			{
				sprintf(ppphisaddr, "2014:1211::%d", i + 6311);
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "server=%s@%s\n", ppphisaddr, if_name); 		
				fwrite(buf, sizeof(char), strlen(buf), fp);
	
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s %s %s\n", ppphisaddr, if_name, 0 != cfg->srv_list[0] ? cfg->srv_list : "OTHER");
				fwrite(buf, sizeof(char), strlen(buf), fp_info);
	
				addDnsRoute6forOtherWan(if_name, (0 != cfg->srv_list[0] ? cfg->srv_list : "OTHER"), ppphisaddr);
			}
#endif

#if defined(TCSUPPORT_CT_FULL_ROUTE) && !defined(TCSUPPORT_CT_JOYME2)
next:
        if(fullroutesw == 1)
        {
            if(flag != 2)
            {
                //set default DNS first
                if(++flag == 2)
                {
                    i = -1;
                }
            }
        }
#endif
    }

#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), "%s", WAN_COMMON_NODE);
    memset(query_delay, 0, sizeof(query_delay));
    if((cfg_get_object_attr(node, "DNSForwardDelay", query_delay, sizeof(query_delay)) > 0) \
        && (0 < strlen(query_delay)))
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "query-delay=%s\n", query_delay);
        fwrite(buf, sizeof(char), strlen(buf), fp);
    }
#endif

#if defined(TCSUPPORT_CT_L2TP_VPN) || defined(TCSUPPORT_CT_VPN_PPTP)
	vpn_fp_info = fopen("/etc/vpn_dnsInfo.conf", "w");
	if(vpn_fp_info == NULL){
		fclose(fp);
		fclose(fp_info);
		SVC_WAN_ERROR_INFO("open vpn_dnsInfo.conf failed");
		return;
	}
	
	for (j = 0; j < 16; j++)
	{
		snprintf(vpn_dns_path, sizeof(vpn_dns_path), "/var/run/xl2tpd/%d/dns", j);	
		vpn_fp = fopen(vpn_dns_path, "r");
		if(vpn_fp == NULL)
		{
			continue;
		}
		else
		{
			memset(dns_buf, 0, sizeof(dns_buf));
			while(fgets(dns_buf, MAXSIZE - 1, vpn_fp))
			{
				if(0 == strlen(dns_buf))
				{
					continue;
				}		

				memset(dns, 0, sizeof(dns));
				if(1 == sscanf(dns_buf, "%s", dns))
				{
					memset(buf, 0, sizeof(buf));
					snprintf(buf, sizeof(buf), "server=%s@ppp%d\n", dns, 100 + i);
					fwrite(buf, sizeof(char), strlen(buf), fp);
					memset(buf, 0, sizeof(buf));
					snprintf(buf, sizeof(buf), "%s ppp%d VPN\n", dns, 100 + i);
					fwrite(buf, sizeof(char), strlen(buf), vpn_fp_info);
				}
				memset(dns_buf, 0, sizeof(dns_buf));
			}
			fclose(vpn_fp);
			vpn_fp = NULL;
		}
	}
	fclose(vpn_fp_info);
#endif
    fclose(fp);
    fclose(fp_info);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd,  sizeof(cmd), "%s", "killall -9 dnsmasq");
    svc_wan_execute_cmd(cmd);

    /* step3: start dnsmasq process */
    svc_wan_execute_cmd("/userfs/bin/dnsmasq &");
cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "0") ;

    /*sleep(1);*/
    
    return;
}

void checkDnsMaskRestart(void){	
	char dnsmaskRestart_flag[8] = {0};
	
	cfg_get_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", dnsmaskRestart_flag, sizeof(dnsmaskRestart_flag)) ;
	if(strcmp(dnsmaskRestart_flag,"1") == 0)
		restart_dnsmasq();

	return;
}



