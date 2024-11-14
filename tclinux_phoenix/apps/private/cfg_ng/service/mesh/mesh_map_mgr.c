/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <utility.h>
#include <svchost_api.h>
#include "mesh_map_cfg.h"
#include "mesh_map_file_io.h"
#include "mesh_map_common.h"
#include "mesh_action.h"
#include "mesh_map_mgr.h"
#include "libapi_lib_wifimgr.h"

#if defined(TCSUPPORT_MAP_R2)
void disable_map_r2_config(){
	char cmd[64];

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapR2Enable=0", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapR2Enable=0", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapTSEnable=0", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapTSEnable=0", cmd, 0);
	system(cmd);
}

void disable_map_config(){
	char cmd[64];
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapEnable=0", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapEnable=0", cmd, 0);
	system(cmd);

}

void enable_map_config(){
	char cmd[64];
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapEnable=1", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapEnable=1", cmd, 0);
	system(cmd);

}

void enable_bs20_config(){
	char cmd[64];
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapEnable=2", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapEnable=2", cmd, 0);
	system(cmd);

}

void enable_map_r2_config(){
	char cmd[64];

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapR2Enable=1", cmd, 0);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapR2Enable=1", cmd, 0);
	system(cmd);
}

void traffic_separation_init(){
	char cmd[64];

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "VLANEn=0", cmd, 0);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "VLANEn=0", cmd, 0);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "mapTSEnable=1", cmd, 0);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "mapTSEnable=1", cmd, 0);
	system(cmd);

}

#endif

static int exit_mesh_map(void);

static void mesh_mgr_send_evt(int type, int evt_id, char *buf)
{
	mesh_evt_t evt;
	char evt_type[EVT_TYPE_LENGTH];

	memset(&evt, 0, sizeof(evt));
	if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
	{
		return ;
	}

	snprintf(evt.buf, sizeof(evt.buf), buf);
	memset(evt_type, 0, sizeof(evt_type));
	switch(type)
	{
		case EVT_MESH_INTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_MESH_INTERNAL);
			break;
		}
		case EVT_MESH_EXTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_MESH_EXTERNAL);
			break;
		}
		default:
		{
			return;
		}
	}

	cfg_send_event(evt_type, evt_id, (void *)&evt , sizeof(mesh_evt_t));
	
	return ;
}

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
			svc_mesh_printf("func = %s, line = %d\n", __func__, __LINE__);
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

int update_dat_evt_handle(void)
{
	char buf[64];
	unsigned char map_enable = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	int commit = 0;
	commit = update_mesh_enable_conf(TRUE);
	if(!commit)
	{
		return 0;
	}
#endif	
	internetRouteWanExist();

	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, buf, sizeof(buf));
	map_enable = atoi(buf);
	if(!map_enable)
	{
		exit_mesh_map();
		cfg_set_object_attr(MESH_DAT_NODE, MAP_TRURNKEY_ATTR, "0");
#if !defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
		apclient_up_down(OP_DOWN);
#endif
	}
	else
	{
		cfg_set_object_attr(MESH_DAT_NODE, MAP_TRURNKEY_ATTR, "1");
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%s", MESH_DAT_NODE);
	mesh_mgr_send_evt(EVT_MESH_EXTERNAL_TYPE, EVT_UPDATE_DAT, buf);

	return 0;    
}

int mesh_boot(void)
{
	int ret = 0;
	unsigned char value[64];
	unsigned char AL_mac[ETH_ALEN];

	/*AL-MAC*/
	memset(AL_mac, 0, sizeof(AL_mac));
	ret = generate_1905_AL_mac(AL_mac);
	if(0 != ret)
	{
		SVC_MESH_DEBUG_INFO("generate AL-MAC fail\n");
		return -1;
	}

	SVC_MESH_DEBUG_INFO("AL_mac = "MAP_ADDR_FMT_STR"\n", MAP_ADDR_FMT_ARS(AL_mac));
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), MAP_ADDR_FMT_STR, MAP_ADDR_FMT_ARS(AL_mac));
	cfg_set_object_attr(MESH_MAP_CFG_NODE, MAP_AL_MAC_ATTR, value);
	cfg_set_object_attr(MESH_COMMON_NODE, MAP_REINIT_WIFI_FLAG, MESH_BLOCK_PAGE_END);
	memset(value, 0, sizeof(value));	
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value,sizeof(value));
	if (MAP_CONTROLLER == atoi(value))
	{
		cfg_set_object_attr(MESH_MAPD_CFG_NODE, MAP_DHCP_CTL_ATTR, MAP_DHCP_CTL_DISABLE);
	}
	else
	{
		cfg_set_object_attr(MESH_MAPD_CFG_NODE, MAP_DHCP_CTL_ATTR, MAP_DHCP_CTL_ENABLE);
	}	
	
	clear_mesh_action();
		
	return 0;
}

static int exit_mesh_map(void)
{
	char buf[256];
	unlink(WAPP_FILE_CMD);
	unlink(WAPP_SERVER_FILE_CMD);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 bs20");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 mapd");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 p1905_managerd");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 wapp");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);
	
	return 0;
}

static int write_file_mesh_init(void)
{
	int ret = 0;
	mesh_config_bss_info  wts_info[MAX_BSS_INFO_NUM];

	memset(&wts_info, 0, sizeof(wts_info));
	ret = load_wts_bss_info(wts_info, MAX_BSS_INFO_NUM);
	if(0 != ret)
	{
		return ret;
	}
	
	ret = write_file_mesh_wapp_cfg();
	
	ret = write_file_mesh_map_cfg();
	if(0 != ret)
	{
		return ret;
	}
	ret = write_file_mesh_mapd_cfg();
	if(0 != ret)
	{
		return ret;
	}
	ret = write_file_mesh_steer_cfg();
	if(0 != ret)
	{
		return ret;
	}

	ret = write_file_mesh_wapp_cfg();

	ret = write_file_mesh_wts_bss_info_config(wts_info, MAX_BSS_INFO_NUM);
	if(0 != ret)
	{
		return ret;
	}
	
	return 0;
}

int restart_map(void)
{
	int   i = 0;
	int ret = 0;
	char value[64];
	char apclibhPath[32];
	unsigned char role = 0;
	unsigned char map_enable = 0;
	unsigned char map_mode = 0;
	unsigned char bs20_enable = 0;
	char cmd[128] = {0};

#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
	unsigned char reinit_flag = 0;

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_REINIT_FLAG_ATTR, value, sizeof(value));
	reinit_flag = atoi(value);

	if (reinit_flag)
	{
		cfg_set_object_attr(MESH_COMMON_NODE, MAP_REINIT_FLAG_ATTR, "0");
		cfg_evt_write_romfile_to_flash();
	}
#endif

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_CLEAN_APCLIBH_FLAG, value, sizeof(value));
	if( 1 == atoi(value) )
	{
		for (i = 0; i < MAX_NUM_OF_RADIO; i++)
		{
			memset(apclibhPath, 0, sizeof(apclibhPath));
			snprintf(apclibhPath, sizeof(apclibhPath), MESH_APCLIBH_ENTRY_NODE, i + 1);
			if(0 <= cfg_query_object(apclibhPath,NULL,NULL))
			{
				cfg_delete_object(apclibhPath);
			}

		}
		cfg_set_object_attr(MESH_COMMON_NODE, MAP_CLEAN_APCLIBH_FLAG, "0");
	}

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, value, sizeof(value));
	map_enable = atoi(value);
	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_DAT_NODE, BS20_ENABLE_ATTR, value, sizeof(value));
	bs20_enable = atoi(value);

#if defined(TCSUPPORT_MAP_R2)
		if(map_enable || bs20_enable)
		{
		cfg_set_object_attr(MESH_COMMON_NODE, MAP_RESTART_FLAG, "1");
		}
#endif
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "brctl stp br0 off");
	system(cmd);

	exit_mesh_map();
#if defined(TCSUPPORT_MAP_R2)
	disable_map_r2_config();
#endif
	if(map_enable || bs20_enable)
	{
		cfg_set_object_attr(MESH_COMMON_NODE, MAP_REINIT_WIFI_FLAG, MESH_BLOCK_PAGE_SATRT);
		ret = write_file_mesh_init();
		if(0 != ret)
		{
			return ret;
		}
#if defined(TCSUPPORT_MAP_R2)
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(MESH_DAT_NODE, MAP_MODE_ATTR, value, sizeof(value));
		map_mode = atoi(value);

		if(2 == map_mode){
			enable_bs20_config();
		}else if(1 == map_mode){
			enable_map_config();
			memset(value, 0, sizeof(value));
			cfg_get_object_attr(MESH_MAP_CFG_NODE, "map_ver", value, sizeof(value));
			if(!strcmp(value, "R2")){
				enable_map_r2_config();
				memset(value, 0, sizeof(value));
				cfg_get_object_attr(MESH_DAT_NODE, MAP_TS_ENABLE_ATTR, value, sizeof(value));
				if(1 == atoi(value)){
					traffic_separation_init();
				}
			}
		}
#endif	

#if !defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
		role = dev_roller(atoi(value));
		if (MAP_CONTROLLER == atoi(value))
		{
			apclient_up_down(OP_DOWN);
		}
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
		else if((MAP_AGENTER == role) || (MAP_ROLE_UNCONFIGURED == role))
#else
		else if (MAP_AGENTER == atoi(value))
#endif
		{
			apclient_up_down(OP_UP);			
		}
#endif
		init_wapp();
	}
	else
	{
#if defined(TCSUPPORT_MAP_R2)
		disable_map_config();
#endif
		set_map_allow_pkt_proc(ALLOW_ALL_1905_PKT);
#if (defined(TCSUPPORT_NP) && !defined(TCSUPPORT_MAP_R2))
		apclient_up_down(OP_UP);
#elif !defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
		apclient_up_down(OP_DOWN);
#endif
	}
	
	return ret;
}

int mesh_debug(struct host_service_pbuf* pbuf)
{
	int level = 0;
	pt_mesh_evt p_evt_data = NULL;

	p_evt_data = (pt_mesh_evt)svchost_get_event_val(pbuf);
	if(NULL == p_evt_data)
	{
		return -1;
	}
	level = atoi(p_evt_data->buf);

	svc_mesh_debug_level(level);

	return 0;
}

int mesh_renew_bssinfo(struct host_service_pbuf* pbuf)
{
	char path[64];
	char value[64];
	int reinit_flag = 0;
	int ret = 0;
	pt_mesh_evt p_evt_data = NULL;
	mesh_config_bss_info  wts_info[MAX_BSS_INFO_NUM];

	p_evt_data = (pt_mesh_evt)svchost_get_event_val(pbuf);
	if(NULL == p_evt_data)
	{
		return -1;
	}
	
	if(0 >= cfg_get_object_attr(MESH_COMMON_NODE, MAP_REINIT_WIFI_FLAG, value, sizeof(value)))
	{
		return -1;
	}
	reinit_flag = atoi(value);
	if(0 != reinit_flag)
	{
		return -1;		
	}
	
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", p_evt_data->buf);

	SVC_MESH_DEBUG_INFO("path = %s\n", path);
	mesh_mgr_send_evt(EVT_MESH_EXTERNAL_TYPE, EVT_RENEW_BSSINFO, path);

	memset(&wts_info, 0, sizeof(wts_info));
	ret = load_wts_bss_info(wts_info, MAX_BSS_INFO_NUM);
	if(0 != ret)
	{
		return ret;
	}

	ret = write_file_mesh_wts_bss_info_config(wts_info, MAX_BSS_INFO_NUM);
	if(0 != ret)
	{
		return ret;
	}

	/*renew action*/
	ret = bss_config_renew();
	if(0 != ret)
	{
		SVC_MESH_ERROR_INFO("renew bss config faile\n");
		return ret;
	}

	return 0;
}

