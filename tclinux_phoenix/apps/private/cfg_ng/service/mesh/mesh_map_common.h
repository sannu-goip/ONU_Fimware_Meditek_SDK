#ifndef _MESH_MAP_COMMON_H_
#define _MESH_MAP_COMMON_H_
#include <inttypes.h>

/*********************************************************************/
#define MESH_ERROR     (-1)
#define MESH_SUCCESS   (0)

#ifndef TRUE
#define TRUE           (1)
#endif

#ifndef FALSE
#define FALSE          (0)
#endif

#ifndef ETH_ALEN
#define ETH_ALEN       (6)
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ       (16)
#endif
/*********************************************************************/
#define MAP_ROLE_UNCONFIGURED           (0)
#define MAP_CONTROLLER                  (1)
#define MAP_AGENTER                     (2)
/*********************************************************************/
#define MAP_ADDR_FMT_STR \
	"%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8

#define MAP_ADDR_FMT_ARS(ea) \
	(ea)[0], (ea)[1], (ea)[2], (ea)[3], (ea)[4], (ea)[5]
	
#define MAP_ADDR_SCAN_FMT \
	"%"SCNx8":%"SCNx8":%"SCNx8":%"SCNx8":%"SCNx8":%"SCNx8
	
#define MAP_ADDR_SCAN_ARGS(ea) \
        &(ea)[0], &(ea)[1], &(ea)[2], &(ea)[3], &(ea)[4], &(ea)[5]
/*********************************************************************/
#define WIFI_ITF_NAME        "ra%d"
#define WIFI_5G_ITF_NAME	 "rai%d"
/*********************************************************************/
#define WAPP_BIN_PATH                   "/userfs/bin/wapp"
#define MAPD_BIN_PATH                   "/userfs/bin/mapd"
#define P1905_MANAGERD_BIN_PATH         "/userfs/bin/p1905_managerd"
#define BS20_BIN_PATH                   "/userfs/bin/bs20"
#define WAPP_FILE_CMD                   "/tmp/wapp_ctrl"
#define WAPP_SERVER_FILE_CMD             "/tmp/wapp_server"

/*********************************************************************/
extern int g_svc_mesh_level;

void svc_mesh_printf(char *fmt,...);

enum _svc_mesh_dbg_level_
{
	E_NO_INFO_LEVEL         = 0,
	E_ERR_INFO_LEVEL        = 1,
	E_WARN_INFO_LEVEL       = 2, 
	E_CRITICAL_INFO_LEVEL   = 4,
	E_NOTICE_INFO_LEVEL     = 8,
	E_TRACE_INFO_LEVEL      = 16,
	E_DBG_INFO_LEVEL        = 32,
};
#define SVC_MESH_ERROR_INFO(fmt, ...)  do{ \
                                            if(g_svc_mesh_level & E_ERR_INFO_LEVEL) \
                                                svc_mesh_printf("error info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                         }while(0)

#define SVC_MESH_CRITIC_INFO(fmt, ...) do{ \
                                            if(g_svc_mesh_level & E_WARN_INFO_LEVEL) \
                                                svc_mesh_printf("critic info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                         }while(0)

#define SVC_MESH_NOTICE_INFO(fmt, ...) do{ \
                                            if(g_svc_mesh_level & E_NOTICE_INFO_LEVEL) \
                                                svc_mesh_printf("notice info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                         }while(0)

#define SVC_MESH_WARN_INFO(fmt, ...)  do{ \
                                           if(g_svc_mesh_level & E_WARN_INFO_LEVEL) \
                                                svc_mesh_printf("Warn info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_MESH_DEBUG_INFO(fmt, ...)  do{ \
                                            if(g_svc_mesh_level & E_DBG_INFO_LEVEL) \
                                                svc_mesh_printf("Debug info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                         }while(0)

#define SVC_MESH_TRACE_INFO(fmt, ...)  do{ \
                                            if(g_svc_mesh_level & E_TRACE_INFO_LEVEL) \
                                                svc_mesh_printf("Trace info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                         }while(0)
/*********************************************************************/
#define RADIO_2G        (0)
#define RADIO_5GL       (1)
#define RADIO_5GH       (2)
#define MAX_RADIO_NUM   (2)
#define MAX_RADIO_BSS_INFO_NUM      (16)
#define MAX_BSS_INFO_NUM            (MAX_RADIO_NUM * MAX_RADIO_BSS_INFO_NUM)
#define BSSINFO_DEF_KEY				"12345678"
/**********************************************************************************************/
#define MAX_ECNT_ETHER_PORT_NUM        (4)
#define M_ETHER_ITF_NAME_FORMAT        "eth0."
#define MAX_LAN_ITF_NAME               ((IFNAMSIZ + 1 ) * MAX_ECNT_ETHER_PORT_NUM)
#define OP_UP    (1)
#define OP_DOWN  (0)

/**********************************************************************************************/
#define MAX_APCLIENT_NUM				1
#define M_APCLIENT_2G_NAME_FORMAT		"apcli%d"
#define M_APCLIENT_5G_NAME_FORMAT		"apclii%d"
#define MESH_BLOCK_PAGE_SATRT           "1"
#define MESH_BLOCK_PAGE_END				"0"

/**********************************************************************************************/
#define MAX_NUM_OF_RADIO 		6
/**********************************************************************************************/
#define SPACE_ASCII 0x20
#define BACKSLASH_ASCII 0x5C
typedef struct _mesh_config_bss_info_
{
    unsigned char Al_mac[ETH_ALEN];
    char oper_class[4];
    char ssid[64+1];
    unsigned short auth_mode;
    unsigned short encrypt_type;
    char key[128+1];
    unsigned char front_haul;
    unsigned char back_haul;
    unsigned char hidden_ssid;
    unsigned short band_width;
#if defined(TCSUPPORT_MAP_R2)
	unsigned short vlanId;
	unsigned short primaryVlan;
	short defaultPCP;
#endif
}mesh_config_bss_info, *pt_mesh_config_bss_info;

typedef struct _mesh_action_
{
	unsigned char map_action_complete;
	unsigned char wifi_trigger_onboarding;
	unsigned char ether_trigger_onboarding;
	unsigned char load_default_setting;
	unsigned char display_runtime_topology;
	unsigned char trigger_uplink_ap_selection;
	unsigned char apply_channel_util_th;
	unsigned char apply_rssi_th;
	unsigned char trigger_mandate_steering;
	unsigned char trigger_backhaul_steering;
	unsigned char trigger_wps_fh_agent;
	unsigned char display_client_capabilities;
	unsigned char display_ap_fh_inf_list;
	unsigned char display_ap_bh_inf_list;
	
}mesh_action_t, *pt_mesh_action;
/*****************************************************************/
void mesh_send_evt(int type, int evt_id, char *buf);
void svc_mesh_debug_level(int level);
int svc_mesh_execute_cmd(char* cmd, char* func, int line);
unsigned char value_convert_bool(int action);
unsigned char dev_roller(unsigned char roller);
int generate_1905_AL_mac(unsigned char* AL_mac);
int convert_authmod_string(char *authmode);
int convert_encryptype_string(char *encryptype);
int convert_hidden_string(unsigned char hidden, unsigned char* buf, unsigned int buf_len);
int mesh_pthread(void);
int convert_bandwidth_24G_string(char *HT_BW, char *HT_BSSCoex);
int convert_bandwidth_5G_string(char *HT_BW, char *HT_BSSCoex, char *VHT_BW);
#ifdef TCSUPPORT_CT_MAP_INSIDE_AGENT
int veth_ready(const char *itf_name);
#endif
int internetRouteWanExist();
#endif




