#ifndef _HAL_ITF_WAN_H_ 
#define _HAL_ITF_WAN_H_

#define HAL_ITF_WAN_FAIL 	(-1)
#define HAL_ITF_WAN_SUCCESS (0)

#if defined(TCSUPPORT_CMCC)
#define MAX_WAN_PVC_NUMBER		(1)
#else
#define MAX_WAN_PVC_NUMBER		(8)
#endif
#define MAX_WAN_ENTRY_NUMBER	(8)
#define MAX_WAN_INTF_NUMBER		(MAX_WAN_PVC_NUMBER * MAX_WAN_ENTRY_NUMBER)
/*****************************************************/
typedef enum
{
    /*default*/
	E_WAN_IF_TYPE_ACTIVE        = 0x0,
    E_WAN_IF_TYPE_WAN_MODE      = 0x1,
    E_WAN_IF_TYPE_LINK_MODE     = 0x2,
	E_WAN_IF_TYPE_SRV_LIST      = 0x3,
    E_WAN_IF_TYPE_VLAN_MODE     = 0x4,
	E_WAN_IF_TYPE_VLAN_ID 		= 0x5,
    E_WAN_IF_TYPE_DOT1P         = 0x6,
    E_WAN_IF_TYPE_ISP           = 0x7,
    E_WAN_IF_TYPE_IP_VERSION    = 0x8,
    E_WAN_IF_TYPE_MTU           = 0x9,
    E_WAN_IF_TYPE_DHCP_ENABLE   = 0xA,
    E_WAN_IF_TYPE_MVID          = 0xB,
    E_WAN_IF_TYPE_NAT_ENABLE    = 0xC,
    E_WAN_IF_TYPE_CREAT_OMCI    = 0xD,


    E_WAN_IF_TYPE_IP_ADDR       = 0x60,
    E_WAN_IF_TYPE_NETMASK       = 0x61,
    E_WAN_IF_TYPE_GATEWAY       = 0x62,
    E_WAN_IF_TYPE_PRI_DNS       = 0x63,
    E_WAN_IF_TYPE_SEC_DNS       = 0x64,
    E_WAN_IF_TYPE_MAC_ADDR      = 0x65,

    E_WAN_IF_TYPE_ENCAP         = 0x80,
    E_WAN_IF_TYPE_PPPGETIP      = 0x81,
    E_WAN_IF_TYPE_CONNECTION    = 0x82,
    E_WAN_IF_TYPE_USERNAME      = 0x83,
    E_WAN_IF_TYPE_PASSWORD      = 0x84,
    E_WAN_IF_TYPE_PPPUNIT       = 0x85,
    E_WAN_IF_TYPE_IFNAME        = 0x86,
    E_WAN_IF_TYPE_CONNECTIONERROR=0x87,
    E_WAN_IF_TYPE_CLOSEIFIDLE   = 0x88,
    E_WAN_IF_TYPE_AUTHEN        = 0x89,
    E_WAN_IF_TYPE_SRVNAME       = 0x8A,

}ecnt_wan_if_type;
/*****************************************************/
typedef enum
{
    E_WAN_SRV_TR069 = 0,
    E_WAN_SRV_VOICE = 1,
}ecnt_wan_srv_list_type;
#define WAN_SRV_TR069_STR     "TR069"  
#define WAN_SRV_VOICE_SRV     "VOICE"
/*****************************************************/
typedef enum
{
    E_WAN_LINK_MODE_IP  = 0,
    E_WAN_LINK_MODE_PPP = 1,
}ecnt_wan_link_mode_type;
#define STR_WAN_LINK_MODE_IP      "linkIP"  
#define STR_WAN_LINK_MODE_PPP     "linkPPP"  
/*****************************************************/
typedef enum
{
    E_WAN_ISP_DHCP     = 0,
    E_WAN_ISP_STATIC   = 1,
    E_WAN_ISP_PPP      = 2,
    E_WAN_ISP_BRIDGE   = 3,
}ecnt_wan_isp_type;
#define STR_WAN_WAN_ISP_DHCP        "0"  
#define STR_WAN_ISP_STATIC          "1"  
#define STR_WAN_ISP_PPP             "2"  
#define STR_WAN_ISP_BRIDGE          "3"  
/**************************************************/
typedef struct _wan_link_path_info_
{
    int  num;
    char path[MAX_WAN_INTF_NUMBER][32];
}wan_link_path_info_t, *pt_wan_link_path_info;


void config_hal_itf_wan_debug(int debug_levle);

int creat_wan_link(char* path, int size);

int set_wan_link_attr(char* path, int path_size, int type, char* value, char* change);

int get_wan_link_attr(char* path, int type, char* value, int size);

int wan_link_effect(char* path);

int del_wan_link(char* path);

int get_wan_link_all_entry_path(wan_link_path_info_t* info);

int find_wan_path_by_serv(char *service,char* wanPath);

int get_wan_link_index(char* path, int * pvcIdx, int * entryIdx);

char is_exist_srv_voice(char* node, int size);

#endif
