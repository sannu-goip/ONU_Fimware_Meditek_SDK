#ifndef _WAN_RELATED_VXLAN_CFG_H_
#define _WAN_RELATED_VXLAN_CFG_H_

#include "wan_related_vxlan_link_list.h"

#define SVC_WAN_RELATED_VXLAN_ENABLE            (1)
#define SVC_WAN_RELATED_VXLAN_DISABLE           (0)

#define SVC_WAN_RELATED_VXLAN_2_LAYER           (1)
#define SVC_WAN_RELATED_VXLAN_3_LAYER           (2)

#define SVC_WAN_RELATED_VXLAN_NAT_ENABLE        (1)
#define SVC_WAN_RELATED_VXLAN_NAT_DISABLE       (0)

#define SVC_WAN_RELATED_VXLAN_VLAN_ENABLE        (1)
#define SVC_WAN_RELATED_VXLAN_VLAN_DISABLE       (0)

#define VXLAN_FLAG_UPV4 (1 << 0)
#define MAX_VXLAN_NUM   (8)
#define MAX_WAN_ITF_NUM	(8)

enum _vxlan_link_op_
{
    E_VXLAN_LINK_DISABLE_2_ENABLE = 1, 
    E_VXLAN_LINK_ENABLE_2_DISABLE = 2, 
    E_VXLAN_LINK_ENABLE_2_ENABLE  = 3, 
};

enum _vxlan_dev_op_
{
    E_CREAT_VXLAN_DEVICE  = 1,
    E_DEL_VXLAN_DEVICE    = 2,
};

enum
{
    E_VXLAN_XPON_UP = 1,
    E_VXLAN_XPON_DOWN = 2,
};

enum
{
    E_VXLAN_WAN_UP = 1,
    E_VXLAN_WAN_DOWN = 2,
};


enum
{
    E_ADD_LINK_ROUTE = 1,
    E_DEL_LINK_ROUTE = 2,
};

#define     NOTBINDSTRING           "NOTBIND"
#define     MAX_BINDLIST_NUM        (64)

enum
{
    E_ADD_VXLAN_PORT_BIND_RULE = 1,
    E_DEL_VXLAN_PORT_BIND_RULE = 2,
};

typedef struct _vxlan_port_bind_info_
{
    int  if_index;
    char mode_flag;
    char bind_ifName_list[128];
    char bind_ifType_list[128];
    char bind_lanif_list[128];
}vxlan_port_bind_info_t, *pt_vxlan_port_bind_info;

int svc_wan_related_start_all_vxlan(void);
int svc_wan_related_stop_all_vxlan(void);
int svc_wan_related_vxlan_update(char *path);
int svc_wan_related_vxlan_delete(char *path);
int svc_wan_related_vxlan_get4(char* dev);
int svc_wan_related_vxlan_lost4(char* dev);
int svc_wan_related_vxlan_xpon_handle(int evt);
void vxlan_dealwith_link_local_route(vxlan_entry_obj_t* obj, int opt);
void svc_wan_related_vxlan_binding_wanif(char* wanif, int opt);

#endif
