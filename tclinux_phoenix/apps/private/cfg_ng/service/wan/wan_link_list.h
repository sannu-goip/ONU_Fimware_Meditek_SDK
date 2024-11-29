#ifndef __SVC_WAN_LINK_LIST_H__
#define __SVC_WAN_LINK_LIST_H__

#include "wan_common.h"

wan_entry_obj_t* svc_wan_get_obj_list(void);
void svc_wan_init_obj_list(void);
int svc_wan_free_object(wan_entry_obj_t* obj);
int svc_wan_del_from_list(wan_entry_obj_t* obj);
wan_entry_obj_t*  svc_wan_find_object(char* path);
wan_entry_obj_t*  svc_wan_find_object_by_dev(char* dev);
wan_entry_obj_t* svc_wan_create_object(char* path);

#endif
