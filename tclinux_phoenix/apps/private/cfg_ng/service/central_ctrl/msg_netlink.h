#ifndef __SVC_CENTRAL_CTRL_MSG_NETLINK_H__
#define __SVC_CENTRAL_CTRL_MSG_NETLINK_H__

/***************************************************/

#if defined(TCSUPPORT_ECNT_MAP)
#define WLAN_ENTRY_ENABLE_SSID_ATTR       "EnableSSID"
#define WLAN_COMMON_BSSID_NUM_ATTR        "BssidNum"
#endif


void msg_netlink_unregister(void);

void msg_netlink_register(void);

#endif

