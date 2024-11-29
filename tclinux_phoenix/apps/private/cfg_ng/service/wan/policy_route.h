#ifndef __SVC_WAN_POLICY_ROUTE_H__
#define __SVC_WAN_POLICY_ROUTE_H__

#include "wan_common.h"

#define WAN_IF_MARK         (0x7f0000) 
#define VLAN_TABLE_BASE     300
void set_policy_route(wan_entry_obj_t* obj, int evt);
void set_policy_route_vlan_mode(wan_entry_obj_t* obj, int evt);


#endif
