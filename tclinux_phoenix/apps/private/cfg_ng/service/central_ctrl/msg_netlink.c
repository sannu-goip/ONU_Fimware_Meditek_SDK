#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_eth.h>
#include <ecnt_event_global/ecnt_event_wifi.h>
#include <ecnt_event_global/ecnt_event_lanhost.h>
#include <ecnt_event_global/ecnt_event_system.h>
#include <ecnt_event_global/ecnt_event_veth.h>
#include <ecnt_event_global/ecnt_event_ether_wan.h>
#include <libecntevent.h>
#ifdef TCSUPPORT_WLAN
#include <libapi_lib_wifimgr.h>
#endif
#include "central_ctrl_common.h"
#include "lan_port_link.h"
#include "msg_netlink.h"
#include "wlan_link.h"
#include "lanhost2_mgr_notify.h"
#include "system_message_notify.h"
#include <utility.h>


/****************************************************************/
static struct ecnt_event_source* source;
static struct ecnt_event_handle msg_netlink_handle[] = 
{
	{
		.maintype	= ECNT_EVENT_ETH,
		.subtype	= ECNT_EVENT_ETH_DOWN,
		.handle		= lan_down_action,
	},
	{
		.maintype	= ECNT_EVENT_ETH,
		.subtype	= ECNT_EVENT_ETH_UP,
		.handle		= lan_up_action,
	},
	{
		.maintype	= ECNT_EVENT_ETH,
		.subtype	= ECNT_EVENT_ETH_WAN_DOWN,
		.handle		= ether_wan_down_action,
	},
	{
		.maintype	= ECNT_EVENT_ETH,
		.subtype	= ECNT_EVENT_ETH_WAN_UP,
		.handle		= ether_wan_up_action,
	},
	{
		.maintype	= ECNT_EVENT_WIFI,
		.subtype	= ECNT_EVENT_WIFI_WSC,
		.handle		= wlan_wsc_action,
	},
	{
		.maintype	= ECNT_EVENT_LANHOST,
		.subtype	= ECNT_EVENT_LANHOST_UPDATE,
		.handle 	= ctc_lan_host2_update
	},
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
	{
		.maintype	= ECNT_EVENT_WIFI,
		.subtype	= ECNT_EVENT_WIFI_UPDATESTAINFO,
		.handle 	= ctc_wifi_update
	},
#endif
	{
		.maintype	= ECNT_EVENT_SYSTEM,
		.subtype	= ECNT_EVENT_SNMP,
		.handle 	= ctc_system_snmp_update,
	},
	{
		.maintype	= ECNT_EVENT_SYSTEM,
		.subtype	= ECNT_EVENT_PLUGIN_INFO,
		.handle 	= ctc_plugin_info_update,
	},
	{
		.maintype	= ECNT_EVENT_WIFI,
		.subtype	= ECNT_EVENT_WIFI_UP,
		.handle		= wifi_up_handle,
	},
	{
		.maintype	= ECNT_EVENT_WIFI,
		.subtype	= ECNT_EVENT_WIFI_DOWN,
		.handle		= wifi_down_handle,
	},
	{
		.maintype	= ECNT_EVENT_VETH,
		.subtype	= ECNT_EVENT_VETH_DOWN,
		.handle		= veth_down_handle,
	},
	{
		.maintype	= ECNT_EVENT_VETH,
		.subtype	= ECNT_EVENT_VETH_UP,
		.handle		= veth_up_handle,
	},
        {
		.maintype	= ECNT_EVENT_SYSTEM,
		.subtype	= ECNT_EVENT_WEB_LOGIN,
		.handle		= ctc_web_login_handle,
	},
	{
		.maintype	= ECNT_EVENT_ETHER_WAN,
		.subtype	= ECNT_EVENT_ETHER_WAN_UP,
		.handle		= ether_wan_itf_up_handle,
	},
	{
		.maintype	= ECNT_EVENT_ETHER_WAN,
		.subtype	= ECNT_EVENT_ETHER_WAN_DOWN,
		.handle		= ether_wan_itf_down_handle,
	},
	{
		.maintype	= ECNT_EVENT_SYSTEM,
		.subtype	= ECNT_EVENT_V6_ADDR_TIMEOUT,
		.handle		= ctc_ipv6_addr_info_handle,
	},
	{
		.maintype	= ECNT_EVENT_MAX,
		.subtype	= ECNT_EVENT_ETH_MAX,
		.handle		= NULL,
	},
};

int msg_netlink_event_hook(struct ecnt_data *in_data)
{
	return ecnt_event_execute(in_data, msg_netlink_handle);
}

void msg_netlink_register(void)
{
	source = ecnt_event_register("central_ctrl", msg_netlink_event_hook);
	if(NULL == source)
	{
		tcdbg_printf("msg_netlink_register faile.\n");
		return ;
	}
	
	return ;
}

void msg_netlink_unregister(void)
{
	ecnt_event_unregister(source);
	
	return ;
}

