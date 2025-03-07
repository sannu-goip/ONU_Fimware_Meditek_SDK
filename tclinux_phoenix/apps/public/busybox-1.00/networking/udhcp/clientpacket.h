#ifndef _CLIENTPACKET_H
#define _CLIENTPACKET_H

#include "packet.h"

unsigned long random_xid(void);
int send_discover(unsigned long xid, unsigned long requested);
int send_selecting(unsigned long xid, unsigned long server, unsigned long requested);
int send_renew(unsigned long xid, unsigned long server, unsigned long ciaddr);
int send_renew(unsigned long xid, unsigned long server, unsigned long ciaddr);
int send_release(unsigned long server, unsigned long ciaddr);
int send_release_raw(unsigned long server, unsigned long ciaddr, uint8_t* mac);
int get_raw_packet(struct dhcpMessage *payload, int fd, char* mac, int flag);

#endif
