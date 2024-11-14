#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "central_ctrl_common.h"
#include "check_vpn_ppptunnels.h"
#include "utility.h"
/*
	check VPN PPP tunnel
*/
void checkVPNPPPTunnels(int xpon_traffic_state)
{
#if !defined(TCSUPPORT_CMCCV2)
	#define VPN_RESTART_DNS_TIMER 5 /* 5 seconds */
	char nodeName[32] = {0};
	char vpn_boot[32];
	char vpn_restart_dns[32];
	char vpn_dns_count[32];
	int restart_dns = 0;
	int dns_count = 0;
	if(E_XPON_LINK_UP != xpon_traffic_state )
	{
		return ;
	}
	
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
	memset(vpn_boot, 0, sizeof(vpn_boot));
	if((cfg_get_object_attr(nodeName, "vpn_boot", vpn_boot, sizeof(vpn_boot)) <= 0) )
	{
		return ;
	}
	
	if ( (0 >= strlen(vpn_boot)) || (1 != atoi(vpn_boot)) )
	{
		return ;
	}

	memset(vpn_restart_dns, 0, sizeof(vpn_restart_dns));
	if((cfg_get_object_attr(nodeName, "vpn_restart_dns", vpn_restart_dns, sizeof(vpn_restart_dns)) <= 0) \
		|| 0 >= strlen(vpn_restart_dns))
	{
		return ;
	}
	restart_dns = atoi(vpn_restart_dns);
	
	memset(vpn_dns_count, 0, sizeof(vpn_dns_count));
	if((cfg_get_object_attr(nodeName, "vpn_dns_count", vpn_dns_count, sizeof(vpn_dns_count)) <= 0) \
		|| 0 >= strlen(vpn_dns_count))
	{
		return ;
	}
	dns_count = atoi(vpn_dns_count);
	

	if ( restart_dns && ++dns_count >= VPN_RESTART_DNS_TIMER )
	{
		cfg_set_object_attr(nodeName, "vpn_restart_dns", "0");
		cfg_set_object_attr(nodeName, "vpn_dns_count",   "0");
		system("/usr/bin/killall -9 dnsmasq");
		system("/userfs/bin/dnsmasq &");
		
		tcdbg_printf("\n vpn restart dnsmasq \n");
	}
	else if(restart_dns)
	{
		memset(vpn_dns_count, 0, sizeof(vpn_dns_count));
		snprintf(vpn_dns_count, sizeof(vpn_dns_count), "%d", dns_count);
		cfg_set_object_attr(nodeName, "vpn_dns_count", vpn_dns_count);
	}
	
#endif

	return ;
}

