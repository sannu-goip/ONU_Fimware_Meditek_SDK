#ifndef __SVC_WLAN_MESH_H__
#define __SVC_WLAN_MESH_H__

#include <svchost_api.h> 

#define MAP_ENABLE_ATTR                               "MapEnable"
#define MAP_TRURNKEY_ATTR                             "MAP_Turnkey"
#define MAP_EXT_ATTR                                  "MAP_Ext"
#define BS20_ENABLE_ATTR                              "BS20Enable"
#define EASYMESH_ENABLE_ATTR                          "EasyMeshEnable"
#define BAND_STEERING_ENABLE_ATTR                     "BandSteeringEnable"
#define MAP_MODE_ATTR                              	  "MapMode"

#define WLAN_ENTRY_SSID_ATTR                          "SSID"
#define WLAN_ENTRY_AUTH_MODE_ATTR                     "AuthMode"
#define WLAN_ENTRY_ENCRYPT_TYPE_ATTR                  "EncrypType"
#define WLAN_ENTRY_KEY_ATTR                           "WPAPSK"
#define WLAN_ENTRY_HIDDEN_SSID_ATTR                   "HideSSID"

#define MAP_BR_ITF_NAME_ATTR            "br_inf"


typedef struct _wlan_mesh_path_info_
{
	char      path[64];
	wifi_type band;
	int  ssid_idx;
}wlan_mesh_path_info_t, *pt_wlan_mesh_path_info;

static inline int is_zero_ether_addr(const unsigned char *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}


int update_mesh_dat(wifi_type band);
int wlan_mesh_update_dat(void);
int wlan_mesh_renew_bssinfo(struct host_service_pbuf* pbuf);

#endif

