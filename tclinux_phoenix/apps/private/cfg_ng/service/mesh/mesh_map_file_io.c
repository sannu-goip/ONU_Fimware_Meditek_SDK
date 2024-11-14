/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility.h>
#include <lan_port/lan_port_info.h>
#include "mesh_map_cfg.h"
#include "mesh_map_common.h"
#include "mesh_map_file_io.h"

enum _svc_mesh_file_type_
{
	MESH_FILE_STRING = 1,
	MESH_FILE_INT,
	MESH_FILE_UNSIGNED_INT, 
	MESH_FILE_LONG,
	MESH_FILE_UNSIGNED_LONG,
};

static mesh_name_value_type_file_attr	MeshMapCfgFile[] =
{
	{MESH_MAP_CFG_NODE,		MAP_AL_MAC_ATTR,				MESH_FILE_STRING,				ELEMENT_MAP_CFG_CONTROLLER_ALID},
	{MESH_MAP_CFG_NODE,		MAP_AL_MAC_ATTR,				MESH_FILE_STRING,				ELEMENT_MAP_CFG_AGETN_ALID},
	{MESH_MAP_CFG_NODE,		MAP_BR_ITF_NAME_ATTR,			MESH_FILE_STRING,				MAP_BR_ITF_NAME_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_ETHER_ITF_NAME_ATTR,		MESH_FILE_STRING,				MAP_ETHER_ITF_NAME_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_BSS_CONFIG_PRIORITY_ATTR,	MESH_FILE_STRING,				MAP_BSS_CONFIG_PRIORITY_ATTR},

	{MESH_MAP_CFG_NODE,		MAP_RADIO_BAND_ATTR,			MESH_FILE_STRING,				MAP_RADIO_BAND_ATTR},

	{MESH_COMMON_NODE,		MAP_DEV_ROLLER_ATTR,			MESH_FILE_INT,					ELEMENT_MAP_CFG_MAP_AGENT},
	{MESH_MAP_CFG_NODE,		MAP_BSS_RETRY_M1_ATTR,		    MESH_FILE_UNSIGNED_INT,	        MAP_BSS_RETRY_M1_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_TOPO_DIS_TTL_ATTR,		    MESH_FILE_UNSIGNED_INT,	        MAP_TOPO_DIS_TTL_ATTR},
	{MESH_COMMON_NODE,		MAP_NON_MESH_INTERFACE,			MESH_FILE_STRING,				MAP_NON_MESH_INTERFACE},
	{MESH_MAPD_CFG_NODE,	MAP_DHCP_CTL_ATTR,				MESH_FILE_INT,					MAP_DHCP_CTL_ATTR},
	{MESH_MAP_CFG_NODE, 	MAP_DEV_NUM_LIMIT_ATTR,			MESH_FILE_INT,					MAP_DEV_NUM_LIMIT_ATTR},
#if defined(TCSUPPORT_MAP_R2)
	{MESH_MAP_CFG_NODE, 	MAP_VER_ATTR,					MESH_FILE_STRING,				MAP_VER_ATTR},
	{MESH_MAP_CFG_NODE, 	MAP_TRANSPARENT_VIDS_ATTR,		MESH_FILE_INT,					MAP_TRANSPARENT_VIDS_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_ROLE_DETECTION_EXTERNAL_ATTR,				MESH_FILE_STRING,		MAP_ROLE_DETECTION_EXTERNAL_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_ENABLE_WPS_TOGGLE_5GL_5GH_ATTR,				MESH_FILE_STRING,		MAP_ENABLE_WPS_TOGGLE_5GL_5GH_ATTR},
	{MESH_STEER_CFG_NODE,	MAP_METRIC_REPLNTV_ATTR,		MESH_FILE_STRING,				MAP_METRIC_REPLNTV_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_WAN_ATTR,					MESH_FILE_STRING,				MAP_WAN_ATTR},
	
#endif
	{NULL,                  NULL,							0,								NULL}
};

static void write_node_to_buf(char *attr_file, int attr_type, char *buf, int bufSize, char *value)
{
	int intValue = 0;
	unsigned uintValue = 0;
	long longValue = 0;
	unsigned long uLongValue = 0;
	switch(attr_type)
	{
		case MESH_FILE_STRING:
			memset(buf, 0, bufSize);
			snprintf(buf, bufSize, "%s=%s", \
				attr_file, value);
			break;
		case MESH_FILE_INT:
			memset(buf, 0, bufSize);
			sscanf(value, "%d", &intValue);
			snprintf(buf, bufSize, "%s=%d", \
				attr_file, intValue);
			break;
		case MESH_FILE_UNSIGNED_INT:
			memset(buf, 0, bufSize);
			sscanf(value, "%u", &uintValue);
			snprintf(buf, bufSize, "%s=%u", \
				attr_file, uintValue);
			break;
		case MESH_FILE_LONG:
			memset(buf, 0, bufSize);
			sscanf(value, "%ld", &longValue);
			snprintf(buf, bufSize, "%s=%ld", \
				attr_file, longValue);
			break;
		case MESH_FILE_UNSIGNED_LONG:
			memset(buf, 0, bufSize);
			sscanf(value, "%lu", &uLongValue);
			snprintf(buf, bufSize, "%s=%lu", \
				attr_file, uLongValue);
			break;
		default:
			SVC_MESH_ERROR_INFO("attr_type = [%d], No this type\n", attr_type);
			break;
	}
	return;
}

unsigned char is_lan_itf_join_wan_itf(void)
{
	int role = MAP_ROLE_UNCONFIGURED;
	char value[32];
	memset(value, 0, sizeof(value));
	if(0 >= cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value)))
	{
		return FALSE;
	}
	role = dev_roller(atoi(value));
	if(MAP_AGENTER == role)
	{
		return TRUE;
	}
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT) && defined(TCSUPPORT_NP) && defined(TCSUPPORT_CT_UBUS)
	memset(value, 0, sizeof(value));
	if (0 >=  cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", value, sizeof(value)))
	{
		return FALSE;
	}
	if( (MAP_ROLE_UNCONFIGURED == role) && (0 == strcmp(value, "bridge")) )
	{
		return TRUE;
	}
#endif
	return FALSE;
}

int write_file_mesh_map_cfg()
{
	FILE *file = NULL;
	unsigned char buf[256];
	char value[256];
	int ret = 0;
	int i = 0, role = 0;

	file = fopen(MAP_CFG_TXT_FILE, "w");
	if(NULL == file)
	{
		SVC_MESH_ERROR_INFO("%s open failed\n", MAP_CFG_TXT_FILE);
		return MESH_ERROR;
	}
	
	load_bss_config_priority();
	while(NULL != MeshMapCfgFile[i].attr_nodeName)
	{
		memset(value, 0, sizeof(value));
		ret = cfg_get_object_attr(MeshMapCfgFile[i].attr_nodeName, MeshMapCfgFile[i].attr_attribute, value, sizeof(value));
		if ( 0 < ret && 0 < strlen(value))
		{
			if(!strcmp(MeshMapCfgFile[i].attr_attribute, MAP_DEV_ROLLER_ATTR))
			{	//DeviceRole=2 means agent, thus set map_agent=1, else map_agent=0. Need transfer here.
				role = atoi(value) - 1;
				if(role < 0 || role > 1){
					SVC_MESH_ERROR_INFO("%s():DeviceRole value is incorrect.\n", __FUNCTION__);
					role = 0;
				}
				write_node_to_buf(MeshMapCfgFile[i].attr_file, MeshMapCfgFile[i].attr_type, buf, sizeof(buf), itoa(role));
				fprintf(file, "%s\n", buf);
				i++;
				continue;
			}

			if(strcmp(MeshMapCfgFile[i].attr_attribute, MAP_ETHER_ITF_NAME_ATTR) == 0)
			{	
#ifdef TCSUPPORT_CT_MAP_INSIDE_AGENT		
				char veth_if[IFNAMSIZ]={0};
				snprintf(veth_if, sizeof(veth_if),"%s", VETH_ITF_NAME_1);
				if(veth_ready(veth_if) >0)
				{
				strcat(value,","VETH_ITF_NAME_1);
				}		
#endif			
				if(is_lan_itf_join_wan_itf())
				{
#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING) && defined(TCSUPPORT_NP) && defined(TCSUPPORT_CT_UBUS)
					strcat(value, ","CT_ETHER_WAN_ITF_NAME);
#endif
			}		
			}		

			write_node_to_buf(MeshMapCfgFile[i].attr_file, MeshMapCfgFile[i].attr_type, buf, sizeof(buf), value);
			fprintf(file, "%s\n", buf);
		}
		i++;
	}

	fclose(file);
	return MESH_SUCCESS;
}

static mesh_name_value_type_file_attr	MeshMapdCfgFile[] =
{
	{MESH_MAPD_CFG_NODE,	MAP_LAN_ITF_NAME_ATTR,						MESH_FILE_STRING,	MAP_LAN_ITF_NAME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_WAN_ITF_NAME_ATTR,						MESH_FILE_STRING,	MAP_WAN_ITF_NAME_ATTR},
	{MESH_DAT_NODE,			MAP_ENABLE_ATTR,							MESH_FILE_INT,		MAP_ENABLE_ATTR},
	{MESH_COMMON_NODE,		MAP_STEER_ENABLE_ATTR,						MESH_FILE_INT,		MAP_STEER_ENABLE_ATTR},
	{MESH_COMMON_NODE,		MAP_DEV_ROLLER_ATTR,						MESH_FILE_INT,		MAP_DEV_ROLLER_ATTR},
	{MESH_COMMON_NODE,		MAP_CH_PLANNING_ENABLE_ATTR,				MESH_FILE_INT,		MAP_CH_PLANNING_ENABLE_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BH_PRIORITY_2G_ATTR,					MESH_FILE_INT,		MAP_BH_PRIORITY_2G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BH_PRIORITY_5GL_ATTR,					MESH_FILE_INT,		MAP_BH_PRIORITY_5GL_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BH_PRIORITY_5GH_ATTR,					MESH_FILE_INT,		MAP_BH_PRIORITY_5GH_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_IDLE_BYTE_COUNT_ATTR,		MESH_FILE_UNSIGNED_LONG,		MAP_CH_PLANNING_IDLE_BYTE_COUNT_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_IDLE_TIME_ATTR,				MESH_FILE_UNSIGNED_LONG,		MAP_CH_PLANNING_IDLE_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_USER_PREFERRED_CH5G_ATTR,	MESH_FILE_INT,		MAP_CH_PLANNING_USER_PREFERRED_CH5G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_USER_PREFERRED_CH2G_ATTR,	MESH_FILE_INT,		MAP_CH_PLANNING_USER_PREFERRED_CH2G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_SCAN_THRESHOLD_2G_ATTR,					MESH_FILE_INT,		MAP_SCAN_THRESHOLD_2G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_SCAN_THRESHOLD_5G_ATTR,					MESH_FILE_INT,		MAP_SCAN_THRESHOLD_5G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_INIT_TIMEOUT_ATTR,			MESH_FILE_INT,		MAP_CH_PLANNING_INIT_TIMEOUT_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NETWORK_OPTIMIZATION_ENABLE_ATTR,		MESH_FILE_INT,		MAP_NETWORK_OPTIMIZATION_ENABLE_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_BOOTUP_WAIT_TIME_ATTR,		MESH_FILE_UNSIGNED_INT,		MAP_NTWRK_OPT_BOOTUP_WAIT_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_CONNECT_WAIT_TIME_ATTR,		MESH_FILE_UNSIGNED_INT,		MAP_NTWRK_OPT_CONNECT_WAIT_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_DISCONNECT_WAIT_TIME_ATTR,	MESH_FILE_UNSIGNED_INT,		MAP_NTWRK_OPT_DISCONNECT_WAIT_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_PERIODICITY_ATTR,				MESH_FILE_UNSIGNED_INT,		MAP_NTWRK_OPT_PERIODICITY_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NETWORK_OPTIMIZATION_SCORE_MARGIN_ATTR,	MESH_FILE_UNSIGNED_INT,		MAP_NETWORK_OPTIMIZATION_SCORE_MARGIN_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_DHCP_CTL_ATTR,							MESH_FILE_INT,		MAP_DHCP_CTL_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_AUTO_BH_SWITCHING_ATTR,					MESH_FILE_INT,		MAP_AUTO_BH_SWITCHING_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BAND_SWITCH_TIME_ATTR,					MESH_FILE_UNSIGNED_INT,		MAP_BAND_SWITCH_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_DISCERN_MANUFACTURER_ATTR,				MESH_FILE_INT,          MAP_DISCERN_MANUFACTURER_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_PRE_CONFIG_MANUFACTURER_ATTR,           MESH_FILE_STRING, 	    MAP_PRE_CONFIG_MANUFACTURER_ATTR},	
	{MESH_MAPD_CFG_NODE,	MAP_BL_TIMEOUT_ATTR,						MESH_FILE_UNSIGNED_INT, 	MAP_BL_TIMEOUT_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_RSSI_ZERO_SCORE_TH_ATTR,					MESH_FILE_INT, 	    		MAP_RSSI_ZERO_SCORE_TH_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CONTROLLER_LOOP_PREVENT,				MESH_FILE_INT, 	    		MAP_CONTROLLER_LOOP_PREVENT},
	{MESH_MAPD_CFG_NODE,	MAP_LINK_METRICS_RCPI_ATTR,					MESH_FILE_INT,		MAP_LINK_METRICS_RCPI_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BH_SWITCH_BY_CU_ENABLE_ATTR,			MESH_FILE_INT,		MAP_BH_SWITCH_BY_CU_ENABLE_ATTR},
#if defined(TCSUPPORT_MAP_R2)
	{MESH_MAPD_CFG_NODE,	MAP_BH_SWITCH_BY_CU_ENHANCE_ATTR,			MESH_FILE_INT,		MAP_BH_SWITCH_BY_CU_ENHANCE_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_ENABLE_R2_ATTR,				MESH_FILE_INT,		MAP_CH_PLANNING_ENABLE_R2_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CH_PLANNING_ENABLE_R2_WITH_BW_ATTR,		MESH_FILE_INT,		MAP_CH_PLANNING_ENABLE_R2_WITH_BW_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_DIVERGENT_CH_PLANNING_ATTR,				MESH_FILE_INT,		MAP_DIVERGENT_CH_PLANNING_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_DUAL_BH_ATTR,							MESH_FILE_INT,		MAP_DUAL_BH_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_MAX_ALLOWED_SCAN_ATTR,					MESH_FILE_INT,		MAP_MAX_ALLOWED_SCAN_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_THIRD_PARTY_CONNECTION_ATTR,			MESH_FILE_INT,		MAP_THIRD_PARTY_CONNECTION_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_BH_STEERTIMEOUT_ATTR,					MESH_FILE_INT,		MAP_BH_STEERTIMEOUT_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NON_MAP_AP_ENABLE_ATTR,					MESH_FILE_INT,		MAP_NON_MAP_AP_ENABLE_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_CENTRALIZED_STEERING_ATTR,				MESH_FILE_INT,		MAP_CENTRALIZED_STEERING_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_POST_CAC_TRIGGER_TIME_ATTR,	MESH_FILE_INT,		MAP_NTWRK_OPT_POST_CAC_TRIGGER_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_DATA_COLLECTION_TIME_ATTR,	MESH_FILE_INT,		MAP_NTWRK_OPT_DATA_COLLECTION_TIME_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_PREFER_5G_OVER_2G_ATTR,		MESH_FILE_INT,		MAP_NTWRK_OPT_PREFER_5G_OVER_2G_ATTR},
	{MESH_MAPD_CFG_NODE,	MAP_NTWRK_OPT_PREFER_5G_OVER_2G_RETRY_CNT_ATTR,		MESH_FILE_INT,		MAP_NTWRK_OPT_PREFER_5G_OVER_2G_RETRY_CNT_ATTR},
#endif
	{NULL,					NULL,										0,							NULL}
};

int write_file_mesh_mapd_cfg()
{
	FILE *file = NULL;
	unsigned char buf[256];
	char value[256];
	int ret = 0;
	int i = 0;

	file = fopen(MAPD_CFG_TXT_FILE, "w");
	if(NULL == file)
	{
		SVC_MESH_ERROR_INFO("%s open failed\n", MAPD_CFG_TXT_FILE);
		return MESH_ERROR;
	}

	while(NULL != MeshMapdCfgFile[i].attr_nodeName)
	{
		memset(value, 0, sizeof(value));
		ret = cfg_get_object_attr(MeshMapdCfgFile[i].attr_nodeName, MeshMapdCfgFile[i].attr_attribute, value, sizeof(value));
		if ( 0 < ret && 0 < strlen(value))
		{
			write_node_to_buf(MeshMapdCfgFile[i].attr_file, MeshMapdCfgFile[i].attr_type, buf, sizeof(buf), value);
		}
		else
		{
			snprintf(buf, sizeof(buf), "%s=", \
				MeshMapdCfgFile[i].attr_file);  
		}
		fprintf(file, "%s\n", buf);
		i++;
	}

	fclose(file);
	return MESH_SUCCESS;
}

static mesh_name_value_type_file_attr	MeshMapdStrngFile[] =
{
	{MESH_STEER_CFG_NODE,		MAP_LOW_RSSI_AP_STEER_EDGE_RE_ATTR,			MESH_FILE_INT,	MAP_LOW_RSSI_AP_STEER_EDGE_RE_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_OVERLOAD_TH_2G_ATTR,					MESH_FILE_INT,	MAP_CU_OVERLOAD_TH_2G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_OVERLOAD_TH_5GL_ATTR,				MESH_FILE_INT,	MAP_CU_OVERLOAD_TH_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_OVERLOAD_TH_5GH_ATTR,				MESH_FILE_INT,	MAP_CU_OVERLOAD_TH_5GH_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_PHY_BASED_SELECTION_ATTR,				MESH_FILE_INT,	MAP_PHY_BASED_SELECTION_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MCS_RATE_XING_THRESHOLD_DG_ATTR,		MESH_FILE_INT,	MAP_MCS_RATE_XING_THRESHOLD_DG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MCS_RATE_XING_THRESHOLD_UG_ATTR,		MESH_FILE_INT,	MAP_MCS_RATE_XING_THRESHOLD_UG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_STEERING_EDGE_DG_ATTR,				MESH_FILE_INT,	MAP_RSSI_STEERING_EDGE_DG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_STEERING_EDGE_UG_ATTR,				MESH_FILE_INT,	MAP_RSSI_STEERING_EDGE_UG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_LOW_RSSI_AP_STEER_EDGE_ROOT_ATTR,		MESH_FILE_INT,	MAP_LOW_RSSI_AP_STEER_EDGE_ROOT_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_SAFETY_TH_2G_ATTR,					MESH_FILE_INT,	MAP_CU_SAFETY_TH_2G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_SAFETY_TH_5G_L_ATTR,					MESH_FILE_INT,	MAP_CU_SAFETY_TH_5G_L_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CU_SAFETY_H_5G_H_ATTR,					MESH_FILE_INT,	MAP_CU_SAFETY_H_5G_H_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_RATE_XING_THRESHOLD_DG_ATTR,		MESH_FILE_UNSIGNED_INT,	MAP_RSSI_RATE_XING_THRESHOLD_DG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_RATE_XING_THRESHOLD_UG_ATTR,		MESH_FILE_UNSIGNED_INT,	MAP_RSSI_RATE_XING_THRESHOLD_UG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_AGE_LIMIT_ATTR,					MESH_FILE_INT,	MAP_RSSI_AGE_LIMIT_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RSSI_MEASURE_SAMPLES_ATTR,				MESH_FILE_INT,	MAP_RSSI_MEASURE_SAMPLES_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MIN_RSSI_OVER_LOAD_ATTR,				MESH_FILE_INT,	MAP_MIN_RSSI_OVER_LOAD_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MIN_RSSI_INC_TH_ROOT_ATTR,				MESH_FILE_INT,	MAP_MIN_RSSI_INC_TH_ROOT_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MIN_RSSI_INC_TH_RE_ATTR,				MESH_FILE_INT,	MAP_MIN_RSSI_INC_TH_RE_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MIN_RSSI_INC_TH_PEER_ATTR,				MESH_FILE_INT,	MAP_MIN_RSSI_INC_TH_PEER_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_ACTIVITY_THRESHOLD_ATTR,				MESH_FILE_UNSIGNED_INT,	MAP_ACTIVITY_THRESHOLD_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_MAX_CLINET_OVER_LOADED_ATTR,			MESH_FILE_UNSIGNED_INT,	MAP_MAX_CLINET_OVER_LOADED_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_FORCED_RSSI_UPDATE_ATTR,				MESH_FILE_INT,	MAP_FORCED_RSSI_UPDATE_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_STEERING_PROHIBIT_TIME_ATTR,			MESH_FILE_UNSIGNED_INT,	MAP_STEERING_PROHIBIT_TIME_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_BTM_STEERING_PROHIBIT_TIME_ATTR,		MESH_FILE_UNSIGNED_INT,	MAP_BTM_STEERING_PROHIBIT_TIME_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_STEERING_UN_FRIENDLY_TIME_ATTR,			MESH_FILE_UNSIGNED_INT,	MAP_STEERING_UN_FRIENDLY_TIME_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_BTM_UN_FRIENDLY_TIME_ATTR,				MESH_FILE_UNSIGNED_INT,	MAP_BTM_UN_FRIENDLY_TIME_ATTR},

	{MESH_STEER_CFG_NODE,		MAP_STEERING_PROHIBIT_TIME_JOIN_ATTR,		MESH_FILE_UNSIGNED_INT,	MAP_STEERING_PROHIBIT_TIME_JOIN_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_FORCE_ROAM_RSSI_TH_ATTR,				MESH_FILE_INT,	MAP_FORCE_ROAM_RSSI_TH_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_RESET_BTM_CSBC_AT_JOIN_ATTR,			MESH_FILE_UNSIGNED_INT,	MAP_RESET_BTM_CSBC_AT_JOIN_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_STEERING_DETECT_INTERVAL_ATTR,			MESH_FILE_UNSIGNED_INT,	MAP_STEERING_DETECT_INTERVAL_ATTR},

	{MESH_STEER_CFG_NODE,		MAP_AP_STEER_THRESH_TOLERANCE,				MESH_FILE_UNSIGNED_INT,	MAP_AP_STEER_THRESH_TOLERANCE},
	{MESH_STEER_CFG_NODE,		MAP_DIS_FORCE_STRNG_FOR_11V_STA,			MESH_FILE_UNSIGNED_INT,	MAP_DIS_FORCE_STRNG_FOR_11V_STA},
	{MESH_STEER_CFG_NODE,		MAP_DISABLE_FORCE_STRNG_ATTR,				MESH_FILE_INT,	MAP_DISABLE_FORCE_STRNG_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_DISABLE_CSBC_STATE,						MESH_FILE_INT,	MAP_DISABLE_CSBC_STATE},
#if defined(TCSUPPORT_MAP_R2)
	{MESH_STEER_CFG_NODE,		MAP_METRIC_REPLNTV_ATTR,					MESH_FILE_INT,	MAP_METRIC_REPLNTV_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_CH_UTIL_THRES_24G_ATTR,	MESH_FILE_INT,	MAP_METRIC_POLICY_CH_UTIL_THRES_24G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_CH_UTIL_THRES_5GL_ATTR,	MESH_FILE_INT,	MAP_METRIC_POLICY_CH_UTIL_THRES_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_CH_UTIL_THRES_5GH_ATTR,	MESH_FILE_INT,	MAP_METRIC_POLICY_CH_UTIL_THRES_5GH_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_CH_UTIL_THRESH_24G_ATTR,	MESH_FILE_INT,	MAP_CH_PLANNING_CH_UTIL_THRESH_24G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_CH_UTIL_THRESH_5GL_ATTR,	MESH_FILE_INT,	MAP_CH_PLANNING_CH_UTIL_THRESH_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_EDCCA_THRESH_24G_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_EDCCA_THRESH_24G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_EDCCA_THRESH_5GL_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_EDCCA_THRESH_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_OBSS_THRESH_24G_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_OBSS_THRESH_24G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_OBSS_THRESH_5GL_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_OBSS_THRESH_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_R2_MONIT_OR_TIMEOUT_SECS_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_R2_MONIT_OR_TIMEOUT_SECS_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_R2_MONIT_OR_PROHIBIT_SECS_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_R2_MONIT_OR_PROHIBIT_SECS_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_R2_METRIC_REPORTING_INTERVAL_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_R2_METRIC_REPORTING_INTERVAL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_CH_PLANNING_R2_MIN_SCORE_MARGIN_ATTR,		MESH_FILE_INT,	MAP_CH_PLANNING_R2_MIN_SCORE_MARGIN_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_RCPI_24G_ATTR,		MESH_FILE_INT,	MAP_METRIC_POLICY_RCPI_24G_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_RCPI_5GL_ATTR,		MESH_FILE_INT,	MAP_METRIC_POLICY_RCPI_5GL_ATTR},
	{MESH_STEER_CFG_NODE,		MAP_METRIC_POLICY_RCPI_5GH_ATTR,		MESH_FILE_INT,	MAP_METRIC_POLICY_RCPI_5GH_ATTR},
#endif
	{NULL,						NULL,										0,						NULL}
};

int write_file_mesh_steer_cfg()
{
	FILE *file = NULL;
	unsigned char buf[256];
	char value[64];
	int ret = 0;
	int i = 0;

	file = fopen(MAPD_STRNG_CONF_FILE, "w");
	if(NULL == file)
	{
		SVC_MESH_ERROR_INFO("%s open failed\n", MAPD_STRNG_CONF_FILE);
		return MESH_ERROR;
	}

	while(NULL != MeshMapdStrngFile[i].attr_nodeName)
	{
		memset(value, 0, sizeof(value));
		ret = cfg_get_object_attr(MeshMapdStrngFile[i].attr_nodeName, MeshMapdStrngFile[i].attr_attribute, value, sizeof(value));
		if ( 0 < ret && 0 < strlen(value))
		{
			write_node_to_buf(MeshMapdStrngFile[i].attr_file, MeshMapdStrngFile[i].attr_type, buf, sizeof(buf), value);
			fprintf(file, "%s\n", buf);
		}
		i++;
	}
	
	fclose(file);
	return MESH_SUCCESS;
}

//special character handling for SSID such as space, backslash....because wts file uses space to be separator
//For every space and backslash in SSID, add a backslash in front of them.
static void ssid_transfer_for_space_backslash(char *replace, char *origin_ssid)
{
	char ch_ptr = 0;
	unsigned short i = 0, j = 0, origin_ssid_len = 0;

	if(replace == NULL || origin_ssid == NULL)
		return;

	origin_ssid_len = strlen(origin_ssid);
	do {
		ch_ptr = origin_ssid[i];
		i++;
		if(ch_ptr == SPACE_ASCII || ch_ptr == BACKSLASH_ASCII){
			replace[j] = BACKSLASH_ASCII; j++;
			replace[j] = ch_ptr; j++;
		}
		else{
			replace[j] = ch_ptr; j++;
		}
	}while(i < origin_ssid_len);

	return;
}

static int check_space_backslash_in_ssid(char *ssid)
{
	char ch_ptr = 0;
	unsigned short i = 0;
	int found = 0, len = 0;

	if(ssid == NULL)
		return 0;

	len = strlen(ssid);
	while(i < len){
		ch_ptr = ssid[i];
		if(ch_ptr == SPACE_ASCII || ch_ptr == BACKSLASH_ASCII){
			found = 1;
			break;
		}
		i++;
	}
	return found;
}

int write_file_mesh_wts_bss_info_config(mesh_config_bss_info info[], int bss_num)
{
	FILE *file = NULL;
	unsigned char buf[256];
	unsigned char hidden[16];
	int i = 0, j = 1;
	char replace[128+1];
#if defined(TCSUPPORT_MAP_R2)
	char primaryVlan[10] = {0};
	char defaultPCP[10] = {0};
#endif
	file = fopen(MAP_WTS_BSS_INFO_CFG_FILE, "w");
	if(NULL == file)
	{
		SVC_MESH_ERROR_INFO("%s open failed\n", MAP_WTS_BSS_INFO_CFG_FILE);
		return MESH_ERROR;
	}

	for(i=0; i<bss_num; i++)
	{
		if(0 >= strlen(info[i].ssid))
		{
			break;
		}
		
		if(check_space_backslash_in_ssid(info[i].ssid)){
			memset(replace, 0, sizeof(replace));
			ssid_transfer_for_space_backslash(replace, info[i].ssid);
			strncpy(info[i].ssid, replace, strlen(replace));
			info[i].ssid[strlen(replace)] = '\0';
		}
		if(check_space_backslash_in_ssid(info[i].key)){
			memset(replace, 0, sizeof(replace));
			ssid_transfer_for_space_backslash(replace, info[i].key);
			strncpy(info[i].key, replace, strlen(replace));
			info[i].key[strlen(replace)] = '\0';
		}
		
		memset(hidden, 0, sizeof(hidden));
		convert_hidden_string(info[i].hidden_ssid, hidden, sizeof(hidden));
		memset(buf, 0, sizeof(buf));

#if defined(TCSUPPORT_MAP_R2)
		memset(primaryVlan, 0, sizeof(primaryVlan));
		if(info[i].primaryVlan == 1)
			strncpy(primaryVlan, "pvid", 4);
		else
			strncpy(primaryVlan, "N/A", 3);

		memset(defaultPCP, 0, sizeof(defaultPCP));
		if(info[i].defaultPCP == -1)
			strncpy(defaultPCP, "N/A", 3);
		else
			snprintf(defaultPCP, sizeof(defaultPCP), "%d", info[i].defaultPCP);
#endif
		
		if ((1 == info[i].back_haul) || (1 == info[i].front_haul))
		{
#if defined(TCSUPPORT_MAP_R2)
			snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s %d %s %s", \
				(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac), info[i].oper_class, info[i].ssid, \
				info[i].auth_mode, info[i].encrypt_type, info[i].key, \
				info[i].back_haul, info[i].front_haul, hidden, info[i].vlanId, \
				primaryVlan, defaultPCP);
#else
			snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s 0x%02x", \
				(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac), info[i].oper_class, info[i].ssid, \
				info[i].auth_mode, info[i].encrypt_type, info[i].key, \
				info[i].back_haul, info[i].front_haul, hidden, info[i].band_width);
#endif
			fprintf(file, "%s\n", buf);
			if ( strcmp(info[i].oper_class, "11x") == 0 )
			{
#if defined(TCSUPPORT_MAP_R2)
			snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s %d %s %s", \
				(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac),  "12x", info[i].ssid, \
				info[i].auth_mode, info[i].encrypt_type, info[i].key, \
				info[i].back_haul, info[i].front_haul, hidden, info[i].vlanId, \
				primaryVlan, defaultPCP);
#else
				snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s 0x%02x", \
					(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac), "12x", info[i].ssid, \
					info[i].auth_mode, info[i].encrypt_type, info[i].key, \
					info[i].back_haul, info[i].front_haul, hidden, info[i].band_width);
#endif
				fprintf(file, "%s\n", buf);
			}
			else if ( strcmp(info[i].oper_class, "12x") == 0 )
			{
#if defined(TCSUPPORT_MAP_R2)
				snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s %d %s %s", \
					(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac),  "11x", info[i].ssid, \
					info[i].auth_mode, info[i].encrypt_type, info[i].key, \
					info[i].back_haul, info[i].front_haul, hidden, info[i].vlanId, \
					primaryVlan, defaultPCP);
#else
				snprintf(buf, sizeof(buf), "%d,"MAP_ADDR_FMT_STR" %s %s 0x%04x 0x%04x %s %d %d %s 0x%02x", \
					(j++), MAP_ADDR_FMT_ARS(info[i].Al_mac), "11x", info[i].ssid, \
					info[i].auth_mode, info[i].encrypt_type, info[i].key, \
					info[i].back_haul, info[i].front_haul, hidden, info[i].band_width);
#endif
				fprintf(file, "%s\n", buf);
			}		
		}
	}


    fclose(file);
    return MESH_SUCCESS;
}

static mesh_name_value_type_file_attr	MeshWappCfgFile[] =
{
	{LAN_ENTRY_NODE,		LAN_IP_ATTR,								MESH_FILE_STRING,		LAN_IP_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_BSS_CONFIG_PRIORITY_ATTR,				MESH_FILE_STRING,		MAP_BSS_CONFIG_PRIORITY_ATTR},
	{MESH_DAT_NODE,			MAP_TRURNKEY_ATTR,							MESH_FILE_STRING,		MAP_TRURNKEY_ATTR},
	{MESH_COMMON_NODE,		MAP_QUICK_CH_CHANGE_ATTR,					MESH_FILE_STRING,		MAP_QUICK_CH_CHANGE_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_CONFIG_STAT_ELAPSED_TIME_ATTR,			MESH_FILE_STRING,		MAP_CONFIG_STAT_ELAPSED_TIME_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_CONFIG_ONGOING_RETRY_TIMES_ATTR,		MESH_FILE_STRING,		MAP_CONFIG_ONGOING_RETRY_TIMES_ATTR},
	{MESH_COMMON_NODE,		MAP_APMetric_Mand_ATTR,						MESH_FILE_STRING,		MAP_APMetric_Mand_ATTR},
#if defined(TCSUPPORT_MAP_R2)
	{MESH_DAT_NODE,			MAP_MODE_ATTR,								MESH_FILE_STRING,		MAP_MODE_ATTR},
	{MESH_COMMON_NODE,		MAP_DEVICE_ROLE_ATTR,						MESH_FILE_STRING,		MAP_DEVICE_ROLE_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_MAX_STA_ALLOWED_ATTR,						MESH_FILE_STRING,		MAP_MAX_STA_ALLOWED_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_ROLE_DETECTION_EXTERNAL_ATTR,				MESH_FILE_STRING,		MAP_ROLE_DETECTION_EXTERNAL_ATTR},
	{MESH_MAP_CFG_NODE,		MAP_ENABLE_WPS_TOGGLE_5GL_5GH_ATTR,				MESH_FILE_STRING,		MAP_ENABLE_WPS_TOGGLE_5GL_5GH_ATTR},
	{MESH_STEER_CFG_NODE,	MAP_METRIC_REPLNTV_ATTR,					MESH_FILE_STRING,		MAP_METRIC_REPLNTV_ATTR},
#endif
	{NULL,					NULL,										0,						NULL}
};

static mesh_name_value_type_file_attr	MeshWappApcliBhFile[] =
{
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_Valid_ATTR,			MESH_FILE_STRING,		MAP_BHPROFILE_Valid_ATTR},
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_Ssid_ATTR,			MESH_FILE_STRING,		MAP_BHPROFILE_Ssid_ATTR},
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_WpaPsk_ATTR,			MESH_FILE_STRING,		MAP_BHPROFILE_WpaPsk_ATTR},
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_AuthMode_ATTR,		MESH_FILE_STRING,		MAP_BHPROFILE_AuthMode_ATTR},
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_EncrypType_ATTR,		MESH_FILE_STRING,		MAP_BHPROFILE_EncrypType_ATTR},
	{MESH_APCLIBH_ENTRY_NODE,		MAP_BHPROFILE_RaID_ATTR,			MESH_FILE_STRING,		MAP_BHPROFILE_RaID_ATTR},
	{NULL,							NULL,								0,						NULL}
};


int write_file_mesh_wapp_cfg()
{
	FILE *file = NULL;
	unsigned char buf[256];
	char value[256];
	int ret = 0;
	int i = 0, j = 0;
	char apclibhPath[32];

	file = fopen(ECNT_WAPP_CFG_TXT_FILE, "w");
	if(NULL == file)
	{
		SVC_MESH_ERROR_INFO("%s open failed\n", ECNT_WAPP_CFG_TXT_FILE);
		return MESH_ERROR;
	}

	
	while(NULL != MeshWappCfgFile[i].attr_nodeName)
	{
		memset(value, 0, sizeof(value));
		ret = cfg_get_object_attr(MeshWappCfgFile[i].attr_nodeName, MeshWappCfgFile[i].attr_attribute, value, sizeof(value));
		if ( 0 < ret && 0 < strlen(value))
		{
			write_node_to_buf(MeshWappCfgFile[i].attr_file, MeshWappCfgFile[i].attr_type, buf, sizeof(buf), value);
		}
		else
		{		
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "%s=", \
				MeshWappCfgFile[i].attr_file);  
		}
		fprintf(file, "%s\n", buf);
		i++;
	}

	
	for (j = 0; j < MAX_NUM_OF_RADIO; j++)
	{
		memset(apclibhPath, 0, sizeof(apclibhPath));
		snprintf(apclibhPath, sizeof(apclibhPath), MESH_APCLIBH_ENTRY_NODE, j + 1);
		if(0 <=  cfg_query_object(apclibhPath,NULL,NULL))
		{			
			i=0;
			while(NULL != MeshWappApcliBhFile[i].attr_nodeName)
			{
				memset(value, 0, sizeof(value));				
				memset(buf, 0, sizeof(buf));
				ret = cfg_get_object_attr(apclibhPath, MeshWappApcliBhFile[i].attr_attribute, value, sizeof(value));
				if ( 0 < ret && 0 < strlen(value))
				{
					snprintf(buf, sizeof(buf), "ApcliBh%d_%s=%s", \
					j, MeshWappApcliBhFile[i].attr_attribute, value);	
				}
				else
				{				
					snprintf(buf, sizeof(buf), "ApcliBh%d_%s=", \
					j, MeshWappApcliBhFile[i].attr_attribute);	
				}
				fprintf(file, "%s\n", buf);
				i++;
			}			
		}
	
	}
	
    fclose(file);
    return MESH_SUCCESS;
}

