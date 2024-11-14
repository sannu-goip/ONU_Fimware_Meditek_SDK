#ifndef __SVC_WAN_NAT_H__
#define __SVC_WAN_NAT_H__


int svc_wan_start_intf_nat(char* dev);
int svc_wan_stop_intf_nat(char* dev);
void check_nat_enable(void);

#endif
