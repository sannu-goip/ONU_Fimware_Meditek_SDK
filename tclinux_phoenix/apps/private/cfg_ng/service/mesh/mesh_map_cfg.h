#ifndef _MESH_MAP_CFG_H_
#define _MESH_MAP_CFG_H_

#include "mesh_map_common.h"

/*********************************file path*******************************************************/
#define MAP_CFG_TXT_FILE                    "/etc/map_cfg.txt"
#define ELEMENT_MAP_CFG_CONTROLLER_ALID     "map_controller_alid"
#define ELEMENT_MAP_CFG_AGETN_ALID          "map_agent_alid"
#define ELEMENT_MAP_CFG_MAP_AGENT           "map_agent"
#define MAPD_CFG_TXT_FILE                   "/etc/mapd_cfg.txt"
#define MAPD_STRNG_CONF_FILE                "/etc/mapd_strng.conf"
#define MAP_WTS_BSS_INFO_CFG_FILE           "/etc/wts_bss_info_config"
#define MAP_RUNTIME_TOPOLOGY_RET_FILE			"/tmp/mesh_run_time_topology.txt"
#define MAP_CLIENT_CAPABILITIES_RET_FILE		"/tmp/mesh_client_capabilities.txt"
#define MAP_AP_FH_INF_LIST_RET_FILE				"/tmp/mesh_ap_fh_inf_list.txt"
#define MAP_AP_BH_INF_LIST_RET_FILE				"/tmp/mesh_ap_bh_inf_list.txt"
#define ECNT_WAPP_CFG_TXT_FILE                   "/etc/ecnt_cfg.txt"

/*******************************Node cfg***************************************************/
/**
#define MESH_ENABLE_CONF_NODE                      	"root.mesh.enableconf"
**/
#define EASYMESH_ENABLE_ATTR                        "EasyMeshEnable"

#define BAND_STEERING_ENABLE_ATTR                   "BandSteeringEnable"

/**
#define MESH_DAT_NODE                               "root.mesh.dat"
**/
#define MESH_DAT_NODE                               "root.mesh.dat"
#define MAP_ENABLE_ATTR                             "MapEnable"

#define BS20_ENABLE_ATTR                            "BS20Enable"

#define MAP_TRURNKEY_ATTR                           "MAP_Turnkey"

#define MAP_EXT_ATTR                                "MAP_Ext"

#if defined(TCSUPPORT_MAP_R2)
#define MAP_MODE_ATTR                                "MapMode"
#define MAP_TS_ENABLE_ATTR							 "TSEnable"
#endif
/**
#define MESH_COMMON_NODE                         "root.mesh.common" 
**/
#define MAP_DEV_ROLLER_ATTR             "DeviceRole"
#define MAP_STEER_ENABLE_ATTR           "SteerEnable"
#define MAP_CH_PLANNING_ENABLE_ATTR     "ChPlanningEnable"
#define MAP_BSSINFO_COUNT_ATTR			"BssInfoCount"
#define MAP_QUICK_CH_CHANGE_ATTR		"MAP_QuickChChange"
#define MAP_TRIGGER_FLAG				"MeshTriggerFlag"
#define MAP_REINIT_WIFI_FLAG			"ReinitWifiFlag"
#define MAP_REINIT_FLAG_ATTR            "ReinitMeshFlag"
#define MAP_CLEAN_APCLIBH_FLAG			"CleanApclibhFlag"
#define MAP_VENDOR_OUI_ATTR            	"VendorOUI"
#define MAP_NON_MESH_INTERFACE			"NonMapIface"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_DEVICE_ROLE_ATTR			"DeviceRole"
#define MAP_MAX_STA_ALLOWED_ATTR		"MaxStaAllowed"
#define MAP_ROLE_DETECTION_EXTERNAL_ATTR "role_detection_external"
#define MAP_WAN_ATTR "wan"
#define MAP_ENABLE_WPS_TOGGLE_5GL_5GH_ATTR	"enable_wps_toggle_5GL_5GH"
#endif

/**
#define MESH_MAP_CFG_NODE                        "root.mesh.mapcfg"
**/
#define MAP_BR_ITF_NAME_ATTR            "br_inf"
#define MAP_ETHER_ITF_NAME_ATTR         "lan"
#define MAP_AL_MAC_ATTR                 "AL-MAC"
#define MAP_BSS_CONFIG_PRIORITY_ATTR    "bss_config_priority"

#define MAP_RADIO_BAND_ATTR             "radio_band"
#define MAP_DEV_NUM_LIMIT_ATTR             "p1905_dev_num_limit"

#define MAP_BSS_RETRY_M1_ATTR            "retry_m1"
#define MAP_TOPO_DIS_TTL_ATTR            "topology_discovery_ttl"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_VER_ATTR					 "map_ver"
#define MAP_TRANSPARENT_VIDS_ATTR        "transparent_vids"
#endif
/**
#define MESH_MAPD_CFG_NODE                          "root.mesh.mapdcfg"
**/
#define MAP_LAN_ITF_NAME_ATTR                       "lan_interface"
#define MAP_WAN_ITF_NAME_ATTR                       "wan_interface"
#define MAP_AP_STEER_RSSI_TH_ATTR                   "APSteerRssiTh"
#define MAP_CH_PLANNING_IDLE_BYTE_COUNT_ATTR        "ChPlanningIdleByteCount"
#define MAP_CH_PLANNING_IDLE_TIME_ATTR              "ChPlanningIdleTime"
#define MAP_BH_PRIORITY_2G_ATTR                     "BhPriority2G"
#define MAP_BH_PRIORITY_5GL_ATTR                    "BhPriority5GL"
#define MAP_BH_PRIORITY_5GH_ATTR                    "BhPriority5GH"
#define MAP_CH_PLANNING_USER_PREFERRED_CH5G_ATTR    "ChPlanningUserPreferredChannel5G"
#define MAP_CH_PLANNING_USER_PREFERRED_CH2G_ATTR    "ChPlanningUserPreferredChannel2G"
#define MAP_SCAN_THRESHOLD_2G_ATTR    				"ScanThreshold2g"
#define MAP_SCAN_THRESHOLD_5G_ATTR    				"ScanThreshold5g"
#define MAP_CH_PLANNING_INIT_TIMEOUT_ATTR    		"ChPlanningInitTimeout"
#define MAP_NETWORK_OPTIMIZATION_ENABLE_ATTR    	"NetworkOptimizationEnabled"
#define MAP_NTWRK_OPT_BOOTUP_WAIT_TIME_ATTR    		"NtwrkOptBootupWaitTime"
#define MAP_NTWRK_OPT_CONNECT_WAIT_TIME_ATTR    	"NtwrkOptConnectWaitTime"
#define MAP_NTWRK_OPT_DISCONNECT_WAIT_TIME_ATTR    	"NtwrkOptDisconnectWaitTime"
#define MAP_NTWRK_OPT_PERIODICITY_ATTR    			"NtwrkOptPeriodicity"
#define MAP_NETWORK_OPTIMIZATION_SCORE_MARGIN_ATTR  "NetworkOptimizationScoreMargin"
#define MAP_DHCP_CTL_ATTR  							"DhcpCtl"
#define MAP_AUTO_BH_SWITCHING_ATTR  				"AutoBHSwitching"
#define MAP_BAND_SWITCH_TIME_ATTR  					"BandSwitchTime"
#define MAP_DISCERN_MANUFACTURER_ATTR  			    "discern_manufacturer"
#define MAP_PRE_CONFIG_MANUFACTURER_ATTR  			"pre_config_manufacturer"
#define MAP_BL_TIMEOUT_ATTR            				"BlTimeout"
#define MAP_RSSI_ZERO_SCORE_TH_ATTR  				"RssiZeroScoreThreshold"
#define MAP_CONTROLLER_LOOP_PREVENT  				"ControllerLoopPrevent"
#define MAP_LINK_METRICS_RCPI_ATTR                   "link_metrics_rcpi"
#define MAP_BH_SWITCH_BY_CU_ENABLE_ATTR  			"BHSwitchbyCUEnable"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_BH_SWITCH_BY_CU_ENHANCE_ATTR  			"BHSwitchbyCUEnhance"
#define MAP_CH_PLANNING_ENABLE_R2_ATTR				"ChPlanningEnableR2"
#define MAP_CH_PLANNING_ENABLE_R2_WITH_BW_ATTR		"ChPlanningEnableR2withBW"
#define MAP_DIVERGENT_CH_PLANNING_ATTR				"DivergentChPlanning"
#define MAP_DUAL_BH_ATTR							"DualBH"
#define MAP_MAX_ALLOWED_SCAN_ATTR					"MaxAllowedScan"
#define MAP_THIRD_PARTY_CONNECTION_ATTR				"ThirdPartyConnection"
#define MAP_BH_STEERTIMEOUT_ATTR					"BHSteerTimeout"
#define MAP_NON_MAP_AP_ENABLE_ATTR					"NonMAPAPEnable"
#define MAP_CENTRALIZED_STEERING_ATTR				"CentralizedSteering"
#define MAP_NTWRK_OPT_POST_CAC_TRIGGER_TIME_ATTR	"NtwrkOptPostCACTriggerTime"
#define MAP_NTWRK_OPT_DATA_COLLECTION_TIME_ATTR		"NtwrkOptDataCollectionTime"
#define MAP_NTWRK_OPT_PREFER_5G_OVER_2G_ATTR		"NetwrkOptPrefer5Gover2G"
#define MAP_NTWRK_OPT_PREFER_5G_OVER_2G_RETRY_CNT_ATTR				"NetwrkOptPrefer5Gover2GRetryCnt"
#endif


/**
#define MESH_STEER_CFG_NODE                          "root.mesh.steercfg"
**/
#define MAP_LOW_RSSI_AP_STEER_EDGE_RE_ATTR           "LowRSSIAPSteerEdge_RE"
#define MAP_LOW_RSSI_AP_STEER_EDGE_ROOT_ATTR         "LowRSSIAPSteerEdge_root"
#define MAP_CU_OVERLOAD_TH_2G_ATTR                   "CUOverloadTh_2G"
#define MAP_CU_OVERLOAD_TH_5GL_ATTR                  "CUOverloadTh_5G_L"
#define MAP_CU_OVERLOAD_TH_5GH_ATTR                  "CUOverloadTh_5G_H"
#define MAP_CU_SAFETY_TH_2G_ATTR         		 	 "CUSafetyTh_2G"
#define MAP_CU_SAFETY_TH_5G_L_ATTR         		 	 "CUSafetyTh_5G_L"
#define MAP_CU_SAFETY_H_5G_H_ATTR         		 	 "CUSafetyh_5G_H"
#define MAP_PHY_BASED_SELECTION_ATTR                 "PHYBasedSelection"
#define MAP_RSSI_STEERING_EDGE_DG_ATTR         		 "RSSISteeringEdge_DG"
#define MAP_RSSI_STEERING_EDGE_UG_ATTR         		 "RSSISteeringEdge_UG"
#define MAP_RSSI_RATE_XING_THRESHOLD_DG_ATTR         "RSSIRateXingThreshold_DG"
#define MAP_RSSI_RATE_XING_THRESHOLD_UG_ATTR         "RSSIRateXingThreshold_UG"
#define MAP_RSSI_AGE_LIMIT_ATTR         			 "RSSIAgeLimit"
#define MAP_RSSI_MEASURE_SAMPLES_ATTR         		 "RSSIMeasureSamples"
#define MAP_MCS_RATE_XING_THRESHOLD_DG_ATTR          "MCSRateXingThreshold_DG"
#define MAP_MCS_RATE_XING_THRESHOLD_UG_ATTR          "MCSRateXingThreshold_UG"
#define MAP_MIN_RSSI_OVER_LOAD_ATTR         		 "MinRSSIOverload"
#define MAP_MIN_RSSI_INC_TH_ROOT_ATTR         		 "MinRssiIncTh_Root"
#define MAP_MIN_RSSI_INC_TH_RE_ATTR         		 "MinRssiIncTh_RE"
#define MAP_MIN_RSSI_INC_TH_PEER_ATTR         		 "MinRssiIncTh_Peer"
#define MAP_ACTIVITY_THRESHOLD_ATTR         		 "ActivityThreshold"
#define MAP_MAX_CLINET_OVER_LOADED_ATTR         	 "MaxClientOverloaded"
#define MAP_FORCED_RSSI_UPDATE_ATTR         		 "ForcedRssiUpdate"
#define MAP_STEERING_PROHIBIT_TIME_ATTR         	 "SteeringProhibitTime"
#define MAP_BTM_STEERING_PROHIBIT_TIME_ATTR          "BTMSteeringProhibitTime"
#define MAP_STEERING_UN_FRIENDLY_TIME_ATTR         	 "SteeringUnfriendlyTime"
#define MAP_BTM_UN_FRIENDLY_TIME_ATTR         		 "BTMUnfriendlyTime"

#define MAP_STEERING_PROHIBIT_TIME_JOIN_ATTR         "SteeringProhibitTimeJoin"
#define MAP_FORCE_ROAM_RSSI_TH_ATTR                  "force_roam_rssi_th"
#define MAP_RESET_BTM_CSBC_AT_JOIN_ATTR              "reset_btm_csbc_at_join"
#define MAP_STEERING_DETECT_INTERVAL_ATTR            "SteeringDetectInterval"

#define MAP_AP_STEER_THRESH_TOLERANCE                "APsteer_thresh_tolerance"
#define MAP_DIS_FORCE_STRNG_FOR_11V_STA              "disable_force_strng_11v_cli"
#define MAP_DISABLE_FORCE_STRNG_ATTR                 "disable_force_strng"
#define MAP_DISABLE_CSBC_STATE			             "disable_csbc_state"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_METRIC_REPLNTV_ATTR			             "MetricRepIntv"
#define MAP_METRIC_POLICY_CH_UTIL_THRES_24G_ATTR	 "MetricPolicyChUtilThres_24G"
#define MAP_METRIC_POLICY_CH_UTIL_THRES_5GL_ATTR	 "MetricPolicyChUtilThres_5GL"
#define MAP_METRIC_POLICY_CH_UTIL_THRES_5GH_ATTR	 "MetricPolicyChUtilThres_5GH"
#define MAP_CH_PLANNING_CH_UTIL_THRESH_24G_ATTR		 "ChPlanningChUtilThresh_24G"
#define MAP_CH_PLANNING_CH_UTIL_THRESH_5GL_ATTR		 "ChPlanningChUtilThresh_5GL"
#define MAP_CH_PLANNING_EDCCA_THRESH_24G_ATTR		 "ChPlanningEDCCAThresh_24G"
#define MAP_CH_PLANNING_EDCCA_THRESH_5GL_ATTR		 "ChPlanningEDCCAThresh_5GL"
#define MAP_CH_PLANNING_OBSS_THRESH_24G_ATTR		 "ChPlanningOBSSThresh_24G"
#define MAP_CH_PLANNING_OBSS_THRESH_5GL_ATTR		 "ChPlanningOBSSThresh_5GL"
#define MAP_CH_PLANNING_R2_MONIT_OR_TIMEOUT_SECS_ATTR "ChPlanningR2MonitorTimeoutSecs"
#define MAP_CH_PLANNING_R2_MONIT_OR_PROHIBIT_SECS_ATTR "ChPlanningR2MonitorProhibitSecs"
#define MAP_CH_PLANNING_R2_MIN_SCORE_MARGIN_ATTR 	 "ChPlanningR2MinScoreMargin"
#define MAP_CH_PLANNING_R2_METRIC_REPORTING_INTERVAL_ATTR	"ChPlanningR2MetricReportingInterval"
#define MAP_METRIC_POLICY_RCPI_24G_ATTR			     "MetricPolicyRcpi_24G"
#define MAP_METRIC_POLICY_RCPI_5GL_ATTR			     "MetricPolicyRcpi_5GL"
#define MAP_METRIC_POLICY_RCPI_5GH_ATTR			     "MetricPolicyRcpi_5GH"
#endif
/**
#define WLAN11AC_ENTRY_NODE				       "root.WLan11ac.Entry"
#define WLAN_ENTRY_NODE						"root.WLan.Entry"
**/
#define WLAN_APON_ATTR                    "APOn"
#define WLAN_ENTRY_SSID_ATTR              "SSID"
#define WLAN_ENTRY_ENABLE_SSID_ATTR       "EnableSSID"
#define WLAN_ENTRY_AUTH_MODE_ATTR         "AuthMode"
#define WLAN_ENTRY_ENCRYPT_TYPE_ATTR      "EncrypType"
#define WLAN_ENTRY_KEY_ATTR               "WPAPSK"
#define WLAN_ENTRY_HIDDEN_SSID_ATTR       "HideSSID"

#define WLAN_ENTRY_FT_SUPPORT_ATTR         "FtSupport"

#define WLAN_ENTRY_BH_ATTR       "EnableSSID"
#define WLAN_ENTRY_ENABLE_SSID_ATTR       "EnableSSID"
/**
#define WLAN_COMMON_NODE				       "root.WLan.Common"
#define WLAN11AC_COMMON_NODE			       "root.WLan11ac.Common"
**/

#define WLAN_COMMON_CHANNEL_ATTR          "Channel"
#define WLAN_COMMON_BSSID_NUM_ATTR        "BssidNum"

/* Authentication types */
#define AUTHMODE_OPEN								"OPEN"
#define AUTHMODE_OPEN_VALUE							0x0001
#define AUTHMODE_WPAPSK               				"WPAPSK"
#define AUTHMODE_WPAPSK_VALUE 						0x0002
#define AUTHMODE_WPA2PSK                  "WPA2PSK"
#define AUTHMODE_WPA2PSK_VALUE 						0x0020
#define AUTHMODE_WPAPSKWPA2PSK                 		"WPAPSKWPA2PSK"
#define AUTHMODE_SAE                 	  "WPA3PSK"
#define AUTHMODE_WPA2PSKWPA3PSK           "WPA2PSKWPA3PSK"
#define AUTHMODE_SAE_VALUE 							0x0040


/* Encryption type */
#define ENCRYPTYPE_NONE								"NONE"
#define ENCRYPTYPE_NONE_VALUE						0x0001
#define ENCRYPTYPE_TKIP			          			"TKIP"
#define ENCRYPTYPE_TKIP_VALUE						0x0004
#define ENCRYPTYPE_AES			          "AES"
#define ENCRYPTYPE_AES_VALUE						0x0008
#define ENCRYPTYPE_TKIPAES			          		"TKIPAES"

#define MAP_20_VALUE								0x02
#define MAP_40_VALUE								0x04
#define MAP_20_40_VALUE								0x06
#define MAP_20_40_80_VALUE							0x0e

/**
#define MESH_RADIO2G_BSSINFO_NODE             		"root.mesh.radio2gbssinfo"
#define MESH_RADIO5GL_BSSINFO_NODE				"root.mesh.radio5glbssinfo"
#define MESH_RADIO5GH_BSSINFO_NODE            		"root.mesh.radio5ghbssinfo
**/
#define MAP_BSSINFO_AL_MAC_ATTR           "AL-MAC"

/**
#define MESH_RADIO2G_BSSINFO_ENTRY_NODE             "root.mesh.radio2gbssinfo.entry.%d"
#define MESH_RADIO5GL_BSSINFO_ENTRY_NODE            "root.mesh.radio5glbssinfo.entry.%d"
#define MESH_RADIO5GH_BSSINFO_ENTRY_NODE            "root.mesh.radio5ghbssinfo.entry.%d"
**/
#define MAP_BSSINFO_FRONT_HAUL_ATTR        "FrontHaul"
#define MAP_BSSINFO_BACK_HAUL_ATTR         "BackHaul"

#define MAP_BSSINFO_HT_BW_ATTR         				"HT_BW"
#define MAP_BSSINFO_HT_BSSCOEXISTENCE_ATTR         	"HT_BSSCoexistence"
#define MAP_BSSINFO_VHT_BW_ATTR         			"VHT_BW"

#define MAP_2G_OPER_CLASS                  "8x"
#define MAP_5GL_OPER_CLASS                 "11x"
#define MAP_5GH_OPER_CLASS                 "12x"

#define MAP_DHCP_CTL_ENABLE				   "1"
#define MAP_DHCP_CTL_DISABLE			   "0"

#define MAP_BSSINFO_VLAN_ID_ATTR 		   "VlanID"
#define MAP_BSSINFO_PRIMARY_VLAN_ATTR	   "PrimaryVlan"
#define MAP_BSSINFO_DEFAULT_PCP_ATTR	   "DefaultPCP"

/**
#define MESH_ACTION_NODE                              "root.mesh.action"
**/
#define MAP_ACTION_COMPLETE                           "map_action_complete"
#define MAP_ACTION_WIFI_TRIGGER_ONBOARDING            "wifi_trigger_onboarding"
#define MAP_ACTION_ETHER_TRIGGER_ONBOARDING           "ether_trigger_onboarding"
#define MAP_ACTION_LOAD_DEFAULT_SETTING               "load_default_setting"
#define MAP_ACTION_DISPLAY_RUNTIME_TOPOLOGY           "display_runtime_topology"
#define MAP_ACTION_TRIGGER_UPLINK_AP_SELECTION        "trigger_uplink_ap_selection"
#define MAP_ACTION_TRIGGER_MANDATE_STEERING_ON_AGENT    	  "trigger_mandate_steering"
#define MAP_ACTION_TRIGGER_BACKHAUL_STEERING       			  "trigger_backhaul_steering"
#define MAP_ACTION_TRIGGER_WPS_AT_FRONTHAUL_BSS_OF_AN_AGENT   "trigger_wps_fh_agent"
#define MAP_ACTION_DISPLAY_CLIENT_CAPABILITIES				  "display_client_capabilities"
#define MAP_ACTION_DISPLAY_AP_FH_INF_LIST					  "display_ap_fh_inf_list"
#define MAP_ACTION_DISPLAY_AP_BH_INF_LIST					  "display_ap_bh_inf_list"
#define MAP_ACTION_APPLY_BH_PRIORITY_ATTR	 	  			  "apply_bh_priority"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_ACTION_TRIGGER_CHANNEL_PLANNING_R2_ATTR			  "trigger_channel_planning_r2"
#define MAP_ACTION_TRIGGER_CHANNEL_PLANNING_BY_ALMAC_ATTR	  "trigger_ChPlanning_by_almac"
#define MAP_ACTION_TRIGGER_DE_DUMP_ATTR	 					  "trigger_de_dump"
#endif

#define MAP_ACTION_TRIGGER_CHANNEL_PLANNING_ATTR			  "trigger_channel_planning"
#define MAP_ACTION_TRIGGER_CHANNEL_FORCE_SWITCHING_ATTR	 	  "trigger_channel_force_switching"


/**
#define MESH_ACTION_PARA_NODE                              "root.mesh.actionpara"
**/
#define MAP_ACTION_PARA_STA_MAC_ATTR					 "mandate_steer_sta_mac"
#define MAP_ACTION_PARA_TARGET_BSSID_ATTR				 "mandate_steer_target_bssid"
#define MAP_ACTION_PARA_BACK_HAUL_MAC_ATTR				 "steer_back_haul_mac"
#define MAP_ACTION_PARA_BACK_HAUL_TARGET_BSSID_ATTR		 "steer_back_haul_target_bssid"
#define MAP_ACTION_PARA_FRONT_HAUL_MAC_ATTR				 "wps_front_haul_mac"

#define MAP_ACTION_PARA_CHANNEL_PLANNING_CHANNEL_ATTR	 "channel_planning_channel"
#define MAP_ACTION_PARA_CHANNEL_FORCE_AL_MAC_ATTR	 	 "channel_force_al_mac"
#define MAP_ACTION_PARA_CHANNEL_FORCE_2G_CHANNEL_ATTR	 "channel_force_2g_channel"
#define MAP_ACTION_PARA_CHANNEL_FORCE_5GL_CHANNEL_ATTR	 "channel_force_5gl_channel"
#define MAP_ACTION_PARA_CHANNEL_FORCE_5GH_CHANNEL_ATTR	 "channel_force_5gh_channel"


#define LAN_IP_ATTR									"IP"
#define MAP_CONFIG_STAT_ELAPSED_TIME_ATTR           "elapsed_time"
#define MAP_CONFIG_ONGOING_RETRY_TIMES_ATTR         "config_retry"
#define MAP_APMetric_Mand_ATTR						"APMetric_MandSend"
#define MAP_BHPROFILE_Valid_ATTR					"BhProfileValid"
#define MAP_BHPROFILE_Ssid_ATTR						"BhProfileSsid"
#define MAP_BHPROFILE_WpaPsk_ATTR					"BhProfileWpaPsk"
#define MAP_BHPROFILE_AuthMode_ATTR					"BhProfileAuthMode"
#define MAP_BHPROFILE_EncrypType_ATTR				"BhProfileEncrypType"
#define MAP_BHPROFILE_RaID_ATTR						"BhProfileRaID"

#define LOAD_11RPARAM	"-c"
/**********************************************************************************/
int load_wts_bss_info(mesh_config_bss_info info[], int max_bss_num);
int load_bss_config_priority(void);
int load_11R_param(char *param, int length);
#endif






