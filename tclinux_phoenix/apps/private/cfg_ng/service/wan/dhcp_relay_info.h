#ifndef __SVC_WAN_DHCP_REALY_INFO_H__
#define __SVC_WAN_DHCP_REALY_INFO_H__
#include "wan_common.h"

enum  _DHCP_RELAY_OP_
{
    E_ADD_DHCPRELAY_RULE = 1,
    E_DEL_DHCPRELAY_RULE = 2,
};

void set_DHCPRelay_info(wan_entry_obj_t* obj, int op);

#endif
