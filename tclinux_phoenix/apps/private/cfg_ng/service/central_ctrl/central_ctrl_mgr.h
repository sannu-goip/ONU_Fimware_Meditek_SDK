#ifndef __SVC_CENTRAL_CTRL_MGR_H__
#define __SVC_CENTRAL_CTRL_MGR_H__
#include <svchost_api.h> 

#include "central_ctrl_common.h"

int cc_mgr_debug_level(struct host_service_pbuf* pbuf);

int central_ctrl_mgr_deal_wan_evt(unsigned int evt, char* buf);

#endif

