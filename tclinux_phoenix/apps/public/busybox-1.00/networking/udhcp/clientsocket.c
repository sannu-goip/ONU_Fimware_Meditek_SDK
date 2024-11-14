/*
 * clientsocket.c -- DHCP client socket creation
 *
 * udhcp client
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <features.h>
#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif

#include "clientsocket.h"
#include "common.h"
#if defined(TCSUPPORT_CT_JOYME4)
#include <linux/filter.h>
#endif

#if defined(TCSUPPORT_CT_JOYME4)
#define OP_LDH (BPF_LD  | BPF_H   | BPF_ABS)
#define OP_LDB (BPF_LD  | BPF_B   | BPF_ABS)
#define OP_JEQ (BPF_JMP | BPF_JEQ | BPF_K)
#define OP_RET (BPF_RET | BPF_K)

#define FILTER_CNT  6
static struct sock_filter bpfcode[FILTER_CNT] = {
	{ OP_LDB, 0, 0, 9 },
	{ OP_JEQ, 0, 2, IPPROTO_UDP },
	{ OP_LDH, 0, 0, 22          },
	{ OP_JEQ, 1, 0, 0x0044 },
	{ OP_RET, 0, 0, 0           },	/* not match */
	{ OP_RET, 0, 0, -1,         },	/* match */
};
#endif

int raw_socket(int ifindex)
{
	int fd;
	struct sockaddr_ll sock;
#if defined(TCSUPPORT_CT_JOYME4)	
	struct sock_fprog bpf = { FILTER_CNT, bpfcode };
#endif

	DEBUG(LOG_INFO, "Opening raw socket on ifindex %d", ifindex);
	if ((fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) < 0) {
		DEBUG(LOG_ERR, "socket call failed: %m");
		return -1;
	}

	sock.sll_family = AF_PACKET;
	sock.sll_protocol = htons(ETH_P_IP);
	sock.sll_ifindex = ifindex;
	if (bind(fd, (struct sockaddr *) &sock, sizeof(sock)) < 0) {
		DEBUG(LOG_ERR, "bind call failed: %m");
		close(fd);
		return -1;
	}

#if defined(TCSUPPORT_CT_JOYME4)
	if ( setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf)) < 0 )
	{
		perror("setsockopt ATTACH_FILTER");
		return -1;
	}
#endif

	return fd;
}
