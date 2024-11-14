#ifndef __SVC_WAN_FULL_ROUTE_H__
#define __SVC_WAN_FULL_ROUTE_H__

#include "wan_common.h"

int full_route_execute(wan_entry_obj_t* obj);

void close_table_by_ifIndex(int index);

unsigned int get_default_pvc_DNSOrder(void);

#endif
