#ifndef __SVC_WAN_INTERFACE_H__
#define __SVC_WAN_INTERFACE_H__

#include "wan_common.h"

int svc_wan_do_start_intf(wan_entry_obj_t* obj);

int svc_wan_do_stop_intf(wan_entry_obj_t* obj);

int stop_pppoe_relay(wan_entry_obj_t* obj);


#endif
