
/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#ifndef __SVC_WAN_COMMON_H__
#define __SVC_WAN_COMMON_H__


#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif


#ifndef ENABLE
#define ENABLE      (1)
#endif

#ifndef DISABLE
#define DISABLE     (0)
#endif

/****************debug******************************/
extern int g_svc_wan_level;

void svc_wan_printf(char *fmt,...);

enum _svc_wan_dbg_level_
{
    E_NO_INFO_LEVEL         = 0,
    E_ERR_INFO_LEVEL        = 1,
    E_WARN_INFO_LEVEL       = 2, 
    E_CRITICAL_INFO_LEVEL   = 4,
    E_NOTICE_INFO_LEVEL     = 8,
    E_TRACE_INFO_LEVEL      = 16,
    E_DBG_INFO_LEVEL        = 32,
};

typedef enum _PON_STATUS_
{
    E_WAN_PON_NULL = 0, 
    E_WAN_PON_DOWN = 1, 
    E_WAN_PON_UP 	 = 2, 
}E_PON_STATUS_T;

#define SVC_WAN_ERROR_INFO(fmt, ...)  do{ \
                                            if(g_svc_wan_level & E_ERR_INFO_LEVEL)\
                                                svc_wan_printf("error info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_WAN_CRITIC_INFO(fmt, ...) do{ \
                                            if(g_svc_wan_level & E_WARN_INFO_LEVEL)\
                                                svc_wan_printf("critic info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_WAN_NOTICE_INFO(fmt, ...) do{ \
                                            if(g_svc_wan_level & E_NOTICE_INFO_LEVEL) \
                                                svc_wan_printf("notice info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_WAN_WARN_INFO(fmt, ...)  do{ \
                                            if(g_svc_wan_level & E_WARN_INFO_LEVEL) \
                                                svc_wan_printf("Warn info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_WAN_DEBUG_INFO(fmt, ...)  do{ \
                                            if(g_svc_wan_level & E_DBG_INFO_LEVEL) \
                                                svc_wan_printf("Debug info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_WAN_TRACE_INFO(fmt, ...)  do{ \
                                            if(g_svc_wan_level & E_TRACE_INFO_LEVEL) \
                                                svc_wan_printf("Trace info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)


/****************buf length******************************/
#define SVC_WAN_BUF_8_LEN               (8)
#define SVC_WAN_BUF_16_LEN              (16)
#define SVC_WAN_BUF_32_LEN              (32)
#define SVC_WAN_BUF_64_LEN              (64)
#define SVC_WAN_BUF_128_LEN             (128)
#define SVC_WAN_BUF_256_LEN             (256)
#define SVC_WAN_BUF_512_LEN             (512)

#define MAX_WAN_IPV4_ADDR_LEN           (16)
#define MAX_WAN_IPV6_ADDR_LEN           (48)
#define MAX_WAN_DEV_NAME_LEN            (16)

#define MAX_WAN_ENTRY_PATH_LEN          (32)
#define MAX_WAN_PID_FILE_LEN            (32)
#define MAX_WAN_SRV_LIST_LEN            (32)
#define MAX_INFO_LENGTH                 (128)

#if defined(TCSUPPORT_CMCCV2)
#define IPOE_MTU_INCREMENT              (14)
#define PPPOE_MTU_INCREMENT             (IPOE_MTU_INCREMENT + 8)
#define MAX_IPOE_MTU_SIZE               (1900)
#define MAX_PPPOE_MTU_SIZE              (MAX_IPOE_MTU_SIZE - 8)
#endif
/*******************************************************/
#if defined(TCSUPPORT_CMCC)
#define SVC_WAN_MAX_PVC_NUM             (1)
#else
#define SVC_WAN_MAX_PVC_NUM             (8)
#endif
#define SVC_WAN_MAX_ENTRY_NUM           (8)  
#define SVC_MAX_WAN_INTF_NUMBER         ( (SVC_WAN_MAX_PVC_NUM) * (SVC_WAN_MAX_ENTRY_NUM) )

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
#define ONDEMAND_NEED_ADDROUTE	  (1<<0)
#define PPPD_PID_TEMP_PATH		  "/tmp/allpppd_pid"
#define WAN_COMMON_DEFROUTEIDXV4  "DefRouteIndexv4"
#define WAN_COMMON_DEFROUTEIDXV6  "DefRouteIndexv6"
#define WAN_COMMON_ONDEMANDIDXV4  "OnDemandRouteIndexv4"
#define WAN_COMMON_ONDEMANDIDXV6  "OnDemandRouteIndexv6"
#endif
/*******************************************************/
#define SVC_WAN_FLAG_STARTED            (1)
#define SVC_WAN_FLAG_UPV4               (2)
#define SVC_WAN_FLAG_UPV6               (4)
#define SVC_WAN_FLAG_PPP_RELAY_CHECK	(8)
/***********************ebtables********************************/
#define EBTABLES_BROUTE         "ebtables -t broute"
#define EBTABLES_FILTER         "ebtables -t filter"
/***********************iptables***************************/
#define IPTABLES_NAT            "iptables -t nat"
#define IPTABLES_RAW            "iptables -t raw"
#define IPTABLES_MANGLE         "iptables -t mangle"
/***********************ip6tables***************************/
#define IP6TABLES_MANGLE        "ip6tables -t mangle"
/*******************************************************/
#define VLAN_BIND               "VlanBind"
/******************************************************/
#define SVC_WAN_ATTR_ACTIVE_ENABLE      (1)
#define SVC_WAN_ATTR_ACTIVE_DISABLE     (0)

#define SVC_WAN_ATTR_WANMODE_ROUTE      (1)
#define SVC_WAN_ATTR_WANMODE_BRIDGE     (2)    

#define SVC_WAN_ATTR_LINKMODE_IP        (1)   
#define SVC_WAN_ATTR_LINKMODE_PPP       (2)


#define SVC_WAN_ATTR_VTAG_UNTAG         (1)
#define SVC_WAN_ATTR_VTAG_TRANS         (2)
#define SVC_WAN_ATTR_VTAG_TAG           (3)
#define SVC_WAN_ATTR_DOT1P_TAG_TRANS	   (4)


#define SVC_WAN_ATTR_VERSION_IPV4       (1)
#define SVC_WAN_ATTR_VERSION_IPV6       (2)
#define SVC_WAN_ATTR_VERSION_BOTH       (3)


#define SVC_WAN_ATTR_ISP_DHCP           (0)
#define SVC_WAN_ATTR_ISP_STATIC         (1)
#define SVC_WAN_ATTR_ISP_PPP            (2)
#define SVC_WAN_ATTR_ISP_BRIDGE         (3)

#define SVC_WAN_ATTR_CONNECT_KEEP_ALIVE (0)
#define SVC_WAN_ATTR_CONNECT_ON_DEMAND  (1)
#define SVC_WAN_ATTR_CONNECT_MANUALLY   (2)

#define SVC_WAN_ATTR_PPP_MANUAL_STATUS_DISCONNECT (0)
#define SVC_WAN_ATTR_PPP_MANUAL_STATUS_CONNECT    (1)

#define SVC_WAN_ATTR_IPV6MODE_DHCP      (1)
#define SVC_WAN_ATTR_IPV6MODE_SLAAC     (2)
#define SVC_WAN_ATTR_IPV6MODE_STATIC    (3)

#define SVC_WAN_ATTR_PD_ENABLE          (1)

#define SVC_WAN_ATTR_PDMODE_AUTO        (1)
#define SVC_WAN_ATTR_PDMODE_MANUAL      (2)


#define SVC_WAN_ATTR_GWV6_OP_AUTO       (0)
#define SVC_WAN_ATTR_GWV6_OP_MANUAL     (1)


#define SVC_WAN_ATTR_DHCPRELAY_NOT_CONFIG       (0)
#define SVC_WAN_ATTR_DHCPRELAY_DISABLE          (1)
#define SVC_WAN_ATTR_DHCPRELAY_ENABLE           (2)


#define SVC_WAN_ATTR_DSLITE_AUTO_MODE           (0)
#define SVC_WAN_ATTR_DSLITE_MANUAL_MODE         (1)

/***********************************************************************************/
enum
{
    E_NOT_DEFAULT_ROUTE     = 0,
    E_DEFAULT_ROUTE         = 1,
    E_DEFAULT_ROUTE_SWITCH  = 2,
};

enum
{
    E_ADD_RULE = 1,
    E_DEL_RULE = 2,
};

enum
{
    E_CREAT_WAN_DEVICE  = 1,
    E_DEL_WAN_DEVICE    = 2,
};

enum
{
	E_SRV_INTENET       = 1 << 0,
	E_SRV_E8C_INTENET   = 1 << 1,
	E_SRV_TR069   	    = 1 << 2,
	E_SRV_E8C_TR069     = 1 << 3,
	E_SRV_OTHER         = 1 << 4,
	E_SRV_IPTV          = 1 << 5,
	E_SRV_OTT           = 1 << 6,
	E_SRV_VOICE         = 1 << 7,
	E_SRV_VR         	= 1 << 8,
	E_SRV_SPE_SER       = 1 << 9,
};

enum
{
    E_SYSTEM_ONLY_IPV4_ENABLE = 1,
    E_SYSTEM_ONLY_IPV6_ENABLE = 2,
    E_SYSTEM_IPV4_IPV6_ENABLE = 3,
};

enum
{
    E_BSSADDR = 0,
    E_CAPABILITYADDR,
    E_ADDR_MAX_NUM,
};



/***********************************************************************************/
/***********************************************************************************/
#define SVC_WAN_IS_ACTIVE(x)            (x->active == SVC_WAN_ATTR_ACTIVE_ENABLE)

#define SVC_WAN_IS_ROUTE(x)             (x->wanmode == SVC_WAN_ATTR_WANMODE_ROUTE)

#define SVC_WAN_SRV_INTERNET(x)         (x->srv_type & E_SRV_INTENET)

#define SVC_WAN_SRV_E8C_INTERNET(x)     (x->srv_type & E_SRV_E8C_INTENET)

#define SVC_WAN_SRV_TR069(x)            (x->srv_type & E_SRV_TR069)

#define SVC_WAN_SRV_E8C_TR069(x)        (x->srv_type & E_SRV_E8C_TR069)

#define SVC_WAN_SRV_OTHER(x)            (x->srv_type & E_SRV_OTHER)

#define SVC_WAN_SRV_IPTV(x)            	(x->srv_type & E_SRV_IPTV)

#define SVC_WAN_SRV_OTT(x)           	(x->srv_type & E_SRV_OTT)

#define SVC_WAN_SRV_VOICE(x)			(x->srv_type & E_SRV_VOICE)

#define SVC_WAN_SRV_VR(x)				(x->srv_type & E_SRV_VR)

#define SVC_WAN_SRV_SPE_SER(x)			(x->srv_type & E_SRV_SPE_SER)

#define SVC_WAN_IS_BRIDGE(x)            (x->wanmode == SVC_WAN_ATTR_WANMODE_BRIDGE || x->isp == SVC_WAN_ATTR_ISP_BRIDGE)

#define SVC_WAN_IS_PPP(x)               (x->linkmode == SVC_WAN_ATTR_LINKMODE_PPP || x->isp == SVC_WAN_ATTR_ISP_PPP)

#define SVC_WAN_IS_CONN_ON_DEMAND(x)    (x->conn_mode == SVC_WAN_ATTR_CONNECT_ON_DEMAND)

#define SVC_WAN_IS_CONN_MANUALLY(x)     (x->conn_mode == SVC_WAN_ATTR_CONNECT_MANUALLY)

#define SVC_WAN_IS_PPP_MANUAL_CONN(x)   (x->ppp_manual_status == SVC_WAN_ATTR_PPP_MANUAL_STATUS_CONNECT)

#define SVC_WAN_IS_PPP_MANUAL_DISCONN(x) (x->ppp_manual_status == SVC_WAN_ATTR_PPP_MANUAL_STATUS_DISCONNECT)

#define SVC_WAN_IS_PPP_HYBRID(x)        (x->ppp_hybrid == ENABLE)

#define SVC_WAN_IS_IP(x)                (x->linkmode == SVC_WAN_ATTR_LINKMODE_IP || x->isp == SVC_WAN_ATTR_ISP_STATIC || x->isp == SVC_WAN_ATTR_ISP_DHCP)

#define SVC_WAN_IS_IPV4(x)              (x->version & SVC_WAN_ATTR_VERSION_IPV4)

#define SVC_WAN_IS_IPV6(x)              (x->version & SVC_WAN_ATTR_VERSION_IPV6)

#define SVC_WAN_IS_IPV4_DHCP(x)         ((x->isp == SVC_WAN_ATTR_ISP_DHCP) && (x->version & SVC_WAN_ATTR_VERSION_IPV4))

#define SVC_WAN_IS_IPV4_STATIC(x)       ((x->isp == SVC_WAN_ATTR_ISP_STATIC) && (x->version & SVC_WAN_ATTR_VERSION_IPV4))

#define SVC_WAN_IS_IPV6_DHCP(x)         ((x->ipv6mode == SVC_WAN_ATTR_IPV6MODE_DHCP) && (x->version & SVC_WAN_ATTR_VERSION_IPV6))

#define SVC_WAN_IS_IPV6_STATIC(x)       ((x->ipv6mode == SVC_WAN_ATTR_IPV6MODE_STATIC) && (x->version & SVC_WAN_ATTR_VERSION_IPV6))

#define SVC_WAN_IS_IPV6_SLAAC(x)        ((x->ipv6mode == SVC_WAN_ATTR_IPV6MODE_SLAAC) && (x->version & SVC_WAN_ATTR_VERSION_IPV6))

#define SVC_WAN_IPV6_PD_ENABLE(x)       (x->pd == ENABLE)

#define SVC_WAN_IPV6_PD_MANUAL(x)       (x->pdmode == SVC_WAN_ATTR_PDMODE_MANUAL)

#define SVC_WAN_DSLITE_ENABLE(x)        (x->dslite == ENABLE)

#define SVC_WAN_DSLITE_AUTO_MODE(x)     (x->dslite_mode == SVC_WAN_ATTR_DSLITE_AUTO_MODE)

#define SVC_WAN_DSLITE_MANUAL_MODE(x)   (x->dslite_mode == SVC_WAN_ATTR_DSLITE_MANUAL_MODE)

#define SVC_WAN_GWV6_IS_MANUAL(x)       (x->gwv6_op == SVC_WAN_ATTR_GWV6_OP_MANUAL)

#define SVC_WAN_PROXY_ENABLE(x)         (x->proxyenable == 1)

#define SVC_WAN_IPV6_DHCPV6PD_ENABLE(x) (x->dhcpv6pd == ENABLE)

#define SVC_WAN_IS_PING_RESP_ENABLE(x)  (x->pingresp == ENABLE)

#define SVC_WAN_NPT_ENABLE(x)            (x->npt == ENABLE)

/**********************************************************************/
typedef struct wan_entry_cfg
{
    unsigned short vid;
    unsigned short mvid;
    unsigned short mtu;
    unsigned int   pdtm1;
    unsigned int   pdtm2;
    unsigned short prefix6;
    unsigned int  bindbit;
    char active;
    char wanmode;
    char linkmode;
    char version;
    char phymode;
    char dot1p;
    char vtag;
    char isp;
    char conn_mode;
    char ppp_manual_status;
    char user[SVC_WAN_BUF_64_LEN];	
    char pwd[SVC_WAN_BUF_64_LEN];
    char addrv4[MAX_WAN_IPV4_ADDR_LEN];
    char maskv4[MAX_WAN_IPV4_ADDR_LEN];
    char gwv4[MAX_WAN_IPV4_ADDR_LEN];
    char dns1v4[MAX_WAN_IPV4_ADDR_LEN];
    char dns2v4[MAX_WAN_IPV4_ADDR_LEN];
    char ipv6mode; 
    char addrv6[MAX_WAN_IPV6_ADDR_LEN];
    char gwv6[MAX_WAN_IPV6_ADDR_LEN];
    char dns1v6[MAX_WAN_IPV6_ADDR_LEN];
    char dns2v6[MAX_WAN_IPV6_ADDR_LEN];	
    char pd;
    char pdmode;
    char dhcpv6pd;
    char pdaddr[MAX_WAN_IPV6_ADDR_LEN];
    char dslite;
    char dslite_mode;
    char srv_list[MAX_WAN_SRV_LIST_LEN];
    unsigned int srv_type;
    char gwv6_op;
    char ppp_hybrid;
    char pdorigin[SVC_WAN_BUF_32_LEN];
    char proxyenable;
    char dhcp_realy;
    char dhcp_enable;
    char vlanmask[SVC_WAN_BUF_32_LEN];
    char sdn;
    char pingresp;
    char npt;
    char pd6addr[MAX_WAN_IPV6_ADDR_LEN];
#if defined(TCSUPPORT_CT_JOYME4)
    char orgpd6addr[MAX_WAN_IPV6_ADDR_LEN];
#endif
}wan_entry_cfg_t;

typedef struct wan_entry_obj
{
    struct wan_entry_obj* next;
    char path[MAX_WAN_ENTRY_PATH_LEN];
    unsigned char flag;
    unsigned char idx;
    char dev[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t cfg;
    char nat;
    char dhcp_release;
    char ip_last[MAX_WAN_IPV4_ADDR_LEN];
    char ip6_last[MAX_WAN_IPV6_ADDR_LEN];
    char child_prefix_bits[SVC_WAN_BUF_64_LEN];
    char dslite_state;
    char pid4[MAX_WAN_PID_FILE_LEN];
    char pid6[MAX_WAN_PID_FILE_LEN];
}wan_entry_obj_t;

void svc_wan_debug_level(int level);

int svc_wan_check_xpon_up(void);

void do_val_put(char *path, char *value);

int isValidDnsIp(char * dnsIP);

int check_ip_format(char *ip_arg);

int check_mask_format(char *mask_arg, char *mask_decimal,int len);

int svc_wan_get_pid(char* file);

int svc_wan_execute_cmd(char* cmd);

int svc_wan_execute_cmd_normal(char* cmd);

int svc_wan_kill_process(char *path, char release_source);

int svc_wan_intf_name(wan_entry_obj_t* obj, char* name, int size);

int svc_wan_add_default_ipv4_route(char* gw,char* dev);

int svc_wan_delete_default_ipv4_route(void);

int svc_wan_add_default_ipv6_route(char* gw,char* dev);

int svc_wan_delete_default_ipv6_route(void);

int svc_wan_get_file_string(char *file_path, char line_return[][MAX_INFO_LENGTH], int line_num);

#if defined(TCSUPPORT_NP_CMCC)
void change_br0_ip(char* wan_ip);
#endif

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
void set_ondemand_route_state(int if_index, int state);

int get_ondemand_route_state(int if_index);

int isOndemandWan(int pvc_index, int entry_index);

void set_portbinding_info_for_ondemand_wan(int pvc_index, int entry_index,char *ifname);

int ClearOndemandWanHisAddr(int if_index);

int delRoute4ViaOndemand(int pvc_index, int entry_index, char *ifname);

int delRoute6ViaOndemand(int pvc_index, int entry_index, char *ifname);

int addDnsRoute6forOtherWan(char *ifname, char *ifsvrapp, char *hisaddrV6);
#endif

#endif



