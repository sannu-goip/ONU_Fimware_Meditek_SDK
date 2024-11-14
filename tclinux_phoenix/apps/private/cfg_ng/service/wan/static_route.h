#ifndef __SVC_WAN_STATIC_ROUTE_H__
#define __SVC_WAN_STATIC_ROUTE_H__

#include "wan_common.h"

#define MAX_ROUTE6_PARA_LEN (45)

enum route6_para_type
{
    DST_IP_TYPE         = 0,
    PREFIX_LEN_TYPE     = 1,
    GATEWAY_TYPE        = 2,
    DEVICE_TYPE         = 3,
};

#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE_NUM        (32)
#else
#define MAX_STATIC_ROUTE_NUM        (16)
#endif


#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE6_NUM       (32)
#else
#define MAX_STATIC_ROUTE6_NUM       (16) 
#endif

#define ROUTE6_ADD                  (1)
#define ROUTE6_DEL                  (2)

void check_route(char *path);
void check_static_route6(char* path);
void del_static_route(wan_entry_obj_t* obj);

#endif
