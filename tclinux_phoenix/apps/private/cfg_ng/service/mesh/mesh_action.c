/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cfg_api.h>
#include <utility.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "mesh_map_cfg.h"
#include "mesh_map_common.h"
#include "mesh_action.h"
#include <mapd_interface_ctrl.h>

#define ACTION_SUCCESS 			0
/**********************************************************************************/
static int trigger_ether_onboarding(void);
static int trigger_wifi_onboarding(void);
static int select_best_ap(void);
static void load_default_settings_value(mesh_name_value_attr *MeshDefSetValList);
static void load_default_settings(void);
static int mandate_steer(void);
static int bh_steer(void);
static int trigger_map_wps(void);
/**********************************************************************************/
int trigger_restart_map(void)
{
	mesh_send_evt(EVT_MESH_EXTERNAL_TYPE, EVT_TRIGGER_MESH_UPDATE, "root.mesh");
	return 0;
}

/**********************************************************************************/
mesh_name_value_attr	MeshDefSetValList[] =
{
	{MESH_COMMON_NODE, 	MAP_DEV_ROLLER_ATTR},
	{MESH_COMMON_NODE, 	MAP_STEER_ENABLE_ATTR},
	{MESH_COMMON_NODE, 	MAP_CH_PLANNING_ENABLE_ATTR},
	{MESH_COMMON_NODE, 	MAP_TRIGGER_FLAG},
	
	{MESH_MAPD_CFG_NODE, 	MAP_AP_STEER_RSSI_TH_ATTR},
	{MESH_MAPD_CFG_NODE, 	MAP_BH_PRIORITY_2G_ATTR},
	{MESH_MAPD_CFG_NODE, 	MAP_BH_PRIORITY_5GL_ATTR},
	{MESH_MAPD_CFG_NODE, 	MAP_BH_PRIORITY_5GH_ATTR},
	
	{MESH_STEER_CFG_NODE, 	MAP_CU_OVERLOAD_TH_2G_ATTR},
	{MESH_STEER_CFG_NODE, 	MAP_CU_OVERLOAD_TH_5GL_ATTR},
	{MESH_STEER_CFG_NODE, 	MAP_CU_OVERLOAD_TH_5GH_ATTR},
	{NULL,NULL}
};

mesh_name_value_attr	MeshActionParamListTable[] =
{
	{MAP_ACTION_PARA_STA_MAC_ATTR,					MAP_ACTION_TRIGGER_MANDATE_STEERING_ON_AGENT},
	{MAP_ACTION_PARA_TARGET_BSSID_ATTR,				MAP_ACTION_TRIGGER_MANDATE_STEERING_ON_AGENT},
	{MAP_ACTION_PARA_BACK_HAUL_MAC_ATTR,			MAP_ACTION_TRIGGER_BACKHAUL_STEERING},
	{MAP_ACTION_PARA_BACK_HAUL_TARGET_BSSID_ATTR,	MAP_ACTION_TRIGGER_BACKHAUL_STEERING},
	{MAP_ACTION_PARA_FRONT_HAUL_MAC_ATTR,			MAP_ACTION_TRIGGER_WPS_AT_FRONTHAUL_BSS_OF_AN_AGENT},
	{MAP_BH_PRIORITY_2G_ATTR,							MAP_ACTION_APPLY_BH_PRIORITY_ATTR},
	{MAP_BH_PRIORITY_5GL_ATTR,							MAP_ACTION_APPLY_BH_PRIORITY_ATTR},
	{MAP_BH_PRIORITY_5GH_ATTR,							MAP_ACTION_APPLY_BH_PRIORITY_ATTR},
#if defined(TCSUPPORT_ECNT_MAP_ENHANCE) || (TCSUPPORT_EASYMESH_R13) || defined(TCSUPPORT_MAP_R2)
	{MAP_ACTION_PARA_CHANNEL_PLANNING_CHANNEL_ATTR,		MAP_ACTION_TRIGGER_CHANNEL_PLANNING_ATTR},
	{MAP_ACTION_PARA_CHANNEL_FORCE_AL_MAC_ATTR,			MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR},
	{MAP_ACTION_PARA_CHANNEL_FORCE_2G_CHANNEL_ATTR,		MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR},
	{MAP_ACTION_PARA_CHANNEL_FORCE_5GL_CHANNEL_ATTR,	MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR},
	{MAP_ACTION_PARA_CHANNEL_FORCE_5GH_CHANNEL_ATTR,	MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR},
#endif
	{NULL,NULL}	
};


/**********************************************************************************/
static int trigger_ether_onboarding(void)
{
	int ret = 0;
	
	ret = mapd_interface_trigger_onboarding(NULL, "0");
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("trigger ether onboarding faile\n");
		return -1;
	}
	
	return 0;
}

static int trigger_wifi_onboarding(void)
{
	int ret = 0;
	
	ret = mapd_interface_trigger_onboarding(NULL, "1");
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("trigger wifi onboarding faile\n");
		return -1;
	}
	
	return 0;
}
/**********************************************************************************/
int bss_config_renew(void)
{
	int ret = 0;
	struct mapd_interface_ctrl *ctrl = NULL;

	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(NULL == ctrl)
	{
		return -1;
	}

	ret = mapd_interface_set_renew(ctrl);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("renew faile\n");
	}
	
	mapd_interface_ctrl_close(ctrl);

	return ret;
	
}

/**********************************************************************************/
static int select_best_ap(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("open file faile\n");
		return -1;
	}
	
	ret = mapd_interface_select_best_ap(ctrl);
	SVC_MESH_DEBUG_INFO(" success, ret = [%d]\n", ret);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("trigger onboarding faile\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
static void load_default_settings_value(mesh_name_value_attr *MeshDefSetValList)
{
	char val[32];
	int i = 0;
	char cmd[128];

	SVC_MESH_DEBUG_INFO(" start\n");
	while(NULL != MeshDefSetValList[i].attr_name)
	{
		memset(val, 0, sizeof(val));
		cfg_get_object_attr(MESH_DEFSETTING_NODE, MeshDefSetValList[i].attr_value, val, sizeof(val));
		cfg_set_object_attr(MeshDefSetValList[i].attr_name, MeshDefSetValList[i].attr_value, val);
		i++;
	}
	
	internetRouteWanExist();

	cfg_set_object_attr(MESH_COMMON_NODE, MAP_CLEAN_APCLIBH_FLAG, "1");

	return;
}

static void load_default_settings(void)
{
	SVC_MESH_DEBUG_INFO("start\n");
	apclient_up_down(OP_DOWN);
	load_default_settings_value(MeshDefSetValList);
	trigger_restart_map();

	return;
}
/**********************************************************************************/
int set_channel_utilization_threshold(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char utilTh2G[32];
	char utilTh5GL[32];
	char utilTh5GH[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("open file faile\n");
		return -1;
	}
	
	memset(utilTh2G, 0, sizeof(utilTh2G));
	memset(utilTh5GL, 0, sizeof(utilTh5GL));
	memset(utilTh5GH, 0, sizeof(utilTh5GH));
	
	cfg_get_object_attr(MESH_STEER_CFG_NODE, MAP_CU_OVERLOAD_TH_2G_ATTR, utilTh2G, sizeof(utilTh2G));
	cfg_get_object_attr(MESH_STEER_CFG_NODE, MAP_CU_OVERLOAD_TH_5GL_ATTR, utilTh5GL, sizeof(utilTh5GL));
	cfg_get_object_attr(MESH_STEER_CFG_NODE, MAP_CU_OVERLOAD_TH_5GH_ATTR, utilTh5GH, sizeof(utilTh5GH));	
	SVC_MESH_DEBUG_INFO("utilTh2G = [%s], utilTh5GL = [%s], utilTh5GH = [%s]\n", utilTh2G, utilTh5GL, utilTh5GH);
	ret = mapd_interface_set_ChUtil_thresh(ctrl, utilTh2G, utilTh5GL, utilTh5GH);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
int set_rssi_thresh(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char rssi[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(rssi, 0, sizeof(rssi));
	cfg_get_object_attr(MESH_MAPD_CFG_NODE, MAP_AP_STEER_RSSI_TH_ATTR, rssi, sizeof(rssi));
	SVC_MESH_DEBUG_INFO("rssi = [%s]\n", rssi);
	ret = mapd_interface_set_rssi_thresh(ctrl, rssi);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
static int mandate_steer(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char mac_addr[32];
	char bssid[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(mac_addr, 0, sizeof(mac_addr));
	memset(bssid, 0, sizeof(bssid));
	
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_STA_MAC_ATTR, mac_addr, sizeof(mac_addr));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_TARGET_BSSID_ATTR, bssid, sizeof(bssid));
	SVC_MESH_DEBUG_INFO("mac_addr = [%s], bssid = [%s]\n", mac_addr, bssid);
	ret = mapd_interface_mandate_steer(ctrl, mac_addr, bssid);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
static int bh_steer(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char bh_mac_addr[32];
	char bh_target_mac_addr[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(bh_mac_addr, 0, sizeof(bh_mac_addr));
	memset(bh_target_mac_addr, 0, sizeof(bh_target_mac_addr));
	
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_BACK_HAUL_MAC_ATTR, bh_mac_addr, sizeof(bh_mac_addr));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_BACK_HAUL_TARGET_BSSID_ATTR, bh_target_mac_addr, sizeof(bh_target_mac_addr));
	SVC_MESH_DEBUG_INFO("bh_mac_addr = [%s], bh_target_mac_addr = [%s]\n", bh_mac_addr, bh_target_mac_addr);
	ret = mapd_interface_bh_steer(ctrl, bh_mac_addr, bh_target_mac_addr);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
static int trigger_map_wps(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char fh_mac_addr[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(fh_mac_addr, 0, sizeof(fh_mac_addr));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_FRONT_HAUL_MAC_ATTR, fh_mac_addr, sizeof(fh_mac_addr));
	SVC_MESH_DEBUG_INFO("fh_mac_addr = [%s]\n", fh_mac_addr);
	ret = mapd_interface_trigger_map_wps(ctrl, fh_mac_addr);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
/**********************************************************************************/
int set_bh_priority(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char bhPriority2G[32], bhPriority5GL[32], bhPriority5GH[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(bhPriority2G, 0, sizeof(bhPriority2G));
	memset(bhPriority5GL, 0, sizeof(bhPriority5GL));
	memset(bhPriority5GH, 0, sizeof(bhPriority5GH));
	
	cfg_get_object_attr(MESH_MAPD_CFG_NODE, MAP_BH_PRIORITY_2G_ATTR, bhPriority2G, sizeof(bhPriority2G));
	cfg_get_object_attr(MESH_MAPD_CFG_NODE, MAP_BH_PRIORITY_5GL_ATTR, bhPriority5GL, sizeof(bhPriority5GL));
	cfg_get_object_attr(MESH_MAPD_CFG_NODE, MAP_BH_PRIORITY_5GH_ATTR, bhPriority5GH, sizeof(bhPriority5GH));
	SVC_MESH_DEBUG_INFO("bhPriority2G = [%s], bhPriority5GL = [%s], bhPriority5GH = [%s]\n", bhPriority2G, bhPriority5GL, bhPriority5GH);

	ret = mapd_interface_set_bh_priority(ctrl, bhPriority2G, bhPriority5GL, bhPriority5GH);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	
	mapd_interface_ctrl_close(ctrl);
	return ret;
}

/**********************************************************************************/
#if defined(TCSUPPORT_EASYMESH_R13) || (TCSUPPORT_CSUPPORT_ECNT_MAP_ENHANCE) || defined(TCSUPPORT_MAP_R2)
static int trigger_channel_planning(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char channel_planning_channel[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(channel_planning_channel, 0, sizeof(channel_planning_channel));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_CHANNEL_PLANNING_CHANNEL_ATTR, channel_planning_channel, sizeof(channel_planning_channel));
	SVC_MESH_DEBUG_INFO("channel_planning_channel = [%s]\n", channel_planning_channel);
	ret = mapd_interface_user_preferred_channel(ctrl, channel_planning_channel);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}

/**********************************************************************************/
static int trigger_channel_force_switching(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char al_mac_addr[32];
	char force_2g_channel[32];
	char force_5gl_channel[32];
	char force_5gh_channel[32];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(al_mac_addr, 0, sizeof(al_mac_addr));
	memset(force_2g_channel, 0, sizeof(force_2g_channel));
	memset(force_5gl_channel, 0, sizeof(force_5gl_channel));
	memset(force_5gh_channel, 0, sizeof(force_5gh_channel));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_CHANNEL_FORCE_AL_MAC_ATTR, al_mac_addr, sizeof(al_mac_addr));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_CHANNEL_FORCE_2G_CHANNEL_ATTR, force_2g_channel, sizeof(force_2g_channel));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_CHANNEL_FORCE_5GL_CHANNEL_ATTR, force_5gl_channel, sizeof(force_5gl_channel));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_PARA_CHANNEL_FORCE_5GH_CHANNEL_ATTR, force_5gh_channel, sizeof(force_5gh_channel));
	SVC_MESH_DEBUG_INFO("al_mac_addr = [%s], force_2g_channel = [%s], force_5gl_channel = [%s], force_5gh_channel = [%s]\n", 
		al_mac_addr, force_2g_channel, force_5gl_channel, force_5gh_channel);
	ret = mapd_interface_forceChSwitch(ctrl, al_mac_addr, force_2g_channel, force_5gl_channel, force_5gh_channel);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}
#endif
#if defined(TCSUPPORT_MAP_R2)
static int trigger_map_trigger_channel_planning_r2(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char band[4];
	int ret;

	SVC_MESH_ERROR_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(band, 0, sizeof(band));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_TRIGGER_CHANNEL_PLANNING_R2_ATTR, band, sizeof(band));
	SVC_MESH_ERROR_INFO("band = [%s]\n", band);
	ret = mapd_interface_trigger_ch_plan_R2(ctrl, band);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}

static int trigger_map_trigger_channel_planning_by_almac(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char almac[30];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(almac, 0, sizeof(almac));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_TRIGGER_CHANNEL_PLANNING_BY_ALMAC_ATTR, almac, sizeof(almac));
	SVC_MESH_DEBUG_INFO("band = [%s]\n", almac);

	ret = mapd_interface_trigger_ch_scan(ctrl, almac);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}

static int trigger_map_trigger_de_dump(void)
{
	struct mapd_interface_ctrl *ctrl = NULL;
	char almac[30];
	int ret;

	SVC_MESH_DEBUG_INFO("start\n");
	ctrl = mapd_interface_ctrl_open(MAPD_CTRL_FILE);
	if(!ctrl)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_ctrl_open failed!\n");
		return -1;
	}
	
	memset(almac, 0, sizeof(almac));
	cfg_get_object_attr(MESH_ACTION_PARA_NODE, MAP_ACTION_TRIGGER_DE_DUMP_ATTR, almac, sizeof(almac));
	SVC_MESH_DEBUG_INFO("band = [%s]\n", almac);

	ret = mapd_interface_trigger_de_dump(ctrl, almac);
	if(ACTION_SUCCESS != ret)
	{
		SVC_MESH_ERROR_INFO("mapd_interface_mandate_steer API Failed!\n");
	}
	mapd_interface_ctrl_close(ctrl);
	return ret;
}

#endif
/**********************************************************************************/
static const mesh_action_attr mesh_action_tbl[] = 
{
	{MAP_ACTION_WIFI_TRIGGER_ONBOARDING,  trigger_wifi_onboarding},
	{MAP_ACTION_ETHER_TRIGGER_ONBOARDING, trigger_ether_onboarding},
	{MAP_ACTION_LOAD_DEFAULT_SETTING, load_default_settings},
	{MAP_ACTION_TRIGGER_UPLINK_AP_SELECTION, select_best_ap},
	{MAP_ACTION_TRIGGER_MANDATE_STEERING_ON_AGENT, mandate_steer},
	{MAP_ACTION_TRIGGER_BACKHAUL_STEERING, bh_steer},
	{MAP_ACTION_TRIGGER_WPS_AT_FRONTHAUL_BSS_OF_AN_AGENT, trigger_map_wps},
	{MAP_ACTION_APPLY_BH_PRIORITY_ATTR, set_bh_priority},
#if defined(TCSUPPORT_EASYMESH_R13) || (TCSUPPORT_CSUPPORT_ECNT_MAP_ENHANCE) || defined(TCSUPPORT_MAP_R2)
	{MAP_ACTION_TRIGGER_CHANNEL_PLANNING_ATTR, trigger_channel_planning},
	{MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR, trigger_channel_force_switching},
#endif
#if defined(TCSUPPORT_MAP_R2)
	{MAP_ACTION_TRIGGER_CHANNEL_PLANNING_R2_ATTR, trigger_map_trigger_channel_planning_r2},
	{MAP_ACTION_TRIGGER_CHANNEL_PLANNING_BY_ALMAC_ATTR, trigger_map_trigger_channel_planning_by_almac},
	{MAP_ACTION_TRIGGER_DE_DUMP_ATTR, trigger_map_trigger_de_dump},
#endif
};
#define MESH_ACTION_TBL_SIZE  (sizeof(mesh_action_tbl) / sizeof(mesh_action_tbl[0]))


void clear_mesh_action(void)
{
    char value[64];
	int i = 0;
	int tbl_size = MESH_ACTION_TBL_SIZE;
    memset(value, 0, sizeof(value));
    snprintf(value, sizeof(value), "%d", 0);
	for(i=0; i<tbl_size; i++)
	{
		cfg_set_object_attr(MESH_ACTION_NODE, mesh_action_tbl[i].attr, value);
	}

    return ;
}

int action_evt_handle(void)
{
	int i = 0;
	int bit = 0;
	int ret = 0;
	int tbl_size = MESH_ACTION_TBL_SIZE;
    char value[64];
	for(i=0; i <tbl_size; i++)
	{
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(MESH_ACTION_NODE, mesh_action_tbl[i].attr, value, sizeof(value));
		bit = value_convert_bool(atoi(value));
		if(bit && (NULL != mesh_action_tbl[i].cb))
		{
			SVC_MESH_DEBUG_INFO("action = %s\n", mesh_action_tbl[i].attr);
			ret = mesh_action_tbl[i].cb();
			break;
		}
	}
	
	clear_mesh_action();
	
	return ret;
}

/****************************************************/
void disconnect_all_sta(void)
{
	char cmdbuf[64];
	char value[64];
	int bssid_num = 0;
	int i = 0;
	
	memset(value, 0, sizeof(value));
	cfg_get_object_attr(WLAN_COMMON_NODE, WLAN_COMMON_BSSID_NUM_ATTR, value, sizeof(value));
	bssid_num = atoi(value);
	for(i=0; i<bssid_num; i++)
	{
		memset(cmdbuf, 0, sizeof(cmdbuf));
		wifimgr_lib_set_WIFI_CMD("ra", i, sizeof(cmdbuf), "DisConnectAllSta=", cmdbuf, 0);
		svc_mesh_execute_cmd(cmdbuf, __func__, __LINE__);
	}

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_COMMON_BSSID_NUM_ATTR, value, sizeof(value));
	bssid_num = atoi(value);
	for(i=0; i<bssid_num; i++)
	{
		memset(cmdbuf, 0, sizeof(cmdbuf));
		wifimgr_lib_set_WIFI_CMD("rai", i, sizeof(cmdbuf), "DisConnectAllSta=", cmdbuf, 0);
		svc_mesh_execute_cmd(cmdbuf, __func__, __LINE__);
	}

	return ;
}

/*CT IOT*/
void set_map_allow_pkt_proc(unsigned char allow_pkt)
{
	char buf[4];

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d\n", allow_pkt);
	doValPut(MAP_ALLOW_PKT_PATH, buf);
	
	return;
}

void init_wapp(void)
{
	char buf[256];
	char param[128];	
	memset(buf, 0, sizeof(buf));
	memset(param, 0, sizeof(param));
	load_11R_param(param, sizeof(param));
	if(0 >= strlen(param))
	{	
	snprintf(buf, sizeof(buf), "%s > /dev/console &", WAPP_BIN_PATH);
	}
	else
	{
	snprintf(buf, sizeof(buf), "%s %s > /dev/console &", WAPP_BIN_PATH, param);	//11r support boot
	}
	svc_mesh_execute_cmd(buf, __func__, __LINE__);

	return ;
}

void init_mapd(void)
{
	char buf[256];
	char value[32];
	int ret = 0;
	int role = MAP_CONTROLLER;
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 mapd");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);
	
	memset(value, 0, sizeof(value));
	ret = cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
	if(ret > 0)
	{
		role = dev_roller(atoi(value));
	}
	SVC_MESH_DEBUG_INFO("role = %d\n", role);
	
	memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)	
	if (MAP_CONTROLLER == role || MAP_AGENTER == role || MAP_ROLE_UNCONFIGURED == role)
#else
	if (MAP_CONTROLLER == role || MAP_AGENTER == role)
#endif
	{
#if defined(TCSUPPORT_EASYMESH_R13)	|| defined(TCSUPPORT_MAP_R2)
		snprintf(buf, sizeof(buf), "%s -I %s -O %s > /dev/console &", \
			MAPD_BIN_PATH, MAPD_CFG_TXT_FILE,\
			MAPD_STRNG_CONF_FILE);
#else
		snprintf(buf, sizeof(buf), "%s -G %s -I %s -O %s > /dev/console&", \
			MAPD_BIN_PATH, MAP_WTS_BSS_INFO_CFG_FILE, MAPD_CFG_TXT_FILE,\
			MAPD_STRNG_CONF_FILE);
#endif
	}
	else
	{
		SVC_MESH_ERROR_INFO("Unknown EasyMesh Device Role!\n");
	}
	
	svc_mesh_execute_cmd(buf, __func__, __LINE__);
#if defined(TCSUPPORT_MAP_R2)
	cfg_set_object_attr(MESH_COMMON_NODE, MAP_RESTART_FLAG, "0");
#endif

	return ;
}

void init_p1905_managerd(void)
{
	char buf[256];
	char value[32];
	int ret = 0;
	int role = MAP_CONTROLLER;
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "killall -15 p1905_managerd");
	svc_mesh_execute_cmd(buf, __func__, __LINE__);
	
	memset(value, 0, sizeof(value));
	ret = cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
	if(ret > 0)
	{
		role = dev_roller(atoi(value));
	}
	SVC_MESH_DEBUG_INFO("role = %d\n", role);

	memset(buf, 0, sizeof(buf));
	if(MAP_CONTROLLER == role)
	{
		snprintf(buf, sizeof(buf), "%s -r0 -f %s -F %s > /dev/console&", \
			P1905_MANAGERD_BIN_PATH, MAP_CFG_TXT_FILE, MAP_WTS_BSS_INFO_CFG_FILE);
	}
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
	else if ((MAP_AGENTER == role) || (MAP_ROLE_UNCONFIGURED == role))
#else
	else if (MAP_AGENTER == role)
#endif		
	{
		snprintf(buf, sizeof(buf), "%s -r1 -f %s -F %s > /dev/console&", \
			P1905_MANAGERD_BIN_PATH, MAP_CFG_TXT_FILE, MAP_WTS_BSS_INFO_CFG_FILE);
	}
	else
	{
		SVC_MESH_ERROR_INFO("Unknown EasyMesh Device Role!\n");
	}
	
	svc_mesh_execute_cmd(buf, __func__, __LINE__);
	
	return ;
}

void init_bs20()
{
	char buf[256];
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%s &", BS20_BIN_PATH);
	svc_mesh_execute_cmd(buf, __func__, __LINE__);

	return ;
}
#if 0 //currently this func is not used due to wifi-eth loop issue---Geo
void ether_port_up_down(int op)
{
	int i = 0;
	char buf[64];
	
	if(OP_UP == op)
	{
		for(i=0; i<MAX_ECNT_ETHER_PORT_NUM; i++)
		{
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_ETHER_ITF_NAME_FORMAT"%d up", (i+1));
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
		}
	}
	else
	{
		for(i=0; i<MAX_ECNT_ETHER_PORT_NUM; i++)
		{
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_ETHER_ITF_NAME_FORMAT"%d down", (i+1));
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
		}
	}
	return ;
}
#endif

void apclient_up_down(int op)
{
	int i = 0;
	char buf[64];
#if defined(TCSUPPORT_NP)
	unsigned char map_enable = 0;
	char value[64] = {0};

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, value, sizeof(value));
	map_enable = atoi(value);
#endif
	
	if(OP_UP == op)
	{
		for(i = 0; i < MAX_APCLIENT_NUM; i++)
		{
#if defined(TCSUPPORT_NP)
			if(map_enable)
#endif
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_2G_NAME_FORMAT" down", i);
				svc_mesh_execute_cmd(buf, __func__, __LINE__);
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_5G_NAME_FORMAT" down", i);
				svc_mesh_execute_cmd(buf, __func__, __LINE__);
			}
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_2G_NAME_FORMAT" up", i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_5G_NAME_FORMAT" up", i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "brctl addif br0 "M_APCLIENT_2G_NAME_FORMAT, i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "brctl addif br0 "M_APCLIENT_5G_NAME_FORMAT, i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			
#if defined(TCSUPPORT_NP)
			if(map_enable)
#endif
			{
				memset(buf, 0, sizeof(buf));
				wifimgr_lib_set_WIFI_CMD("apcli", i, sizeof(buf), "ApCliEnable=0", buf, 0);
				svc_mesh_execute_cmd(buf, __func__, __LINE__);
				memset(buf, 0, sizeof(buf));
				wifimgr_lib_set_WIFI_CMD("apclii", i, sizeof(buf), "ApCliEnable=0", buf, 0);
				svc_mesh_execute_cmd(buf, __func__, __LINE__);
			}
		}
	}
	else
	{
		for(i = 0; i < MAX_APCLIENT_NUM; i++)
		{
			memset(buf, 0, sizeof(buf));
			wifimgr_lib_set_WIFI_CMD("apcli", i, sizeof(buf), "ApCliEnable=0", buf, 0);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			
			memset(buf, 0, sizeof(buf));
			wifimgr_lib_set_WIFI_CMD("apclii", i, sizeof(buf), "ApCliEnable=0", buf, 0);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "brctl delif br0 "M_APCLIENT_2G_NAME_FORMAT, i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "brctl delif br0 "M_APCLIENT_5G_NAME_FORMAT, i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);

			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_2G_NAME_FORMAT" down", i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "ifconfig "M_APCLIENT_5G_NAME_FORMAT" down", i);
			svc_mesh_execute_cmd(buf, __func__, __LINE__);
		}
	}
	return ;
}


