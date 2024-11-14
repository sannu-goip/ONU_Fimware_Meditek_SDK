#ifndef __SVC_CENTRAL_CTRL_LAN_PORT_LINK_H__
#define __SVC_CENTRAL_CTRL_LAN_PORT_LINK_H__

#include <ecnt_event_global/ecnt_event_global.h>
#include <stdint.h>

void lan_down_action(struct ecnt_event_data *event_data);
void lan_up_action(struct ecnt_event_data *event_data);
void ether_wan_down_action(struct ecnt_event_data *event_data);
void ether_wan_up_action(struct ecnt_event_data *event_data);


//void detect_lan_port_link(void);


#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
typedef struct {
    unsigned char  tmac[6];
    unsigned char  smac[6];
    unsigned short type;
} __attribute__((packed)) ether_hdr_t ;

typedef struct {
    unsigned short hwtype;
    unsigned short prot;
    unsigned char  hwlen;
    unsigned char  protlen;
    unsigned short opcode;
    unsigned char  smac[6];
    unsigned int   sip;
    unsigned char  tmac[6];
    unsigned int   tip;
} __attribute__((packed)) arp_hdr_t ;

typedef struct {
    ether_hdr_t ethhdr;
    arp_hdr_t  arphdr;
} __attribute__((packed)) arp_packet_t ;

#define HW_TYPE_ETHERNET    0x0001
#define ARP_OPCODE_REQUEST 0x0001
#define ARP_OPCODE_REPLY   0x0002

void check_lanip_exist(void);

int conntrack_delete(u_int8_t l3protonum, u_int8_t protonum, unsigned int src_ip, u_int16_t l4dst);

int inet_open_rawsock(char *iface, int protocol);

int forge_arp_request(arp_packet_t *pkt, unsigned char *smac, unsigned int sip, unsigned int tip);

int inet_get_ifaddr(char *iface, unsigned char* mac, unsigned int *ip);

int getArpReply(char *iface, unsigned int tip, int timeout);
#endif

void flush_arp();

int notifyBrHostMacs(int portmask, int is_online);

void check_flush_arp();

#endif


