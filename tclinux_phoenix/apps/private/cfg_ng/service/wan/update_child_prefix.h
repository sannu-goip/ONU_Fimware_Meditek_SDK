#ifndef __SVC_WAN_CHILD_PREFIX_H__
#define __SVC_WAN_CHILD_PREFIX_H__

#include "wan_common.h"

#define PREFIX_ORIGN_DFT        (0)
#define PREFIX_ORIGN_SLLA       (1)
#define PREFIX_ORIGN_DHCP       (2)
#define PREFIX_ORIGN_STATIC     (3)
#define PREFIX_ORIGN_NONE       (4)
#define TMP_PREFIX_IF_PATH      "/tmp/prefix_if.conf"

void set_childprefx_file(wan_entry_obj_t* obj);

#endif

