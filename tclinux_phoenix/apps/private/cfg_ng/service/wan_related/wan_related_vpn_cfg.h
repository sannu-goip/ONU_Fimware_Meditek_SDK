#ifndef _WAN_RELATED_VPN_CFG_H_
#define _WAN_RELATED_VPN_CFG_H_

#include <svchost_evt.h>


int vpnlist_write(char* path, int idx);
int vpnlist_execute(char* path, int idx);

#if defined(TCSUPPORT_CT_JOYME2)
int vpnlist_l2tp_write(char* entry_path, int entry_number);
#if defined(TCSUPPORT_CT_VPN_PPTP)
int vpnlist_pptp_write_for_boot(void);
#endif
#endif
int vpnlist_boot_execute(void);

#endif
