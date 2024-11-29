#ifndef __SVC_CENTRAL_CTRL_WLAN_LINK_H__
#define __SVC_CENTRAL_CTRL_WLAN_LINK_H__

#include <ecnt_event_global/ecnt_event_global.h>

#define WLAN_ENTRY_ENABLE_SSID_ATTR       "EnableSSID"
#define WLAN_COMMON_BSSID_NUM_ATTR        "BssidNum"

#define CONTROLLER    1
#define AGENTER       2
#define UNCONFIGURED  0

#define MIN(a,b)			((a) < (b) ? (a) : (b))

void wlan_wsc_action(struct ecnt_event_data *event_data);

void wifi_up_handle(struct ecnt_event_data *event_data);
void wifi_down_handle(struct ecnt_event_data *event_data);
void veth_up_handle(struct ecnt_event_data *event_data);
void veth_down_handle(struct ecnt_event_data *event_data);

void ether_wan_itf_up_handle(struct ecnt_event_data *event_data);
void ether_wan_itf_down_handle(struct ecnt_event_data *event_data);
unsigned char is_easymesh_role_detect(void);
int update_mesh_role(char role);
#if defined(TCSUPPORT_ECNT_MAP)
void prepare_trigger_mesh(void);
#endif

#endif


