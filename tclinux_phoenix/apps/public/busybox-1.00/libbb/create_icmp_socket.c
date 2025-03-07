/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * create raw socket for icmp protocol test permission
 * and drop root privileges if running setuid
 *
 */

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "libbb.h"

#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
extern int ipoe_flag;
#endif

int create_icmp_socket(void)
{
	struct protoent *proto;
	int sock;

	proto = getprotobyname("icmp");
	/* if getprotobyname failed, just silently force
	 * proto->p_proto to have the correct value for "icmp" */
	if ((sock = socket(AF_INET, SOCK_RAW,
			(proto ? proto->p_proto : 1))) < 0) {        /* 1 == ICMP */
		if (errno == EPERM)
			bb_error_msg_and_die(bb_msg_perm_denied_are_you_root);
		else
			bb_perror_msg_and_die(bb_msg_can_not_create_raw_socket);
	}

#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
	if ( 1 == ipoe_flag )
	{
		if( setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, "ipoe_emu", sizeof("ipoe_emu")) == -1)
			tcdbg_printf("\nerror: %s setsockopt is faild\n", __FUNCTION__);
	}
#endif

	/* drop root privs if running setuid */
	setuid(getuid());

	return sock;
}
