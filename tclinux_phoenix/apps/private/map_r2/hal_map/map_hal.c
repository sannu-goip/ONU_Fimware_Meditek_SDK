/*************************
	author:  ye.li
	date:    20190128
*************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map_hal.h"
#include <lan_port/lan_port_info.h>
#if defined(TCSUPPORT_CT_UBUS)
#include <notify/notify.h>
#endif

#define TCAPI_PROCESS_OK 0

#define ECNT_RADIO_24G 0 
#define ECNT_RADIO_5GL 1 
#define ECNT_RADIO_5GH 2 
#define ECNT_RADIO_5G  3 

#define ECNT_MAP_ROLE_UNCONFIGURED           (0)
#define ECNT_MAP_CONTROLLER                  (1)
#define ECNT_MAP_AGENTER                     (2)

int get_map_attr_lan_ipaddr(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;

	if(tcapi_get(LAN_NODE, LAN_IP_NODE, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
	
	return 0;
}


int get_map_attr_MAP_Turnkey(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;

	if(tcapi_get(MESH_DAT_NODE, MAP_TRURNKEY_ATTR, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
	
	return 0;
}

int get_map_attr_EtherWanReinitFlag(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	if(tcapi_get(MESH_COMMON_NODE, MAP_REINIT_FLAG_ATTR, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
	
	return 0;
}

int get_map_attr_Enable_WPS_toggle_5GL_5GH(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_MAP_QuickChChange(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;

	if(tcapi_get(MESH_COMMON_NODE, MAP_QUICK_CH_CHANGE_ATTR, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
	
	return 0;
}

int get_map_attr_MAP_11ax(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;

	if(tcapi_get(MESH_COMMON_NODE, MAP_11AX_ATTR, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
	
	return 0;
}

int get_map_attr_MAP_BssConfigPriority(char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;

	if(tcapi_get(MESH_MAP_CFG_NODE, MAP_BSS_CONFIG_PRIORITY_ATTR, buffer) != TCAPI_PROCESS_OK )
	{
		return -1;
	}
		
	return 0;
}

int get_map_attr_BhProfileValid(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_BhProfileSsid(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_BhProfileWpaPsk(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_BhProfileAuthMode(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_BhProfileEncrypType(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int get_map_attr_BhProfileRaID(int num, char *buffer, int size)
{
	if (buffer == NULL || size <= 0)
		return -1;
	
	return 0;
}

int set_map_attr_BhProfileValid(int num, int value)
{
	return 0;
}


int set_map_attr_BhProfileSsid(int num, char *buffer)
{
	if (buffer == NULL)
		return -1;

	return 0;
}

int set_map_attr_BhProfileAuthMode(int num, char *buffer)
{
	if (buffer == NULL)
		return -1;

	return 0;
}

int set_map_attr_BhProfileEncrypType(int num, char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	return 0;
}

int set_map_attr_BhProfileWpaPsk(int num, unsigned char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	return 0;
}

int set_map_attr_BhProfileRaID(int num, char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	return 0;
}

int set_map_attr_NetworkOptimizationEnabled(unsigned char value)
{
	tcapi_set(MESH_MAPD_CFG_NODE, MAP_NETWORK_OPTIMIZATION_ENABLE_ATTR, value ? "1":"0");
	tcapi_save();
	
	return 0;
}

int set_map_attr_dhcpd_start(char *buffer)
{
	char addrParts[8] = {0};
	char startIP[64];
	char* token;
	char value[32];
	int i = 0;

	if (buffer == NULL)
	{
		return -1;
	}
	
	memset(value, 0, sizeof(value));
	memset(startIP, 0, sizeof(startIP));
	strncpy(startIP,buffer,sizeof(startIP)-1);		
	token = strtok(startIP, ".");
	if(token != NULL)
		snprintf(value, sizeof(value), "%s", token);
	while(token != NULL)
	{
		strcat(value, ".");	
		token = strtok(NULL, ".");		
		if(2 == i)
		{
			if(atoi(token) < 254)
				snprintf(addrParts,sizeof(addrParts),"%d", atoi(token)+1);
			else
				snprintf(addrParts,sizeof(addrParts),"%d", 254);
			strcat(value,addrParts);
			break;
		}
		snprintf(value + strlen(value), sizeof(value) - strlen(value), "%s", token);
		i++;
	}
	tcapi_set(DHCPD, DHCPD_START, value);
	tcapi_set(DHCPD_OPTION60, DHCPD_START, value);	
	
	return 0;
}

int set_map_attr_lan_dhcpd(char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	tcapi_set(LAN_DHCP, LAN_DHCP_NODE, buffer);
	tcapi_commit(LAN_DHCP);
	sleep(1);
	tcapi_commit(DHCPD);
	sleep(1);
	tcapi_save();
	return 0;
}

int set_map_attr_R1ChannelScanStatus(char *buffer)
{
	if (buffer == NULL)
		return -1;
	tcapi_set("Mesh_action", "R1_ch_scan_status", buffer);
	
	return 0;
}

int set_map_attr_R2ChannelScanStatus(char *buffer)
{
	if (buffer == NULL)
		return -1;
	tcapi_set("Mesh_action", "R2_ch_scan_status", buffer);
	return 0;
}

int ecnt_set_map_parameters(int num, char* param, char* value)
{
	char aplibhEntry[32];
	memset(aplibhEntry, 0, sizeof(aplibhEntry));
	snprintf(aplibhEntry,sizeof(aplibhEntry),MESH_APCLIBH_NODE"_entry%d",num);
	
	tcapi_set(aplibhEntry, param, value);
	
	return 0;
}

int ecnt_get_map_parameters(int num, char* param, char* value, int val_len)
{

	char tmp_buffer[350];
	char aplibhEntry[32];
	if (param == NULL || value == NULL || val_len <= 0)
		return -1;	
	memset(tmp_buffer, 0, sizeof(tmp_buffer));
	memset(aplibhEntry, 0, sizeof(aplibhEntry));
	snprintf(aplibhEntry,sizeof(aplibhEntry),MESH_APCLIBH_NODE"_entry%d",num);
	if(TCAPI_PROCESS_OK != tcapi_get(aplibhEntry, param, tmp_buffer))
	{
		return -1;
	}
	strncpy(value, tmp_buffer, val_len);
	return 0;
}

int ecnt_save_map_parameters(void)
{
	tcapi_save();

	return 0;
}

int ecnt_restart_map(void)
{
#if 1
	char cmd[128];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "ifconfig ra0 down");
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "ifconfig ra0 up");
	system(cmd);
#else
	system("reboot");
#endif
	return 0;
}

static int read_wifi_interface(char *wifiIfname, int *wifi_type)
{
	
	if ((WLAN_AC_ITF_NAME_SIZE == strlen(wifiIfname)) \
		&& (NULL != strstr(wifiIfname, WLAN_AC_ITF_NAME_FORMAT)))
	{
		*wifi_type = ECNT_RADIO_5G;
		return  wifiIfname[WLAN_AC_ITF_NAME_SIZE - 1] - '0';
	}
	else if ((WLAN_ITF_NAME_SIZE == strlen(wifiIfname)) \
		&& (NULL != strstr(wifiIfname, WLAN_ITF_NAME_FORMAT)))
	{
		*wifi_type = ECNT_RADIO_24G;
		return wifiIfname[WLAN_ITF_NAME_SIZE - 1] - '0';
	}

	return -1;
}

int update_ts_vlan_info(struct ts_setting *ts_info)
{
	int wifi_type = 0, entryIdx = 0;
	char nodeName[32];
	int i = 0;
	char attr[36] = {0};
	char DeviceRole[8] = {0};
	int flag_have_primary = 0;
	
	tcapi_get("Mesh_Common", "DeviceRole", DeviceRole);
	if (atoi(DeviceRole) != 2)
		return -1;
					
	// only update fh info
	for (i = 0; i < ts_info->fh_bss_setting.itf_num; i++)
	{
		entryIdx = read_wifi_interface(ts_info->fh_bss_setting.fh_configs[i].itf_name, &wifi_type);
		if( -1 == entryIdx)
		{
			return -1;
		}

		memset(nodeName, 0, sizeof(nodeName));
		if (ECNT_RADIO_24G == wifi_type)
		{
			snprintf(nodeName, sizeof(nodeName), "Mesh_Radio2gbssinfo_Entry%d", entryIdx);
		}
		else 
		{
			snprintf(nodeName, sizeof(nodeName), "Mesh_Radio5glbssinfo_Entry%d", entryIdx);
		}

		memset(attr, 0, sizeof(attr));
		snprintf(attr, sizeof(attr), "%d", ts_info->fh_bss_setting.fh_configs[i].vid);
		tcapi_set(nodeName, "VlanID", attr);
			
		if (ts_info->common_setting.primary_vid == ts_info->fh_bss_setting.fh_configs[i].vid 
			&& ts_info->fh_bss_setting.fh_configs[i].vid != VLAN_N_VID && !flag_have_primary)
		{
			tcapi_set(nodeName, "PrimaryVlan", "1");
			memset(attr, 0, sizeof(attr));
			snprintf(attr, sizeof(attr), "%d", ts_info->common_setting.primary_pcp);
			tcapi_set(nodeName, "DefaultPCP", attr);
			flag_have_primary = 1;
		}
		else
		{
			tcapi_set(nodeName, "PrimaryVlan", "0");
			tcapi_set(nodeName, "DefaultPCP", "");	
		}
	}

	return 0;
}

int update_wlan_entry_info(struct bss_info_cofig bss_info)
{
	int wifi_type = 0, entryIdx = 0;
	char nodeName[32], wifi_common[32];
	char test[32];
	char num[8] = {0};

	entryIdx = read_wifi_interface(bss_info.itf_name, &wifi_type);
	if( -1 == entryIdx)
	{
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	memset(wifi_common, 0, sizeof(wifi_common));
	if (ECNT_RADIO_24G == wifi_type)
	{
		snprintf(nodeName, sizeof(nodeName), "WLan_Entry%d", entryIdx);
		snprintf(wifi_common, sizeof(wifi_common), "WLan_Common");
	}
	else 
	{
		snprintf(nodeName, sizeof(nodeName), "WLan11ac_Entry%d", entryIdx);
		snprintf(wifi_common, sizeof(wifi_common), "WLan11ac_Common");
	}

	tcapi_get("WLan_Common", "syn_num", num);
	if ('\0' == num[0] || 0 == atoi(num))
	{
		tcapi_set("WLan_Common", "syn_num", "1");
	}
	else if (1 == atoi(num))
	{
		tcapi_set("WLan_Common", "syn_num", "2");
	}
	
	if(bss_info.isStopFlag)
	{
		tcapi_set(nodeName, "SSID", "MAP-UNCONF");
	}
	else
	{
		tcapi_set(nodeName, "SSID", bss_info.Ssid);
		if (AUTHMODE_OPEN_VALUE == bss_info.AuthMode)
			tcapi_set(nodeName, "AuthMode", AUTHMODE_OPEN);
		else if (AUTHMODE_WPA2PSK_VALUE == bss_info.AuthMode)
		{
			tcapi_set(nodeName, "AuthMode", AUTHMODE_WPA2PSK);
			tcapi_set(nodeName, "WPAPSK", bss_info.WPAKey);
		}
		else if (AUTHMODE_WPAPSK_VALUE == bss_info.AuthMode)
		{
			tcapi_set(nodeName, "AuthMode", AUTHMODE_WPAPSK);
			tcapi_set(nodeName, "WPAPSK", bss_info.WPAKey);
		}
		else if ((AUTHMODE_WPAPSK_VALUE | AUTHMODE_WPA2PSK_VALUE) == bss_info.AuthMode)
		{
			tcapi_set(nodeName, "AuthMode", AUTHMODE_WPAPSKWPA2PSK);
			tcapi_set(nodeName, "WPAPSK", bss_info.WPAKey);
		}
		else if (AUTHMODE_WPA3PSK_VALUE == bss_info.AuthMode)
		{
			tcapi_set(nodeName, "AuthMode", AUTHMODE_WPA3PSK);
			tcapi_set(nodeName, "WPAPSK", bss_info.WPAKey);
		}
		else if (((AUTHMODE_WPA2PSK_VALUE | AUTHMODE_WPA3PSK_VALUE)) == bss_info.AuthMode)
		{
			tcapi_set(nodeName, "AuthMode", AUTHMODE_WPA2PSKWPA3PSK);
			tcapi_set(nodeName, "WPAPSK", bss_info.WPAKey);
		}
		
		if (ENCRYPTYPE_AES_VALUE == bss_info.EncrypType)
			tcapi_set(nodeName, "EncrypType", ENCRYPTYPE_AES);
		else if (ENCRYPTYPE_TKIP_VALUE == bss_info.EncrypType) 
			tcapi_set(nodeName, "EncrypType", ENCRYPTYPE_TKIP);
		else if ((ENCRYPTYPE_AES_VALUE | ENCRYPTYPE_TKIP_VALUE) == bss_info.EncrypType) 
			tcapi_set(nodeName, "EncrypType", ENCRYPTYPE_TKIPAES);
		else if (ENCRYPTYPE_NONE_VALUE == bss_info.EncrypType)
			tcapi_set(nodeName, "EncrypType", ENCRYPTYPE_NONE);
		
		if (0 == bss_info.hidden_ssid)
			tcapi_set(nodeName, "HideSSID", "0");
		else
			tcapi_set(nodeName, "HideSSID", "1");
	}
	return 0;

}

#if defined(TCSUPPORT_CT_UBUS)
int send_wifimeshmapinfo2ubus()
{
	wifimeshinfo_msg_t msgdata;
	char almac[16];
	char mode[10];
	char ssid[32];
	char key[32];
	char auth[32];
	char encrypt[32];
	char role[4];

	memset(&msgdata, 0, sizeof(wifimeshinfo_msg_t));
	memset(almac, 0, sizeof(almac));
	memset(mode, 0, sizeof(mode));
	memset(ssid, 0, sizeof(ssid));
	memset(key, 0, sizeof(key));
	memset(auth, 0, sizeof(auth));
	memset(encrypt, 0, sizeof(encrypt));
		
	tcapi_get("Mesh_mapcfg","AL-MAC",almac);
	strncpy(msgdata.almac, almac, sizeof(msgdata.almac) - 1);
	tcapi_get("Mesh_Common", "DeviceRole", role);
	
	if(0==strcmp(role,"1")){/*controller*/	
	strncpy(msgdata.mode, "controller", sizeof(msgdata.mode) - 1);
	/*entry 1 2.4g backhaulbss msg*/	
	tcapi_get("Wlan_Entry","SSID",ssid);
	tcapi_get("Wlan_Entry","WPAPSK",key);
	tcapi_get("Wlan_Entry","AuthMode",auth);
	tcapi_get("Wlan_Entry","EncrypType",encrypt);
	strncpy(msgdata.ssid, ssid, sizeof(msgdata.ssid) - 1);
	strncpy(msgdata.key, key, sizeof(msgdata.key) - 1);
	strncpy(msgdata.auth, auth, sizeof(msgdata.auth) - 1);
	strncpy(msgdata.encrypt, encrypt, sizeof(msgdata.encrypt) - 1);

	/*entry 1 5g backhaulbss msg*/
	memset(ssid, 0, sizeof(ssid));
	memset(key, 0, sizeof(key));
	memset(auth, 0, sizeof(auth));
	memset(encrypt, 0, sizeof(encrypt));
	tcapi_get("Wlan11ac_Entry","SSID",ssid);
	tcapi_get("Wlan11ac_Entry","WPAPSK",key);
	tcapi_get("Wlan11ac_Entry","AuthMode",auth);
	tcapi_get("Wlan11ac_Entry","EncrypType",encrypt);
	strncpy(msgdata.ssid11ac, ssid, sizeof(msgdata.ssid11ac) - 1);
	strncpy(msgdata.key11ac, key, sizeof(msgdata.key11ac) - 1);
	strncpy(msgdata.auth11ac, auth, sizeof(msgdata.auth11ac) - 1);
	strncpy(msgdata.encrypt11ac, encrypt, sizeof(msgdata.encrypt11ac) - 1);
	}
	else if(0==strcmp(role,"2")){
	strncpy(msgdata.mode, "agent", sizeof(msgdata.mode) - 1);
	}
	else{
	strncpy(msgdata.mode, "unknown", sizeof(msgdata.mode) - 1);

	}	
	ctc_notify2ctcapd(NOTIFY_WIFI_MESHMAP_STATUS, &msgdata, sizeof(wifimeshinfo_msg_t));
	return 0;
}

int set_map_attr_uptime(char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	tcapi_set(MESH_MAP_CFG_NODE, "uptime", buffer);
	return 0;
}

int set_map_attr_status(char *buffer)
{
	if (buffer == NULL)
		return -1;
	
	tcapi_set(MESH_DAT_NODE, "status", buffer);
	return 0;
}

#endif 

/***********************************************************************/
#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING) && defined(TCSUPPORT_NP)
#define ROUTER    1
#define BRIDGE    2
#define REPEATER  3
static int get_ap_work_mode(void)
{
	unsigned work_mode = 0;
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));

	if ( tcapi_get("wan_common", "WorkMode", buf) != 0)
	{
		return -1;
	}
	
	if(0 == strcmp(buf, "router"))
	{
		return ROUTER;
	}
	else if(0 == strcmp(buf, "bridge"))
	{
		return BRIDGE;
	}
	else if(0 == strcmp(buf, "repeater"))
	{
		return REPEATER;
	}
	
	return -1;
}

static int update_ap_work_mode(int mode)
{
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	
	switch(mode)
	{
		case ROUTER:
		{
			snprintf(buf, sizeof(buf), "%s", "router");
			break;
		}
		case BRIDGE:
		{
			snprintf(buf, sizeof(buf), "%s", "bridge");
			break;
		}
		default:
		{
			return -1;
		}
	}
	if(0 != tcapi_set("wan_common", "WorkMode", buf))
	{
		return -1;
	}
		
	tcapi_commit("wan_common");
	return 0;
}

static void ap_mode_bridge_route_switch(int mode)
{
	int work_mode = 0;
	work_mode = get_ap_work_mode();
	if(0 > work_mode)
	{
		return ;
	}
	
	if(!(work_mode ^ mode))
	{
		return ;
	}
	
	update_ap_work_mode(mode);

	return ;
}
#endif

int update_trigger_mesh_reinit(unsigned char flag)
{
	unsigned char buf[32];
	switch(flag)
	{
		case 0:
		{
			snprintf(buf, sizeof(buf), "%d", 0);
			break;
		}
		default:
		{
			snprintf(buf, sizeof(buf), "%d", 1);
			break;
		}
	}
	tcapi_set(MESH_COMMON_NODE, MAP_REINIT_FLAG_ATTR, buf);
	
	tcapi_save();
	return 0;
}

int update_device_role(unsigned char role)
{
	unsigned char device_role = ECNT_MAP_AGENTER;
	unsigned char buf[32];
	switch(role)
	{
		case 0:
		{
			device_role = ECNT_MAP_CONTROLLER;
			break;
		}
		default:
		{
			device_role = ECNT_MAP_AGENTER;
			break;
		}
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", device_role);
	tcapi_set(MESH_COMMON_NODE, MAP_DEVICE_ROLE_ATTR, buf);
	
#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING) && defined(TCSUPPORT_NP)
	/*switch AP mode*/
	if(ECNT_MAP_AGENTER == device_role)
	{
		ap_mode_bridge_route_switch(BRIDGE);
		/*trigger mesh restart wait for eth link*/
	}
#endif

	tcapi_save();

	return 0;
}

int device_is_autorole(void)
{
	unsigned char device_role = ECNT_MAP_ROLE_UNCONFIGURED;
	unsigned char is_autorole = 0;
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	tcapi_get(MESH_COMMON_NODE, MAP_DEVICE_ROLE_ATTR, buf);
	device_role = atoi(buf);
	switch(device_role)
	{
		case ECNT_MAP_AGENTER:
		case ECNT_MAP_CONTROLLER:
		{
			is_autorole = 0;
			break;
		}
		default:
		{
			is_autorole = 1;
			break;
		}
	}

	return is_autorole;
}

int update_current_ap_mode(int mode)
{
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	
	switch(mode)
	{
		case ROUTE:
		{
			snprintf(buf, sizeof(buf), "%s", "Route");
			break;
		}
		case BRIDGE:
		{
			snprintf(buf, sizeof(buf), "%s", "Bridge");
			break;
		}
		case APCLIENT:
		{
			snprintf(buf, sizeof(buf), "%s", "APClient");
			break;
		}
		default:
		{
			return -1;
		}
	}
	if(0 != tcapi_set("WanInfo_Common", "CurAPMode", buf))
	{
		return -1;
	}
		
	return 0;
}


int update_current_radio(int radio)
{
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	
	switch(radio)
	{
		case RADIO_2G:
		{
			snprintf(buf, sizeof(buf), "%s", "0");
			break;
		}
		case RADIO_5G:
		{
			snprintf(buf, sizeof(buf), "%s", "1");
			break;
		}
		default:
		{
			return -1;
		}
	}
	if(0 != tcapi_set("APCli_Common", "currentRadio", buf))
	{
		return -1;
	}
		
	return 0;
}

int ecnt_set_map_almac(char* almac)
{

	tcapi_set("Mesh_mapcfg","AL-MAC",almac);
	tcapi_save();

	return 0;
}
