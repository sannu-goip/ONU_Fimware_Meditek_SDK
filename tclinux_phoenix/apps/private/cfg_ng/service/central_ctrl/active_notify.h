#ifndef _ACTIVE_NOTIFY_H
#define _ACTIVE_NOTIFY_H

#define NDA_RTA(r) 	((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#define RTA_LENGTH(len)	(RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))

#define BR_STB_LIST_PATH				"/proc/br_fdb_host/stb_list"

#define MAX_DEVNUM_RULE 64
#define LANINFO_NUMBER_ATTR			"Number"
#define LANINFO_ACTIVE_ATTR			"Active"
#define LANINFO_MACADDR_ATTR		"MacAddr"
#define LANINFO_IPADDR_ATTR			"IpAddr"
#define LANINFO_PORT_ATTR			"Port"

#define RTMGRP_NEIGH		4

#define MAX_BUF_SIZE	2048
#if defined(TCSUPPORT_CT_E8GUI)
#define IFINFO_ETHCMD_PATH	"/tmp/ifinfo_ethcmd_stats"
#endif

#endif
