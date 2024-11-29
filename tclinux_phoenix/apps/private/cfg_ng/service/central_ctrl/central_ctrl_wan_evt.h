#ifndef __SVC_CENTRAL_CTRL_WAN_EVT_H__
#define __SVC_CENTRAL_CTRL_WAN_EVT_H__

typedef struct _ipv4_info_
{
	unsigned int s_addr;
	unsigned int subnet_mask;
	unsigned int subnet_id;

}ipv4_info_t, *pt_ipv4_info;

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

int get_ap_work_mode(void);

int cc_handle_wan_v4_up(struct host_service_pbuf* pbuf);

#endif


