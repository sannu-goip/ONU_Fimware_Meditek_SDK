/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <cfg_api.h>
#include <utility.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <svchost_api.h>
#include <cfg_api.h>
#include <libapi_lib_wifimgr.h>
#include "wlan_cfg.h"
#include "wlan_mesh.h"
#include "wlan_utils.h"
#include <lan_port/lan_port_info.h>

#define MAP_BSSINFO_FRONT_HAUL_ATTR        "FrontHaul"
#define MAP_BSSINFO_BACK_HAUL_ATTR         "BackHaul"

enum MAPRole {
	MAP_ROLE_TEARDOWN = 4,
	MAP_ROLE_FRONTHAUL_BSS = 5,
	MAP_ROLE_BACKHAUL_BSS = 6,
	MAP_ROLE_BACKHAUL_STA = 7,
};

/********************************************************************************************/
/********************************************************************************************/
static int map_bssinfo_wlan_entry(char *bssinfo_path, wlan_mesh_path_info_t *info)
{
	char *p = NULL;

	if(NULL != strstr(bssinfo_path, MESH_RADIO2G_BSSINFO_ENTRY_NODE_FROMAT))
	{
		info->band     = WIFI_2_4G;
		p = bssinfo_path + strlen(MESH_RADIO2G_BSSINFO_ENTRY_NODE_FROMAT);
		info->ssid_idx  = atoi(p);
		snprintf(info->path, sizeof(info->path), "%s.%d", WLAN_ENTRY_NODE, info->ssid_idx);
	}
	else if(NULL != strstr(bssinfo_path, MESH_RADIO5GL_BSSINFO_ENTRY_NODE_FORMAT))
	{
		info->band     = WIFI_5G;
		p = bssinfo_path + strlen(MESH_RADIO5GL_BSSINFO_ENTRY_NODE_FORMAT);
		info->ssid_idx  = atoi(p);
		snprintf(info->path, sizeof(info->path), "%s.%d", WLAN11AC_ENTRY_NODE, info->ssid_idx);
	}
	else if(NULL != strstr(bssinfo_path, MESH_RADIO5GH_BSSINFO_ENTRY_NODE_FORMAT))
	{
		/*reserve*/
		return -1;
	}
	else
	{
		return -1;
	}

	return 0;
}

/******************************dat restart app********************************************/
static const wifi_attr_fun wlan_mesh_dat_tabl[] = 
{
	{MAP_ENABLE_ATTR,		wifimgr_lib_set_MapEnable},
	{MAP_TRURNKEY_ATTR,		wifimgr_lib_set_MAP_Turnkey},
	{MAP_EXT_ATTR,			wifimgr_lib_set_MAP_Ext},
#if defined(TCSUPPORT_MAP_R2)
	{MAP_MODE_ATTR, wifimgr_lib_set_MapMode},
#endif

};
#define WLAN_MESH_DAT_TBL_SIZE  (sizeof(wlan_mesh_dat_tabl) / sizeof(wlan_mesh_dat_tabl[0]))

#if defined(TCSUPPORT_CT_JOYME4)
static int update_mesh_enable_conf(unsigned char conf_commit)
{
	char buf[64];
	unsigned char map_enable = 0;
	unsigned char bs20_enable = 0;
	unsigned char bandsteering_enable = 0;
	unsigned char easymesh_enable = 0;
	unsigned int enable = 0;
	unsigned char commit = 0;
	
	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(MESH_ENABLE_CONF_NODE, EASYMESH_ENABLE_ATTR, buf, sizeof(buf));
	easymesh_enable = atoi(buf);
	if(easymesh_enable)
	{
		easymesh_enable = 1;
	}
	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(MESH_ENABLE_CONF_NODE, BAND_STEERING_ENABLE_ATTR, buf, sizeof(buf));
	bandsteering_enable = atoi(buf);
	if(bandsteering_enable)
	{
		bandsteering_enable = 1;
	}
	enable = ((easymesh_enable << 1) | bandsteering_enable);
	switch (enable)
	{
		case 1:
		{
			easymesh_enable 	= 0;
			bandsteering_enable = 1;
			break;
		}
		case 2:
		case 3:
		{
			easymesh_enable 	= 1;
			bandsteering_enable = 0;
			break;
		}
		default:
		{
			easymesh_enable 	= 0;
			bandsteering_enable = 0;
			break;
		}
	}

	if(conf_commit)
	{
		/*avoid duplicate operation*/
		memset(buf, 0, sizeof(buf));
		cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, buf, sizeof(buf));
		map_enable = atoi(buf);
		memset(buf, 0, sizeof(buf));
		cfg_get_object_attr(MESH_DAT_NODE, BS20_ENABLE_ATTR, buf, sizeof(buf));
		bs20_enable = atoi(buf);
		commit = ((easymesh_enable ^ map_enable) || (bandsteering_enable ^ bs20_enable));
		if(!commit)
		{
			return 0;
		}
	}

	if(easymesh_enable)
	{
		cfg_set_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR,   "1");
		cfg_set_object_attr(MESH_DAT_NODE, MAP_TRURNKEY_ATTR, "1");
	}
	else
	{
		cfg_set_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR,   "0");
		cfg_set_object_attr(MESH_DAT_NODE, MAP_TRURNKEY_ATTR, "0");
	}
	
	if(bandsteering_enable)
	{
		cfg_set_object_attr(MESH_DAT_NODE, BS20_ENABLE_ATTR,  "1");
	}
	else
	{
		cfg_set_object_attr(MESH_DAT_NODE, BS20_ENABLE_ATTR,  "0");
	}

	return 1;
}
#endif

#if defined(TCSUPPORT_MAP_R2)
static int update_mesh_map_mode_conf()
{
	char tmp[10] = {0};
	memset(tmp, 0, sizeof(tmp));
	cfg_get_object_attr(MESH_DAT_NODE, "MapEnable", tmp, sizeof(tmp));
	if(1 == atoi(tmp)){
		cfg_set_object_attr(MESH_DAT_NODE,	"MapMode", "1");
	}else{
		memset(tmp, 0, sizeof(tmp));
		cfg_get_object_attr(MESH_DAT_NODE, "BS20Enable", tmp, sizeof(tmp));
		if(1 == atoi(tmp)){
			cfg_set_object_attr(MESH_DAT_NODE,	"MapMode", "2");
		}else{
			cfg_set_object_attr(MESH_DAT_NODE,	"MapMode", "0");
		}
	}	
}
#endif

/*
*/
static int config_mesh_dat_attr(wifi_type band)
{
	char path[64];
	char buf[128];
	char itf_name[IFNAMSIZ];
	int bssid_num = 0;
	int i = 0;
	int itf_up = 0;
	int map_enable = 0;
	memset(path, 0, sizeof(path));
	switch(band) 
	{
		case WIFI_2_4G:
		{
			snprintf(path, sizeof(path), WLAN_COMMON_NODE);
			break;
		}
		case WIFI_5G:
		{
			snprintf(path, sizeof(path), WLAN11AC_COMMON_NODE);
			break;
		}
		default:
		{
			return -1;
		}
	}

	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(path, "BssidNum", buf, sizeof(buf)) < 0)
	{
		return -1;
	}
	bssid_num = atoi(buf);
	for(i=0; i<bssid_num; i++)
	{
		memset(itf_name, 0, sizeof(itf_name));
		if(WIFI_2_4G == band)
		{
			snprintf(itf_name, sizeof(itf_name), "%s%d", WLAN_ITF_NAME_FORMAT, i);
		}
		else
		{
			snprintf(itf_name, sizeof(itf_name), "%s%d", WLAN_AC_ITF_NAME_FORMAT, i);
		}
		itf_up = test_itf_status(itf_name, IFF_UP);
		if(itf_up)
		{
			break;
		}
	}
	if(!itf_up)
	{
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, buf, sizeof(buf));
	map_enable = atoi(buf);
	if(map_enable)
	{
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_set_WIFI_CMD(itf_name, -1, sizeof(buf), "mapEnable=1", buf, 0);
		system(buf);
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_set_WIFI_CMD(itf_name, -1, sizeof(buf), "mapTurnKey=1", buf, 0);
		system(buf);
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_set_WIFI_CMD(itf_name, -1, sizeof(buf), "mapEnable=1", buf, 0);
		system(buf);
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_set_WIFI_CMD(itf_name, -1, sizeof(buf), "mapTurnKey=1", buf, 0);
		system(buf);
	}
		
	return 0;
}

int update_mesh_dat(wifi_type band)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	int dat_tbl_size = 0;
	unsigned char value[512];
	char tmp_value[512];
	char path[64];
	char bss_value = 0;
	char Bssid_Num[4]	= {0};
	dat_tbl_size = WLAN_MESH_DAT_TBL_SIZE;
	
#if defined(TCSUPPORT_CT_JOYME4)
	update_mesh_enable_conf(FALSE);
#endif

#if defined(TCSUPPORT_MAP_R2)
	update_mesh_map_mode_conf();
#endif

	for(i = 0; i<dat_tbl_size; i++)
	{
		memset(value, 0, sizeof(value));
		memset(path,  0, sizeof(path));
		ret = cfg_get_object_attr(MESH_DAT_NODE, wlan_mesh_dat_tabl[i].attr_name, value, sizeof(value));
		if(0 >= ret)
		{
			continue;
		}
		if(strcmp(wlan_mesh_dat_tabl[i].attr_name,MAP_EXT_ATTR) == 0){
			memset(value, 0, sizeof(value));
			if (band == WIFI_2_4G)
			{
				cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", Bssid_Num, sizeof(Bssid_Num));
				for(j = 0; j<atoi(Bssid_Num); j++)
				{
					snprintf(path, sizeof(path), MESH_RADIO2G_BSSINFO_ENTRY_NODE,j+1);
					memset(tmp_value, 0, sizeof(tmp_value));
					bss_value = 0;
					cfg_get_object_attr(path, MAP_BSSINFO_FRONT_HAUL_ATTR, tmp_value, sizeof(tmp_value));
					if(atoi(tmp_value) == 1)
						bss_value |= 1 << MAP_ROLE_FRONTHAUL_BSS;
					memset(tmp_value, 0, sizeof(tmp_value));
					cfg_get_object_attr(path, MAP_BSSINFO_BACK_HAUL_ATTR, tmp_value, sizeof(tmp_value));
					if(atoi(tmp_value) == 1)
						bss_value |= 1 << MAP_ROLE_BACKHAUL_BSS;					
					snprintf(value+strlen(value), sizeof(value), "%d;", bss_value);					
				}
			}
			else if (band == WIFI_5G)
			{
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", Bssid_Num, sizeof(Bssid_Num));
				for(j = 0; j<atoi(Bssid_Num); j++)
				{
					snprintf(path, sizeof(path), MESH_RADIO5GL_BSSINFO_ENTRY_NODE,j+1);
					memset(tmp_value, 0, sizeof(tmp_value));
					bss_value = 0;
					cfg_get_object_attr(path, MAP_BSSINFO_FRONT_HAUL_ATTR, tmp_value, sizeof(tmp_value));
					if(atoi(tmp_value) == 1)
						bss_value |= 1 << MAP_ROLE_FRONTHAUL_BSS;
					memset(tmp_value, 0, sizeof(tmp_value));
					cfg_get_object_attr(path, MAP_BSSINFO_BACK_HAUL_ATTR, tmp_value, sizeof(tmp_value));
					if(atoi(tmp_value) == 1)
						bss_value |= 1 << MAP_ROLE_BACKHAUL_BSS;					
					snprintf(value+strlen(value), sizeof(value), "%d;", bss_value);
				}
			}
		}
		if(NULL != wlan_mesh_dat_tabl[i].wifi_api)
		{
			ret = wlan_mesh_dat_tabl[i].wifi_api(band, -1, value);
			if(ECNT_WIFIMGR_SUCCESS != ret)
			{
				tcdbg_printf("func = %s, line = %d, attr = %s\n", __func__, __LINE__, wlan_mesh_dat_tabl[i].attr_name);
				return -1;
			}
		}
	}
	config_mesh_dat_attr(band);
	
	return 0;
}

static void down_itf(void)
{
	char APOn[8];
	char Bssid_Num[8];
	char ssid_enable[8];
	char cmd_buf[128];
	char node_name[64];
	int i = 0;
	int APON_24G = 0;
	int APON_5G = 0;
	int BssidNum_24G = 0;
	int BssidNum_5G = 0;
	
	memset(APOn, 0, sizeof(APOn));
	memset(Bssid_Num, 0, sizeof(Bssid_Num));
	cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", APOn, sizeof(APOn));
	cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", Bssid_Num, sizeof(Bssid_Num));
	APON_24G     = atoi(APOn);
	BssidNum_24G = atoi(Bssid_Num);
	
	memset(APOn, 0, sizeof(APOn));
	memset(Bssid_Num, 0, sizeof(Bssid_Num));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", APOn, sizeof(APOn));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", Bssid_Num, sizeof(Bssid_Num));
	APON_5G    = atoi(APOn);
	BssidNum_5G = atoi(Bssid_Num);

	/*down itf */
	if(1 == APON_24G)
	{	
		for(i = 0; i < BssidNum_24G; i++)
		{
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "%s.%d", WLAN_ENTRY_NODE, i+1);
			memset(ssid_enable, 0, sizeof(ssid_enable));
			if(cfg_get_object_attr(node_name, "EnableSSID", ssid_enable, sizeof(ssid_enable)) > 0)
			{
				if(atoi(ssid_enable) == 1)
				{
					memset(cmd_buf, 0, sizeof(cmd_buf));
					snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig ra%d down", i);
					system(cmd_buf);
				}
			}
		}
		/*down apclint*/
		memset(cmd_buf, 0, sizeof(cmd_buf));
		snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig apcli0 down");
		system(cmd_buf);
	}
	
	if(1 == APON_5G)
	{	
		for(i = 0; i < BssidNum_5G; i++)
		{
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "%s.%d", WLAN11AC_ENTRY_NODE, i+1);
			memset(ssid_enable, 0, sizeof(ssid_enable));
			if(cfg_get_object_attr(node_name, "EnableSSID", ssid_enable, sizeof(ssid_enable)) > 0)
			{
				if(atoi(ssid_enable) == 1)
				{
					memset(cmd_buf, 0, sizeof(cmd_buf));
					snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig rai%d down", i);
					system(cmd_buf);
				}
			}
		}
		
		memset(cmd_buf, 0, sizeof(cmd_buf));
		snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig apclii0 down");
		system(cmd_buf);
	}

	return ;
}

static void up_apclient_itf(void)
{
	char cmd_buf[128];

	memset(cmd_buf, 0, sizeof(cmd_buf));
	snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig apcli0 up");
	system(cmd_buf);
	
	memset(cmd_buf, 0, sizeof(cmd_buf));
	snprintf(cmd_buf, sizeof(cmd_buf), "/sbin/ifconfig apclii0 up");
	system(cmd_buf);
	return ;
}

int wlan_mesh_update_dat(void)
{
	wlan_evt_t evt;
	int ret = 0;
	
	down_itf();
	
	memset(&evt, 0, sizeof(evt));
	snprintf(evt.buf, sizeof(evt), "%s", WLAN_NODE);
	svc_wlan_handle_event_update(&evt);

	memset(&evt, 0, sizeof(evt));
	snprintf(evt.buf, sizeof(evt), "%s", WLAN11AC_NODE);
	svc_wlan_handle_event_update(&evt);	

#if defined(TCSUPPORT_NP)
	up_apclient_itf();
#endif

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH)
		memset(&evt, 0, sizeof(evt));
		snprintf(evt.buf, sizeof(evt), "%s", WLAN_NODE);
		cfg_send_event(EVT_WLAN_EXTERNAL, EVT_WLAN_TRIGGER_MESH_UPDATE, (void *)&evt , sizeof(wlan_evt_t));
#endif

	return 0;
}

/******************************mesh renew only write dat********************************************/
static const wifi_attr_fun wlan_mesh_renew_attr_tbl[] = 
{
	{WLAN_ENTRY_SSID_ATTR,         wifimgr_lib_set_ssid},
	{WLAN_ENTRY_AUTH_MODE_ATTR,    wifimgr_lib_set_authMode},
	{WLAN_ENTRY_ENCRYPT_TYPE_ATTR, wifimgr_lib_set_encrypType},
	{WLAN_ENTRY_KEY_ATTR,          wifimgr_lib_set_WPAPSK},
	{WLAN_ENTRY_HIDDEN_SSID_ATTR,  wifimgr_lib_set_hideSSID},
};
#define WLAN_MESH_RENEW_ATTR_TBL_SIZE  (sizeof(wlan_mesh_renew_attr_tbl) / sizeof(wlan_mesh_renew_attr_tbl[0]))

int wlan_mesh_renew_bssinfo(struct host_service_pbuf* pbuf)
{
	int ret = 0;
	char path[64];
	char value[1024];
	int i = 0;
	int tbl_size = 0;
	pt_mesh_evt p_evt_data = NULL;
	wlan_mesh_path_info_t path_info;

	p_evt_data = (pt_mesh_evt)svchost_get_event_val(pbuf);
	if(NULL == p_evt_data)
	{
		return -1;
	}
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", p_evt_data->buf);
	memset(&path_info, 0, sizeof(path_info));
	ret = map_bssinfo_wlan_entry(path, &path_info);
	tcdbg_printf("func = %s, line = %d, path = %s, ssid = %d, band = %d\n", \
		__func__, __LINE__, path_info.path, path_info.ssid_idx, path_info.band);
	if(0 != ret)
	{
		return -1;
	}

	tbl_size = WLAN_MESH_RENEW_ATTR_TBL_SIZE;
	for(i=0; i<tbl_size; i++)
	{
		memset(value, 0, sizeof(value));
		ret = cfg_get_object_attr(path_info.path, wlan_mesh_renew_attr_tbl[i].attr_name, value, sizeof(value));
		if(0 >= ret)
		{
			tcdbg_printf("func = %s, line = %d, path = %s, attr_name = %s\n", \
				__func__, __LINE__, path_info.path, wlan_mesh_renew_attr_tbl[i].attr_name);
			continue;
		}

		if(NULL != wlan_mesh_renew_attr_tbl[i].wifi_api)
		{
			ret = wlan_mesh_renew_attr_tbl[i].wifi_api(path_info.band, path_info.ssid_idx, value);
			if(ECNT_WIFIMGR_SUCCESS != ret)
			{
				tcdbg_printf("func = %s, line = %d, attr = %s\n", __func__, __LINE__, wlan_mesh_renew_attr_tbl[i].attr_name);
				return -1;
			}
		}
	}

	ret = wifimgr_lib_save_dataFile(path_info.band);
	if(ECNT_WIFIMGR_SUCCESS != ret)
	{
		return -1;
	}
	return 0;
}
/******************************gener ra/rai interface mac********************************************/
int config_itf_mac_dat(wifi_type band)
{
	int ret = 0;
	char value[64];
	unsigned char br_itf_name[IFNAMSIZ];
	unsigned char mac[ETH_ALEN];
	unsigned char def_mac[] = {0x00,0xaa,0xbb,0x01,0x23,0x45};
	
	/*get br_itf name*/
	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_MAP_CFG_NODE, MAP_BR_ITF_NAME_ATTR, value, sizeof(value));
	snprintf(br_itf_name, sizeof(br_itf_name), value);
	/*get br_itf mac*/
	memset(mac, 0, sizeof(mac));
	wifimgr_lib_get_interface_mac(br_itf_name, mac);

	if(is_zero_ether_addr(mac))
	{
		memcpy(mac, def_mac, ETH_ALEN);
	}
	
	/*get br_itf name*/
	switch(band)
	{
		case WIFI_2_4G:
		{
			mac[5] = mac[5] + 0x04;
			break;
		}
		case WIFI_5G:
		{
			mac[5] = mac[5] + 0x05;
			break;
		}
		default:
		{
			return -1;
		}
	}
	
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), ADDR_FMT_STR, ADDR_FMT_ARS(mac));
	ret = wifimgr_lib_set_MacAddress(band, -1, value);
	if(ECNT_WIFIMGR_SUCCESS != ret)
	{
		return -1;
	}
	
	return 0;
}

