#ifndef __UPDATE_IPV6_GATEWAY_H__
#define __UPDATE_IPV6_GATEWAY_H__

enum _ipv6_msg_update_
{
    E_GATEWAY_MSG_UPDATE    = (1 << 0),
    E_SLAAC_ADDR_MSG_UPDATE = (1 << 1),
};

void update_ipv6_slaac_addr_and_gateway(void);

#endif

