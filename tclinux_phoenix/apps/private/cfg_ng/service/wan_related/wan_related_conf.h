#ifndef __WAN_RELATED_CONF_H__
#define __WAN_RELATED_CONF_H__

#define SVC_WAN_RELATED_ATTR_ACTIVE_ENABLE		(1)
#define SVC_WAN_RELATED_ATTR_ACTIVE_DISABLE		(0)

#define SVC_WAN_RELATED_ATTR_LINKMODE_IP		(1)   
#define SVC_WAN_RELATED_ATTR_LINKMODE_PPP		(2)

#define MAX_WAN_RELATED_DEV_NAME_LEN			(16)
#define MAX_WAN_RELATED_ENTRY_PATH_LEN			(32)

typedef struct wan_entry_cfg_t
{
    char active;
	char linkmode;
	char isp;
}wan_entry_cfg;

typedef struct wan_related_conf_obj_t
{
    struct wan_related_conf_obj_t* next;
    char path[MAX_WAN_RELATED_ENTRY_PATH_LEN];
    unsigned char idx;
    char dev[MAX_WAN_RELATED_DEV_NAME_LEN];
    wan_entry_cfg cfg;
}wan_related_conf_obj;

wan_related_conf_obj* svc_wan_related_get_obj_list(void);
void svc_wan_related_init_obj_list(void);
int svc_wan_related_free_object(wan_related_conf_obj* obj);
int svc_wan_related_del_from_list(wan_related_conf_obj* obj);
int svc_wan_related_load_cfg(char* path, wan_entry_cfg* cfg);

wan_related_conf_obj*  svc_wan_related_find_object(char* path);
wan_related_conf_obj*  svc_wan_related_find_object_by_dev(char* dev);
wan_related_conf_obj* svc_wan_related_create_object(char* path);

#endif
