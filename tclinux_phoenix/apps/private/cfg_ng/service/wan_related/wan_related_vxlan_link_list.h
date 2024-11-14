#ifndef _WAN_RELATED_VXLAN_LINK_LIST_H_
#define _WAN_RELATED_VXLAN_LINK_LIST_H_


#define MAX_VXLAN_PATH_LEN              (32)
#define MAX_VXLAN_VNI_LEN               (16)
#define MAX_VXLAN_IPADDR_LEN            (16)
#define MAX_VXLAN_ADDR_TYPE_LEN         (8)
#define MAX_VXLAN_DEV_NAME_LEN          (8)
#define MAX_VXLAN_VLAN_DEV_NAME_LEN     (16)
#define MAX_VXLAN_PID_LEN               (32)

#define MAX_VXLAN_ENTRY_NUMBER          (8)
typedef struct vxlan_entry_cfg
{
	char tunnelkey[MAX_VXLAN_VNI_LEN];
	char enable;
	char remote_ip[MAX_VXLAN_IPADDR_LEN];
	char workmode;
	unsigned int max_mtu;
	char ip_addr[MAX_VXLAN_IPADDR_LEN];
	char subnetmask[MAX_VXLAN_IPADDR_LEN];
	char addr_type[MAX_VXLAN_ADDR_TYPE_LEN];
	char nat_enabled;
	char dnsservers_master[MAX_VXLAN_IPADDR_LEN];
	char dnsservers_slave[MAX_VXLAN_IPADDR_LEN];
	char default_gateway[MAX_VXLAN_IPADDR_LEN];
	char vlan_enable;
	unsigned int vlan;
	unsigned int bind_lanbit;
	char bind_wanitf[MAX_VXLAN_DEV_NAME_LEN];
}vxlan_entry_cfg_t;

typedef struct vxlan_entry_obj
{
	struct vxlan_entry_obj* next;
	char path[MAX_VXLAN_PATH_LEN];
	char vxlan_dev[MAX_VXLAN_VLAN_DEV_NAME_LEN];
	char original_dev[MAX_VXLAN_VLAN_DEV_NAME_LEN];
	char pid[MAX_VXLAN_PID_LEN];
	char ip_last[MAX_VXLAN_IPADDR_LEN];
	char dhcp_release;
	unsigned char flag;
	unsigned char idx;
	vxlan_entry_cfg_t cfg;
}vxlan_entry_obj_t;


vxlan_entry_obj_t* svc_wan_related_get_vxlan_obj_list(void);
void svc_wan_related_init_vxlan_obj_list(void);
int svc_wan_related_free_vxlan_obj(vxlan_entry_obj_t* obj);
int svc_wan_related_del_vxlan_obj_from_list(vxlan_entry_obj_t* obj);
vxlan_entry_obj_t*  svc_wan_related_find_vxlan_obj(char* path);
vxlan_entry_obj_t*  svc_wan_related_find_vxlan_obj_by_dev(char* dev);
vxlan_entry_obj_t* svc_wan_related_create_vxlan_obj(char* path);
vxlan_entry_obj_t* svc_wan_related_find_vxlan_obj_by_bind_wanif(char* wanif);

#endif
