#ifndef __SVC_WAN_MISC_SRV_H__
#define __SVC_WAN_MISC_SRV_H__

#include "wan_common.h"


#define TRO69_IF_CONF           "/var/run/cwmp_if"
#define TRO69_IF_CONF2          "/var/run/cwmp_if2"

void update_wan_up_time(wan_entry_obj_t* obj, int evt);

#if defined(TCSUPPORT_CT_UBUS)
void update_br_wan_up_time(int evt);
#endif

void misc_srv_manage(char* path, int evt);
int open_VPN_PPP_tunnel(void);
void restartVPN(wan_entry_obj_t* obj);
#if defined(TCSUPPORT_CT_JOYME2)
int send_cwmp_wan_status(char* status, char *ip);
#endif
#endif
