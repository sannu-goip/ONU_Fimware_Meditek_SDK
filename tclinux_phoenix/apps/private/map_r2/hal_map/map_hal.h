#ifndef __MAP_TCAPI_H__
#define __MAP_TCAPI_H__

#include <libtcapi.h>


#ifdef TCSUPPORT_MAP_R2

#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif /* GNU_PACKED */

#define MAX_VLAN_NUM 	48
#define VLAN_N_VID		4095

struct GNU_PACKED ts_fh_config {
	unsigned char itf_mac[6];
	unsigned short vid;
	char itf_name[16];
};

struct GNU_PACKED ts_fh_bss_setting {
	unsigned char itf_num;
	struct ts_fh_config fh_configs[0];
};

struct GNU_PACKED ts_common_setting {
	unsigned short primary_vid;
	unsigned char primary_pcp;
	unsigned char policy_vid_num;
	unsigned short policy_vids[MAX_VLAN_NUM];
};

struct GNU_PACKED ts_setting {
	struct ts_common_setting common_setting;
	struct ts_fh_bss_setting fh_bss_setting;
};

struct GNU_PACKED trans_vlan_config {
	unsigned char trans_vid_num;
	unsigned short vids[128];
};

struct GNU_PACKED trans_vlan_setting {
	struct trans_vlan_config trans_vlan_configs;
	unsigned char apply_itf_num;
	unsigned char apply_itf_mac[0];
};

#endif


#ifndef TRUE
#define TRUE						1
#endif
#ifndef FALSE
#define FALSE						0
#endif

struct bss_info_cofig{
	unsigned char   itf_name[16];
	unsigned char	Ssid[32 + 1];
	unsigned short	AuthMode;
	unsigned short	EncrypType;
	unsigned char	WPAKey[64 + 1];
	unsigned char   hidden_ssid;
	unsigned char   isStopFlag;
};

/* Authentication types */
#define AUTHMODE_OPEN								"OPEN"
#define AUTHMODE_OPEN_VALUE							0x0001
#define AUTHMODE_WPAPSK               				"WPAPSK"
#define AUTHMODE_WPAPSK_VALUE 						0x0002
#define AUTHMODE_WPA2PSK                  			"WPA2PSK"
#define AUTHMODE_WPA2PSK_VALUE 						0x0020
#define AUTHMODE_WPAPSKWPA2PSK                 		"WPAPSKWPA2PSK"
#define AUTHMODE_WPA3PSK_VALUE 						0x0040
#define AUTHMODE_WPA3PSK                  			"WPA3PSK"
#define AUTHMODE_WPA2PSKWPA3PSK                  	"WPA2PSKWPA3PSK"

/* Encryption type */
#define ENCRYPTYPE_NONE								"NONE"
#define ENCRYPTYPE_NONE_VALUE						0x0001
#define ENCRYPTYPE_TKIP			          			"TKIP"
#define ENCRYPTYPE_TKIP_VALUE						0x0004
#define ENCRYPTYPE_AES			          			"AES"
#define ENCRYPTYPE_AES_VALUE						0x0008
#define ENCRYPTYPE_TKIPAES			          		"TKIPAES"

#define MESH_DAT_NODE                               "mesh_dat"
#define MAP_TRURNKEY_ATTR                           "MAP_Turnkey"

#define MESH_COMMON_NODE                         	"mesh_common" 
#define MAP_QUICK_CH_CHANGE_ATTR					"MAP_QuickChChange"
#define MAP_DEVICE_ROLE_ATTR                        "DeviceRole"
#define MAP_11AX_ATTR								"Mesh11axFlag"
#define MAP_REINIT_FLAG_ATTR                        "ReinitMeshFlag"


#define MESH_MAP_CFG_NODE                           "mesh_mapcfg"
#define INFO_MESH_NODE								"info_mesh"

#define MAP_BSS_CONFIG_PRIORITY_ATTR                "bss_config_priority"

#define MESH_MAPD_CFG_NODE                          "mesh_mapdcfg"
#define MAP_NETWORK_OPTIMIZATION_ENABLE_ATTR    	"NetworkOptimizationEnabled"

#define MESH_APCLIBH_NODE                         	"mesh_apclibh" 

#define LAN_NODE									"Lan_Entry"
#define LAN_IP_NODE									"IP"
#define LAN_DHCP									"Lan_Dhcp"
#define LAN_DHCP_NODE								"type"
#define LAN_DHCP_ENABLE								"1"
#define LAN_DHCP_DISABLE							"0"
#define DHCPD										"Dhcpd_Common"
#define DHCPD_OPTION60								"Dhcpd_Option60"
#define DHCPD_START									"start"

#define ROUTE     1
#define BRIDGE    2
#define APCLIENT  3

#define RADIO_2G  0
#define RADIO_5G  1

int get_map_attr_lan_ipaddr(char *buffer, int size);

int get_map_attr_MAP_Turnkey(char *buffer, int size);
int get_map_attr_Enable_WPS_toggle_5GL_5GH(char *buffer, int size);
int get_map_attr_MAP_QuickChChange(char *buffer, int size);
int get_map_attr_MAP_11ax(char *buffer, int size);

int get_map_attr_MAP_BssConfigPriority(char *buffer, int size);
int get_map_attr_EtherWanReinitFlag(char *buffer, int size);

int get_map_attr_BhProfileValid(int num, char *buffer, int size);
int get_map_attr_BhProfileSsid(int num, char *buffer, int size);
int get_map_attr_BhProfileWpaPsk(int num, char *buffer, int size);
int get_map_attr_BhProfileAuthMode(int num, char *buffer, int size);
int get_map_attr_BhProfileEncrypType(int num, char *buffer, int size);
int get_map_attr_BhProfileRaID(int num, char *buffer, int size);

int set_map_attr_BhProfileValid(int num, int value);
int set_map_attr_BhProfileSsid(int num, char *buffer);
int set_map_attr_BhProfileAuthMode(int num, char *buffer);
int set_map_attr_BhProfileEncrypType(int num, char *buffer);
int set_map_attr_BhProfileWpaPsk(int num, unsigned char *buffer);
int set_map_attr_BhProfileRaID(int num, char *buffer);
int set_map_attr_NetworkOptimizationEnabled(unsigned char value);

int set_map_attr_dhcpd_start(char* buffer);
int set_map_attr_lan_dhcpd(char *buffer);
int set_map_attr_R1ChannelScanStatus(char *buffer);
int set_map_attr_R2ChannelScanStatus(char *buffer);
int ecnt_set_map_parameters(int num, char* param, char* value);
int ecnt_get_map_parameters(int num, char* param, char* value, int val_len);
int ecnt_save_map_parameters(void);
int ecnt_restart_map(void);
int update_wlan_entry_info(struct bss_info_cofig bss_info);
int update_device_role(unsigned char role);
int device_is_autorole(void);
int update_trigger_mesh_reinit(unsigned char flag);
int ecnt_set_map_almac(char* almac);


#if defined(TCSUPPORT_CT_UBUS)
int send_wifimeshmapinfo2ubus();
int set_map_attr_uptime(char *buffer);
int set_map_attr_status(char *buffer);
#endif
int update_current_ap_mode(int mode);
int update_current_radio(int radio);
int update_ts_vlan_info(struct ts_setting *ts_info);

#endif

