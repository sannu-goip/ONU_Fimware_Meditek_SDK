#ifndef __SVC_WAN_DHCP_PORT_FILTER_INFO_H__
#define __SVC_WAN_DHCP_PORT_FILTER_INFO_H__

#include "wan_common.h"

enum  _DHCP_PORT_FILTER_RULE_OP_
{
    E_ADD_DHCP_PORT_FILTER_RULE = (1 << 0),
    E_DEL_DHCP_PORT_FILTER_RULE = (1 << 2),
};

#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
int setSingleBridgeWanPortFilter(wan_entry_cfg_t* cfg, char* PortFilterInputTable);
void set_DHCP_PortFilter_info(wan_entry_obj_t* obj, int op);
#endif

#endif
